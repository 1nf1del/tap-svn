#include "tap.h"

//DALP = Alpha control
volatile dword *EMMA_DALP_ALPHA1    = (dword *) 0xb0004200;
volatile dword *EMMA_DALP_ALPHA2    = (dword *) 0xb0004202;
volatile dword *EMMA_DALP_ALPHA3    = (dword *) 0xb0004204;
volatile dword *EMMA_DALP_ALPHA4    = (dword *) 0xb0004206;
volatile dword *EMMA_DALP_ALPHA5    = (dword *) 0xb0004208;
volatile dword *EMMA_DALP_ALPHA6    = (dword *) 0xb000420a;
volatile dword *EMMA_DALP_ALPHA_CTR = (dword *) 0xb000420c;
volatile dword *EMMA_DALP_BCKGND1   = (dword *) 0xb0004210;
volatile dword *EMMA_DALP_BORDER1   = (dword *) 0xb0004214;
volatile dword *EMMA_DALP_BCKGND2   = (dword *) 0xb0004218;
volatile dword *EMMA_DALP_BORDER2   = (dword *) 0xb000421c;
volatile dword *EMMA_DALP_FBLANK    = (dword *) 0xb0004220;
volatile dword *EMMA_DALP_ALPHALMT0 = (dword *) 0xb0004224;
volatile dword *EMMA_DALP_ALPHALMT1 = (dword *) 0xb0004228;
volatile dword *EMMA_DALP_ALPHALMT2 = (dword *) 0xb000422c;
