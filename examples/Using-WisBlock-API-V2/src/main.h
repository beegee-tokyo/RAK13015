/**
 * @file main.h
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Includes and Defines
 * @version 0.1
 * @date 2024-04-14
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <Arduino.h>
#include <WisBlock-API-V2.h>
#include <RAK13015.h>

// Debug output set to 0 to disable app debug output
#ifndef MY_DEBUG
#define MY_DEBUG 1
#endif

#if MY_DEBUG > 0
#define MYLOG(tag, ...)                     \
	do                                      \
	{                                       \
		if (tag)                            \
			PRINTF("[%s] ", tag);           \
		PRINTF(__VA_ARGS__);                \
		PRINTF("\r\n");                     \
		Serial.flush();                     \
		if (g_ble_uart_is_connected)        \
		{                                   \
			g_ble_uart.printf(__VA_ARGS__); \
			g_ble_uart.printf("\r\n");      \
			g_ble_uart.flush();             \
		}                                   \
	} while (0)
#else
#define MYLOG(...)
#endif

/** Define the version of your SW */
#ifndef SW_VERSION_1
#define SW_VERSION_1 1 // major version increase on API change / not backwards compatible
#define SW_VERSION_2 0 // minor version increase on API change / backward compatible
#define SW_VERSION_3 0 // patch version increase on bugfix, no affect on API
#endif

/** Application function definitions */
void setup_app(void);
bool init_app(void);
void app_event_handler(void);
void ble_data_handler(void) __attribute__((weak));
void lora_data_handler(void);

#define START_MEASURE 0b1000000000000000
#define N_START_MEASURE 0b0111111111111111

/** RAk13015 functions */
bool init_rak13015_analog(void);
void read_rak13015_4_20_ch0(bool add_payload = true);
void read_rak13015_4_20_ch1(bool add_payload = true);
void read_rak13015_4_20_ch2(bool add_payload = true);
void read_rak13015_v_ch0(bool add_payload = true);
void read_rak13015_v_ch1(bool add_payload = true);
void init_rak13015_rs485(void);
bool read_rak13015_rs485(uint8_t sensor_type, bool add_payload = true);

/** WisCayenne channel definitions */
#define LPP_TDS 64				  // TDS sensor EC value
#define LPP_TEMP 65				  // TDS sensor temperature
#define LPP_CHANNEL_4_20_CH0 70 // 4-20mA channel 0
#define LPP_CHANNEL_4_20_CH1 71 // 4-20mA channel 1
#define LPP_CHANNEL_4_20_CH2 72 // 4-20mA channel 2
#define LPP_CHANNEL_V_CH0 73	  // external battery voltage
#define LPP_CHANNEL_V_CH1 74	  // external solar voltage
extern WisCayenne payload;
