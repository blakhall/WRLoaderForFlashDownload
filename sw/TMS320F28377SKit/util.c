/*
 * invControl.c
 *
 *  Created on: 2016. 3. 22.
 *      Author: huni
 */

/*
 * modbus.c
 *
 *  Created on: 2012. 11. 4.
 *      Author: huni
 */
#include "util.h"
TIMEClock timeClock={0.};
SCI_PORT serialport0={{0},{0},0,0,0,0,0,0,0,0};
EPWM_INFO epwm1_info;


int RxFIFO(SCI_PORT* _s, struct  SCI_REGS portreg )
{

	Uint16 i;
	int data;
	int rxcnt;
	if(ScibRegs.SCIFFRX.bit.RXFFOVF==1)		//over Flow
	{
		ScibRegs.SCIFFRX.bit.RXFFOVRCLR=1;   // Clear Overflow flag
		ScibRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag
		return -1;
	}
	else
	{
		rxcnt=ScibRegs.SCIFFRX.bit.RXFFST;
		for(i=0;i<rxcnt;i++)
		{
		data = ScibRegs.SCIRXBUF.bit.SAR;

   		_s->rx_buffer[_s->rx_wr_index]=data;
   		if (++_s->rx_wr_index == RX_BUFFER_SIZE) _s->rx_wr_index=0;
   		if (++_s->rx_counter == RX_BUFFER_SIZE)
     	{
      		_s->rx_counter=0;
      		_s->rx_buffer_overflow=1;
     	}
		_s->rx_buffer[_s->rx_wr_index]=data;
		if(_s->rx_wr_index==RX_BUFFER_SIZE)_s->rx_wr_index=0;
		if(_s->rx_counter==RX_BUFFER_SIZE)
		{
			_s->rx_counter=0;
			_s->rx_buffer_overflow=1;
			return -2;
		}
		//portreg.SCITXBUF=rdataB[i];
		}
		ScibRegs.SCIFFRX.bit.RXFFINTCLR=1;   // Clear Interrupt flag
	}
	return 0;


}

char Kb_hit(SCI_PORT _s)
{
	//return rx_rd_index0!=rx_wr_index0;
	return _s.rx_rd_index != _s.rx_wr_index;
}

int Getch(SCI_PORT* _s)
{
	char data;
	data=_s->rx_buffer[_s->rx_rd_index];
	if (++_s->rx_rd_index == RX_BUFFER_SIZE) _s->rx_rd_index=0;
	--_s->rx_counter;
	return data;
}


//**********************************************************************************************
void SCI_Init(volatile struct SCI_REGS* sciRegs,long _baud)
//**********************************************************************************************
{
// SCI Registers Settings
//  ScibRegs.SCIFFTX.all= 0xa000;       // FIFO reset
    int baudrate=0;
    sciRegs->SCIFFTX.all = 0xE000;
    sciRegs->SCIFFRX.all = 0x206F;
    sciRegs->SCIFFCT.all= 0x0000;       // Clear ABD

    sciRegs->SCICCR.all = 0x7;      // 1 stop & no parity & 8bit char, no loopback, idle-line
    sciRegs->SCICTL1.all = 0x03;//BIT1 | BIT0;  // enable RX-ERR, TX, RX INT

//  baudrate = (int)(((float)LSP_CLK/(_baud*8.) - 1) + 0.5);
    baudrate = (int)(((float)50e6/(_baud*8.) - 1) + 0.5);

    sciRegs->SCIHBAUD.all = baudrate >> 8;
    sciRegs->SCILBAUD.all = baudrate & 0xFF;

//  ScibRegs.SCICTL2.all =  BIT1;           // enable RX/BK INT, TX INT
    sciRegs->SCICTL1.all =  0x24;//BIT5 | BIT1 | BIT0;  // Relinquish SCI from Reset


    //
//  ScibRegs.SCIFFTX.bit.TXFFIL = 8;
    sciRegs->SCIFFRX.bit.RXFFIL = 0x0A;

}



//**********************************************************************************************
void SCIB_Init(long _baud)
//**********************************************************************************************
{
// SCI Registers Settings
//	ScibRegs.SCIFFTX.all= 0xa000;		// FIFO reset
	int baudrate=0;
	ScibRegs.SCIFFTX.all = 0xE000;
	ScibRegs.SCIFFRX.all = 0x206F;
	ScibRegs.SCIFFCT.all= 0x0000;		// Clear ABD

	ScibRegs.SCICCR.all = 0x7;		// 1 stop & no parity & 8bit char, no loopback, idle-line
 	ScibRegs.SCICTL1.all = 0x03;//BIT1 | BIT0;	// enable RX-ERR, TX, RX INT

//	baudrate = (int)(((float)LSP_CLK/(_baud*8.) - 1) + 0.5);
	baudrate = (int)(((float)50e6/(_baud*8.) - 1) + 0.5);

	ScibRegs.SCIHBAUD.all = baudrate >> 8;
	ScibRegs.SCILBAUD.all = baudrate & 0xFF;

//	ScibRegs.SCICTL2.all =  BIT1;			// enable RX/BK INT, TX INT
	ScibRegs.SCICTL1.all =  0x24;//BIT5 | BIT1 | BIT0;	// Relinquish SCI from Reset


	//
//	ScibRegs.SCIFFTX.bit.TXFFIL = 8;
	ScibRegs.SCIFFRX.bit.RXFFIL = 0x0A;

}

int Putch(SCI_PORT* _s,int ch)
{
	int iTemp;
	iTemp=ch;
	_s->tx_buffer[_s->tx_wr_index++]=iTemp;
	if (_s->tx_wr_index == TX_BUFFER_SIZE) _s->tx_wr_index=0;
	if (++_s->tx_counter == TX_BUFFER_SIZE)
    {
    	_s->tx_counter=0;
    	_s->tx_buffer_overflow=1;
		return 1;
    }
	return 0;
}
void Move_TxBuf_to_UART(SCI_PORT* _s, struct  SCI_REGS portreg)
{
	if(_s->tx_wr_index != _s->tx_rd_index)
	{
		if(ScibRegs.SCIFFTX.bit.TXFFST <= 0x0A )
		{
			ScibRegs.SCITXBUF.bit.TXDT = _s->tx_buffer[_s->tx_rd_index++];
			if(_s->tx_rd_index==RX_BUFFER_SIZE)_s->tx_rd_index=0;
			--_s->tx_counter;
		}
	}
}

void Init_adc(void)
{

}

//============================================================================================
int pwmPulseWidth=0x10;
int pwmPriod=0x1000;
void jump_to_hardware_addr(Uint32 hard_addr)
{
    void (*p)(void);                  // Declare a local function pointer
    p = (void (*)(void))hard_addr;    // Assign the pointer address
    (*p)();                           // Call the function
}
void updatePWM(void)
{
	EPwm1Regs.CMPA.bit.CMPA=pwmPulseWidth;
	EPwm1Regs.TBPRD=pwmPriod;
}


