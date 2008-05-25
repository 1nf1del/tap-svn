#include "tap.h"
#include "libFireBird.h"


#define PGM_NAME "DialogDemo"
#define PGM_VERS "1"

TAP_ID(0x81bb1000);
TAP_PROGRAM_NAME(PGM_NAME " " PGM_VERS);
TAP_AUTHOR_NAME("ibbi");
TAP_DESCRIPTION("A Dialog Demo");
TAP_ETCINFO(__DATE__);


#define WIN_W 200
#define WIN_L 10
#define WIN_I info_h(1)
#define WIN_P 35

tDialogWindow DialogWindow;
tDialogProfile DialogProfile;


dword TAP_EventHandler (word event, dword param1, dword param2)
{
  static bool ProfileChecked = FALSE;
  dword WinWidth, ParamWidth;

  if (event == EVT_IDLE)
  {
    if (!ProfileChecked)
    {
      // check the profile so that the user's preference for the hdd can be (automatically) followed
      DialogProfileCheck(&DialogProfile, PGM_NAME, FALSE);
      ProfileChecked = TRUE;
    }
  }

  if (event == EVT_KEY)
  {
    switch (param1)
    {
      // activate the window
      case RKEY_Ok:
        WinWidth = WIN_W;
        ParamWidth = WIN_P;

        // initialize the window
        DialogWindowInit(&DialogWindow, &DialogProfile, 0, 0, WinWidth, WIN_L, PGM_NAME, NULL, NULL, ParamWidth, WIN_I, DialogWindowType_NormalLinesNoIcons);

        // check the user's special preferences
        DialogProfileLoadMy(PGM_NAME, FALSE);

        // medium font needs new window calculation
        if (DialogProfile.FontSize)
        {
          WinWidth = WinWidth * 59 / 50;
          ParamWidth = ParamWidth * 59 / 50;

          // recalculate the window sizes
          DialogWindowReInit(0, 0, WinWidth, WIN_L, ParamWidth, WIN_I);
        }

        // now center the window
        DialogWindowReInit((720 - DialogWindow.OSDWidth) >> 1, (576 - DialogWindow.OSDHeight) >> 1, WinWidth, WIN_L, ParamWidth, WIN_I);

        // fill window
        DialogWindowItemAdd("First", 0, "Line", 0, TRUE, TRUE, DialogItemIcon_None, NULL);
        DialogWindowInfoAddS(5, 0, 100, "Info Area", 0, 0, FNT_Size_1419, (byte) FALSE, ALIGN_LEFT);

        // activate
        DialogWindowShow();
        param1 = 0;
        break;

      // deactivate the window
      case RKEY_Exit:
        DialogWindowExit();
        TAP_Exit();
        param1 = 0;
        break;

      // handle navigation keys
      default:
        DialogEvent(&event, &param1, &param2);
    }
  }

  return param1;
}


int TAP_Main (void)
{
  return 1;
}
