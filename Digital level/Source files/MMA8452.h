/*
 * MMA8452.h
 *
 *  Created on: 10 lis 2014
 *      Author: Mariusz
 */

#ifndef MMA8452_H_
#define MMA8452_H_

#define WHO_AM_I 0x0D
#define DEVICE_ADDRES_READ 0x3B
#define DEVICE_ADDRES_WRITE 0x3A
#define CTRL_REG1 0x2A
#define CTRL_REG4 0x2D
#define CTRL_REG5 0x2E
#define XYZ_DATA_CFG 0x0E
#define OFF_X_Register 0x2F
#define OFF_Y_Register 0x30
#define RANGE 2
/*
	SCALE RANGE
	2	2G
	4	4G
	8	8G
*/
#define DATA_RATE 1
/*
	DATA RATE
	800	800  Hz	- 1.25 ms
	400	400  Hz	- 2.5  ms
	200	200  Hz	- 5    ms
	100	100  Hz	- 10   ms
	50	50 	 Hz	- 20   ms
	12	12.5 Hz	- 80   ms
	6	6.25 Hz	- 160  ms
	1	1.56 Hz	- 640  ms
*/

void MMA8452_Init();
uint8_t MMA8452_Read_Single_Byte(uint8_t);
void MMA8452_Write_Single_Byte(uint8_t, uint8_t);
void MMA8452_Read_Multiple_Bytes(uint8_t, uint8_t, uint8_t *);
void MMA8452_Turn_On_Active_Mode();
void MMA8452_Turn_Off_Active_Mode();
void MMA8452_Turn_On_248G();
void MMA8452_Data_Rate();
void MMA8452_Turn_On_Interrupts();
void MMA8452_Change_Offset_X(int8_t);
void MMA8452_Change_Offset_Y(int8_t);


#endif /* MMA8452_H_ */
