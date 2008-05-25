#include "../libFireBird.h"

void intUnlock (dword RS)
{
  (void) RS;
  __asm__ __volatile__ (
  "sll  $26,1\n"            // sll   $k0, 1
  "mtc0  $04,$12\n"         // mtc0   $a0, R12
  "nop\n"
  "nop\n"
  "nop\n"
  "srl  $26,1\n");          // srl   $k0, 1
}
