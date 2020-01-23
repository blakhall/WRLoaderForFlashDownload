/*
 * system.c
 *
 *  Created on: 2016. 4. 24.
 *      Author: huni
 */
#include "mydef.h"


volatile struct EPWM_REGS *ePWM[] ={ &EPwm1Regs,
									 &EPwm2Regs,
									 &EPwm3Regs,
									 &EPwm4Regs,
									 &EPwm5Regs,
									 &EPwm6Regs};

void InitEPwm()
{
    // Setup TBCLK
	ePWM[0]->TBPRD = 150e6/150e6;          // Set timer period 801 TBCLKs
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
