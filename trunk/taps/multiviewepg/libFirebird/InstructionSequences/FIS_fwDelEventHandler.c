#include "../libFireBird.h"

dword FIS_fwDelEventHandler (void)
{
  static dword          *fwDelEvtHndlr0 = NULL;

  if (!fwDelEvtHndlr0)
  {
    fwDelEvtHndlr0 = (dword*)FindInstructionSequence("3c058011 24a5d128 0c0565f0 3404e500",
                                                     "ffff0000 ffff0000 fc000000 ffffffff",
                                                     0x80130000, 0x80180000, 0, FALSE);

    if(fwDelEvtHndlr0) fwDelEvtHndlr0 = (dword*)ABS_ADDR(fwDelEvtHndlr0[2]);
  }

  return (dword)fwDelEvtHndlr0;
}
