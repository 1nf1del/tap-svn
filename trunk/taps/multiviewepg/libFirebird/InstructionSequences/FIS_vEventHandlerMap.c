#include "../libFireBird.h"

dword FIS_vEventHandlerMap (void)
{
  static dword          vEventHandlerMap = 0;

  if (!vEventHandlerMap)
  {
    vEventHandlerMap = FindInstructionSequence("27bdffe8 afbf0014 0c000986 24041800 af829c8c",
                                               "ffff0000 ffff0000 fc000000 ffffffff ffff0000",
                                                0x80130000, 0x80200000, 4, FALSE);

    if (vEventHandlerMap) vEventHandlerMap = *(dword*)(FWgp +  (short)(*(dword*)vEventHandlerMap & 0xffff));
  }

  return vEventHandlerMap;
}
