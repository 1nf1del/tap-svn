#include                "../libFireBird.h"
#include                "FBLib_fwpatches.h"

dword *PatchFindNextListEntry (dword *p)
{

  if (!p || !isValidRAMPtr(p)) return NULL;

  //Loop until the next
  do
  {
    p++;

    if (!isValidRAMPtr(p)) return NULL;
  }
  while ((*p != NOP_CMD) && ((*p & 0xffff0000) != LUI_CMD));

  return p;
}
