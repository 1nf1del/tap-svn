#include                "FBLib_dialog.h"

void DialogProfileScrollBehaviourChange (bool ScrollScreen, bool WrapAround)
{
  if (FBDialogProfile)
  {
    FBDialogProfile->ScrollScreen     = ScrollScreen;
    FBDialogProfile->ScrollWrapAround = WrapAround;
  }
}
