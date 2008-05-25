#include                "../libFireBird.h"

//Return codes: 0 = no change
//              1 = patch installed
//              2 = patch removed
//
dword PatchApply (dword *UncompressedFirmware, tPatchModule *PatchModule, tPatchData *PatchData, bool Install)
{
  dword                 k, l;

  if ((dword)PatchModule < 0x80000000) return 0;

  //Check if a patch should be installed
  if (((PatchData->PatchType & 1) != 0) && Install)
  {
    // Check patch is still applicable
    for (k = 0; k < PatchModule->PatchDetails [PatchData->PatchSubindex]->NrOfSegments; k++)
    {
      tPatchSegment *seg = PatchModule->PatchDetails [PatchData->PatchSubindex]->PatchSegment [k];
      for (l = 0; l < seg->CodeLength; l++)
        if((UncompressedFirmware [((seg->Address - 0x80000000) >> 2) + l] & seg->SearchMask [l])
      != (seg->OriginalCode [l] & seg->SearchMask [l]))
            return 0;
    }

    for (k = 0; k < PatchModule->PatchDetails [PatchData->PatchSubindex]->NrOfSegments; k++)
    {
      tPatchSegment *seg = PatchModule->PatchDetails [PatchData->PatchSubindex]->PatchSegment [k];
      for (l = 0; l < seg->CodeLength; l++)
        UncompressedFirmware [((seg->Address - 0x80000000) >> 2) + l] =
          (seg->PatchedCode [l] & seg->PatchMask [l]) | (UncompressedFirmware [((seg->Address - 0x80000000) >> 2) + l] & ~(seg->PatchMask [l]));
    }
    PatchData->PatchType = 2;
    return 1;
  }

  //Check if a patch should be uninstalled
  if ((PatchData->PatchType == 2) && !Install)
  {
    // Check patch is still applied and removable
    for (k = 0; k < PatchModule->PatchDetails [PatchData->PatchSubindex]->NrOfSegments; k++)
    {
      tPatchSegment *seg = PatchModule->PatchDetails [PatchData->PatchSubindex]->PatchSegment [k];
      for (l = 0; l < seg->CodeLength; l++)
      {
        if(seg->PatchMask [l] != (seg->SearchMask [l] & seg->PatchMask [l]))
           return 0; // Not removable!

        if((UncompressedFirmware [((seg->Address - 0x80000000) >> 2) + l] & (seg->SearchMask [l] | seg->PatchMask [l]))
          != ((seg->OriginalCode [l] & seg->SearchMask [l] & ~seg->PatchMask [l]) | (seg->PatchedCode [l] & seg->PatchMask [l])))
            return 0; // Not applied!
      }
    }

    for (k = 0; k < PatchModule->PatchDetails [PatchData->PatchSubindex]->NrOfSegments; k++)
    {
      tPatchSegment *seg = PatchModule->PatchDetails [PatchData->PatchSubindex]->PatchSegment [k];
      for (l = 0; l < seg->CodeLength; l++)
      {
        dword* curr = &UncompressedFirmware [((seg->Address - 0x80000000) >> 2) + l];
        *curr = (*curr & ~seg->PatchMask [l]) | (seg->OriginalCode [l] & seg->PatchMask [l]);
      }
    }
    PatchData->PatchType = 1;
    return 2;
  }

  return 0;
}
