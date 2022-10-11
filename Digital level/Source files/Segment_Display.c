#include <avr/io.h>
#include "Segment_Display.h"

void SD_init()
{
	// Wyjscia na segmenty
	DDR_LED = 0xFF;
	PORT_LED = 0xFF;

	// Wyjscia wyboru wyswietlacza
	DDR_DISPLAY = 0x0F;
	PORT_DISPLAY = 0x00;

	// Timer przelaczajacy wyswietlacze
}

void SD_Print_Number(uint8_t number)
{
	switch(number)
	{
		case 0:
			PORT_LED = ZERO;
		break;

		case 1:
			PORT_LED = ONE;
		break;

		case 2:
			PORT_LED = TWO;
		break;

		case 3:
			PORT_LED = THREE;
		break;

		case 4:
			PORT_LED = FOUR;
		break;

		case 5:
			PORT_LED = FIVE;
		break;

		case 6:
			PORT_LED = SIX;
		break;

		case 7:
			PORT_LED = SEVEN;
		break;

		case 8:
			PORT_LED = EIGHT;
		break;

		case 9:
			PORT_LED = NINE;
		break;

		case 10:
			PORT_LED = NOTHING;
		break;

		case 11:
			PORT_LED = MINUS;
		break;

		case 12:
			PORT_LED = LITERA_P;
		break;
	}
}

void SD_Select_Display(uint8_t display)
{
	switch(display)
	{
		case 0:
			PORT_DISPLAY |= 0x01;
			PORT_DISPLAY &= 0xF1;
			break;

		case 1:
			PORT_DISPLAY |= 0x02;
			PORT_DISPLAY &= 0xF2;
			break;

		case 2:
			PORT_DISPLAY |= 0x04;
			PORT_DISPLAY &= 0xF4;
			break;

		case 3:
			PORT_DISPLAY |= 0x08;
			PORT_DISPLAY &= 0xF8;
			break;
	}
}

void SD_Print_Number_With_Dot(uint8_t number)
{
	switch(number)
	{
		case 0:
			PORT_LED = ZERO_D;
		break;

		case 1:
			PORT_LED = ONE_D;
		break;

		case 2:
			PORT_LED = TWO_D;
		break;

		case 3:
			PORT_LED = THREE_D;
		break;

		case 4:
			PORT_LED = FOUR_D;
		break;

		case 5:
			PORT_LED = FIVE_D;
		break;

		case 6:
			PORT_LED = SIX_D;
		break;

		case 7:
			PORT_LED = SEVEN_D;
		break;

		case 8:
			PORT_LED = EIGHT_D;
		break;

		case 9:
			PORT_LED = NINE_D;
		break;

		case 11:
			PORT_LED = MINUS;
		break;

		case 12:
			PORT_LED = LITERA_P;
		break;
	}
}
