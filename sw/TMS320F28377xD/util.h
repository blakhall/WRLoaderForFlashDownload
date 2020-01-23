/*
 * util.h
 *
 *  Created on: 2016. 3. 22.
 *      Author: huni
 */

#ifndef UTIL_H_
#define UTIL_H_

#include "F28x_Project.h"
//#include "inc/hw_types.h"
//#include "inc/hw_memmap.h"
//#include "inc/hw_can.h"
//#include "driverlib/can.h""
//#include "DSP2833x_Sci.h"

#define TX_BUFFER_SIZE	0x100
#define RX_BUFFER_SIZE	0x100


#define EPWM1_TIMER_TBPRD  2000  // Period register
#define EPWM1_MAX_CMPA     1950
#define EPWM1_MIN_CMPA       50
#define EPWM1_MAX_CMPB     1950
#define EPWM1_MIN_CMPB       50

typedef struct
{
	int rx_buffer[RX_BUFFER_SIZE];
	int tx_buffer[TX_BUFFER_SIZE];
	int rx_buffer_overflow;
	int tx_buffer_overflow;
	int tx_wr_index;
	int tx_rd_index;
	int rx_rd_index;
	int rx_wr_index;
	int tx_counter;
	int rx_counter;

} SCI_PORT;

typedef struct
{
	double frequency;

}PWMMODULE;
typedef struct
{
    volatile struct EPWM_REGS *EPwmRegHandle;
    Uint16 EPwm_CMPA_Direction;
    Uint16 EPwm_CMPB_Direction;
    Uint16 EPwmTimerIntCount;
    Uint16 EPwmMaxCMPA;
    Uint16 EPwmMinCMPA;
    Uint16 EPwmMaxCMPB;
    Uint16 EPwmMinCMPB;
}EPWM_INFO;


typedef struct
{
	double appTimer;

}TIMEClock;


/*인터럽트 함수*/
interrupt void epwm1_isr(void);

/*외부변수*/
extern TIMEClock timeClock;
extern SCI_PORT serialport0;
extern volatile struct SCI_REGS ScibRegs;

/* 함수 */
int RxFIFO(SCI_PORT* _s,struct SCI_REGS portreg );
int Getch(SCI_PORT* _s);
char Kb_hit(SCI_PORT _s);
int Putch(SCI_PORT* _s,int ch);
void Move_TxBuf_to_UART(SCI_PORT* _s, struct  SCI_REGS portreg);
void SCIB_Init(long _baud);
void jump_to_hardware_addr(Uint32 hard_addr);
void Init_adc(void);
void updatePWM(void);
#endif /* UTIL_H_ */
