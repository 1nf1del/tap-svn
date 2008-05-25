#include "type.h"

byte _Info_C_Cpm[] =
{
  0x00, 0x08, 0x1B, 0x90, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x01, 
  0x00, 0x13, 0xB7, 0x6B, 0xFF, 0xFF, 0x01, 0x90, 
  0x00, 0x04, 0x28, 0x05, 0x3F, 0xE2, 0xA6, 0x12, 0xC7, 0xF8, 0xBB, 0x01, 0x4E, 
};

TYPE_GrData _Info_C_Gd =
{
  1,                              //version
  0,                              //reserved
  OSD_1555,                       //data format
  COMPRESS_Tfp,                   //compressed method
  _Info_C_Cpm,                    //data
   400,                           //size
    10,                           //width
    20                            //height
};

