#include "type.h"

byte _MsgBox_C_Border_Cpm[] =
{
  0x00, 0x08, 0x1B, 0x90, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x01, 
  0x00, 0x86, 0x95, 0x88, 0xFF, 0xFF, 0xD3, 0xC0, 
  0x00, 0xF1, 0x55, 0x4C, 0xA2, 0x06, 0xD7, 0xF9, 0x11, 0x12, 0x89, 0x60, 0xB5, 0x80, 0xCC, 0x6E, 
  0x80, 0x04, 0xC4, 0x04, 0x4A, 0x0D, 0xF1, 0xF8, 0x9A, 0x26, 0x01, 0x13, 0x09, 0x61, 0xCD, 0x39, 
  0x3D, 0x15, 0x80, 0x84, 0xAC, 0x4F, 0x19, 0xFB, 0x28, 0x00, 0x00, 0x01, 0x64, 0xE5, 0xC5, 0xA5, 
  0xBF, 0x69, 0x62, 0xA9, 0x25, 0xFA, 0xBD, 0x25, 0xD0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xAE, 0x2D, 0x2C, 
  0x56, 0x4D, 0xC6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7B, 0x64, 0x3F, 
  0xBD, 0x69, 0x2C, 0x55, 0x24, 0x92, 0xF9, 0x6D, 0xE6, 0x57, 0xA4, 0xBC, 0x6B, 0x2E, 0xFD, 0x64, 
};

TYPE_GrData _MsgBox_C_Border_Gd =
{
  1,                              //version
  0,                              //reserved
  OSD_1555,                       //data format
  COMPRESS_Tfp,                   //compressed method
  _MsgBox_C_Border_Cpm,           //data
  54208,                           //size
   121,                           //width
   224                            //height
};

