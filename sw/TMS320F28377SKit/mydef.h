/*
 * mydef.h
 *
 *  Created on: 2016. 4. 24.
 *      Author: huni
 */

#ifndef MYDEF_H_
#define MYDEF_H_
#include "F28x_Project.h"
#include "util.h"

typedef struct
{
    int digitalInput;
}DEVICE;

extern DEVICE device;
void ConfigureADC(void);
void SetupADCEpwm(void);
void ConfigureEPWM(void);

void InitEPwmK();
int GetInputData();
int initializeSystem(void);



#endif /* MYDEF_H_ */
