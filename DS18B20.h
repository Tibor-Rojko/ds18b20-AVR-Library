/***********************************************************************************************************************************
* DS18B20 library
* 
* Maxim Ds18b20 is a low-cost temperature sensor, digitally reports the temperature data with 9-12 bit precision.
* This sensor uses the One Wire interface, for data transmission
* Temperature range: -55°C - 125°C (+/- 0.5°C)
* Use 4.7K pullup resistor, beetween DQ pin, and Vcc, for one wire bus communication!!!
*
* pinout:
* PC0 to Ds18B20 DQ pin
* 4k7R between Vcc & DQ
* 
* copyright (c) Tibor Rojko, 2019
*************************************************************************************************************************************/


#ifndef DS18B20_H_
#define DS18B20_H_

//pin configuration
#define DS18B20_PORT    PORTC       
#define DS18B20_DDR     DDRC
#define DS18B20_PIN     PINC
#define DS18B20_DQ      PINC0

//commands for ds18b20
#define CMD_CONVERTTEMP     0x44     //convert temp
#define CMD_RSCRATCHPAD     0xbe     //read scratchpad
#define CMD_SKIPROM         0xCC     //skip rom

uint8_t ds18b20_GetTemp();									   //Get temperature value from sensor
//void ds18b20_GetTemp(uint8_t *digit, uint16_t *decimal);     //Get temperature value from sensor
	
#endif
