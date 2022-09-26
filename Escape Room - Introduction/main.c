#define DIMM_TIME 100
#define LED_ON(x) (1 << x)
#define LED_OFF(x) ~(1 << x)
#define PICTURE_1_DELAY 500
#define PICTURE_2_DELAY 500
#define PICTURE_3_DELAY 500
#define PICTURE_4_DELAY 500
#define PICTURE_5_DELAY 500
#define PICTURE_1 0
#define PICTURE_2 1
#define PICTURE_3 2
#define PICTURE_4 3
#define PICTURE_5 4
#define START_BUTTON 0x01
#define STOP_BUTTON 0x02
#define LAMP_RELAY_ON PORTB |= 1 << PD5;
#define LAMP_RELAY_OFF PORTB &= ~(1 << PD5)
#define DEBOUNCE_TIME 100
#define BAUD 9600
#define MYUBBR ((F_CPU / (BAUD * 16L)) - 1)

#include <avr/io.h>
#include <util/delay.h>

void Picture_Pulse(uint8_t no_picture);
void Init(void);
void Init_UART(void);
void Send_UART(uint8_t c);
uint8_t Receive_UART(void);

int main()
{
	Init();

	while(1)
	{
		// Wait for start button
		while(PIND & START_BUTTON);
		_delay_ms(DEBOUNCE_TIME);

		// Start track


		// Star Dimming
		Picture_Pulse(PICTURE_1);
		Picture_Pulse(PICTURE_2);
		Picture_Pulse(PICTURE_3);
		Picture_Pulse(PICTURE_4);
		Picture_Pulse(PICTURE_5);

		LAMP_RELAY_ON;

		// Stop track

		// Wait for reset button
		while(PIND & STOP_BUTTON);
		_delay_ms(DEBOUNCE_TIME);

		LAMP_RELAY_OFF;
	}
}

void Picture_Pulse(uint8_t no_picture)
{
	uint16_t i, j;

	for(i=1; i < DIMM_TIME; i++)
	{
		PORTB = LED_ON(no_picture);
		for(j = 0; j < i; j++) _delay_ms(1);
		PORTB = LED_OFF(no_picture);
		for(j = 0; j < DIMM_TIME - i; j++) _delay_ms(1);
	}

	PORTB = LED_ON(no_picture);
	switch(no_picture)
	{
		case PICTURE_1:
			_delay_ms(PICTURE_1_DELAY);
		break;

		case PICTURE_2:
			_delay_ms(PICTURE_2_DELAY);
		break;

		case PICTURE_3:
			_delay_ms(PICTURE_3_DELAY);
		break;

		case PICTURE_4:
			_delay_ms(PICTURE_4_DELAY);
		break;

		case PICTURE_5:
			_delay_ms(PICTURE_5_DELAY);
		break;
	}

	for(i=0; i < DIMM_TIME; i++)
	{
		PORTB = LED_ON(no_picture);
		for(j = 0; j < DIMM_TIME - i; j++) _delay_ms(1);
		PORTB = LED_OFF(no_picture);
		for(j = 0; j < i; j++) _delay_ms(1);
	}
}

void Init(void)
{
	// Output signal for pictures
	DDRB = 0x0F;
	PORTB = 0x00;

	// Input signal from steering buttons
	DDRD = 0x00;
	PORTD = 0x0C;

	// Output signal for LED lamp
	DDRD |= 0x20;
	PORTD &= ~(1 << PD5);

	// UART Config
	Init_UART();
}

void Init_UART(void)
{
  // set baud rate
  UBRRH = (uint8_t)(MYUBBR >> 8);
  UBRRL = (uint8_t)(MYUBBR);
  // enable receive and transmit
  UCSRB = (1 << RXEN) | (1 << TXEN) | (1 << RXCIE);
  // set frame format
  UCSRC = (1 << USBS) | (3 << UCSZ0);	// asynchron 8n1
}

void Send_UART(uint8_t c)
{
  while(!(UCSRA & (1<<UDRE)));
  UDR = c;
}

uint8_t Receive_UART()
{
  while(!(UCSRA & (1<<RXC)));

  return UDR;
}
