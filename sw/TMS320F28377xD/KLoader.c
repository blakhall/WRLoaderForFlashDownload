/*
 * KLoader.c
 *
 *  Created on: 2016. 3. 25.
 *      Author: huni
 */
#include "KLoader.h"

//Include Flash API example header file
#define  WORDS_IN_FLASH_BUFFER    0xFF  // Programming data buffer, words
uint16   Buffer[WORDS_IN_FLASH_BUFFER + 1];
uint32   *Buffer32 = (uint32 *)Buffer;
enum LoaderSequence loaderSeq=init;
enum LoaderSubSequence loaderSubSeq=startChar ;
IntelHexFormat intelFormat;
extern uint16 flashBuffer[0x10];
int loaderBusSeq=0;
int receiveBuffer[0x10]={0,};
int getCnt=0;
float coffTestFloat;
uint32 jumpAddress=0x90000;
uint32 jumpAddressLength=0x8000;
int getData(void)
{
	int _ch;
	if(Kb_hit(serialport0))
	{
		_ch = Getch(&serialport0);
		switch(loaderSeq)
		{
		case init:
			InitCheck(_ch);
			break;
		case program:
			FlashProgramSEQ(_ch);
			break;
		default:
			break;
		}
	}
	if(timeClock.appTimer>1.0 && loaderSubSeq == jumpaddress)
	{
		loaderSubSeq = startChar;
        DINT;
        IER = 0x0000;
        IFR = 0x0000;
        DisableDog();
        jump_to_hardware_addr(jumpAddress);
	}
	return 0;
}

int flashData[0x10];
Fapi_StatusType            oReturnCheck;
volatile Fapi_FlashStatusType       oFlashStatus;
Fapi_FlashStatusWordType   oFlashStatusWord;

void FlashTest1()
{

    volatile Fapi_FlashStatusType       oFlashStatus;

    // Disable ECC.  ECC does not have to be disabled to do FSM operations like
    // program and erase.
    // However, on Sonata Rev. 0 silicon, due to an OTP ECC errata,
    // disable ECC to avoid ECC errors while using Flash API functions that
    // read TI-OTP
    EALLOW;
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0x0;
    EDIS;

    EALLOW;

    // This function is required to initialize the Flash API based on System
    // frequency before any other Flash API operation can be performed

	oReturnCheck = Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, 200);//for now keeping it out

    if(oReturnCheck != Fapi_Status_Success)
    {
        // Check Flash API documentation for possible errors
        Example_Error(oReturnCheck);
    }


}
void FlashErase()
{

    // Erase Sector C
    oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,
                   (uint32 *)jumpAddress);

    // Wait until FSM is done with erase sector operation
    while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady){}

    // Verify that SectorL is erased.  The Erase step itself does a
    // verify as it goes.  This verify is a 2nd verification that can be done.
    oReturnCheck = Fapi_doBlankCheck((uint32 *)jumpAddress,
    			jumpAddressLength,
                   &oFlashStatusWord);

}
uint32 u32Index = 0;
uint32 u32add=0;
uint32 u32length=1;
void FlashWrite1(uint16 *buffer)
{
	uint16 au16DataBuffer[8] = {0x0001, 0x0203, 0x0405, 0x0607, 0x0809, 0x0A0B, 0x0C0D, 0x0E0F};
	//Fapi_FlashBankType oActiveFlashBank = Fapi_FlashBank0;
	//Fapi_StatusType oReturnCheck;
	uint32 u32Index;
	int i;
	for(i=0,u32Index = 0x84000+u32add;(u32Index < 0x84000+u32add+u32length+8) && (oReturnCheck == Fapi_Status_Success);	i+=u32length,u32Index+=u32length)
	{
	//u32Index = 0x84000;
	oReturnCheck = Fapi_issueProgrammingCommand(
	(uint32 *) u32Index,
	au16DataBuffer+i,
	u32length,
	0,
	0,
//	Fapi_AutoEccGeneration);
	Fapi_DataOnly);
	while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);
	//oFlashStatusWord = Fapi_getFsmStatus();
	 oFlashStatus = Fapi_getFsmStatus();
	}

}

int32 offsetAddress=0x10000;
void FlashWrite()
{
	//Fapi_FlashBankType oActiveFlashBank = Fapi_FlashBank0;
	//Fapi_StatusType oReturnCheck;
	uint32 u32Index;
	int i;
	int ilength;
	for(i=0;i<intelFormat.length;i++)
	{
		intelFormat.data[i]= (intelFormat.data[i*2]<<0x08) + intelFormat.data[i*2+1];
	}
    for(i=0;i<intelFormat.length;)
	{
		if((intelFormat.length-i)>=8)ilength=8;
		else ilength=intelFormat.length-i;
		//ilength=8;
		ilength=0x01;
		u32Index = (intelFormat.extended << 0x10) + intelFormat.address+i+offsetAddress;//- 0x34000;
		oReturnCheck = Fapi_issueProgrammingCommand(
				(uint32 *) u32Index,
				intelFormat.data +i,
				ilength,
				0,
				0,
					//Fapi_AutoEccGeneration);
				Fapi_DataOnly);
		i += ilength;
		while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);
		//oFlashStatusWord = Fapi_getFsmStatus();
		oFlashStatus = Fapi_getFsmStatus();
	}

}
void FlashTest()
{
    uint32 u32Index = 0;
    uint16 i = 0;

    Fapi_StatusType            oReturnCheck;
    volatile Fapi_FlashStatusType       oFlashStatus;
    Fapi_FlashStatusWordType   oFlashStatusWord;

    // Disable ECC.  ECC does not have to be disabled to do FSM operations like
    // program and erase.
    // However, on Sonata Rev. 0 silicon, due to an OTP ECC errata,
    // disable ECC to avoid ECC errors while using Flash API functions that
    // read TI-OTP
    EALLOW;
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0x0;
    EDIS;

    EALLOW;

    // This function is required to initialize the Flash API based on System
    // frequency before any other Flash API operation can be performed

	oReturnCheck = Fapi_initializeAPI(F021_CPU0_BASE_ADDRESS, 200);//for now keeping it out

    if(oReturnCheck != Fapi_Status_Success)
    {
        // Check Flash API documentation for possible errors
        Example_Error(oReturnCheck);
    }

    // Fapi_setActiveFlashBank function sets the Flash bank and FMC for further
    // Flash operations to be performed on the bank
    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
    if(oReturnCheck != Fapi_Status_Success)
    {
        // Check Flash API documentation for possible errors
        Example_Error(oReturnCheck);
    }

    // Erase Sector C
    oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,
                   (uint32 *)Bzero_SectorC_start);

    // Wait until FSM is done with erase sector operation
    while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady){}

    // Verify that SectorL is erased.  The Erase step itself does a
    // verify as it goes.  This verify is a 2nd verification that can be done.
    oReturnCheck = Fapi_doBlankCheck((uint32 *)Bzero_SectorC_start,
                   Bzero_16KSector_u32length,
                   &oFlashStatusWord);

    if(oReturnCheck != Fapi_Status_Success)
    {
        // Check Flash API documentation for possible errors
        // If Erase command fails, use Fapi_getFsmStatus() function to get the
        // FMSTAT register contents to see if any of the EV bit, ESUSP bit,
        // CSTAT bit or VOLTSTAT bit is set (Refer to API documentation for
        // more details)
        Example_Error(oReturnCheck);
    }

    // Erase Sector B
    oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,
                   (uint32 *)Bzero_SectorB_start);

    // Wait until FSM is done with erase sector operation
    while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady){}
    // Verify that SectorB is erased.  The Erase step itself does a verify as
    // it goes.  This verify is a 2nd verification that can be done.
    oReturnCheck = Fapi_doBlankCheck((uint32 *)Bzero_SectorB_start,
                   Bzero_16KSector_u32length,
                   &oFlashStatusWord);

    if(oReturnCheck != Fapi_Status_Success)
    {
        // Check Flash API documentation for possible errors
        // If Erase command fails, use Fapi_getFsmStatus() function
        // to get the FMSTAT register contents
        // to see if any of the EV bit, ESUSP bit, CSTAT bit or VOLTSTAT
        // bit is set (Refer to API documentation for more details)
        Example_Error(oReturnCheck);
    }


    // A data buffer of max 8 words can be supplied to the program function.
    // Each word is programmed until the whole buffer is programmed or a
    // problem is found. However to program a buffer that has more than 8
    // words, program function can be called in a loop to program 8 words for
    // each loop iteration until the whole buffer is programmed


    // Example: Program 0xFF bytes in Flash Sector C along with auto-
    // generated ECC

    // In this case just fill a buffer with data to program into the flash.
    for(i=0;i<=WORDS_IN_FLASH_BUFFER;i++)
    {
        Buffer[i] = i;
    }

    for(i=0, u32Index = Bzero_SectorC_start;
       (u32Index < (Bzero_SectorC_start + WORDS_IN_FLASH_BUFFER))
       && (oReturnCheck == Fapi_Status_Success); i+= 8, u32Index+= 8)
    {
        oReturnCheck = Fapi_issueProgrammingCommand((uint32 *)u32Index,Buffer+i,
                       8,
                       0,
                       0,
                       Fapi_AutoEccGeneration);

        while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);

        if(oReturnCheck != Fapi_Status_Success)
        {
        // Check Flash API documentation for possible errors
        Example_Error(oReturnCheck);
        }

        // Read FMSTAT register contents to know the status of FSM after
        // program command for any debug
        oFlashStatus = Fapi_getFsmStatus();

        // Verify the values programmed.  The Program step itself does a verify
        // as it goes.  This verify is a 2nd verification that can be done.
        oReturnCheck = Fapi_doVerify((uint32 *)u32Index,
                       4,
                       Buffer32+(i/2),
                       &oFlashStatusWord);
        if(oReturnCheck != Fapi_Status_Success)
        {
            // Check Flash API documentation for possible errors

            Example_Error(oReturnCheck);
        }
    }

    // Example:  Program 0xFF bytes in Flash Sector B with out ECC
    // Disable ECC so that error is not generated when reading Flash contents
    // without ECC
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0x0;

    for(i=0, u32Index = Bzero_SectorB_start;
       (u32Index < (Bzero_SectorB_start + WORDS_IN_FLASH_BUFFER))
       && (oReturnCheck == Fapi_Status_Success); i+= 8, u32Index+= 8)
    {
        oReturnCheck = Fapi_issueProgrammingCommand((uint32 *)u32Index,
                       Buffer+i,
                       8,
                       0,
                       0,
                       Fapi_DataOnly);
        while(Fapi_checkFsmForReady() == Fapi_Status_FsmBusy);

        if(oReturnCheck != Fapi_Status_Success)
        {
            // Check Flash API documentation for possible errors
            Example_Error(oReturnCheck);
        }

        // Read FMSTAT register contents to know the status of FSM
        // after program command for any debug
        oFlashStatus = Fapi_getFsmStatus();

        // Verify the values programmed.  The Program step itself does a verify
        // as it goes.  This verify is a 2nd verification that can be done.
        oReturnCheck = Fapi_doVerify((uint32 *)u32Index,
                       4,
                       Buffer32+(i/2),
                       &oFlashStatusWord);
        if(oReturnCheck != Fapi_Status_Success)
        {
            // Check Flash API documentation for possible errors
            Example_Error(oReturnCheck);
        }
    }

    // Erase the sectors that we have programmed above
    // Erase Sector C
    oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,
                   (uint32 *)Bzero_SectorC_start);

    // Wait until FSM is done with erase sector operation
    while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady){}

    // Verify that SectorC is erased.  The Erase step itself does a verify as
    // it goes.
    // This verify is a 2nd verification that can be done.
    oReturnCheck = Fapi_doBlankCheck((uint32 *)Bzero_SectorC_start,
                   Bzero_16KSector_u32length,
                   &oFlashStatusWord);

    if(oReturnCheck != Fapi_Status_Success)
    {
        // Check Flash API documentation for possible errors
        // If Erase command fails, use Fapi_getFsmStatus() function to get the
        // FMSTAT register contents
        // to see if any of the EV bit, ESUSP bit, CSTAT bit or VOLTSTAT bit is
        // set (Refer to API documentation for more details)
        Example_Error(oReturnCheck);
    }

    // Erase Sector B
    oReturnCheck = Fapi_issueAsyncCommandWithAddress(Fapi_EraseSector,
                   (uint32 *)Bzero_SectorB_start);
    // Wait until FSM is done with erase sector operation
    while (Fapi_checkFsmForReady() != Fapi_Status_FsmReady){}
    // Verify that SectorB is erased.  The Erase step itself does a verify
    // as it goes.  This verify is a 2nd verification that can be done.
    oReturnCheck = Fapi_doBlankCheck((uint32 *)Bzero_SectorB_start,
                   Bzero_16KSector_u32length,
                   &oFlashStatusWord);

    if(oReturnCheck != Fapi_Status_Success)
    {
        // Check Flash API documentation for possible errors
        // If Erase command fails, use Fapi_getFsmStatus() function to get
        // the FMSTAT register contents to see if any of the EV bit, ESUSP bit,
        // CSTAT bit or VOLTSTAT bit is set (Refer to API documentation
        // for more details)
        Example_Error(oReturnCheck);
    }

    // Enable ECC
    Flash0EccRegs.ECC_ENABLE.bit.ENABLE = 0xA;

    EDIS;

    // Example is done here
    Example_Done();
}
//******************************************************************************
// For this example, if an error is found just stop here
//******************************************************************************
#pragma CODE_SECTION(Example_Error,"ramfuncs");
void Example_Error(Fapi_StatusType status)
{
    //  Error code will be in the status parameter
        __asm("    ESTOP0");
}

//******************************************************************************
//  For this example, once we are done just stop here
//******************************************************************************
#pragma CODE_SECTION(Example_Done,"ramfuncs");
void Example_Done(void)
{
    __asm("    ESTOP0");
}

void InitCheck(int _data)	// [S]=>program
{
	int _ch=_data;
	switch(loaderSubSeq)
	{
		case startChar:
			if(_ch=='[')loaderSubSeq = command;
			receiveBuffer[0x00]= _ch;
			break;
		case command:
			if(_ch=='S'||_ch=='I' || _ch=='G')loaderSubSeq = endChar;
			else loaderSubSeq = startChar;
			receiveBuffer[0x01]= _ch;
			break;
		case endChar:
			if(_ch==']')
			{
				if(receiveBuffer[0x01]=='S')
				{
					SendFlashStatus('R');
					FlashTest1();
					FlashErase();
					loaderSeq = program;
				}
				else if(receiveBuffer[0x01]=='I')
				{
					SendFlashStatus('I');
					//loaderSeq = status;
				}
				else if(receiveBuffer[0x01]=='G') //program go
				{
					SendFlashStatus('R');
					timeClock.appTimer =0.0;
					loaderSubSeq = jumpaddress;
					break;
				}
			}
			loaderSubSeq = startChar;
			break;

		case jumpaddress:
			break;
		default:

			break;

	}

}


void FlashProgramSEQ(int _data)	// [S]=>program
{
	unsigned int _ch=_data;
	switch(intelFormat.programSubSeq)
	{
		case stx:
			if(_ch==':')intelFormat.programSubSeq = length;
			receiveBuffer[0x00]= _ch;
			break;
		case length:
			intelFormat.length=_data/2;
			intelFormat.programSubSeq = address;
			intelFormat.byteCount=0x00;
			intelFormat.checkSum = 0x00;
			break;
		case address:
			intelFormat.data[intelFormat.byteCount++] = _ch;
			if(intelFormat.byteCount>=2)
			{
				intelFormat.programSubSeq = cmd;
				intelFormat.address=(intelFormat.data[0x00]<<0x08) + intelFormat.data[0x01];
			}
			break;
		case cmd:
			intelFormat.recordType=_ch;
			if(_data==0x01)intelFormat.programSubSeq = crc;//end
			else intelFormat.programSubSeq = data;
			intelFormat.byteCount=0x00;
			break;
		case data:
			intelFormat.data[intelFormat.byteCount++] = _ch;
			if(intelFormat.byteCount==intelFormat.length*2) intelFormat.programSubSeq = crc;
			break;
		case crc:
			_ch = ~_ch+1;
			if((_ch%0x100) == (intelFormat.checkSum%0x100) )
			{
				switch(intelFormat.recordType)
				{
					case 0x00:
						FlashWrite();
						SendFlashStatus('R');
					break;
					case 0x01:
						loaderSeq = init;
						SendFlashStatus('R');
					break;
					case 0x04:
						intelFormat.extended = (intelFormat.data[0]<<0x10)+intelFormat.data[1];
						SendFlashStatus('R');
						intelFormat.data[4]=0x1234;
					break;
					default:
						SendFlashStatus('E');
						loaderSeq = init;
						break;

				}

			}
			else
				{
				SendFlashStatus('E');
				intelFormat.data[0]=_ch;
				intelFormat.data[1]=intelFormat.checkSum;
				intelFormat.data[2]=(_ch%0x100);
				intelFormat.data[3]=(intelFormat.checkSum%0x100);
				loaderSeq = init;
				}
			intelFormat.programSubSeq = stx;
			break;
		default:
			break;

	}
	intelFormat.checkSum +=_data;

}
void SendFlashStatus(int _request)
{
	if(_request=='R'|| _request=='E')
	{
		Putch(&serialport0,'[');
		Putch(&serialport0,_request);
		Putch(&serialport0,']');
	}
	else if(_request=='I')
	{
		Putch(&serialport0,'[');
		Putch(&serialport0,'I');
		Putch(&serialport0,']');

	}

}
