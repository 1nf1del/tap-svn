#include                "../libFireBird.h"
#include                "FBLib_compression.h"

dword UncompressedTFDSize (byte *pSrc)
{
  word                  compSize = 0, uncompSize = 0, NrBlocks = 0;
  dword                 outSize = 0, i;

  if (LOAD_WORD(pSrc) != 8) return 0;                                     //Invalid header?
  if (CRC16 (0, pSrc + 4, 6) != LOAD_WORD(pSrc + 2)) return 0;     //Invalid header CRC?
  if (LOAD_WORD(pSrc + 6) != 1) return 0;                                 //Invalid file version?

  NrBlocks = LOAD_WORD(pSrc + 8);

  pSrc += 10;

  for (i = 0; i < NrBlocks; i++)
  {
    compSize   = LOAD_WORD(pSrc) - 6;
    uncompSize = LOAD_WORD(pSrc + 6);

    if (uncompSize > 0x7ffa) return 0;

    pSrc += compSize + 8;
    outSize += uncompSize;
  }

  return outSize;
}
