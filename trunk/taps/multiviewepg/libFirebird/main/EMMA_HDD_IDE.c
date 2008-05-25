#include "tap.h"

volatile dword *EMMA_HDD_IDE_DMAData_Reg                         = (dword *) 0xb0012d00;
volatile dword *EMMA_HDD_IDE_PIOData_Reg                         = (dword *) 0xb0012d20;
volatile dword *EMMA_HDD_IDE_Feature_Error_Reg                   = (dword *) 0xb0012d24;
volatile dword *EMMA_HDD_IDE_SectorCount_Reg                     = (dword *) 0xb0012d28;
volatile dword *EMMA_HDD_IDE_LBALow_Reg                          = (dword *) 0xb0012d2c;
volatile dword *EMMA_HDD_IDE_LBAMid_Reg                          = (dword *) 0xb0012d30;
volatile dword *EMMA_HDD_IDE_LBAHigh_Reg                         = (dword *) 0xb0012d34;
volatile dword *EMMA_HDD_IDE_Device_Reg                          = (dword *) 0xb0012d38;
volatile dword *EMMA_HDD_IDE_Command_Status_Reg                  = (dword *) 0xb0012d3c;
volatile dword *EMMA_HDD_IDE_Device_Control_Alternate_Status_Reg = (dword *) 0xb0012d58;
