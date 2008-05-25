#include "../main/FBLib_main.h"

dword FIS_fwMHEGStatus (void)
{
  static dword          fwMHEGStatus = 0;
  short                *pMHEG;

  if (!fwMHEGStatus)
  {
    if (!LibInitialized) InitTAPex ();
    if (!LibInitialized) return -1;

    pMHEG = (word *) FindInstructionSequence("27bdffe0 afb50014 afb60018 afbe001c afbf0010 8f9e8b64 13c00007 0080a825 0c070c1a",
                                             "ffffffff ffffffff ffffffff ffffffff ffffffff ffff0000 ffffffff ffffffff ff000000",
                                             0x80190000, 0x80200000, 5, FALSE );

    if (!pMHEG) return 0;

    fwMHEGStatus = (FWgp + pMHEG[1]);
    if (!fwMHEGStatus) return 0;
  }

  return fwMHEGStatus;
}
