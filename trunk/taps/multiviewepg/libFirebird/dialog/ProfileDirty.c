#include                "FBLib_dialog.h"

void ProfileDirty (void)
{
  if (!FBDialogProfile) return;

  // force redraw due to profile changes
  FBDialogProfile->OldBorderColor                      = ~FBDialogProfile->BorderColor;
  FBDialogProfile->OldTitleBackgroundColor             = ~FBDialogProfile->TitleBackgroundColor;
  FBDialogProfile->OldTitleTextColor                   = ~FBDialogProfile->TitleTextColor;
  FBDialogProfile->OldItemsLineBackgroundColor         = ~FBDialogProfile->ItemsLineBackgroundColor;
  FBDialogProfile->OldItemsLineBackgroundSelectedColor = ~FBDialogProfile->ItemsLineBackgroundSelectedColor;
  FBDialogProfile->OldItemsIconBackgroundColor         = ~FBDialogProfile->ItemsIconBackgroundColor;
  FBDialogProfile->OldItemsIconBackgroundSelectedColor = ~FBDialogProfile->ItemsIconBackgroundSelectedColor;
  FBDialogProfile->OldItemsTextColor                   = ~FBDialogProfile->ItemsTextColor;
  FBDialogProfile->OldItemsTextDisabledColor           = ~FBDialogProfile->ItemsTextDisabledColor;
  FBDialogProfile->OldItemsTextSelectedColor           = ~FBDialogProfile->ItemsTextSelectedColor;
  FBDialogProfile->OldItemsGridColor                   = ~FBDialogProfile->ItemsGridColor;
  FBDialogProfile->OldInfoBackgroundColor              = ~FBDialogProfile->InfoBackgroundColor;
  FBDialogProfile->OldScrollBarColor                   = ~FBDialogProfile->ScrollBarColor;
  FBDialogProfile->OldFontSize                         = ~FBDialogProfile->FontSize;
}
