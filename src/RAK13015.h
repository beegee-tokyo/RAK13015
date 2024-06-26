/**
 * @file RAK13015.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Defines and includes for RAK13015 library
 * @version 0.1
 * @date 2024-05-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef _RAK13015_H_
#define _RAK13015_H_

#include <Arduino.h>
#include "ADC_SGM58031.h"
#include "ModbusRtu.h"

// Debug output set to 0 to disable app debug output
#ifndef RAK13015_DEBUG_MODE
#define RAK13015_DEBUG_MODE 0
#endif

#if RAK13015_DEBUG_MODE > 0
#define RAK13015_LOG(tag, ...)           \
	do                                   \
	{                                    \
		if (tag)                         \
			Serial.printf("[%s] ", tag); \
		Serial.printf(__VA_ARGS__);      \
		Serial.printf("\n");             \
		Serial.flush();                  \
	} while (0)
#else
#define RAK13015_LOG(...)
#endif

// ADC I2C addresses
#define ad0_addr 0x49
#define ad1_addr 0x48

// Analog channels
#define I_4_20_CH_0 0xC383
#define I_4_20_CH_1 0xD383
#define I_4_20_CH_2 0xE383
#define ANA_CH_0 0xF383
#define ANA_CH_1 0xC383

// Base Board Slots
#define SLOT_A 0
#define SLOT_B 1
#define SLOT_C 2
#define SLOT_D 3
#define SLOT_E 4
#define SLOT_F 5

// Base Board types
#define RAK19007 0
#define RAK19003 1
#define RAK19001 2


// If using Slot A
#define ALERT_A -1
#define TCON_A -1
// If using Slot B
#define ALERT_B -1
#define TCON_B -1
// If using Slot C
#define ALERT_C WB_IO4
#define TCON_C WB_IO3
// If using Slot D
#define ALERT_D WB_IO6
#define TCON_D WB_IO5
// If using Slot E
#define ALERT_E WB_IO4
#define TCON_E WB_IO3
// If using Slot F
#define ALERT_F WB_IO5
#define TCON_F WB_IO6

/**
 * @brief Class to manage RAK13015 interfaces
 * 
 */
class RAK13015
{
public:
	/**
	 * @brief Construct a new RAK13015 object
	 * 		Control pins and alert pins are setup depending on the provided BaseBoard and Slot parameters
	 *
	 * @param slot WisBlock Base Board Slot. Possible SLOT_A ... SLOT_F
	 * @param base_board WisBlock Base Board used. Possible values RAK19007 RAK19003 RAK19001
	 *
	 * @par Usage
	 * @code
	 * RAK13015 rak_in(SLOT_D, RAK19007); // Use Slot D on RAk19007
	 * @endcode
	 */
	RAK13015(uint8_t slot, uint8_t base_board);

	/**
	 * @brief Initialize RAK13015.
	 * 		Initializes Analog inputs, 4-20mA inputs and RS485 as simple ModBus RTU master
	 *
	 * @param analog_resolution ADC resolution. Possible values SGM58031_FS_6_144 SGM58031_FS_4_096 SGM58031_FS_2_048 SGM58031_FS_1_024 SGM58031_FS_0_512 SGM58031_FS_0_256
	 * @param baud Baudrate for RS485
	 * @return true if initialization was successfull
	 * @return false if slot/base board selection is invalid or initialization failed
	 *
	 * @par Usage
	 * @code
	 * bool has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);
	 * if (has_rak13015)
	 * {
	 * 	Serial.println("RAK13015 initialized");
	 * }
	 * else
	 * {
	 * 	Serial.println("No RAK13015 found");
	 * }
	 * @endcode
	 */
	bool initRAK13015(float analog_resolution = SGM58031_FS_4_096, uint16_t baud = 9600);

	/**
	 * @brief Initialize Analog inputs and 4-20mA inputs only.
	 * 		Use when RS485 is setup with other RS485/Modbus drivers
	 *
	 * @param resolution  ADC resolution. Possible values SGM58031_FS_6_144 SGM58031_FS_4_096 SGM58031_FS_2_048 SGM58031_FS_1_024 SGM58031_FS_0_512 SGM58031_FS_0_256
	 * @return true if initialization was successfull
	 * @return false if slot/base board selection is invalid or initialization failed
	 *
	 * @par Usage
	 * @code
	 * bool has_rak13015 = rak_in.initAnalog_4_20(SGM58031_FS_4_096);
	 * if (has_rak13015)
	 * {
	 * 	Serial.println("RAK13015 initialized");
	 * }
	 * else
	 * {
	 * 	Serial.println("No RAK13015 found");
	 * }
	 * @endcode
	 */
	bool initAnalog_4_20(float resolution = SGM58031_FS_4_096);

	/**
	 * @brief Read analog port.
	 * 		Reads selected analog port
	 *
	 * @param port Analog port to be read. Possible values ANA_CH_0 ANA_CH_1
	 * @return float Measured voltage in volt
	 *
	 * @par Usage
	 * @code
	 * bool has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);
	 * if (has_rak13015)
	 * {
	 * 	Serial.println("RAK13015 initialized");
	 * }
	 * else
	 * {
	 * 	Serial.println("No RAK13015 found");
	 * }
	 * float a_4_20[5];
	 * // Read analog interfaces
	 * Serial.println("===============================================================");
	 * Serial.println("Read analog interfaces");
	 * Serial.printf("ANA_CH_0: %.2f V\r\n",rak_in.readAnalog(ANA_CH_0));
	 * Serial.printf("ANA_CH_1: %.2f V\r\n",rak_in.readAnalog(ANA_CH_1));
	 * @endcode
	 */
	float readAnalog(uint16_t port);

	/**
	 * @brief Read 4-20mA port.
	 * 		Reads selected 4-20mA port
	 *
	 * @param port 4-20mA port to be read. Possible values I_4_20_CH_0 I_4_20_CH_1 I_4_20_CH_2
	 * @return float Measured current in mA
	 *
	 * @par Usage
	 * @code
	 * bool has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);
	 * if (has_rak13015)
	 * {
	 * 	Serial.println("RAK13015 initialized");
	 * }
	 * else
	 * {
	 * 	Serial.println("No RAK13015 found");
	 * }
	 * float a_4_20[5];
	 * // Read 4-20mA interfaces
	 * Serial.println("===============================================================");
	 * Serial.println("Read 4-20mA interfaces");
	 * Serial.printf("4-20mA CH_0: %.2f mA\r\n", rak_in.read4_20ma(I_4_20_CH_0));
	 * Serial.printf("4-20mA CH_1: %.2f mA\r\n", rak_in.read4_20ma(I_4_20_CH_1));
	 * Serial.printf("4-20mA CH_2: %.2f mA\r\n", rak_in.read4_20ma(I_4_20_CH_2));
	 * @endcode
	 */
	float read4_20ma(uint16_t port);

	/**
	 * @brief Initialize the RS485 interface as simple Modbus RTU master device
	 *
	 * @param baud RS485 baud rate
	 * @return true if initialization was successfull
	 * @return false if slot/base board selection is invalid or initialization failed
	 *
	 * @par Usage
	 * @code
	 * bool has_rak13015 = rak_in.initModbus(9600);
	 * if (has_rak13015)
	 * {
	 * 	Serial.println("RAK13015 initialized");
	 * }
	 * else
	 * {
	 * 	Serial.println("No RAK13015 found");
	 * }
	 * @endcode
	 */
	bool initModbus(uint16_t baud);

	/**
	 * @brief Request data from slave device on Modbus
	 *
	 * @param slave_addr Slave address
	 * @param address Register/Coil start address for reading
	 * @param num_coils Number of registers/coils to be read
	 * @param coils_regs Buffer to save returned data
	 * @param timeout Timeout in ms to wait for failed return
	 * @return true Data was received
	 * @return false No Data was received
	 *
	 * @par Usage
	 * @code
	 * bool has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);
	 * if (has_rak13015)
	 * {
	 * 	Serial.println("RAK13015 initialized");
	 * }
	 * else
	 * {
	 * 	Serial.println("No RAK13015 found");
	 * }
	 * uint16_t coils_n_regs[8];
	 * // Read first 5 registers from ModBus device 1
	 * Serial.println("===============================================================");
	 * Serial.println("Read first 5 registers from Modbus slave device #1");
	 * rak_in.requestModBus(1, 0, 5, coils_n_regs, 5000);
	 *
	 * Serial.println("===============================================================");
	 * Serial.println("Modbus device 1 first 5 registers:");
	 * Serial.printf("HEX 0: %04X 1: %04X 2: %04X 3: %04X 4: %04X\r\n", coils_n_regs[0], coils_n_regs[1], coils_n_regs[2], coils_n_regs[3], coils_n_regs[4]);
	 * Serial.printf("DEC 0: %d 1: %d 2: %d 3: %d 4: %d\r\n", coils_n_regs[0], coils_n_regs[1], coils_n_regs[2], coils_n_regs[3], coils_n_regs[4]);
	 * @endcode
	 */
	bool requestModBus(uint8_t slave_addr, uint16_t address, uint16_t num_coils, uint16_t *coils_regs, time_t timeout);

	/**
	 * @brief Send data over Modbus to a slave device
	 *
	 * @param slave_addr Slave address
	 * @param address Register/Coil start address for writing
	 * @param num_coils Number of registers/coils to be written
	 * @param coils_regs Buffer with data to write
	 * @param timeout Timeout in ms to wait for failed response
	 * @return true Data was sent
	 * @return false Failure to send data
	 * @code
	 * bool has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);
	 * if (has_rak13015)
	 * {
	 * 	Serial.println("RAK13015 initialized");
	 * }
	 * else
	 * {
	 * 	Serial.println("No RAK13015 found");
	 * }
	 * uint16_t coils_n_regs[8];
	 * coils_n_regs[0] = 0x00;
	 * coils_n_regs[1] = 0x01;
	 * coils_n_regs[2] = 0x02;
	 * coils_n_regs[3] = 0x03;
	 * coils_n_regs[4] = 0x04;
	 * // Write to first 5 registers of ModBus device 1
	 * Serial.println("===============================================================");
	 * Serial.println("Write to first 5 registers of Modbus slave device #1");
	 * if (rak_in.writeModBus(1, 0, 5, coils_n_regs, 5000))
	 * {
	 * 	Serial.println("Modbus write success");
	 * }
	 * else
	 * {
	 * 	Serial.println("Modbus write failed");
	 * }
	 * @endcode
	 */
	bool writeModBus(uint8_t slave_addr, uint16_t address, uint16_t num_coils, uint16_t *coils_regs, time_t timeout);

	/** UART to be used for Modbus RTU master */
	Stream &_rs485 = Serial1;

private:
	uint8_t _used_slot = SLOT_D;
	uint8_t _used_base = RAK19007;
	uint8_t _used_serial = 0;
	uint8_t _alert_pin = ALERT_D;
	uint8_t _tcon_pin = TCON_D;

	float _current_ch0;
	float _current_ch1;
	float _current_ch2;
	float _voltage_ch0;
	float _voltage_ch1;

	uint8_t _deviceID = 0;
	uint8_t _bidx = 0;
	uint8_t _buffer[50]; //  internal receive buffer
};
#endif // _RAK13015_H_
