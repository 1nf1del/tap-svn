#include                "../libFireBird.h"
#include                "FBLib_compression.h"

//--------------------------------------------------------------
// UncompressLoader() is a function wrapper that decodes data blocks
// encoded with AR002 algorithm until the uncompressed size field
// is set to 0xfefe. This is normaly used for the Loader or the Firmware
// inside of the flash memory.
// The expected block structure is as follows:
// 0 - uncompressed size (word)
// 2 - compressed size (word)
// 4 .. - compressed data (byte array)
dword UncompressLoader (byte *pSrc, byte *pDest, void *pPercentFinishedCallback)
{
  word                  compSize, uncompSize;
  dword                 outSize = 0, NrBlocks = 0, CurrentBlock = 0;
  byte                  *OrigpSrc;

  //PercentFinishedCallback is called for every block. PercentFinished contains a number between 0 and 100
  void (*PercentFinishedCallback) (dword PercentFinished) = pPercentFinishedCallback;

  OrigpSrc   = pSrc;
  uncompSize = LOAD_WORDLE(pSrc + 0);
  compSize   = LOAD_WORDLE(pSrc + 2);

  //Count the number of blocks
  while (uncompSize != 0xfefe)
  {
    NrBlocks++;

    if (uncompSize > 0x8000) return 0;

    pSrc += 4;
    pSrc += compSize;

    uncompSize = LOAD_WORDLE(pSrc + 0);
    compSize   = LOAD_WORDLE(pSrc + 2);
  }

  pSrc = OrigpSrc;
  uncompSize = LOAD_WORDLE(pSrc + 0);
  compSize   = LOAD_WORDLE(pSrc + 2);

  while (uncompSize != 0xfefe)
  {
    if (PercentFinishedCallback) PercentFinishedCallback (CurrentBlock * 100 / NrBlocks);
    CurrentBlock++;

    if (uncompSize > 0x8000) return 0;

    pSrc += 4;

    if(compSize == uncompSize)
    {
      //data not compressed, copy it directly
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

    uncompSize = LOAD_WORDLE(pSrc + 0);
    compSize   = LOAD_WORDLE(pSrc + 2);
  }
  if (PercentFinishedCallback) PercentFinishedCallback (100);

  return outSize;
}
