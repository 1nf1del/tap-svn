#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
// This function combines the TAP display data with the display data
// provided by the FW.
// pData is used as input/output pointer. If the function is called from the
// TAP context (indirectly) then pData should be identical to pVFDDisplay->fwData.
//
// IMPORTANT: The function MUST NOT access any global data except for the
// data accessible via pointer passed in the second parameter. Otherwise,
// the compiler will generate instructions based on wrong assumptions.
//
void combineVfdData (byte *pData, tDisplayMemory *pVFDDisplay)
{
  if(pVFDDisplay->displayMode != DISPLAY_FW)
  {
    byte                *pTapData = (byte*)pVFDDisplay->tapData;
    byte                *pFwCopy = (byte*)pVFDDisplay->fwData;
    byte                *pMask;
    int                 i;

    // make a copy if the pointers are not identical
    if(pFwCopy != pData)
    {
      for(i = 0; i < 48; i++)
      {
        pFwCopy[i] = pData[i];
      }
    }

    // select the applicable mask
    pMask = (byte*)StringsMask;

    // merge the FW data and the selected TAP data
    for(i = 0; i < 48; i++)
    {
      // reset bits in the FW display data which correspond to
      // the segments of the selected display
      // the result can be ORed with the masked TAP data
      pData[i] = (pFwCopy[i] & pMask[i]) | (pTapData[i] & ~pMask[i]);
    }
  }
}


//-----------------------------------------------------------------------------
// exitHook() is supposed to execute the two instructions replaced by the
// installed hook and continue with the execution of the firmware code at the
// position right after where the CPU jumped to the hook.
// exitHook() is actually part of the fwHook() function. It has been separated
// for convinience reasons (i.e. now it is easier to replace the instructions
// without performing a search).
void exitHook (void)
{
  __asm__ __volatile__ (
  "nop\n"   /* two placeholders for the replaced FW instruction */
  "nop\n"   /* */
  "nop\n"   /* two placeholders for the j(ump) instruction (back to the FW code) */
  "nop\n");   /* */
}


//-----------------------------------------------------------------------------
// fwHook() is a proxy function which passes the call from the FW to the
// combineVfdData function. The function to be called shall take
// two parameters (in $a0 from the FW and in $a1 a pointer to TAP data).
void fwHook (dword *pData)
{
  (void) pData;
  __asm__ __volatile__ (
  "lui  $05,0x0\n"  /* This register is used for the address of the structure containing display data */
  "or  $05,$05,0x0\n" /* the two instructions are to be replaced with the actual address on start-up */
  "add  $29,$29,-20\n" /* allocate stack space */
  "sw  $31,4($29)\n" /* push return address to stack */
  "sw  $04,0($29)\n" /* push $a0 to stack, because it might be modified in the called function */
  "bal  combineVfdData\n" /* call the function */
  "lw  $04,0($29)\n" /* restore $a0 from stack */
  "lw  $31,4($29)\n" /* restore return address from stack */
  "add  $29,$29,20\n" /* free stack space */
  "b  exitHook\n"); /* jump to the exit code segment */
}
