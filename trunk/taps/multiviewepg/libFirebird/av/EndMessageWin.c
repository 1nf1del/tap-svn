#include "FBLib_av.h"
#include "../libFireBird.h"

// ------------------------------ EndMessageWin ------------------------------------
//
void EndMessageWin (void)
{
  if (fbl_rgn) TAP_Osd_Delete(fbl_rgn);
  fbl_rgn = 0;
}
