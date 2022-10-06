
#include "simplelink.h"
#include "sl_common.h"
#include <stdio.h>

#define SL_STOP_TIMEOUT     0xFF
#define POWER_LEVEL_TONE    1           /* Power level tone valid range 0-15 */
#define PREAMBLE            1           /* Preamble value 0- short, 1- long */
#define RATE                RATE_24M    /* Speed */
//#define CHANNEL             1           /* Channel (1-13) used during the tx and rx*/
#define BUF_SIZE 1400                   // Buffer for received data

#define RADIO_ID 0x02, 0x01

//#define STREAM_FRAME_FREQ         333     // clock ticks == 30Hz
//#define MOTION_FRAME_FREQ         1000    // clock ticks == 10Hz
//#define BODY_CONFIG_FRAME_FREQ    5000    // clock ticks == 2Hz
//#define SENSOR_DATA_FRAME_FREQ    10000   // clock ticks == 1Hz

#define STREAM_FRAME_FREQ         8500     // clock ticks == 30Hz
#define MOTION_FRAME_FREQ         9000    // clock ticks == 10Hz
#define BODY_CONFIG_FRAME_FREQ    9500    // clock ticks == 2Hz
#define SENSOR_DATA_FRAME_FREQ    10000   // clock ticks == 1Hz
#define LISTEN_TIME               10000
#define DATA_TIMEOUT              50000

#define STREAM_DATA_FRAME_SIZE          1400
#define STREAM_DATA_REQUEST_FRAME_SIZE  14
#define SENSOR_DATA_FRAME_SIZE          14
#define SENSOR_DATA_REQUEST_FRAME_SIZE  14
#define BODY_CONFIG_FRAME_SIZE          14
#define MOTION_FRAME_SIZE               14
#define FIND_ROBOT_FRAME_SIZE           14
#define FIND_ROBOT_RESPONSE_FRAME_SIZE  14
#define ACK_RESPONSE_FRAME_SIZE         14
#define ACK_REQUEST_FRAME_SIZE          14

#define StDF    1   // Stream Data Frame
#define StDRF   2   // Stream Data Request Frame
#define SDF     3   // Sensor Data Frame
#define SDRF    4   // Sensor Data Request Frame
#define CF      5   // Config Frame
#define MF      6   // Motion Frame
#define FRF     7   // Find Robot Frame
#define FRRF    8   // Find Robot Response
#define AF      9   // ACK Response
#define ARF    10   // ACK Request

/* Application specific status/error codes */
typedef enum
{
    DEVICE_NOT_IN_STATION_MODE = -0x7D0,        /* Choosing this number to avoid overlap w/ host-driver's error codes */
    STATUS_CODE_MAX = -0xBB8
} e_AppStatusCodes;

/*
 * GLOBAL VARIABLES -- Start
 */
_u8 g_Status = 0;
_u8 EN = 0;
_u8 EN2 = 0;
_u8 koniec = 1;
_u64 start = 0;
_u64 stop = 0;
_u8 ON = 0;
_u32 GLOBAL = 0;
_i32 zmienna = 0;
_u8 CHANNEL = 1;
_u8 State = 0;

_u16 STREAM_FRAME_COUNTER = 0;
_u16 MOTION_FRAME_COUNTER = 0;
_u16 BODY_CONFIG_FRAME_COUNTER = 0;
_u16 SENSOR_DATA_FRAME_COUNTER = 0;
_u8 STREAM_FRAME_COUNTER_EN = 0;
_u8 MOTION_FRAME_COUNTER_EN = 0;
_u8 BODY_CONFIG_FRAME_COUNTER_EN = 0;
_u8 SENSOR_DATA_FRAME_COUNTER_EN = 0;
_u16 LISTEN_COUNTER = 0;
_u8 LISTEN_COUNTER_EN = 0;
_u16 DATA_TIMEOUT_COUNTER = 0;
_u8 DATA_TIMEOUT_COUNTER_EN = 0;
_u8 Voltage_of_Battery = 0;
_u8 Interrupt_Buffer = 0;

union
{
  _u8 BsdBuf[BUF_SIZE];
  _u32 demobuf[BUF_SIZE/4];
} uBuf;

typedef struct
{
    _u8 Rate;
    _u8 Channel;
    _i8 Rssi;
    _u8 Padding;
    _u32 Timestamp;
    _u8 Standard;
    _u16 Source;
    _u16 Destination;
    _u8 Type;
} TransceiverRxOverHead_t;

_u8 Frame_Stream_Data_Response[STREAM_DATA_FRAME_SIZE] = {0};

_u8 Frame_Stream_Data_Request[STREAM_DATA_FRAME_SIZE] =
{
    0x08,                   // 802.11
    RADIO_ID,               // Source ID
    0xFF, 0xFF,             // Destination ID
    StDRF,
    '#', '#', '#', '#',
    0xFF, 0xFF, 0xFF, 0xFF  // CRC
};

_u8 Frame_Sensor_Data_Response[SENSOR_DATA_FRAME_SIZE] =
{
    0x08,                   // 802.11
    RADIO_ID,               // Source ID
    0xFF, 0xFF,             // Destination ID
    SDF,
    0x00,                   // Battery
    '#', '#', '#',
    0xFF, 0xFF, 0xFF, 0xFF  // CRC
};

_u8 Frame_Sensor_Data_Request[SENSOR_DATA_REQUEST_FRAME_SIZE] =
{
    0x08,                   // 802.11
    RADIO_ID,               // Source ID
    0xFF, 0xFF,             // Destination ID
    SDRF,
    '#', '#', '#', '#',
    0xFF, 0xFF, 0xFF, 0xFF  // CRC
};

_u8 Frame_Body_Config[BODY_CONFIG_FRAME_SIZE] =
{
    0x08,                   // 802.11
    RADIO_ID,               // Source ID
    0xFF, 0xFF,             // Destination ID
    CF,
    0x00,                   // Turn Left / Right
    0x00,                   // Lean Forward / Backward
    '#', '#',
    0xFF, 0xFF, 0xFF, 0xFF  // CRC
};

_u8 Frame_Motion[MOTION_FRAME_SIZE] =
{
    0x08,                   // 802.11
    RADIO_ID,               // Source ID
    0xFF, 0xFF,             // Destination ID
    MF,
    0x00,                   // Forward / Backward
    0x00,                   // Left / Right
    '#', '#',
    0xFF, 0xFF, 0xFF, 0xFF  // CRC
};

_u8 Frame_Find_Robot[FIND_ROBOT_FRAME_SIZE] =
{
    0x08,                   // 802.11
    RADIO_ID,               // Source ID
    0xFF, 0xFF,             // Destination Broadcast
    FRF,
    '#', '#', '#', '#',
    0xFF, 0xFF, 0xFF, 0xFF  // CRC
};

_u8 Frame_Find_Robot_Response[FIND_ROBOT_RESPONSE_FRAME_SIZE] =
{
    0x08,                   // 802.11
    RADIO_ID,               // Source ID
    0x00, 0x00,             // Destination ID
    FRRF,
    '#', '#', '#', '#',
    0xFF, 0xFF, 0xFF, 0xFF  // CRC
};

_u8 Frame_ACK_Response[ACK_RESPONSE_FRAME_SIZE] =
{
    0x08,                   // 802.11
    RADIO_ID,               // Source ID
    0x00, 0x00,             // Destination ID
    AF,
    '#', '#', '#', '#',
    0xFF, 0xFF, 0xFF, 0xFF  // CRC
};

_u8 Frame_ACK_Request[ACK_REQUEST_FRAME_SIZE] =
{
    0x08,                   // 802.11
    RADIO_ID,               // Source ID
    0x00, 0x00,             // Destination ID
    ARF,
    '#', '#', '#', '#',
    0xFF, 0xFF, 0xFF, 0xFF  // CRC
};

static _i32 configureSimpleLinkToDefaultState();
static _i32 initializeAppVariables();
void Init_User_Data (void);
_i32 Start_Device (void);
void Stop_Device (void);
void Start_Timer (void);
void Stop_Timer (void);
void Confirm_ACK_Data (_i8 channel_number);
_u8 Find_Channel(void);
void Init_UART(void);
void Send_via_UART (_u8 data);
void Wypisz(_u32 data);

void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(pWlanEvent == NULL)
    {
        CLI_Write(" [WLAN EVENT] NULL Pointer Error \n\r");
        return;
    }
    
    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            /*
             * Information about the connected AP (like name, MAC etc) will be
             * available in 'slWlanConnectAsyncResponse_t' - Applications
             * can use it if required
             *
             * slWlanConnectAsyncResponse_t *pEventData = NULL;
             * pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
             *
             */
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_Status, STATUS_BIT_CONNECTION);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                CLI_Write(" Device disconnected from the AP on application's request \n\r");
            }
            else
            {
                CLI_Write(" Device disconnected from the AP on an ERROR..!! \n\r");
            }
        }
        break;

        default:
        {
            CLI_Write(" [WLAN EVENT] Unexpected event \n\r");
        }
        break;
    }
}

void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(pNetAppEvent == NULL)
    {
        CLI_Write(" [NETAPP EVENT] NULL Pointer Error \n\r");
        return;
    }
 
    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SET_STATUS_BIT(g_Status, STATUS_BIT_IP_ACQUIRED);
        }
        break;

        default:
        {
            CLI_Write(" [NETAPP EVENT] Unexpected event \n\r");
        }
        break;
    }
}

void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent, SlHttpServerResponse_t *pHttpResponse)
{
    /* Unused in this application */
    CLI_Write(" [HTTP EVENT] Unexpected event \n\r");
}

void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
    CLI_Write(" [GENERAL EVENT] \n\r");
}

void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(pSock == NULL)
    {
        CLI_Write(" [SOCK EVENT] NULL Pointer Error \n\r");
        return;
    }
    
    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status )
            {
                case SL_ECLOSE:
                    CLI_Write(" [SOCK EVENT] Close socket operation failed to transmit all queued packets\n\r");
                    break;
                default:
                    CLI_Write(" [SOCK EVENT] Unexpected event \n\r");
                    break;
            }
            break;

        default:
            CLI_Write(" [SOCK EVENT] Unexpected event \n\r");
            break;
    }
}

int main(int argc, char** argv)
{
    Start_Device();
    Init_User_Data();
    Init_UART();

    Delay(250);

    while(1)
    {
        switch(State)
        {
            case 0:
                CHANNEL = Find_Channel();
            break;

            case 1:
                Confirm_ACK_Data(CHANNEL);
            break;
        }
    }
}

void Init_UART(void)
{
    WDTCTL = WDTPW + WDTHOLD;
    P3SEL |= BIT3+BIT4;
    UCA0CTL1 |= UCSWRST;
    UCA0CTL1 |= UCSSEL_2;
    UCA0BR0 = 0x40; // BAUD 50,000
    UCA0BR1 = 0x1F; // BAUD 50,000
    UCA0MCTL |= UCBRS_1 + UCBRF_0;
    UCA0CTL1 &= ~UCSWRST;
    UCA0IE |= UCRXIE;
}

void Send_via_UART (_u8 data)
{
    // Is the USCI_A0 TX buffer ready?
    while (!(UCA0IFG & UCTXIFG));

    UCA0TXBUF = data;
}

_i32 Start_Device (void)
{
    _i32 retVal = -1;

    retVal = initializeAppVariables();
    ASSERT_ON_ERROR(retVal);

    stopWDT();
    initClk();
    CLI_Configure();

    retVal = configureSimpleLinkToDefaultState();
    if(retVal < 0) LOOP_FOREVER();

    retVal = sl_Start(0, 0, 0);
    if ((retVal < 0) || (ROLE_STA != retVal) ) LOOP_FOREVER();

    return SUCCESS;
}

void Stop_Device (void)
{
    _i32 retVal = -1;

    retVal = sl_Stop(SL_STOP_TIMEOUT);
    if(retVal < 0) LOOP_FOREVER();
}

static _i32 configureSimpleLinkToDefaultState()
{
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    _u8           val = 1;
    _u8           configOpt = 0;
    _u8           configLen = 0;
    _u8           power = 0;

    _i32          retVal = -1;
    _i32          mode = -1;

    mode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(mode);

    /* If the device is not in station-mode, try configuring it in station-mode */
    if (ROLE_STA != mode)
    {
        if (ROLE_AP == mode)
        {
            /* If the device is in AP mode, we need to wait for this event before doing anything */
            while(!IS_IP_ACQUIRED(g_Status)) { _SlNonOsMainLoopTask(); }
        }

        /* Switch to STA role and restart */
        retVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(retVal);

        retVal = sl_Stop(SL_STOP_TIMEOUT);
        ASSERT_ON_ERROR(retVal);

        retVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(retVal);

        /* Check if the device is in station again */
        if (ROLE_STA != retVal)
        {
            /* We don't want to proceed if the device is not coming up in station-mode */
            ASSERT_ON_ERROR(DEVICE_NOT_IN_STATION_MODE);
        }
    }

    /* Get the device's version-information */
    configOpt = SL_DEVICE_GENERAL_VERSION;
    configLen = sizeof(ver);
    retVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &configOpt, &configLen, (_u8 *)(&ver));
    ASSERT_ON_ERROR(retVal);

    /* Set connection policy to Auto + SmartConfig (Device's default connection policy) */
    retVal = sl_WlanPolicySet(SL_POLICY_CONNECTION, SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Remove all profiles */
    retVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(retVal);

    /*
     * Device in station-mode. Disconnect previous connection if any
     * The function returns 0 if 'Disconnected done', negative number if already disconnected
     * Wait for 'disconnection' event if 0 is returned, Ignore other return-codes
     */
    retVal = sl_WlanDisconnect();
    if(0 == retVal)
    {
        /* Wait */
        while(IS_CONNECTED(g_Status)) { _SlNonOsMainLoopTask(); }
    }

    /* Enable DHCP client*/
    retVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&val);
    ASSERT_ON_ERROR(retVal);

    /* Disable scan */
    configOpt = SL_SCAN_POLICY(0);
    retVal = sl_WlanPolicySet(SL_POLICY_SCAN , configOpt, NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Set Tx power level for station mode
       Number between 0-15, as dB offset from max power - 0 will set maximum power */
    power = 0;
    retVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (_u8 *)&power);
    ASSERT_ON_ERROR(retVal);

    /* Set PM policy to normal */
    retVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(retVal);

    /* Unregister mDNS services */
    retVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(retVal);

    /* Remove  all 64 filters (8*8) */
    pal_Memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    retVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(retVal);

    retVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(retVal);

    retVal = initializeAppVariables();
    ASSERT_ON_ERROR(retVal);

    return retVal; /* Success */
}

_u8 Find_Channel(void)
{
    _u8 buffer[BUF_SIZE] = {'\0'};
    long NonBlocking = 1;
    _i32 SockID = -1;
    _i32 recievedBytes = 0;
    _i32 Status = 0;
    _u8 i = 0;
    _u8 Best_Channel = 1;
    _u8 Switch_Channel = 1;

    while(1)
    {
        Switch_Channel = 1;

        if(i < 13)
        {
            i++;
        }
        else i = 1;

        SockID = sl_Socket(SL_AF_RF, SL_SOCK_DGRAM, i); // Layer 2
        Status = sl_SetSockOpt(SockID, SL_SOL_SOCKET, SL_SO_NONBLOCKING, &NonBlocking, sizeof(NonBlocking));
        if (Status < 0)
        {
            sl_Close(SockID);
            return 1;
        }

        Start_Timer();

        while(Switch_Channel)
        {
            memset(&buffer[0], 0, sizeof(buffer));
            recievedBytes = sl_Recv(SockID, buffer, BUF_SIZE, 0);

            switch(buffer[13])
            {
                case FRF:
                    Status = sl_Send(SockID, Frame_Find_Robot_Response, FIND_ROBOT_RESPONSE_FRAME_SIZE, SL_RAW_RF_TX_PARAMS(i, RATE, POWER_LEVEL_TONE, PREAMBLE));
                    Best_Channel = i;
                    State = 1;
                    Stop_Timer();
                    sl_Close(SockID);
                    Delay(50);
                    return Best_Channel;
            }

            if(LISTEN_COUNTER_EN == 1)
            {
                LISTEN_COUNTER_EN = 0;
                LISTEN_COUNTER = 0;
                Switch_Channel = 0;
            }
        }

        sl_Close(SockID);
        Stop_Timer();
        Delay(50);
    }
}

void Confirm_ACK_Data (_i8 channel_number)
{
    _u8 buffer[BUF_SIZE] = {'\0'};
    long NonBlocking = 1;
    _i32 SockID = -1;
    _i32 recievedBytes = 0;
    _i32 Status = 0;
    _u8 licznik = 0;

    _u32 Ilosc_odebranych_StDRF = 0;
    _u32 Ilosc_odebranych_MF = 0;
    _u32 Ilosc_odebranych_CF = 0;
    _u32 Ilosc_odebranych_SDRF = 0;
    _u32 Ilosc_nadanych_StDF = 0;
    _u32 Ilosc_nadanych_SDF = 0;

    SockID = sl_Socket(SL_AF_RF, SL_SOCK_DGRAM, channel_number); // Layer 2

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
        recievedBytes = sl_Recv(SockID, buffer, BUF_SIZE, 0);

        switch(buffer[13]) // Frame Type
        {
            // Stream request
            case StDRF:
                for(licznik = 0; licznik < 26; licznik++)
                {
                    Status = sl_Send(SockID, Frame_Stream_Data_Response, STREAM_DATA_FRAME_SIZE, SL_RAW_RF_TX_PARAMS(channel_number, RATE, POWER_LEVEL_TONE, PREAMBLE));
                }

                DATA_TIMEOUT_COUNTER = 0;

                Ilosc_odebranych_StDRF++;
                Ilosc_nadanych_StDF += 26;
            break;

            // Sensor data request
            case SDRF:
                Send_via_UART(0x55);
                Send_via_UART(0x02);
                Frame_Sensor_Data_Response[6] = Voltage_of_Battery;
                Status = sl_Send(SockID, Frame_Sensor_Data_Response, SENSOR_DATA_FRAME_SIZE, SL_RAW_RF_TX_PARAMS(channel_number, RATE, POWER_LEVEL_TONE, PREAMBLE));
                DATA_TIMEOUT_COUNTER = 0;

                Ilosc_odebranych_SDRF++;
                Ilosc_nadanych_SDF++;
            break;

            // Body configuration data
            case CF:
                Ilosc_odebranych_CF++;
                // Configuration data -> Send to Hexapod
            break;

            // Motion data
            case MF:
                Send_via_UART(0x55);
                Send_via_UART(0x01);
                Send_via_UART(buffer[14]); // Coordinate x
                Send_via_UART(buffer[15]); // Coordinate y
                Send_via_UART(buffer[16]); // Coordinate z
                Send_via_UART(buffer[17]); // Speed
                Ilosc_odebranych_MF++;
            break;
        }

        if(DATA_TIMEOUT_COUNTER_EN == 1)
        {
            State = 0;

            CLI_Write("O Stream data request: ");
            Wypisz(Ilosc_odebranych_StDRF);
            CLI_Write("\n\rN Stream data: ");
            Wypisz(Ilosc_nadanych_StDF);
            CLI_Write("\n\rO Sensor data request: ");
            Wypisz(Ilosc_odebranych_SDRF);
            CLI_Write("\n\rN Sensor data: ");
            Wypisz(Ilosc_nadanych_SDF);
            CLI_Write("\n\rO Motion data: ");
            Wypisz(Ilosc_odebranych_MF);
            CLI_Write("\n\rO Config data: ");
            Wypisz(Ilosc_odebranych_CF);

            while(1);

            sl_Close(SockID);
            return;
        }
    }
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

static _i32 initializeAppVariables()
{
    g_Status = 0;
    pal_Memset(uBuf.BsdBuf, 0, sizeof(uBuf));

    return SUCCESS;
}

void Init_User_Data (void)
{
    P1DIR &= ~(BIT0);
    P1REN |= BIT0;
    P1OUT &= ~BIT0;

    P4DIR |= BIT7;
    P1DIR |= 0x01;

    // Timer Init
    TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
    TA0CCR0 = 0;
    TA0CTL = TASSEL_2 + MC_1 + TACLR;         // SMCLK, upmode, clear TAR

    Frame_Stream_Data_Response[0] = 0x08;
    Frame_Stream_Data_Response[1] = RADIO_ID >> 8;
    Frame_Stream_Data_Response[2] = RADIO_ID;
    Frame_Stream_Data_Response[5] = StDF;
}

void Start_Timer (void)
{
    GLOBAL = 0;
    TA0CCR0 = 2500;

    LISTEN_COUNTER = 0;
    DATA_TIMEOUT_COUNTER = 0;

    LISTEN_COUNTER_EN = 0;
    DATA_TIMEOUT_COUNTER_EN = 0;
}

void Stop_Timer (void)
{
    TA0CCR0 = 0;
}

// -------------------------------------------------------------------------

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=TIMER0_A0_VECTOR
__interrupt void TIMER0_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER0_A0_VECTOR))) TIMER0_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    if(LISTEN_COUNTER > LISTEN_TIME)
    {
        LISTEN_COUNTER_EN = 1;
    }
    else LISTEN_COUNTER++;

    if(DATA_TIMEOUT_COUNTER > DATA_TIMEOUT)
    {
        DATA_TIMEOUT_COUNTER_EN = 1;
    }
    else DATA_TIMEOUT_COUNTER++;
}



// Echo back RXed character, confirm TX buffer is ready first
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(USCI_A0_VECTOR))) USCI_A0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    static uint8_t State = 0;
    Interrupt_Buffer = UCA0RXBUF;

    switch(State)
    {
        // Beginning of command protocol
        case 0:
            if(Interrupt_Buffer == 0x44) State = 1;
        break;

        // Commands recognition
        case 1:
            if(Interrupt_Buffer == 0x01) State = 20;
        break;

        // Battery
        case 10:
//            Controller_x = Interrupt_Buffer;
            State = 0;
        break;
    }
}
