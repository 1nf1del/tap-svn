#include "FBLib_string.h"

//-------------------------------- isLegalChar ---------------------------
//
bool isLegalChar(unsigned char c, eRemoveChars ControlCharacters)
{
  if ((ControlCharacters & ControlChars) != 0)
  {
    // Anything below 32 is a control character!
    if ((c < 0x20) && (c != 0x0a)) return FALSE;
    // these characters are unused in Windows system font and cause trouble
    if ((c >= 0x7f) && (c <= 0x9f) && (c != 0x80) && (c != 0x8a) && (c != 0x91) && (c != 0x92)) return FALSE;
  }

  if ((ControlCharacters & LFChars) != 0)
  {
    if ((c == 0x0a) || (c == 0x8a)) return FALSE;
  }

  if ((ControlCharacters & InvalidFileNameChars) != 0)
  {
    switch (c)
    {
      // this character will cause a filename not displayed in recording list
      case 'à':

      // These characters are not allowed in Windows.
      case ':':
      case '/':
      case '\\':
      case '*':
      case '?':
      case '"':
      case '<':
      case '>':
      case '|':
        return FALSE;
        break;
    }
  }

  if ((ControlCharacters & NonPrintableChars) != 0) return (c >= ' ' && c <= '~');

  return TRUE;
}
