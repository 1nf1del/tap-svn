#include "FBLib_main.h"

//---------------------------------------  SuppressedAutoStart --------------------------------
//
bool SuppressedAutoStart (void)
{
  byte                 *i;

  if (!(i = (byte*)FIS_vSuppressedAutoStart())) return FALSE;

  return *i != 0;
}
