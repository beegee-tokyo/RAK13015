/**
 * @file Simple-Read.ino
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Simple-Read example for RAK13015
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

		// Read first 5 registers from ModBus device 1
		Serial.println("===============================================================");
		Serial.println("Read 5 registers from Modbus slave device #1");
		rak_in.requestModBus(1, 0, 5, coils_n_regs, 5000);

		Serial.println("===============================================================");
		Serial.println("Results:");
		Serial.printf("4-20mA CH0 : %.2f\r\n", a_4_20[0]);
		Serial.printf("4-20mA CH1 : %.2f\r\n", a_4_20[1]);
		Serial.printf("4-20mA CH2 : %.2f\r\n", a_4_20[2]);
		Serial.printf("Analog CH0 : %.2f\r\n", a_4_20[3]);
		Serial.printf("Analog CH1 : %.2f\r\n", a_4_20[4]);
		Serial.println("Modbus device 1 first 5 registers:");
		Serial.printf("HEX 0: %04X 1: %04X 2: %04X 3: %04X 4: %04X\r\n", coils_n_regs[0], coils_n_regs[1], coils_n_regs[2], coils_n_regs[3], coils_n_regs[4]);
		Serial.printf("DEC 0: %d 1: %d 2: %d 3: %d 4: %d\r\n", coils_n_regs[0], coils_n_regs[1], coils_n_regs[2], coils_n_regs[3], coils_n_regs[4]);
	}
	else
	{
		Serial.println("No RAK13015 available");
	}
	delay(10000);
}