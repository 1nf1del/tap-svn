#include "FBLib_hook.h"
#include "../libFireBird.h"

//--------------------------------------- HookIsEnabled --------------------------------
//
bool HookIsEnabled (dword HookIndex)
{
  return (HookIndex >= NrHooks ? FALSE : Hooks [HookIndex].Enabled);
}
