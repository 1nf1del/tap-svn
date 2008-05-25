#include "type.h"

byte _Scroll_Back_Cpm[] =
{
  0x00, 0x08, 0x1B, 0x90, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x01, 
  0x00, 0x13, 0xEE, 0x90, 0xFF, 0xFF, 0x01, 0xB8, 
  0x00, 0x04, 0x28, 0x49, 0x3F, 0xED, 0x84, 0x81, 0xB1, 0x18, 0xD4, 0x02, 0x9C, 
};

TYPE_GrData _Scroll_Back_Gd =
{
  1,                              //version
  0,                              //reserved
  OSD_1555,                       //data format
  COMPRESS_Tfp,                   //compressed method
  _Scroll_Back_Cpm,               //data
   440,                           //size
    10,                           //width
    22                            //height
};

