/*
 * system.c
 *
 *  Created on: 2016. 4. 24.
 *      Author: huni
 */
#include "mydef.h"


DEVICE device;

volatile struct EPWM_REGS *ePWM[] ={ &EPwm1Regs,
									 &EPwm2Regs,
									 &EPwm3Regs,
									 &EPwm4Regs,
									 &EPwm5Regs,
									 &EPwm6Regs};
//Write ADC configurations and power up the ADC for both ADC A and ADC B
void ConfigureADC(void)
{
    EALLOW;

    //write configurations
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    //Set pulse positions to late
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;

    //power up the ADC
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;

    //delay for 1ms to allow ADC time to power up
    DELAY_US(1000);

    EDIS;
}

void ConfigureEPWM(void)
{
    EALLOW;
    // Assumes ePWM clock is already enabled
    EPwm1Regs.ETSEL.bit.SOCAEN  = 0;            // Disable SOC on A group
    EPwm1Regs.ETSEL.bit.SOCASEL = 4;            // Select SOC on up-count
    EPwm1Regs.ETPS.bit.SOCAPRD = 1;             // Generate pulse on 1st event
    EPwm1Regs.CMPA.bit.CMPA = 0x0800;          // Set compare A value to 2048 counts
    EPwm1Regs.TBPRD = 0x2000;                   // Set period to 4096 counts
    EPwm1Regs.TBCTL.bit.CTRMODE = 3;            //
    EDIS;
}



void InitEPwmK()
{
    // Setup TBCLK
	ePWM[0]->TBPRD = 0x1000;          // Set timer period 801 TBCLKs
	ePWM[0]->TBPHS.bit.TBPHS = 0x0000;           // Phase is 0
    ePWM[0]->TBCTR = 0x0000;                     // Clear counter

    // Setup counter mode
    ePWM[0]->TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up and down
    ePWM[0]->TBCTL.bit.PHSEN = TB_DISABLE;       // Disable phase loading
    ePWM[0]->TBCTL.bit.HSPCLKDIV = TB_DIV1;      // Clock ratio to SYSCLKOUT
    ePWM[0]->TBCTL.bit.CLKDIV = TB_DIV1;

    // Setup shadowing
    ePWM[0]->CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    ePWM[0]->CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    ePWM[0]->CMPCTL.bit.LOADAMODE = CC_CTR_ZERO; // Load on Zero
    ePWM[0]->CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

    // Set actions
    ePWM[0]->AQCTLA.bit.CAU = AQ_CLEAR;            // Set PWM1A on event A, up
    ePWM[0]->AQCTLA.bit.CAD = AQ_CLEAR;          // Clear PWM1A on event A,
    ePWM[0]->AQCTLB.bit.CBU = AQ_CLEAR;            // Set PWM1B on event B, up
    ePWM[0]->AQCTLB.bit.CBD = AQ_CLEAR;          // Clear PWM1B on event B,

    // Interrupt where we will change the Compare Values
    ePWM[0]->ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
    ePWM[0]->ETSEL.bit.INTEN = 1;                // Enable INT
    ePWM[0]->ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event
}
int initializeSystem(void)
{

    //
     // Step 1. Initialize System Control:
     // Enable Peripheral Clocks
     // This example function is found in the F2837xS_SysCtrl.c file.
     //
         InitSysCtrl();

     //
     //  Unlock CSM
     //
     //  If the API functions are going to run in unsecured RAM
     //  then the CSM must be unlocked in order for the flash
     //  API functions to access the flash.
     //  If the flash API functions are executed from secure memory
     //  then this step is not required.
     //
         //DcsmZ1Unlock();

     //
     // Step 2. Initialize GPIO:
     // This example function is found in the F2837xS_Gpio.c file and
     // illustrates how to set the GPIO to it's default state.
     //

         InitGpio();
         InitEPwm1Gpio();

         //XIN
         GPIO_SetupPinMux(44, GPIO_MUX_CPU1, 0);
         GPIO_SetupPinMux(45, GPIO_MUX_CPU1, 0);
         GPIO_SetupPinOptions(44, GPIO_INPUT, GPIO_PULLUP);
         GPIO_SetupPinOptions(45, GPIO_INPUT, GPIO_PULLUP);

         //LED
         GPIO_SetupPinMux(31, GPIO_MUX_CPU1, 0);
         GPIO_SetupPinMux(34, GPIO_MUX_CPU1, 0);
         GPIO_SetupPinOptions(31, GPIO_OUTPUT, GPIO_PUSHPULL);
         GPIO_SetupPinOptions(34, GPIO_OUTPUT, GPIO_PUSHPULL);

         //SCIB
         GPIO_SetupPinMux(55, GPIO_MUX_CPU1, 6);
         GPIO_SetupPinOptions(55, GPIO_INPUT, GPIO_PUSHPULL);
         GPIO_SetupPinMux(54, GPIO_MUX_CPU1, 6);
         GPIO_SetupPinOptions(54, GPIO_OUTPUT, GPIO_ASYNC);

     //    SCI_Init(&ScibRegs,115200);

     //
     // Step 3. Clear all interrupts and initialize PIE vector table:
     // Disable CPU interrupts
     //
         //InitEQep1();

            DINT;

        //
        // Initialize the PIE control registers to their default state.
        // The default state is all PIE interrupts disabled and flags
        // are cleared.
        // This function is found in the F2837xS_PieCtrl.c file.
        //
            InitPieCtrl();

        //
        // Disable CPU interrupts and clear all CPU interrupt flags:
        //
            IER = 0x0000;
            IFR = 0x0000;

        //
        // Initialize the PIE vector table with pointers to the shell Interrupt
        // Service Routines (ISR).
        // This will populate the entire table, even if the interrupt
        // is not used in this example.  This is useful for debug purposes.
        // The shell ISR routines are found in F2837xS_DefaultIsr.c.
        // This function is found in F2837xS_PieVect.c.
        //
            InitPieVectTable();
        //Map ISR functions
            EALLOW;
            PieVectTable.ADCA1_INT = &adca1_isr;
            //PieVectTable.TIMER0_INT = &cpu_timer0_isr;
            //PieVectTable.SCIB_RX_INT = &scibRxIsr;
            //PieVectTable.EPWM1_INT = &epwm1_isr;
            EDIS;
            //Configure the ADC and power it up
            ConfigureADC();

            //Configure the ePWM
            ConfigureEPWM();
            //Setup the ADC for ePWM triggered conversions on channel 0
            SetupADCEpwm();
            // CPU Timer 초기화
            InitCpuTimers();
        // CPU Timer0 을 3초 주기로 설정
        //    ConfigCpuTimer(&CpuTimer0, 200, (int)(1e-3/1e-6));

        // CPU Timer0 시작
         //   StartCpuTimer0();
        //Enable global Interrupts and higher priority real-time debug events:
            IER = 0x0000;
            IER |= M_INT1; //ADC,TIMER0
          //  IER |= M_INT3; //PWM
         //   IER |= M_INT9; //SCIB

            PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
            //PieCtrlRegs.PIEIER1.bit.INTx7 = 1;          // PIE 인터럽트(CPU Timer0) : Enable
            //PieCtrlRegs.PIEIER3.bit.INTx1 = 1;          //epwm1
            //PieCtrlRegs.PIEIER9.bit.INTx3 = 1;          //scib
            EINT;  // Enable Global interrupt INTM
            ERTM;  // Enable Global realtime interrupt DBGM
            //
            // sync ePWM
            //
                EALLOW;
                CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;

            return 0;

}

void SetupADCEpwm(void)
{
    Uint16 acqps;
    //
    //determine minimum acquisition window (in SYSCLKS) based on resolution
    //
    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14; //75ns
    }
    else //resolution is 16-bit
    {
        acqps = 63; //320ns
    }

    //
    //Select the channels to convert and end of conversion flag
    //
    EALLOW;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;          // SOC0 : ADCINA0
    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 1;          // SOC1 : ADCINA1
    AdcaRegs.ADCSOC2CTL.bit.CHSEL = 2;          // SOC2 : ADCINA2
    AdcaRegs.ADCSOC3CTL.bit.CHSEL = 3;          // SOC3 : ADCINA3
/*    AdcaRegs.ADCSOC4CTL.bit.CHSEL = 4;          // SOC4 : ADCINA4
    AdcaRegs.ADCSOC5CTL.bit.CHSEL = 5;          // SOC5 : ADCINA5
    AdcaRegs.ADCSOC6CTL.bit.CHSEL = 6;          // SOC6 : ADCINA6
    AdcaRegs.ADCSOC7CTL.bit.CHSEL = 7;          // SOC7 : ADCINA7*/

    AdcaRegs.ADCSOC0CTL.bit.ACQPS  = acqps;    //// (S/H 시간) 설정 75ns = (ACQPS+1)/SYSCLK
    AdcaRegs.ADCSOC1CTL.bit.ACQPS  = acqps;    //// (S/H 시간) 설정 75ns = (ACQPS+1)/SYSCLK
    AdcaRegs.ADCSOC2CTL.bit.ACQPS  = acqps;    //// (S/H 시간) 설정 75ns = (ACQPS+1)/SYSCLK
    AdcaRegs.ADCSOC3CTL.bit.ACQPS  = acqps;    //// (S/H 시간) 설정 75ns = (ACQPS+1)/SYSCLK
/*    AdcaRegs.ADCSOC4CTL.bit.ACQPS  = acqps;    //// (S/H 시간) 설정 75ns = (ACQPS+1)/SYSCLK
    AdcaRegs.ADCSOC5CTL.bit.ACQPS  = acqps;    //// (S/H 시간) 설정 75ns = (ACQPS+1)/SYSCLK
    AdcaRegs.ADCSOC6CTL.bit.ACQPS  = acqps;    //// (S/H 시간) 설정 75ns = (ACQPS+1)/SYSCLK
    AdcaRegs.ADCSOC7CTL.bit.ACQPS  = acqps;    //// (S/H 시간) 설정 75ns = (ACQPS+1)/SYSCLK*/

    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5;        // SOC0: ePWM1 SOCA/C로 트리거
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 5;        // SOC1: ePWM1 SOCA/C로 트리거
    AdcaRegs.ADCSOC2CTL.bit.TRIGSEL = 5;        // SOC2: ePWM1 SOCA/C로 트리거
    AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = 5;        // SOC3: ePWM1 SOCA/C로 트리거
/*    AdcaRegs.ADCSOC4CTL.bit.TRIGSEL = 5;        // SOC4: ePWM1 SOCA/C로 트리거
    AdcaRegs.ADCSOC5CTL.bit.TRIGSEL = 5;        // SOC5: ePWM1 SOCA/C로 트리거
    AdcaRegs.ADCSOC6CTL.bit.TRIGSEL = 5;        // SOC6: ePWM1 SOCA/C로 트리거
    AdcaRegs.ADCSOC7CTL.bit.TRIGSEL = 5;        // SOC7: ePWM1 SOCA/C로 트리거*/

    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; //end of SOC0 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    EDIS;
}
int GetInputData()
{
    int _inp=0x00;
    _inp |= GPIO_ReadPin(44) << 0x00;
    _inp |= GPIO_ReadPin(45) << 0x01;
    device.digitalInput=_inp;
    return 0x00;
}

