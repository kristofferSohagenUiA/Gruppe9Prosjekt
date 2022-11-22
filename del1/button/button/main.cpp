#include <avr/io.h>


int main(void)
{
	
    DDRB |= (1 << PB1); //Set PB1 as output
	DDRB &= ~(1 << PB2); //Set PB2 as input
	PORTB = (1 << PB2); //Enable internal pull-up resistor
	
	PORTB &= ~(1 << PB1); //Set led off as default
	
    while (1) 
    {
		int pinValue = (PINB & (1 << PB2)) >> PB2; //Read pin PB2 (1/0)
		
		if (pinValue == 0) 
		{
			PORTB |= (1 << PB1); 
		}
		else if (pinValue == 1)
		{
			PORTB &= ~(1 << PB1);
		}
			
    }
}

