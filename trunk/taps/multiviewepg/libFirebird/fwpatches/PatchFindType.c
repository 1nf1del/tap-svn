#include                "../libFireBird.h"
#include                "FBLib_fwpatches.h"

//Return codes: 0 = Not intended for this firmware
//              1 = Not patched
//              2 = Already patched
//              3 = Not tested with this firmware
//              4 = Already patched but can't be removed
//

dword PatchFindType (dword *UncompressedFirmware, dword SizeOfFirmware, tPatchModule *PatchModule, tPatchData *PatchData)
{
  dword                 i, j, k, l;
  bool                  Original, Patched;
  dword                 gp;
  tPatchDetails         *details;
  tPatchSegment         *seg;

  if ((dword)PatchModule < 0x80000000) return 0;

  //Calculate the $gp register of the loaded FW
  gp = FIS_GetGP (UncompressedFirmware);

  PatchData->OriginalCount = 0;
  PatchData->PatchedCount  = 0;

  for (j = 0; j < PatchModule->NrOfPatches; j++)
  {
    //tPatchDetails *details = (tPatchDetails*)&PatchModule->PatchDetails [j];
    details = (tPatchDetails*)PatchModule->PatchDetails [j];

    //Check if the F/W's $gp matches one of the included patch $gp
    if (gp == details->DestinationGP)
    {

      for (k = 0; k < details->NrOfSegments; k++)
      {
        //tPatchSegment *seg = (tPatchSegment*)&details->PatchSegment [k];
        seg = (tPatchSegment*)details->PatchSegment [k];

        Original = TRUE;
        Patched  = TRUE;
        for (l = 0; l < seg->CodeLength; l++)
        {
          if ((UncompressedFirmware [((seg->Address - 0x80000000) >> 2) + l] != seg->OriginalCode [l])) Original = FALSE;
          if ((UncompressedFirmware [((seg->Address - 0x80000000) >> 2) + l] != seg->PatchedCode [l]))  Patched = FALSE;
        }
        if (Original) PatchData->OriginalCount++;
        if (Patched)  PatchData->PatchedCount++;
      }

      if ((PatchData->OriginalCount == details->NrOfSegments) && (PatchData->PatchedCount == 0))
      {
        PatchData->PatchType     = 1;
        PatchData->PatchSubindex = j;
        return PatchData->PatchType;
      }

      if ((PatchData->PatchedCount == details->NrOfSegments) && (PatchData->OriginalCount == 0))
      {
        PatchData->PatchType     = 2;
        PatchData->PatchSubindex = j;
        return PatchData->PatchType;
      }

      PatchData->PatchType     = 0;
      PatchData->PatchSubindex = 0;
      return PatchData->PatchType;
    }
  }

  PatchData->OriginalCount = 0;
  PatchData->PatchedCount  = 0;

  for (j = 0; j < PatchModule->NrOfPatches; j++)
  {
    //tPatchDetails *details = (tPatchDetails*)&PatchModule->PatchDetails [j];
    details = (tPatchDetails*)PatchModule->PatchDetails [j];

    //if $gp = 0 then this is a generic code sequence
    if (details->DestinationGP == 0)
    {
      int removable = TRUE;
      PatchData->OriginalCount = 0;
      PatchData->PatchedCount = 0;

      for (k = 0; k < details->NrOfSegments; k++)
      {
        if((PatchData->OriginalCount != k) && (PatchData->PatchedCount != k))
        {
          // Shortcut failed patch
          PatchData->OriginalCount = 0;
          PatchData->PatchedCount = 0;
          break;
        }
        seg = details->PatchSegment [k];

        // Check whether patch sets bits that weren't matched by search
        // (i.e. original code will be unknown once patched)
        for (l = 0; l < seg->CodeLength; l++)
        {
          if(seg->PatchMask [l] != (seg->SearchMask [l] & seg->PatchMask [l]))
          {
             removable = FALSE;
             break;
          }
        }

        for (i = 0; i < SizeOfFirmware; i++)
        {
          Original = TRUE;
          Patched  = TRUE;
          if ((seg->Address == 0) || (seg->Address == ((i << 2) | 0x80000000)))
          {
            for (l = 0; l < seg->CodeLength; l++)
            {
              dword  OrigInstr, OrigExp, PatchInstr, PatchExp;

              OrigInstr   = UncompressedFirmware [i + l] & seg->SearchMask [l];
              OrigExp  = seg->OriginalCode [l] & seg->SearchMask [l];
              PatchInstr  = UncompressedFirmware [i + l] & (seg->SearchMask [l] | seg->PatchMask [l]);
              PatchExp = (seg->OriginalCode [l] & seg->SearchMask [l] & ~seg->PatchMask [l]) | (seg->PatchedCode [l] & seg->PatchMask [l]);

              if (OrigInstr  != OrigExp)  Original = FALSE;
              if (PatchInstr != PatchExp) Patched = FALSE;

              if (!Original && !Patched) break;
            }
            if (Original != Patched) seg->Address = (i << 2) + 0x80000000;
            if (Original) PatchData->OriginalCount++;
            if (Patched)  PatchData->PatchedCount++;
          }
        }
      }

      if ((PatchData->OriginalCount == details->NrOfSegments) && (PatchData->PatchedCount == 0))
      {
        PatchData->PatchType     = 3;
        PatchData->PatchSubindex = j;
        return PatchData->PatchType;
      }

      if ((PatchData->PatchedCount == details->NrOfSegments) && (PatchData->OriginalCount == 0))
      {
        PatchData->PatchType     = removable ? 2 : 4;
        PatchData->PatchSubindex = j;
        return PatchData->PatchType;
      }
    }
  }

  PatchData->PatchType     = 0;
  PatchData->PatchSubindex = 0;
  return PatchData->PatchType;
}
