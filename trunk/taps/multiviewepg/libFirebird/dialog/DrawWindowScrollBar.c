#include                "FBLib_dialog.h"

void DrawWindowScrollBar (void)
{
  static dword          LastPercent = (dword) -1;
  int                   n;
  dword                 Percent, w, x, max;

  if (!FBDialogWindow || (FBDialogWindow->NrItems <= FBDialogWindow->NrItemLines) || !FBDialogProfile) return;

  // for a reasonable scrollbar display
  n = FBDialogWindow->NrItems - (FBDialogWindow->hasCursor ? 1 : FBDialogWindow->NrItemLines);

  Percent = (n == 0 ? 0 : FBDialogWindow->SelectedItem * 100 / n);

  if (Percent != LastPercent) FBDialogWindow->OSDScrollBarForeDirty = TRUE;

  if (FBDialogWindow->OSDScrollBarForeDirty || FBDialogWindow->OSDScrollBarBackDirty)
  {
    if (FBDialogWindow->OSDScrollBarBackDirty) BuildWindowScrollBar();

    if (FBDialogWindow->MemOSDLines)
    {
      w = GetOSDRegionWidth(FBDialogWindow->MemOSDScrollBar) >> 1;

      TAP_Osd_Copy(FBDialogWindow->MemOSDScrollBar, FBDialogWindow->MemOSDScrollBar, 0, 0, w, GetOSDRegionHeight(FBDialogWindow->MemOSDScrollBar), w, 0, TRUE);

      max = GetOSDRegionHeight(FBDialogWindow->MemOSDScrollBar) - (Scroll_End_Gd->height << 1) - Scroll_Cursor_Gd->height;

      TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDScrollBar, w, Scroll_End_Gd->height + (max * Percent / 100), Scroll_Cursor_Gd, TRUE, FBDialogProfile->ScrollBarColor);

      x = FBDialogWindow->OSDWidth - Border_Items_W_Gd->width - Border_Items_E_Gd->width - Scroll_Back_Gd->width + 9;   // Border_Items_E_Gd has a transparent left part

      // draw (into MemOSDLines)
      TAP_Osd_Copy(FBDialogWindow->MemOSDScrollBar, FBDialogWindow->MemOSDLines, w, 0, w, GetOSDRegionHeight(FBDialogWindow->MemOSDScrollBar), x, Items_Line_C_Gd->height, TRUE);

      LastPercent = Percent;
      FBDialogWindow->OSDScrollBarForeDirty = FALSE;
      FBDialogWindow->OSDScrollBarBackDirty = FALSE;
    }
  }
}
