#include "type.h"

byte _Border_NW_Cpm[] =
{
  0x00, 0x08, 0x1B, 0x90, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x01, 
  0x00, 0xD7, 0x42, 0x22, 0xFF, 0xFF, 0x0F, 0x50, 
  0x00, 0x7F, 0x53, 0x53, 0x8D, 0xAF, 0xF7, 0x98, 0x88, 0xF0, 0x11, 0x12, 0xF0, 0x12, 0x89, 0xB8, 
  0x09, 0x84, 0xBC, 0x04, 0x44, 0xDC, 0x04, 0xC2, 0x50, 0x50, 0x10, 0x30, 0x18, 0x04, 0x0C, 0x02, 
  0x06, 0x05, 0x02, 0x81, 0x40, 0xA0, 0x60, 0x28, 0x18, 0x0D, 0x2C, 0x66, 0x96, 0x67, 0x0D, 0x40, 
  0xC0, 0x31, 0x9A, 0x32, 0x21, 0xA0, 0x33, 0x00, 0xC6, 0x46, 0x4D, 0x33, 0x63, 0x10, 0x1E, 0x2B, 
  0xA4, 0x3C, 0x25, 0xFA, 0x6C, 0x01, 0x29, 0x74, 0x38, 0x45, 0x9E, 0xAF, 0x31, 0xA2, 0xEA, 0xF5, 
  0x3E, 0x8D, 0xAC, 0x7D, 0x9C, 0x34, 0xCF, 0xE2, 0xDA, 0x36, 0x99, 0xD6, 0x75, 0x9E, 0x88, 0xBB, 
  0xD6, 0x8D, 0xC4, 0x5D, 0xA3, 0xF2, 0x1B, 0x44, 0x56, 0x63, 0xB8, 0x8D, 0x68, 0xF7, 0x8D, 0xAC, 
  0x7F, 0xBC, 0x75, 0xA3, 0xDE, 0x37, 0xF8, 0xF5, 0x19, 0x30, 0x26, 0x04, 0x12, 0xFB, 0xC4, 0x7A, 
  0x8D, 0xBB, 0x3C, 0xD1, 0xCB, 0x0E, 0xBD, 0x1A, 0xB3, 0x5F, 0x6E, 0x1E, 0xA3, 0x20, 0x9B, 0x34, 
  0xF3, 0x0F, 0xC8, 0x7B, 0x31, 0x99, 0x0B, 0xBF, 0xC5, 0x0E, 0x91, 0xC3, 0xD8, 0x37, 0xA8, 0xBB, 
  0x87, 0xA0, 0x92, 0xC2, 0xCC, 0x74, 0x03, 0x21, 0xEA, 0x34, 0xD1, 0xF6, 0x1A, 0xE1, 0xE8, 0x24, 
  0x85, 0x90, 0xA0, 0x0F, 0x1E, 0xA3, 0x4E, 0xBC, 0x31, 0xD7, 0x0F, 0xDB, 0x36, 0x18, 0xEB, 0x87, 
  0xB3, 0xE7, 0xBF, 0x07, 0xA6, 0x01, 0x1E, 0x23, 0xC4, 0x78, 0x8F, 0x11, 0xE2, 0x3C, 0x47, 0xFE, 
  0x1E, 
};

TYPE_GrData _Border_NW_Gd =
{
  1,                              //version
  0,                              //reserved
  OSD_1555,                       //data format
  COMPRESS_Tfp,                   //compressed method
  _Border_NW_Cpm,                 //data
  3920,                           //size
    40,                           //width
    49                            //height
};

