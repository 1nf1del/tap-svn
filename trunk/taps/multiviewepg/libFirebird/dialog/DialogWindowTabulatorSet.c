#include                <string.h>
#include                "FBLib_dialog.h"

void DialogWindowTabulatorSet (dword Pos, eDialogItemArea Which)
{
  dword                 *tabs;
  int                   i;

  if (!FBDialogWindow || (Which != PARAMETER && Which != VALUE)) return;

  tabs = (Which == PARAMETER ? FBDialogWindow->TabsParameter : FBDialogWindow->TabsValue);

  if (!Pos) memset(tabs, 0, DIALOGWINDOW_MAX_TABS * sizeof(dword));
  else
  {
    for (i = 0; i < DIALOGWINDOW_MAX_TABS; i++)
    {
      if (Pos < tabs[i] || tabs[i] == 0)
      {
        if (i + 1 < DIALOGWINDOW_MAX_TABS) memmove(tabs + i + 1, tabs + i, (DIALOGWINDOW_MAX_TABS - i - 1) * sizeof(dword));

        tabs[i] = Pos;
        break;
      }
    }
  }
}
