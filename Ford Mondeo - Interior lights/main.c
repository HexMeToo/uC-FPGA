/*
 * main.c
 *
 *  Created on: 28 lis 2014
 *      Author: Mariusz
 */
#define OKRES_LICZNIKA 200
#define OKRES_PROG 2900
#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned int i = 0;
volatile uint8_t stan = 0;

void Timer_On(void)
{
	TCNT1 = 0;
	TCCR0B |= (1 << CS02) | (1 << CS00); // Preskaler
}

void Timer_Off(void)
{
	TCNT1 = 0;
	TCCR0B &= 0xF8; // Preskaler
}

int main()
{

	DDRD = 0x00;
	PORTD = 0x03;
	DDRB = 0x03;
	PORTB = 0x00;

	// Timer - Przerwania
	TCCR0A |= (1 << WGM01); // Tryb CTC
	OCR0A = OKRES_LICZNIKA; // Licznik CTC
	TIMSK |= (1 << OCIE0A); // Zezwolenie na przerwania

	Timer_On();

	sei();

	while(1)
	{
		switch (stan)
		{
			case 0:
			{
				if((PIND & 0x02) && (PIND & 0x01))
				{
					stan = 0;
					PORTB = 0x00;
				}

				if(!(PIND & 0x02) && (PIND & 0x01))
				{
					stan = 1;
					PORTB = 0x01;
				}

				if((PIND & 0x02) && !(PIND & 0x01))
				{
					stan = 2;
					PORTB = 0x01;
				}
			}
			break;

			case 1:
			{
				if(!(PIND & 0x02) && (PIND & 0x01))
				{
					stan = 1;
					PORTB = 0x01;
				}

				if((PIND & 0x02) && (PIND & 0x01))
				{
					stan = 0;
					PORTB = 0x00;
				}
			}
			break;

			case 2:
			{
				if((PIND & 0x02) && !(PIND & 0x01))
				{
					stan = 2;
					PORTB = 0x01;
				}

				if(!(PIND & 0x02) && !(PIND & 0x01))
				{
					stan = 3;
					PORTB = 0x01;
				}

				if((PIND & 0x02) && (PIND & 0x01))
				{
					stan = 4;
					PORTB = 0x01;
					Timer_On();
				}
			}
			break;

			case 3:
			{
				if((PIND & 0x02) && !(PIND & 0x01))
				{
					stan = 2;
					PORTB = 0x01;
				}

				if(!(PIND & 0x02) && (PIND & 0x01))
				{
					stan = 4;
					PORTB = 0x01;
					Timer_On();
				}

				if(!(PIND & 0x02) && !(PIND & 0x01))
				{
					stan = 3;
					PORTB = 0x01;
				}
			}
			break;

			case 4:
			{
				if(!(PIND & 0x01))
				{
					stan = 2;
					PORTB = 0x01;
					Timer_Off();
				}
			}
			break;
		}
	}

	return 0;
}

ISR(TIMER0_COMPA_vect)
{
	i++;

	if(i>=OKRES_PROG && !(PIND & 0x01))
	{
		stan = 0;
		i=0;
		Timer_Off();
	}

	if(i>=OKRES_PROG && (PIND & 0x01))
	{
		stan = 1;
		i=0;
		Timer_Off();
	}
}
