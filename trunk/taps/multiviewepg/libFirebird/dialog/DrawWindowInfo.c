#include                "FBLib_dialog.h"

void DrawWindowInfo (void)
{
  tDialogInfoItem       *item;

  if (!FBDialogWindow || !FBDialogProfile) return;

  if (FBDialogWindow->OSDInfoForeDirty || FBDialogWindow->OSDInfoBackDirty)
  {
    if (FBDialogWindow->OSDInfoBackDirty) BuildWindowInfo();

    if (FBDialogWindow->OSDRgn)
    {
      if (FBDialogWindow->NrInfoLines)
      {
        OSDCopy(FBDialogWindow->MemOSDInfo, 0, 0, GetOSDRegionWidth(FBDialogWindow->MemOSDInfo), Info_C_Gd->height, FBDialogWindow->NrInfoLines, Y);

        item = FBDialogWindow->InfoItems;

        while (item)
        {
          switch (item->ItemType)
          {
            case InfoString:
              TAP_Osd_PutS(FBDialogWindow->MemOSDInfo, Info_W_Gd->width + item->X, item->Y + Info_C_Gd->height, Info_W_Gd->width + item->MaxX, item->data, (item->FColor ? item->FColor : FBDialogProfile->ItemsTextColor), item->BColor, 0, item->FontSize, item->bDot, item->Align);
              break;

            case InfoIcon:
              TAP_Osd_PutGd(FBDialogWindow->MemOSDInfo, Info_W_Gd->width + item->X, item->Y + Info_C_Gd->height, item->data, TRUE);
              break;
          }

          item = item->next;
        }
      }

      // draw

      if (FBDialogWindow->OSDInfoBackDirty)
      {
        TAP_Osd_Copy(FBDialogWindow->MemOSDInfoN, FBDialogWindow->OSDRgn, 0, 0, GetOSDRegionWidth(FBDialogWindow->MemOSDInfoN), GetOSDRegionHeight(FBDialogWindow->MemOSDInfoN), 0, FBDialogWindow->InfoNY, TRUE);
        TAP_Osd_Copy(FBDialogWindow->MemOSDInfoS, FBDialogWindow->OSDRgn, 0, 0, GetOSDRegionWidth(FBDialogWindow->MemOSDInfoS), GetOSDRegionHeight(FBDialogWindow->MemOSDInfoS), 0, FBDialogWindow->InfoSY, TRUE);
      }

      if (FBDialogWindow->NrInfoLines) TAP_Osd_Copy(FBDialogWindow->MemOSDInfo, FBDialogWindow->OSDRgn, 0, Info_C_Gd->height, GetOSDRegionWidth(FBDialogWindow->MemOSDInfo), GetOSDRegionHeight(FBDialogWindow->MemOSDInfo) - Info_C_Gd->height, 0, FBDialogWindow->InfoY, TRUE);

      FBDialogWindow->OSDInfoForeDirty = FALSE;
      FBDialogWindow->OSDInfoBackDirty = FALSE;
    }
  }
}
