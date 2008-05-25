#include <stdlib.h>
#include "../libFireBird.h"

//--------------------------------------- PatchInstructionSequence --------------------------------
//
bool PatchInstructionSequence (dword PatchAddress, char PatchPattern [200], char PatchMask [200])
{
  dword                 PP [50], PM [50];
  dword                 i, NrOfInstr;
  dword                 *a;

  if (!isValidRAMPtr(PatchAddress) || (strlen(PatchPattern) != strlen(PatchMask))) return FALSE;

  //Copy the hex string into the dword arrays
  NrOfInstr = 0;
  for (i = 0; i < strlen (PatchPattern); i += 9)
  {
    PP [NrOfInstr] = strtoul (&PatchPattern [i], NULL, 16);
    PM [NrOfInstr] = strtoul (&PatchMask [i], NULL, 16);
    PP [NrOfInstr] &= PM [NrOfInstr];

    NrOfInstr++;
  }

  a = (dword*)PatchAddress;
  for (i = 0; i < NrOfInstr; i++)
  {
    a[i] = PP[i] | (a[i] & ~PM[i]);
  }

  return TRUE;
}
