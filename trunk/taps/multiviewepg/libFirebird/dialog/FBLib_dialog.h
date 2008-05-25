#ifndef FBLIB_DIALOG_H
#define FBLIB_DIALOG_H

#include                "../libFireBird.h"

#define DEFAULTPROFILE  "DialogProfile.ini"
#define DIALOGPROFILE_MAGIC 0xfb0e1bde

#define PROGRESSBARLEFT                              9
#define PROGRESSBARWIDTH                           389
#define PROGRESSBARTOP                              62
#define PROGRESSBARHEIGHT                           22

#define DEFAULTBorderColor                      0xa9d5
#define DEFAULTTitleBackgroundColor             0xbe36
#define DEFAULTTitleTextColor                   0xffff
#define DEFAULTItemsLineBackgroundColor         0xadd4
#define DEFAULTItemsLineBackgroundSelectedColor 0x9531
#define DEFAULTItemsIconBackgroundColor         0xbe36
#define DEFAULTItemsIconBackgroundSelectedColor 0x9531
#define DEFAULTItemsTextColor                   0xffff
#define DEFAULTItemsTextDisabledColor           0xca52
#define DEFAULTItemsTextSelectedColor           0xffff
#define DEFAULTItemsGridColor                   0xa98f
#define DEFAULTInfoBackgroundColor              0x9531
#define DEFAULTScrollBarColor                   0x56b5
#define DEFAULTScrollScreen                     TRUE
#define DEFAULTScrollWrapAround                 TRUE
#define DEFAULTProgressBarColor                 0xadd4
#define DEFAULTButtonIn                         0xbe36
#define DEFAULTButtonOut                        0xbe36
#define DEFAULTFade                             1000
#define DEFAULTFontSize                         0
#define DEFAULTHDDAccess                        2

#define ITEMS_TO_ADD    11
#define TITLE_OFFSET_XY 24
#define ITEM_OFFSET_X    5

#define sizeof1st(x) (sizeof(x) / sizeof *(x))

void        BuildWindowBorder (void);
void        BuildWindowLine (void);
void        BuildWindowLineSelected (void);
void        BuildWindowInfo (void);
void        BuildWindowScrollBar (void);
void        BuildWindowTitle (void);
void        DrawWindowBorder (void);
void        DrawWindowTitle (void);
void        DrawWindowLine (int ItemIndex);
void        DrawWindowLines (void);
void        DrawWindowScrollBar (void);
void        DrawWindowInfo (void);
void        DrawMsgBoxTitle (void);
void        DrawMsgBoxButtons (void);
void        DrawProgressBarTitle (void);
void        DrawProgressBarBar (int Value, word BarColor);
bool        HasEnoughItemMemory (void);
void        CalcPrepare (void);
void        CalcTopIndex (int, int);
void        CheckSelectable (int, int);
bool        OSDLinesForeDirty (bool);
void        ProfileInit (void);
void        ProfileDirty (void);
INILOCATION ProfileLoad (char *, bool, char *);
bool        ProfileMayReload (void);
void        WindowDirty (void);

extern int PrevTopItem;
extern int PrevSelItem;

extern tDialogProfile       *FBDialogProfile;
extern tDialogWindow        *FBDialogWindow;
extern tDialogMsgBox        *FBDialogMsgBox;
extern tDialogProgressBar   *FBDialogProgressBar;

extern TYPE_GrData          *Border_InfoN_E_Gd;
extern TYPE_GrData          *Border_InfoN_W_Gd;
extern TYPE_GrData          *Border_Info_E_Gd;
extern TYPE_GrData          *Border_Info_W_Gd;
extern TYPE_GrData          *Border_Items_E_Gd;
extern TYPE_GrData          *Border_Items_W_Gd;
extern TYPE_GrData          *Border_N_Gd;
extern TYPE_GrData          *Border_NE_Gd;
extern TYPE_GrData          *Border_NW_Gd;
extern TYPE_GrData          *Border_S_Gd;
extern TYPE_GrData          *Border_SE_Gd;
extern TYPE_GrData          *Border_SW_Gd;
extern TYPE_GrData          *Title_C_Gd;
extern TYPE_GrData          *Title_E_Gd;
extern TYPE_GrData          *Title_W_Gd;

extern TYPE_GrData          *Items_IconButton_Gd;
extern TYPE_GrData          *Items_Line_C_Gd;
extern TYPE_GrData          *Items_Line_E_Gd;
extern TYPE_GrData          *Items_NoIcons_IconButton_Gd;
extern TYPE_GrData          *Items_NoIcons_Line_C_Gd;
extern TYPE_GrData          *Items_NoIcons_Line_E_Gd;
extern TYPE_GrData          *Items_NoLinesNoIcons_IconButton_Gd;
extern TYPE_GrData          *Items_NoLinesNoIcons_Line_C_Gd;
extern TYPE_GrData          *Items_NoLinesNoIcons_Line_E_Gd;

extern TYPE_GrData          *Scroll_Back_Gd;
extern TYPE_GrData          *Scroll_Cursor_Gd;
extern TYPE_GrData          *Scroll_End_Gd;
extern TYPE_GrData          *InfoN_C_Gd;
extern TYPE_GrData          *InfoN_E_Gd;
extern TYPE_GrData          *InfoN_W_Gd;
extern TYPE_GrData          *Info_C_Gd;
extern TYPE_GrData          *Info_W_Gd;
extern TYPE_GrData          *InfoS_C_Gd;
extern TYPE_GrData          *InfoS_E_Gd;
extern TYPE_GrData          *InfoS_W_Gd;

extern TYPE_GrData          *MsgBox_W_Title_Gd;
extern TYPE_GrData          *MsgBox_W_Border_Gd;
extern TYPE_GrData          *MsgBox_W_SubTitle_Gd;
extern TYPE_GrData          *MsgBox_W_Text_Gd;
extern TYPE_GrData          *MsgBox_C_Title_Gd;
extern TYPE_GrData          *MsgBox_C_Border_Gd;
extern TYPE_GrData          *MsgBox_C_SubTitle_Gd;
extern TYPE_GrData          *MsgBox_C_Text_Gd;
extern TYPE_GrData          *MsgBox_E_Title_Gd;
extern TYPE_GrData          *MsgBox_E_Border_Gd;
extern TYPE_GrData          *MsgBox_E_SubTitle_Gd;
extern TYPE_GrData          *MsgBox_E_Text_Gd;
extern TYPE_GrData          *MsgBox_In_Gd;
extern TYPE_GrData          *MsgBox_Out_Gd;

extern TYPE_GrData          *Icon_Checkbox_Checked_Gd;
extern TYPE_GrData          *Icon_Checkbox_Unchecked_Gd;
extern TYPE_GrData          *Icon_Exclamation_Gd;
extern TYPE_GrData          *Icon_Wait_Gd;
extern TYPE_GrData          *Icon_OK_Gd;

extern TYPE_GrData          *ProgressBar_Border_Top_Gd;
extern TYPE_GrData          *ProgressBar_Border_Bottom_Gd;
extern TYPE_GrData          *ProgressBar_Title_Gd;

#endif
