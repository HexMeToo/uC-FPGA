#define BULB_PIN_ON PORTC |= (1 << PC1)
#define BULB_PIN_OFF PORTC &= ~(1 << PC1)
#define BACKLIGHT_OFF PORTD &= ~(1 << PD7)
#define BACKLIGHT_ON PORTD |= (1 << PD7)
#define BUZZER_ON PORTC |= (1 << PC2);
#define BUZZER_OFF PORTC &= ~(1 << PC2)

#define BUTTON_1_PRESSED 1
#define BUTTON_2_PRESSED 2
#define BUTTON_3_PRESSED 3
#define BUTTON_4_PRESSED 4
#define ROW_LENGTH 16
#define TIMER2_CAP 256
#define IDLE_LEVEL 128
#define PWM_MAX 10
#define TWI_WRITE_BIT 0
#define TWI_READ_BIT 1
#define BUZZER_DELAY 75
#define FAST_BUTTON_DELAY 240

#define MAIN_ROW " MAN  AUT  PROG "
#define MAIN_KBRD_ROW "    <-  -> ENTER"
#define DIMM_ROW "DIMM LEVEL: --- "
#define DIMM_KBRD_ROW "ESC   -   +     "
#define AUTO_ROW "      AUTO      "
#define AUTO_KBRD_ROW "ESC             "
#define PROG_ROW "   --:--        "
#define PROG_KBRD_P_ROW "      -   +   ->"
#define PROG_KBRD_H_ROW "ESC   U       ->"
#define HOUR_KBRD_ROW "ESC   -   +     "

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "hd44780.h"
#include "twi.h"

volatile uint8_t Button = 0;
volatile uint8_t Auto_Dimm_Level;
volatile uint8_t Mode = 0;
volatile uint8_t Dimm_Level = 1;
char Row_1[16] = "----------------";
char Row_2[16] = "----------------";

void Buzzer_Confirm(uint8_t Beeps)
{
	for(Beeps=0; Beeps<2; Beeps++)
	{
		BUZZER_ON;
		_delay_ms(BUZZER_DELAY);
		BUZZER_OFF;
		_delay_ms(BUZZER_DELAY);
	}
}

void DS1307_Init(void)
{
	twistart();
	twiwrite((0x68 << 1) | TWI_WRITE_BIT);
	twiwrite(0x00); // Adres pocz¹tkowy
	twiwrite(0x00); // Sekundy
	twistop();
}

void DS1307_Write_Clock(uint8_t Hour, uint8_t Minutes)
{
	uint8_t Buffer_H, Buffer_M;

	Buffer_H = Hour / 10;
	Buffer_M = Minutes / 10;
	Hour %= 10;
	Minutes %= 10;

	twistart();
	twiwrite((0x68 << 1) | TWI_WRITE_BIT);
	twiwrite(0x00); // Adres pocz¹tkowy
	twiwrite(0x00); // Sekundy
	twiwrite(((Buffer_M << 4) | (Minutes & 0x0F)) & 0x7F);
	twiwrite(((Buffer_H << 4) | (Hour & 0x0F)) & 0x3F);
	twistop();
}

void DS1307_Read_Clock(uint8_t *Clock)
{
	twistart();
	twiwrite((0x68 << 1) | TWI_WRITE_BIT);
	twiwrite(0x01); // Adres pocz¹tkowy
	twistart();
	twiwrite((0x68 << 1) | TWI_READ_BIT);
	Clock[0] = twiread(ACK);
	Clock[1] = twiread(NOACK);
	twistop();
	Clock[0] = (Clock[0] >> 4) * 10 + (Clock[0] & 0x0F);
	Clock[1] = (Clock[1] >> 4) * 10 + (Clock[1] & 0x0F);
}

void Update_Row_Buffer(char *Row_Data, char *New_Data)
{
	uint8_t i;
	for(i=0; i<ROW_LENGTH; i++) Row_Data[i] = New_Data[i];
}

void Convert_Dec_to_BCD_Manual(uint8_t Number)
{
	uint8_t i;

	for(i=14; i>11; i--)
	{
		Row_1[i] = (Number % 10) + 48;
		Number/= 10;
	}
}

void Convert_Hour_to_BCD(uint8_t Hour, uint8_t Minutes)
{
	uint8_t i;

	for(i=4; i>2; i--)
	{
		Row_1[i] = (Hour % 10) + 48;
		Hour/= 10;
	}

	for(i=7; i>5; i--)
	{
		Row_1[i] = (Minutes % 10) + 48;
		Minutes/= 10;
	}
}

int main(void)
{
	uint8_t Clock[2] = {0};
	uint8_t P1_H = 0;
	uint8_t P1_M = 0;
	uint8_t P2_H = 0;
	uint8_t P2_M = 0;
	uint8_t P3_H = 0;
	uint8_t P3_M = 0;
	uint8_t P4_H = 0;
	uint8_t P4_M = 0;
	uint8_t Main_Hour = 0;
	uint8_t Main_Minutes = 0;
	uint16_t i = 0;
	uint16_t key_lock_1 = 0;
	uint16_t key_lock_2 = 0;
	uint16_t key_lock_3 = 0;
	uint16_t key_lock_4 = 0;

	// Inicjalizacja LCD
	lcd_init();
	LCD_DISPLAY(LCDDISPLAY);
	LCD_CLEAR;

	// Inicjalizacja I2C
	TWI_Init();

	DS1307_Init();

	// Podswietlenie
	DDRD |= (1 << PD7);
	PORTD |= (1 << PD7);
//	PORTD &= ~(1 << PD7);

	// Przyciski
	DDRB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2));
	PORTB |= (1 << PB0) | (1 << PB1) | (1 << PB2);
	DDRC &= ~(1 << PC1);
	PORTC |= (1 << PC1);

	// Wejscie ADC
	DDRC &= ~(1 << PC0);
	PORTC &= ~(1 << PC0);

	// Zarowka
	DDRB |= (1 << PB3);
	PORTB &= ~(1 << PB3);

	// Buzzer
	DDRC |= (1 << PC2);
	PORTC &= ~(1 << PC2);

	// Inicjalizacja ADC
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADEN) | (1 << 5) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	ADCSRA |= (1 << ADSC);

	// Inicjalizacja Timer1
	TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);
	TIMSK |= (1 << OCIE1A);
	OCR1A = 2000;

	// Inicjalizacja Timer2 - Fast PWM
	TCCR2 |= (1 << WGM21) | (1 << WGM20) | (1 << COM21);
	TCCR2 |= (1 << CS22) | (1 << CS21) | (1 << CS20);
	OCR2 = 10;

	sei();

	Update_Row_Buffer(Row_1, MAIN_ROW);
	Update_Row_Buffer(Row_2, MAIN_KBRD_ROW);
	Row_1[0] = '*';

	while(1)
	{
		if(Button != 0)
		{
			if(Mode >= 0 && Mode <= 2)
			{
				if(Button == BUTTON_3_PRESSED)
				{
					if(Mode == 2) Mode = 0;
					else Mode++;
				}
				if(Button == BUTTON_2_PRESSED)
				{
					if(Mode == 0) Mode = 2;
					else Mode--;
				}
				if(Button == BUTTON_4_PRESSED)
				{
					if(Mode == 0)
					{
						Mode = 3;
						Update_Row_Buffer(Row_1, DIMM_ROW);
						Update_Row_Buffer(Row_2, DIMM_KBRD_ROW);
					}
					if(Mode == 1)
					{
						Mode = 4;
						Update_Row_Buffer(Row_1, AUTO_ROW);
						Update_Row_Buffer(Row_2, AUTO_KBRD_ROW);
					}
					if(Mode == 2)
					{
						Mode = 5;
						Update_Row_Buffer(Row_1, PROG_ROW);
						Update_Row_Buffer(Row_2, PROG_KBRD_H_ROW);
					}
				}
			}
			else if(Mode == 3) // Tryb manual
			{
				if(Button == BUTTON_1_PRESSED)
				{
					Mode = 0;
					Update_Row_Buffer(Row_1, MAIN_ROW);
					Update_Row_Buffer(Row_2, MAIN_KBRD_ROW);
				}
				if(Button == BUTTON_3_PRESSED)
				{
					if(OCR2 >= 0 && OCR2 <= 254)
					{
						OCR2++;
						Convert_Dec_to_BCD_Manual(OCR2);
					}
				}
				if(Button == BUTTON_2_PRESSED)
				{
					if(OCR2 >= 1 && OCR2 <= 255)
					{
						OCR2--;
						Convert_Dec_to_BCD_Manual(OCR2);
					}
				}
			}
			else if(Mode == 4) // Tryb auto
			{
				if(Button == BUTTON_1_PRESSED)
				{
					Mode = 0;
					Update_Row_Buffer(Row_1, MAIN_ROW);
					Update_Row_Buffer(Row_2, MAIN_KBRD_ROW);
				}
			}
			else if(Mode == 5) // Tryb programowania godziny
			{
				if(Button == BUTTON_1_PRESSED)
				{
					Mode = 0;
					Update_Row_Buffer(Row_1, MAIN_ROW);
					Update_Row_Buffer(Row_2, MAIN_KBRD_ROW);
				}
				if(Button == BUTTON_4_PRESSED)
				{
					Mode = 6;
					Update_Row_Buffer(Row_1, PROG_ROW);
					P1_H = Clock[1];
					P1_M = Clock[0];
					Convert_Hour_to_BCD(P1_H, P1_M);
					Update_Row_Buffer(Row_2, PROG_KBRD_P_ROW);
				}
				if(Button == BUTTON_2_PRESSED)
				{
					Mode = 10;
					Update_Row_Buffer(Row_1, PROG_ROW);
					Update_Row_Buffer(Row_2, HOUR_KBRD_ROW);
				}
			}
			else if(Mode >= 6 && Mode <= 9)
			{
//				if(Button == BUTTON_1_PRESSED)
//				{
//					Mode = 0;
//					Update_Row_Buffer(Row_1, MAIN_ROW);
//					Update_Row_Buffer(Row_2, MAIN_KBRD_ROW);
//				}
				if(Button == BUTTON_4_PRESSED)
				{
					Mode++;

					if(Mode == 10)
					{
						Mode = 5;
						Update_Row_Buffer(Row_1, PROG_ROW);
						Update_Row_Buffer(Row_2, PROG_KBRD_H_ROW);
						Buzzer_Confirm(2);
					}

					if(Mode == 6 || Mode == 8)
					{
						Update_Row_Buffer(Row_1, PROG_ROW);
						if(Mode == 6)
						{
							P1_H = Clock[1];
							P1_M = Clock[0];
							Convert_Hour_to_BCD(P1_H, P1_M);
						}
						if(Mode == 8)
						{
							P3_H = Clock[1];
							P3_M = Clock[0];
							Convert_Hour_to_BCD(P3_H, P3_M);
						}
						Update_Row_Buffer(Row_2, PROG_KBRD_P_ROW);
						Buzzer_Confirm(2);
					}

					if(Mode == 7 || Mode == 9)
					{
						Update_Row_Buffer(Row_1, PROG_ROW);
						if(Mode == 7)
						{
							P2_H = Clock[1];
							P2_M = Clock[0];
							Convert_Hour_to_BCD(P2_H, P2_M);
						}
						if(Mode == 9)
						{
							P4_H = Clock[1];
							P4_M = Clock[0];
							Convert_Hour_to_BCD(P4_H, P4_M);
						}
						Update_Row_Buffer(Row_2, PROG_KBRD_P_ROW);
					}
				}
				if(Button == BUTTON_2_PRESSED)
				{
					if(Mode == 6)
					{
						if(P1_M == 0)
						{
							P1_M = 59;
							if(P1_H == 0) P1_H = 23;
							else P1_H--;
						}
						else P1_M--;

						Convert_Hour_to_BCD(P1_H, P1_M);
					}
					if(Mode == 7)
					{
						if(P2_M == 0)
						{
							P2_M = 59;
							if(P2_H == 0) P2_H = 23;
							else P2_H--;
						}
						else P2_M--;

						Convert_Hour_to_BCD(P2_H, P2_M);
					}
					if(Mode == 8)
					{
						if(P3_M == 0)
						{
							P3_M = 59;
							if(P3_H == 0) P3_H = 23;
							else P3_H--;
						}
						else P3_M--;

						Convert_Hour_to_BCD(P3_H, P3_M);
					}
					if(Mode == 9)
					{
						if(P4_M == 0)
						{
							P4_M = 59;
							if(P4_H == 0) P4_H = 23;
							else P4_H--;
						}
						else P4_M--;

						Convert_Hour_to_BCD(P4_H, P4_M);
					}
				}
				if(Button == BUTTON_3_PRESSED)
				{
					if(Mode == 6)
					{
						if(P1_M == 59)
						{
							P1_M = 0;
							if(P1_H == 23) P1_H = 0;
							else P1_H++;
						}
						else P1_M++;

						Convert_Hour_to_BCD(P1_H, P1_M);
					}
					if(Mode == 7)
					{
						if(P2_M == 59)
						{
							P2_M = 0;
							if(P2_H == 23) P2_H = 0;
							else P2_H++;
						}
						else P2_M++;

						Convert_Hour_to_BCD(P2_H, P2_M);
					}
					if(Mode == 8)
					{
						if(P3_M == 59)
						{
							P3_M = 0;
							if(P3_H == 23) P3_H = 0;
							else P3_H++;
						}
						else P3_M++;

						Convert_Hour_to_BCD(P3_H, P3_M);
					}
					if(Mode == 9)
					{
						if(P4_M == 59)
						{
							P4_M = 0;
							if(P4_H == 23) P4_H = 0;
							else P4_H++;
						}
						else P4_M++;

						Convert_Hour_to_BCD(P4_H, P4_M);
					}
				}
			}
			else if(Mode == 10)
			{
				if(Button == BUTTON_1_PRESSED)
				{
					Mode = 5;
					Update_Row_Buffer(Row_1, PROG_ROW);
					Update_Row_Buffer(Row_2, PROG_KBRD_H_ROW);
					DS1307_Write_Clock(Main_Hour, Main_Minutes);
					Buzzer_Confirm(2);
				}
				if(Button == BUTTON_3_PRESSED)
				{
					if(Main_Minutes == 59)
					{
						Main_Minutes = 0;
						if(Main_Hour == 23) Main_Hour = 0;
						else Main_Hour++;
					}
					else Main_Minutes++;

					Convert_Hour_to_BCD(Main_Hour, Main_Minutes);
				}
				if(Button == BUTTON_2_PRESSED)
				{
					if(Main_Minutes == 0)
					{
						Main_Minutes = 59;
						if(Main_Hour == 0) Main_Hour = 23;
						else Main_Hour--;
					}
					else Main_Minutes--;

					Convert_Hour_to_BCD(Main_Hour, Main_Minutes);
				}
			}

			switch(Mode)
			{
				// Menu glowne
				case 0:
					Row_1[0] = '*';
					Row_1[5] = ' ';
					Row_1[10] = ' ';
				break;

				case 1:
					Row_1[0] = ' ';
					Row_1[5] = '*';
					Row_1[10] = ' ';
				break;

				case 2:
					Row_1[0] = ' ';
					Row_1[5] = ' ';
					Row_1[10] = '*';
				break;

				case 3:
				break;

				case 4:
				break;

				case 5:
					Row_1[14] = ' ';
					Row_1[15] = 'H';
				break;

				case 6:
					Row_1[14] = 'O';
					Row_1[15] = 'N';
				break;

				case 7:
					Row_1[13] = 'O';
					Row_1[14] = 'F';
					Row_1[15] = 'F';
				break;

				case 8:
					Row_1[14] = 'O';
					Row_1[15] = 'N';
				break;

				case 9:
					Row_1[13] = 'O';
					Row_1[14] = 'F';
					Row_1[15] = 'F';
				break;
			}

			Button = 0;
		}

		if(Mode == 5 && i>65000)
		{
			i=0;
			DS1307_Read_Clock(Clock);
			Convert_Hour_to_BCD(Clock[1], Clock[0]);

			if(Clock[1] == P1_H && Clock[0] == P1_M) OCR2 = 255;
			else if(Clock[1] == P2_H && Clock[0] >= P2_M) OCR2 = 0;
			else if(Clock[1] == P3_H && Clock[0] >= P3_M) OCR2 = 255;
			else if(Clock[1] == P4_H && Clock[0] >= P4_M) OCR2 = 0;
		}
		else i++;

		if(!(PINB & (1 << PB0)) && key_lock_1 == 0)
		{
			key_lock_1 = 1;
			Button = BUTTON_4_PRESSED;

		}
		else if((PINB & (1 << PB0)) && key_lock_1!=0) key_lock_1++;

		if(!(PINB & (1 << PB1)) && key_lock_2 == 0)
		{
			if(Mode == 3 || (Mode >=6 && Mode <=10)) _delay_ms(FAST_BUTTON_DELAY);
			else key_lock_2 = 1;
			Button = BUTTON_3_PRESSED;
		}
		else if((PINB & (1 << PB1)) && key_lock_2!=0) key_lock_2++;

		if(!(PINB & (1 << PB2)) && key_lock_3 == 0)
		{
			if(Mode == 3 || (Mode >=6 && Mode <=10)) _delay_ms(FAST_BUTTON_DELAY);
			else key_lock_3 = 1;
			Button = BUTTON_2_PRESSED;
		}
		else if((PINB & (1 << PB2)) && key_lock_3!=0) key_lock_3++;

		if(!(PINC & (1 << PC1)) && key_lock_4 == 0)
		{
			key_lock_4 = 1;
			Button = BUTTON_1_PRESSED;
		}
		else if((PINC & (1 << PC1)) && key_lock_4!=0) key_lock_4++;
	}
}

ISR(TIMER1_COMPA_vect)
{
	LCD_LOCATE(0,0);
	lcd_puts(Row_1);
	LCD_LOCATE(0,1);
	lcd_puts(Row_2);
}

ISR(ADC_vect)
{
	if(Mode == 4) OCR2 = 255 - (ADC * 0.25); // Zakres 0 - 256
}
