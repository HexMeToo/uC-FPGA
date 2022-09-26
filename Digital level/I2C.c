#include <avr/io.h>
#include "I2C.h"
#include <util/twi.h>

void I2C_Init()
{
	TWCR =_BV(TWEA) | _BV(TWEN); //W³¹cz interfejs I2C
	I2C_SetBusSpeed(I2CBUSCLOCK/100);
}

void I2C_SetBusSpeed(uint16_t speed)
{
	speed=(F_CPU/speed/100-16)/2; //speed=TWBR·4^TWPS
	uint8_t prescaler=0;

	while(speed>255) //Oblicz wartoœæ preskalera
	{
		prescaler++;
		speed=speed/4;
	};

	TWSR=(TWSR & (_BV(TWPS1)|_BV(TWPS0))) | prescaler;
	TWBR=speed;
}

void I2C_Start()
{
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);
	I2C_WaitForComplete();
	if (TW_STATUS != TW_START) I2C_SetError(I2C_STARTError);
}

void I2C_WaitForComplete()
{
	while (!(TWCR & _BV(TWINT)));
}

void I2C_Stop()
{
	TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWSTO);
}

/*void I2C_WaitTillStopWasSent()
{
	while (TWCR & _BV(TWSTO));
}*/

void I2C_SendAddr(uint8_t address)
{
	uint8_t Status;
	if((address & 0x01)==0) Status=TW_MT_SLA_ACK;
	else Status = TW_MR_SLA_ACK;
	TWDR=address;
	TWCR=_BV(TWINT) | _BV(TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS!=Status) I2C_SetError(I2C_NoACK);
}

void I2C_SendStartAndSelect(uint8_t addr)
{
	I2C_Start();
	I2C_SendAddr(addr);
}

void I2C_SendByte(uint8_t byte)
{
	TWDR=byte;
	TWCR=_BV(TWINT) | _BV(TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS!=TW_MT_DATA_ACK) I2C_SetError(I2C_NoACK);
}

uint8_t I2C_ReceiveData_NACK()
{
	TWCR=_BV(TWINT) | _BV(TWEN);
	I2C_WaitForComplete();
	if (TW_STATUS!=TW_MR_DATA_NACK) I2C_SetError(I2C_NoNACK);
	return TWDR;
}

uint8_t I2C_ReceiveData_ACK()
{
	TWCR=_BV(TWEA) | _BV(TWINT) | _BV(TWEN);
	I2C_WaitForComplete();
	if(TW_STATUS!=TW_MR_DATA_ACK) I2C_SetError(I2C_NoACK);
	return TWDR;
}

void I2C_SetError(uint8_t err)
{
	uint8_t I2C_Error;
	I2C_Error=err;
}
