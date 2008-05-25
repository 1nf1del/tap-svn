#include                <string.h>
#include                "FBLib_dialog.h"

void DrawWindowLine (int ItemIndex)
{
  int                   LineIndex, y;
  dword                 IconButtonWidth, color, maxx, x, tabc;
  char                  *p, *t;

  if (!FBDialogWindow || !FBDialogWindow->NrItemLines || !FBDialogWindow->DialogItems || !FBDialogProfile) return;

  if (ItemIndex < FBDialogWindow->TopItem || ItemIndex > FBDialogWindow->TopItem + FBDialogWindow->NrItemLines - 1 || ItemIndex >= FBDialogWindow->NrItems) return;

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

  LineIndex = ItemIndex - FBDialogWindow->TopItem;
  y = (LineIndex + 1) * Items_Line_C_Gd->height;

  // selected line
  if ((ItemIndex == FBDialogWindow->SelectedItem) && FBDialogWindow->hasCursor)
    TAP_Osd_Copy(FBDialogWindow->MemOSDLineSelected, FBDialogWindow->MemOSDLines, 0, 0, GetOSDRegionWidth(FBDialogWindow->MemOSDLineSelected), GetOSDRegionHeight(FBDialogWindow->MemOSDLineSelected), 0, y, TRUE);

  // optional icon
  if (IconButtonWidth > 0)
  {
    switch (FBDialogWindow->DialogItems[ItemIndex].Icon)
    {
      case DialogItemIcon_Checkbox_Checked:
        TAP_Osd_PutGd(FBDialogWindow->MemOSDLines, 0, y, Icon_Checkbox_Checked_Gd, TRUE);
        break;

      case DialogItemIcon_Checkbox_Unchecked:
        TAP_Osd_PutGd(FBDialogWindow->MemOSDLines, 0, y, Icon_Checkbox_Unchecked_Gd, TRUE);
        break;

      case DialogItemIcon_Exclamation:
        TAP_Osd_PutGd(FBDialogWindow->MemOSDLines, 0, y, Icon_Exclamation_Gd, TRUE);
        break;

      case DialogItemIcon_Wait:
        TAP_Osd_PutGd(FBDialogWindow->MemOSDLines, 0, y, Icon_Wait_Gd, TRUE);
        break;

      case DialogItemIcon_OK:
        TAP_Osd_PutGd(FBDialogWindow->MemOSDLines, 0, y, Icon_OK_Gd, TRUE);
        break;

      case DialogItemIcon_User:
        if (FBDialogWindow->DialogItems[ItemIndex].UserIcon) TAP_Osd_PutGd(FBDialogWindow->MemOSDLines, 0, y, FBDialogWindow->DialogItems[ItemIndex].UserIcon, TRUE);
        break;

      default:
        ;
    }
  }

  // line content
  if (*FBDialogWindow->DialogItems[ItemIndex].Parameter)
  {
    color = FBDialogWindow->DialogItems[ItemIndex].ParameterColor;

    if (!color)
    {
      if (!FBDialogWindow->DialogItems[ItemIndex].Enabled) color = FBDialogProfile->ItemsTextDisabledColor;
      else if ((ItemIndex == FBDialogWindow->SelectedItem) && FBDialogWindow->hasCursor) color = FBDialogProfile->ItemsTextSelectedColor;
      else color = FBDialogProfile->ItemsTextColor;
    }

    maxx = IconButtonWidth + ITEM_OFFSET_X + FBDialogWindow->ParameterWidth;

    if (maxx >= GetOSDRegionWidth(FBDialogWindow->MemOSDLines)) maxx = GetOSDRegionWidth(FBDialogWindow->MemOSDLines) - Items_Line_E_Gd->width - ITEM_OFFSET_X;   // remember: the scrollbar lies over Items_Line_E_Gd

    x = IconButtonWidth + ITEM_OFFSET_X;
    p = FBDialogWindow->DialogItems[ItemIndex].Parameter;
    tabc = 0;

    while ((t = strchr(p, '\t')))
    {
      *t = '\0';
      TAP_Osd_PutS(FBDialogWindow->MemOSDLines, x, y + (FBDialogProfile->FontSize ? 0 : 1), maxx, p, color, 0, 0, FBDialogProfile->FontSize, TRUE, ALIGN_LEFT);
      *t = '\t';
      p = t + 1;
      x = IconButtonWidth + ITEM_OFFSET_X + (tabc < DIALOGWINDOW_MAX_TABS ? FBDialogWindow->TabsParameter[tabc++] : 0);
    }

    if (*p) TAP_Osd_PutS(FBDialogWindow->MemOSDLines, x, y + (FBDialogProfile->FontSize ? 0 : 1), maxx, p, color, 0, 0, FBDialogProfile->FontSize, TRUE, ALIGN_LEFT);

    if (*FBDialogWindow->DialogItems[ItemIndex].Value)
    {
      color = FBDialogWindow->DialogItems[ItemIndex].ValueColor;

      if (!color)
      {
        if (!FBDialogWindow->DialogItems[ItemIndex].Enabled) color = FBDialogProfile->ItemsTextDisabledColor;
        else if ((ItemIndex == FBDialogWindow->SelectedItem) && FBDialogWindow->hasCursor) color = FBDialogProfile->ItemsTextSelectedColor;
        else color = FBDialogProfile->ItemsTextColor;
      }

      x = IconButtonWidth + ITEM_OFFSET_X + FBDialogWindow->ParameterWidth + (ITEM_OFFSET_X << 1);
      p = FBDialogWindow->DialogItems[ItemIndex].Value;
      tabc = 0;

      while ((t = strchr(p, '\t')))
      {
        *t = '\0';
        if (x + Items_Line_E_Gd->width + ITEM_OFFSET_X < GetOSDRegionWidth(FBDialogWindow->MemOSDLines)) TAP_Osd_PutS(FBDialogWindow->MemOSDLines, x, y + (FBDialogProfile->FontSize ? 0 : 1), GetOSDRegionWidth(FBDialogWindow->MemOSDLines) - Items_Line_E_Gd->width - ITEM_OFFSET_X, p, color, 0, 0, FBDialogProfile->FontSize, TRUE, ALIGN_LEFT);   // remember: the scrollbar lies over Items_Line_E_Gd
        *t = '\t';
        p = t + 1;
        x = IconButtonWidth + ITEM_OFFSET_X + FBDialogWindow->ParameterWidth + (ITEM_OFFSET_X << 1) + (tabc < DIALOGWINDOW_MAX_TABS ? FBDialogWindow->TabsValue[tabc++] : 0);
      }

      if (*p && (x + Items_Line_E_Gd->width + ITEM_OFFSET_X < GetOSDRegionWidth(FBDialogWindow->MemOSDLines))) TAP_Osd_PutS(FBDialogWindow->MemOSDLines, x, y + (FBDialogProfile->FontSize ? 0 : 1), GetOSDRegionWidth(FBDialogWindow->MemOSDLines) - Items_Line_E_Gd->width - ITEM_OFFSET_X, p, color, 0, 0, FBDialogProfile->FontSize, TRUE, ALIGN_LEFT);   // remember: the scrollbar lies over Items_Line_E_Gd
    }
  }
  else
  {
    y += (Items_Line_C_Gd->height >> 1);
    DrawOSDLine(FBDialogWindow->MemOSDLines, IconButtonWidth + ITEM_OFFSET_X, y, Items_IconButton_Gd->width + FBDialogWindow->NrItemColumns * Items_Line_C_Gd->width - (FBDialogWindow->NrItems <= FBDialogWindow->NrItemLines ? 0 : Scroll_Back_Gd->width), y, FBDialogProfile->ItemsGridColor);
  }
}
