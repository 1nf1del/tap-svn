#include                "FBLib_dialog.h"

void BuildWindowLineSelected (void)
{
  TYPE_GrData           *This_Items_IconButton_Gd, *This_Items_Line_C_Gd, *This_Items_Line_E_Gd;
  dword                 bgcolor;

  if (!FBDialogWindow || !FBDialogProfile) return;

  switch (FBDialogWindow->Type)
  {
    case DialogWindowType_NoLinesNoIcons:
    case DialogWindowType_NoLinesSmallIcons:
      This_Items_IconButton_Gd = Items_NoLinesNoIcons_IconButton_Gd;
      This_Items_Line_C_Gd     = Items_NoLinesNoIcons_Line_C_Gd;
      This_Items_Line_E_Gd     = Items_NoLinesNoIcons_Line_E_Gd;
      bgcolor = FBDialogProfile->ItemsLineBackgroundSelectedColor;
      break;

    case DialogWindowType_NormalLinesNoIcons:
      This_Items_IconButton_Gd = Items_NoIcons_IconButton_Gd;
      This_Items_Line_C_Gd     = Items_NoIcons_Line_C_Gd;
      This_Items_Line_E_Gd     = Items_NoIcons_Line_E_Gd;
      bgcolor = FBDialogProfile->ItemsLineBackgroundSelectedColor;
      break;

    default:
      This_Items_IconButton_Gd = Items_IconButton_Gd;
      This_Items_Line_C_Gd     = Items_Line_C_Gd;
      This_Items_Line_E_Gd     = Items_Line_E_Gd;
      bgcolor = FBDialogProfile->ItemsIconBackgroundSelectedColor;
  }

  // cache selected line
  if (!FBDialogWindow->MemOSDLineSelected || !isOSDRegionAlive(FBDialogWindow->MemOSDLineSelected))
    FBDialogWindow->MemOSDLineSelected = TAP_Osd_Create(0, 0, FBDialogWindow->ItemLineWidth, This_Items_Line_C_Gd->height, 0, OSD_Flag_MemRgn);

  // fill cache

  TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDLineSelected, 0, 0, This_Items_IconButton_Gd, TRUE, bgcolor);

  if (FBDialogWindow->NrItemColumns)
  {
    TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDLineSelected, This_Items_IconButton_Gd->width, 0, This_Items_Line_C_Gd, TRUE, FBDialogProfile->ItemsLineBackgroundSelectedColor);
    OSDCopy(FBDialogWindow->MemOSDLineSelected, This_Items_IconButton_Gd->width, 0, This_Items_Line_C_Gd->width, This_Items_Line_C_Gd->height, FBDialogWindow->NrItemColumns - 1, X);
  }

  TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDLineSelected, This_Items_IconButton_Gd->width + FBDialogWindow->NrItemColumns * This_Items_Line_C_Gd->width, 0, This_Items_Line_E_Gd, TRUE, FBDialogProfile->ItemsLineBackgroundSelectedColor);

  FBDialogWindow->OSDLineSelectedBackDirty = TRUE;
}
