#include                <string.h>
#include                "FBLib_dialog.h"

void DialogProgressBarTitleSet (char *Title)
{
  if (!FBDialogProgressBar) return;

  if (Title)
  {
    strncpy(FBDialogProgressBar->Title, Title, sizeof(FBDialogProgressBar->Title) - 1);
    FBDialogProgressBar->Title[sizeof(FBDialogProgressBar->Title) - 1] = '\0';
  }

  if (FBDialogProgressBar->isVisible) DrawProgressBarTitle();
}
