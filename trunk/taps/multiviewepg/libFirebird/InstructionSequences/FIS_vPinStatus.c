#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vPinStatus (void)
{
  dword                 *EP;
  static dword          pIRDLocked = 0;

  if (!pIRDLocked)
  {
    if (!LibInitialized) InitTAPex ();
    if (!LibInitialized) return 0;

    EP = (dword*) FindInstructionSequence ("3404e300 02a02825 00003025 0c054965 240700f1 8f988974",
                                           "ffffffff fc1fffff ffffffff fc000000 ffffffff ffe00000",
                                           0x80150000, 0x80200000, 5, FALSE);

    if (EP) pIRDLocked = (FWgp + (short) (EP [0] & 0x0000ffff));
  }

  return pIRDLocked;
}
