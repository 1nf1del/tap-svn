#include "FBLib_mpvfd.h"

#include <string.h>

void MPDisplayClearDisplay (void)
{
  memset (VFDDisplay.Memory.tapData, 0, sizeof(VFDDisplay.Memory.tapData));
  memset (VFDDisplay.Memory.fwData, 0, sizeof(VFDDisplay.Memory.fwData));
}
