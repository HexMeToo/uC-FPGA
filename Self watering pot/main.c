// Definicje konfiguracyjne
// Jednostka miary -> 1V == 204.8 -> np. 1.3V -> 1.3 * 204.8 = 266.24 = 266
#define MOISTURE_LEVEL 696 		// Prog wilgotnosci zalaczajacy pompe
#define NIGHT_LEVEL 980 		// Prog jasnosci okreslajacy noc
#define MORNING_LEVEL 930 		// Prog jasnosci okreslajacy dzien

// Jednostka miary - sekundy
#define SMALL_PLANT_TIME 2 		// Czas nawadniania malej rosliny
#define MEDIUM_PLANT_TIME 4 	// Czas nawadniania sredniej rosliny
#define LARGE_PLANT_TIME 6 	// Czas nawadniania duzej rosliny

// Definicje sterowania wyjsciami
#define PUMP_ON PORTC |= (1 << PC3) 				// Instrukcja wlaczajaca pompe
#define PUMP_OFF PORTC &= ~(1 << PC3) 				// Instrukcja wylaczajaca pompe
#define MORNING_LED_ON PORTD |= (1 << PD7)  		// Instrukcja wlaczajaca diode dzienna
#define MORNING_LED_OFF PORTD &= ~(1 << PD7) 		// Instrukcja wylaczajaca diode dzienna
#define NIGHT_LED_ON PORTD |= (1 << PD6) 			// Instrukcja wlaczajaca diode nocna
#define NIGHT_LED_OFF PORTD &= ~(1 << PD6) 			// Instrukcja wylaczajaca diode nocna
#define SMALL_PLANT_LED_ON PORTD |= (1 << PD5) 		// Instrukcja wlaczajaca diode malej rosliny
#define SMALL_PLANT_LED_OFF PORTD &= ~(1 << PD5) 	// Instrukcja wylaczajaca diode malej rosliny
#define MEDIUM_PLANT_LED_ON PORTD |= (1 << PD3) 	// Instrukcja wlaczajaca diode sredniej rosliny
#define MEDIUM_PLANT_LED_OFF PORTD &= ~(1 << PD3) 	// Instrukcja wylaczajaca diode sredniej rosliny
#define LARGE_PLANT_LED_ON PORTD |= (1 << PD2) 		// Instrukcja wlaczajaca diode duzej rosliny
#define LARGE_PLANT_LED_OFF PORTD &= ~(1 << PD2) 	// Instrukcja wylaczajaca diode duzej rosliny

// Definicje steruj¹ce interfejsem sprzetowym
#define CHOOSE_LIGHT_SENSOR_INPUT ADMUX &= ~(1 << MUX0)		// Instrukcja wybierajaca wejscie czujnika swiatla
#define CHOOSE_MOISTURE_SENSOR_INPUT ADMUX |= (1 << MUX0)	// Instrukcja wybierajaca wejscie czujnika wilgotnosci gleby
#define START_ADC_MEASUREMENT ADCSRA |= (1 << ADSC)			// Uruchomienie pomiaru ADC

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint16_t ADC_Light = 0;
volatile uint16_t ADC_Moisture = 0;
volatile uint8_t ADC_MODE = 0;

int main(void)
{
	uint8_t Button_Lock_B1 = 0;
	uint8_t Button_Lock_B2 = 0;
	uint8_t Plant = 1;
	uint16_t Day_Time = 0;
	uint16_t i;

	// Wejscie ADC
	DDRC &= ~(1 << PC0); // Czujnik wilgotnosci
	DDRC &= ~(1 << PC1); // Czujnik natezetnia swiatla

	// Inicjalizacja ADC
	ADMUX |= (1 << REFS0);
	ADCSRA |= (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	// Konfiguracja wyjscia sterujacego pomp¹
	DDRC |= (1 << PC3);
	PORTC &= ~(1 << PC3);

	// Konfiguracja przycisków steruj¹cych
	DDRD &= ~((1 << PD0) | (1 << PD1));
	PORTD |= ((1 << PD0) | (1 << PD1));

	// Konfiguracja diod sygnalizacyjnych
	DDRD |= (1 << PD2) | (1 << PD3) | (1 << PD5) | (1 << PD6) | (1 << PD7);
	PORTD &= ~((1 << PD2) | (1 << PD3) | (1 << PD5) | (1 << PD6) | (1 << PD7));

	TCCR1B |= (1 << WGM12) | (1 << CS12) | (1 << CS10);
	OCR1A = 500;
	TIMSK |= (1 << OCIE1A);

	sei();

	while(1)
	{
		if(!(PIND & (1 << PD1)) && Button_Lock_B1 == 0)
		{
			Plant++;
			if(Plant >= 4) Plant = 1;
			Button_Lock_B1 = 1;
		}
		else if((PIND & (1 << PD1)) && Button_Lock_B1!=0) Button_Lock_B1++;

		if(!(PIND & (1 << PD0)) && Button_Lock_B2 == 0)
		{
			if(Day_Time == 0) Day_Time = 1;
			else Day_Time = 0;
			Button_Lock_B2 = 1;
		}
		else if((PIND & (1 << PD0)) && Button_Lock_B2!=0) Button_Lock_B2++;

		switch(Plant)
		{
			// Mala roslina
			case 1:
				SMALL_PLANT_LED_ON;
				MEDIUM_PLANT_LED_OFF;
				LARGE_PLANT_LED_OFF;
			break;

			// Srednia roslina
			case 2:
				SMALL_PLANT_LED_OFF;
				MEDIUM_PLANT_LED_ON;
				LARGE_PLANT_LED_OFF;
			break;

			// Duza roslina
			case 3:
				SMALL_PLANT_LED_OFF;
				MEDIUM_PLANT_LED_OFF;
				LARGE_PLANT_LED_ON;
			break;
		}

		switch(Day_Time)
		{
			// Dzien
			case 0:
				MORNING_LED_ON;
				NIGHT_LED_OFF;
			break;

			// Noc
			case 1:
				MORNING_LED_OFF;
				NIGHT_LED_ON;
			break;
		}

		if(ADC_Moisture < MOISTURE_LEVEL)
		{
			if((ADC_Light > NIGHT_LEVEL && Day_Time == 1) || (ADC_Light < MORNING_LEVEL && Day_Time == 0))
			{
				switch(Plant)
				{
					case 1:
						PUMP_ON;
						for(i = 0; i < SMALL_PLANT_TIME; i++) _delay_ms(1000);
						PUMP_OFF;
					break;

					case 2:
						PUMP_ON;
						for(i = 0; i < MEDIUM_PLANT_TIME; i++) _delay_ms(1000);
						PUMP_OFF;
					break;

					case 3:
						PUMP_ON;
						for(i = 0; i < LARGE_PLANT_TIME; i++) _delay_ms(1000);
						PUMP_OFF;
					break;
				}
			}
		}
	}
}

ISR(TIMER1_COMPA_vect)
{
	if(ADC_MODE == 0)
	{
		CHOOSE_LIGHT_SENSOR_INPUT;
		START_ADC_MEASUREMENT;
	}
	else
	{
		CHOOSE_MOISTURE_SENSOR_INPUT;
		START_ADC_MEASUREMENT;
	}

	if(ADC_MODE == 0) ADC_MODE = 1;
	else ADC_MODE = 0;
}

ISR(ADC_vect)
{
	if(ADC_MODE == 0) ADC_Light = ADC;
	else ADC_Moisture = ADC;
}
