#include                <string.h>
#include                "FBLib_dialog.h"

void DialogWindowTitleChange (char *LTitle, char *CTitle, char *RTitle)
{
  if (!FBDialogWindow) return;

  if (LTitle)
  {
    strncpy (FBDialogWindow->LTitle, LTitle, sizeof(FBDialogWindow->LTitle) - 1);
    FBDialogWindow->LTitle[sizeof(FBDialogWindow->LTitle) - 1] = '\0';
  }

  if (CTitle)
  {
    strncpy (FBDialogWindow->CTitle, CTitle, sizeof(FBDialogWindow->CTitle) - 1);
    FBDialogWindow->CTitle[sizeof(FBDialogWindow->CTitle) - 1] = '\0';
  }

  if (RTitle)
  {
    strncpy (FBDialogWindow->RTitle, RTitle, sizeof(FBDialogWindow->RTitle) - 1);
    FBDialogWindow->RTitle[sizeof(FBDialogWindow->RTitle) - 1] = '\0';
  }

  FBDialogWindow->OSDTitleForeDirty = TRUE;
}
