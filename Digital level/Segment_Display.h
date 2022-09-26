/*
 * Segment_Dispaly.h
 *
 *  Created on: 12 lis 2014
 *      Author: Mariusz
 */

#ifndef SEGMENT_DISPLAY_H_
#define SEGMENT_DISPLAY_H_

// Wybór portów do poszczególnych funkcji
#define PORT_LED PORTD
#define DDR_LED DDRD
#define PORT_DISPLAY PORTC
#define DDR_DISPLAY DDRC

// Tablica segmentow
#define SEG_A (1 << 7)
#define SEG_C (1 << 1)
#define SEG_B (1 << 6)
#define SEG_D (1 << 2)
#define SEG_E (1 << 0)
#define SEG_F (1 << 5)
#define SEG_G (1 << 3)
#define SEG_DOT (1 << 4)

// Tablica cyfr
#define ZERO 	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F
#define ONE 	SEG_B|SEG_C
#define TWO 	SEG_A|SEG_B|SEG_G|SEG_E|SEG_D
#define THREE 	SEG_A|SEG_B|SEG_C|SEG_D|SEG_G
#define FOUR 	SEG_B|SEG_C|SEG_F|SEG_G
#define FIVE 	SEG_A|SEG_F|SEG_G|SEG_C|SEG_D
#define SIX 	SEG_A|SEG_F|SEG_G|SEG_C|SEG_D|SEG_E
#define SEVEN 	SEG_A|SEG_B|SEG_C
#define EIGHT 	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G
#define NINE 	SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G

// Tablica cyfr z kropk¹
#define ZERO_D 		SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_DOT
#define ONE_D 		SEG_B|SEG_C|SEG_DOT
#define TWO_D 		SEG_A|SEG_B|SEG_G|SEG_E|SEG_D|SEG_DOT
#define THREE_D 	SEG_A|SEG_B|SEG_C|SEG_D|SEG_G|SEG_DOT
#define FOUR_D 		SEG_B|SEG_C|SEG_F|SEG_G|SEG_DOT
#define FIVE_D 		SEG_A|SEG_F|SEG_G|SEG_C|SEG_D|SEG_DOT
#define SIX_D 		SEG_A|SEG_F|SEG_G|SEG_C|SEG_D|SEG_E|SEG_DOT
#define SEVEN_D 	SEG_A|SEG_B|SEG_C|SEG_DOT
#define EIGHT_D 	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_DOT
#define NINE_D 		SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G|SEG_DOT

// Tablica znaków
#define ALL 	SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G|SEG_DOT
#define DOT		SEG_DOT
#define NOTHING 0x00
#define MINUS 	SEG_G
#define LITERA_P SEG_A|SEG_B|SEG_E|SEG_F|SEG_G

void SD_init();
void SD_Print_Number(uint8_t number);
void SD_Print_Number_With_Dot(uint8_t number);
void SD_Select_Display(uint8_t display);

#endif /* SEGMENT_DISPLAY_H_ */
