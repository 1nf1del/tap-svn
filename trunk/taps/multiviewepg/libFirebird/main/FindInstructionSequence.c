#include <stdlib.h>
#include "../libFireBird.h"

//--------------------------------------- FindInstructionSequence --------------------------------
//
dword FindInstructionSequence (char SearchPattern [200], char SearchMask [200], dword StartAddress, dword EndAddress, int EntryPointOffset, bool SearchForPrevADDIUSP)
{
  dword                 SP [50], SM [50];
  dword                 i, p, NrOfInstr;
  bool                  OK;

  if (!isValidRAMPtr(StartAddress) || !isValidRAMPtr(EndAddress) || (strlen(SearchPattern) != strlen(SearchMask))) return 0;

  //Copy the hex string into the dword arrays
  NrOfInstr = 0;
  for (i = 0; i < strlen (SearchPattern); i += 9)
  {
    SP [NrOfInstr] = strtoul (&SearchPattern [i], NULL, 16);
    SM [NrOfInstr] = strtoul (&SearchMask [i], NULL, 16);
    SP [NrOfInstr] &= SM [NrOfInstr];

    NrOfInstr++;
  }

  for (p = StartAddress; p < EndAddress; p += 4)
  {
    OK = TRUE;
    for (i = 0; i < NrOfInstr; i++)
    {
      if (((* (dword *) (p + (i << 2))) & SM [i]) != SP [i])
      {
        OK = FALSE;
        break;
      }
    }

    if (OK)
    {
      if (SearchForPrevADDIUSP)
      {
       //FireBird 2006-11-28: also stop on JMP to catch hooked functions
       while ((((*(dword *) p) & 0xffff0000) != 0x27bd0000) && (((*(dword *) p) & CMD_MASK) != JMP_CMD))
       {
         p -= 4;

         //ibbi 2007-01-07: at which address should be stopped anyway if there is no PrevADDIUSP?
         if (p < StartAddress) return 0;
       }
      }
      return p + (EntryPointOffset << 2);
    }
  }

  return 0;
}
