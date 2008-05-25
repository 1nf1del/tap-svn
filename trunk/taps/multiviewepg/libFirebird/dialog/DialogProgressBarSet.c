#include                "FBLib_dialog.h"

void DialogProgressBarSet  (int Value, word BarColor)
{
  if (!FBDialogProgressBar || !FBDialogProgressBar->isVisible) return;

  DrawProgressBarBar(Value, BarColor);
}
