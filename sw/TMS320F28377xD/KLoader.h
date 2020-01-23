/*
 * KLoader.h
 *
 *  Created on: 2016. 3. 25.
 *      Author: huni
 */

#ifndef KLOADER_H_
#define KLOADER_H_
#include "util.h"
#include <string.h>
#include <F021_F2837xD_C28x.h>
#include "flash_programming_c28.h"

#define STARTCODE ':'
#define BASEADDRESS	0x80000


enum ProgramSubSequence
{
	stx,
	length,
	address,
	cmd,
	data,
	crc
};
typedef struct
{
	unsigned int address;
	uint32 extended;
	int length;
	int byteCount;
	int recordType;
	unsigned int data[0x50];
	unsigned int checkSum;
	enum ProgramSubSequence programSubSeq;
}IntelHexFormat;

enum LoaderSequence
{
	init,
	program,
	status,
	app
};

enum LoaderSubSequence
{
	startChar,
	command,
	endChar,
	jumpaddress
};

void SendFlashStatus(int _request);
void InitCheck(int _data);	// [S]=>program
int getData(void);
void Example_Error(Fapi_StatusType status);
void Example_Done(void);
void FlashTest1();
void FlashWrite1(uint16 *buffer);
void FlashErase();
void FlashProgramSEQ(int _data);

#endif /* KLOADER_H_ */
