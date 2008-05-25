#include                "../libFireBird.h"

void PatchUnloadModule (tPatchModule *PatchModule)
{
  if ((void*)PatchModule < (void*)0x80000000) return;

  TAP_MemFree (PatchModule);
  PatchModule = NULL;
}
