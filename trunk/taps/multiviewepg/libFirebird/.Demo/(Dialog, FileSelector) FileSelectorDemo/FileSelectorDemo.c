#include "tap.h"
#include "libFireBird.h"

#define PGM_NAME "FileSelectorDemo"
#define PGM_VERS "6"

TAP_ID(0x81bb1005);
TAP_PROGRAM_NAME(PGM_NAME " " PGM_VERS);
TAP_AUTHOR_NAME("ibbi & Sigittarius-E");
TAP_DESCRIPTION("FileSelector Demo");
TAP_ETCINFO(__DATE__);


static bool fileselector = FALSE;


dword TAP_EventHandler (word event, dword param1, dword param2)
{
  static dword last_key = 0;
  static char filename[TS_FILE_NAME_SIZE];

  (void) param2;

  if (event == EVT_KEY)
  {
    last_key = param1;
    return 0;
  }

  if (event == EVT_IDLE)
  {
    // Start
    if (!fileselector) fileselector = FileSelector(NULL, NULL, NULL, FS_DIALOGWINDOW);

    if (last_key != 0)
    {
      fileselector = FileSelector(&last_key, filename, NULL, 0);

      if (last_key == 0)
      {
        if (!fileselector)
        {
          if (*filename) ShowMessageWin(NULL, filename, NULL, 333);     // Auswahl ist erfolgt
          else ShowMessageWin(NULL, "Abbruch durch EXIT", NULL, 333);   // Abbruch

          TAP_Exit();
        }
      }
      else last_key = 0;
    }
  }

  return param1;
}


int TAP_Main (void)
{
  return 1;
}
