#include                "../libFireBird.h"
#include                "FBLib_compression.h"

dword CompressedTFDSize (byte *pSrc, dword SourceBufferSize, void *pPercentFinishedCallback)
{
  return CompressTFD (pSrc, SourceBufferSize, NULL, 0, 0, pPercentFinishedCallback);
}
