#define BUTTON_PRESSED !(PIND && (1 << PD7))
#define TIMER_1_ON TCCR1B |= (1 << CS12) | (1 << CS10)
#define TIMER_1_OFF TCCR1B &= ~((1 << CS12) | (1 << CS10))
#define OFFSET 5
#define BCD_ASCII 48

#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "hd44780.h"

// Zmienna konwersji napiecia na temperature
uint16_t Conversion_Table[] = {
		989,	320,
		987,	321,
		985,	322,
		983,	323,
		981,	324,
		979,	325,
		977,	326,
		975,	327,
		973,	328,
		971,	329,
		970,	330,
		968,	331,
		966,	332,
		965,	333,
		963,	334,
		961,	335,
		960,	336,
		958,	337,
		956,	338,
		954,	339,
		953,	340,
		951,	341,
		949,	342,
		948,	343,
		946,	344,
		944,	345,
		941,	346,
		939,	347,
		937,	348,
		935,	349,
		932,	350,
		930,	351,
		928,	352,
		926,	353,
		923,	354,
		921,	355,
		919,	356,
		917,	357,
		914,	358,
		912,	359,
		910,	360,
		908,	361,
		906,	362,
		903,	363,
		901,	364,
		899,	365,
		897,	366,
		895,	367,
		892,	368,
		890,	369,
		888,	370,
		886,	371,
		883,	372,
		881,	373,
		879,	374,
		877,	375,
		875,	376,
		873,	377,
		871,	378,
		869,	379,
		868,	380,
		867,	381,
		866,	382,
		865,	383,
		864,	384,
		863,	385,
		860,	386,
		857,	387,
		855,	388,
		852,	389,
		849,	390,
		847,	391,
		845,	392,
		843,	393,
		841,	394,
		839,	395,
		837,	396,
		835,	397,
		833,	398,
		831,	399,
		829,	400,
		827,	401,
		825,	402,
		823,	403,
		821,	404,
		819,	405,
		817,	406,
		815,	407,
		813,	408,
		811,	409,
		809,	410,
		807,	411,
		805,	412,
		803,	413,
		801,	414,
		799,	415,
		797,	416,
		795,	417,
		793,	418,
		790,	419,
		788,	420,
		785,	421,
		783,	422,
		781,	423,
		779,	424,
		777,	425,
		775,	426,
		773,	427,
		772,	428,
		770,	429,
		769,	430,
		767,	431,
		765,	432,
		763,	433,
		761,	434,
		759,	435,
		758,	436,
		756,	437,
		755,	438,
		753,	439,
		751,	440,
		749,	441,
		747,	442,
		745,	443,
		743,	444,
		741,	445,
		739,	446,
		737,	447,
		735,	448,
		733,	449,
		731,	450
};

char Row_1[] = "Temp: --.--     "; // Zmienna - pierwszy wiersz lcd
char Row_2[] = "Ost: --.--     O"; // Zmienna - drugi wiersz lcd
volatile uint16_t Temperature_ADC; // Zmienna przechowuj¹ca aktualne dane zmierzone na przetworniku ADC
volatile uint16_t Previous_Temperature = 0; // Zmienna pamiêtaj¹ca ostatni pomiar temperatury
volatile uint8_t Machine_State = 0; // Zmienna kontrolna maszyny stanow
volatile uint8_t EN = 1; // Zmienna potwierdzajaca koniec pomiaru

void Init_uC(void); // Funkcja inicjalizujaca porty i rejestry
void Update_Temperature(uint16_t Index, uint8_t Type); // Funkcja aktualizacji temperatury na wyswietlaczu
void Convert_Data(uint16_t Actual_Temperature, uint8_t Row); // Funkcja konwersji danych ADC na temperature w stopniach Celsjusza
void Update_Prev_Temperature(uint16_t Index, uint8_t Type); // Funkcja aktualizacji temperatury ostatniego pomiaru

int main(void)
{
	// Inicjalizacja zmiennych programu
	uint8_t M_Complete = 1;
	Row_1[11] = 223;
	Row_2[10] = 223;

	// Inicjalizacja wyswietlacza lcd
	_delay_ms(500);
	lcd_init();
	LCD_DISPLAY(LCDDISPLAY);
	LCD_CLEAR;

	Init_uC();

	while(1)
	{

		// Stan oczekujacy na wcisniecie przycisku rozpoczynajacego pomiar
		if(Machine_State == 0)
		{
			if(BUTTON_PRESSED)
			{
				while(BUTTON_PRESSED);
				_delay_ms(100);

				Machine_State = 1;
				Row_2[15] = 'P';
			}
		}

		// Stan pomiarowy - trwa do momentu az wzrost temperatury bedzie sie zmienial co 0.1 stopnia C w zadanym interwale czasu
		if(Machine_State == 1)
		{
			while(M_Complete)
			{
				EN = 1;
				Previous_Temperature = Temperature_ADC;
				TIMER_1_ON;
				while(EN)
				{
					Convert_Data(Temperature_ADC, 0);
				}
				if((Previous_Temperature - Temperature_ADC) < 4)
				{
					Convert_Data(Temperature_ADC, 1);
					M_Complete = 0;
				}
			}
			M_Complete = 1;

			Machine_State = 2;
			Row_2[15] = 'Z';
		}

		// Stan zapisu poprzedniego pomiaru oraz przygotowania termometru do nastepnych pomiarow
		if(Machine_State == 2)
		{
			if(BUTTON_PRESSED)
			{
				while(BUTTON_PRESSED);
				_delay_ms(100);

				Machine_State = 0;
				Row_2[15] = 'O';
				Row_1[6] = '-';
			    Row_1[7] = '-';
				Row_1[9] = '-';
				Row_1[10] = '-';
				Row_1[15] = ' ';
			}
		}
	}
}

void Init_uC(void)
{
	// Inicjalizacja portow przycisku
	DDRD &= ~((1 << PD7));
	PORTD |= (1 << PD7);

	// Inicjalizacja licznika odswiezajacego wyswietlacz
	TCCR0 |= (1 << WGM01) | (1 << CS02) | (1 << CS00);
	OCR0 = 200;
	TIMSK |= (1 << OCIE0);

	// Inicjalizacja licznika odmierzajacego interwal pomiarowy
	TCCR1B |= (1 << WGM12)
	OCR1A = 6000;
	TIMSK |= (1 << OCIE1A);

	// Inicjalizacja przetwornika ADC
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADMUX |= (1 << REFS1) | (1 << REFS0);
	ADMUX &= ~((1 << MUX4) | (1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
	ADCSRA |= (1 << ADEN);
	ADCSRA |= (1 << ADATE);
	ADCSRA |= (1 << ADIE);
	ADCSRA |= (1 << ADSC);

	sei();
}

void Convert_Data(uint16_t Actual_Temperature, uint8_t Row)
{
	uint16_t i;

	// Petla przeszukujaca tablice danych
	for(i=0; i < 132; i++)
	{
		// Warunek idealnego dopasowania
		if(Conversion_Table[2*i] == Actual_Temperature)
		{
			if(Row == 0) Update_Temperature(i, 0);
			else Update_Prev_Temperature(i, 0);
			break;
		}
		// Warunek dla interpolacji danych tabeli
		else if((Conversion_Table[2*i] > Actual_Temperature) && (Conversion_Table[2*i+2] < Actual_Temperature))
		{
			if(Row == 0) Update_Temperature(i, 1);
			else Update_Prev_Temperature(i, 1);
			break;
		}
		//Warunek dla temperatury wyzszej niz 45 stopni C
		if(Actual_Temperature > 989)
		{
			if(Row == 0)
			{
				Update_Temperature(0, 0);
				Row_1[15] = 'L';
			}
			else Update_Prev_Temperature(0, 0);
		}
		//Warunek dla temperatury nizszej niz 45 stopni C
		else if(Actual_Temperature < 731)
		{
			if(Row == 0)
			{
				Update_Temperature(130, 0);
				Row_1[15] = 'H';
			}
			else Update_Prev_Temperature(130, 0);
		}
		else Row_1[15] = ' ';
	}
}

void Update_Temperature(uint16_t Index, uint8_t Type)
{
	uint16_t Temporary;
	Temporary = Conversion_Table[2*Index + 1];

	// Konwersja liczby binarnej na BCD
	if(Type == 0) Row_1[5 + OFFSET] = 0 + BCD_ASCII;
	else Row_1[5 + OFFSET] = 5 + BCD_ASCII;

	Row_1[4 + OFFSET] = Temporary % 10 + BCD_ASCII;
	Temporary /= 10;
	Row_1[2 + OFFSET] = Temporary % 10 + BCD_ASCII;
	Temporary /= 10;
	Row_1[1 + OFFSET] = Temporary % 10 + BCD_ASCII;
	Temporary /= 10;
}

void Update_Prev_Temperature(uint16_t Index, uint8_t Type)
{
	uint16_t Temporary;
	Temporary = Conversion_Table[2*Index + 1];

	// Konwersja liczby binarnej na BCD
	if(Type == 0) Row_2[4 + OFFSET] = 0 + BCD_ASCII;
	else Row_2[4 + OFFSET] = 5 + BCD_ASCII;

	Row_2[3 + OFFSET] = Temporary % 10 + BCD_ASCII;
	Temporary /= 10;
	Row_2[1 + OFFSET] = Temporary % 10 + BCD_ASCII;
	Temporary /= 10;
	Row_2[0 + OFFSET] = Temporary % 10 + BCD_ASCII;
	Temporary /= 10;
}

//Przerwanie odswiezania LCD
ISR(TIMER0_COMP_vect)
{
	LCD_LOCATE(0,0);
	lcd_puts(Row_1);
	LCD_LOCATE(0,1);
	lcd_puts(Row_2);
}

// Przerwanie interwalu pomiarowego
ISR(TIMER1_COMPA_vect)
{
	EN = 0;
	TIMER_1_OFF;
}

// Przerwanie od wykonanego pomiaru przetwornika ADC
ISR(ADC_vect)
{
	Temperature_ADC = ADC;
}
