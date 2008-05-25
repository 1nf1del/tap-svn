#include "tap.h"

//MPEG = MPEG Decoder
volatile dword *EMMA_MPEG_RESET           = (dword *) 0xb0002000;
volatile dword *EMMA_MPEG_BS_CTL_MAIN_CH  = (dword *) 0xb0002010;
volatile dword *EMMA_MPEG_BS_PTR_MAIN_CH  = (dword *) 0xb0002014;
volatile dword *EMMA_MPEG_START_MODE      = (dword *) 0xb0002018;
volatile dword *EMMA_MPEG_START_CTL       = (dword *) 0xb000201c;
volatile dword *EMMA_MPEG_START_STATUS    = (dword *) 0xb0002020;
volatile dword *EMMA_MPEG_START_STATUS2   = (dword *) 0xb0002024;
volatile dword *EMMA_MPEG_b0002038        = (dword *) 0xb0002038;
volatile dword *EMMA_MPEG_b000203c        = (dword *) 0xb000203c;
volatile dword *EMMA_MPEG_DEC_OV          = (dword *) 0xb0002050;
volatile dword *EMMA_MPEG_DEC_MODE        = (dword *) 0xb0002054;
volatile dword *EMMA_MPEG_DEC_CTL         = (dword *) 0xb0002058;
volatile dword *EMMA_MPEG_WAIT_COND       = (dword *) 0xb000205c;
volatile dword *EMMA_MPEG_TIMEOUT_MAIN    = (dword *) 0xb0002070;
volatile dword *EMMA_MPEG_TIMEOUT_SUB     = (dword *) 0xb0002074;
volatile dword *EMMA_MPEG_MAIN_STATUS     = (dword *) 0xb0002080;
volatile dword *EMMA_MPEG_ZERO23_STATUS   = (dword *) 0xb0002084;
volatile dword *EMMA_MPEG_DEC_STATUS      = (dword *) 0xb0002088;
volatile dword *EMMA_MPEG_BRL_STATUS      = (dword *) 0xb000208c;
volatile dword *EMMA_MPEG_SUB_FIFO_STATUS = (dword *) 0xb0002090;
volatile dword *EMMA_MPEG_TIMEOUT_STATUS  = (dword *) 0xb0002094;
volatile dword *EMMA_MPEG_VLC_ERROR       = (dword *) 0xb0002098;
volatile dword *EMMA_MPEG_BRL_RD          = (dword *) 0xb0002100;
volatile dword *EMMA_MPEG_BRL_SHIFT       = (dword *) 0xb0002104;
volatile dword *EMMA_MPEG_MATRIX_I        = (dword *) 0xb0002200;
volatile dword *EMMA_MPEG_MATRIX_N        = (dword *) 0xb0002240;

volatile dword *EMMA_MPEG_PARAM_PICH      = (dword *) 0xb0002284;
volatile dword *EMMA_MPEG_PARAM_PICE      = (dword *) 0xb0002288;
volatile dword *EMMA_MPEG_MC_MODE         = (dword *) 0xb000228c;
volatile dword *EMMA_MPEG_CPU_INT         = (dword *) 0xb00022c0;
volatile dword *EMMA_MPEG_CPU_MASK        = (dword *) 0xb00022c4;
