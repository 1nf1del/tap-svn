#include "tap.h"

//ZCCT = Capture Timers
volatile dword *EMMA_ZCCT1_TIMER_CMD    = (dword *) 0xb2010800;
volatile dword *EMMA_ZCCT1_TIMER_MODE   = (dword *) 0xb2010804;
volatile dword *EMMA_ZCCT1_TIMER_VALUE  = (dword *) 0xb2010808;
volatile dword *EMMA_ZCCT1_TIMER_STATUS = (dword *) 0xb201080c;
volatile dword *EMMA_ZCCT1_INT_STATUS   = (dword *) 0xb2010810;
volatile dword *EMMA_ZCCT1_EVENT_ACT    = (dword *) 0xb2010814;
volatile dword *EMMA_ZCCT1_CAPCOM_0     = (dword *) 0xb2010818;
volatile dword *EMMA_ZCCT1_CAPCOM_1     = (dword *) 0xb201081c;
volatile dword *EMMA_ZCCT1_CAPCOM_2     = (dword *) 0xb2010820;
volatile dword *EMMA_ZCCT1_CAPCOM_3     = (dword *) 0xb2010824;

volatile dword *EMMA_ZCCT2_TIMER_CMD    = (dword *) 0xb2010840;
volatile dword *EMMA_ZCCT2_TIMER_MODE   = (dword *) 0xb2010844;
volatile dword *EMMA_ZCCT2_TIMER_VALUE  = (dword *) 0xb2010848;
volatile dword *EMMA_ZCCT2_TIMER_STATUS = (dword *) 0xb201084c;
volatile dword *EMMA_ZCCT2_INT_STATUS   = (dword *) 0xb2010850;
volatile dword *EMMA_ZCCT2_EVENT_ACT    = (dword *) 0xb2010854;
volatile dword *EMMA_ZCCT2_CAPCOM_0     = (dword *) 0xb2010858;
volatile dword *EMMA_ZCCT2_CAPCOM_1     = (dword *) 0xb201085c;
volatile dword *EMMA_ZCCT2_CAPCOM_2     = (dword *) 0xb2010860;
volatile dword *EMMA_ZCCT2_CAPCOM_3     = (dword *) 0xb2010864;

volatile dword *EMMA_ZCCT3_TIMER_CMD    = (dword *) 0xb2010880;
volatile dword *EMMA_ZCCT3_TIMER_MODE   = (dword *) 0xb2010884;
volatile dword *EMMA_ZCCT3_TIMER_VALUE  = (dword *) 0xb2010888;
volatile dword *EMMA_ZCCT3_TIMER_STATUS = (dword *) 0xb201088c;
volatile dword *EMMA_ZCCT3_INT_STATUS   = (dword *) 0xb2010890;
volatile dword *EMMA_ZCCT3_EVENT_ACT    = (dword *) 0xb2010894;
volatile dword *EMMA_ZCCT3_CAPCOM_0     = (dword *) 0xb2010898;
volatile dword *EMMA_ZCCT3_CAPCOM_1     = (dword *) 0xb201089c;
volatile dword *EMMA_ZCCT3_CAPCOM_2     = (dword *) 0xb20108a0;
volatile dword *EMMA_ZCCT3_CAPCOM_3     = (dword *) 0xb20108a4;

volatile dword *EMMA_ZCCT4_TIMER_CMD    = (dword *) 0xb20108c0;
volatile dword *EMMA_ZCCT4_TIMER_MODE   = (dword *) 0xb20108c4;
volatile dword *EMMA_ZCCT4_TIMER_VALUE  = (dword *) 0xb20108c8;
volatile dword *EMMA_ZCCT4_TIMER_STATUS = (dword *) 0xb20108cc;
volatile dword *EMMA_ZCCT4_INT_STATUS   = (dword *) 0xb20108d0;
volatile dword *EMMA_ZCCT4_EVENT_ACT    = (dword *) 0xb20108d4;
volatile dword *EMMA_ZCCT4_CAPCOM_0     = (dword *) 0xb20108d8;
volatile dword *EMMA_ZCCT4_CAPCOM_1     = (dword *) 0xb20108dc;
volatile dword *EMMA_ZCCT4_CAPCOM_2     = (dword *) 0xb20108e0;
volatile dword *EMMA_ZCCT4_CAPCOM_3     = (dword *) 0xb20108e4;
