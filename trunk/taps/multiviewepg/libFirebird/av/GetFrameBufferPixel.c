#include "../libFireBird.h"

// ------------------------------ GetFrameBufferPixel ------------------------------------
//
dword GetFrameBufferPixel (dword x, dword y)
{
  dword                 Width, Height, picAddr, Offset;
  byte                  Y, U, V;

  Width  = (*EMMA_MPEG_PARAM_SEQ & 0x000000ff) << 4;
  Height = (*EMMA_MPEG_PARAM_SEQ & 0x0000ff00) >> 4;

  picAddr = (((*EMMA_WMIF_ATBL0_Y & 0xffff0000) >> 6) | 0x80000000);

  //If the horizontal size of the frame is not 720 pixel, scale down the x coordinate
  if (Width != 720) x = x * Width / 720;

  //If the vertical size of the frame is not 576 pixel (e.g. NTSC), scale down the y coordinate
  if (Height != 576) y = y * Height / 576;

  Offset = (x & 0x1f) + ((y & 0x1f) << 5) + ((x & 0xffe0) << 5) + ((y & 0xffe0) * 736);

  Y = *(byte*)(picAddr + Offset);

  picAddr = (((*EMMA_WMIF_ATBL0_C & 0xffff0000) >> 6) | 0x80000000);
  V = *(byte*)(picAddr + (Offset << 1));
  U = *(byte*)(picAddr + (Offset << 1) + 1);

  return (Y << 16) | (U << 8) | V;
}
