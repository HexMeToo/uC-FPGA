#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

#define DATA_SET		PORTB |=  (1<<PB1);
#define DATA_CLR		PORTB &= ~(1<<PB1);
#define CLK_SET			PORTB |=  (1<<PB2);
#define CLK_CLR			PORTB &= ~(1<<PB2);
#define LE_SET			PORTB |=  (1 << PB0);
#define LE_CLR			PORTB &= ~(1 << PB0);
#define NOP asm volatile("nop\n\t""nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" "nop\n\t" ::);

#define THREE_STATE_OUTPUT 					0x00
#define DVDD 								0x01
#define DGND 								0x02
#define R_DIVIDER_OUTPUT 					0x03
#define N_DIVIDER_OUTPUT 					0x04
#define ANALOG_LOCK_DETECT 					0x05
#define DIGITAL_LOCK_DETECT 				0x06
#define SERIAL_DATA_OUTPUT 					0x07
#define CLOCK_DIVIDER 						0x0A
#define FAST_LOCK_SWITCH 					0x0C
#define R_DIVIDER_2 						0x0D
#define N_DIVIDER_2 						0x0E

#define CYCLE_SLIP_REDUCTION_DIS 			0x00
#define CYCLE_SLIP_REDUCTION_EN 			0x01
#define LOW_NOISE_MODE 						0x00
#define LOW_SPUR_MODE 						0x03
#define REFERENCE_DOUBLER_DIS 				0x00
#define REFERENCE_DOUBLER_EN 				0x01
#define R_DIVIDER_DIS 						0x00
#define R_DIVIDER_EN 						0x01
#define PRESCALER_4_5 						0x00
#define PRESCALER_8_9 						0x01

#define I_CP_031 							0x00
#define I_CP_063 							0x01
#define I_CP_094 							0x02
#define I_CP_125 							0x03
#define I_CP_157 							0x04
#define I_CP_188 							0x05
#define I_CP_219 							0x06
#define I_CP_250 							0x07
#define I_CP_281 							0x08
#define I_CP_313 							0x09
#define I_CP_344 							0x0A
#define I_CP_375						 	0x0B
#define I_CP_406 							0x0C
#define I_CP_438 							0x0D
#define I_CP_469 							0x0E
#define I_CP_500 							0x0F

#define SIG_DEL_RESET_EN 					0x00
#define SIG_DEL_RESET_DIS 					0x01
#define LDP_10n 							0x00
#define LDP_6n 								0x01
#define PD_POLARITY_NEGATIVE 				0x00
#define PD_POLARITY_POSITIVE				0x01
#define COUNTER_RESET_DIS 					0x00
#define COUNTER_RESET_EN 					0x01
#define CP_THREE_STATE_DIS 					0x00
#define CP_THREE_STATE_EN 					0x01
#define POWER_DOWN_DIS 						0x00
#define POWER_DOWN_EN 						0x01
#define CLK_DIV_MODE_CLK_DIV_OFF 			0x00
#define CLK_DIV_MODE_FAST_LOCK_MODE 		0x01
#define CLK_DIV_MODE_RESYNC_TIMER_ENABLED 	0x02

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <math.h>

void Init(void);
void SPI_Init(void);
void SPI_Send(uint8_t send);
uint16_t Calculate_Frequency(void);
void UART_Init(unsigned int ubrr);

void Load_Frac_Int_Register(uint8_t Muxout_Control, uint16_t Integer_Value, uint16_t Fractional_Value);
void Load_Phase_Register(uint16_t Phase_Value);
void Load_Mod_R_Register(uint8_t Noise_Mode, uint8_t CSR_EN, uint8_t Current_Setting, uint8_t Prescaler, uint8_t RDIV_2_1, uint8_t Reference_Doubler, uint8_t Counter, uint16_t Modulus);
void Load_Function_Register(uint8_t Sigma_Reset, uint8_t LDP, uint8_t PD_Polarity, uint8_t PD, uint8_t CP_Three_State, uint8_t Counter_Reset);
void Load_CLK_DIV_Register(uint8_t CLK_DIV_Mode, uint8_t Clock_Divider_Value);
void Init_ADF4156(void);
void Load_Register(void);
void SPI_Software_Init(void);
void SPI_Software_Send(uint32_t send);

volatile uint8_t counter = 0;
volatile uint8_t received_data[6] = {'c', 'a', '0', '0', '0', '0'};
volatile uint8_t latch = 1;


int main (void)
{
	uint16_t buffer;

	Init();

	while(1)
	{
		if(latch == 0)
		{
			latch = 1;
			buffer = Calculate_Frequency();

			_delay_ms(1);
			PORTA &=~(1 << PA0);								//activate slave
			SPI_Send((buffer >> 8) & 0x0F);
			SPI_Send(buffer);
			PORTA &=~(1 << PA2);								//activate slave's load
			_delay_ms(1);
			PORTA |= (1<< PA2);									//deactivate slave's load
			PORTA |= (1<< PA0);									//deactivate slave

			_delay_ms(1);
			PORTA &=~(1 << PA7);
			SPI_Send((buffer >> 8) & 0x0F);
			SPI_Send(buffer);
			PORTA &=~(1 << PA5);
			_delay_ms(1);
			PORTA |= (1<< PA5);
			PORTA |= (1<< PA7);
		}
	}
}

void SPI_Software_Init()
{
	DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2);              //MOSI, SCK, SS' Output
}

void SPI_Software_Send(uint32_t data)
{
	int8_t m;

	for(m = 31; m >= 0; m--)
	{
		if(data & (1 << m)) DATA_SET
		else DATA_CLR

		CLK_SET
		NOP
		CLK_CLR
	}
}

void Load_Register(void)
{
	LE_SET
	NOP
	LE_CLR
}

void Load_Frac_Int_Register(uint8_t Muxout_Control, uint16_t Integer_Value, uint16_t Fractional_Value)
{
	uint32_t Register_Data = 0;

	Register_Data |= ((uint32_t)Muxout_Control) << 27;
	Register_Data |= ((uint32_t)Integer_Value) << 15;
	Register_Data |= ((uint32_t)Fractional_Value) << 3;

	SPI_Software_Send(Register_Data);
	Load_Register();
}

void Load_Phase_Register(uint16_t Phase_Value)
{
	uint32_t Register_Data = 0;

	Register_Data |= ((uint32_t)Phase_Value) << 3;
	Register_Data |= 0x00000001;

	SPI_Software_Send(Register_Data);
	Load_Register();
}

void Load_Mod_R_Register(uint8_t Noise_Mode, uint8_t CSR_EN, uint8_t Current_Setting, uint8_t Prescaler, uint8_t RDIV_2_1, uint8_t Reference_Doubler, uint8_t Counter, uint16_t Modulus)
{
	uint32_t Register_Data = 0;

	Register_Data |= ((uint32_t)Noise_Mode) << 29;
	Register_Data |= ((uint32_t)CSR_EN) << 28;
	Register_Data |= ((uint32_t)Current_Setting) << 24;
	Register_Data |= ((uint32_t)Prescaler) << 22;
	Register_Data |= ((uint32_t)RDIV_2_1) << 21;
	Register_Data |= ((uint32_t)Reference_Doubler) << 20;
	Register_Data |= ((uint32_t)Counter) << 15;
	Register_Data |= ((uint32_t)Modulus) << 3;
	Register_Data |= 0x00000002;

	SPI_Software_Send(Register_Data);
	Load_Register();
}

void Load_Function_Register(uint8_t Sigma_Reset, uint8_t LDP, uint8_t PD_Polarity, uint8_t PD, uint8_t CP_Three_State, uint8_t Counter_Reset)
{
	uint32_t Register_Data = 0;

	Register_Data |= ((uint32_t)Sigma_Reset) << 14;
	Register_Data |= ((uint32_t)LDP) << 7;
	Register_Data |= ((uint32_t)PD_Polarity) << 6;
	Register_Data |= ((uint32_t)PD) << 5;
	Register_Data |= ((uint32_t)CP_Three_State) << 4;
	Register_Data |= ((uint32_t)Counter_Reset) << 3;
	Register_Data |= 0x00000003;

	SPI_Software_Send(Register_Data);
	Load_Register();
}

void Load_CLK_DIV_Register(uint8_t CLK_DIV_Mode, uint8_t Clock_Divider_Value)
{
	uint32_t Register_Data = 0;

	Register_Data |= ((uint32_t)CLK_DIV_Mode) << 19;
	Register_Data |= ((uint32_t)Clock_Divider_Value) << 7;
	Register_Data |= 0x00000004;

	SPI_Software_Send(Register_Data);
	Load_Register();
}

void Init_ADF4156(void)
{
	Load_Frac_Int_Register(ANALOG_LOCK_DETECT, 1000, 250);
	Load_Phase_Register(1);
	Load_Mod_R_Register(LOW_NOISE_MODE, CYCLE_SLIP_REDUCTION_EN, I_CP_250, PRESCALER_4_5, R_DIVIDER_EN, REFERENCE_DOUBLER_DIS, 0, 0);
	Load_Function_Register(SIG_DEL_RESET_EN, LDP_10n, PD_POLARITY_POSITIVE, POWER_DOWN_DIS, CP_THREE_STATE_DIS, COUNTER_RESET_DIS);
	Load_CLK_DIV_Register(CLK_DIV_MODE_CLK_DIV_OFF, 0);
}

void Init()
{
	//output for SPI: PA0 is Weak, PA7 is Strong
	// Port do aktywacji i dezaktywacji DAC
	// oraz sygna³ ³aduj¹cy do rejestrów
	DDRA |= 0xFF;
	PORTA |= 0xFF;

	SPI_Init();
	UART_Init(MYUBRR);
	SPI_Software_Init();
	Init_ADF4156();

	sei();
}

void UART_Init(unsigned int ubrr)
{
	/*Set baud rate */
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	UCSRB = (1<<RXEN)|(1<<RXCIE);	//Enable receiver and transmitter
	UCSRC = (1<<URSEL)|(3<<UCSZ0);	 //Set frame format: 8data, 1stop bit
}

uint16_t Calculate_Frequency(void)
{
	uint16_t buffer = 0;
	float voltage = 0;

	buffer += (received_data[2] - 48) * 1000;
	buffer += (received_data[3] - 48) * 100;
	buffer += (received_data[4] - 48) * 10;
	buffer += received_data[5] - 48;

//	y = 11,236x + 146,07     R=9,16 Ohm   y to czestotliwosc   x to prad

//	R=U/I => U=I*R  x=(y-146,07)/11,236   U=[(y-146,07)/11,236]*9,16
	if(buffer >= 1000 && buffer <= 1200)
	{
		voltage = (((float)buffer - 146.07)/11.236) * 9.16;
	}

//	y = 12,987x - 18,182	   x=(y+18,182)/12,987		U=[(y+18,182)/12,987]*9,16
	if(buffer > 1200 && buffer <= 1400)
	{
		voltage = (((float)buffer + 18.182)/12.987) * 9.16;
	}

//	y = 11,494x + 144,83	   x=(y-144,83)/11,494	    U=[(y-144,83)/11,494]*9,16
	if(buffer > 1400 && buffer <= 1600)
	{

		voltage = (((float)buffer - 144.83)/11.494) * 9.16;
	}

//	y = 13,072x - 54,902	   x=(y+54,902)/13,072		U=[(y+54,902)/13,072]*9,16
	if(buffer > 1600 && buffer <= 1800)
	{
		voltage = (((float)buffer + 54.902)/13.072) * 9.16;
	}

//	y = 9,7087x + 422,33     x=(y-422,33)/9,7087		U=[(y-422,33)/9,7087]*9,16
	if(buffer > 1800 && buffer <= 2000)
	{
		voltage = (((float)buffer - 422.33)/9.7087) * 9.16;
	}

	return voltage;
}

void SPI_Init()
{
	DDRB |= ( 1 << PB4 ) | ( 1 << PB5 ) | ( 1 << PB7 );              //MOSI, SCK, SS' jako wyjœcia
	SPCR |= ( 1 << SPE ) | ( 1 << MSTR ) | ( 1 << SPR1 ) | ( 1 << SPR0 );
}

void SPI_Send(uint8_t send)
{
	SPDR = send;
	while (!(SPSR & (1<<SPIF)));
}

ISR(USART_RXC_vect)
{
	received_data[counter] = UDR;

	if(counter>=5)
	{
		counter = 0;
		latch = 0;
	}
	else counter++;
}
