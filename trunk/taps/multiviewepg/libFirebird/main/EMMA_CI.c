#include "tap.h"

//CI = Common Interface
volatile dword *EMMA_CI_b4020000 = (dword *) 0xb4020000;
volatile dword *EMMA_CI_b4040000 = (dword *) 0xb4040000;
volatile dword *EMMA_CI_Slot1    = (dword *) 0xb4080000;
volatile dword *EMMA_CI_b40a0000 = (dword *) 0xb40a0000;
volatile dword *EMMA_CI_Slot2    = (dword *) 0xb40c0000;
volatile dword *EMMA_CI_b40e0000 = (dword *) 0xb40e0000;
