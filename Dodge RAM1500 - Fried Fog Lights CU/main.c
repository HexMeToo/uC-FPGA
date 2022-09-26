/*
 * main.c
 *
 *  Created on: 2 maj 2015
 *      Author: Hexapod
 */

/*
 * Za³o¿enia projektu:
 * + Sterowanie przekaŸnikiem na podstawie pomiaru napiêcia
 * + pomiar o czêstotliwosci 100Hz
 * + postarac siê o NOPy w mainie
 */

#define ADC_W 205
#define DELAY 30
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

int main()
{
	// Inicjalizacja Portow WE/WY
	DDRB = 0x08;
	PORTB = 0x00;

	// ADC
	ADMUX &= ~(1 << REFS0); // Wewnetrzne napiecie odniesienia
	ADMUX &= ~(1 << ADLAR); // Wyjustowanie do prawej
	ADMUX |= (1 << MUX0); // Wybor kanalu ADC1 (PB2)
	ADCSRA |= (1 << ADEN); // ADC Enable
	ADCSRA |= (1 << ADIE); // ADC Interrput Enable
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // ADC Prescaler

	// Timer
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << CS02) | (1 << CS00);
	TIMSK0 |= (1 << OCIE0A);
	OCR0A = 94; // 100Hz - Czestotliwosc wywolywania przerwania

	// Globalne przerwania
	sei();

	while(1)
	{

	}

	return 0;
}

ISR(ADC_vect)
{
	// Zmienna okreslajaca stan maszyny
	static uint8_t mode = 1;
	// Zmienna zliczajaca czas przytrzymania przycisku
	static uint8_t timer = 0;

	// Instrukcja wyboru stanu
	switch(mode)
	{
		// Zalaczenie przekaznika
		case 1:
			if(ADC > 500)
			{
				timer++;
				if(timer >= DELAY)
				{
					mode = 2;
					PORTB |= (1 << PB3);
					timer = 0;
				}
			}
		break;

		// Oczekiwanie na zwolnienie przycisku
		case 2:
			if(ADC < ADC_W)
			{
				mode = 3;
			}
		break;

		// Wylaczenie przekaznika
		case 3:
			if(ADC > ADC_W)
			{
				timer++;
				if(timer >= DELAY)
				{
					mode = 4;
					PORTB &= ~(1 << PB3);
					timer = 0;
				}
			}
		break;

		// Oczekiwanie na zwolnienie przycisku
		case 4:
			if(ADC < ADC_W)
			{
				mode = 1;
			}
		break;
	}
}

ISR(TIM0_COMPA_vect)
{
	// Zalaczenie pomiaru napiecia
	ADCSRA |= (1 << ADSC);
}
