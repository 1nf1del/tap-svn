#include "FBLib_hook.h"
#include "../libFireBird.h"

//--------------------------------------- HookMIPS_Set --------------------------------
//
void HookMIPS_Set (dword *pFirmware, dword *pHook, dword *pHookReturn)
{
  bool                  AlreadyHooked = FALSE;
  dword                 *p;

  AlreadyHooked = ((pFirmware [0] & CMD_MASK) == JMP_CMD);
#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FireBirdLib: F/W=%8.8p, Hook=%8.8p, HookRet=%8.8p, AlreadyHooked = %s\n", pFirmware, pHook, pHookReturn, AlreadyHooked ? "Yes" : "No");
#endif

  if (!AlreadyHooked)
  {
    //Wir sind die Ersten, die den Hook setzen
    //Patche die beiden OpCodes der Original-Routine, die durch unseren Hook überschrieben werden
    pHookReturn [-2] = pFirmware [0];
    pHookReturn [-1] = pFirmware [1];

    //Jetzt noch der Return-Jump
    pHookReturn [0] = JMP_CMD | REL_ADDR ((dword) pFirmware + 8);

    //Aktiviere den Hook durch Umleiten auf unseren MIPS-Code. Ab jetzt wird scharf geschossen.
    pFirmware [0] = JMP_CMD | REL_ADDR ((dword) pHook);
    pFirmware [1] = NOP_CMD;
  }
  else
  {
    //Ein anderes TAP hat bereits einen Hook gesetzt, wir müssen dieses suchen
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
    } while (ABS_ADDR (*p) != ((dword) &pFirmware [0] + 8));
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: Found JMP: %p -> %x\n", p, ABS_ADDR (*p));
#endif

    //Gefunden, p zeigt auf den Return-JMP des letzten TAPs der Chain. Kopiere die beiden original OpCodes und leite den Jump auf uns um
    pHookReturn [-2] = p [-2];
    pHookReturn [-1] = p [-1];

    //Jetzt noch der Return-Jump
    pHookReturn [0] = JMP_CMD | REL_ADDR ((dword) pFirmware + 8);

    p [0] = JMP_CMD | REL_ADDR ((dword) pHook);
    p [-2] = 0;
    p [-1] = 0;
  }
}
