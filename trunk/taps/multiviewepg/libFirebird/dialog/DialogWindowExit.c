#include                "FBLib_dialog.h"

void DialogWindowExit (void)
{
  if (!FBDialogWindow) return;

  if (FBDialogWindow->isVisible) DialogWindowHide();

  TAP_Osd_Delete (FBDialogWindow->MemOSDBorderN);
  TAP_Osd_Delete (FBDialogWindow->MemOSDBorderW);
  TAP_Osd_Delete (FBDialogWindow->MemOSDBorderE);
  TAP_Osd_Delete (FBDialogWindow->MemOSDBorderS);
  TAP_Osd_Delete (FBDialogWindow->MemOSDTitle);
  TAP_Osd_Delete (FBDialogWindow->MemOSDLines);
  TAP_Osd_Delete (FBDialogWindow->MemOSDLineSelected);
  TAP_Osd_Delete (FBDialogWindow->MemOSDScrollBar);
  TAP_Osd_Delete (FBDialogWindow->MemOSDInfoN);
  TAP_Osd_Delete (FBDialogWindow->MemOSDInfo);
  TAP_Osd_Delete (FBDialogWindow->MemOSDInfoS);

  if (FBDialogWindow->DialogItems)
  {
    TAP_MemFree (FBDialogWindow->DialogItems);
    FBDialogWindow->DialogItems = NULL;
  }

  DialogWindowInfoDeleteAll();

  FBDialogWindow  = NULL;
}
