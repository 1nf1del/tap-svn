#include "tap.h"

//Video Encoder
volatile dword *EMMA_VENC_b0008000     = (dword *) 0xb0008000;
volatile dword *EMMA_VENC_b0008004     = (dword *) 0xb0008004;
volatile dword *EMMA_VENC_b0008008     = (dword *) 0xb0008008;
volatile dword *EMMA_VENC_b000800c     = (dword *) 0xb000800c;
volatile dword *EMMA_VENC_VEE_CTR1     = (dword *) 0xb0008010;
volatile dword *EMMA_VENC_VEE_CTR2     = (dword *) 0xb0008014;
volatile dword *EMMA_VENC_VEE_CTR3     = (dword *) 0xb0008018;
volatile dword *EMMA_VENC_VEE_CTR4     = (dword *) 0xb000801c;
volatile dword *EMMA_VENC_VEE_CTR5     = (dword *) 0xb0008020;
volatile dword *EMMA_VENC_VEE_CTR6     = (dword *) 0xb0008024;
volatile dword *EMMA_VENC_VEE_CTR7     = (dword *) 0xb0008028;
volatile dword *EMMA_VENC_VEE_TTE1     = (dword *) 0xb000802c;
volatile dword *EMMA_VENC_VEE_TTE2     = (dword *) 0xb0008030;
volatile dword *EMMA_VENC_VEE_SECAM1   = (dword *) 0xb0008034;
volatile dword *EMMA_VENC_VEE_SECAM2   = (dword *) 0xb0008038;
volatile dword *EMMA_VENC_VEE_CTR8_DBG = (dword *) 0xb000803c;
volatile dword *EMMA_VENC_VEE_CTR9     = (dword *) 0xb0008040;
volatile dword *EMMA_VENC_VEE_CTR10    = (dword *) 0xb0008044;
volatile dword *EMMA_VENC_VEE_VID_WSS  = (dword *) 0xb0008048;
volatile dword *EMMA_VENC_VEE_CC1      = (dword *) 0xb000804c;
volatile dword *EMMA_VENC_VEE_CC2      = (dword *) 0xb0008050;
volatile dword *EMMA_VENC_VEE_TTE3     = (dword *) 0xb0008054;
