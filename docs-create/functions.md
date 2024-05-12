## Construct a new RAK13015 object
Control pins and alert pins are setup depending on the provided BaseBoard and Slot parameters
    
```cpp
	RAK13015(uint8_t slot, uint8_t base_board);
```

### Parameters
@param slot WisBlock Base Board Slot. Possible SLOT_A ... SLOT_F     
@param base_board WisBlock Base Board used. Possible values RAK19007 RAK19003 RAK19001
    
### Usage     
```cpp    
RAK13015 rak_in(SLOT_D, RAK19007); // Use Slot D on RAk19007     
```

## Initialize RAK13015.
Initializes Analog inputs, 4-20mA inputs and RS485 as simple ModBus RTU master
    
```cpp
	bool initRAK13015(float analog_resolution = SGM58031_FS_4_096, uint16_t baud = 9600);
```

### Parameters
@param analog_resolution ADC resolution. Possible values SGM58031_FS_6_144 SGM58031_FS_4_096 SGM58031_FS_2_048 SGM58031_FS_1_024 SGM58031_FS_0_512 SGM58031_FS_0_256     
@param baud Baudrate for RS485     
@return true if initialization was successfull     
@return false if slot/base board selection is invalid or initialization failed
    
### Usage     
```cpp    
bool has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);     
if (has_rak13015)     
{     
	Serial.println("RAK13015 initialized");     
}     
else     
{     
	Serial.println("No RAK13015 found");     
}     
```

## Initialize Analog inputs and 4-20mA inputs only.
Use when RS485 is setup with other RS485/Modbus drivers
    
```cpp
	bool initAnalog_4_20(float resolution = SGM58031_FS_4_096);
```

### Parameters
@param resolution  ADC resolution. Possible values SGM58031_FS_6_144 SGM58031_FS_4_096 SGM58031_FS_2_048 SGM58031_FS_1_024 SGM58031_FS_0_512 SGM58031_FS_0_256     
@return true if initialization was successfull     
@return false if slot/base board selection is invalid or initialization failed
    
### Usage     
```cpp    
bool has_rak13015 = rak_in.initAnalog_4_20(SGM58031_FS_4_096);     
if (has_rak13015)     
{     
	Serial.println("RAK13015 initialized");     
}     
else     
{     
	Serial.println("No RAK13015 found");     
}     
```

## Read analog port.
Reads selected analog port
    
```cpp
	float readAnalog(uint16_t port);
```

### Parameters
@param port Analog port to be read. Possible values ANA_CH_0 ANA_CH_1     
@return float Measured voltage in volt
    
### Usage     
```cpp    
bool has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);     
if (has_rak13015)     
{     
	Serial.println("RAK13015 initialized");     
}     
else     
{     
	Serial.println("No RAK13015 found");     
}     
float a_4_20[5];     
// Read analog interfaces     
Serial.println("===============================================================");     
Serial.println("Read analog interfaces");     
Serial.printf("ANA_CH_0: %.2f V\r\n",rak_in.readAnalog(ANA_CH_0));     
Serial.printf("ANA_CH_1: %.2f V\r\n",rak_in.readAnalog(ANA_CH_1));     
```

## Read 4-20mA port.
Reads selected 4-20mA port
    
```cpp
	float read4_20ma(uint16_t port);
```

### Parameters
@param port 4-20mA port to be read. Possible values I_4_20_CH_0 I_4_20_CH_1 I_4_20_CH_2     
@return float Measured current in mA
    
### Usage     
```cpp    
bool has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);     
if (has_rak13015)     
{     
	Serial.println("RAK13015 initialized");     
}     
else     
{     
	Serial.println("No RAK13015 found");     
}     
float a_4_20[5];     
// Read 4-20mA interfaces     
Serial.println("===============================================================");     
Serial.println("Read 4-20mA interfaces");     
Serial.printf("4-20mA CH_0: %.2f mA\r\n", rak_in.read4_20ma(I_4_20_CH_0));     
Serial.printf("4-20mA CH_1: %.2f mA\r\n", rak_in.read4_20ma(I_4_20_CH_1));     
Serial.printf("4-20mA CH_2: %.2f mA\r\n", rak_in.read4_20ma(I_4_20_CH_2));     
```

## Initialize the RS485 interface as simple Modbus RTU master device
    
```cpp
	bool initModbus(uint16_t baud);
```

### Parameters
@param baud RS485 baud rate     
@return true if initialization was successfull     
@return false if slot/base board selection is invalid or initialization failed
    
### Usage     
```cpp    
bool has_rak13015 = rak_in.initModbus(9600);     
if (has_rak13015)     
{     
	Serial.println("RAK13015 initialized");     
}     
else     
{     
	Serial.println("No RAK13015 found");     
}     
```

## Request data from slave device on Modbus
    
```cpp
	bool requestModBus(uint8_t slave_addr, uint16_t address, uint16_t num_coils, uint16_t *coils_regs, time_t timeout);
```

### Parameters
@param slave_addr Slave address     
@param address Register/Coil start address for reading     
@param num_coils Number of registers/coils to be read     
@param coils_regs Buffer to save returned data     
@param timeout Timeout in ms to wait for failed return     
@return true Data was received     
@return false No Data was received
    
### Usage     
```cpp    
bool has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);     
if (has_rak13015)     
{     
	Serial.println("RAK13015 initialized");     
}     
else     
{     
	Serial.println("No RAK13015 found");     
}     
uint16_t coils_n_regs[8];     
// Read first 5 registers from ModBus device 1     
Serial.println("===============================================================");     
Serial.println("Read first 5 registers from Modbus slave device #1");     
rak_in.requestModBus(1, 0, 5, coils_n_regs, 5000);
    
Serial.println("===============================================================");     
Serial.println("Modbus device 1 first 5 registers:");     
Serial.printf("HEX 0: %04X 1: %04X 2: %04X 3: %04X 4: %04X\r\n", coils_n_regs[0], coils_n_regs[1], coils_n_regs[2], coils_n_regs[3], coils_n_regs[4]);     
Serial.printf("DEC 0: %d 1: %d 2: %d 3: %d 4: %d\r\n", coils_n_regs[0], coils_n_regs[1], coils_n_regs[2], coils_n_regs[3], coils_n_regs[4]);     
```

## Send data over Modbus to a slave device
    
```cpp
	bool writeModBus(uint8_t slave_addr, uint16_t address, uint16_t num_coils, uint16_t *coils_regs, time_t timeout);
```

### Parameters
@param slave_addr Slave address     
@param address Register/Coil start address for writing     
@param num_coils Number of registers/coils to be written     
@param coils_regs Buffer with data to write     
@param timeout Timeout in ms to wait for failed response     
@return true Data was sent     
@return false Failure to send data     

### Usage     
```cpp    
bool has_rak13015 = rak_in.initRAK13015(SGM58031_FS_4_096, 9600);     
if (has_rak13015)     
{     
	Serial.println("RAK13015 initialized");     
}     
else     
{     
	Serial.println("No RAK13015 found");     
}     
uint16_t coils_n_regs[8];     
coils_n_regs[0] = 0x00;     
coils_n_regs[1] = 0x01;     
coils_n_regs[2] = 0x02;     
coils_n_regs[3] = 0x03;     
coils_n_regs[4] = 0x04;     
// Write to first 5 registers of ModBus device 1     
Serial.println("===============================================================");     
Serial.println("Write to first 5 registers of Modbus slave device #1");     
if (rak_in.writeModBus(1, 0, 5, coils_n_regs, 5000))     
{     
	Serial.println("Modbus write success");     
}     
else     
{     
	Serial.println("Modbus write failed");     
}     
```

