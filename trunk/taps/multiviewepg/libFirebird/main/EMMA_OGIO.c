#include "tap.h"

//OGIO = GIO INTERFACE
volatile dword *EMMA_OGIO_AC_CNT   = (dword *) 0xb0000800;
volatile dword *EMMA_OGIO_b0000808 = (dword *) 0xb0000808;
volatile dword *EMMA_OGIO_b000080c = (dword *) 0xb000080c;
volatile dword *EMMA_OGIO_CS0_BASE = (dword *) 0xb4000000;
volatile dword *EMMA_OGIO_CS1_BASE = (dword *) 0xb4400000;
