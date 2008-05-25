#include "../libFireBird.h"

dword FIS_fwSendToFP (void)
{
  static dword          fwSendFrontPanelString = 0;

  if (!fwSendFrontPanelString)
  {
    //Find the entry point of the 'SendFrontPanelString' procedure
    //The 5000 and MP 'SendFrontPanelString' routines are different at the beginning, therefore we have to
    //find a common point inside of the routine and step back until we reach the “addiu $sp” opcode.
    fwSendFrontPanelString = FindInstructionSequence ("0080f025 93d90000 3335000f 24040002 0c00111c 00000000",
                                                      "fc0007ff fc00ffff fc00ffff ffffffff fc000000 ffffffff",
                                                       0x80000000, 0x80010000, 0, TRUE);
  }

  return fwSendFrontPanelString;
}
