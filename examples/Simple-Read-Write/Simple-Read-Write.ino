/**
 * @file Simple-Read-Write.ino
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Simple-Read-Write example for RAK13015
 * @version 0.1
 * @date 2024-05-12
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <Arduino.h>
#include <RAK13015.h>

// Setup RAk13015 for RAK19007 Base Board and Slot D
RAK13015 rak_in(SLOT_D, RAK19007);

/** Data array for modbus for 8 registers/coils */
uint16_t coils_n_regs[8];

bool has_rak13015 = false;

float a_4_20[5];

void setup(void)
{
	// Wait for Serial to be available
	pinMode(LED_GREEN, OUTPUT);
	pinMode(LED_BLUE, OUTPUT);
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

	// Power up the RAK13015
	pinMode(WB_IO2, OUTPUT);
	digitalWrite(WB_IO2, HIGH);
	delay(500);

	// Initialize RS485 Modbus, 4-20mA and Analog In interfaces
	has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);

	if (has_rak13015)
	{
		Serial.println("RAK13015 initialized");
	}
	else
	{
		Serial.println("No RAK13015 found");
	}
}

void loop(void)
{
	if (has_rak13015)
	{
		// Read 4-20mA interfaces
		Serial.println("===============================================================");
		Serial.println("Read 4-20mA interfaces");
		a_4_20[0] = rak_in.read4_20ma(I_4_20_CH_0);
		a_4_20[1] = rak_in.read4_20ma(I_4_20_CH_1);
		a_4_20[2] = rak_in.read4_20ma(I_4_20_CH_2);

		// Read analog interfaces
		Serial.println("===============================================================");
		Serial.println("Read analog interfaces");
		a_4_20[3] = rak_in.readAnalog(ANA_CH_0);
		a_4_20[4] = rak_in.readAnalog(ANA_CH_1);

		// Write first 5 registers from ModBus device 1
		Serial.println("===============================================================");
		Serial.println("Write 5 registers to Modbus slave device #1");
		coils_n_regs[0] = 0x01;
		coils_n_regs[1] = 0x02;
		coils_n_regs[2] = 0x03;
		coils_n_regs[3] = 0x04;
		coils_n_regs[4] = 0x05;
		bool modbus_write = rak_in.writeModBus(1, 0, 5, coils_n_regs, 5000);

		Serial.println("===============================================================");
		Serial.println("Results:");
		Serial.printf("4-20mA CH0 : %.2f\r\n", a_4_20[0]);
		Serial.printf("4-20mA CH1 : %.2f\r\n", a_4_20[1]);
		Serial.printf("4-20mA CH2 : %.2f\r\n", a_4_20[2]);
		Serial.printf("Analog CH0 : %.2f\r\n", a_4_20[3]);
		Serial.printf("Analog CH1 : %.2f\r\n", a_4_20[4]);
		Serial.printf("Modbus write to device 1 returned %s\r\n", modbus_write ? "success" : "fail");
	}
	else
	{
		Serial.println("No RAK13015 available");
	}
	delay(10000);
}