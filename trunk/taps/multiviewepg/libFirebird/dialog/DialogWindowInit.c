#include                <string.h>
#include                "FBLib_dialog.h"

#include                "graphic/Items_NoIcons_IconButton.gd"
#include                "graphic/Items_NoIcons_Line_C.gd"
#include                "graphic/Items_NoIcons_Line_E.gd"
#include                "graphic/Items_NoLinesNoIcons_IconButton.gd"
#include                "graphic/Items_NoLinesNoIcons_Line_C.gd"
#include                "graphic/Items_NoLinesNoIcons_Line_E.gd"
#include                "graphic/Scroll_Back.gd"
#include                "graphic/Scroll_Cursor.gd"
#include                "graphic/Scroll_End.gd"
#include                "graphic/InfoS_C.gd"
#include                "graphic/InfoS_E.gd"
#include                "graphic/InfoS_W.gd"
#include                "graphic/Icon_Checkbox_Checked.gd"
#include                "graphic/Icon_Checkbox_Unchecked.gd"
#include                "graphic/Icon_Exclamation.gd"
#include                "graphic/Icon_Wait.gd"
#include                "graphic/Icon_OK.gd"

TYPE_GrData             *Items_NoIcons_IconButton_Gd;
TYPE_GrData             *Items_NoIcons_Line_C_Gd;
TYPE_GrData             *Items_NoIcons_Line_E_Gd;
TYPE_GrData             *Items_NoLinesNoIcons_IconButton_Gd;
TYPE_GrData             *Items_NoLinesNoIcons_Line_C_Gd;
TYPE_GrData             *Items_NoLinesNoIcons_Line_E_Gd;
TYPE_GrData             *Scroll_Back_Gd;
TYPE_GrData             *Scroll_Cursor_Gd;
TYPE_GrData             *Scroll_End_Gd;
TYPE_GrData             *InfoS_C_Gd;
TYPE_GrData             *InfoS_E_Gd;
TYPE_GrData             *InfoS_W_Gd;
TYPE_GrData             *Icon_Checkbox_Checked_Gd;
TYPE_GrData             *Icon_Checkbox_Unchecked_Gd;
TYPE_GrData             *Icon_Exclamation_Gd;
TYPE_GrData             *Icon_Wait_Gd;
TYPE_GrData             *Icon_OK_Gd;

tDialogWindow           *FBDialogWindow  = NULL;

void DialogWindowInit (tDialogWindow *DialogWindow, tDialogProfile *DialogProfile, dword X, dword Y, dword Width, dword NrItemLines, char *LTitle, char *CTitle, char *RTitle, dword ParameterWidth, dword InfoHeight, eDialogWindowTypes WindowType)
{
  if (FBDialogWindow || !(FBDialogWindow = DialogWindow) || !DialogProfile) return;

  Items_NoIcons_IconButton_Gd = &_Items_NoIcons_IconButton_Gd;
  Items_NoIcons_Line_C_Gd     = &_Items_NoIcons_Line_C_Gd;
  Items_NoIcons_Line_E_Gd     = &_Items_NoIcons_Line_E_Gd;
  Items_NoLinesNoIcons_IconButton_Gd = &_Items_NoLinesNoIcons_IconButton_Gd;
  Items_NoLinesNoIcons_Line_C_Gd     = &_Items_NoLinesNoIcons_Line_C_Gd;
  Items_NoLinesNoIcons_Line_E_Gd     = &_Items_NoLinesNoIcons_Line_E_Gd;
  Scroll_Back_Gd              = &_Scroll_Back_Gd;
  Scroll_Cursor_Gd            = &_Scroll_Cursor_Gd;
  Scroll_End_Gd               = &_Scroll_End_Gd;
  InfoS_C_Gd                  = &_InfoS_C_Gd;
  InfoS_E_Gd                  = &_InfoS_E_Gd;
  InfoS_W_Gd                  = &_InfoS_W_Gd;
  Icon_Checkbox_Checked_Gd    = &_Icon_Checkbox_Checked_Gd;
  Icon_Checkbox_Unchecked_Gd  = &_Icon_Checkbox_Unchecked_Gd;
  Icon_Exclamation_Gd         = &_Icon_Exclamation_Gd;
  Icon_Wait_Gd                = &_Icon_Wait_Gd;
  Icon_OK_Gd                  = &_Icon_OK_Gd;

  memset (FBDialogWindow, 0, sizeof (tDialogWindow));

  DialogWindowReInit(X, Y, Width, NrItemLines, ParameterWidth, InfoHeight);
  DialogWindowTitleChange(LTitle, CTitle, RTitle);
  DialogWindowTypeChange(WindowType);

  FBDialogWindow->DialogItems = NULL;
  FBDialogWindow->InfoItems  = NULL;
  DialogWindowTabulatorSet(0, PARAMETER);
  DialogWindowTabulatorSet(0, VALUE);

  // get space for items
  (void) HasEnoughItemMemory();

  FBDialogWindow->InfoOwnForeColor = TRUE;
  FBDialogWindow->hasCursor = TRUE;

  FBDialogProfile = DialogProfile;
  if (ProfileMayReload()) ProfileInit();
}
