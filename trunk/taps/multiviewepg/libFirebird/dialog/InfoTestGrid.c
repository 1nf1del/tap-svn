#include                "FBLib_dialog.h"

void InfoTestGrid (void)
{
  int                   x;
  dword                 y;

  if (!FBDialogWindow) return;

  for (x = 10; x < Info_C_Gd->width * FBDialogWindow->NrInfoColumns; x += 10)
    DrawOSDLine (FBDialogWindow->OSDRgn, Border_Info_W_Gd->width + x, FBDialogWindow->InfoY, Border_Info_W_Gd->width + x, FBDialogWindow->InfoSY, (x % 100) ? ARGB (1, 31, 31, 31) : ARGB (1, 10, 10, 31));

  for (y = 10; y < FBDialogWindow->InfoSY - FBDialogWindow->InfoY; y += 10)
    DrawOSDLine (FBDialogWindow->OSDRgn, Border_Info_W_Gd->width, FBDialogWindow->InfoY + y, Border_Info_W_Gd->width + Info_C_Gd->width * FBDialogWindow->NrInfoColumns, FBDialogWindow->InfoY + y, (y % 100) ? ARGB (1, 31, 31, 31) : ARGB (1, 10, 10, 31));
}
