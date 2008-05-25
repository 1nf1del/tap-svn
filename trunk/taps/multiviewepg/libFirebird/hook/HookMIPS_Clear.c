#include "FBLib_hook.h"
#include "../libFireBird.h"

//--------------------------------------- HookMIPS_Clear --------------------------------
//
void HookMIPS_Clear (dword *pFirmware, dword *pHook, dword *pHookReturn)
{
  bool                  FirstInChain = FALSE, LastInChain = FALSE;
  dword                 *p;

  FirstInChain = (ABS_ADDR (pFirmware [0]) == (dword) pHook);
  LastInChain = ((pHookReturn [-1] != 0) || (pHookReturn [-2] != 0));

  if (FirstInChain)
  {
    if (LastInChain)
    {
      //Den Sprung in unserern Hook durch die beiden original OpCodes ersetzen
      pFirmware [0] = pHookReturn [-2];
      pFirmware [1] = pHookReturn [-1];
    }
    else
    {
      //In diesem Fall muß nur der Jump in das nächste TAP in die F/W-Routine gepatcht werden
      pFirmware [0] = pHookReturn [0];
    }
  }
  else
  {
    p = pFirmware;
    do
    {
#ifdef DEBUG_FIREBIRDLIB
      TAP_Print ("FireBirdLib: Search for JMP: %p -> %x\n", p, ABS_ADDR (*p));
#endif
      p = (dword *) ABS_ADDR (*p);
      while ((*p & CMD_MASK) != JMP_CMD)
      {
        p++;
      }
    } while (ABS_ADDR (*p) != (dword) &pHook [0]);
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: Found JMP: %p -> %x\n", p, ABS_ADDR (*p));
#endif

    //Gefunden, p zeigt auf den Forward-JMP des vorhergehenden TAPs der Chain
    if (LastInChain)
    {
      p [-2] = pHookReturn [-2];
      p [-1] = pHookReturn [-1];
      p [ 0] = pHookReturn [ 0];
    }
    else
    {
      p [0] = pHookReturn [0];
    }
  }
}
