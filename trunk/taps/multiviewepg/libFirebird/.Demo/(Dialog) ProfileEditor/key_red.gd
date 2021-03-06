#include "type.h"

byte _key_red_Cpm[] =
{
  0x00, 0x08, 0x1B, 0x90, 0xFF, 0xFF, 0x00, 0x01, 0x00, 0x01, 
  0x00, 0xDF, 0xDE, 0xAF, 0xFF, 0xFF, 0x03, 0x2A, 
  0x00, 0xA3, 0x5B, 0x57, 0xAD, 0x39, 0x9B, 0x44, 0xA2, 0x63, 0x68, 0x94, 0x4C, 0x6D, 0x13, 0x09, 
  0xC2, 0x70, 0xA8, 0x98, 0x44, 0xDA, 0x02, 0x02, 0x06, 0x02, 0x81, 0xC0, 0x51, 0x6C, 0x88, 0x2C, 
  0x88, 0x1B, 0x06, 0x80, 0x80, 0xCA, 0x16, 0x8C, 0x70, 0x36, 0x70, 0xA8, 0x1A, 0x74, 0xC0, 0x31, 
  0x94, 0x1A, 0x03, 0xC0, 0xFE, 0xB9, 0xD9, 0x87, 0xE5, 0xDA, 0x4D, 0x23, 0xE3, 0x06, 0xBA, 0x80, 
  0x11, 0x3A, 0xDB, 0xD7, 0xF9, 0x0A, 0x5E, 0xA6, 0xF0, 0xCC, 0xE5, 0x4A, 0x0C, 0xF7, 0x7F, 0x53, 
  0xAF, 0xFF, 0xEE, 0xE5, 0xB4, 0xF9, 0x10, 0x37, 0xE9, 0x96, 0xD0, 0x9C, 0x59, 0xB4, 0xEC, 0xB5, 
  0x92, 0x64, 0x44, 0xC8, 0xAC, 0xD9, 0x61, 0x1C, 0xDD, 0x0A, 0xC1, 0x9C, 0x56, 0x6D, 0x0E, 0x3C, 
  0xC2, 0x73, 0x15, 0x4A, 0xDC, 0x35, 0xEA, 0xB9, 0x77, 0xB3, 0x0E, 0x1F, 0x47, 0x42, 0x56, 0x29, 
  0xD2, 0x82, 0x87, 0xF2, 0x9D, 0x92, 0x04, 0x9C, 0x9E, 0xDA, 0x8B, 0xF7, 0xFA, 0x64, 0xC6, 0xFE, 
  0x0D, 0x76, 0x7C, 0x6B, 0x06, 0x94, 0x2D, 0xD5, 0x07, 0xAB, 0x5E, 0x54, 0x85, 0xBB, 0xD2, 0xA0, 
  0x8A, 0xB1, 0x71, 0x54, 0xA6, 0x38, 0xD0, 0x66, 0x5A, 0xE9, 0x4F, 0x40, 0xE3, 0xF0, 0x73, 0x06, 
  0x75, 0x2B, 0x79, 0xBF, 0x0A, 0xEC, 0x56, 0xA1, 0x2E, 0x9C, 0xD6, 0x13, 0xBC, 0xCE, 0x20, 0xCD, 
  0x05, 0x87, 0x0C, 0x87, 0xA1, 0x35, 0x2A, 0x74, 0xCD, 0xE4, 0x94, 0x24, 0xF0, 0xE3, 0x97, 0xCF, 
  0x6E, 0x1A, 0x0B, 0x1B, 0xF9, 0x87, 0x1F, 0x6C, 0xF2, 
};

TYPE_GrData _key_red_Gd =
{
  1,                              //version
  0,                              //reserved
  OSD_1555,                       //data format
  COMPRESS_Tfp,                   //compressed method
  _key_red_Cpm,                   //data
   810,                           //size
    27,                           //width
    15                            //height
};

