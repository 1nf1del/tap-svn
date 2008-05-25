#include                "FBLib_dialog.h"

void DialogWindowRefresh (void)
{
  int                   i;

  if (!FBDialogWindow || !FBDialogWindow->isVisible || !FBDialogProfile) return;

  // check whether parts of the profile have changed

  if (FBDialogProfile->BorderColor != FBDialogProfile->OldBorderColor)
  {
    FBDialogWindow->OSDBorderBackDirty = TRUE;
    FBDialogProfile->OldBorderColor = FBDialogProfile->BorderColor;
  }

  if (FBDialogProfile->TitleTextColor != FBDialogProfile->OldTitleTextColor)
  {
    FBDialogWindow->OSDTitleForeDirty = TRUE;
    FBDialogProfile->OldTitleTextColor = FBDialogProfile->TitleTextColor;
  }

  if (FBDialogProfile->TitleBackgroundColor != FBDialogProfile->OldTitleBackgroundColor)
  {
    FBDialogWindow->OSDTitleBackDirty = TRUE;
    FBDialogProfile->OldTitleBackgroundColor = FBDialogProfile->TitleBackgroundColor;
  }

  if (FBDialogProfile->ItemsTextColor         != FBDialogProfile->OldItemsTextColor ||
      FBDialogProfile->ItemsTextDisabledColor != FBDialogProfile->OldItemsTextDisabledColor ||
      FBDialogProfile->ItemsTextSelectedColor != FBDialogProfile->OldItemsTextSelectedColor ||
      FBDialogProfile->ItemsGridColor         != FBDialogProfile->OldItemsGridColor)
  {
    for (i = 0; i < FBDialogWindow->NrItemLines; i++) FBDialogWindow->OSDLineForeDirty[i] = TRUE;

    if ((FBDialogProfile->ItemsTextColor != FBDialogProfile->OldItemsTextColor) && !FBDialogWindow->InfoOwnForeColor) FBDialogWindow->OSDInfoForeDirty = TRUE;

    FBDialogProfile->OldItemsTextColor         = FBDialogProfile->ItemsTextColor;
    FBDialogProfile->OldItemsTextDisabledColor = FBDialogProfile->ItemsTextDisabledColor;
    FBDialogProfile->OldItemsTextSelectedColor = FBDialogProfile->ItemsTextSelectedColor;
    FBDialogProfile->OldItemsGridColor         = FBDialogProfile->ItemsGridColor;
  }

  if (FBDialogProfile->ItemsLineBackgroundColor != FBDialogProfile->OldItemsLineBackgroundColor ||
      FBDialogProfile->ItemsIconBackgroundColor != FBDialogProfile->OldItemsIconBackgroundColor)
  {
    FBDialogWindow->OSDLineBackDirty = TRUE;
    FBDialogProfile->OldItemsLineBackgroundColor = FBDialogProfile->ItemsLineBackgroundColor;
    FBDialogProfile->OldItemsIconBackgroundColor = FBDialogProfile->ItemsIconBackgroundColor;
  }

  if (FBDialogProfile->ItemsLineBackgroundSelectedColor != FBDialogProfile->OldItemsLineBackgroundSelectedColor ||
      FBDialogProfile->ItemsIconBackgroundSelectedColor != FBDialogProfile->OldItemsIconBackgroundSelectedColor)
  {
    FBDialogWindow->OSDLineSelectedBackDirty = TRUE;
    FBDialogProfile->OldItemsLineBackgroundSelectedColor = FBDialogProfile->ItemsLineBackgroundSelectedColor;
    FBDialogProfile->OldItemsIconBackgroundSelectedColor = FBDialogProfile->ItemsIconBackgroundSelectedColor;
  }

  if (FBDialogProfile->ScrollBarColor != FBDialogProfile->OldScrollBarColor)
  {
    FBDialogWindow->OSDScrollBarForeDirty = TRUE;   // same color as background
    FBDialogWindow->OSDScrollBarBackDirty = TRUE;
    FBDialogProfile->OldScrollBarColor = FBDialogProfile->ScrollBarColor;
  }

  if (FBDialogProfile->InfoBackgroundColor != FBDialogProfile->OldInfoBackgroundColor)
  {
    FBDialogWindow->OSDInfoBackDirty = TRUE;
    FBDialogProfile->OldInfoBackgroundColor = FBDialogProfile->InfoBackgroundColor;
  }

  if (FBDialogProfile->FontSize != FBDialogProfile->OldFontSize)
  {
    for (i = 0; i < FBDialogWindow->NrItemLines; i++) FBDialogWindow->OSDLineForeDirty[i] = TRUE;

    FBDialogWindow->OSDTitleForeDirty = TRUE;
    FBDialogProfile->OldFontSize = FBDialogProfile->FontSize;
  }

  // check whether the memory OSDs still exist

  if (!FBDialogWindow->MemOSDBorderN      || !isOSDRegionAlive(FBDialogWindow->MemOSDBorderN) ||
      !FBDialogWindow->MemOSDBorderW      || !isOSDRegionAlive(FBDialogWindow->MemOSDBorderW) ||
      !FBDialogWindow->MemOSDBorderE      || !isOSDRegionAlive(FBDialogWindow->MemOSDBorderE) ||
      !FBDialogWindow->MemOSDBorderS      || !isOSDRegionAlive(FBDialogWindow->MemOSDBorderS))      FBDialogWindow->OSDBorderBackDirty       = TRUE;

  if (!FBDialogWindow->MemOSDTitle        || !isOSDRegionAlive(FBDialogWindow->MemOSDTitle))        FBDialogWindow->OSDTitleBackDirty        = TRUE;

  if (!FBDialogWindow->MemOSDLines        || !isOSDRegionAlive(FBDialogWindow->MemOSDLines))        FBDialogWindow->OSDLineBackDirty         = TRUE;

  if (!FBDialogWindow->MemOSDLineSelected || !isOSDRegionAlive(FBDialogWindow->MemOSDLineSelected)) FBDialogWindow->OSDLineSelectedBackDirty = TRUE;

  if (!FBDialogWindow->MemOSDScrollBar    || !isOSDRegionAlive(FBDialogWindow->MemOSDScrollBar))    FBDialogWindow->OSDScrollBarBackDirty    = TRUE;

  if (!FBDialogWindow->MemOSDInfoN        || !isOSDRegionAlive(FBDialogWindow->MemOSDInfoN) ||
      !FBDialogWindow->MemOSDInfo         || !isOSDRegionAlive(FBDialogWindow->MemOSDInfo)  ||
      !FBDialogWindow->MemOSDInfoS        || !isOSDRegionAlive(FBDialogWindow->MemOSDInfoS))        FBDialogWindow->OSDInfoBackDirty         = TRUE;

  // draw the window parts

  DrawWindowBorder();
  DrawWindowTitle();
  DrawWindowLines();
  DrawWindowInfo();
}
