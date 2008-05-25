#include "tap.h"

volatile dword *EMMA_PIO1_Status = (dword *) 0xb2010c10;
volatile dword *EMMA_PIO2_Status = (dword *) 0xb2010c14;
