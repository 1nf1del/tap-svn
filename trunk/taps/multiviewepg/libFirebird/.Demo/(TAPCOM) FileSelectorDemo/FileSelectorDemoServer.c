#include "tap.h"
#include "FileSelectorDemoServerTAPCOM.h"
#include "libFireBird.h"

#define PGM_NAME "FileSelectorDemoServer"
#define PGM_VERS "1"

TAP_ID(FileSelectorDemoServer_ID);
TAP_PROGRAM_NAME(PGM_NAME " " PGM_VERS);
TAP_AUTHOR_NAME("ibbi");
TAP_DESCRIPTION("FileSelector Demo TAPCOM Server");
TAP_ETCINFO(__DATE__);


dword TAP_EventHandler (word event, dword param1, dword param2)
{
  static bool inCommunication = FALSE, fileselector;
  static dword last_key = 0;
  static TAPCOM_Channel channel;
  dword caller, service, version;
  static FileSelectorDemoServer_ParamBlock *combuf;
  (void) param2;

  if (!inCommunication && (event == EVT_TAPCOM))
  {
    if ((channel = TAPCOM_GetChannel(param1, &caller, &service, &version, (void *) &combuf)))
    {
      if (service != FileSelectorDemoServer_Service_GetFileName ||
          version != FileSelectorDemoServer_ParamBlock_Version)
        TAPCOM_Reject(channel);
      else
      {
        inCommunication = TRUE;
        fileselector = FileSelector(NULL, NULL, NULL, 0);   // Start
      }
    }
  }

  if (inCommunication && (event == EVT_KEY))
  {
    last_key = param1;
    return 0;
  }

  if (inCommunication && (event == EVT_IDLE))
  {
    if (last_key != 0)
    {
      TAPCOM_StillAlive(channel);
      fileselector = FileSelector(&last_key, combuf->filename, NULL, 0);

      if (last_key == 0)
      {
        if (!fileselector)
        {
          TAPCOM_Finish(channel, (*combuf->filename ? FileSelectorDemoServer_Result_OK
                                                    : FileSelectorDemoServer_Result_Cancel));
          inCommunication = FALSE;
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
