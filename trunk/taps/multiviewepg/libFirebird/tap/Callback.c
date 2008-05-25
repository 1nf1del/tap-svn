#include "FBLib_tap.h"

//--------------------------------------- Callback --------------------------------
//
dword Callback (int Index, void *ProcedureAddress, dword param1, dword param2, dword param3, dword param4)
{
  tTAPTable* tap = (tTAPTable*)(TAP_TableAddress) + Index;

  if (tap->EntryPoints)
  {
    dword tapGP = TAPgp;    //needs to be a local variable on the stack
    dword ret;

    // any diagnostics must be placed *before* this line
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FBLib: Calling TAP %d, p=%8.8p\n", Index, ProcedureAddress);
#endif
    *(dword*)TAP_IndexAddress = Index;

    ret = CallbackHelper(tap, ProcedureAddress, param1, param2, param3, param4);

    // restore gp for the calling TAP. This must be done within the loop so that
    // currentTAPIndex can be accessed
    asm volatile ( "or $28,%0,$00\n" : : "r"(tapGP));
    *(dword*)TAP_IndexAddress = TAP_TableIndex;

    // any diagnostics must be placed *after* this line
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("done\n");
#endif

    return ret;
  }

  return 0;
}
