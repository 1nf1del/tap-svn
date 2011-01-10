#include "type.h"

byte _MsgBox_C_Text_Cpm[] =
{
  0x00, 0x08, 0x1B, 0x90, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x01, 
  0x00, 0x62, 0xEE, 0x7E, 0xFF, 0xFF, 0x88, 0x20, 
  0x00, 0x9C, 0x4C, 0x48, 0xA0, 0x54, 0xFF, 0xD9, 0x01, 0xC6, 0xE4, 0x7B, 0x21, 0x5C, 0x80, 0xE4, 
  0x19, 0x03, 0x11, 0x09, 0x80, 0x80, 0x90, 0xB4, 0x97, 0x24, 0x03, 0x21, 0x9C, 0x3E, 0xEE, 0xF9, 
  0x9E, 0xB4, 0x40, 0x00, 0x00, 0x15, 0xA5, 0x0B, 0xCE, 0x1E, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0D, 0xBC, 0xEC, 
  0xF9, 0x9F, 0xEC, 0x57, 0x15, 0x7D, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0xE2, 0xF3, 0x85, 0xA5, 0x0E, 0x92, 0x49, 0x24, 0x92, 0x49, 0x2D, 0x40, 
};

TYPE_GrData _MsgBox_C_Text_Gd =
{
  1,                              //version
  0,                              //reserved
  OSD_1555,                       //data format
  COMPRESS_Tfp,                   //compressed method
  _MsgBox_C_Text_Cpm,             //data
  34848,                           //size
   121,                           //width
   144                            //height
};
