#include "FBLib_main.h"

//--------------------------------------- CallFirmware --------------------------------
//
dword CallFirmware (dword pFunction, dword p0, dword p1, dword p2, dword p3)
{
  dword        v0;

  if (!isValidRAMPtr(pFunction)) return 0;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return 0;

  // The function has to be implemented in assembler in order to
  // gain full control over register usage.
  // The firmware makes use of the $gp register which contains
  // a different value when running in TAP context.
  __asm__ __volatile__ (
  "addiu $29,$29,0xffe0\n"        //addiu $sp,$sp,-0x20
  "sw  $31,0x0014($29)\n"         //sw  $ra,0x0014($sp)   save ra
  "sw  $22,0x0018($29)\n"         //sw  $s6,0x0018($sp)   save s6

  "or  $15,%1,$00\n"              //or  $t7,TAP_IndexAddress,$00  ptr to TAP_IndexAddress
  "or  $22,%2,$00\n"              //or  $s6,TAP_TableAddress,$00  ptr to TAP_TableAddress

  "or  $08,$00,%3\n"              //or  $t0,$00,pFunction  beware of the order here ...
  "or  $04,$00,%4\n"              //or  $a0,$00,p0
  "or  $05,$00,%5\n"              //or  $a1,$00,p1
  "or  $06,$00,%6\n"              //or  $a2,$00,p2
  "or  $07,$00,%7\n"              //or  $a3,$00,p3

  "lw  $15,0x0000($15)\n"         //lw  $t7,0x0000($t7)   get TAP_IndexAddress
  "sll  $15,$15,0x05\n"           //sll  $t7,$t7,0x05   TAP_IndexAddress * 32
  "addu $22,$22,$15\n"            //addu $s6,$s6,$t7    offset to this taps TAP_TableAddress

  "lw  $03,0x0000($22)\n"         //lw  $v1,0x0000($s6)   get firmware gp
  "or  $28,$03,$00\n"             //or  $gp,$v1,$00    set gp

  "jal  $08\n"                    //jal  $t0      call function(p0, p1, p2, p3)
  "nop\n"

  "lw  $03,0x0004($22)\n"         //lw  $v1,0x0004($s6)   get tap gp
  "or  $28,$03,$00\n"             //or  $gp,$v1,$00    restore gp

  "lw  $31,0x0014($29)\n"         //lw  $ra,0x0014($sp)   restore ra
  "lw  $22,0x0018($29)\n"         //lw  $s6,0x0018($sp)   restore s6
  "addiu $29,$29,0x0020\n"        //addiu $sp,$sp,0x20
  "or  %0,$2,$0\n"                //firmware return code is in $2
  : "=r"(v0)
  : "r"(TAP_IndexAddress), "r"(TAP_TableAddress), "r"(pFunction), "r"(p0), "r"(p1), "r"(p2), "r"(p3)
  : "$2");

  return v0;
}
