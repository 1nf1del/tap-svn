#include                "FBLib_dialog.h"

void BuildWindowInfo (void)
{
  dword                 InfoNCWidth, InfoSCWidth;

  if (!FBDialogWindow || !FBDialogProfile) return;

  // cache info's northern border line

  InfoNCWidth = FBDialogWindow->NrInfoNColumns * InfoN_C_Gd->width;

  if (!FBDialogWindow->MemOSDInfoN || !isOSDRegionAlive(FBDialogWindow->MemOSDInfoN))
    FBDialogWindow->MemOSDInfoN = TAP_Osd_Create(0, 0, InfoN_W_Gd->width + InfoNCWidth + InfoN_E_Gd->width, InfoN_C_Gd->height, 0, OSD_Flag_MemRgn);

  // fill cache

  TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDInfoN, 0, 0, InfoN_W_Gd, TRUE, FBDialogProfile->InfoBackgroundColor);

  if (FBDialogWindow->NrInfoNColumns)
  {
    TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDInfoN, InfoN_W_Gd->width, 0, InfoN_C_Gd, TRUE, FBDialogProfile->InfoBackgroundColor);
    OSDCopy(FBDialogWindow->MemOSDInfoN, InfoN_W_Gd->width, 0, InfoN_C_Gd->width, InfoN_C_Gd->height, FBDialogWindow->NrInfoNColumns - 1, X);
  }

  TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDInfoN, InfoN_W_Gd->width + InfoNCWidth, 0, InfoN_E_Gd, TRUE, FBDialogProfile->InfoBackgroundColor);

  // cache info lines
  if (!FBDialogWindow->MemOSDInfo || !isOSDRegionAlive(FBDialogWindow->MemOSDInfo))
    FBDialogWindow->MemOSDInfo = TAP_Osd_Create(0, 0, Info_W_Gd->width + FBDialogWindow->NrInfoColumns * Info_C_Gd->width, (FBDialogWindow->NrInfoLines + 1) * Info_C_Gd->height, 0, OSD_Flag_MemRgn);

  // fill cache

  TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDInfo, 0, 0, Info_W_Gd, TRUE, FBDialogProfile->InfoBackgroundColor);

  if (FBDialogWindow->NrInfoColumns)
  {
    TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDInfo, Info_W_Gd->width, 0, Info_C_Gd, TRUE, FBDialogProfile->InfoBackgroundColor);
    OSDCopy(FBDialogWindow->MemOSDInfo, Info_W_Gd->width, 0, Info_C_Gd->width, Info_C_Gd->height, FBDialogWindow->NrInfoColumns - 1, X);
  }

  // cache info's southern border line

  InfoSCWidth = FBDialogWindow->NrInfoSColumns * InfoS_C_Gd->width;

  if (!FBDialogWindow->MemOSDInfoS || !isOSDRegionAlive(FBDialogWindow->MemOSDInfoS))
    FBDialogWindow->MemOSDInfoS = TAP_Osd_Create(0, 0, InfoS_W_Gd->width + InfoSCWidth + InfoS_E_Gd->width, InfoS_C_Gd->height, 0, OSD_Flag_MemRgn);

  // fill cache

  TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDInfoS, 0, 0, InfoS_W_Gd, TRUE, FBDialogProfile->InfoBackgroundColor);

  if (FBDialogWindow->NrInfoSColumns)
  {
    TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDInfoS, InfoS_W_Gd->width, 0, InfoS_C_Gd, TRUE, FBDialogProfile->InfoBackgroundColor);
    OSDCopy(FBDialogWindow->MemOSDInfoS, InfoS_W_Gd->width, 0, InfoS_C_Gd->width, InfoS_C_Gd->height, FBDialogWindow->NrInfoSColumns - 1, X);
  }

  TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDInfoS, InfoS_W_Gd->width + InfoSCWidth, 0, InfoS_E_Gd, TRUE, FBDialogProfile->InfoBackgroundColor);

  FBDialogWindow->OSDInfoBackDirty = TRUE;
}
