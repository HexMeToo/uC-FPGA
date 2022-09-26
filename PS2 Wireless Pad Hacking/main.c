/*
 * main.c
 *
 *  Created on: 11-02-2013
 *      Author: Mariusz
 */



#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

// Przypisujemy sygna³y z gamepada do portów we/wa AVRa
// DATA - wejœcie
#define DATA_DIR_IN DDRB &= ~(1<<4)
#define DATA_IN    (PINB&(1<<4))
// CMD - wyjœcie
#define CMD_SET PORTB |= (1<<3)
#define CMD_CLR PORTB &= ~(1<<3)
#define CMD_DIR_OUT DDRB |= (1<<3)
// ATT - wyjœcie
#define ATT_SET PORTB |= (1<<2)
#define ATT_CLR PORTB &= ~(1<<2)
#define ATT_DIR_OUT DDRB |= (1<<2)
// CLK - wyjœcie
#define CLK_SET PORTB |= (1<<5)
#define CLK_CLR PORTB &= ~(1<<5)
#define CLK_DIR_OUT DDRB |= (1<<5)

// Krotkie opóŸnienie
#define _NOP_ asm volatile("nop\n\t""nop\n\t" "nop\n\t" "nop\n\t" ::)

volatile unsigned char i=0;

void USART_Transmit(unsigned char data) // Definicja funkcji nadawczej
{
	while(!(UCSRA&(1<<UDRE))); // Czekaj a¿ bufor nadawczy bêdzie pusty
	UDR=data; // Wrzuæ dane do bufora nadawczego, start transmisji
}


unsigned char USART_Receive(void) // Definicja funkcji odbiornika
{
	while (!(UCSRA & (1<<RXC))); // Odczekanie na odbior danych
	return UDR; // Pobierz i zwroc dane z bufora
}

void USART_Init(uint32_t baud) // definicja funkcji inicjalizuj¹cej UART
{
	uint16_t _ubr = (F_CPU/16/baud-1); // wyliczenie UBRR dla trybu asynchronicznego (U2X=0)
	UBRRH = (uint8_t)(_ubr>>8); // ustawienie prêdkoœci
	UBRRL = (uint8_t) _ubr;
	UCSRB = (1<<RXEN)|(1<<TXEN)|(1<<RXCIE); // za³¹czenie nadajnika i odbiornika
	UCSRC = (1<<URSEL)|(3<<UCSZ0); // ustawienie formatu ramki: 8 bitów danych, 1 bit stopu
}



//---------------------------------------------------------------------------
// 9 bajtów danych odczytanych/zapisanyc z/do gamepada
unsigned char tab[9];

//---------------------------------------------------------------------------
// Konfiguruje porty we/wy i  sprzêtowy interfejs SPI
void init(void)
{
    // wejœcia - wybór  bajtu
#define  SWITCH (PINC)
    DDRC = 0x00;
    PORTC = 0x0f;

    CMD_DIR_OUT;
    ATT_DIR_OUT;
    CLK_DIR_OUT;

    ATT_SET;
    CLK_SET;
    CMD_SET;

	// Konfiguracja bitow SPI
    SPCR =(1<<SPE)|(1<<MSTR)|(1<<DORD)|(1<<CPOL)|(1<<CPHA)|(1<<SPR0)|(1<<SPR1);
}

//---------------------------------------------------------------------------
// Wysy³a/odczytuje jeden bajt do/z gamepada
// Funkcja korzysta ze sprzêtowego interfejsu SPI AVRa atmega
unsigned char pad_byte(unsigned char byte)
{
    SPDR = byte;
    while(!(SPSR & (1<<SPIF)));

    _delay_us(20);
    return SPDR;
}

//---------------------------------------------------------------------------
// Wysy³a/odbiera  do/z gamepada n bajtów
void pad_cmd(unsigned char t[],unsigned char n)
{
   unsigned char i;

   ATT_CLR;

   for(i=0; i<n; i++)
      t[i] = pad_byte(t[i]);

   ATT_SET;
   _delay_us(50);
}


//---------------------------------------------------------------------------
// Komendy konfiguracyjne gamepada
void pad_config(void)
{
   // Komenda 0x43 "Go into configuration mode"
   tab[0]= 0x01;
   tab[1]= 0x43;
   tab[2]= 0x00;
   tab[3]= 0x01;
   tab[4]= 0x00;
   pad_cmd(tab, 5);

   // Prze³¹czamy gamepada w tryb analogowy, aby mieæ
   // mo¿liwoœæ odczytywania d¿ojstików analogowych

   // Komenda 0x44 "Turn on analog mode"
   tab[0]= 0x01;
   tab[1]= 0x44;
   tab[2]= 0x00;
   tab[3]= 0x01;
   tab[4]= 0x03; //
   tab[5]= 0x00;
   tab[6]= 0x00;
   tab[7]= 0x00;
   tab[8]= 0x00;
   pad_cmd(tab, 9);

   // Komenda 0x43 "Exit config mode"
   tab[0] = 0x01;
   tab[1] = 0x43;
   tab[2] = 0x00;
   tab[3] = 0x00;
   tab[4] = 0x5A;
   tab[5] = 0x5A;
   tab[6] = 0x5A;
   tab[7] = 0x5A;
   tab[8] = 0x5A;
   pad_cmd(tab, 9);
}

void pobierz_pozycje()
{
	// Nag³ówek pakietu
	tab[0] = 0x01;
    tab[1] = 0x42; // komenda 0x42 - Controller poll
    tab[2] = 0x00;

    tab[3] = 0x00; // przyciski
    tab[4] = 0x00; // przyciski
    tab[5] = 0x00; // d¿ojstik prawy  lewo-prawo
    tab[6] = 0x00; // d¿ojstik prawy  góra-dó³
    tab[7] = 0x00; // d¿ojstik lewy   lewo-prawo
    tab[8] = 0x00; // d¿ojstik lewy   gora-dó³

    // Wysy³amy pakiet danych z komend¹ 0x42(Controller poll),
    // w rezultacie otrzymujemy 9 bajtów z informacj¹ o stanie
    // przycisków i d¿ojstików analogowych
    pad_cmd(tab, 9);
}

int main(void)
{
	USART_Init(50000);

	init();
	pad_config();

	sei();

	while(1)
	{
		if (i==1)
		{
			pobierz_pozycje();
			USART_Transmit(tab[5]);
			USART_Transmit(tab[6]);
			USART_Transmit(tab[7]);
			USART_Transmit(tab[8]);
			i=0;
		}
	}

	return 0;
}

ISR(USART_RXC_vect)
{
	if(0xff==UDR)
	{
		i=1;
	}
}
