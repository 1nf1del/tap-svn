#include "FBLib_mpvfd.h"

void MPDisplayUpdateDisplay (void)
{
  combineVfdData (VFDDisplay.Memory.fwData, &VFDDisplay.Memory);
  sendToVfdDisplay (VFDDisplay.Memory.fwData);
}
