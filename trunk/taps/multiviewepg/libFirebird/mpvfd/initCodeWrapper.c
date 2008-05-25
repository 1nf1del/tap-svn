#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
// initCodeWrapper() creates a code wrapper based on TAP_GetTime. The function
// allocates a buffer and copies the code of TAP_GetTime into it except for
// the actual call of the getTime() firmware function. The address of the
// getTime() is replaced with the provided address.
// The wrapped function call supports up to 4 parameters and returns a
// short value.
// Parameters:
//  fwFuncAddr - address of the FW function to be wrapped
// initCodeWrapper() returns a buffer with code wrapping the call to provided
// firmware function address.
// The buffer can be assigned to a function pointer which can be used to execute
// the wrapped code.
dword* initCodeWrapper (dword *pFwFuncAddr)
{
  dword                 i;

  // TAP_GetTime provides a template for wrapper code. The only thing to be
  // changed is the address of the wrapped function.
  dword                 *pSrc = (dword*)TAP_GetTime;
  dword                 *pBuffer;
  dword                 bufSize = 0;
  bool                  found = FALSE;

  // ######################
  // Step 2
  // get a copy of TAP_GetTime()
  pSrc = (dword*)TAP_GetTime;
  bufSize = 0;
  // find the return statement
  while(pSrc[bufSize] != JR_RA_CMD)
  {
    bufSize++;
  }

  // the buffer should include the return statement and the command following
  // the return statement
  bufSize += 2;

  // allocate memory
  pBuffer = malloc(sizeof(dword) * bufSize);

  if(pBuffer == NULL) return NULL;

  // patch the copy with the call to the address of the copied sendToVfdDisplay()
  DBG_PRINT("MPDisplayLib: wrapper started for %08x ...\n", pFwFuncAddr);
  for(i = 0; i < bufSize; i++)
  {
    if((pSrc[i] & CMD_MASK) == JAL_CMD)
    {
      // this is the wrapped call to the actual FW implementation
      // replace it with the call to the provided address
      pBuffer[i] = JAL_CMD | REL_ADDR(pFwFuncAddr);
      found = TRUE;
    }
    else
    {
      pBuffer[i] = pSrc[i];
    }

    DBG_PRINT("MPDisplayLib: %08x: %08x\n", pBuffer + i, pBuffer[i]);
  }

  if(!found)
  {
    free(pBuffer);
    pBuffer = NULL;
  }

  // return the address of the allocated code wrapper
  return pBuffer;
}
