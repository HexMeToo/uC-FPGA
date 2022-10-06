#define BAUD 9600
#define MYUBRR ((F_CPU / (BAUD * 16L)) - 1)
#define del 20

#include <avr/io.h>
#include <util/delay.h>

uint32_t RotR(uint32_t Data, uint8_t Rot_quantity);
uint32_t ShR(uint32_t Data, uint8_t Shift_quantity);
uint32_t Ro_0(uint32_t Data);
uint32_t Ro_1(uint32_t Data);
uint32_t Sigma_0(uint32_t Data);
uint32_t Sigma_1(uint32_t Data);
uint32_t Ch(uint32_t X, uint32_t Y, uint32_t Z);
uint32_t Maj(uint32_t X, uint32_t Y, uint32_t Z);
void UART_Transmit(uint8_t data);
void UART_Init(void);
void Convert_data_to_ASCII(uint8_t data);

int main(void)
{

	DDRD |= (1 << PD1);
	UART_Init();

	uint32_t H [8] = {	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

	const uint32_t K[64] = {	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
								0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
								0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
								0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
								0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
								0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
								0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
								0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

	uint32_t a, b, c, d, e, f, g, h;
	uint32_t T1, T2;
	uint32_t W[64] = {0};
	uint8_t i;

	W[0] = 1633837952;
	W[15] = 24;

	while(1)
	{
		for(i=16; i<64; i++)
		{
			W[i] = Ro_1(W[i-2]) + W[i-7] + Ro_0(W[i-15]) + W[i-16];
		}

		a = H[0];
		b = H[1];
		c = H[2];
		d = H[3];
		e = H[4];
		f = H[5];
		g = H[6];
		h = H[7];

		for(i=0; i<64; i++)
		{
			T1 = h + Sigma_1(e) + Ch(e, f, g) + K[i] + W[i];
			T2 = Sigma_0(a) + Maj(a, b, c);
			h = g;
			g = f;
			f = e;
			e = d + T1;
			d = c;
			c = b;
			b = a;
			a = T1 + T2;
		}

		H[0] += a;
		H[1] += b;
		H[2] += c;
		H[3] += d;
		H[4] += e;
		H[5] += f;
		H[6] += g;
		H[7] += h;

		for(i=0; i<8; i++)
		{
			Convert_data_to_ASCII((H[i] & 0xFF000000) >> 24);
			Convert_data_to_ASCII((H[i] & 0x00FF0000) >> 16);
			Convert_data_to_ASCII((H[i] & 0x0000FF00) >> 8);
			Convert_data_to_ASCII((H[i] & 0x000000FF) >> 0);
		}
		UART_Transmit(13);

		_delay_ms(2000);
	}
}

void Convert_data_to_ASCII(uint8_t data)
{
	uint8_t bufor;

	bufor = (data & 0xF0) >> 4;
	if(bufor >= 10) UART_Transmit(bufor + 55);
	else UART_Transmit(bufor + 48);
	bufor = (data & 0x0F) >> 0;
	if(bufor >= 10) UART_Transmit(bufor + 55);
	else UART_Transmit(bufor + 48);
}

uint32_t RotR(uint32_t Data, uint8_t Rot_quantity)
{
	uint32_t bit;
	uint8_t i;

	for(i=0; i<Rot_quantity; i++)
	{
		bit = (Data & 0x01) << 31;
		Data >>= 1;
		Data += bit;
	}

	return Data;
}

uint32_t ShR(uint32_t Data, uint8_t Shift_quantity)
{
	return Data >>= Shift_quantity;
}

uint32_t Ro_0(uint32_t Data)
{
	return RotR(Data, 7) ^ RotR(Data, 18) ^ ShR(Data, 3);
}

uint32_t Ro_1(uint32_t Data)
{
	return RotR(Data, 17) ^ RotR(Data, 19) ^ ShR(Data, 10);
}

uint32_t Sigma_0(uint32_t Data)
{
	return RotR(Data, 2) ^ RotR(Data, 13) ^ RotR(Data, 22);
}
uint32_t Sigma_1(uint32_t Data)
{
	return RotR(Data, 6) ^ RotR(Data, 11) ^ RotR(Data, 25);
}

uint32_t Ch(uint32_t X, uint32_t Y, uint32_t Z)
{
	return ((X & Y) ^ ((X ^ 0xFFFFFFFF) & Z));
}
uint32_t Maj(uint32_t X, uint32_t Y, uint32_t Z)
{
	return ((X & Y) ^ (X & Z) ^ (Y & Z));
}

void UART_Transmit(uint8_t data)
{
	while(!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void UART_Init(void)
{
	UBRR0H = (unsigned char)(MYUBRR >> 8);
	UBRR0L = (unsigned char) MYUBRR;
	UCSR0B |= (1 << TXEN0) | (1 << RXEN0);
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
}
