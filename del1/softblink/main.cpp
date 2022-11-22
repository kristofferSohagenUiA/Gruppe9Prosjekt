#define F_CPU 10000000
#include <avr/io.h>
#include <util/delay.h>

void delay(int n)
{
	while(n--)
	{
		_delay_us(1);
	}
}


int main(void)
{
	DDRB = (1 << PB1); //Set pin PB1 as output
	
	int cycleTime = 1000; //Define PWM period in us
	int dutyCycleTime = 0; //Set dutyCycle start value
	int i = 0;
	
	while (1)
	{
		PORTB |= (1 << PB1); //Set PB1 HIGH
		delay(dutyCycleTime); //Wait
		PORTB &= ~(1 << PB1); //Set PB1 LOW
		delay(cycleTime - dutyCycleTime); //Wait remaining time
		
		i++;
		
		if (i == 5)
		{
			
			if (dutyCycleTime < 100)
			{
				dutyCycleTime++;
			}
			else if(dutyCycleTime >= 100 && dutyCycleTime < 200)
			{
				dutyCycleTime+=2;
			}
			else if(dutyCycleTime >= 200 && dutyCycleTime < 300)
			{
				dutyCycleTime+=3;
			}
			else if(dutyCycleTime >= 300 && dutyCycleTime < 400)
			{
				dutyCycleTime+=4;
			}
			else if(dutyCycleTime >= 400 && dutyCycleTime < 500)
			{
				dutyCycleTime+=5;
			}
			else if(dutyCycleTime >= 500 && dutyCycleTime < 600)
			{
				dutyCycleTime+=6;
			}
			else if(dutyCycleTime >= 600 && dutyCycleTime < 700)
			{
				dutyCycleTime+=7;
			}
			else if(dutyCycleTime >= 800 && dutyCycleTime < 900)
			{
				dutyCycleTime+=8;
			}
			else if(dutyCycleTime >= 900 && dutyCycleTime <= 991)
			{
				dutyCycleTime+=9;
			}
			else
			{
				dutyCycleTime = 0;
			}
			i = 0;
		}
	 }
}