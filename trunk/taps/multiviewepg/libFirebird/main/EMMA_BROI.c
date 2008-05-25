#include "tap.h"

//BROI = ROM Interface
volatile dword *EMMA_BROI_FN_FSM  = (dword *) 0xb0000400;
volatile dword *EMMA_BROI_OAT_FAT = (dword *) 0xb0000410;
volatile dword *EMMA_BROI_WErr_WM = (dword *) 0xb0000420;
