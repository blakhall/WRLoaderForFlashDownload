################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Add inputs and outputs from these tool invocations to the build variables 
CMD_SRCS += \
../2837xD_FLASH_lnk_cpu1.cmd \
../F2837xD_Headers_nonBIOS_cpu1.cmd 

LIB_SRCS += \
../F021_API_F2837xD_FPU32.lib 

ASM_SRCS += \
C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_CodeStartBranch.asm \
C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_usDelay.asm 

C_SRCS += \
../F2837xD_Adc.c \
../F2837xD_CpuTimers.c \
C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_DefaultISR.c \
../F2837xD_EPwm.c \
../F2837xD_EQep.c \
C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_headers/source/F2837xD_GlobalVariableDefs.c \
C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_Gpio.c \
C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_Ipc.c \
C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_PieCtrl.c \
C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_PieVect.c \
C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_SysCtrl.c \
C:/ti/controlSUITE/device_support/F2837xS/v180/F2837xS_examples_Cpu1/flash_programming/cpu01/Fapi_UserDefinedFunctions.c \
../KLoader.c \
../main.c \
../system.c \
../util.c 

C_DEPS += \
./F2837xD_Adc.d \
./F2837xD_CpuTimers.d \
./F2837xD_DefaultISR.d \
./F2837xD_EPwm.d \
./F2837xD_EQep.d \
./F2837xD_GlobalVariableDefs.d \
./F2837xD_Gpio.d \
./F2837xD_Ipc.d \
./F2837xD_PieCtrl.d \
./F2837xD_PieVect.d \
./F2837xD_SysCtrl.d \
./Fapi_UserDefinedFunctions.d \
./KLoader.d \
./main.d \
./system.d \
./util.d 

OBJS += \
./F2837xD_Adc.obj \
./F2837xD_CodeStartBranch.obj \
./F2837xD_CpuTimers.obj \
./F2837xD_DefaultISR.obj \
./F2837xD_EPwm.obj \
./F2837xD_EQep.obj \
./F2837xD_GlobalVariableDefs.obj \
./F2837xD_Gpio.obj \
./F2837xD_Ipc.obj \
./F2837xD_PieCtrl.obj \
./F2837xD_PieVect.obj \
./F2837xD_SysCtrl.obj \
./F2837xD_usDelay.obj \
./Fapi_UserDefinedFunctions.obj \
./KLoader.obj \
./main.obj \
./system.obj \
./util.obj 

ASM_DEPS += \
./F2837xD_CodeStartBranch.d \
./F2837xD_usDelay.d 

OBJS__QUOTED += \
"F2837xD_Adc.obj" \
"F2837xD_CodeStartBranch.obj" \
"F2837xD_CpuTimers.obj" \
"F2837xD_DefaultISR.obj" \
"F2837xD_EPwm.obj" \
"F2837xD_EQep.obj" \
"F2837xD_GlobalVariableDefs.obj" \
"F2837xD_Gpio.obj" \
"F2837xD_Ipc.obj" \
"F2837xD_PieCtrl.obj" \
"F2837xD_PieVect.obj" \
"F2837xD_SysCtrl.obj" \
"F2837xD_usDelay.obj" \
"Fapi_UserDefinedFunctions.obj" \
"KLoader.obj" \
"main.obj" \
"system.obj" \
"util.obj" 

C_DEPS__QUOTED += \
"F2837xD_Adc.d" \
"F2837xD_CpuTimers.d" \
"F2837xD_DefaultISR.d" \
"F2837xD_EPwm.d" \
"F2837xD_EQep.d" \
"F2837xD_GlobalVariableDefs.d" \
"F2837xD_Gpio.d" \
"F2837xD_Ipc.d" \
"F2837xD_PieCtrl.d" \
"F2837xD_PieVect.d" \
"F2837xD_SysCtrl.d" \
"Fapi_UserDefinedFunctions.d" \
"KLoader.d" \
"main.d" \
"system.d" \
"util.d" 

ASM_DEPS__QUOTED += \
"F2837xD_CodeStartBranch.d" \
"F2837xD_usDelay.d" 

C_SRCS__QUOTED += \
"../F2837xD_Adc.c" \
"../F2837xD_CpuTimers.c" \
"C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_DefaultISR.c" \
"../F2837xD_EPwm.c" \
"../F2837xD_EQep.c" \
"C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_headers/source/F2837xD_GlobalVariableDefs.c" \
"C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_Gpio.c" \
"C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_Ipc.c" \
"C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_PieCtrl.c" \
"C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_PieVect.c" \
"C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_SysCtrl.c" \
"C:/ti/controlSUITE/device_support/F2837xS/v180/F2837xS_examples_Cpu1/flash_programming/cpu01/Fapi_UserDefinedFunctions.c" \
"../KLoader.c" \
"../main.c" \
"../system.c" \
"../util.c" 

ASM_SRCS__QUOTED += \
"C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_CodeStartBranch.asm" \
"C:/ti/controlSUITE/device_support/F2837xD/v180/F2837xD_common/source/F2837xD_usDelay.asm" 


