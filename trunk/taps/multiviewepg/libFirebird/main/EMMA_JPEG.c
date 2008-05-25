#include "tap.h"

//JPEG registers
volatile dword *EMMA_MPEG_JPEG_WADR = (dword *) 0xb0002060;
volatile dword *EMMA_MPEG_JPEG_MODE = (dword *) 0xb0002064;
volatile dword *EMMA_MPEG_JPEG_CTL  = (dword *) 0xb0002068;
