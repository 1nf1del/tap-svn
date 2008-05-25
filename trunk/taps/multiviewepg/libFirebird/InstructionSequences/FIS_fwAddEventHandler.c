#include "../libFireBird.h"

dword FIS_fwAddEventHandler (void)
{
  static dword          *fwAddEvtHndlr0 = NULL;

  if (!fwAddEvtHndlr0)
  {
    fwAddEvtHndlr0 = (dword*)FindInstructionSequence("3c058011 24a5aea0 3404ef00 00003025 0c054a82 24070064",
                                                     "FFFF0000 FFFF0000 FFFFFFFF FFFFFFFF FC000000 FFFFFFFF",
                                                     0x800C0000, 0x80140000, 0, FALSE);

    if(fwAddEvtHndlr0) fwAddEvtHndlr0 = (dword*)ABS_ADDR(fwAddEvtHndlr0[4]);
  }

  return (dword)fwAddEvtHndlr0;
}
