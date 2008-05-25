#include "tap.h"
#include "FileSelectorDemoServerTAPCOM.h"
#include "libFireBird.h"

#define PGM_NAME "FileSelectorDemoClient"
#define PGM_VERS "1"

TAP_ID(0x81bb1007);
TAP_PROGRAM_NAME(PGM_NAME " " PGM_VERS);
TAP_AUTHOR_NAME("ibbi");
TAP_DESCRIPTION("FileSelector Demo TAPCOM Client");
TAP_ETCINFO(__DATE__);


#define DELAY 250
#define ShowMessage(msg, dly) {content[0] = PGM_NAME; content[1] = msg; content[2] = NULL; ShowMessageWindow(content, 40, 40, FNT_Size_1926, ALIGN_LEFT, RGB(19,19,19), RGB(31,31,10), RGB(28,31,28), RGB(3,5,10), dly);}


dword TAP_EventHandler (word event, dword param1, dword param2)
{
  static bool staging = FALSE;
  static word stage = 0;
  static TAPCOM_Channel channel;
  static FileSelectorDemoServer_ParamBlock combuf;
  static dword oldtick, starttick;
  char *content[3], text[256];
  (void) param2;

  if (event == EVT_IDLE)
  {
    switch (stage)
    {
      // versuche, Verbindung herzustellen
      case 0:
        if ((channel = TAPCOM_OpenChannel(FileSelectorDemoServer_ID, FileSelectorDemoServer_Service_GetFileName, FileSelectorDemoServer_ParamBlock_Version, &combuf)))
        {
          if (TAPCOM_GetStatus(channel) == TAPCOM_Status_SERVER_NOT_AVAILABLE)
          {
            if (!staging)
            {
              ShowMessage("Server läuft nicht.", DELAY);
              staging = TRUE;
              stage = 1;
            }
            else
            {
              ShowMessage("Server konnte nicht gestartet werden.", DELAY);
              TAP_Exit();
            }

            TAPCOM_CloseChannel(channel);
          }
          else
          {
            ShowMessage("Verbindung zum Server ist hergestellt.", 0);
            stage = 2;
          }
        }
        else
        {
          ShowMessage("Interner TAPCOM-Fehler!", DELAY);
          TAP_Exit();
        }
        break;

      // versuche, Server zu starten
      case 1:
        ShowMessage("Server wird gestartet...", DELAY);
        (void) HDD_TAP_Start(FileSelectorDemoServer_FileName, FALSE, NULL, NULL);
        stage = 0;
        break;

      // prüfe Verbindung
      case 2:
        if (TAPCOM_GetStatus(channel) == TAPCOM_Status_ACKNOWLEDGED)
        {
          starttick = oldtick = TAP_GetTick();
          stage = 3;
        }
        else
        {
          EndMessageWin();
          ShowMessage("Server konnte Anfrage nicht bearbeiten.", DELAY);
          TAPCOM_CloseChannel(channel);
          TAP_Exit();
        }
        break;

      // warte auf Ergebnis
      case 3:
        if (TAPCOM_GetStatus(channel) == TAPCOM_Status_ACKNOWLEDGED)
        {
          if (TAP_GetTick() - oldtick >= 100)
          {
            sprintf(text, "Server läuft seit %d Sekunden.", (TAP_GetTick() - starttick) / 100);
            EndMessageWin();
            ShowMessage(text, 0);
            oldtick = TAP_GetTick();
          }
          break;
        }
        else if (TAPCOM_GetStatus(channel) == TAPCOM_Status_FINISHED)
        {
          if (TAPCOM_GetReturnValue(channel) == FileSelectorDemoServer_Result_OK)
          {
            sprintf(text, "Es wurde  %s  ausgewählt.", combuf.filename);
            EndMessageWin();
            ShowMessage(text, DELAY);
          }
          else
          {
            EndMessageWin();
            ShowMessage("Es wurde keine Datei gewählt.", DELAY);
          }
        }
        else
        {
          EndMessageWin();
          ShowMessage("Verbindung wurde unterbrochen.", DELAY);
        }

        ShowMessage("Verbindung wird beendet.", DELAY);
        TAPCOM_CloseChannel(channel);
        TAP_Exit();

        break;

      default:
        ;
    }
  }

  return param1;
}


int TAP_Main (void)
{
  return 1;
}
