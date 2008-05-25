#include "tap.h"

//DTOP = Timing, Global Control, and Interrupts
volatile dword *EMMA_DTOP_DSCL1_RELD  = (dword *) 0xb0004600;
volatile dword *EMMA_DTOP_DSCL2_RELD  = (dword *) 0xb0004604;
volatile dword *EMMA_DTOP_DSCL3_RELD  = (dword *) 0xb0004608;
volatile dword *EMMA_DTOP_DOSD1_RELD  = (dword *) 0xb000460c;
volatile dword *EMMA_DTOP_DOSD2_RELD  = (dword *) 0xb0004610;
volatile dword *EMMA_DTOP_DHWC_RELD   = (dword *) 0xb0004614;
volatile dword *EMMA_DTOP_DALP_RELD   = (dword *) 0xb000461c;
volatile dword *EMMA_DTOP_SYNC_SEL    = (dword *) 0xb0004700;
volatile dword *EMMA_DTOP_SYNC_SELST  = (dword *) 0xb0004704;
volatile dword *EMMA_DTOP_SYNC12_VFLD = (dword *) 0xb0004708;
volatile dword *EMMA_DTOP_SYNC3_VFLD  = (dword *) 0xb000470c;
volatile dword *EMMA_DTOP_INT1_SEL    = (dword *) 0xb0004710;
volatile dword *EMMA_DTOP_INT1_EN     = (dword *) 0xb0004714;
volatile dword *EMMA_DTOP_STAT1_SEL   = (dword *) 0xb0004718;
volatile dword *EMMA_DTOP_INT2_SEL    = (dword *) 0xb000471c;
volatile dword *EMMA_DTOP_INT2_EN     = (dword *) 0xb0004720;
volatile dword *EMMA_DTOP_STAT2_SEL   = (dword *) 0xb0004724;
volatile dword *EMMA_DTOP_SOFT_RST    = (dword *) 0xb0004728;
