#include "simplelink.h"
#include "sl_common.h"
#include "C:\Users\Mariusz\workspace_ccs\MGR_Files\Frames.h"
#include "C:\Users\Mariusz\workspace_ccs\MGR_Files\CC3100_Config.h"
#include "C:\Users\Mariusz\workspace_ccs\MGR_Files\Global_Var.h"
#include "Global_Var_Remote.h"
//#include <msp430.h>
//#include "msp430f5529.h"
#include "User_Functions.h"


int main(int argc, char** argv)
{
    Start_Device();
    Init_User_Data();
    Init_ADC();

    Delay(SOFT_START_TIME);



    CHANNEL = Find_Channel();
    Connect_To_Robot(CHANNEL);

    while(1)
    {
        Confirm_ACK_Data(CHANNEL);
    }
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
    if(STREAM_FRAME_COUNTER > STREAM_FRAME_FREQ)
    {
        STREAM_FRAME_COUNTER_EN = 1;
    }
    else STREAM_FRAME_COUNTER++;

    if(MOTION_FRAME_COUNTER > MOTION_FRAME_FREQ)
    {
        MOTION_FRAME_COUNTER_EN = 1;
    }
    else MOTION_FRAME_COUNTER++;

    if(BODY_CONFIG_FRAME_COUNTER > BODY_CONFIG_FRAME_FREQ)
    {
        BODY_CONFIG_FRAME_COUNTER_EN = 1;
    }
    else BODY_CONFIG_FRAME_COUNTER++;

    if(SENSOR_DATA_FRAME_COUNTER > SENSOR_DATA_FRAME_FREQ)
    {
        SENSOR_DATA_FRAME_COUNTER_EN = 1;
    }
    else SENSOR_DATA_FRAME_COUNTER++;

    if(LISTEN_COUNTER > LISTEN_TIME)
    {
        LISTEN_COUNTER_EN = 1;
    }
    else LISTEN_COUNTER++;

    if(FIND_ROBOT_COUNTER > FIND_ROBOT_FREQ)
    {
        FIND_ROBOT_COUNTER_EN = 1;
    }
    else FIND_ROBOT_COUNTER++;
}


#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12_ISR (void)
#else
#error Compiler not supported!
#endif
{


    switch(__even_in_range(ADC12IV,34))
    {
    case  0: break;                           // Vector  0:  No interrupt
    case  2: break;                           // Vector  2:  ADC overflow
    case  4: break;                           // Vector  4:  ADC timing overflow

    case  6:

    break;

    case  8:
//        Coordinate_x = ADC12MEM0;
//        Coordinate_z = ADC12MEM1;
        Frame_Motion[6] = (float)ADC12MEM0 * (255.0/4095.0);
        Frame_Motion[8] = (float)ADC12MEM1 * (255.0/4095.0);
    break;

    case 10: break;                           // Vector 10:  ADC12IFG2
    case 12: break;                                 // Vector 12:  ADC12IFG3
    case 14: break;                           // Vector 14:  ADC12IFG4
    case 16: break;                           // Vector 16:  ADC12IFG5
    case 18: break;                           // Vector 18:  ADC12IFG6
    case 20: break;                           // Vector 20:  ADC12IFG7
    case 22: break;                           // Vector 22:  ADC12IFG8
    case 24: break;                           // Vector 24:  ADC12IFG9
    case 26: break;                           // Vector 26:  ADC12IFG10
    case 28: break;                           // Vector 28:  ADC12IFG11
    case 30: break;                           // Vector 30:  ADC12IFG12
    case 32: break;                           // Vector 32:  ADC12IFG13
    case 34: break;                           // Vector 34:  ADC12IFG14
    default: break;
    }
}
