#include "tap.h"

//IIC Bus Controller 1
//Communicates with the EEPROM (addresses 0xA0/0xA1), the A/V-Matrix (addresses 0x96/0x97)
//  and the first tuner (addresses 0x../0x..), Modulator?
volatile dword *EMMA_IIC1_00         = (dword *) 0xb2007000;
volatile dword *EMMA_IIC1_10         = (dword *) 0xb2007010;
volatile dword *EMMA_IIC1_20         = (dword *) 0xb2007020;
volatile dword *EMMA_IIC1_Data       = (dword *) 0xb2007040;
volatile dword *EMMA_IIC1_Cmd_Status = (dword *) 0xb2007050;
volatile dword *EMMA_IIC1_60         = (dword *) 0xb2007060;

//IIC Bus Controller 2
//Communicates with the second tuner (addresses 0x../0x..)
volatile dword *EMMA_IIC2_00         = (dword *) 0xb2008000;
volatile dword *EMMA_IIC2_10         = (dword *) 0xb2008010;
volatile dword *EMMA_IIC2_20         = (dword *) 0xb2008020;
volatile dword *EMMA_IIC2_Data       = (dword *) 0xb2008040;
volatile dword *EMMA_IIC2_Cmd_Status = (dword *) 0xb2008050;
volatile dword *EMMA_IIC2_60         = (dword *) 0xb2008060;
