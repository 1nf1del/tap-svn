#include                "../libFireBird.h"
#include                "FBLib_compression.h"

dword TFDSize (byte *pTFD)
{
  word                  NrBlocks = 0;
  dword                 i;
  byte                  *p = pTFD;

  if (LOAD_WORD(p) != 8) return 0;                              //Invalid header?
  if (CRC16 (0, p + 4, 6) != LOAD_WORD(p + 2)) return 0;        //Invalid header CRC?
  if (LOAD_WORD(p + 6) != 1) return 0;                          //Invalid file version?

  NrBlocks = LOAD_WORD(p + 8);
  p += 10;
  for (i = 0; i < NrBlocks; i++)
    p += LOAD_WORD(p) + 2;

  return p - pTFD;
}
