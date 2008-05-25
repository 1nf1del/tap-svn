#include "tap.h"

volatile dword *EMMA_RTC_Timer_Command = (dword *) 0xb2010400;
volatile dword *EMMA_RTC_Timer_Mode    = (dword *) 0xb2010404;
volatile dword *EMMA_RTC_Timer_Value   = (dword *) 0xb2010408;
volatile dword *EMMA_RTC_IntReason     = (dword *) 0xb2010410;
volatile dword *EMMA_RTC_Event_Action  = (dword *) 0xb2010414;
volatile dword *EMMA_RTC_Compare       = (dword *) 0xb2010418;
