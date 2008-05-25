#include "FBLib_hook.h"
#include "../libFireBird.h"

//--------------------------------------- HookEnable --------------------------------
//
void HookEnable (dword HookIndex, bool Enable)
{
  if (HookIndex >= NrHooks) return;

  if (Enable && !Hooks [HookIndex].Enabled)
  {
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: Enabling hook #%d\n", HookIndex);
#endif
    Hooks [HookIndex].pFWSource [0] = JMP_CMD | REL_ADDR ((dword) Hooks [HookIndex].pHookHandler);
    Hooks [HookIndex].pFWSource [1] = NOP_CMD;
    Hooks [HookIndex].Enabled = TRUE;
  }

  if (!Enable && Hooks [HookIndex].Enabled)
  {
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: Disabling hook #%d\n", HookIndex);
#endif
    memcpy (&Hooks [HookIndex].pFWSource [0], &Hooks [HookIndex].OriginalWords [0], 8);
    Hooks [HookIndex].Enabled = FALSE;
  }
}
