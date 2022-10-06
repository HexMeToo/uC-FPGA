#define ASCII 48
#define ZNAK_PELNY 43
#define ZNAK_PUSTY ' '
#include <avr/io.h>
#include <util/delay.h>
#include "hd44780.h"
#include <avr/interrupt.h>

char Zegar[] = "    00:00:00    ";
char Zegar_2[] = "                ";
volatile uint16_t Licznik = 0;
volatile uint16_t Odswiez = 0;
volatile uint8_t Godzina = 0;
volatile uint8_t Minuta = 0;
volatile uint8_t Sekunda = 0;
volatile uint8_t Aktualizacja = 0;
volatile uint8_t Tryb_Ustawiania_Czasu = 0;

void Aktualizuj_Czas(void);
void Dzwonek(void);

int main(void)
{
	uint8_t Pozycja = 3;

	_delay_ms(500);
	lcd_init();
	LCD_DISPLAY(LCDDISPLAY);
	LCD_CLEAR;

	DDRD |= 1 << PD6;
	PORTD &= ~(1 << PD6);
	DDRD &= ~((1 << PD4) | (1 << PD5));
	PORTD |= (1 << PD4) | (1 << PD5);

	TCCR0 |= (1 << WGM01) | (1 << CS01);
	OCR0 = 200;
	TIMSK |= (1 << OCIE0);

	sei();

	while(1)
	{
		if(Tryb_Ustawiania_Czasu == 0)
		{
			if(Godzina == 8 && Minuta == 0 && Sekunda == 0) Dzwonek();
			if(Godzina == 8 && Minuta == 45 && Sekunda == 0) Dzwonek();
			if(Godzina == 8 && Minuta == 55 && Sekunda == 0) Dzwonek();
			if(Godzina == 9 && Minuta == 40 && Sekunda == 0) Dzwonek();
			if(Godzina == 9 && Minuta == 50 && Sekunda == 0) Dzwonek();
			if(Godzina == 10 && Minuta == 35 && Sekunda == 0) Dzwonek();
			if(Godzina == 10 && Minuta == 45 && Sekunda == 0) Dzwonek();
			if(Godzina == 11 && Minuta == 30 && Sekunda == 0) Dzwonek();
			if(Godzina == 11 && Minuta == 40 && Sekunda == 0) Dzwonek();
			if(Godzina == 12 && Minuta == 25 && Sekunda == 0) Dzwonek();
			if(Godzina == 12 && Minuta == 35 && Sekunda == 0) Dzwonek();
			if(Godzina == 13 && Minuta == 20 && Sekunda == 0) Dzwonek();
			if(Godzina == 13 && Minuta == 30 && Sekunda == 0) Dzwonek();
			if(Godzina == 14 && Minuta == 15 && Sekunda == 0) Dzwonek();
			if(Godzina == 14 && Minuta == 25 && Sekunda == 0) Dzwonek();
			if(Godzina == 15 && Minuta == 15 && Sekunda == 0) Dzwonek();
		}

		if(!(PIND & (1 << PD4)))
		{
			_delay_ms(200);

			if(Pozycja == 2)
			{
				if(Sekunda == 59) Sekunda = 0;
				else Sekunda++;
			}
			if(Pozycja == 1)
			{
				if(Minuta == 59) Minuta = 0;
				else Minuta++;
			}
			if(Pozycja == 0)
			{
				if(Godzina == 23) Godzina = 0;
				else Godzina++;
			}
		}

		if(!(PIND & (1 << PD5)))
		{
			_delay_ms(300);
			Tryb_Ustawiania_Czasu = 1;

			Pozycja++;

			if(Pozycja >= 4) Pozycja = 0;
			if(Pozycja == 3)
			{
				Tryb_Ustawiania_Czasu = 0;
				Zegar_2[4] = ZNAK_PUSTY;
				Zegar_2[5] = ZNAK_PUSTY;
				Zegar_2[7] = ZNAK_PUSTY;
				Zegar_2[8] = ZNAK_PUSTY;
				Zegar_2[10] = ZNAK_PUSTY;
				Zegar_2[11] = ZNAK_PUSTY;
			}
		}

		if(Tryb_Ustawiania_Czasu == 1)
		{
			if(Pozycja == 0)
			{
				Zegar_2[4] = ZNAK_PELNY;
				Zegar_2[5] = ZNAK_PELNY;
				Zegar_2[7] = ZNAK_PUSTY;
				Zegar_2[8] = ZNAK_PUSTY;
				Zegar_2[10] = ZNAK_PUSTY;
				Zegar_2[11] = ZNAK_PUSTY;
			}
			if(Pozycja == 1)
			{
				Zegar_2[4] = ZNAK_PUSTY;
				Zegar_2[5] = ZNAK_PUSTY;
				Zegar_2[7] = ZNAK_PELNY;
				Zegar_2[8] = ZNAK_PELNY;
				Zegar_2[10] = ZNAK_PUSTY;
				Zegar_2[11] = ZNAK_PUSTY;
			}
			if(Pozycja == 2)
			{
				Zegar_2[4] = ZNAK_PUSTY;
				Zegar_2[5] = ZNAK_PUSTY;
				Zegar_2[7] = ZNAK_PUSTY;
				Zegar_2[8] = ZNAK_PUSTY;
				Zegar_2[10] = ZNAK_PELNY;
				Zegar_2[11] = ZNAK_PELNY;
			}
		}

		if(Aktualizacja == 1)
		{
			Aktualizuj_Czas();
		}
	}
}

void Dzwonek(void)
{
	uint8_t i;
	for(i=0; i<5; i++)
	{
		PORTD |= 1 << PD6;
		_delay_ms(50);
		PORTD &= ~(1 << PD6);
		_delay_ms(50);
	}
}

void Aktualizuj_Czas(void)
{
	uint8_t Bufor;

	Bufor = Godzina;
	Zegar[5] = (Bufor % 10) + ASCII;
	Bufor /= 10;
	Zegar[4] = (Bufor % 10) + ASCII;

	Bufor = Minuta;
	Zegar[8] = (Bufor % 10) + ASCII;
	Bufor /= 10;
	Zegar[7] = (Bufor % 10) + ASCII;

	Bufor = Sekunda;
	Zegar[11] = (Bufor % 10) + ASCII;
	Bufor /= 10;
	Zegar[10] = (Bufor % 10) + ASCII;

	Aktualizacja = 0;
}

ISR(TIMER0_COMP_vect)
{
	if(Licznik >= 10000)
	{
		Licznik = 0;

		if(Sekunda >= 59)
		{
			Sekunda = 0;
			Minuta++;
			if(Minuta >= 59)
			{
				Minuta = 0;
				Godzina++;
				if(Godzina >= 23) Godzina = 0;
			}
			else Minuta++;
		}
		else Sekunda++;
	}
	else Licznik++;

	if(Odswiez >= 2000)
	{
		Odswiez = 0;
		Aktualizacja = 1;
		LCD_LOCATE(0,0);
		lcd_puts(Zegar);
		LCD_LOCATE(0,1);
		lcd_puts(Zegar_2);
	}
	else Odswiez++;
}
