#include "tap.h"

//DSCL = Scaler Control
volatile dword *EMMA_DSCL_SCL1_HDRTOP = (dword *) 0xb0004000;
volatile dword *EMMA_DSCL_SCL1_HDRBOT = (dword *) 0xb0004004;
volatile dword *EMMA_DSCL_SCL2_HDRTOP = (dword *) 0xb0004010;
volatile dword *EMMA_DSCL_SCL2_HDRBOT = (dword *) 0xb0004014;
volatile dword *EMMA_DSCL_SCL3_HDRTOP = (dword *) 0xb0004020;
volatile dword *EMMA_DSCL_SCL3_HDRBOT = (dword *) 0xb0004024;
volatile dword *EMMA_DSCL_CUR_DSPLINE = (dword *) 0xb0004030;
