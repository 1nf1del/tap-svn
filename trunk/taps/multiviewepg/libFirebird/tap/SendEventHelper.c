#include "FBLib_tap.h"

//--------------------------------------- SendEventHelper --------------------------------
//
dword SendEventHelper(tTAPTable* tap, word event, dword param1, dword param2 )
{
  dword                 v0;

  (void) tap;
  (void) event;
  (void) param1;
  (void) param2;

  __asm__ __volatile__
  (
    "addiu  $sp, -4\n"
    "sw     $31,0($sp)\n"     // save ra
    "lw     $28,4($04)\n"     // load gp with TAP gp value
    "lw     $08,16($04)\n"    // get TAP_EventHandler
    "lw     $09,28($08)\n"
    "addiu  $04,$05,0\n"
    "addiu  $05,$06,0\n"
    "addiu  $06,$07,0\n"
    "jalr   $09\n"
    "nop\n"
    "lw     $31,0($sp)\n"    // restore ra
    "addiu  $sp,4\n"
    "or  %0,$2,$0\n"          //firmware return code is in $2
    : "=r"(v0)
    :
    : "$2");

  return v0;
}
