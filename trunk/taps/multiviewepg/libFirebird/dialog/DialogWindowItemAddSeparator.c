#include                "FBLib_dialog.h"

void DialogWindowItemAddSeparator (void)
{
  DialogWindowItemAdd("", 0, "", 0, FALSE, FALSE, DialogItemIcon_None, NULL);
}
