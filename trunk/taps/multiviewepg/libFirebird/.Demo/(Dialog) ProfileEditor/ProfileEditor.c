#include                <tap.h>
#include                <string.h>
#include                "libFireBird.h"

#include                "icon_red.gd"
#include                "icon_green.gd"
#include                "icon_blue.gd"
#include                "key_red.gd"
#include                "key_green.gd"
#include                "key_blue.gd"
#include                "key_progdown.gd"
#include                "key_progup.gd"
#include                "key_voldown.gd"
#include                "key_volup.gd"
#include                "key_exit.gd"

#define PROGRAM_NAME    "ProfileEditor"
#define VERSION         "V1.2a"
#define LANGINI         "ProfileEditor.lng"

#ifndef RKEY_Red
#define RKEY_Red        RKEY_NewF1
#endif

#ifndef RKEY_Green
#define RKEY_Green      RKEY_F2
#endif

#ifndef RKEY_Blue
#define RKEY_Blue       RKEY_F4
#endif

TAP_ID                  (0x8E0A42F1);
TAP_PROGRAM_NAME	(PROGRAM_NAME " " VERSION);
TAP_AUTHOR_NAME		("FireBird");
TAP_DESCRIPTION		("Modifies a dialog's profile");
TAP_ETCINFO		(__DATE__);



/*****************************************************************************************************************************/
/* Prototypes                                                                                                                */
/*****************************************************************************************************************************/
char   *RGB2String (word RGB);
char   *WString (word w);
void    SetProfileColorItem (void);
void    ProgressBar (void);
bool    Modified = FALSE;

typedef enum
{
  CURRENTMode_Select,
  CURRENTMode_EditRed,
  CURRENTMode_EditGreen,
  CURRENTMode_EditBlue
} eCurrentMode;

enum
{
  DIALOGITEM_BorderColor,
  DIALOGITEM_TitleBackgroundColor,
  DIALOGITEM_TitleTextColor,
  DIALOGITEM_ItemsLineBackgroundColor,
  DIALOGITEM_ItemsLineBackgroundSelectedColor,
  DIALOGITEM_ItemsIconBackgroundColor,
  DIALOGITEM_ItemsIconBackgroundSelectedColor,
  DIALOGITEM_ItemsTextColor,
  DIALOGITEM_ItemsTextDisabledColor,
  DIALOGITEM_ItemsTextSelectedColor,
  DIALOGITEM_ItemsGridColor,
  DIALOGITEM_InfoBackgroundColor,
  DIALOGITEM_ScrollBarColor,
  DIALOGITEM_ProgressBarColor,
  DIALOGITEM_ButtonIn,
  DIALOGITEM_ButtonOut,
  DIALOGITEM_ScrollScreen,
  DIALOGITEM_ScrollWrapAround,
  DIALOGITEM_Fade,
  DIALOGITEM_FontSize,
  DIALOGITEM_HDDAccess,
  DIALOGITEM_Separator,
  DIALOGITEM_Exit
};

typedef enum
{
  STRING_Allowed,
  STRING_Avoid,
  STRING_Exit,
  STRING_ExitEditorColor,
  STRING_Yes,
  STRING_No,
  STRING_Small,
  STRING_Medium,
  STRING_BorderColor,
  STRING_ButtonIn,
  STRING_ButtonOut,
  STRING_ColorEditor,
  STRING_SaveChanges,
  STRING_Fade,
  STRING_FontSize,
  STRING_HDDAccess,
  STRING_InfoBackgroundColor,
  STRING_ItemsGridColor,
  STRING_ItemsIconBackgroundColor,
  STRING_ItemsIconBackgroundSelectedColor,
  STRING_ItemsLineBackgroundColor,
  STRING_ItemsLineBackgroundSelectedColor,
  STRING_ItemsTextColor,
  STRING_ItemsTextDisabledColor,
  STRING_ItemsTextSelectedColor,
  STRING_ProgressBarColor,
  STRING_ScrollEditColor,
  STRING_ScrollPage,
  STRING_ScrollBarColor,
  STRING_ScrollScreen,
  STRING_ScrollWrapAround,
  STRING_SelectColor,
  STRING_System,
  STRING_TitleBackgroundColor,
  STRING_TitleTextColor,
  STRING_NRStrings
} TEXTSTRINGS;

tDialogWindow           DialogWindow;
tDialogProfile          DialogProfile;
tDialogMsgBox           DialogMsgBox;
tDialogProgressBar      DialogProgressBar;
eCurrentMode            CurrentMode = CURRENTMode_Select;

bool                    ShowMainWindow;
word                    *pProfileColorItem = NULL;
word                    OldFade;


void SetProfileColorItem (void)
{
  switch (DialogWindow.SelectedItem)
  {
    case DIALOGITEM_BorderColor                      : pProfileColorItem = &DialogProfile.BorderColor; break;
    case DIALOGITEM_TitleBackgroundColor             : pProfileColorItem = &DialogProfile.TitleBackgroundColor; break;
    case DIALOGITEM_TitleTextColor                   : pProfileColorItem = &DialogProfile.TitleTextColor; break;
    case DIALOGITEM_ItemsLineBackgroundColor         : pProfileColorItem = &DialogProfile.ItemsLineBackgroundColor; break;
    case DIALOGITEM_ItemsLineBackgroundSelectedColor : pProfileColorItem = &DialogProfile.ItemsLineBackgroundSelectedColor; break;
    case DIALOGITEM_ItemsIconBackgroundColor         : pProfileColorItem = &DialogProfile.ItemsIconBackgroundColor; break;
    case DIALOGITEM_ItemsIconBackgroundSelectedColor : pProfileColorItem = &DialogProfile.ItemsIconBackgroundSelectedColor; break;
    case DIALOGITEM_ItemsTextColor                   : pProfileColorItem = &DialogProfile.ItemsTextColor; break;
    case DIALOGITEM_ItemsTextDisabledColor           : pProfileColorItem = &DialogProfile.ItemsTextDisabledColor; break;
    case DIALOGITEM_ItemsTextSelectedColor           : pProfileColorItem = &DialogProfile.ItemsTextSelectedColor; break;
    case DIALOGITEM_ItemsGridColor                   : pProfileColorItem = &DialogProfile.ItemsGridColor; break;
    case DIALOGITEM_InfoBackgroundColor              : pProfileColorItem = &DialogProfile.InfoBackgroundColor; break;
    case DIALOGITEM_ScrollBarColor                   : pProfileColorItem = &DialogProfile.ScrollBarColor; break;
    case DIALOGITEM_ProgressBarColor                 : pProfileColorItem = &DialogProfile.ProgressBarColor; break;
    case DIALOGITEM_ButtonIn                         : pProfileColorItem = &DialogProfile.ButtonIn; break;
    case DIALOGITEM_ButtonOut                        : pProfileColorItem = &DialogProfile.ButtonOut; break;
    default:
      pProfileColorItem = NULL;
      ;
  }
}


void ProgressBar (void)
{
  int i, imax = 10;

  DialogProgressBarInit (&DialogProgressBar, &DialogProfile, 155, 275, "ProgressBar", 0, imax);
  DialogProgressBarShow ();
  DialogProgressBarSet (0, DialogProfile.ProgressBarColor);

  for (i = 0; i <= imax; i++)
  {
    DialogProgressBarSet (i, DialogProfile.ProgressBarColor);
    TAP_Delay(25);
  }

  TAP_Delay(25);
  DialogProgressBarExit ();
}


dword TAP_EventHandler(word event, dword param1, dword param2)
{
  word                    c;

  (void) param2;

  if ((event != EVT_KEY) && (event != EVT_IDLE)) return param1;

  if (ShowMainWindow)
  {
    //Display the dialog
    DialogWindowShow ();
    ShowMainWindow = FALSE;
  }

  //Send all events to the messagebox dialog if it is visible
  if (DialogMsgBox.isVisible)
  {
    DialogEvent (&event, &param1, &param2);

    //Check if the dialog disappeared (user has selected a button or typed exit)
    if (!DialogMsgBox.isVisible)
    {
      switch (DialogMsgBox.SelectedButton)
      {
        case -1:    //User has pressed exit
          break;

        case 0:     //First button

          //User has selected YES. Save the profile.
          DialogProfileSaveDefault();
          DialogWindowHide ();
          TAP_Exit();
          break;

        case 1:     //Second button

          //User has selected NO. Simply exit.
          DialogProfile.Fade = OldFade;
          DialogWindowHide ();
          TAP_Exit();
          break;

        case 2:     //Third button
          break;

        case 3:     //Fourth button
          break;

        case 4:     //Fifth button
          break;

        default:    //This shouldn't happen
          ;
      }
    }
    return 0;
  }


  if (event == EVT_KEY)
  {
    if (CurrentMode == CURRENTMode_Select)
    {
      if (DialogWindow.SelectedItem != DIALOGITEM_Fade || (param1 != RKEY_VolUp && param1 != RKEY_VolDown)) DialogEvent (&event, &param1, &param2);
      SetProfileColorItem();
    }
    switch (param1)
    {
      case RKEY_ChDown:
      {
        switch (CurrentMode)
        {
          case CURRENTMode_EditRed:
            if (pProfileColorItem)
            {
              Modified = TRUE;
              c = (*pProfileColorItem >> 10) & 0x1f;
              if (c > 0) c--;
              *pProfileColorItem = (*pProfileColorItem & 0x83ff) | (c << 10);
              DialogWindowItemChangeValue (DialogWindow.SelectedItem, RGB2String (*pProfileColorItem), 0);
              DialogWindowRefresh ();
            }
            break;

          case CURRENTMode_EditGreen:
            if (pProfileColorItem)
            {
              Modified = TRUE;
              c = (*pProfileColorItem >> 5) & 0x1f;
              if (c > 0) c--;
              *pProfileColorItem = (*pProfileColorItem & 0xfc1f) | (c << 5);
              DialogWindowItemChangeValue (DialogWindow.SelectedItem, RGB2String (*pProfileColorItem), 0);
              DialogWindowRefresh ();
            }
            break;

          case CURRENTMode_EditBlue:
            if (pProfileColorItem)
            {
              Modified = TRUE;
              c = *pProfileColorItem & 0x1f;
              if (c > 0) c--;
              *pProfileColorItem = (*pProfileColorItem & 0xffe0) | c;
              DialogWindowItemChangeValue (DialogWindow.SelectedItem, RGB2String (*pProfileColorItem), 0);
              DialogWindowRefresh ();
            }
            break;

          default:
            ;
        }
        break;
      }

      case RKEY_ChUp:
      {
        switch (CurrentMode)
        {
          case CURRENTMode_EditRed:
            if (pProfileColorItem)
            {
              Modified = TRUE;
              c = (*pProfileColorItem >> 10) & 0x1f;
              if (c < 31) c++;
              *pProfileColorItem = (*pProfileColorItem & 0x83ff) | (c << 10);
              DialogWindowItemChangeValue (DialogWindow.SelectedItem, RGB2String (*pProfileColorItem), 0);
              DialogWindowRefresh ();
            }
            break;

          case CURRENTMode_EditGreen:
            if (pProfileColorItem)
            {
              Modified = TRUE;
              c = (*pProfileColorItem >> 5) & 0x1f;
              if (c < 31) c++;
              *pProfileColorItem = (*pProfileColorItem & 0xfc1f) | (c << 5);
              DialogWindowItemChangeValue (DialogWindow.SelectedItem, RGB2String (*pProfileColorItem), 0);
              DialogWindowRefresh ();
            }
            break;

          case CURRENTMode_EditBlue:
            if (pProfileColorItem)
            {
              Modified = TRUE;
              c = *pProfileColorItem & 0x1f;
              if (c < 31) c++;
              *pProfileColorItem = (*pProfileColorItem & 0xffe0) | c;
              DialogWindowItemChangeValue (DialogWindow.SelectedItem, RGB2String (*pProfileColorItem), 0);
              DialogWindowRefresh ();
            }
            break;

          default:
            ;
        }
        break;
      }

      case RKEY_Red:
      {
        if (DialogWindow.SelectedItem <= DIALOGITEM_ButtonOut)
        {
          if (CurrentMode != CURRENTMode_EditRed)
          {
            CurrentMode = CURRENTMode_EditRed;
            DialogWindowItemChangeIcon (DialogWindow.SelectedItem, DialogItemIcon_User, &_icon_red_Gd);
          }
          else
          {
            CurrentMode = CURRENTMode_Select;
            DialogWindowItemChangeIcon (DialogWindow.SelectedItem, DialogItemIcon_None, NULL);
          }
          DialogWindowRefresh ();
        }
        break;
      }

      case RKEY_Green:
      {
        if (DialogWindow.SelectedItem <= DIALOGITEM_ButtonOut)
        {
          if (CurrentMode != CURRENTMode_EditGreen)
          {
            CurrentMode = CURRENTMode_EditGreen;
            DialogWindowItemChangeIcon (DialogWindow.SelectedItem, DialogItemIcon_User, &_icon_green_Gd);
          }
          else
          {
            CurrentMode = CURRENTMode_Select;
            DialogWindowItemChangeIcon (DialogWindow.SelectedItem, DialogItemIcon_None, NULL);
          }
          DialogWindowRefresh ();
        }
        break;
      }

      case RKEY_Blue:
      {
        if (DialogWindow.SelectedItem <= DIALOGITEM_ButtonOut)
        {
          if (CurrentMode != CURRENTMode_EditBlue)
          {
            CurrentMode = CURRENTMode_EditBlue;
            DialogWindowItemChangeIcon (DialogWindow.SelectedItem, DialogItemIcon_User, &_icon_blue_Gd);
          }
          else
          {
            CurrentMode = CURRENTMode_Select;
            DialogWindowItemChangeIcon (DialogWindow.SelectedItem, DialogItemIcon_None, NULL);
          }
          DialogWindowRefresh ();
        }
        break;
      }

      case RKEY_VolUp:
      {
        if (CurrentMode == CURRENTMode_Select)
        {
          if (0xffff - DialogProfile.Fade < 50) DialogProfile.Fade = 0;
          else DialogProfile.Fade += 50;
          DialogWindowItemChangeValue (DialogWindow.SelectedItem, WString(DialogProfile.Fade), 0);
          DialogWindowRefresh ();
          Modified = TRUE;
        }
        break;
      }

      case RKEY_VolDown:
      {
        if (CurrentMode == CURRENTMode_Select)
        {
          if (DialogProfile.Fade < 50) DialogProfile.Fade = 65500;
          else DialogProfile.Fade -= 50;
          DialogWindowItemChangeValue (DialogWindow.SelectedItem, WString(DialogProfile.Fade), 0);
          DialogWindowRefresh ();
          Modified = TRUE;
        }
        break;
      }

      case RKEY_Ok:
      {
        switch (DialogWindow.SelectedItem)
        {
          case DIALOGITEM_ProgressBarColor:
            ProgressBar();
            break;

          case DIALOGITEM_ScrollScreen:
            DialogProfile.ScrollScreen = !DialogProfile.ScrollScreen;
            DialogWindowItemChangeValue (DialogWindow.SelectedItem, DialogProfile.ScrollScreen == 0 ? LangGetString (STRING_No) : LangGetString (STRING_Yes), 0);
            DialogWindowRefresh ();
            Modified = TRUE;
            break;

          case DIALOGITEM_ScrollWrapAround:
            DialogProfile.ScrollWrapAround = !DialogProfile.ScrollWrapAround;
            DialogWindowItemChangeValue (DialogWindow.SelectedItem, DialogProfile.ScrollWrapAround == 0 ? LangGetString (STRING_No) : LangGetString (STRING_Yes), 0);
            DialogWindowRefresh ();
            Modified = TRUE;
            break;

          case DIALOGITEM_Fade:
            DialogWindowHide();
            DialogWindowShow();
            break;

          case DIALOGITEM_FontSize:
            DialogProfile.FontSize = !DialogProfile.FontSize;
            DialogWindowItemChangeValue (DialogWindow.SelectedItem, DialogProfile.FontSize ? LangGetString (STRING_Medium) : LangGetString (STRING_Small), 0);
            DialogWindowRefresh ();
            Modified = TRUE;
            break;

          case DIALOGITEM_HDDAccess:
            DialogProfile.HDDAccess++;
            DialogProfile.HDDAccess %= 3;
            DialogWindowItemChangeValue (DialogWindow.SelectedItem, DialogProfile.HDDAccess == 0 ? LangGetString (STRING_Avoid) : (DialogProfile.HDDAccess == 1 ? LangGetString (STRING_Allowed) : LangGetString (STRING_System)), 0);
            DialogWindowRefresh ();
            Modified = TRUE;
            break;

          case DIALOGITEM_Exit:
            if (Modified)
            {
              DialogMsgBoxInit (&DialogMsgBox, &DialogProfile, "ColorEditor", LangGetString (STRING_SaveChanges));
              DialogMsgBoxShowYesNo (0);
            }
            else
            {
              DialogWindowHide ();
              TAP_Exit();
            }
            break;

          default:
            ;
        }
        break;
      }

      case RKEY_Exit:
      {
        if (CurrentMode == CURRENTMode_Select)
        {
          if (Modified)
          {
            DialogMsgBoxInit (&DialogMsgBox, &DialogProfile, "ColorEditor", LangGetString (STRING_SaveChanges));
            DialogMsgBoxShowYesNo (1);
          }
          else
          {
            DialogWindowHide ();
            TAP_Exit();
          }
        }
        else
        {
          CurrentMode = CURRENTMode_Select;
          DialogWindowItemChangeIcon (DialogWindow.SelectedItem, DialogItemIcon_None, NULL);
          DialogWindowRefresh ();
        }
        break;
      }

      default:
        ;
    }
    return 0;
  }

  return param1;
}

char *RGB2String (word RGB)
{
  static char           s [16];

  TAP_SPrint (s, "R=%2.2d G=%2.2d B=%2.2d", (RGB >> 10) & 0x1f, (RGB >> 5) & 0x1f, RGB & 0x1f);

  return s;
}

char *WString (word w)
{
  static char           s [6];

  TAP_SPrint (s, "%u", w);

  return s;
}

int TAP_Main(void)
{
  char                  s [50];

  if (!LangLoadStrings (LANGINI, STRING_NRStrings, LAN_English))
  {
    TAP_SPrint (s, "Language file %s not found!", LANGINI);
    DialogMsgBoxInit (&DialogMsgBox, &DialogProfile, PROGRAM_NAME " " VERSION, s);
    DialogMsgBoxShowInfo (5000);
    do
    {
      DialogEvent (NULL, NULL, NULL);
    }while (DialogMsgBox.isVisible);
    DialogMsgBoxExit();

    return 0;
  }

  //Initialize all pointers and set the basic geometry
  DialogWindowInit (&DialogWindow, &DialogProfile, 0, 0, 520, 10, "Profile Editor", "", VERSION, 290, 70, FALSE);
  DialogWindowReInit ((720 - DialogWindow.OSDWidth) >> 1, (576 - DialogWindow.OSDHeight) >> 1, 520, 10, 290, 70);

  //Bear this in mind
  OldFade = DialogProfile.Fade;

  //Add all items to the list
  DialogWindowItemAdd (LangGetString (STRING_BorderColor)                      , 0, RGB2String (DialogProfile.BorderColor)                     , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_TitleBackgroundColor)             , 0, RGB2String (DialogProfile.TitleBackgroundColor)            , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_TitleTextColor)                   , 0, RGB2String (DialogProfile.TitleTextColor)                  , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ItemsLineBackgroundColor)         , 0, RGB2String (DialogProfile.ItemsLineBackgroundColor)        , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ItemsLineBackgroundSelectedColor) , 0, RGB2String (DialogProfile.ItemsLineBackgroundSelectedColor), 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ItemsIconBackgroundColor)         , 0, RGB2String (DialogProfile.ItemsIconBackgroundColor)        , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ItemsIconBackgroundSelectedColor) , 0, RGB2String (DialogProfile.ItemsIconBackgroundSelectedColor), 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ItemsTextColor)                   , 0, RGB2String (DialogProfile.ItemsTextColor)                  , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ItemsTextDisabledColor)           , 0, RGB2String (DialogProfile.ItemsTextDisabledColor)          , 0, TRUE, FALSE, DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ItemsTextSelectedColor)           , 0, RGB2String (DialogProfile.ItemsTextSelectedColor)          , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ItemsGridColor)                   , 0, RGB2String (DialogProfile.ItemsGridColor)                  , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_InfoBackgroundColor)              , 0, RGB2String (DialogProfile.InfoBackgroundColor)             , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ScrollBarColor)                   , 0, RGB2String (DialogProfile.ScrollBarColor)                  , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ProgressBarColor)                 , 0, RGB2String (DialogProfile.ProgressBarColor)                , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ButtonIn)                         , 0, RGB2String (DialogProfile.ButtonIn)                        , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ButtonOut)                        , 0, RGB2String (DialogProfile.ButtonOut)                       , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);

  DialogWindowItemAdd (LangGetString (STRING_ScrollScreen)                     , 0, DialogProfile.ScrollScreen == 0 ? LangGetString (STRING_No) : LangGetString (STRING_Yes)             , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);
  DialogWindowItemAdd (LangGetString (STRING_ScrollWrapAround)                 , 0, DialogProfile.ScrollWrapAround == 0 ? LangGetString (STRING_No) : LangGetString (STRING_Yes)         , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);

  DialogWindowItemAdd (LangGetString (STRING_Fade)                             , 0, WString(DialogProfile.Fade)                                , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);

  DialogWindowItemAdd (LangGetString (STRING_FontSize)                         , 0, DialogProfile.FontSize ? LangGetString (STRING_Medium) : LangGetString (STRING_Small)                , 0, TRUE, TRUE,  DialogItemIcon_None, NULL);

  DialogWindowItemAdd (LangGetString (STRING_HDDAccess)                        , 0, DialogProfile.HDDAccess == 0 ? LangGetString (STRING_Avoid) : (DialogProfile.HDDAccess == 1 ? LangGetString (STRING_Allowed) : LangGetString (STRING_System)), 0, TRUE, TRUE,  DialogItemIcon_None, NULL);

  DialogWindowItemAddSeparator();
  DialogWindowItemAdd (LangGetString (STRING_Exit), ARGB (1, 31, 31, 0), "", 0, TRUE, TRUE, DialogItemIcon_None, NULL);

  DialogWindowInfoAddIcon (3                        , 3, &_key_red_Gd);
  DialogWindowInfoAddIcon (5 +     _key_red_Gd.width, 3, &_key_green_Gd);
  DialogWindowInfoAddIcon (7 + 2 * _key_red_Gd.width, 3, &_key_blue_Gd);
  DialogWindowInfoAddS    (9 + 3 * _key_red_Gd.width, 1, 500, LangGetString (STRING_SelectColor), 0, 0, FNT_Size_1419, TRUE, ALIGN_LEFT);

  DialogWindowInfoAddIcon (5 +     _key_red_Gd.width, 20, &_key_progup_Gd);
  DialogWindowInfoAddIcon (7 + 2 * _key_red_Gd.width, 20, &_key_progdown_Gd);
  DialogWindowInfoAddS    (9 + 3 * _key_red_Gd.width, 18, 500, LangGetString (STRING_ScrollEditColor), 0, 0, FNT_Size_1419, TRUE, ALIGN_LEFT);

  DialogWindowInfoAddIcon (5 +     _key_red_Gd.width, 37, &_key_volup_Gd);
  DialogWindowInfoAddIcon (7 + 2 * _key_red_Gd.width, 37, &_key_voldown_Gd);
  DialogWindowInfoAddS    (9 + 3 * _key_red_Gd.width, 35, 500, LangGetString (STRING_ScrollPage), 0, 0, FNT_Size_1419, TRUE, ALIGN_LEFT);

  DialogWindowInfoAddIcon (7 + 2 * _key_red_Gd.width, 54, &_key_exit_Gd);
  DialogWindowInfoAddS    (9 + 3 * _key_red_Gd.width, 52, 500, LangGetString (STRING_ExitEditorColor), 0, 0, FNT_Size_1419, TRUE, ALIGN_LEFT);

  //This is needed because the firmware returns to normal mode after exiting TAP_Main, regardless of a previous TAP_ExitNormal();
  ShowMainWindow = TRUE;

  return 1;
}
