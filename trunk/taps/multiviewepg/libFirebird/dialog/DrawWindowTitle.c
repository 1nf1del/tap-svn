#include                "FBLib_dialog.h"

void DrawWindowTitle (void)
{
  dword                 h;

  if (!FBDialogWindow || !FBDialogProfile) return;

  // remember: all Title_*_Gd have transparent upper parts of (the visible part of) Border_N_Gd width

  if (FBDialogWindow->OSDTitleForeDirty || FBDialogWindow->OSDTitleBackDirty)
  {
    if (FBDialogWindow->OSDTitleBackDirty) BuildWindowTitle();

    if (FBDialogWindow->OSDRgn)
    {
      h = GetOSDRegionHeight(FBDialogWindow->MemOSDTitle) >> 1;

      TAP_Osd_Copy(FBDialogWindow->MemOSDTitle, FBDialogWindow->MemOSDTitle, 0, 0, GetOSDRegionWidth(FBDialogWindow->MemOSDTitle), h, 0, h, TRUE);

      if (*FBDialogWindow->LTitle) TAP_Osd_PutS(FBDialogWindow->MemOSDTitle, TITLE_OFFSET_XY, h + TITLE_OFFSET_XY - (FBDialogProfile->FontSize ? 2 : 0), GetOSDRegionWidth(FBDialogWindow->MemOSDTitle) - TITLE_OFFSET_XY, FBDialogWindow->LTitle, FBDialogProfile->TitleTextColor, 0, 0, FBDialogProfile->FontSize, (byte) TRUE, ALIGN_LEFT);
      if (*FBDialogWindow->CTitle) TAP_Osd_PutS(FBDialogWindow->MemOSDTitle, TITLE_OFFSET_XY, h + TITLE_OFFSET_XY - (FBDialogProfile->FontSize ? 2 : 0), GetOSDRegionWidth(FBDialogWindow->MemOSDTitle) - TITLE_OFFSET_XY, FBDialogWindow->CTitle, FBDialogProfile->TitleTextColor, 0, 0, FBDialogProfile->FontSize, (byte) TRUE, ALIGN_CENTER);
      if (*FBDialogWindow->RTitle) TAP_Osd_PutS(FBDialogWindow->MemOSDTitle, TITLE_OFFSET_XY, h + TITLE_OFFSET_XY - (FBDialogProfile->FontSize ? 2 : 0), GetOSDRegionWidth(FBDialogWindow->MemOSDTitle) - TITLE_OFFSET_XY, FBDialogWindow->RTitle, FBDialogProfile->TitleTextColor, 0, 0, FBDialogProfile->FontSize, (byte) TRUE, ALIGN_RIGHT);

      // draw
      TAP_Osd_Copy(FBDialogWindow->MemOSDTitle, FBDialogWindow->OSDRgn, 0, h, GetOSDRegionWidth(FBDialogWindow->MemOSDTitle), h, 0, FBDialogWindow->TitleY, TRUE);

      FBDialogWindow->OSDTitleForeDirty = FALSE;
      FBDialogWindow->OSDTitleBackDirty = FALSE;
    }
  }
}
