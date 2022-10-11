#include <avr/io.h>
#include "MMA8452.h"
#include "I2C.h"

void MMA8452_Init()
{
	MMA8452_Turn_Off_Active_Mode();
	MMA8452_Turn_On_248G();
	MMA8452_Data_Rate();
	MMA8452_Turn_On_Interrupts();
	MMA8452_Turn_On_Active_Mode();
}

void MMA8452_Data_Rate()
{
	uint8_t byte, mode;
	byte = MMA8452_Read_Single_Byte(CTRL_REG1);
	switch (DATA_RATE)
	{
		case 800:
			mode = 0x00;
		break;
		case 400:
			mode = 0x08;
		break;
		case 200:
			mode = 0x10;
		break;
		case 100:
			mode = 0x18;
		break;
		case 50:
			mode = 0x20;
		break;
		case 12:
			mode = 0x28;
		break;
		case 6:
			mode = 0x30;
		break;
		case 1:
			mode = 0x38;
		break;
	}
	MMA8452_Write_Single_Byte(CTRL_REG1, (byte & 0xC7) | mode);
}

uint8_t MMA8452_Read_Single_Byte(uint8_t register_to_read)
{
	uint8_t bajt;
	I2C_SendStartAndSelect(DEVICE_ADDRES_WRITE);
	I2C_SendAddr(register_to_read);
	I2C_SendStartAndSelect(DEVICE_ADDRES_READ);
	bajt = I2C_ReceiveData_NACK();
	I2C_Stop();

	return bajt;
}

void MMA8452_Write_Single_Byte(uint8_t register_to_write, uint8_t data_to_write)
{
	I2C_SendStartAndSelect(DEVICE_ADDRES_WRITE);
	I2C_SendAddr(register_to_write);
	I2C_SendByte(data_to_write);
	I2C_Stop();
}

void MMA8452_Read_Multiple_Bytes(uint8_t start_register, uint8_t how_many_bytes, uint8_t *OUT_XYZ_array)
{
	uint8_t i;

	I2C_SendStartAndSelect(DEVICE_ADDRES_WRITE);
	I2C_SendAddr(start_register);
	I2C_SendStartAndSelect(DEVICE_ADDRES_READ);

	for(i=0; i<how_many_bytes-1; i++)
	{
		OUT_XYZ_array[i] = I2C_ReceiveData_ACK();
	}

	OUT_XYZ_array[i] = I2C_ReceiveData_NACK();
	I2C_Stop();
}

void MMA8452_Turn_On_Active_Mode()
{
	uint8_t byte;
	byte = MMA8452_Read_Single_Byte(CTRL_REG1);
	MMA8452_Write_Single_Byte(CTRL_REG1, byte | 0x01);
}

void MMA8452_Turn_Off_Active_Mode()
{
	uint8_t byte;
	byte = MMA8452_Read_Single_Byte(CTRL_REG1);
	MMA8452_Write_Single_Byte(CTRL_REG1, byte & 0xFE);
}

void MMA8452_Turn_On_248G()
{
	uint8_t byte, mode;

	switch (RANGE)
	{
		case 2:
			mode = 0x00;
		break;
		case 4:
			mode = 0x01;
		break;
		case 8:
			mode = 0x02;
		break;
	}

	byte = MMA8452_Read_Single_Byte(XYZ_DATA_CFG);
	MMA8452_Write_Single_Byte(XYZ_DATA_CFG, (byte & 0xFC) | mode);
}

void MMA8452_Turn_On_Interrupts()
{
	uint8_t byte;
	byte = MMA8452_Read_Single_Byte(CTRL_REG4);
	MMA8452_Write_Single_Byte(CTRL_REG4, (byte & 0xFE) | 0x01);

	byte = MMA8452_Read_Single_Byte(CTRL_REG5);
	MMA8452_Write_Single_Byte(CTRL_REG5, (byte & 0xFE) | 0x01);
}

void MMA8452_Change_Offset_X(int8_t Offset)
{
	MMA8452_Turn_Off_Active_Mode();
	MMA8452_Write_Single_Byte(OFF_X_Register, Offset);
	MMA8452_Turn_On_Active_Mode();
}

void MMA8452_Change_Offset_Y(int8_t Offset)
{
	MMA8452_Turn_Off_Active_Mode();
	MMA8452_Write_Single_Byte(OFF_Y_Register, Offset);
	MMA8452_Turn_On_Active_Mode();
}
