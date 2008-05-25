#include                "FBLib_flash.h"

/*
BFC00000-BFC01FFF: uncompressed primary loader
BFC02000-BFC0FFFF: compressed secondary loader
BFC10000-BFC1FFFF: compressed firmware or compressed aXeL tertiary loader
BFC20000-BFDBFFFF: compressed firmware cont. or compressed firmware
BFDC0000-BFDDFFFF: compressed user settings
BFDE0000-BFDFFFFF: compressed factory settings

00: 16  BFC00000–BFC03FFF PL + SL (SL starts at 0xbfc02000 on a TF5000MP)
01:  8  BFC04000–BFC05FFF SL...
02:  8  BFC06000–BFC07FFF SL...
03: 32  BFC08000–BFC0FFFF SL...
04: 64  BFC10000–BFC1FFFF FW    or aXeL
05: 64  BFC20000–BFC2FFFF FW...
06: 64  BFC30000–BFC3FFFF FW...
07: 64  BFC40000–BFC4FFFF FW...
08: 64  BFC50000–BFC5FFFF FW...
09: 64  BFC60000–BFC6FFFF FW...
10: 64  BFC70000–BFC7FFFF FW...
11: 64  BFC80000–BFC8FFFF FW...
12: 64  BFC90000–BFC9FFFF FW...
13: 64  BFCA0000–BFCAFFFF FW...
14: 64  BFCB0000–BFCBFFFF FW...
15: 64  BFCC0000–BFCCFFFF FW...
16: 64  BFCD0000–BFCDFFFF FW...
17: 64  BFCE0000–BFCEFFFF FW...
18: 64  BFCF0000–BFCFFFFF FW...
19: 64  BFD00000–BFD0FFFF FW...
20: 64  BFD10000–BFD1FFFF FW...
21: 64  BFD20000–BFD2FFFF FW...
22: 64  BFD30000–BFD3FFFF FW...
23: 64  BFD40000–BFD4FFFF FW...
24: 64  BFD50000–BFD5FFFF FW...
25: 64  BFD60000–BFD6FFFF FW...
26: 64  BFD70000–BFD7FFFF FW...
27: 64  BFD80000–BFD8FFFF FW...
28: 64  BFD90000–BFD9FFFF FW...
29: 64  BFDA0000–BFDAFFFF FW...
30: 64  BFDB0000–BFDBFFFF FW...
31: 64  BFDC0000–BFDCFFFF Settings
32: 64  BFDD0000–BFDDFFFF Settings...
33: 64  BFDE0000–BFDEFFFF Factory
34: 64  BFDF0000–BFDFFFFF Factory...
*/

bool FlashWrite (byte *SourceAddress, byte *FlashAddress, dword DataSize, void *pPercentFinishedCallback)
{
  dword                 FlashSector, SectorSize, FirstFlashSector, LastFlashSector, i;
  word                  *SA, *FA, *SEA;
  byte                  *FlashMemoryBase = (byte *) 0xbfc00000;

  //PercentFinishedCallback is called for every block. PercentFinished contains a number between 0 and 100
  void (*PercentFinishedCallback) (dword PercentFinished) = pPercentFinishedCallback;

  static bool           (*FWFlashEraseSector)(word *FlashAddress) = NULL;
  static bool           (*FWFlashProgram)(word *FlashAddress, word Data) = NULL;
  static dword          (*FWFlashFindSectorAddressIndex)(dword FlashOffset) = NULL;
  static dword          (*FWFlashGetSectorAddress)(dword SectorIndex) = NULL;

  //Assign their addresses
  if (!(FWFlashEraseSector            = (void*)FIS_fwFlashEraseSector()))            return FALSE;
  if (!(FWFlashProgram                = (void*)FIS_fwFWFlashProgram()))              return FALSE;
  if (!(FWFlashFindSectorAddressIndex = (void*)FIS_fwFlashFindSectorAddressIndex())) return FALSE;
  if (!(FWFlashGetSectorAddress       = (void*)FIS_fwFlashGetSectorAddress()))       return FALSE;

#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FWFlashEraseSector            = %p\n", FWFlashEraseSector);
  TAP_Print ("FWFlashProgram                = %p\n", FWFlashProgram);
  TAP_Print ("FWFlashFindSectorAddressIndex = %p\n", FWFlashFindSectorAddressIndex);
  TAP_Print ("FWFlashGetSectorAddress       = %p\n", FWFlashGetSectorAddress);
#endif

  FirstFlashSector = FWFlashFindSectorAddressIndex (FlashAddress - FlashMemoryBase);
  LastFlashSector = FWFlashFindSectorAddressIndex (FlashAddress + DataSize - 1 - FlashMemoryBase);
  FlashSector = FirstFlashSector;
  SA = (word*)SourceAddress;
  SEA = (word*)(SourceAddress+DataSize);

  while (FlashSector <= LastFlashSector)
  {
    dword FAindex = FWFlashGetSectorAddress (FlashSector);
    SectorSize = FWFlashGetSectorAddress(FlashSector + 1) - FAindex;

    if (PercentFinishedCallback)
    {
      if(LastFlashSector > FirstFlashSector) PercentFinishedCallback ((FlashSector - FirstFlashSector) * 100 / (LastFlashSector - FirstFlashSector));
                                        else PercentFinishedCallback (0);
    }

    FA = (word*)(FlashMemoryBase + FAindex);

    //Security check: do not allow to touch the loader or any address outside the flash
    if (((dword)(FA) < 0xbfc10000) || ((dword)(FA) >= 0xbfe00000)) return FALSE;

    if (memcmp (FA, SA, SectorSize) == 0)
    {
      SA += SectorSize >> 1;
    }
    else
    {
      if (!FWFlashEraseSector (FA)) return FALSE;

      while ((SectorSize > 0) && (SA < SEA))
      {
        if (!FWFlashProgram (FA, *SA)) return FALSE;

        SectorSize -= 2;
        FA++;
        SA++;
      }
    }
    FlashSector++;
  }

  if (PercentFinishedCallback) PercentFinishedCallback (100);

  //Check if the written data is correct
  FA = (word*)FlashAddress;
  SA = (word*)SourceAddress;
  for (i = 0; i < DataSize; i += 2);
  {
    if (*FA != *SA) return FALSE;

    FA++;
    SA++;
  }

  return TRUE;
}
