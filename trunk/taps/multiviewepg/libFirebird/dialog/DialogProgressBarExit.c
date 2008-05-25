#include                "FBLib_dialog.h"

void DialogProgressBarExit (void)
{
  if (!FBDialogProgressBar || !FBDialogProgressBar->isVisible) return;

  TAP_Osd_Delete (FBDialogProgressBar->MemOSDRgn);

  if (FBDialogProgressBar->OSDSaveBox)
  {
    TAP_Osd_RestoreBox (FBDialogProgressBar->OSDRgn, 0, 0, GetOSDRegionWidth(FBDialogProgressBar->OSDRgn), GetOSDRegionHeight(FBDialogProgressBar->OSDRgn), FBDialogProgressBar->OSDSaveBox);
    TAP_MemFree (FBDialogProgressBar->OSDSaveBox);
    FBDialogProgressBar->OSDSaveBox = NULL;
  }

  FreeOSDRegion(FBDialogProgressBar->OSDRgn);   // doesn't wipe out

  FBDialogProgressBar->isVisible = FALSE;

  if (FBDialogProgressBar->isNormalMode) TAP_EnterNormal();

  FBDialogProgressBar = NULL;
}
