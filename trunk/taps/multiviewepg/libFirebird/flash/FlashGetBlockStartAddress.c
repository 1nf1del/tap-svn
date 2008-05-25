#include "FBLib_flash.h"

//--------------------------------------- FlashGetBlockStartAddress --------------------------------
//
dword FlashGetBlockStartAddress (TYPE_FlashBlock BlockNumber)
{
  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized || (FlashOffset == 0 || (int) BlockNumber <= 0 || BlockNumber > BLOCKS)) return 0;

  return (FlashBlockOffset [BlockNumber]);
}
