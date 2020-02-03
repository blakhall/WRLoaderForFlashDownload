

/**
 * main.c
 */
#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "util.h"
#include <KLoader.h>
#define RESULTS_BUFFER_SIZE 256


/* Define for this example */
#define ENCODER_REV     24          /* Pulse/Revolution */
#define SYSTEM_FREQ     150E6       /* Hz */
#define UTIME_FREQ      2           /* Hz */

int mainCnt=0;
double timer0Cnt[0x10]={0,};

int main(void)
{
    initializeSystem();
    for(;;)
    {
        mainCnt++;
        if(timer0Cnt[0]>0.2)
        {
            //Putch(&serialport0,sciCnt++);
            GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
            timer0Cnt[0]=0.0;
            GetInputData();
            if(Kb_hit(serialport0))
            {
                Putch(&serialport0,Getch(&serialport0));
            }
        }

    }
    return 0;
}




//============================================================================================
//  ISR 함수 정의  Util.h
//--------------------------------------------------------------------------------------------
int adcCnt=0x00;
Uint16 AdcaResults[RESULTS_BUFFER_SIZE];
Uint16 resultsIndex;
volatile Uint16 bufferFull;

interrupt void adca1_isr(void)
{
    adcCnt++;
    AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT0;
    if(RESULTS_BUFFER_SIZE <= resultsIndex)
    {
        resultsIndex = 0;
        bufferFull = 1;
    }

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1;
}

int CPU_Timer0_Isr_cnt;
interrupt void cpu_timer0_isr(void)
{
    int i;
    CPU_Timer0_Isr_cnt++;
    timeClock.appTimer += 1e-3;
    for(i=0;i<0x10;i++)timer0Cnt[i]+=1e-3;
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP1;
}
int scibRxCnt=0;
interrupt void scibRxIsr(void)
{
    scibRxCnt++;
    RxFIFO(&serialport0, ScibRegs );
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP9;       // Issue PIE ack
}
interrupt void epwm1_isr(void)
{

    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP3;
}
interrupt void epwm2_isr(void)
{

    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP3;
}
interrupt void epwm3_isr(void)
{
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP3;

}
interrupt void epwm4_isr(void)
{
    PieCtrlRegs.PIEACK.all |= PIEACK_GROUP3;
}


