#include "type.h"

byte _InfoN_C_Cpm[] =
{
  0x00, 0x08, 0x1B, 0x90, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x01, 
  0x00, 0x25, 0x48, 0x60, 0xFF, 0xFF, 0x00, 0x78, 
  0x00, 0x12, 0x38, 0x4D, 0x60, 0xC4, 0x30, 0x11, 0x85, 0xAC, 0x25, 0x48, 0xC0, 0x44, 0x8C, 0x00, 
  0x40, 0xA5, 0x22, 0x44, 0x0C, 0x03, 0xE0, 0x37, 0x05, 0x31, 0x88, 0x6A, 0x9C, 0xE7, 0xC8, 
};

TYPE_GrData _InfoN_C_Gd =
{
  1,                              //version
  0,                              //reserved
  OSD_1555,                       //data format
  COMPRESS_Tfp,                   //compressed method
  _InfoN_C_Cpm,                   //data
   120,                           //size
    10,                           //width
     6                            //height
};
