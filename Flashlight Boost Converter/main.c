/*
 * main.c
 *
 *  Created on: 10 paü 2018
 *      Author: Administrator
 */

#define Threshold 950

#define Boost_Converter_ON  TCCR0B |= (1 << CS00);
#define Boost_Converter_OFF TCCR0B &= ~(1 << CS00);

#define Main_LED_ON  PORTB |= (1 << PB1);
#define Main_LED_OFF PORTB &= ~(1 << PB1);

#define Secondary_LEDs_ON  PORTB |= (1 << PB2);
#define Secondary_LEDs_OFF PORTB &= ~(1 << PB2);

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

volatile uint8_t EN = 0;

int main()
{
	uint8_t Machine_State = 1;

	DDRD |= (1 << PD6);
	PORTD &= ~(1 << PD6);

	DDRC &= ~(1 << PC0);

	ADMUX |= (1 << REFS1) | (1 << REFS0);
	ADCSRA |= (1 << ADEN) | (1 << ADATE) | (1 << ADIE) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADSC);

	TCCR0A |= (1 <<WGM01) | (1 << WGM00);
	TCCR0A |= (1 << COM0A1);
	TCCR0B &= ~(1 << CS00);
	OCR0A = 1;

	sei();

	while(1)
	{
		if(!(PINB && 0x01))
		{
			if(Machine_State < 3) Machine_State++;
			else Machine_State = 1;
		}

		switch(Machine_State)
		{
			// Latarka wylaczona
			case 1:
				Boost_Converter_OFF
				Main_LED_OFF
				Secondary_LEDs_OFF
			break;

			// Dioda inspekcyjna
			case 2:
				Boost_Converter_ON
				Main_LED_ON
				Secondary_LEDs_OFF
			break;

			// Diody rzedowe
			case 3:
				Boost_Converter_ON
				Main_LED_OFF
				Secondary_LEDs_ON
			break;
		}
	}
}

ISR(ADC_vect)
{
	if(EN <= 5)
	{
		if(ADC < Threshold)
		{
			if(OCR0A < 255) OCR0A++;
			EN = 0;
		}
		else
		{
			if(OCR0A > 0) OCR0A--;
			EN = 0;
		}
	}
	else EN++;
}
