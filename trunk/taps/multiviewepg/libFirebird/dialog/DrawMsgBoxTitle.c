#include                <string.h>
#include                "FBLib_dialog.h"

void DrawMsgBoxTitle (void)
{
  char                  *LF;

  if (!FBDialogMsgBox || !FBDialogProfile) return;

  if (FBDialogMsgBox->MemOSDRgn && FBDialogMsgBox->OSDRgn)
  {
    //Redraw the title bar
    TAP_Osd_PutFreeColorGd (FBDialogMsgBox->MemOSDRgn, 0, 0,                         MsgBox_W_Title_Gd,    TRUE, FBDialogProfile->TitleBackgroundColor);
    TAP_Osd_PutFreeColorGd (FBDialogMsgBox->MemOSDRgn, 0, MsgBox_W_Title_Gd->height, MsgBox_W_SubTitle_Gd, TRUE, FBDialogProfile->TitleBackgroundColor);

    if (FBDialogMsgBox->VirtualButtonCount)
    {
      TAP_Osd_PutFreeColorGd (FBDialogMsgBox->MemOSDRgn, MsgBox_W_Title_Gd->width, 0, MsgBox_C_Title_Gd, TRUE, FBDialogProfile->TitleBackgroundColor);
      OSDCopy(FBDialogMsgBox->MemOSDRgn, MsgBox_W_Title_Gd->width, 0, MsgBox_C_Title_Gd->width, MsgBox_C_Title_Gd->height, FBDialogMsgBox->VirtualButtonCount - 1, X);
      TAP_Osd_PutFreeColorGd (FBDialogMsgBox->MemOSDRgn, MsgBox_W_Title_Gd->width, MsgBox_W_Title_Gd->height, MsgBox_C_SubTitle_Gd, TRUE, FBDialogProfile->TitleBackgroundColor);
      OSDCopy(FBDialogMsgBox->MemOSDRgn, MsgBox_W_Title_Gd->width, MsgBox_W_Title_Gd->height, MsgBox_C_SubTitle_Gd->width, MsgBox_C_SubTitle_Gd->height, FBDialogMsgBox->VirtualButtonCount - 1, X);
    }

    TAP_Osd_PutFreeColorGd (FBDialogMsgBox->MemOSDRgn, MsgBox_W_Title_Gd->width + FBDialogMsgBox->VirtualButtonCount * MsgBox_C_Title_Gd->width, 0,                         MsgBox_E_Title_Gd,    TRUE, FBDialogProfile->TitleBackgroundColor);
    TAP_Osd_PutFreeColorGd (FBDialogMsgBox->MemOSDRgn, MsgBox_W_Title_Gd->width + FBDialogMsgBox->VirtualButtonCount * MsgBox_C_Title_Gd->width, MsgBox_W_Title_Gd->height, MsgBox_E_SubTitle_Gd, TRUE, FBDialogProfile->TitleBackgroundColor);
  }

  TAP_Osd_PutS (FBDialogMsgBox->MemOSDRgn, TITLE_OFFSET_XY, 19, FBDialogMsgBox->DialogWidth - TITLE_OFFSET_XY, FBDialogMsgBox->DialogTitle, FBDialogProfile->TitleTextColor, 0, 0, FNT_Size_1622, TRUE, ALIGN_CENTER);

  LF = strchr (FBDialogMsgBox->DialogSubTitle, '\n');

  if (LF)
  {
    *LF = '\0';
    TAP_Osd_PutS (FBDialogMsgBox->MemOSDRgn, 14, 49, FBDialogMsgBox->DialogWidth - 14, FBDialogMsgBox->DialogSubTitle, FBDialogProfile->TitleTextColor, 0, 0, FNT_Size_1419, TRUE, ALIGN_CENTER);
    TAP_Osd_PutS (FBDialogMsgBox->MemOSDRgn, 14, 66, FBDialogMsgBox->DialogWidth - 14, LF + 1,                         FBDialogProfile->TitleTextColor, 0, 0, FNT_Size_1419, TRUE, ALIGN_CENTER);
    *LF = '\n';
  }
  else TAP_Osd_PutS (FBDialogMsgBox->MemOSDRgn, 14, 57, FBDialogMsgBox->DialogWidth - 14, FBDialogMsgBox->DialogSubTitle, FBDialogProfile->TitleTextColor, 0, 0, FNT_Size_1419, TRUE, ALIGN_CENTER);

  TAP_Osd_Copy (FBDialogMsgBox->MemOSDRgn, FBDialogMsgBox->OSDRgn, 0, 0, FBDialogMsgBox->DialogWidth, MsgBox_C_Title_Gd->height + MsgBox_C_SubTitle_Gd->height, 0, 0, TRUE);
}
