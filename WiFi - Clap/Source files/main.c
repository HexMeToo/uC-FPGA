#define CLAP 2 // Ilosc klasniec potrzebna do zalaczenia / wylaczenia przekaznika
// Wystartowanie timera dla odmierzenia czasu potrzebnego na klasniecia
#define START_TIMER_0 TCCR1B |= (1 << CS12) | (1 << CS10)
// Zatrzymanie timera
#define STOP_TIMER_0 TCCR1B &= ~((1 << CS12) | (1 << CS10))
// Wyzerowanie timera w celu eliminacji pozostalosci z poprzednich pomiarow
#define CLEAR_TIMER_0 TCNT1 = 0

#include <avr/io.h> // Biblioteka wejsc / wyjsc
#include <avr/interrupt.h> // Biblioteka przerwan ukladu

volatile uint8_t clap_counter = 0; // Zmienna liczaca ilosc klasniec

void Init(void); // Funkcja inicjalizujaca peryferia i porty

void UART_Init(long int); // Inicjalizacja UART - Nieużywane w programie - Zarezerwowane na rozwój projektu

int main()
{
	uint8_t latch = 0; // Zatrzask

	Init(); // Initialize periphery
	sei(); // Enable interrupts

	while(1)
	{
//		if(!(PINB & 0x40) && !latch)
//		{
//			if(clap_counter == 0) START_TIMER_0;
//			if(clap_counter <= CLAP-1) clap_counter++;
//			latch = 1;
//		}
//		else if((PINB & 0x40) && latch) latch++;
//
//		if(clap_counter == CLAP)
//		{
//			clap_counter = 0;
//			STOP_TIMER_0;
//			CLEAR_TIMER_0;
//			PORTC ^= (1 << PC5);
//		}

		if((ADC > 1000) && !latch) // Sprawdzenie czy nastapilo klasniecie
		{
			if(clap_counter == 0) START_TIMER_0; // Jesli pierwsze klasniecie to wystartuj timer
			if(clap_counter <= CLAP-1) clap_counter++;
			latch = 1; // Zablokuj ponowne wejscie do warunku
		}
		else if((ADC < 1000) && latch) latch++; // Odczekanie do wartosci ustalonej klaskacza

		if(clap_counter == CLAP) // warunek osiagniecia wymaganej ilosci klasniec
		{
			clap_counter = 0; // warunek poczatkowy - reset zmiennej
			STOP_TIMER_0; // wylaczenie timera
			CLEAR_TIMER_0; // wyzerowanie pozostalosci
			PORTC ^= (1 << PC5); // Stan przeciwny diody sygnalizacyjnej
		}
	}

	return 0;
}

void Init(void)
{
	// Relay
	DDRD |= (1 << PD4);
	PORTD &= ~(1 << PD4);

	// Reset ESP8266
	DDRD &= ~(1 << PD3);
	PORTD |= (1 << PD3);

	// LED light
	DDRC |= (1 << PC5);
	PORTC &= ~(1 << PC5);

	// Signal from clapping module - without noise
	DDRB &= ~(1 << PB6);
	PORTB &= ~(1 << PB6);

	// Signal from clapping module - with noise
	DDRC &= ~(1 << PC0);
	PORTC &= ~(1 << PC0);

	// Signal from ESP8266
	DDRD &= ~(1 << PD2);
	PORTD &= ~(1 << PD2);

	// Configure Timer1
	TCCR1B |= (1 << WGM12); // Tryb CTC
	TIMSK |= (1 << OCIE1B); // Wlaczenie przerwan od CTC
	OCR1B = 2000; // Przerwanie po 2 sekundach od wlaczenia timera

	// UART PINs
	DDRD &= ~((1 << PD0) | (1 << PD1));
	//DDRD |= (1 << PD0) | (1 << PD1);
	PORTD |= (1 << PD0) | (1 << PD1);

	// Initialize USART
	//UART_Init(9600); // USART

	// External interrupts
	MCUCR |= (1 << ISC01) | (1 << ISC00); // Narastajace zbocze sygnalu
	GICR |= (1 << INT0); // Przerwanie zewnetrzne od INT0

	// ADC
	ADMUX |= (1 << REFS0); // Napiecie odniesienia od AVCC
	//ADMUX &= ~((1 << MUX3) | (1 << MUX2) | (1 << MUX1) | (1 << MUX0));
	//ADMUX &= ~(1 << ADLAR);
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADFR); // Zalaczenie ADC w trybie Free Runing
	ADCSRA |= (1 << ADPS1) | (1 << ADPS0); // Ustawienie czestotliwosci pomiaru na 125 kHz

	// Timer0
	TCCR2 |= (1 << WGM21) | (1 << CS22) | (1 << CS21) | (1 << CS20); // Tryb CTC, 1024 prescaler
	OCR2 = 100; // 100 ms - wywolanie przerwania
	TIMSK |= (1 << OCIE2); // wlaczenie przerwan od CTC dla Timera2
}

void UART_Init(long int baud)
{
	// Calculate UBRR for asynchronous mode (U2X = 0)
	uint16_t _ubr = (F_CPU/16/baud-1);
	// Set speed
	UBRRH = (uint8_t)(_ubr>>8);
	UBRRL = (uint8_t) _ubr;
	// Turn ON transmitter and receiver
	UCSRB = (1<<RXEN)|(1<<TXEN);
	// Frame: 8 data bits, 1 stop bit
	UCSRC = (1<<URSEL)|(3<<UCSZ0);
}

// Rising edge - external interrupt
ISR(INT0_vect)
{
	PORTC ^= (1 << PC5); // Stan przeciwny w odpowiedzi na zdarzenie od modulu WIFI
}

// Przerwanie kopiujace stan diody sygnalizacyjnej LED na przekaznik
ISR(TIMER2_COMP_vect)
{
	if(PORTC & (1 << PC5)) PORTD &= ~(1 << PD4); // Jesli dioda zapalona -> wlacz przekaznik
	else PORTD |= (1 << PD4); // Jesli nie to wylacz
}

// Przerwanie od osiagniecia maksimum czasu oczekiwania na klasniecie
ISR(TIMER1_COMPB_vect)
{
	clap_counter = 0; // warunek poczatkowy - reset zmiennej
	STOP_TIMER_0; // wylaczenie timera
	CLEAR_TIMER_0; // wyzerowanie pozostalosci
}
