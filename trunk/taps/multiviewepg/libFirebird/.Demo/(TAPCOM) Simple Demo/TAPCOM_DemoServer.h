#ifndef __TAPCOM_DEMOSERVER__
#define __TAPCOM_DEMOSERVER__

#define TAPCOM_App_DemoServer               0x8E0A42F8

// TAPCOM Services:

#define TAPCOM_DemoServer_ReserveServer     1
#define TAPCOM_DemoServer_ExecAdd           2
#define TAPCOM_DemoServer_ReleaseServer     3

// ... und die dazugehörenden Datenstrukturen:

typedef struct
{
  dword                 Number1;
  dword                 Number2;
  dword                 Result;
} tTAPCOM_DemoServer_Parameter;

#define TAPCOM_DemoServer_ParameterVersion  1

//TAPCOM Result Codes

#define TAPCOM_DemoServer_ResultOK                0
#define TAPCOM_DemoServer_ResultNoParameter       -1
#define TAPCOM_DemoServer_ResultInvalidParameter  -2


#endif
