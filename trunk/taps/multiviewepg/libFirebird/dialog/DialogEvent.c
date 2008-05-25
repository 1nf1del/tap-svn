#include                "FBLib_dialog.h"

void DialogEvent (word *event, dword *param1, dword *param2)
{
  int                   nr = -1;

  //The MsgBox has the highest priority
  if (FBDialogMsgBox && FBDialogMsgBox->isVisible)
  {
    if (!event || (*event == EVT_IDLE))
    {
      if (FBDialogMsgBox->InfoBoxTimeout > 0 && TAP_GetTick() > FBDialogMsgBox->InfoBoxTimeout)
      {
        FBDialogMsgBox->SelectedButton = -1;
        DialogMsgBoxExit();
      }
    }
    else if (*event == EVT_KEY)
    {
      if (FBDialogMsgBox->NrButtons)
      {
        switch (*param1)
        {
          case RKEY_Exit:
            FBDialogMsgBox->SelectedButton = -1;
            // no break here
          case RKEY_Ok:
            DialogMsgBoxExit();
            break;

          case RKEY_1:
            nr = 1;
            // no break here
          case RKEY_2:
            if (nr == -1) nr = 2;
            // no break here
          case RKEY_3:
            if (nr == -1) nr = 3;
            // no break here
          case RKEY_4:
            if (nr == -1) nr = 4;
            // no break here
          case RKEY_5:
            if (nr == -1) nr = 5;
            if (FBDialogMsgBox->NrButtons >= nr)
            {
              FBDialogMsgBox->SelectedButton = nr - 1;
              DrawMsgBoxButtons();
            }
            break;

          case RKEY_VolUp:
            FBDialogMsgBox->SelectedButton = ++FBDialogMsgBox->SelectedButton % FBDialogMsgBox->NrButtons;
            DrawMsgBoxButtons();
            break;

          case RKEY_VolDown:
            FBDialogMsgBox->SelectedButton--;
            if (FBDialogMsgBox->SelectedButton < 0) FBDialogMsgBox->SelectedButton = FBDialogMsgBox->NrButtons - 1;
            DrawMsgBoxButtons();
            break;

          case RKEY_Prev:
            FBDialogMsgBox->SelectedButton = 0;
            DrawMsgBoxButtons();
            break;

          case RKEY_Next:
            if (FBDialogMsgBox->NrButtons)
            {
              FBDialogMsgBox->SelectedButton = FBDialogMsgBox->NrButtons - 1;
              DrawMsgBoxButtons();
            }
            break;

          default:
            return;
        }

        *param1 = 0;
        *param2 = 0;
      }
    }

    return;
  }

  //The dialog window has the lowest priority
  if (event && FBDialogWindow && FBDialogWindow->isVisible)
  {
    switch (*event)
    {
      case EVT_KEY:
        switch (*param1)
        {
          case RKEY_ChDown:
            DialogWindowScrollDown();
            break;

          case RKEY_ChUp:
            DialogWindowScrollUp();
            break;

          case RKEY_VolUp:
            DialogWindowScrollDownPage();
            break;

          case RKEY_VolDown:
            DialogWindowScrollUpPage();
            break;

          case RKEY_Prev:
            DialogWindowCursorSet (0);
            break;

          case RKEY_Next:
            DialogWindowCursorSet (FBDialogWindow->NrItems - 1);
            break;

          default:
            return;
        }

        *param1 = 0;
        *param2 = 0;
        break;
    }

    return;
  }
}
