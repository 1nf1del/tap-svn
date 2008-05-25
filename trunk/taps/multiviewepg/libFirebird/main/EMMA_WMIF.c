#include "tap.h"

//WMIF = Memory Interface
volatile dword *EMMA_WMIF_SOFT_RST     = (dword *) 0xb0010000;
volatile dword *EMMA_WMIF_CONFIG       = (dword *) 0xb0010004;
volatile dword *EMMA_WMIF_INIT         = (dword *) 0xb0010008;
volatile dword *EMMA_WMIF_REFCNT       = (dword *) 0xb001000c;
volatile dword *EMMA_WMIF_DISP_CFG     = (dword *) 0xb0010010;
volatile dword *EMMA_WMIF_MPEG_CFG     = (dword *) 0xb0010014;
volatile dword *EMMA_WMIF_DMA_CFG      = (dword *) 0xb0010018;
volatile dword *EMMA_WMIF_IDE_CFG      = (dword *) 0xb001001c;
volatile dword *EMMA_WMIF_TS_CFG       = (dword *) 0xb0010020;
volatile dword *EMMA_WMIF_DVI_CFG      = (dword *) 0xb0010024;
volatile dword *EMMA_WMIF_CPU_CFG      = (dword *) 0xb0010028;
volatile dword *EMMA_WMIF_BLT_CFG      = (dword *) 0xb001002c;
volatile dword *EMMA_WMIF_CNT_CFG      = (dword *) 0xb0010030;
volatile dword *EMMA_WMIF_REQCNT       = (dword *) 0xb0010034;
volatile dword *EMMA_WMIF_INT          = (dword *) 0xb0010038;
volatile dword *EMMA_WMIF_INT_EN       = (dword *) 0xb001003c;

//These registers hold pointers to the 8 EMMA_MPEG decode windows
volatile dword *EMMA_WMIF_ATBL0_Y      = (dword *) 0xb0010040;
volatile dword *EMMA_WMIF_ATBL0_C      = (dword *) 0xb0010044;
volatile dword *EMMA_WMIF_ATBL1_Y      = (dword *) 0xb0010048;
volatile dword *EMMA_WMIF_ATBL1_C      = (dword *) 0xb001004c;
volatile dword *EMMA_WMIF_ATBL2_Y      = (dword *) 0xb0010050;
volatile dword *EMMA_WMIF_ATBL2_C      = (dword *) 0xb0010054;
volatile dword *EMMA_WMIF_ATBL3_Y      = (dword *) 0xb0010058;
volatile dword *EMMA_WMIF_ATBL3_C      = (dword *) 0xb001005c;
volatile dword *EMMA_WMIF_ATBL4_Y      = (dword *) 0xb0010060;
volatile dword *EMMA_WMIF_ATBL4_C      = (dword *) 0xb0010064;
volatile dword *EMMA_WMIF_ATBL5_Y      = (dword *) 0xb0010068;
volatile dword *EMMA_WMIF_ATBL5_C      = (dword *) 0xb001006c;
volatile dword *EMMA_WMIF_ATBL6_Y      = (dword *) 0xb0010070;
volatile dword *EMMA_WMIF_ATBL6_C      = (dword *) 0xb0010074;
volatile dword *EMMA_WMIF_ATBL7_Y      = (dword *) 0xb0010078;
volatile dword *EMMA_WMIF_ATBL7_C      = (dword *) 0xb001007c;

//The rest are still unexplained by NEC
volatile dword *EMMA_WMIF_YTBL_DISP_T0 = (dword *) 0xb0010080;
volatile dword *EMMA_WMIF_YTBL_DISP_T1 = (dword *) 0xb0010084;
volatile dword *EMMA_WMIF_YTBL_DISP_T2 = (dword *) 0xb0010088;
volatile dword *EMMA_WMIF_YTBL_DISP_T3 = (dword *) 0xb001008c;
volatile dword *EMMA_WMIF_YTBL_DISP_T4 = (dword *) 0xb0010090;
volatile dword *EMMA_WMIF_YTBL_DISP_B0 = (dword *) 0xb00100a0;
volatile dword *EMMA_WMIF_YTBL_DISP_B1 = (dword *) 0xb00100a4;
volatile dword *EMMA_WMIF_YTBL_DISP_B2 = (dword *) 0xb00100a8;
volatile dword *EMMA_WMIF_YTBL_DISP_B3 = (dword *) 0xb00100ac;
volatile dword *EMMA_WMIF_YTBL_DISP_B4 = (dword *) 0xb00100b0;
volatile dword *EMMA_WMIF_YTBL_MPEG_T0 = (dword *) 0xb00100c0;
volatile dword *EMMA_WMIF_YTBL_MPEG_T1 = (dword *) 0xb00100c4;
volatile dword *EMMA_WMIF_YTBL_MPEG_T2 = (dword *) 0xb00100c8;
volatile dword *EMMA_WMIF_YTBL_MPEG_T3 = (dword *) 0xb00100cc;
volatile dword *EMMA_WMIF_YTBL_MPEG_T4 = (dword *) 0xb00100d0;
volatile dword *EMMA_WMIF_YTBL_MPEG_B0 = (dword *) 0xb00100e0;
volatile dword *EMMA_WMIF_YTBL_MPEG_B1 = (dword *) 0xb00100e4;
volatile dword *EMMA_WMIF_YTBL_MPEG_B2 = (dword *) 0xb00100e8;
volatile dword *EMMA_WMIF_YTBL_MPEG_B3 = (dword *) 0xb00100ec;
volatile dword *EMMA_WMIF_YTBL_MPEG_B4 = (dword *) 0xb00100f0;
volatile dword *EMMA_WMIF_DUMMY        = (dword *) 0xb00100f8;
volatile dword *EMMA_WMIF_CLOCK_DELAY  = (dword *) 0xb00100fc;
