#include "tap.h"

volatile dword *EMMA_PIO1_PortData      = (dword *) 0xb2010c00;
volatile dword *EMMA_PIO2_PortData      = (dword *) 0xb2010c04;
volatile dword *EMMA_PIO1_PortDirection = (dword *) 0xb2010c08;
volatile dword *EMMA_PIO2_PortDirection = (dword *) 0xb2010c0c;

volatile dword *EMMA_PIO1_Mode          = (dword *) 0xb2010c18;
volatile dword *EMMA_PIO2_Mode          = (dword *) 0xb2010c1c;
volatile dword *EMMA_PIO1_ConditionA    = (dword *) 0xb2010c20;
volatile dword *EMMA_PIO2_ConditionA    = (dword *) 0xb2010c24;
volatile dword *EMMA_PIO1_ConditionB    = (dword *) 0xb2010c28;
volatile dword *EMMA_PIO2_ConditionB    = (dword *) 0xb2010c2c;
