#include "FBLib_hook.h"
#include "../libFireBird.h"

//--------------------------------------- HookExit --------------------------------
//
void HookExit ()
{
  dword                 i;

  for (i = 0; i < NrHooks; i++)
    if (Hooks [i].Enabled) HookEnable (i, FALSE);
}
