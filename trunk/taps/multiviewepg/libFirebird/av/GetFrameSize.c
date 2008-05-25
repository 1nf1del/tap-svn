#include "../libFireBird.h"

// ------------------------------ GetFrameSize ------------------------------------
//
void GetFrameSize (dword *Width, dword *Height)
{
  *Width = (*EMMA_MPEG_PARAM_SEQ & 0x000000ff) << 4;
  *Height = (*EMMA_MPEG_PARAM_SEQ & 0x0000ff00) >> 4;
}
