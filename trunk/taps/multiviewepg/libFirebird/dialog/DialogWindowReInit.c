#include                <type.h>
#include                "FBLib_dialog.h"

#include                "graphic/Border_N.gd"
#include                "graphic/Border_NE.gd"
#include                "graphic/Border_NW.gd"
#include                "graphic/Border_Items_E.gd"
#include                "graphic/Border_Items_W.gd"
#include                "graphic/Border_InfoN_E.gd"
#include                "graphic/Border_InfoN_W.gd"
#include                "graphic/Border_Info_E.gd"
#include                "graphic/Border_Info_W.gd"
#include                "graphic/Border_S.gd"
#include                "graphic/Border_SE.gd"
#include                "graphic/Border_SW.gd"
#include                "graphic/Title_C.gd"
#include                "graphic/Title_E.gd"
#include                "graphic/Title_W.gd"
#include                "graphic/Items_IconButton.gd"
#include                "graphic/Items_Line_C.gd"
#include                "graphic/Items_Line_E.gd"
#include                "graphic/InfoN_C.gd"
#include                "graphic/InfoN_E.gd"
#include                "graphic/InfoN_W.gd"
#include                "graphic/Info_C.gd"
#include                "graphic/Info_W.gd"

TYPE_GrData             *Border_N_Gd;
TYPE_GrData             *Border_NE_Gd;
TYPE_GrData             *Border_NW_Gd;
TYPE_GrData             *Border_Items_E_Gd;
TYPE_GrData             *Border_Items_W_Gd;
TYPE_GrData             *Border_InfoN_E_Gd;
TYPE_GrData             *Border_InfoN_W_Gd;
TYPE_GrData             *Border_Info_E_Gd;
TYPE_GrData             *Border_Info_W_Gd;
TYPE_GrData             *Border_S_Gd;
TYPE_GrData             *Border_SE_Gd;
TYPE_GrData             *Border_SW_Gd;
TYPE_GrData             *Title_C_Gd;
TYPE_GrData             *Title_E_Gd;
TYPE_GrData             *Title_W_Gd;
TYPE_GrData             *Items_IconButton_Gd;
TYPE_GrData             *Items_Line_C_Gd;
TYPE_GrData             *Items_Line_E_Gd;
TYPE_GrData             *InfoN_C_Gd;
TYPE_GrData             *InfoN_E_Gd;
TYPE_GrData             *InfoN_W_Gd;
TYPE_GrData             *Info_C_Gd;
TYPE_GrData             *Info_W_Gd;

void DialogWindowReInit (dword X, dword Y, dword Width, dword NrItemLines, dword ParameterWidth, dword InfoHeight)
{
  if (!FBDialogWindow || FBDialogWindow->OSDRgn || FBDialogWindow->MemOSDBorderN || FBDialogWindow->MemOSDBorderW || FBDialogWindow->MemOSDBorderE || FBDialogWindow->MemOSDBorderS || FBDialogWindow->MemOSDTitle || FBDialogWindow->MemOSDLines || FBDialogWindow->MemOSDLineSelected || FBDialogWindow->MemOSDScrollBar || FBDialogWindow->MemOSDInfoN || FBDialogWindow->MemOSDInfo || FBDialogWindow->MemOSDInfoS) return;

  Border_N_Gd                 = &_Border_N_Gd;
  Border_NE_Gd                = &_Border_NE_Gd;
  Border_NW_Gd                = &_Border_NW_Gd;
  Border_Items_E_Gd           = &_Border_Items_E_Gd;
  Border_Items_W_Gd           = &_Border_Items_W_Gd;
  Border_InfoN_E_Gd           = &_Border_InfoN_E_Gd;
  Border_InfoN_W_Gd           = &_Border_InfoN_W_Gd;
  Border_Info_E_Gd            = &_Border_Info_E_Gd;
  Border_Info_W_Gd            = &_Border_Info_W_Gd;
  Border_S_Gd                 = &_Border_S_Gd;
  Border_SE_Gd                = &_Border_SE_Gd;
  Border_SW_Gd                = &_Border_SW_Gd;
  Title_C_Gd                  = &_Title_C_Gd;
  Title_E_Gd                  = &_Title_E_Gd;
  Title_W_Gd                  = &_Title_W_Gd;
  Items_IconButton_Gd         = &_Items_IconButton_Gd;
  Items_Line_C_Gd             = &_Items_Line_C_Gd;
  Items_Line_E_Gd             = &_Items_Line_E_Gd;
  InfoN_C_Gd                  = &_InfoN_C_Gd;
  InfoN_E_Gd                  = &_InfoN_E_Gd;
  InfoN_W_Gd                  = &_InfoN_W_Gd;
  Info_C_Gd                   = &_Info_C_Gd;
  Info_W_Gd                   = &_Info_W_Gd;

  FBDialogWindow->OSDX = X;
  FBDialogWindow->OSDY = Y;
  FBDialogWindow->ParameterWidth = ParameterWidth;

  // round the requested width and info height to the next 10 pixels

  if (Width % Items_Line_C_Gd->width) Width++;
  FBDialogWindow->OSDWidth = Width / Items_Line_C_Gd->width * Items_Line_C_Gd->width;

  FBDialogWindow->NrInfoLines = InfoHeight / Info_C_Gd->height;
  if ((InfoHeight % Info_C_Gd->height) > 4) FBDialogWindow->NrInfoLines++;

  // calculate the graphical elements

  FBDialogWindow->NrTitleColumns = (FBDialogWindow->OSDWidth - (Border_NW_Gd->width + Border_NE_Gd->width)) / Border_N_Gd->width;
  FBDialogWindow->NrItemLines    = min(NrItemLines, DIALOGWINDOW_MAX_NR_ITEM_LINES);
  FBDialogWindow->NrItemColumns  = (FBDialogWindow->OSDWidth - (Border_Items_W_Gd->width + Items_IconButton_Gd->width + Border_Items_E_Gd->width)) / Items_Line_C_Gd->width;   // Items_Line_E_Gd->width can be ignored, because it has transparent right part of (non-transparent) Border_Items_E_Gd->width width (and Border_Items_E_Gd has a left transparent part of (non-transparent) Items_Line_E_Gd->width width)
  FBDialogWindow->NrInfoNColumns = (FBDialogWindow->OSDWidth - (InfoN_W_Gd->width + Border_InfoN_E_Gd->width)) / InfoN_C_Gd->width;   // Border_InfoN_W_Gd->width can be ignored, because InfoN_W_Gd->width has a transparent left part of Border_InfoN_W_Gd->width width
  FBDialogWindow->NrInfoColumns  = (FBDialogWindow->OSDWidth - (Info_W_Gd->width + Border_Info_E_Gd->width)) / Info_C_Gd->width;   // Border_Info_W_Gd->width can be ignored, because Info_W_Gd->width has a transparent left part of Border_Info_W_Gd->width width
  FBDialogWindow->NrInfoSColumns = (FBDialogWindow->OSDWidth - (Border_SW_Gd->width + Border_SE_Gd->width)) / Border_S_Gd->width;

  FBDialogWindow->TitleY = 0;   // Title_C_Gd has a transparent upper part of (the visible part of) Border_N_Gd width
  FBDialogWindow->ItemsY = FBDialogWindow->TitleY + Title_C_Gd->height;
  FBDialogWindow->InfoNY = FBDialogWindow->ItemsY + FBDialogWindow->NrItemLines * Items_Line_C_Gd->height;
  FBDialogWindow->InfoY  = FBDialogWindow->InfoNY + InfoN_C_Gd->height;
  FBDialogWindow->InfoSY = FBDialogWindow->InfoY  + FBDialogWindow->NrInfoLines * Info_C_Gd->height;      //=Border_S
  FBDialogWindow->OSDHeight = FBDialogWindow->InfoSY + Border_S_Gd->height;
  FBDialogWindow->ItemLineWidth = Items_IconButton_Gd->width + FBDialogWindow->NrItemColumns * Items_Line_C_Gd->width  + Items_Line_E_Gd->width;
}
