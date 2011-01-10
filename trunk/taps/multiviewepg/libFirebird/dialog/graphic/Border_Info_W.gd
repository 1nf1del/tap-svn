#include "type.h"

byte _Border_Info_W_Cpm[] =
{
  0x00, 0x08, 0x1B, 0x90, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x01, 
  0x00, 0x2B, 0xC4, 0xEB, 0xFF, 0xFF, 0x02, 0x80, 
  0x00, 0x10, 0x3C, 0x6A, 0x87, 0xFC, 0x88, 0xA8, 0x5A, 0x80, 0x88, 0x18, 0xC8, 0xD2, 0xC8, 0xC8, 
  0xC0, 0xEE, 0x73, 0xB9, 0x10, 0x52, 0x2F, 0xD9, 0x20, 0x01, 0xBC, 0xA9, 0xE5, 0x3B, 0x83, 0x65, 
  0x2D, 0x83, 0xC3, 0xFD, 0xE0, 
};

TYPE_GrData _Border_Info_W_Gd =
{
  1,                              //version
  0,                              //reserved
  OSD_1555,                       //data format
  COMPRESS_Tfp,                   //compressed method
  _Border_Info_W_Cpm,             //data
   640,                           //size
    16,                           //width
    20                            //height
};
