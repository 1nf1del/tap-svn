#include "FBLib_mpvfd.h"

//-----------------------------------------------------------------------------
void MPDisplayDisplayLongString (char *pString)
{
  dword                 i, j;
  unsigned char         string[8] = "  ";

  if(!bFwHookInstalled || (VFDDisplay.Memory.displayMode == DISPLAY_FW)) return;

  // initialize the TAP display data relevant for the long string (17-25 & 33-41)
  VFDDisplay.Memory.tapData[17] &= 0xc0;
  VFDDisplay.Memory.tapData[18] = 0x00;
  VFDDisplay.Memory.tapData[19] = 0x00;
  VFDDisplay.Memory.tapData[20] = 0x00;
  VFDDisplay.Memory.tapData[21] = 0x00;
  VFDDisplay.Memory.tapData[22] = 0x00;
  VFDDisplay.Memory.tapData[23] = 0x00;
  VFDDisplay.Memory.tapData[24] = 0x00;
  VFDDisplay.Memory.tapData[25] &= 0x03;
  VFDDisplay.Memory.tapData[33] &= 0xc0;
  VFDDisplay.Memory.tapData[34] = 0x00;
  VFDDisplay.Memory.tapData[35] = 0x00;
  VFDDisplay.Memory.tapData[36] = 0x00;
  VFDDisplay.Memory.tapData[37] = 0x00;
  VFDDisplay.Memory.tapData[38] = 0x00;
  VFDDisplay.Memory.tapData[39] = 0x00;
  VFDDisplay.Memory.tapData[40] = 0x00;
  VFDDisplay.Memory.tapData[41] &= 0x03;

  // copy the provided string
  i = 0;
  j = 0;
  while((i < 8) && (pString[j] != 0))
  {
    string[i] = pString[j];

    //Translate German umlaute
    switch (string [i])
    {
      case 0xe4:  //äÄ
      case 0xec:
        string [i] = 0x41;
        string [i+1] = 0x45;
        i++;
        break;

      case 0xf6:  //öÖ
      case 0xd6:
        string [i] = 0x4F;
        string [i+1] = 0x45;
        i++;
        break;

      case 0xfc:  //
      case 0xdc:
        string [i] = 0x55;
        string [i+1] = 0x45;
        i++;
        break;

      case 0xc8:  //E
      case 0xc9:
      case 0xca:
      case 0xcb:
      case 0xe8:
      case 0xe9:
      case 0xea:
      case 0xeb:
        string [i] = 0x45;
        break;

      case 0xb2:  //²
        string [i] = 0x32;
        break;

      case 0xb3:  //³
        string [i] = 0x33;
        break;

      case 0xc0:  //A
      case 0xc1:
      case 0xc2:
      case 0xc3:
      case 0xc4:
      case 0xc5:
      case 0xc6:
      case 0xe0:
      case 0xe1:
      case 0xe2:
      case 0xe3:
      case 0xe5:
      case 0xe6:
        string [i] = 0x41;
        break;

      case 0xcc:  //I
      case 0xcd:
      case 0xce:
      case 0xcf:
      case 0xed:
      case 0xee:
      case 0xef:
        string [i] = 0x49;
        break;

      case 0xd2:  //O
      case 0xd3:
      case 0xd4:
      case 0xd5:
      case 0xd8:
      case 0xf2:
      case 0xf3:
      case 0xf4:
      case 0xf5:
      case 0xf8:
        string [i] = 0x4F;
        break;

      case 0xd9:  //U
      case 0xda:
      case 0xdb:
      case 0xf9:
      case 0xfa:
      case 0xfb:
        string [i] = 0x55;
        break;

      case 0xff:  //Y
        string [i] = 0x59;
        break;

      case 0xd0:  //G
      case 0xf0:
        string [i] = 0x47;
        break;

      case 0xd1:  //N
      case 0xf1:
        string [i] = 0x4e;
        break;

      case 0xde:  //S
      case 0xfe:
        string [i] = 0x53;
        break;

    }
    i++;
    j++;
  }

  // now map each character to symbol segments and set
  // the symbol at the corresponding position
  for(i = 0; i < 8; i++)
  {
    // get the symbol offset in the map for characters [0x00 .. 0x7f]
    if(string[i] < 0x80)
    {
      setSymbol17(i, symbolMap17[(int)string[i]]);
    }
    // else nothing to display
  }

  // combine the tap display data with the last FW display data
  // and send the resulting data array to the display
  sendToVfdDisplay(VFDDisplay.Memory.fwData);
  DBG_PRINT("MPDisplayLib: displayShortString\n");
}
