/**
 * @file RAK13015.cpp
 * @author Bernd Giesecke (bernd@giesecke.tk)
 * @brief Functions for RAK13015
 * @version 0.1
 * @date 2024-05-10
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "RAK13015.h"

/** First ADC on the RAK13015 */
RAK_ADC_SGM58031 _ad0(ad0_addr);
/** Second ADC on the RAK13015 */
RAK_ADC_SGM58031 _ad1(ad1_addr);
/** Simple Modbus RTU */
Modbus master(0, Serial1, 0);

RAK13015::RAK13015(uint8_t slot, uint8_t base_board) : _used_slot(slot), _used_base(base_board)
{
	switch (_used_slot)
	{
	case SLOT_A: // Not possible, WB_IO2 conflict
		_alert_pin = ALERT_A;
		_tcon_pin = TCON_A;
		break;
	case SLOT_B: // Not possible, WB_IO2 conflict
		_alert_pin = ALERT_B;
		_tcon_pin = TCON_B;
		break;
	case SLOT_C: // Only possible on RAK19003
		// Serial1.begin(9600);
		_used_serial = 1;
		_rs485 = Serial1;
		if (_used_base == RAK19003)
		{
			_alert_pin = ALERT_C;
			_tcon_pin = TCON_C;
		}
		else
		{
			_alert_pin = -1;
			_tcon_pin = -1;
		}
		break;
	case SLOT_D: // Possible on all BaseBoards
		if (_used_base == RAK19001)
		{
			// Serial2.begin(9600);
			_used_serial = 2;
			_rs485 = Serial2;
			_alert_pin = ALERT_D;
			_tcon_pin = TCON_D;
		}
		else
		{
			// Serial1.begin(9600);
			_used_serial = 1;
			_rs485 = Serial1;
			_alert_pin = ALERT_D;
			_tcon_pin = TCON_D;
		}
		_alert_pin = ALERT_D;
		_tcon_pin = TCON_D;
		break;
	case SLOT_E: // Only possible on RAK19001
		// Serial2.begin(9600);
		_used_serial = 2;
		_rs485 = Serial2;
		if (_used_base == RAK19001)
		{
			_alert_pin = ALERT_E;
			_tcon_pin = TCON_E;
		}
		else
		{
			_alert_pin = -1;
			_tcon_pin = -1;
		}
		break;
	case SLOT_F: // Only possible on RAK19001
		// Serial1.begin(9600);
		_used_serial = 1;
		_rs485 = Serial1;
		if (_used_base == RAK19001)
		{
			_alert_pin = ALERT_E;
			_tcon_pin = TCON_E;
		}
		else
		{
			_alert_pin = -1;
			_tcon_pin = -1;
		}
		break;
	default:
		_alert_pin = -1;
		_tcon_pin = -1;
		break;
	}
}

bool RAK13015::initRAK13015(float analog_resolution, uint16_t baud)
{
	if ((_alert_pin == -1) || (_tcon_pin == -1))
	{
		RAK13015_LOG("RAK13015", "Invalid slot / base board selection");
		return false;
	}

	bool result_analog, result_modbus = true;

	result_analog = initAnalog_4_20(analog_resolution);
	if (!result_analog)
	{
		RAK13015_LOG("RAK13015", "Init analog & 4-20mA failed");
	}

	result_modbus = initModbus(baud);
	if (!result_analog)
	{
		RAK13015_LOG("RAK13015", "Init Modbus failed");
	}

	if (result_analog && result_modbus)
	{
		return true;
	}
	return false;
}

bool RAK13015::initAnalog_4_20(float resolution)
{
	if ((_alert_pin == -1) || (_tcon_pin == -1))
	{
		RAK13015_LOG("RAK13015", "Invalid slot / base board selection");
		return false;
	}

	// Check for I2C devices
	byte error;
	bool found_ch0 = false;
	bool found_ch1 = false;

	Wire.begin();

	for (byte address = 1; address < 127; address++)
	{
		Wire.beginTransmission(address);
		error = Wire.endTransmission();
		if (error == 0)
		{
			RAK13015_LOG("SCAN", "Found sensor at I2C1 0x%02X", address);
			if (address == ad0_addr)
			{
				found_ch0 = true;
			}
			if (address == ad1_addr)
			{
				found_ch1 = true;
			}
		}
	}

	if (found_ch0 && found_ch1)
	{
		_ad0.setVoltageResolution(resolution);
		_ad1.setVoltageResolution(resolution);
		return true;
	}
	return false;
}

bool RAK13015::initModbus(uint16_t baud)
{
	if ((_alert_pin == -1) || (_tcon_pin == -1))
	{
		RAK13015_LOG("RAK13015", "Invalid slot / base board selection");
		return false;
	}

	switch (_used_serial)
	{
	case 0:
		Serial.begin(baud);
		break;
	case 1:
		Serial1.begin(baud);
		break;
	case 2:
		Serial2.begin(baud);
		break;
	default:
		return false;
		break;
	}
	master.setUART(_rs485);
	master.start();
	master.setTimeOut(2000); // if there is no answer in 2000 ms, roll over

	return true;
}

float RAK13015::readAnalog(uint16_t port)
{
	float measured = -50.0;
	switch (port)
	{
	case ANA_CH_0:
		_ad0.setConfig(ANA_CH_0);
		delay(100); // Estimated conversion time
		_voltage_ch0 = _ad0.getVoltage();
		_voltage_ch0 = _voltage_ch0 * 11;
		RAK13015_LOG("V", "CH0 = %.2fV", _voltage_ch0);
		measured = _voltage_ch0;
		break;
	case ANA_CH_1:
		_ad1.setConfig(ANA_CH_1);
		delay(100); // Estimated conversion time
		_voltage_ch1 = _ad1.getVoltage();
		_voltage_ch1 = _voltage_ch1 * 11;
		RAK13015_LOG("V", "CH0 = %.2fV", _voltage_ch1);
		measured = _voltage_ch1;
		break;
	}
	return measured;
}

float RAK13015::read4_20ma(uint16_t port)
{
	float measured = -50.0;
	switch (port)
	{
	case I_4_20_CH_0:
		_ad0.setConfig(I_4_20_CH_0);
		delay(100); // Estimated conversion time
		_current_ch0 = _ad0.getVoltage();
		_current_ch0 = _current_ch0 / 150 * 1000;
		RAK13015_LOG("4-20", "CH0 = %.2fmA", _current_ch0);
		measured = _current_ch0;
		break;
	case I_4_20_CH_1:
		_ad0.setConfig(I_4_20_CH_1);
		delay(100); // Estimated conversion time
		_current_ch1 = _ad0.getVoltage();
		_current_ch1 = _current_ch1 / 150 * 1000;
		RAK13015_LOG("4-20", "CH1 = %.2fmA", _current_ch1);
		measured = _current_ch1;
		break;
	case I_4_20_CH_2:
		_ad0.setConfig(I_4_20_CH_2);
		delay(100); // Estimated conversion time
		_current_ch2 = _ad0.getVoltage();
		_current_ch2 = _current_ch2 / 150 * 1000;
		RAK13015_LOG("4-20", "CH1 = %.2fmA", _current_ch2);
		measured = _current_ch2;
		break;
	}
	return measured;
}

bool RAK13015::requestModBus(uint8_t slave_addr, uint16_t address, uint16_t num_coils, uint16_t *coils_regs, time_t timeout)
{
	modbus_t telegram;
	telegram.u8id = slave_addr;				  // slave address
	telegram.u8fct = MB_FC_READ_REGISTERS;	  // function code (this one is registers read)
	telegram.u16RegAdd = address;			  // start address in slave
	telegram.u16CoilsNo = num_coils;		  // number of elements (coils or registers) to read
	telegram.au16reg = (int16_t *)coils_regs; // pointer to a memory array

	master.setTimeOut(timeout + 1000);

	if (master.query(telegram) != 0) // send query (only once)
	{
		RAK13015_LOG("Mod", "Query read failed");
		return false;
	}

	time_t start_poll = millis();

	while ((millis() - start_poll) < timeout)
	{
		master.poll(); // check incoming messages
		if (master.getState() == COM_IDLE)
		{
			RAK13015_LOG("Mod", "State COM_IDLE");
			return true;
		}
	}
	RAK13015_LOG("Mod", "Poll read timeout");
	return false;
}

bool RAK13015::writeModBus(uint8_t slave_addr, uint16_t address, uint16_t num_coils, uint16_t *coils_regs, time_t timeout)
{
	modbus_t telegram;
	telegram.u8id = slave_addr;					 // slave address
	telegram.u8fct = MB_FC_WRITE_MULTIPLE_COILS; // function code (this one is registers write)
	telegram.u16RegAdd = address;				 // start address in slave
	telegram.u16CoilsNo = num_coils;			 // number of elements (coils or registers) to read
	telegram.au16reg = (int16_t *)coils_regs;	 // pointer to a memory array

	master.setTimeOut(timeout + 1000);

	if (master.query(telegram) != 0) // send query (only once)
	{
		RAK13015_LOG("Mod", "Query write failed");
		return false;
	}

	time_t start_poll = millis();

	while ((millis() - start_poll) < timeout)
	{
		master.poll(); // check incoming messages
		if (master.getState() == COM_IDLE)
		{
			RAK13015_LOG("Mod", "State COM_IDLE");
			return true;
		}
	}
	RAK13015_LOG("Mod", "Poll write timeout");
	return false;
}
