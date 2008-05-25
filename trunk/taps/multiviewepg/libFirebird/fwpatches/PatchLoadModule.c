#include                "../libFireBird.h"

//Return codes: 1 = file not found
//              2 = failed to open the file
//              3 = failed to reserve memory
//              4 = invalid file
//              5 = unsupported tfp version
//   >=0x80000000 = Pointer to the patch module. Use TAP_MemFree after processing the patch
//
tPatchModule *PatchLoadModule (char *FileName)
{
  TYPE_File             *fp;
  byte                  *PatchFile = NULL;
  dword                 i, j, k;
  byte                  *FilePos, *BufferPos;
  tPatchModule          *PatchModule = NULL;
  dword                 BlockSize, totSegments = 0, totCodeLens = 0;

  tPatchDetails         *PatchDetails = NULL;
  tPatchSegment         *PatchSegment = NULL;

  //Load the patch file
  if (!TAP_Hdd_Exist (FileName)) return (void*)1;
  if ((fp = TAP_Hdd_Fopen (FileName)) == NULL) return (void*)2;
  i = TAP_Hdd_Flen (fp);

  if (!(PatchFile = TAP_MemAlloc (i)))
  {
    TAP_Hdd_Fclose (fp);
    return (void*)3;
  }
  TAP_Hdd_Fread (PatchFile, i, 1, fp);
  TAP_Hdd_Fclose (fp);

  FilePos = PatchFile;

  PatchModule = (tPatchModule*)FilePos;

  //The file has been created on an Intel/AMD PC, we need to swap endians
  PatchModule->FileVersion = LE32 (PatchModule->FileVersion);
  PatchModule->NrOfPatches = LE32 (PatchModule->NrOfPatches);

  if (PatchModule->Magic != 0x5446501a)
  {
    TAP_MemFree (PatchFile);
    return (void*)4;
  }

  if (PatchModule->FileVersion != 0x0004)
  {
    TAP_MemFree (PatchFile);
    return (void*)5;
  }

  //Count the number of segments and code lengths
  FilePos += sizeof (tPatchModule) - 4; //Subtract the *PatchDetails pointer;

  for (i = 0; i < PatchModule->NrOfPatches; i++)
  {
    PatchDetails = (tPatchDetails*)FilePos;
    PatchDetails->DestinationGP = LE32 (PatchDetails->DestinationGP);
    PatchDetails->NrOfSegments = LE32 (PatchDetails->NrOfSegments);
    totSegments += PatchDetails->NrOfSegments;
    FilePos += sizeof (tPatchDetails) - 4;
    for (j = 0; j < PatchDetails->NrOfSegments; j++)
    {
      PatchSegment = (tPatchSegment*)FilePos;
      PatchSegment->Address = LE32 (PatchSegment->Address);
      PatchSegment->CodeLength = LE32 (PatchSegment->CodeLength);
      totCodeLens += PatchSegment->CodeLength;
      FilePos += sizeof (tPatchSegment) + (PatchSegment->CodeLength << 4) - 16;
    }
  }

  //Calculate the block size to be allocated
  BlockSize = sizeof (tPatchModule) - 4
              + PatchModule->NrOfPatches * (sizeof (tPatchDetails) + sizeof (dword)) //Leave space for the *PatchDetails pointers
              + totSegments * (sizeof (tPatchSegment) + sizeof (dword))              //Leave space for the *PatchSegment pointers
              + (totCodeLens << 4);

  PatchModule = TAP_MemAlloc (BlockSize);
  if (!PatchModule)
  {
    TAP_MemFree (PatchFile);
    return (void*)3;
  }
  memset (PatchModule, 0, BlockSize);   //Can be removed after debugging

  //Now we just have to fill the structure and direct all pointers to the correct location
  FilePos = PatchFile;
  BufferPos = (byte*)PatchModule;
  memcpy (BufferPos, FilePos, sizeof (tPatchModule) - 4);

  FilePos   += (sizeof (tPatchModule) - 4);
  BufferPos += (sizeof (tPatchModule) - 4 + (PatchModule->NrOfPatches << 2)); //Leave space for the *PatchDetails pointers

  for (i = 0; i < PatchModule->NrOfPatches; i++)
  {
    tPatchDetails      *det;

    det = PatchModule->PatchDetails [i] = (tPatchDetails*)BufferPos;
    memcpy (BufferPos, FilePos, sizeof (tPatchDetails) - 4);

    FilePos   += (sizeof (tPatchDetails) - 4);
    BufferPos += (sizeof (tPatchDetails) - 4 + (det->NrOfSegments << 2));  //Leave space for the *PatchSegment pointers

    for (j = 0; j < PatchModule->PatchDetails[i]->NrOfSegments; j++)
    {
      tPatchSegment     *seg;

      seg = det->PatchSegment [j] = (tPatchSegment*)BufferPos;
      memcpy (BufferPos, FilePos, sizeof (tPatchSegment) - 16);

      seg->OriginalCode = (dword*)(BufferPos + sizeof (tPatchSegment));
      seg->SearchMask   = (dword*)(BufferPos + sizeof (tPatchSegment) + (     seg->CodeLength << 2));
      seg->PatchedCode  = (dword*)(BufferPos + sizeof (tPatchSegment) + (2 * (seg->CodeLength << 2)));
      seg->PatchMask    = (dword*)(BufferPos + sizeof (tPatchSegment) + (3 * (seg->CodeLength << 2)));
      FilePos   += (sizeof (tPatchSegment) - 16);
      BufferPos +=  sizeof (tPatchSegment);

      memcpy (BufferPos, FilePos, seg->CodeLength << 4);
      for (k = 0; k < seg->CodeLength; k++)
      {
        seg->OriginalCode [k] = LE32 (seg->OriginalCode [k]);
        seg->SearchMask [k] = LE32 (seg->SearchMask [k]);
        if (det->DestinationGP != 0) seg->SearchMask [k] = 0xffffffff;
        seg->PatchedCode [k] = LE32 (seg->PatchedCode [k]);
        seg->PatchMask [k] = LE32 (seg->PatchMask [k]);
      }
      FilePos   += (seg->CodeLength << 4);
      BufferPos += (seg->CodeLength << 4);
    }
  }

  TAP_MemFree (PatchFile);

  return PatchModule;
}
