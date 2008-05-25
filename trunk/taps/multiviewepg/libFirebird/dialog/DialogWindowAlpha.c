#include                "FBLib_dialog.h"

void DialogWindowAlpha (byte Alpha)
{
  dword                 i, j;

  if (!FBDialogWindow || !FBDialogWindow->isVisible || !FBDialogWindow->OSDRgn || !FBDialogProfile) return;

  if (FBDialogProfile->Fade && !FBDialogWindow->Multiple)
  {
    if (Alpha > FBDialogWindow->Transparency)
    {
      for (i = FBDialogWindow->Transparency; i < Alpha; i++)
      {
        TAP_Osd_SetTransparency(FBDialogWindow->OSDRgn, i);
        for (j = 0; j < FBDialogProfile->Fade; j++) TAP_Delay(0);
      }
    }

    if (Alpha < FBDialogWindow->Transparency)
    {
      for (i = FBDialogWindow->Transparency; i > Alpha; i--)
      {
        TAP_Osd_SetTransparency(FBDialogWindow->OSDRgn, i);
        for (j = 0; j < FBDialogProfile->Fade; j++) TAP_Delay(0);
      }
    }
  }

  FBDialogWindow->Transparency = Alpha;
  TAP_Osd_SetTransparency(FBDialogWindow->OSDRgn, FBDialogWindow->Transparency);
}
