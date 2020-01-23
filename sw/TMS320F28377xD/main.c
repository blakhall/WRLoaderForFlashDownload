//###########################################################################
// FILE:   blinky_cpu01.c
// TITLE:  LED Blink Example for F2837xD.
//
//! \addtogroup cpu01_example_list
//! <h1> Blinky </h1>
//!
//! This example blinks LED X
//
//###########################################################################
// $TI Release: F2837xD Support Library v180 $
// $Release Date: Fri Nov  6 16:19:46 CST 2015 $
// $Copyright: Copyright (C) 2013-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "util.h"
#include "KLoader.h"
#define RESULTS_BUFFER_SIZE 256


/* Define for this example */
#define	ENCODER_REV		24			/* Pulse/Revolution */
#define SYSTEM_FREQ		150E6		/* Hz */
#define UTIME_FREQ		2			/* Hz */
void InitEQep1(void);
void ConfigureADC(void);
void ConfigureEPWM(void);
void SetupADCEpwm();
void jump_to_hardware_addr(Uint32 hard_addr);
interrupt void adca1_isr(void);
interrupt void cpu_timer0_isr(void);
interrupt void scibRxIsr(void);
extern SCI_PORT serialport0;

//buffer for storing conversion results
Uint16 AdcaResults[RESULTS_BUFFER_SIZE];
Uint16 resultsIndex;
volatile Uint16 bufferFull;
int mainCnt;
int eqepCnt=0;
int eqepA=0;
int eqepB=0;
int eqepIs=0;
int eqepIi=0;
int digitalInput;
int iTemp;
int sciCnt=0;
double timer0Cnt[0x10]={0,};
int flashTestFlag=0;
uint16 *flashReadAddress;
uint16 flashBuffer[0x10];
int flashcnt=0;
int flash_buf[0x04];
float ADCEXOffset;
float ADCEXOffset1;
uint32 addressoffet=0;
void main(void)
{
// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the F2837xD_SysCtrl.c file.
    InitSysCtrl();

// Step 2. Initialize GPIO:
// This example function is found in the F2837xD_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
    CpuSysRegs.PCLKCR2.bit.EPWM1=1;
    InitGpio();
    InitEQep1Gpio();
    InitEPwmGpio();
    GPIO_SetupPinMux(31, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinMux(34, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(31, GPIO_OUTPUT, GPIO_PUSHPULL);
    GPIO_SetupPinOptions(34, GPIO_OUTPUT, GPIO_PUSHPULL);
    //SCIB
	GPIO_SetupPinMux(55, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinOptions(55, GPIO_INPUT, GPIO_PUSHPULL);
	GPIO_SetupPinMux(54, GPIO_MUX_CPU1, 6);
	GPIO_SetupPinOptions(54, GPIO_OUTPUT, GPIO_ASYNC);
	//END SCIB
    InitEQep1();
    SCIB_Init(115200);
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
    DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the F2837xD_PieCtrl.c file.
    InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xD_DefaultIsr.c.
// This function is found in F2837xD_PieVect.c.
    InitPieVectTable();
    //Map ISR functions
        EALLOW;
        PieVectTable.ADCA1_INT = &adca1_isr; //function for ADCA interrupt 1
    	PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    	//PieVectTable.SCIRXINTB = &scibRxIsr;
    	PieVectTable.SCIB_RX_INT = &scibRxIsr;
    	PieVectTable.EPWM1_INT = &epwm1_isr;
        EDIS;

    //Configure the ADC and power it up
        ConfigureADC();

    //Configure the ePWM
        ConfigureEPWM();
//        InitEPwm();

        //Setup the ADC for ePWM triggered conversions on channel 0
            SetupADCEpwm(0);
       // CPU Timer 초기화
       	InitCpuTimers();
    	// CPU Timer0 을 3초 주기로 설정
    	ConfigCpuTimer(&CpuTimer0, 200, (int)(1e-3/1e-6));

    	// CPU Timer0 시작
    	StartCpuTimer0();
        //Enable global Interrupts and higher priority real-time debug events:
            IER |= M_INT1; //Enable group 1 interrupts
        	IER |= M_INT3;


// Step 6. IDLE loop. Just sit and loop forever (optional):
    //enable PIE interrupt
        PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
    	PieCtrlRegs.PIEIER1.bit.INTx7 = 1;			// PIE 인터럽트(CPU Timer0) : Enable
    	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;			//epwm1


    //sync ePWM
        EALLOW;
        CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
        EPwm1Regs.ETSEL.bit.SOCAEN = 1;  //enable SOCA
        EDIS;
        // Enable global Interrupts and higher priority real-time debug events:
            EINT;  // Enable Global interrupt INTM
            ERTM;  // Enable Global realtime interrupt DBGM
            asm(" nop");
    for(;;)
    {

    	mainCnt++;
		flash_buf[0]= (int)(ADCEXOffset*100.0)%0x100;
		flash_buf[1]= (int)(ADCEXOffset*100.0)/0x100;
		flash_buf[2] = flash_buf[0];
		flash_buf[3] = ( flash_buf[1]<< 8) + flash_buf[2];
		ADCEXOffset1 = flash_buf[3]/100.0;
		//updatePWM();
//    	if(GpioDataRegs.GPBDAT.bit.GPIO44)GPIO_WritePin(34, 1);
  //  	else GPIO_WritePin(34, 0);
    	eqepA = GpioDataRegs.GPBDAT.bit.GPIO50;
    	eqepB = GpioDataRegs.GPBDAT.bit.GPIO51;
    	eqepIs = GpioDataRegs.GPBDAT.bit.GPIO52;
    	eqepIi = GpioDataRegs.GPBDAT.bit.GPIO53;
    	eqepCnt = EQep1Regs.QPOSCNT;
    	iTemp=GpioDataRegs.GPBDAT.bit.GPIO45;
    	iTemp =GpioDataRegs.GPBDAT.bit.GPIO44 + (iTemp << 1);
    	digitalInput = iTemp;
    	if(GpioDataRegs.GPBDAT.bit.GPIO44) //SW2
    	{
    		jump_to_hardware_addr(0x80000);
    	}
    	else if(GpioDataRegs.GPBDAT.bit.GPIO45) //SW1
    	{
    		jump_to_hardware_addr(0x90000);

    	}
    	if(flashTestFlag==1) // init
    	{
    		FlashTest1();
    	}
    	else if(flashTestFlag==2)//read
		{
    	    flashReadAddress = (uint16*)addressoffet;
    		for(flashcnt=0;flashcnt<0x10;flashcnt++)
    		{
    			flashBuffer[flashcnt]=*(flashReadAddress+flashcnt);
    		}

    	}
    	else if(flashTestFlag==3)//write
		{
    		FlashWrite1(flashBuffer);
    	}
		else if(flashTestFlag==4)//erase
		{
			FlashErase();
		}
    	flashTestFlag=0;
    	//if(Kb_hit(serialport0))getData(Getch(&serialport0));
    	getData();
		RxFIFO(&serialport0, ScibRegs );
		//if(Kb_hit(serialport0))Putch(&serialport0,Getch(&serialport0));
		Move_TxBuf_to_UART(&serialport0,ScibRegs);
    	if(timer0Cnt[0]>0.2)
    	{
    		//Putch(&serialport0,sciCnt++);
        	GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1;
        	timer0Cnt[0]=0.0;
    	}

    }
}
void InitEQep1(void)
{
	EQep1Regs.QUPRD = (SYSTEM_FREQ/UTIME_FREQ);	/* Unit Timer for 2Hz at 150 MHz SYSCLKOUT */

	EQep1Regs.QDECCTL.bit.QSRC = 0;			/* Quadrature count mode */
	EQep1Regs.QEPCTL.bit.FREE_SOFT = 1;
	EQep1Regs.QEPCTL.bit.UTE = 1; 			/* Unit Timer Enable */
	EQep1Regs.QEPCTL.bit.QCLM = 1; 			/* Position counter latch on unit time out */
	EQep1Regs.QEPCTL.bit.PCRM = 1;			/* Position Counter Reset on Maximum Position */
	EQep1Regs.QPOSMAX = 4*24;		/* 24 pulse @ 1 revolution */
	EQep1Regs.QEPCTL.bit.QPEN = 1; 			/* QEP enable */
	EQep1Regs.QEINT.bit.UTO = 1;			/* Unit Time Out Interrupt Enable */
}

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
	EPwm1Regs.ETSEL.bit.SOCAEN	= 0;	        // Disable SOC on A group
	EPwm1Regs.ETSEL.bit.SOCASEL	= 4;	        // Select SOC on up-count
	EPwm1Regs.ETPS.bit.SOCAPRD = 1;		        // Generate pulse on 1st event
	EPwm1Regs.CMPA.bit.CMPA = 0x0800;          // Set compare A value to 2048 counts
	EPwm1Regs.TBPRD = 0x1000;			        // Set period to 4096 counts
	EPwm1Regs.TBCTL.bit.CTRMODE = 3;            // freeze counter
	EDIS;
}

void SetupADCEpwm(void)
{
	Uint16 acqps;

	//determine minimum acquisition window (in SYSCLKS) based on resolution
	if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION){
		acqps = 14; //75ns
	}
	else { //resolution is 16-bit
		acqps = 63; //320ns
	}

	//Select the channels to convert and end of conversion flag
	EALLOW;
	AdcaRegs.ADCSOC0CTL.bit.CHSEL = 0;  //SOC0 will convert pin A0
	AdcaRegs.ADCSOC2CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
	AdcaRegs.ADCSOC3CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA/C
	AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; //end of SOC0 will set INT1 flag
	AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
	AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
	EDIS;

}

int adcCnt=0;
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
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}
int scibRxCnt=0;
interrupt void scibRxIsr(void)
{
	scibRxCnt++;
	RxFIFO(&serialport0, ScibRegs );
	PieCtrlRegs.PIEACK.all |= PIEACK_GROUP9;       // Issue PIE ack
}

//============================================================================================
//	ISR 함수 정의
//--------------------------------------------------------------------------------------------
int CPU_Timer0_Isr_cnt;
interrupt void cpu_timer0_isr(void)
{
	int i;
	CPU_Timer0_Isr_cnt++;
	timeClock.appTimer += 1e-3;
	for(i=0;i<0x10;i++)timer0Cnt[i]+=1e-3;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

int epwm1Cnt=0;
interrupt void epwm1_isr(void)
{

	epwm1Cnt++;
	// Clear INT flag for this timer
    EPwm1Regs.ETCLR.bit.INT = 1;

    // Acknowledge this interrupt to receive more interrupts from group 3
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

}
