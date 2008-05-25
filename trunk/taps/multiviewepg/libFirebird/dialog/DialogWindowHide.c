#include                "FBLib_dialog.h"

void DialogWindowHide (void)
{
  if (!FBDialogWindow || !FBDialogWindow->isVisible || !FBDialogProfile) return;

  if (FBDialogWindow->Multiple)
  {
    if (FBDialogWindow->MultipleOSDSaveBox)
    {
      TAP_Osd_RestoreBox (FBDialogWindow->OSDRgn, 0, 0, GetOSDRegionWidth(FBDialogWindow->OSDRgn), GetOSDRegionHeight(FBDialogWindow->OSDRgn), FBDialogWindow->MultipleOSDSaveBox);
      TAP_MemFree (FBDialogWindow->MultipleOSDSaveBox);
      FBDialogWindow->MultipleOSDSaveBox = NULL;
    }
    FreeOSDRegion(FBDialogWindow->OSDRgn);   // doesn't wipe out
  }
  else
  {
    DialogWindowAlpha(0);
    TAP_Osd_FillBox (FBDialogWindow->OSDRgn, 0, 0, FBDialogWindow->OSDWidth, FBDialogWindow->OSDHeight, 0);
    TAP_Osd_SetTransparency (FBDialogWindow->OSDRgn, 255);
    TAP_Osd_Delete (FBDialogWindow->OSDRgn);
  }

  FBDialogWindow->OSDRgn = 0;

  FBDialogWindow->isVisible = FALSE;

  if (FBDialogWindow->isNormalMode) TAP_EnterNormal();
}
