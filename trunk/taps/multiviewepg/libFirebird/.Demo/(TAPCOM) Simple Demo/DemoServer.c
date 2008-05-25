#include                <tap.h>
#include                "libFireBird.h"
#include                "TAPCOM_DemoServer.h"

#define VERSION         "V1.0"
#define PROGRAM_NAME    "TAPCOM DemoServer"

TAP_ID                  (0x8E0A42F8);
TAP_PROGRAM_NAME        (PROGRAM_NAME" "VERSION);
TAP_AUTHOR_NAME         ("FireBird");
TAP_DESCRIPTION         ("TAPCOM DemoServer");
TAP_ETCINFO             (__DATE__);

//----------- TAP_EventHandler ------------------------------------------
//
dword TAP_EventHandler (word event, dword param1, dword param2)
{
  dword                 CallerID, ServiceID, ParamBlockVersion;
  tTAPCOM_DemoServer_Parameter *ParamBlock = NULL;
  TAPCOM_Channel        Channel = 0;
  static dword          ServerReservedByID = 0;

  (void) param2;



  if (event == EVT_TAPCOM)
  {
    Channel = TAPCOM_GetChannel (param1, &CallerID, &ServiceID, &ParamBlockVersion, (void*) &ParamBlock);
    if (Channel != 0)
    {
      switch (ServiceID)
      {
        case TAPCOM_DemoServer_ReserveServer:
        {
          if (ServerReservedByID)
          {
            TAP_Print (PROGRAM_NAME "  : Server is already reserved by 0x%8.8x\n", ServerReservedByID);
            TAPCOM_Reject (Channel);
          }
          else
          {
            TAP_Print (PROGRAM_NAME "  : Server sucessfully reserved by 0x%8.8x\n", CallerID);
            ServerReservedByID = CallerID;
            TAPCOM_Finish (Channel, TAPCOM_DemoServer_ResultOK);
          }
          break;
        }

        case TAPCOM_DemoServer_ExecAdd:
        {
          if (!ServerReservedByID || (CallerID != ServerReservedByID))
          {
            TAP_Print (PROGRAM_NAME "  : Server hasn't been reserved by 0x%8.8x\n", CallerID);
            TAPCOM_Reject (Channel);
          }
          else
          {
            if (ParamBlockVersion != TAPCOM_DemoServer_ParameterVersion)
            {
              TAP_Print (PROGRAM_NAME "  : Unexpected parameter version\n");
              TAPCOM_Finish (Channel, TAPCOM_DemoServer_ResultInvalidParameter);
            }
            else if (ParamBlock == NULL)
            {
              TAP_Print (PROGRAM_NAME "  : Parameters are needed to complete the command\n");
              TAPCOM_Finish (Channel, TAPCOM_DemoServer_ResultNoParameter);
            }
            else
            {
              ParamBlock->Result = ParamBlock->Number1 + ParamBlock->Number2;
              TAP_Print (PROGRAM_NAME "  : Calculation done, result = %d\n", ParamBlock->Result);
              TAPCOM_Finish (Channel, TAPCOM_DemoServer_ResultOK);
            }
          }
          break;
        }

        case TAPCOM_DemoServer_ReleaseServer:
        {
          if (CallerID == ServerReservedByID)
          {
            TAP_Print (PROGRAM_NAME "  : Server sucessfully released by 0x%8.8x\n", CallerID);
            ServerReservedByID = 0;
            TAPCOM_Finish (Channel, TAPCOM_DemoServer_ResultOK);
            break;
          }
          else
          {
            TAP_Print (PROGRAM_NAME "  : Server is reserved by different client 0x%8.8x\n", ServerReservedByID);
            TAPCOM_Reject (Channel);
          }

          break;
        }

        default:
        {
          TAP_Print (PROGRAM_NAME "  : Client 0x%8.8x is trying to access the unknown service 0x%8.8x\n", CallerID, ServiceID);
          TAPCOM_Reject (Channel);
          break;
        }
      }
    }
  }

  if ((event == EVT_IDLE) && ServerReservedByID)
  {
    if (!HDD_TAP_isRunning (ServerReservedByID))
    {
       TAP_Print (PROGRAM_NAME "  : The client 0x%8.8x has been terminated while I'm reserved for that client. Closing channel\n", ServerReservedByID);
       ServerReservedByID = 0;
       TAPCOM_Reject (Channel);
    }
  }

  if (event == EVT_KEY)
  {
    switch (param1)
    {
      case RKEY_Uhf:
      {
        TAP_Print (PROGRAM_NAME "  : Exiting\n");
        TAP_Exit();
        param1 = 0;
        break;
      }
    }
  }

  return param1;
}


//----------- TAP_Main ------------------------------------------
//
int TAP_Main(void)
{
  return 1;
}
