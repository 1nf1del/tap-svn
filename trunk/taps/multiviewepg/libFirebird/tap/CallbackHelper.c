#include "FBLib_tap.h"

//--------------------------------------- CallbackHelper --------------------------------
//
dword CallbackHelper(tTAPTable* tap, void *ProcedureAddress, dword param1, dword param2, dword param3, dword param4)
{
  dword                 v0;

  (void) tap;
  (void) ProcedureAddress;
  (void) param1;
  (void) param2;
  (void) param3;
  (void) param4;

  __asm__ __volatile__
  (
    "addiu  $sp, -4\n"
    "sw     $31,0($sp)\n"     // save ra
    "lw     $28,4($04)\n"     // lw $gp, 4($a0)  load gp with TAP gp value
    "addiu  $09,$05,0\n"      // addiu $t1,$a1,0

    "addiu  $04,$06,0\n"      // addiu $a0,$a2,0
    "addiu  $05,$07,0\n"      // addiu $a1,$a3,0
    "lw     $06,20($sp)\n"    // lw $a2,20($sp)
    "lw     $07,24($sp)\n"    // lw $a3,24($sp)
    "jalr   $09\n"            // jalr $ra,$t1
    "nop\n"
    "lw     $31,0($sp)\n"     // restore ra
    "addiu  $sp,4\n"
    "or  %0,$2,$0\n"          //firmware return code is in $2
    : "=r"(v0)
    :
    : "$2");

  return v0;
}
