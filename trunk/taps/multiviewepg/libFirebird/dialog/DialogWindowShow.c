#include                "FBLib_dialog.h"

void DialogWindowShow (void)
{
  dword                 State, SubState;

  if (!FBDialogWindow || FBDialogWindow->isVisible || !FBDialogProfile) return;

  TAP_GetState (&State, &SubState);
  FBDialogWindow->isNormalMode = ((State == STATE_Normal) && (SubState == SUBSTATE_Normal));
  if (FBDialogWindow->isNormalMode) TAP_ExitNormal();

  // check if the OSD has been defined
  if (!FBDialogWindow->OSDRgn)
  {
    FBDialogWindow->OSDRgn = TAP_Osd_Create (FBDialogWindow->OSDX, FBDialogWindow->OSDY, FBDialogWindow->OSDWidth, FBDialogWindow->OSDHeight, 0, 0);
    if (!FBDialogWindow->Multiple) TAP_Osd_SetTransparency (FBDialogWindow->OSDRgn, 0);
    WindowDirty();
  }

  //Backup the OSD region
  if (FBDialogWindow->Multiple)
    FBDialogWindow->MultipleOSDSaveBox = TAP_Osd_SaveBox (FBDialogWindow->OSDRgn, 0, 0, FBDialogWindow->OSDWidth, FBDialogWindow->OSDHeight);

  FBDialogWindow->isVisible = TRUE;
  DialogWindowRefresh();

  // fade it in
  DialogWindowAlpha(255 - TAP_GetSystemVar(SYSVAR_OsdAlpha));
}
