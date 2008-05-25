#include "tap.h"

volatile dword *EMMA_TMR1_Timer_Command = (dword *) 0xb2010200;
volatile dword *EMMA_TMR1_Timer_Mode    = (dword *) 0xb2010204;
volatile dword *EMMA_TMR1_Timer_Value   = (dword *) 0xb2010208;
volatile dword *EMMA_TMR1_Int_Status    = (dword *) 0xb2010210;
volatile dword *EMMA_TMR1_Event_Action  = (dword *) 0xb2010214;
volatile dword *EMMA_TMR1_Compare       = (dword *) 0xb2010218;

//TMR2 is not used by the Toppy
volatile dword *EMMA_TMR2_Timer_Command = (dword *) 0xb2010240;
volatile dword *EMMA_TMR2_Timer_Mode    = (dword *) 0xb2010244;
volatile dword *EMMA_TMR2_Timer_Value   = (dword *) 0xb2010248;
volatile dword *EMMA_TMR2_Int_Status    = (dword *) 0xb2010250;
volatile dword *EMMA_TMR2_Event_Action  = (dword *) 0xb2010254;
volatile dword *EMMA_TMR2_Compare       = (dword *) 0xb2010258;
