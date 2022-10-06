#ifndef USER_FUNCTIONS_H_
#define USER_FUNCTIONS_H_

void Confirm_ACK_Data (_i8 channel_number);
void Init_User_Data (void);
void Start_Timer (void);
void Stop_Timer (void);
_u8 Find_Channel(void);
void BinToByteASCII (_u8 data);
void Connect_To_Robot(_i8 channel_number);
void Init_ADC(void);
void Wypisz(_u32 data);

#endif
