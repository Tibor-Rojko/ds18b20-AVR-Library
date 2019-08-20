/*
 * ds18b20 example
 * Author : Tibor Rojko
 */ 

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>
#include <stdio.h>
#include "USART.h"
#include "DS18B20.h"

#define wLED PINC1

int main(void)
{
	DDRC |= (1<<wLED);			//set the warrning led pin as output

	char buff[50];
	uint8_t temp = 0;			//temperature variable for actual temp
	uint8_t threshold = 30;		//threshold variable for warning led	

	initUSART();				//init USART

	printString("DS18B20 temperature sensor test: \r\n");	//welcome msg

	while (1)
	{
		temp = ds18b20_GetTemp();	//get temperature
		
		//if actual temp is equal or higher the threshold, the warning led goes on...
		if(temp >= threshold)
		{
			PORTC |= (1<<PINC1);	//turn on warrning led
			//warrning msg
			sprintf(buff, "WARRNING! High temperature: %d�C\r\n", temp);
			printString(buff);	
		}
		else //...else turn off the LED
		{
			PORTC &= ~(1<<PINC1);	//turn off warrning led
			//print to serial the actual temp
			sprintf(buff, "Actual temperature is %d�C\r\n", temp);
			printString(buff);
		}
		
	}



/*
	uint8_t digit = 0;
	uint16_t decimal = 0;
	char buff[30];

    initUSART();
	printString("DS18B20 temperature sensor test: \r\n");
	
    while (1) 
    {
		ds18b20_GetTemp(&digit, &decimal);
		sprintf(buff, "Actual temperature is %d.%d�C\r\n", digit, decimal);
		printString(buff);
		_delay_ms(5000);
    }
*/

	return 0;
}

