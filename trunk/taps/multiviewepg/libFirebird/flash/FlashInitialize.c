#include "FBLib_flash.h"

// FlashBlock[0] is a dummy entry to make counting start from 1

dword FlashBlockOffsetS[] = {0x00000000, 0x00000000, 0x0001B584, 0x00027108,
                             0x0002BCC0, 0x00037848, 0x00039D18, 0x00039D60,
                             0x00039D6C, 0x0003C2A0, 0x0003C4D4, 0x0004C2C0,
                             0x0004D7C4, 0x0004D7E8, 0x0004DBEC, 0x0004DFEC};

dword FlashBlockOffsetT[] = {0x00000000, 0x00000000, 0x00007D04, 0x0000FA08,
                             0x0000FA2C, 0x00010A24, 0x00012EF4, 0x00012F3C,
                             0x00012F48, 0x0001547C, 0x000156B0, 0x0002549C,
                             0x000269A0, 0x000269A0, 0x000269A0, 0x000269A0};

dword FlashBlockOffsetC[] = {0x00000000, 0x00000000, 0x00007D04, 0x0000FA08,
                             0x0000FA2C, 0x00010628, 0x00012AF8, 0x00012B40,
                             0x00012B4C, 0x00014F68, 0x0001519C, 0x00024F88,
                             0x0002648C, 0x0002648C, 0x0002648C, 0x0002648C};

dword FlashBlockOffsetT5700[] =
                            {0x00000000, 0x00000000, 0x00007D04, 0x0000FA08,
                             0x0000FA2C, 0x0001121C, 0x000136EC, 0x00013734,
                             0x00013740, 0x00015EA4, 0x000160D8, 0x00025EC4,
                             0x000273C8, 0x000273C8, 0x000273C8, 0x000273C8};

dword                   FlashBlockOffset[BLOCKS + 1];
dword                   FlashOffset = 0;
SYSTEM_TYPE             SystemType = ST_UNKNOWN;

//--------------------------------------- FlashInitialize --------------------------------
//
dword FlashInitialize (dword SystemID)
{
  dword                 i;
  tToppyInfo           *ToppyInfo;
  tFWDATHeader         *FWDatHeader;

  if (FlashOffset != 0) return FlashOffset;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return 0;

  //Get toppy information
  if (!LoadFirmwareDat(&FWDatHeader, &ToppyInfo, NULL)) return 0;

  for (i = 0; i < FWDatHeader->NrOfToppyInfoEntries; i++, ToppyInfo++)
  {
    if (ToppyInfo->SysID == SystemID)
    {
      SystemType = ToppyInfo->SystemType;
      break;
    }
  }

  //Copy the appropriate offset table
  switch (SystemType)
  {
    case ST_DVBS    : memcpy (FlashBlockOffset, FlashBlockOffsetS, (BLOCKS + 1) * sizeof (dword)); break ;
    case ST_DVBT    : memcpy (FlashBlockOffset, FlashBlockOffsetT, (BLOCKS + 1) * sizeof (dword)); break ;
    case ST_DVBC    : memcpy (FlashBlockOffset, FlashBlockOffsetC, (BLOCKS + 1) * sizeof (dword)); break ;
    case ST_DVBT5700: memcpy (FlashBlockOffset, FlashBlockOffsetT5700, (BLOCKS + 1) * sizeof (dword)); break ;
    default: return 0;
  }

  //The following code searches for the start of the Flash in the RAM. It uses the following method:
  //the Flash starts with the CRC which is never 0xFFFF. The word after the CRC is always 0xFFFF and the next word isn't 0xFFFF again.
  if (!(FlashOffset = FIS_vFlash())) return 0;

  //Add the start of the flash to the FlashBlockOffset array to make its address entries absolute.
  for (i = TVServices; i <= BLOCKS; i++) FlashBlockOffset [i] += FlashOffset;

  return FlashOffset;
}
