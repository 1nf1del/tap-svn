#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
// This function displays the specified string in the 4-symbol display.
void MPDisplayDisplayShortString (char *pString)
{
  dword                 i;
  char                  string[4] = " ";

  if(!bFwHookInstalled || (VFDDisplay.Memory.displayMode == DISPLAY_FW)) return;

  // initialize the TAP display data relevant for the short string
  VFDDisplay.Memory.tapData[1] &= 0xf0;
  VFDDisplay.Memory.tapData[2] = 0x00;
  VFDDisplay.Memory.tapData[3] = 0x00;
  VFDDisplay.Memory.tapData[4] = 0x00;
  VFDDisplay.Memory.tapData[5] &= 0x80;
  VFDDisplay.Memory.tapData[6] &= 0x01;
  VFDDisplay.Memory.tapData[7] &= 0xc0;
  VFDDisplay.Memory.tapData[8] = 0x00;

  // copy the provided string
  i = 0;
  while((i < sizeof(string)) && (pString[i] != 0))
  {
    string[i] = pString[i];
    i++;
  }

  // now map each character to symbol segments and set
  // the symbol at the corresponding position
  for(i = 0; i < sizeof(string); i++)
  {
    // get the symbol offset in the map for characters [0x00 .. 0x7f]
    if(string[i] >= 0)
    {
      // set a digit
      setSymbol14(i, symbolMap14[(int)string[i]]);
    }
    // else nothing to display
  }

  // combine the tap display data with the last FW display data
  // and send the resulting data array to the display
  sendToVfdDisplay(VFDDisplay.Memory.fwData);
  DBG_PRINT("MPDisplayLib: displayShortString\n");
}
