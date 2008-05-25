#include		<tap.h>
#include		"libFireBird.h"
#include                "TAPCOM_DemoServer.h"

#define PROGRAM_NAME    "TAPCOM DemoClient 2"

TAP_ID                  (0x8E0A42FA);
TAP_PROGRAM_NAME	(PROGRAM_NAME);
TAP_AUTHOR_NAME		("FireBird");
TAP_DESCRIPTION		("TAPCOM DemoClient 2");
TAP_ETCINFO		(__DATE__);

char                    StatusStrings [8][25] = {"Open", "Server not available", "Ack", "Rejected", "Finished", "TAPCOM Version mismatch", "No channel"};


dword TAP_EventHandler(word event, dword param1, dword param2)
{
  static TAPCOM_Channel         Channel = 0;
  TAPCOM_Status                 Status;
  tTAPCOM_DemoServer_Parameter  Parameter;
  int                           ret = 0;

  (void) param2;

  if (event == EVT_KEY)
  {
    switch (param1)
    {
      case RKEY_4:
      {
        TAP_Print (PROGRAM_NAME ": Trying to reserve the server\n");
        Channel = TAPCOM_OpenChannel (TAPCOM_App_DemoServer, TAPCOM_DemoServer_ReserveServer, 0, NULL);

        Status = TAPCOM_GetStatus (Channel);
        TAP_Print (PROGRAM_NAME ": Server answered with status %s\n", StatusStrings [Status]);

        TAPCOM_CloseChannel (Channel);

        param1 = 0;
        break;
      }

      case RKEY_5:
      {
        TAP_Print (PROGRAM_NAME ": Trying to calculate\n");
        Parameter.Number1 = 1;
        Parameter.Number2 = 2;

        Channel = TAPCOM_OpenChannel (TAPCOM_App_DemoServer, TAPCOM_DemoServer_ExecAdd, TAPCOM_DemoServer_ParameterVersion, NULL); //(void*) &Parameter

        Status = TAPCOM_GetStatus (Channel);
        TAP_Print (PROGRAM_NAME ": Server answered with status %s\n", StatusStrings [Status]);

        if (Status == TAPCOM_Status_FINISHED)
        {
          ret = TAPCOM_GetReturnValue (Channel);
          TAP_Print (PROGRAM_NAME ": Return value was %d\n", ret);

          if (ret == TAPCOM_DemoServer_ResultOK) TAP_Print (PROGRAM_NAME ": Result = %d\n", Parameter.Result);
        }

        TAPCOM_CloseChannel (Channel);

        param1 = 0;

        break;
      }

      case RKEY_6:
      {
        TAP_Print (PROGRAM_NAME ": Trying to release the server\n");
        Channel = TAPCOM_OpenChannel (TAPCOM_App_DemoServer, TAPCOM_DemoServer_ReleaseServer, 0, NULL);

        Status = TAPCOM_GetStatus (Channel);
        TAP_Print (PROGRAM_NAME ": Server answered with status %s\n", StatusStrings [Status]);

        TAPCOM_CloseChannel (Channel);

        param1 = 0;
        break;
      }

      case RKEY_Uhf:
      {
        TAP_Print (PROGRAM_NAME ": Exiting\n");
        TAP_Exit();
        param1 = 0;
        break;
      }
    }
  }

  return param1;
}


int TAP_Main(void)
{
  return 1;
}

