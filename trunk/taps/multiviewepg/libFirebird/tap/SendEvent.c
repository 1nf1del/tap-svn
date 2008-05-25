#include "FBLib_tap.h"

//--------------------------------------- SendEvent --------------------------------
//
dword SendEvent (int Index, word event, dword param1, dword param2)
{
  tTAPTable* tap = (tTAPTable*)(TAP_TableAddress) + Index;

  if (tap->EntryPoints)
  {
    dword tapGP = TAPgp;    //needs to be a local variable on the stack

    // any diagnostics must be placed *before* this line
    *(dword*)TAP_IndexAddress = Index;

    param1 = SendEventHelper (tap, event, param1, param2);

    // restore gp for the calling TAP. This must be done within the loop so that
    // currentTAPIndex can be accessed
    asm volatile ( "or $28,%0,$00\n" : : "r"(tapGP));
    *(dword*)TAP_IndexAddress = TAP_TableIndex;

    // any diagnostics must be placed *after* this line
  }
  return param1;
}
