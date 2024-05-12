/**
 * @file main.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief RAK13015 Test
 * @version 0.1
 * @date 2024-04-14
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "main.h"

/** Send Fail counter **/
uint8_t send_fail = 0;

/** Set the device name, max length is 10 characters */
char g_ble_dev_name[10] = "RAK-IO";

/** Payload */
WisCayenne payload(255);

RAK13015 rak_in(SLOT_D, RAK19007);

/** Data array for modbus for 8 registers/coils */
uint16_t coils_n_regs[8];

bool has_rak13015 = false;

SoftwareTimer delay_measure_timer;

void start_measure(TimerHandle_t unused)
{
	api_wake_loop(START_MEASURE);
}

/**
 * @brief Initial setup of the application (before LoRaWAN and BLE setup)
 *
 */
void setup_app(void)
{
	Serial.begin(115200);
	time_t serial_timeout = millis();
	// On nRF52840 the USB serial is not available immediately
	while (!Serial)
	{
		if ((millis() - serial_timeout) < 5000)
		{
			delay(100);
			digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
		}
		else
		{
			break;
		}
	}
	digitalWrite(LED_GREEN, LOW);

	// Set firmware version
	api_set_version(SW_VERSION_1, SW_VERSION_2, SW_VERSION_3);

	MYLOG("APP", "Setup application");
	g_enable_ble = true;
}

/**
 * @brief Final setup of application  (after LoRaWAN and BLE setup)
 *
 * @return true
 * @return false
 */
bool init_app(void)
{
	MYLOG("APP", "Initialize application");
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(500);
	restart_advertising(30);

	// Initialize RS485 Modbus, 4-20mA and Analog In interfaces
	has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);

	if (has_rak13015)
	{
		MYLOG("APP", "RAK13015 initialized");
	}

	//******************************************************************
	// It is possible to initialize every part of the RAK19015 separate
	// This is useful if an external library for RS485 or ModBus is used
	//
	// Initialize only analog inputs and 4-20mA inputs
	// has_rak13015 = rak_in.initAnalog_4_20(SGM58031_FS_4_096);
	//
	// Initialize RS485
	// USE RS485 library like https://github.com/RobTillaart/RS485
	//
	// Initialize ModBus
	// USE ModBus library like https://github.com/4-20ma/ModbusMaster
	//******************************************************************

	// Power down interface
	digitalWrite(WB_IO2, LOW);

	// Create timer for delayed sensor readings
	delay_measure_timer.begin(5000, start_measure, NULL, false);
	return true;
}

/**
 * @brief Handle events
 * 		Events can be
 * 		- timer (setup with AT+SENDINT=xxx)
 * 		- interrupt events
 * 		- wake-up signals from other tasks
 */
void app_event_handler(void)
{
	// Timer triggered event
	if ((g_task_event_type & STATUS) == STATUS)
	{
		g_task_event_type &= N_STATUS;
		MYLOG("APP", "Timer wakeup");
		digitalWrite(WB_IO2, HIGH);
		delay_measure_timer.start();
	}

	// Start measurements
	if ((g_task_event_type & START_MEASURE) == START_MEASURE)
	{
		g_task_event_type &= N_START_MEASURE;
		MYLOG("APP", "Start measuring");
		// Prepare payload
		payload.reset();

		// Get Battery status
		float batt_level_f = 0.0;
		for (int readings = 0; readings < 10; readings++)
		{
			batt_level_f += read_batt();
		}
		batt_level_f = batt_level_f / 10;
		payload.addVoltage(LPP_CHANNEL_BATT, batt_level_f / 1000.0);

		if (has_rak13015)
		{
			payload.addAnalogInput(LPP_CHANNEL_4_20_CH0, rak_in.read4_20ma(I_4_20_CH_0));
			payload.addAnalogInput(LPP_CHANNEL_4_20_CH1, rak_in.read4_20ma(I_4_20_CH_1));
			payload.addAnalogInput(LPP_CHANNEL_4_20_CH2, rak_in.read4_20ma(I_4_20_CH_2));

			payload.addAnalogInput(LPP_CHANNEL_V_CH0, rak_in.readAnalog(ANA_CH_0));
			payload.addAnalogInput(LPP_CHANNEL_V_CH1, rak_in.readAnalog(ANA_CH_1));

			rak_in.requestModBus(1, 0, 5, coils_n_regs, 5000);
			MYLOG("MODR", "0: %04X 1: %04X 2: %04X 3: %04X 4: %04X", coils_n_regs[0], coils_n_regs[1], coils_n_regs[2], coils_n_regs[3], coils_n_regs[4]);
			MYLOG("MODR", "0: %d 1: %d 2: %d 3: %d 4: %d", coils_n_regs[0], coils_n_regs[1], coils_n_regs[2], coils_n_regs[3], coils_n_regs[4]);
			MYLOG("MODR", "T = %.2f EC = %.2f", (float)coils_n_regs[1] / 10, (float)coils_n_regs[3] / 100);

			payload.addAnalogInput(LPP_TEMP, (float)coils_n_regs[1] / 10.0);
			payload.addAnalogInput(LPP_TDS, (float)coils_n_regs[3] / 100.0);
		}
		if (g_lorawan_settings.lorawan_enable)
		{
			if (g_lpwan_has_joined)
			{

				lmh_error_status result = send_lora_packet(payload.getBuffer(), payload.getSize(), 2);
				switch (result)
				{
				case LMH_SUCCESS:
					MYLOG("APP", "Packet enqueued");
					break;
				case LMH_BUSY:
					MYLOG("APP", "LoRa transceiver is busy");
					break;
				case LMH_ERROR:
					MYLOG("APP", "Packet error, too big to send with current DR");
					break;
				}
			}
			else
			{
				MYLOG("APP", "Network not joined, skip sending");
			}
		}
		else
		{
			send_p2p_packet(payload.getBuffer(), payload.getSize());
		}
		digitalWrite(WB_IO2, LOW);
	}
}

/**
 * @brief Handle BLE events
 *
 */
void ble_data_handler(void)
{
	if (g_enable_ble)
	{
		/**************************************************************/
		/**************************************************************/
		/// \todo BLE UART data arrived
		/// \todo or forward them to the AT command interpreter
		/// \todo parse them here
		/**************************************************************/
		/**************************************************************/
		if ((g_task_event_type & BLE_DATA) == BLE_DATA)
		{
			MYLOG("AT", "RECEIVED BLE");
			// BLE UART data arrived
			// in this example we forward it to the AT command interpreter
			g_task_event_type &= N_BLE_DATA;

			while (g_ble_uart.available() > 0)
			{
				at_serial_input(uint8_t(g_ble_uart.read()));
				delay(5);
			}
			at_serial_input(uint8_t('\n'));
		}
	}
}

/**
 * @brief Handle LoRa events
 *
 */
void lora_data_handler(void)
{
	// LoRa Join finished handling
	if ((g_task_event_type & LORA_JOIN_FIN) == LORA_JOIN_FIN)
	{
		g_task_event_type &= N_LORA_JOIN_FIN;
		if (g_join_result)
		{
			MYLOG("APP", "Successfully joined network");
		}
		else
		{
			MYLOG("APP", "Join network failed");
			/// \todo here join could be restarted.
			// lmh_join();
		}
	}

	// LoRa data handling
	if ((g_task_event_type & LORA_DATA) == LORA_DATA)
	{
		/**************************************************************/
		/**************************************************************/
		/// \todo LoRa data arrived
		/// \todo parse them here
		/**************************************************************/
		/**************************************************************/
		g_task_event_type &= N_LORA_DATA;
		MYLOG("APP", "Received package over LoRa");
		MYLOG("APP", "Last RSSI %d", g_last_rssi);

		char log_buff[g_rx_data_len * 3] = {0};
		uint8_t log_idx = 0;
		for (int idx = 0; idx < g_rx_data_len; idx++)
		{
			sprintf(&log_buff[log_idx], "%02X ", g_rx_lora_data[idx]);
			log_idx += 3;
		}
		MYLOG("APP", "%s", log_buff);
	}

	// LoRa TX finished handling
	if ((g_task_event_type & LORA_TX_FIN) == LORA_TX_FIN)
	{
		g_task_event_type &= N_LORA_TX_FIN;

		if (g_lorawan_settings.lorawan_enable)
		{
			if (g_lorawan_settings.confirmed_msg_enabled == LMH_UNCONFIRMED_MSG)
			{
				MYLOG("APP", "LPWAN TX cycle finished");
			}
			else
			{
				MYLOG("APP", "LPWAN TX cycle %s", g_rx_fin_result ? "finished ACK" : "failed NAK");
			}
			if (!g_rx_fin_result)
			{
				// Increase fail send counter
				send_fail++;

				if (send_fail == 10)
				{
					// Too many failed sendings, reset node and try to rejoin
					delay(100);
					api_reset();
				}
			}
		}
		else
		{
			MYLOG("APP", "P2P TX finished");
		}
	}
}
