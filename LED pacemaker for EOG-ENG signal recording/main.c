/*
 * main.c
 *
 *  Created on: 23 mar 2015
 *      Author: Hexapod
 */

/*
 * Kwarc 10 170 000 Hz
 * Pojedynczy impuls - 10^(-4) - 100us
 * Maksimum zakresu - 65536 - 6,5987s
 * od 1ms do ? 1s jak narazie
 * 1ms	- 10
 * 1s	- 9931
 *
 * TODO:
 * + Rozszerzyc zakres Czas W
 * + Poprawic menu aby wyswietlalo wlasciwy czas i krok
 * - Sprawdzic co sie dzieje z ustawieniem czasu po start/stop/resecie/zmianie trybu
 *
 */

#define DELAY	50
#define OCR		105
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "hd44780.h"

volatile uint8_t Data[3];
volatile uint8_t ENABLE = 0;
volatile uint8_t EN_OCR = 0;
volatile int8_t Duty = 99;
volatile uint8_t NumSeq = 0;
volatile uint8_t Blink = 0;
volatile uint8_t Blink2 = 0;
volatile uint8_t Timer = 0;
uint16_t Static_OCR = 4890;

typedef struct Arc
{
	uint8_t LED[3];
} TArc;

// Inicjalizacja ukladu
void Init();
// Inicjalizacja Sekwencji 1
void Init_S1(TArc *);
// Inicjalizacja Sekwencji 2
void Init_S2(TArc *);

void Init_S3_10deg(TArc *);

void Init_S3_20deg(TArc *);

void Init_S3_30deg(TArc *);

void Init_S3_40deg(TArc *);

void Init_S3_50deg(TArc *);

void Init_S3_60deg(TArc *);

void Init_S3_70deg(TArc *);

void Init_S3_80deg(TArc *);

void Init_S4(TArc *);
// Wrzucenie w pamiec aktualnej konfiguracji LED
void Set_Sequence(uint8_t, TArc *);
// Konwerter uint32_t na 3 bajty wyjciowe dla poszczegolnych portow
void Write(uint32_t, uint8_t *);
// Przejscie miedzy poszczegolnymi stanami MENU
void Test_Encoder1(uint8_t *, uint8_t *, uint8_t *);
// Konfiguracja Trybu, Czasu, Jasnosci
void Test_Encoder2(uint8_t *, uint8_t *, uint8_t *, uint8_t *, uint16_t *, uint8_t *, uint8_t *, uint16_t *);
// Przeliczenie liczby dziesietnej na BCD
void Calc_BCD(uint8_t *, uint16_t);

void Print_Menu(char *, char *, char *, char *, uint8_t, uint8_t, uint16_t, uint8_t, uint8_t *, uint8_t);

void Rewrite(uint16_t *);

int main(void)
{
	char Str1[] = "Jasnosc";
	char Str2[] = "Czas";
	char Str3[] = "Tryb";
	char Str4[] = "ms";
	uint8_t BCD[4];
	uint8_t Encoder1;
	uint8_t Encoder2;
	uint8_t Encoder1_temp;
	uint8_t Encoder2_temp;
	uint8_t Mode = 1;
	uint16_t Time = 500;
	uint8_t Light = 9;
	uint8_t Menu = 0;
	uint8_t TimeS = 6;
	uint8_t Play_Pause = 1;
	uint8_t Stop = 1;
	uint8_t key_lock1 = 0;
	uint8_t key_lock2 = 0;
	uint8_t i;

	TArc Sequence1[17];
	TArc Sequence2[17];
	TArc Sequence3[2];
	TArc Sequence4[2];
	TArc Sequence5[2];
	TArc Sequence6[2];
	TArc Sequence7[2];
	TArc Sequence8[2];
	TArc Sequence9[2];
	TArc Sequence10[2];
	TArc Sequence11[34];
	uint16_t OCR_Table[34] = {5000,	2908, 2136, 1611,
							1229,949,752,625,
							563,625,752,949,
							1229,1611,2136,2908,5000,
							5000,2908,2136,1611,1229,
							949,752,625,563,625,
							752,949,1229,1611,
							2136,2908,5000};

	/*
	 * 0.15 - ~0.5s
	 * 1	- ~2.9s
	 *
	 */

	Init();
	Init_S1(Sequence1);
	Init_S2(Sequence2);
	Init_S3_10deg(Sequence3);
	Init_S3_20deg(Sequence4);
	Init_S3_30deg(Sequence5);
	Init_S3_40deg(Sequence6);
	Init_S3_50deg(Sequence7);
	Init_S3_60deg(Sequence8);
	Init_S3_70deg(Sequence9);
	Init_S3_80deg(Sequence10);
	Init_S4(Sequence11);

	// Wartosc poczatkowa dla enkoderow
	Encoder1 = 0x03 & PINB;
	Encoder2 = (0x0C & PINB) >> 2;

	Data[0] = 0x01;
	Data[1] = 0x01;
	Data[2] = 0x01;

	for(i=0; i<34; i++)
	{
		OCR_Table[i] *= TimeS * 0.05;
	}

    while(1)
    {
    	if(!key_lock1 && !(PINB & 0x10)) // Stop
    	{
    		TCCR1B &= ~(1 << CS12) & ~(1 << CS10);
    		TCNT1 = 0;
    		NumSeq = 0;
    		Data[0] = 0x01;
    		Data[1] = 0x01;
    		Data[2] = 0x01;
    		key_lock1 = 1;
    		Stop = 1;
    		Play_Pause = 1;
    		LCD_Clear();
    	}
    	else if(key_lock1 && (PINB & 0x10)) key_lock1++;

    	if(!key_lock2 && !(PINB & 0x20)) // Start / Pause
    	{
    		key_lock2 = 1;
    		Stop = 0;

    		if(Play_Pause == 0)
    		{
    			Play_Pause = 1;
    		}
    		else
    		{
				Play_Pause = 0;
    		}

    		LCD_Clear();
    	}
    	else if(key_lock2 && (PINB & 0x20)) key_lock2++;

    	// Menu
    	if(Stop == 1)
    	{
    		Test_Encoder1(&Encoder1, &Encoder1_temp, &Menu);
			Test_Encoder2(&Encoder2, &Encoder2_temp, &Menu, &Mode, &Time, &Light, &TimeS, OCR_Table);
			Print_Menu(Str1, Str2, Str3, Str4, Menu, Light, Time, Mode, BCD, TimeS);
    	}

    	// Play
    	if(Play_Pause == 0 && Stop == 0)
    	{
    		TCCR1B |= (1 << CS12) | (1 << CS10);
    		LCD_GoTo(1,0);
    		if(Blink >= 3) Blink = 0;
    		if(Blink == 0)
    		{
    			if(Mode == 1) LCD_WriteText(" =>   ");
				if(Mode == 2) LCD_WriteText("   <= ");
    		}
    		if(Blink == 1)
    		{
    			if(Mode == 1) LCD_WriteText("  =>  ");
				if(Mode == 2) LCD_WriteText("  <=  ");
    		}
    		if(Blink == 2)
    		{
    			if(Mode == 1) LCD_WriteText("   => ");
				if(Mode == 2) LCD_WriteText(" <=   ");
    		}

    		if(Blink2 == 1)
			{
				if(Mode >= 3) LCD_WriteText(" <=   ");
			}
    		else
    		{
    			if(Mode >= 3) LCD_WriteText("   => ");
    		}

    	}

    	// Pause
    	if(Play_Pause == 1 && Stop == 0)
    	{
    		TCCR1B &= ~(1 << CS12) & ~(1 << CS10);
    		LCD_GoTo(2,0);
    		LCD_WriteText("STOP");
    	}

    	// Stop
    	if(Stop == 0)
    	{
    		switch(Mode)
			{
    			case 1:
    				Set_Sequence(17, Sequence1);
    				OCR1A = Static_OCR;
    			break;

    			case 2:
    				Set_Sequence(17, Sequence2);
    				OCR1A = Static_OCR;
    			break;

    			case 3:
    				Set_Sequence(2, Sequence3);
    				OCR1A = Static_OCR;
    			break;

    			case 4:
    				Set_Sequence(2, Sequence4);
    				OCR1A = Static_OCR;
    			break;

    			case 5:
    				Set_Sequence(2, Sequence5);
    				OCR1A = Static_OCR;
    			break;

    			case 6:
    				Set_Sequence(2, Sequence6);
    				OCR1A = Static_OCR;
    			break;

    			case 7:
    				Set_Sequence(2, Sequence7);
    				OCR1A = Static_OCR;
    			break;

    			case 8:
    				Set_Sequence(2, Sequence8);
    				OCR1A = Static_OCR;
    			break;

    			case 9:
    				Set_Sequence(2, Sequence9);
    				OCR1A = Static_OCR;
    			break;

    			case 10:
    				Set_Sequence(2, Sequence10);
    				OCR1A = Static_OCR;
    			break;

    			case 11:
    				Set_Sequence(34, Sequence11);
    				if(EN_OCR == 1)
    				{
    					// Wyzerowac Timer
    					TCNT1 = 0;
    					// Przestawic OCR
    					OCR1A = OCR_Table[NumSeq];

    					EN_OCR = 0;
    				}
    			break;
			}

    	}
    }
    return 0;
}

void Init()
{
	// Konfiguracja wyjsc - LED
	DDRC = 0xFF;
	PORTC = 0x00;
	DDRD = 0xFF;
	PORTD = 0x00;
	DDRD |= (1 << PA7);
	PORTD &= 0x7F;

	// Konfiguracja wejsc
	DDRB = 0x00;
	PORTB = 0xFF;

	// Konfiguracja Timera 0
	TCCR0 |= (1 << WGM01) | (1 << CS02);
	TIMSK |= (1 << OCIE0);
	OCR0 = OCR;

	// Konfiguracja Timera 1 - 16bit
	TCCR1B |= (1 << WGM12);
	TIMSK |= (1 << OCIE1A);
	OCR1A = 4890;

	// Konfiguracja Timera 2 - 8 bit
	TCCR2 |= (1 << WGM21) | (1 << CS22) | (1 << CS21) | (1 << CS20);
	TIMSK |= (1 << OCIE2);
	OCR2 = 255;

	// Inicjalizacja wyswietlacza LCD
	LCD_Initalize();
	LCD_Clear();

	// Wlaczenie przerwan globalnych
	sei();
}

void Init_S1(TArc *Sekwencja)
{
	uint32_t data = 0x00000001;
	uint8_t i;

	for(i=0; i<17; i++)
	{
		Write(data, Sekwencja[i].LED);
		data *= 2;
	}
}

void Init_S2(TArc *Sekwencja)
{
	uint32_t data = 0x00000001;
	int8_t i;

	for(i=16; i>=0; i--)
	{
		Write(data, Sekwencja[i].LED);
		data *= 2;
	}
}

void Init_S3_10deg(TArc *Sekwencja)
{
	Write(0x00000080, Sekwencja[0].LED);
	Write(0x00000200, Sekwencja[1].LED);
}

void Init_S3_20deg(TArc *Sekwencja)
{
	Write(0x00000040, Sekwencja[0].LED);
	Write(0x00000400, Sekwencja[1].LED);
}

void Init_S3_30deg(TArc *Sekwencja)
{
	Write(0x00000020, Sekwencja[0].LED);
	Write(0x00000800, Sekwencja[1].LED);
}

void Init_S3_40deg(TArc *Sekwencja)
{
	Write(0x00000010, Sekwencja[0].LED);
	Write(0x00001000, Sekwencja[1].LED);
}

void Init_S3_50deg(TArc *Sekwencja)
{
	Write(0x00000008, Sekwencja[0].LED);
	Write(0x00002000, Sekwencja[1].LED);
}

void Init_S3_60deg(TArc *Sekwencja)
{
	Write(0x00000004, Sekwencja[0].LED);
	Write(0x00004000, Sekwencja[1].LED);
}

void Init_S3_70deg(TArc *Sekwencja)
{
	Write(0x00000002, Sekwencja[0].LED);
	Write(0x00008000, Sekwencja[1].LED);
}

void Init_S3_80deg(TArc *Sekwencja)
{
	Write(0x00000001, Sekwencja[0].LED);
	Write(0x00010000, Sekwencja[1].LED);
}

void Init_S4(TArc *Sekwencja)
{
	uint32_t data = 0x00000001;
	uint8_t i;

	for(i=0; i<17; i++)
	{
		Write(data, Sekwencja[i].LED);
		data *= 2;
	}

	data = 0x00010000;

	for(i=17; i<35; i++)
	{
		Write(data, Sekwencja[i].LED);
		data /= 2;
	}
}


void Set_Sequence(uint8_t ilosc, TArc *Sekwencja)
{
	if(NumSeq >= ilosc) NumSeq = 0;
	Data[0] = Sekwencja[NumSeq].LED[0];
	Data[1] = Sekwencja[NumSeq].LED[1];
	Data[2] = (0x01 & Sekwencja[NumSeq].LED[2]) << PA7;
}

void Write(uint32_t Arg, uint8_t *Dioda)
{
	uint8_t temp;
	temp = Arg;
	Dioda[0] = temp;
	Arg = Arg >> 8;
	temp = Arg;
	Dioda[1] = temp;
	Arg = Arg >> 8;
	temp = Arg;
	Dioda[2] = temp;
}

void Test_Encoder1(uint8_t *Encoder1, uint8_t *Encoder1_temp, uint8_t *Menu)
{
	*Encoder1_temp = 0x03 & PINB;

	if(*Encoder1 != *Encoder1_temp)
	{
		if((*Encoder1 == 3 && *Encoder1_temp == 1))
		{
			if(*Menu == 0) *Menu = 2;
			else if(*Menu == 1) *Menu = 0;
			else if(*Menu == 2) *Menu = 1;
			LCD_Clear();
		}
		else if((*Encoder1 == 2 && *Encoder1_temp == 0))
		{
			if(*Menu == 0) *Menu = 1;
			else if(*Menu == 1) *Menu = 2;
			else if(*Menu == 2) *Menu = 0;
			LCD_Clear();
		}

		*Encoder1 = *Encoder1_temp;
	}
}

void Test_Encoder2(uint8_t *Encoder2, uint8_t *Encoder2_temp, uint8_t *Menu, uint8_t *Mode, uint16_t *Time, uint8_t *Light, uint8_t *TimeS, uint16_t *OCR_Table)
{
	uint8_t i;

	*Encoder2_temp = (0x0C & PINB) >> 2;

	if(*Encoder2 != *Encoder2_temp)
	{
		if((*Encoder2 == 3 && *Encoder2_temp == 1))
		{
			if(*Menu == 0)
			{
				if(*Light < 9)
				{
					LCD_Clear();
					(*Light)++;
					Duty = *Light * 11;
				}
			}

			if(*Menu == 1)
			{
				if(*Mode == 11)
				{
					if(*TimeS > 4)
					{
						LCD_Clear();
						*TimeS -= 1;
						Rewrite(OCR_Table);
						for(i=0; i<34; i++)
						{
							OCR_Table[i] *= *TimeS * 0.05;
						}
					}
				}
				else
				{
					if(*Time > 60)
					{
						LCD_Clear();
						*Time -= DELAY;
						OCR1A = (*Time * 9.96) - 89.67;
						Static_OCR = OCR1A;
					}
				}
			}

			if(*Menu == 2)
			{
				if(*Mode > 1)
				{
					LCD_Clear();
					(*Mode)--;
				}
			}
		}
		else if((*Encoder2 == 2 && *Encoder2_temp == 0))
		{
			if(*Menu == 0)
			{
				if(*Light > 1)
				{
					LCD_Clear();
					(*Light)--;
					Duty = *Light * 11;
				}
			}

			if(*Menu == 1)
			{
				if(*Mode == 11)
				{
					if(*TimeS < 16)
					{
						LCD_Clear();
						*TimeS += 1;
						Rewrite(OCR_Table);
						for(i=0; i<34; i++)
						{
							OCR_Table[i] *= *TimeS * 0.05;
						}
					}
				}
				else
				{
					if(*Time < 1000)
					{
						LCD_Clear();
						*Time += DELAY;
						OCR1A = (*Time * 9.96) - 89.67;
						Static_OCR = OCR1A;
					}
				}
			}

			if(*Menu == 2)
			{
				if(*Mode < 11)
				{
					LCD_Clear();
					(*Mode)++;
				}
			}
		}

		*Encoder2 = *Encoder2_temp;
	}
}

void Calc_BCD(uint8_t *BCD, uint16_t Time)
{
	int8_t i;

	for(i=3; i>=0; i--)
	{

		BCD[i] = Time % 10;
		Time /= 10;

	}
}

void Print_Menu(char *Str1, char *Str2, char *Str3, char *Str4, uint8_t Menu, uint8_t Light, uint16_t Time, uint8_t Mode, uint8_t *BCD, uint8_t TimeS)
{
	uint8_t i;

	if(Menu == 0)
	{
		LCD_GoTo(0,0);
		LCD_WriteText(Str1);
		LCD_GoTo(3,1);
		LCD_WriteData(48);
		LCD_WriteData((Light*(-1)+10)+48);
	}
	if(Menu == 1)
	{
		if(Mode >= 11)
		{
			LCD_GoTo(1,0);
			LCD_WriteText("CZAS W");
			LCD_GoTo(2,1);
			if(TimeS <= 5)
			{
				LCD_WriteText("0.");
				TimeS += 4;
				LCD_WriteData(TimeS + 48);
				LCD_WriteText("s");
			}
			else
			{
				if(TimeS == 16) LCD_WriteText("2.0s");
				else
				{
					LCD_WriteText("1.");
					TimeS -= 6;
					LCD_WriteData(TimeS + 48);
					LCD_WriteText("s");
				}

			}
		}
		else
		{
			LCD_GoTo(2,0);
			LCD_WriteText(Str2);
			LCD_GoTo(1,1);
			Calc_BCD(BCD, Time);
			for(i=0; i<4; i++)
			{
				LCD_WriteData(BCD[i]+48);
			}

			LCD_WriteText(Str4);
		}

	}
	if(Menu == 2)
	{
		LCD_GoTo(2,0);
		LCD_WriteText(Str3);
		if(Mode >= 3)
		{
			LCD_GoTo(1, 1);

			switch(Mode)
			{
				case 3:
					LCD_WriteText("10 DEG");
				break;

				case 4:
					LCD_WriteText("20 DEG");
				break;

				case 5:
					LCD_WriteText("30 DEG");
				break;

				case 6:
					LCD_WriteText("40 DEG");
				break;

				case 7:
					LCD_WriteText("50 DEG");
				break;

				case 8:
					LCD_WriteText("60 DEG");
				break;

				case 9:
					LCD_WriteText("70 DEG");
				break;

				case 10:
					LCD_WriteText("80 DEG");
				break;
			}

			if(Mode >= 11)
			{
				LCD_GoTo(0,1);
				LCD_WriteText("WAHADLO");
			}
		}
		else
		{
			LCD_GoTo(3,1);

			switch(Mode)
			{
				case 1:
					LCD_WriteText("RP");
				break;

				case 2:
					LCD_WriteText("RL");
				break;
			}
		}

	}
}

void Rewrite(uint16_t *Table)
{
	Table[0] = 5000;
	Table[1] = 2908;
	Table[2] = 2136;
	Table[3] = 1611;
	Table[4] = 1229;
	Table[5] = 949;
	Table[6] = 752;
	Table[7] = 625;
	Table[8] = 563;
	Table[9] = 625;
	Table[10] = 752;
	Table[11] = 949;
	Table[12] = 1229;
	Table[13] = 1611;
	Table[14] = 2136;
	Table[15] = 2908;
	Table[16] = 5000;
	Table[17] = 5000;
	Table[18] = 2908;
	Table[19] = 2136;
	Table[20] = 1611;
	Table[21] = 1229;
	Table[22] = 949;
	Table[23] = 752;
	Table[24] = 625;
	Table[25] = 563;
	Table[26] = 625;
	Table[27] = 752;
	Table[28] = 949;
	Table[29] = 1229;
	Table[30] = 1611;
	Table[31] = 2136;
	Table[32] = 2908;
	Table[33] = 5000;
}

ISR(TIMER1_COMPA_vect)
{
	NumSeq++;
	if(EN_OCR == 0) EN_OCR = 1;
}

ISR(TIMER2_COMP_vect)
{
	Timer++;
	if(Timer >= 15)
	{
		Blink++;
		Blink2 ^= 1;
		Timer = 0;
	}
}

ISR(TIMER0_COMP_vect)
{
	if(ENABLE == 0)
	{
		PORTC = 0x00;
		PORTD = 0x00;
		PORTA &= ~(1 << PA7);
		ENABLE = 1;
		OCR0 = OCR + Duty;
	}
	else
	{
		PORTC = Data[0];
		PORTD = Data[1];
		PORTA |= (0x01 && Data[2]) << PA7;
		ENABLE = 0;
		OCR0 = OCR - Duty;
	}
}
