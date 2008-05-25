#include                "../libFireBird.h"
#include                "FBLib_compression.h"

//--------------------------------------------------------------
// UncompressTFD() is a function wrapper that decodes data blocks
// encoded with AR002 algorithm. This type uses a header

// The expected block structure is as follows:
// 0 - header length (always 0x0008)
// 2 - header CRC16
// 4 - System ID
// 6 - file version (always 0x0001)
// 8 - number of compressed blocks

//Every block has the following header:
// 0 - compressed size includeing header (word)
// 2 - CRC-16 (word)
// 4 - block type
// 6 - uncompressed size (word)
// 8 .. (compressed size + 5) - compressed data (byte array)
dword UncompressTFD (byte *pSrc, byte *pDest, void *pPercentFinishedCallback)
{
  word                  compSize = 0, uncompSize = 0, NrBlocks = 0;
  dword                 outSize = 0, i;

  //PercentFinishedCallback is called for every block. PercentFinished contains a number between 0 and 100
  void (*PercentFinishedCallback) (dword PercentFinished) = pPercentFinishedCallback;

  if (LOAD_WORD(pSrc) != 8) return 0;                              //Invalid header?
  if (CRC16 (0, pSrc + 4, 6) != LOAD_WORD(pSrc + 2)) return 0;     //Invalid header CRC?
  if (LOAD_WORD(pSrc + 6) != 1) return 0;                          //Invalid file version?

  NrBlocks = LOAD_WORD(pSrc + 8);

  pSrc += 10;

  for (i = 0; i < NrBlocks; i++)
  {
    if (PercentFinishedCallback) PercentFinishedCallback (i * 100 / NrBlocks);

    compSize   = LOAD_WORD(pSrc) - 6;
    uncompSize = LOAD_WORD(pSrc + 6);

    if (uncompSize > 0x7ffa) return 0;

    pSrc += 8;

    if(compSize == uncompSize)
    {
      // not compressed data, copy it directly
      if (pDest) memcpy(pDest, pSrc, uncompSize);
    }
    else
    {
      // compressed data, uncompress it
      if (!UncompressBlock (pSrc, compSize, pDest, uncompSize)) return 0;
    }

    if (pDest) pDest += uncompSize;
    pSrc += compSize;
    outSize += uncompSize;
  }
  if (PercentFinishedCallback) PercentFinishedCallback (100);

  return outSize;
}
