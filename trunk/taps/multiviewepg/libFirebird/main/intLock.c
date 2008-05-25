#include "../libFireBird.h"

dword intLock (void)
{
  dword                 v0;

  __asm__ __volatile__
  (
    "sll  $26,1\n"            // sll    $k0, 1
    "mfc0  $02,$12\n"         // mfc0   $v0, R12
    "nop\n"                   // nop
    "andi  $09,$02,0x1\n"     // andi   $t1, $v0, 0x01
    "beq  $09,0,24\n"         // beq    $t1, 0, 2cr
    "xori  $09,$02,0x1\n"     // xori   $t1, $v0, 0x01
    "mtc0  $09,$12\n"         // mtc0   $t1, R12
    "nop\n"
    "nop\n"
    "nop\n"
    "srl  $26,1\n"            // srl    $k0, 1
    "or  %0,$2,$0\n"          //firmware return code is in $2
    : "=r"(v0)
    :
    : "$2");

  return v0;
}
