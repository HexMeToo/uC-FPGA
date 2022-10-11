/*
 * I2C.h
 *
 *  Created on: 10 lis 2014
 *      Author: Mariusz
 */

#ifndef I2C_H_
#define I2C_H_

#define I2C_STARTError 1
#define I2C_NoNACK 3
#define I2C_NoACK 4
#define I2CBUSCLOCK 50000

void I2C_Init();
void I2C_SetBusSpeed(uint16_t speed);
void I2C_Start();
void I2C_WaitForComplete();
void I2C_Stop();
//void I2C_WaitTillStopWasSent();
void I2C_SendAddr(uint8_t address);
void I2C_SendStartAndSelect(uint8_t addr);
void I2C_SendByte(uint8_t byte);
uint8_t I2C_ReceiveData_NACK();
uint8_t I2C_ReceiveData_ACK();
void I2C_SetError(uint8_t err);

#endif /* I2C_H_ */
