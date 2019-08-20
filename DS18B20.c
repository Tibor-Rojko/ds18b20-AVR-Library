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

//Includes...
#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <avr/interrupt.h>
#include "DS18B20.h"


/*
DS18B20 reset:
    Set data line as output & pull data line low
    Wait 480 μS – the entire “send” timeslot
    Release data line
    Wait 60 μS
    Read data line
*/
static uint8_t ds18b20_Reset()
{
	uint8_t state;

	//low for 480us
	DS18B20_PORT &= ~ (1<<DS18B20_DQ);              //DQ line to low
	DS18B20_DDR |= (1<<DS18B20_DQ);                 //setting the DQ port as output
	_delay_us(480);                                 //wait 480uS

	//release line and wait for 60uS
	DS18B20_DDR &= ~(1<<DS18B20_DQ);                //change DQ to input
	_delay_us(60);                                  //wait 60uS

	//get data and wait 420us
	state = (DS18B20_PIN & (1<<DS18B20_DQ));        //read the data from DQ pin
	_delay_us(420);                                 //wait 420uS            

	//return the read value, when the value is zero, we hava error, another OK
	return state;
}

/*
 Write bit:
    pulling down the data line for 1 μS, set DQ to low, and set as output
    to wirte 1, release the DQ line, or for 0 hold it as output with LOW state
    wait 60uS
    release the bus
 */
static void ds18b20_W_bit(uint8_t bit)
{
	DS18B20_PORT &= ~ (1<<DS18B20_DQ);              //DQ line to low
	DS18B20_DDR |= (1<<DS18B20_DQ);                 //setting the DQ port as output
	_delay_us(1);                                   //wait 1uS

	if(bit)	DS18B20_DDR &= ~(1<<DS18B20_DQ);        //if we want to write 1, release the line (if not will keep low) 

	_delay_us(60);                                  //wait 60uS
	DS18B20_DDR &= ~(1<<DS18B20_DQ);                //release DQ
}

/*
 Read one bit:
    pulling down the data line for 1 μS, set DQ to low, and set as output
    release line and wait for 14uS
    read the value
    wait 45uS and return read value
 */
static uint8_t ds18b20_R_bit(void)
{
	uint8_t bit = 0;

	DS18B20_PORT &= ~ (1<<DS18B20_DQ);              //DQ line to low
	DS18B20_DDR |= (1<<DS18B20_DQ);                 //setting the DQ port as output
	_delay_us(1);                                   //wait 1uS

	
	DS18B20_DDR &= ~(1<<DS18B20_DQ);                //release DQ, set as input
	_delay_us(14);                                  //wait 14uS

	if(DS18B20_PIN & (1<<DS18B20_DQ)) bit = 1;        //if DQ value HIGH, change the bit variable to 1

	_delay_us(45);                                  //wait 45uS

	return bit;                         
}


//Write byte
static void ds18b20_W_byte(uint8_t byte){
	uint8_t i=8;
	while(i--){
		ds18b20_W_bit(byte&1);
		byte >>= 1;
	}
}

//Read byte
static uint8_t ds18b20_R_byte(void){
	uint8_t i=8, byte=0;
	while(i--){
		byte >>= 1;
		byte |= (ds18b20_R_bit()<<7);
	}
	return byte;
}

//Get temperature
uint8_t ds18b20_GetTemp()
{
	uint8_t digit;
	uint16_t decimal;
	uint8_t temperature[2];

	//disable global interrupt
	cli();

	ds18b20_Reset();                    //reset
	ds18b20_W_byte(CMD_SKIPROM);        //skip ROM
	ds18b20_W_byte(CMD_CONVERTTEMP);    //start temperature conversion

	while(!ds18b20_R_bit());            //wait until conversion is complete

	ds18b20_Reset();                    //reset
	ds18b20_W_byte(CMD_SKIPROM);        //skip ROM
	ds18b20_W_byte(CMD_RSCRATCHPAD);    //read scratchpad

	//read 2 byte from scratchpad
	temperature[0] = ds18b20_R_byte();
	temperature[1] = ds18b20_R_byte();

	ds18b20_Reset();                    //reset

	//enable global interrupt
	sei();

	//Store temperature digits
	digit = temperature[0] >> 4;
	digit |= (temperature[1] & 0x7) << 4;
	
	//Store decimal digits
	decimal = temperature[0] & 0xf;
	decimal *= 625;

	return digit; //return without decimal
}

/*
//Get temperature
void ds18b20_GetTemp(uint8_t *digit, uint16_t *decimal)
{
	
    uint8_t temperature[2];

    //disable global interrupt
	cli();      

	ds18b20_Reset();                    //reset
	ds18b20_W_byte(CMD_SKIPROM);        //skip ROM
	ds18b20_W_byte(CMD_CONVERTTEMP);    //start temperature conversion

	while(!ds18b20_R_bit());            //wait until conversion is complete

	ds18b20_Reset();                    //reset
	ds18b20_W_byte(CMD_SKIPROM);        //skip ROM
	ds18b20_W_byte(CMD_RSCRATCHPAD);    //read scratchpad

	//read 2 byte from scratchpad
	temperature[0] = ds18b20_R_byte();
	temperature[1] = ds18b20_R_byte();

	ds18b20_Reset();                    //reset

    //enable global interrupt
	sei();      

	//Store temperature integer digits and decimal digits
	*digit = temperature[0] >> 4;
	*digit |= (temperature[1] & 0x7) << 4;
	//Store decimal digits
	*decimal = temperature[0] & 0xf;
	*decimal *= 625;
}
*/

