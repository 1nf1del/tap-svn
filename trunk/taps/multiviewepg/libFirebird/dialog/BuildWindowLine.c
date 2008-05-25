#include                "FBLib_dialog.h"

void BuildWindowLine (void)
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
      bgcolor = FBDialogProfile->ItemsLineBackgroundColor;
      break;

    case DialogWindowType_NormalLinesNoIcons:
      This_Items_IconButton_Gd = Items_NoIcons_IconButton_Gd;
      This_Items_Line_C_Gd     = Items_NoIcons_Line_C_Gd;
      This_Items_Line_E_Gd     = Items_NoIcons_Line_E_Gd;
      bgcolor = FBDialogProfile->ItemsLineBackgroundColor;
      break;

    default:
      This_Items_IconButton_Gd = Items_IconButton_Gd;
      This_Items_Line_C_Gd     = Items_Line_C_Gd;
      This_Items_Line_E_Gd     = Items_Line_E_Gd;
      bgcolor = FBDialogProfile->ItemsIconBackgroundColor;
  }

  // cache item lines
  if (!FBDialogWindow->MemOSDLines || !isOSDRegionAlive(FBDialogWindow->MemOSDLines))
    FBDialogWindow->MemOSDLines = TAP_Osd_Create(0, 0, FBDialogWindow->ItemLineWidth, (FBDialogWindow->NrItemLines + 1) * This_Items_Line_C_Gd->height, 0, OSD_Flag_MemRgn);

  // fill cache

  TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDLines, 0, 0, This_Items_IconButton_Gd, TRUE, bgcolor);

  if (FBDialogWindow->NrItemColumns)
  {
    TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDLines, This_Items_IconButton_Gd->width, 0, This_Items_Line_C_Gd, TRUE, FBDialogProfile->ItemsLineBackgroundColor);
    OSDCopy(FBDialogWindow->MemOSDLines, This_Items_IconButton_Gd->width, 0, This_Items_Line_C_Gd->width, This_Items_Line_C_Gd->height, FBDialogWindow->NrItemColumns - 1, X);
  }

  TAP_Osd_PutFreeColorGd(FBDialogWindow->MemOSDLines, This_Items_IconButton_Gd->width + FBDialogWindow->NrItemColumns * This_Items_Line_C_Gd->width, 0, This_Items_Line_E_Gd, TRUE, FBDialogProfile->ItemsLineBackgroundColor);

  FBDialogWindow->OSDLineBackDirty = TRUE;
}
