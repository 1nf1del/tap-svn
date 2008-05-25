#include                "FBLib_dialog.h"

void DrawWindowLines (void)
{
  bool                  allLinesDirty;
  int                   i;
  dword                 IconButtonWidth, x;

  if (!FBDialogWindow || !FBDialogProfile) return;

  if (OSDLinesForeDirty(FALSE) || FBDialogWindow->OSDLineBackDirty || FBDialogWindow->OSDLineSelectedBackDirty)
  {
    switch (FBDialogWindow->Type)
    {
      case DialogWindowType_NoLinesNoIcons:
      case DialogWindowType_NormalLinesNoIcons:
        IconButtonWidth = 0;
        break;

      case DialogWindowType_NoLinesSmallIcons:
        IconButtonWidth = Items_NoLinesNoIcons_IconButton_Gd->width >> 1;
        break;

      default:
        IconButtonWidth = Items_IconButton_Gd->width;
    }

    if (FBDialogWindow->OSDLineBackDirty)
    {
      BuildWindowLine();
      for (i = 0; i < FBDialogWindow->NrItemLines; i++) FBDialogWindow->OSDLineForeDirty[i] = TRUE;
    }

    if (FBDialogWindow->OSDLineSelectedBackDirty) BuildWindowLineSelected();

    if (FBDialogWindow->OSDRgn)
    {
      if (FBDialogWindow->NrItemLines)
      {
        allLinesDirty = OSDLinesForeDirty(TRUE);

        if (allLinesDirty) OSDCopy(FBDialogWindow->MemOSDLines, 0, 0, GetOSDRegionWidth(FBDialogWindow->MemOSDLines), Items_Line_C_Gd->height, FBDialogWindow->NrItemLines, Y);

        for (i = 0; i < FBDialogWindow->NrItemLines; i++)
        {
          if (FBDialogWindow->OSDLineForeDirty[i])
          {
            if (!allLinesDirty) TAP_Osd_Copy(FBDialogWindow->MemOSDLines, FBDialogWindow->MemOSDLines, 0, 0, GetOSDRegionWidth(FBDialogWindow->MemOSDLines), Items_Line_C_Gd->height, 0, (i + 1) * Items_Line_C_Gd->height, TRUE);
            DrawWindowLine(i + FBDialogWindow->TopItem);
          }
        }

        // the value separator line
        x = IconButtonWidth + ITEM_OFFSET_X + FBDialogWindow->ParameterWidth + ITEM_OFFSET_X;
        if (x < GetOSDRegionWidth(FBDialogWindow->MemOSDLines))
          DrawOSDLine(FBDialogWindow->MemOSDLines, x, Items_Line_C_Gd->height, x, GetOSDRegionHeight(FBDialogWindow->MemOSDLines), FBDialogProfile->ItemsGridColor);

        // scrollbar
        FBDialogWindow->OSDScrollBarForeDirty = TRUE;
        DrawWindowScrollBar();

        //draw
        if (allLinesDirty) TAP_Osd_Copy(FBDialogWindow->MemOSDLines, FBDialogWindow->OSDRgn, 0, Items_Line_C_Gd->height, GetOSDRegionWidth(FBDialogWindow->MemOSDLines), GetOSDRegionHeight(FBDialogWindow->MemOSDLines) - Items_Line_C_Gd->height, Border_Items_W_Gd->width, FBDialogWindow->ItemsY, TRUE);
        else
        {
          for (i = 0; i < FBDialogWindow->NrItemLines; i++)
            if (FBDialogWindow->OSDLineForeDirty[i]) TAP_Osd_Copy(FBDialogWindow->MemOSDLines, FBDialogWindow->OSDRgn, 0, (i + 1) * Items_Line_C_Gd->height, GetOSDRegionWidth(FBDialogWindow->MemOSDLines), Items_Line_C_Gd->height, Border_Items_W_Gd->width, FBDialogWindow->ItemsY + i * Items_Line_C_Gd->height, TRUE);

          // scrollbar
          x = FBDialogWindow->OSDWidth - Border_Items_W_Gd->width - Border_Items_E_Gd->width - Scroll_Back_Gd->width + 9;   // Border_Items_E_Gd has a transparent left part
          TAP_Osd_Copy(FBDialogWindow->MemOSDLines, FBDialogWindow->OSDRgn, x, Items_Line_C_Gd->height, GetOSDRegionWidth(FBDialogWindow->MemOSDScrollBar) >> 1, GetOSDRegionHeight(FBDialogWindow->MemOSDScrollBar), x + Border_Items_W_Gd->width, FBDialogWindow->ItemsY, TRUE);
        }
      }

      for (i = 0; i < FBDialogWindow->NrItemLines; i++) FBDialogWindow->OSDLineForeDirty[i] = FALSE;
      FBDialogWindow->OSDLineBackDirty         = FALSE;
      FBDialogWindow->OSDLineSelectedBackDirty = FALSE;
    }
  }
}
