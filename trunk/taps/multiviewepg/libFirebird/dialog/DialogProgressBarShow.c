#include                "FBLib_dialog.h"

#include                "graphic/ProgressBar_Border_Top.gd"
#include                "graphic/ProgressBar_Border_Bottom.gd"
#include                "graphic/ProgressBar_Title.gd"

TYPE_GrData             *ProgressBar_Border_Top_Gd;
TYPE_GrData             *ProgressBar_Border_Bottom_Gd;
TYPE_GrData             *ProgressBar_Title_Gd;

void DialogProgressBarShow (void)
{
  dword                 State, SubState;

  if (!FBDialogProgressBar || FBDialogProgressBar->isVisible || !FBDialogProfile) return;

  ProgressBar_Border_Top_Gd     = &_ProgressBar_Border_Top_Gd;
  ProgressBar_Border_Bottom_Gd  = &_ProgressBar_Border_Bottom_Gd;
  ProgressBar_Title_Gd          = &_ProgressBar_Title_Gd;

  TAP_GetState (&State, &SubState);
  FBDialogProgressBar->isNormalMode = ((State == STATE_Normal) && (SubState == SUBSTATE_Normal));
  if (FBDialogProgressBar->isNormalMode) TAP_ExitNormal();

  //Draw the main window
  if (!FBDialogProgressBar->MemOSDRgn)
  {
    FBDialogProgressBar->MemOSDRgn = TAP_Osd_Create (0, 0, ProgressBar_Border_Top_Gd->width, ProgressBar_Border_Top_Gd->height + ProgressBar_Border_Bottom_Gd->height, 0, OSD_Flag_MemRgn);

    TAP_Osd_FillBox (FBDialogProgressBar->MemOSDRgn, PROGRESSBARLEFT, PROGRESSBARTOP, PROGRESSBARWIDTH, PROGRESSBARHEIGHT, RGB(0, 0, 0));

    TAP_Osd_PutFreeColorGd (FBDialogProgressBar->MemOSDRgn, 0, 0                                , ProgressBar_Title_Gd        , TRUE, FBDialogProfile->TitleBackgroundColor);
    TAP_Osd_PutFreeColorGd (FBDialogProgressBar->MemOSDRgn, 0, 0                                , ProgressBar_Border_Top_Gd   , TRUE, FBDialogProfile->BorderColor);
    TAP_Osd_PutFreeColorGd (FBDialogProgressBar->MemOSDRgn, 0, ProgressBar_Border_Top_Gd->height, ProgressBar_Border_Bottom_Gd, TRUE, FBDialogProfile->BorderColor);
  }

  if (!FBDialogProgressBar->OSDRgn) FBDialogProgressBar->OSDRgn = TAP_Osd_Create (FBDialogProgressBar->OSDX, FBDialogProgressBar->OSDY, GetOSDRegionWidth(FBDialogProgressBar->MemOSDRgn), GetOSDRegionHeight(FBDialogProgressBar->MemOSDRgn), 0, 0);

  FBDialogProgressBar->OSDSaveBox = TAP_Osd_SaveBox (FBDialogProgressBar->OSDRgn, 0, 0, GetOSDRegionWidth(FBDialogProgressBar->OSDRgn), GetOSDRegionHeight(FBDialogProgressBar->OSDRgn));
  TAP_Osd_Copy (FBDialogProgressBar->MemOSDRgn, FBDialogProgressBar->OSDRgn, 0, 0, GetOSDRegionWidth(FBDialogProgressBar->MemOSDRgn), GetOSDRegionHeight(FBDialogProgressBar->MemOSDRgn), 0, 0, TRUE);

  FBDialogProgressBar->isVisible = TRUE;

  DrawProgressBarTitle();
}
