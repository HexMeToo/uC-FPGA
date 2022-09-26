/*
 * main.c
 *
 *  Created on: 7-12-2015
 *      Author: Mariusz
 */

#define START_TIMER16 TCCR1B |= (1 << CS12) | (1 << CS10)
#define STOP_TIMER16 TCCR1B &= ~((1 << CS12) | (1 << CS10))
#define CLEAR_TIMER16 TCNT1 = 0
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <math.h>
#include "I2C.h"
#include "MMA8452.h"
#include "Segment_Display.h"

volatile int8_t Digit[4]= {0};
volatile uint8_t ENABLE_READ = 0;
volatile uint8_t ENABLE_COPY = 0;
volatile uint8_t CHANGE_AXIS = 1;
volatile uint16_t LATCH = 0;
volatile uint8_t MODE = 0;

typedef struct Acceleration
{
	float Acceleration_X;
	float Acceleration_Y;
	float Acceleration_Z;
	uint8_t Acc_Minus;
} TAcceleration;

typedef struct Acceleration2
{
	int Total;
	int Fraction;
	int Sign;
} TAcceleration2;

uint8_t Offset[2] EEMEM;

void Read_Acceleration(TAcceleration*);
void Calculate_Degrees(TAcceleration*);
void Init();
void Timer8_Init();
void Timer16_Init();
void External_Interrupt_Init();
void Divide_Measurement(TAcceleration *Acc_XYZ, TAcceleration2 *Acc_M);
void Decompose_Number(volatile int8_t *Array, TAcceleration2 *Acc_M);
void Print(volatile int8_t *Digit);
void Calibrate(int16_t *);
void Quick_Print(int8_t, int8_t, int8_t, int8_t);

int main(void)
{
	_delay_ms(100);

	int16_t Calibration[2] = {0};
	uint8_t Block = 0;
	uint8_t Sample = 2; // Warunek nie pasujacy do zadnego kryterium
	TAcceleration Acceleration_XYZ = {0};
	TAcceleration2 Acceleration_M = {0};

	Init();

    while(1)
    {
    	if(ENABLE_READ && !MODE)
    	{
    		Read_Acceleration(&Acceleration_XYZ);
    		Calculate_Degrees(&Acceleration_XYZ);
    		Divide_Measurement(&Acceleration_XYZ, &Acceleration_M);
    		cli();
    		Decompose_Number(Digit, &Acceleration_M);
    		sei();
    		ENABLE_READ = 0;
    	}

		if(MODE && !Sample && !Block)
		{
			Read_Acceleration(&Acceleration_XYZ);
			if(CHANGE_AXIS) Calibration[0] = Acceleration_XYZ.Acceleration_Y;
			if(!CHANGE_AXIS) Calibration[0] = Acceleration_XYZ.Acceleration_X;

			Block = 1;
		}

		if(MODE && (Sample == 1) && !Block)
		{
			Read_Acceleration(&Acceleration_XYZ);
			if(CHANGE_AXIS) Calibration[1] = Acceleration_XYZ.Acceleration_Y;
			if(!CHANGE_AXIS) Calibration[1] = Acceleration_XYZ.Acceleration_X;

			Calibrate(Calibration);
			Calibration[0] = 0;
			Calibration[1] = 0;
			Sample = 2;
			MODE = 0;
			Block = 0;
		}

    	if(!(PINB & 0x02) && !LATCH) // Warunek wcisnietego przycisku
		{
    		START_TIMER16;
    		if(MODE) MODE++;
    		LATCH = 1; // Wymuszenie jednorazowego wykonania instrukcji flagowej
		}
    	else if((PINB & 0x02) && LATCH) // Warunek dla zwolnionego przycisku
		{
    		STOP_TIMER16;
    		CLEAR_TIMER16;

    		if(!MODE) // Jesli przycisk zostal krotko wcisniety
    		{
    			if(!(++LATCH)) // Oczekiwanie na przepelnienie zmiennej
				{
					PCMSK0 |= (1 << PCINT1); // Przywrocenie przerwan dla pinu 2
					CHANGE_AXIS ^= 1; // Zmiana osi pomiarowej
				}
    		}
    		else // Jesli przycisk zostal wcisniety na dluzej niz 2 sekundy
    		{
    			switch(MODE)
				{
    				case 1:
    					if(!(++LATCH)) // Oczekiwanie na przepelnienie zmiennej
						{
							PCMSK0 |= (1 << PCINT1); // Przywrocenie przerwan dla pinu 2
							Quick_Print(10, 12, 1, 10);
						}
    				break;

    				case 2:
    					if(!(++LATCH)) // Oczekiwanie na przepelnienie zmiennej
						{
							PCMSK0 |= (1 << PCINT1); // Przywrocenie przerwan dla pinu 2
							Sample = 0;
							Quick_Print(10, 12, 2, 10);
						}
    				break;

    				case 3:
    					if(!(++LATCH)) // Oczekiwanie na przepelnienie zmiennej
						{
							PCMSK0 |= (1 << PCINT1); // Przywrocenie przerwan dla pinu 2
							Sample = 1;
							Block = 0;
						}
    				break;
				}
    		}
		}
    }
    return 0;
}

void Init()
{
	uint8_t Byte;
	// Wejscia dla mma8452 oraz przycisku mode
	DDRB &= ~((1 << PB0) | (1 << PB1));
	PORTB |= (1 << PB0) | (1 << PB1);

	SD_init();
	I2C_Init();
	MMA8452_Init();
	Byte = eeprom_read_byte(&Offset[0]);
	MMA8452_Change_Offset_X((int8_t)Byte);
	Byte = eeprom_read_byte(&Offset[1]);
	MMA8452_Change_Offset_Y((int8_t)Byte);
	Timer8_Init();
	Timer16_Init();
	External_Interrupt_Init();
	sei();
}

void Timer8_Init()
{
	// Tryb pracy CTC - 250Hz
	TCCR0B |= (1 << WGM01);
	TCCR0B |= (1 << CS01) | (1 << CS00);
	OCR0B = 250;
	TIMSK0 |= (1 << OCIE0B);
}

void Timer16_Init()
{
	// Tryb pracy CTC - 0.5Hz
	TCCR1B |= (1 << WGM12);
	OCR1A = 7800;
	TIMSK1 |= (1 << OCIE1A);
}

void External_Interrupt_Init()
{
	// Przerwania zewnetrzne dla PCINT0, PCINT1
	PCICR |= (1 << PCIE0);
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1);
}

void Calibrate(int16_t *Calibration)
{
	if(Calibration[0] & 0x0800) Calibration[0] |= 0xF000;
	if(Calibration[1] & 0x0800) Calibration[1] |= 0xF000;

	Calibration[0] = Calibration[1] + Calibration[0];
	Calibration[0] /= 2;
	Calibration[0] *= -1;
	Calibration[0] /= 2;

	if(CHANGE_AXIS)
	{
		MMA8452_Change_Offset_Y((int8_t) Calibration[0]);
		eeprom_write_byte(&Offset[1], (uint8_t)Calibration[0]);
	}
	if(!CHANGE_AXIS)
	{
		MMA8452_Change_Offset_X((int8_t) Calibration[0]);
		eeprom_write_byte(&Offset[0], (uint8_t)Calibration[0]);
	}
}

void Read_Acceleration(TAcceleration *Acc_XYZ)
{
	uint8_t Array[6];
	MMA8452_Read_Multiple_Bytes(0x01, 6, Array);
	Acc_XYZ->Acceleration_X = ((uint16_t)Array[0]<<4) + ((uint16_t)Array[1]>>4);
	Acc_XYZ->Acceleration_Y = ((uint16_t)Array[2]<<4) + ((uint16_t)Array[3]>>4);
	Acc_XYZ->Acceleration_Z = ((uint16_t)Array[4]<<4) + ((uint16_t)Array[5]>>4);
}

void Calculate_Degrees(TAcceleration *Acc_XYZ)
{
	int16_t Temp = 0;

	if(CHANGE_AXIS == 0)
	{
		if(Acc_XYZ->Acceleration_X < 2048)
		{
			Acc_XYZ->Acceleration_X = asin(Acc_XYZ->Acceleration_X/1024.0)*(180.0/M_PI);
			Acc_XYZ->Acc_Minus = 0;
		}
		else
		{
			Temp = Acc_XYZ->Acceleration_X;
			Temp |= 0xF000;
			Acc_XYZ->Acceleration_X = Temp;

			Acc_XYZ->Acceleration_X = asin(Acc_XYZ->Acceleration_X/1024.0)*(-180.0/M_PI);
			Acc_XYZ->Acc_Minus = 1;
		}
	}

	if(CHANGE_AXIS == 1)
	{
		if(Acc_XYZ->Acceleration_Y < 2048)
		{
			Acc_XYZ->Acceleration_Y = asin(Acc_XYZ->Acceleration_Y/1024.0)*(180.0/M_PI);
			Acc_XYZ->Acc_Minus = 0;
		}
		else
		{
			Temp = Acc_XYZ->Acceleration_Y;
			Temp |= 0xF000;
			Acc_XYZ->Acceleration_Y = Temp;

			Acc_XYZ->Acceleration_Y = (asin(Acc_XYZ->Acceleration_Y/1024.0)*(-180.0/M_PI));
			Acc_XYZ->Acc_Minus = 1;
		}
	}
}

void Divide_Measurement(TAcceleration *Acc_XYZ, TAcceleration2 *Acc_M)
{
	if(CHANGE_AXIS == 0)
	{
		Acc_M->Total = Acc_XYZ->Acceleration_X;
		Acc_M->Fraction = (Acc_XYZ->Acceleration_X - Acc_M->Total)*10;
		Acc_M->Sign = Acc_XYZ->Acc_Minus;
	}
	else
	{
		Acc_M->Total = Acc_XYZ->Acceleration_Y;
		Acc_M->Fraction = (Acc_XYZ->Acceleration_Y - Acc_M->Total)*10;
		Acc_M->Sign = Acc_XYZ->Acc_Minus;
	}
}

void Decompose_Number(volatile int8_t *Array, TAcceleration2 *Acc_M)
{
	int8_t i;

	Array[3] = Acc_M->Fraction;

	for(i=2; i>0; Acc_M->Total/=10, i--)
	{
		Array[i] = Acc_M->Total % 10;
	}

	if(Acc_M->Sign == 1) Array[0] = 11;
	else Array[0] = 10;

	if(Array[1] == 0) Array[1] = 10;
}

void Print(volatile int8_t *Digit)
{
	static uint8_t i = 0;

	SD_Select_Display(i);

	if(i!=2) SD_Print_Number(Digit[i]);
	else SD_Print_Number_With_Dot(Digit[i]);

	i++;

	if(i >= 4 ) i = 0;
}

void Quick_Print(int8_t D1, int8_t D2, int8_t D3, int8_t D4)
{
	Digit[0] = D1;
	Digit[1] = D2;
	Digit[2] = D3;
	Digit[3] = D4;
}

ISR(PCINT0_vect)
{
	uint8_t EN = 1;

	if(!(PINB & 0x01)) // Sprawdzam czy przyszlo przerwanie od MMA8452Q
	{
		ENABLE_READ = 1; // Flaga zezwalajaca na pobranie pomiaru z czujnika
		EN = 0;
	}

	if(!(PINB & 0x02) && EN) // Sprawdzam czy zostal wcisniety przycisk na poziomicy
	{
		LATCH = 0; // Ustawiam zatrzask dla programowej eliminacji drgan stykow
		PCMSK0 &= ~(1 << PCINT1); // Wylaczam przerwanie dla pinu 2 od zmiany stanu
	}
}

ISR(TIMER0_COMPB_vect)
{
	Print(Digit); // Wyswietlenie pomiaru na poziomicy
}

ISR(TIMER1_COMPA_vect)
{
	MODE = 1; // Flaga zmieniajaca tryb: Praca normalna -> Kalibracja
	STOP_TIMER16;
	CLEAR_TIMER16;

	Quick_Print(11, 11, 11, 11);

	if(!CHANGE_AXIS) MMA8452_Change_Offset_X(0);
	if(CHANGE_AXIS) MMA8452_Change_Offset_Y(0);
}
