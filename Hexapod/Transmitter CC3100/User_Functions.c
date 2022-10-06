#include "simplelink.h"
#include "sl_common.h"
#include "C:\Users\Mariusz\workspace_ccs\MGR_Files\Frames.h"
#include "C:\Users\Mariusz\workspace_ccs\MGR_Files\CC3100_Config.h"
#include "C:\Users\Mariusz\workspace_ccs\MGR_Files\Global_Var.h"
#include "Global_Var_Remote.h"
#include "User_Functions.h"

void Connect_To_Robot(_i8 channel_number)
{
    _u8 buffer[BUF_SIZE] = {'\0'};
    long NonBlocking = 1;
    _i32 SockID = -1;
    _i32 Status = 0;
    _u8 Connect_Status = 1;

//    socket_hanlde = sl_Socket(SL_AF_RF, SL_SOCK_RAW, channel_number); // L1
    SockID = sl_Socket(SL_AF_RF, SL_SOCK_DGRAM, channel_number); // L2

    Status = sl_SetSockOpt(SockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &NonBlocking, sizeof(NonBlocking));
    if (Status < 0)
    {
        sl_Close(SockID);
        return;
    }

    Start_Timer();

    while(Connect_Status)
    {
        memset(&buffer[0], 0, sizeof(buffer));
        sl_Recv(SockID, buffer, BUF_SIZE, 0);

        switch(buffer[13])
        {
            case FRRF:
                CLI_Write("<-FRRF\n\r");
                Connect_Status = 0;
            break;
        }

        if(FIND_ROBOT_COUNTER_EN == 1)
        {
            Status = sl_Send(SockID, Frame_Find_Robot, FIND_ROBOT_FRAME_SIZE, SL_RAW_RF_TX_PARAMS(channel_number, RATE, POWER_LEVEL_TONE, PREAMBLE));
            FIND_ROBOT_COUNTER_EN = 0;
            FIND_ROBOT_COUNTER = 0;
            CLI_Write("->FRF\n\r");
        }
    }

    Stop_Timer();
    sl_Close(SockID);
}

void Confirm_ACK_Data (_i8 channel_number)
{
    _u8 buffer[BUF_SIZE] = {'\0'};
    long NonBlocking = 1;
    _i32 SockID = -1;
    _i32 Status = 0;

    _u32 Ilosc_nadanych_StDRF = 0;
    _u32 Ilosc_nadanych_MF = 0;
    _u32 Ilosc_nadanych_CF = 0;
    _u32 Ilosc_nadanych_SDRF = 0;
    _u32 Ilosc_odebranych_StDF = 0;
    _u32 Ilosc_odebranych_SDF = 0;

    SockID = sl_Socket(SL_AF_RF, SL_SOCK_DGRAM, channel_number); // L2

    Status = sl_SetSockOpt(SockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &NonBlocking, sizeof(NonBlocking));
    if (Status < 0)
    {
        sl_Close(SockID);
        return;
    }

    Start_Timer();

    while(1)
    {
        memset(&buffer[0], 0, sizeof(buffer));
        sl_Recv(SockID, buffer, BUF_SIZE, 0);

        switch(buffer[13])
        {
            case StDF:
                Ilosc_odebranych_StDF++;
//                CLI_Write("<-StDF\n\r");
            break;

            case SDF:
                Ilosc_odebranych_SDF++;
//                CLI_Write("<-SDF\n\r");
            break;

            case FRRF:
//                CLI_Write("<-FRRF\n\r");
            break;

            case AF:
//                CLI_Write("<-AF\n\r");
            break;
        }

//        Status = sl_Send(SockID, Frame_Sensor_Data_Request, SENSOR_DATA_REQUEST_FRAME_SIZE, SL_RAW_RF_TX_PARAMS(channel_number, RATE, POWER_LEVEL_TONE, PREAMBLE));

        if(STREAM_FRAME_COUNTER_EN == 1)
        {
            Status = sl_Send(SockID, Frame_Stream_Data_Request, STREAM_DATA_REQUEST_FRAME_SIZE, SL_RAW_RF_TX_PARAMS(channel_number, RATE, POWER_LEVEL_TONE, PREAMBLE));
            STREAM_FRAME_COUNTER_EN = 0;
            STREAM_FRAME_COUNTER = 0;
//            CLI_Write("->StDRF\n\r");
//            Delay_10us(150);
            Ilosc_nadanych_StDRF++;
        }

        if(MOTION_FRAME_COUNTER_EN == 1)
        {
            ADC12CTL0 |= ADC12SC; // Measure actual value of joystick
            Status = sl_Send(SockID, Frame_Motion, MOTION_FRAME_SIZE, SL_RAW_RF_TX_PARAMS(channel_number, RATE, POWER_LEVEL_TONE, PREAMBLE));
            MOTION_FRAME_COUNTER_EN = 0;
            MOTION_FRAME_COUNTER = 0;
//            CLI_Write("->MF\n\r");
//            Delay_10us(150);
            Ilosc_nadanych_MF++;
        }

        if(BODY_CONFIG_FRAME_COUNTER_EN == 1)
        {
            Status = sl_Send(SockID, Frame_Body_Config, BODY_CONFIG_FRAME_SIZE, SL_RAW_RF_TX_PARAMS(channel_number, RATE, POWER_LEVEL_TONE, PREAMBLE));
            BODY_CONFIG_FRAME_COUNTER_EN = 0;
            BODY_CONFIG_FRAME_COUNTER = 0;
//            CLI_Write("->CF\n\r");
//            Delay_10us(150);
            Ilosc_nadanych_CF++;
        }

        if(SENSOR_DATA_FRAME_COUNTER_EN == 1)
        {
            Status = sl_Send(SockID, Frame_Sensor_Data_Request, SENSOR_DATA_REQUEST_FRAME_SIZE, SL_RAW_RF_TX_PARAMS(channel_number, RATE, POWER_LEVEL_TONE, PREAMBLE));
            SENSOR_DATA_FRAME_COUNTER_EN = 0;
            SENSOR_DATA_FRAME_COUNTER = 0;
//            CLI_Write("->SDRF\n\r");
//            Delay_10us(150);
            Ilosc_nadanych_SDRF++;
        }

        if(Ilosc_nadanych_SDRF >= 600)
        {
            CLI_Write("N Stream data request: ");
            Wypisz(Ilosc_nadanych_StDRF);
            CLI_Write("\n\rO Stream data: ");
            Wypisz(Ilosc_odebranych_StDF);
            CLI_Write("\n\rN Sensor data request: ");
            Wypisz(Ilosc_nadanych_SDRF);
            CLI_Write("\n\rO Sensor data: ");
            Wypisz(Ilosc_odebranych_SDF);
            CLI_Write("\n\rN Motion: ");
            Wypisz(Ilosc_nadanych_MF);
            CLI_Write("\n\rN Config: ");
            Wypisz(Ilosc_nadanych_CF);

            while(1)
            {

            }
        }
    }

    sl_Close(SockID);
}

void Wypisz(_u32 data)
{
    _u8 temp[11] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '\0'};

    temp[9] = (data % 10) + 48;
    data /= 10;

    temp[8] = (data % 10) + 48;
    data /= 10;

    temp[7] = (data % 10) + 48;
    data /= 10;

    temp[6] = (data % 10) + 48;
    data /= 10;

    temp[5] = (data % 10) + 48;
    data /= 10;

    temp[4] = (data % 10) + 48;
    data /= 10;

    temp[3] = (data % 10) + 48;
    data /= 10;

    temp[2] = (data % 10) + 48;
    data /= 10;

    temp[1] = (data % 10) + 48;
    data /= 10;

    temp[0] = (data % 10) + 48;

    CLI_Write(&temp[0]);
}

_u8 Find_Channel(void)
{
    _u8 buffer[BUF_SIZE] = {'\0'};
    long NonBlocking = 1;
    _i32 SockID = -1;
    _i32 Status = 0;
    _u8 i = 0;
    _i32 Best_Channel = 1;

    for(i=1; i<14; i++)
    {
        Max_RSSI[i-1] = 255;

        SockID = sl_Socket(SL_AF_RF, SL_SOCK_DGRAM, i); // Layer 2
        Status = sl_SetSockOpt(SockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &NonBlocking, sizeof(NonBlocking));
        if (Status < 0)
        {
            sl_Close(SockID);
            return 1;
        }

        Start_Timer();

        while(1)
        {
            memset(&buffer[0], 0, sizeof(buffer));
            sl_Recv(SockID, buffer, BUF_SIZE, 0);

            if(buffer[2] > 0)
            {
                if(Max_RSSI[i-1] > buffer[2]) Max_RSSI[i-1] = buffer[2];
            }

            if(LISTEN_COUNTER_EN == 1) break;
        }

        sl_Close(SockID);
        Stop_Timer();
        Delay(50);

        CLI_Write("RSSI Channel ");
        BinToByteASCII(i);
        CLI_Write(": ");
        BinToByteASCII(Max_RSSI[i-1]);
        CLI_Write("\n\r");
    }

    for(i=1; i<13; i++)
    {
        if(Max_RSSI[0] < Max_RSSI[i])
        {
            Max_RSSI[0] = Max_RSSI[i];
            Best_Channel = i+1;
        }
    }

    return Best_Channel;
}

void BinToByteASCII (_u8 data)
{
    _u8 temp[4] = {0x00, 0x00, '\0'};

    temp[2] = (data % 10) + 48;
    data /= 10;

    temp[1] = (data % 10) + 48;
    data /= 10;

    temp[0] = (data % 10) + 48;

    CLI_Write(&temp[0]);
}

void Init_User_Data (void)
{
//    P1DIR &= ~(BIT1);
//    P1REN |= BIT1;
//    P1OUT |= BIT1;
//
//    P4DIR |= BIT7;
//    P1DIR |= 0x01;

    // Timer Init
    TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
    TA0CCR0 = 0;
    TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR

    Frame_Stream_Data_Response[0] = 0x08;
    Frame_Stream_Data_Response[1] = RADIO_ID >> 8;
    Frame_Stream_Data_Response[2] = RADIO_ID;
}

void Start_Timer (void)
{
    TA0CCR0 = 2500;

    STREAM_FRAME_COUNTER = 0;
    MOTION_FRAME_COUNTER = 0;
    BODY_CONFIG_FRAME_COUNTER = 0;
    SENSOR_DATA_FRAME_COUNTER = 0;
    LISTEN_COUNTER = 0;
    FIND_ROBOT_COUNTER = 0;

    STREAM_FRAME_COUNTER_EN = 0;
    MOTION_FRAME_COUNTER_EN = 0;
    BODY_CONFIG_FRAME_COUNTER_EN = 0;
    SENSOR_DATA_FRAME_COUNTER_EN = 0;
    LISTEN_COUNTER_EN = 0;
    FIND_ROBOT_COUNTER_EN = 0;
}

void Init_ADC(void)
{
    WDTCTL = WDTPW+WDTHOLD;                   // Stop watchdog timer
    ADC12CTL0 = ADC12ON+ADC12MSC+ADC12SHT0_15;// Turn on ADC12, set sampling time
    ADC12CTL1 = ADC12SHP+ADC12CONSEQ_1;       // Use sampling timer, single sequence
    ADC12MCTL0 = ADC12INCH_0;                 // ref+=AVcc, channel = A0
    ADC12MCTL1 = ADC12INCH_1+ADC12EOS;        // ref+=AVcc, channel = A1, end seq.
    ADC12IE = 0x02;                           // Enable ADC12IFG.1, end channel
    ADC12CTL0 |= ADC12ENC;                    // Enable conversions
}

void Stop_Timer (void)
{
    TA0CCR0 = 0;
}
