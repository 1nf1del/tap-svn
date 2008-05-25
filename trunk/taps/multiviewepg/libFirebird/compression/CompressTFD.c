#include                "../libFireBird.h"
#include                "FBLib_compression.h"

//--------------------------------------------------------------
// CompressTFD() is a function wrapper that encodes data blocks
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
dword CompressTFD (byte *pSrc, dword SourceBufferSize, byte *pDest, word TFDType, word SysID, void *pPercentFinishedCallback)
{
  word                  OrigSize, CompSize;
  dword                 OutBufferSize, NrBlocks = 0, FullSize = SourceBufferSize;
  byte                  *FileHeader;

  //PercentFinishedCallback is called for every block. PercentFinished contains a number between 0 and 100
  void (*PercentFinishedCallback) (dword PercentFinished) = pPercentFinishedCallback;

  //Build the tfd file header
  FileHeader = pDest;
  if (pDest)
  {
    STORE_WORD (pDest    , 0x0008);
    STORE_WORD (pDest + 4, SysID);
    STORE_WORD (pDest + 6, 0x0001);
    pDest += 10;
  }
  OutBufferSize = 10;

  while (SourceBufferSize)
  {
    if (PercentFinishedCallback) PercentFinishedCallback ((FullSize - SourceBufferSize) * 100 / FullSize);

    NrBlocks++;
    OrigSize = (SourceBufferSize > 0x7ffa) ? 0x7ffa : SourceBufferSize;

    if (pDest)
    {
      CompSize = CompressBlock (pSrc, OrigSize, pDest + 8);

      //Build the block header
      STORE_WORD (pDest    , CompSize + 6);
      STORE_WORD (pDest + 4, TFDType);
      STORE_WORD (pDest + 6, OrigSize);
      STORE_WORD (pDest + 2, CRC16 (0, pDest + 4, 4 + CompSize));
      pDest += CompSize + 8;
    }
    else CompSize = CompressBlock (pSrc, OrigSize, NULL);

    OutBufferSize    += CompSize + 8;
    SourceBufferSize -= OrigSize;
    pSrc             += OrigSize;
  }

  if (FileHeader)
  {
    STORE_WORD (FileHeader + 8, NrBlocks);
    STORE_WORD (FileHeader + 2, CRC16 (0, FileHeader + 4, 6));
  }

  if (PercentFinishedCallback) PercentFinishedCallback (100);

  return OutBufferSize;
}
