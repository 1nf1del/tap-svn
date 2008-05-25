#include "../libFireBird.h"

bool SetCrashBehaviour(tCrashMode Mode)
{
  dword                 Reboot, *MemMonitor, SendFrontPanelString;


  //Locate the call into the memory monitor from the ExceptionPrint routine
  if (!(MemMonitor = (dword*)FIS_fwMemMonitor())) return FALSE;


  switch (Mode)
  {
    case CRASHMODE_REBOOT:
      //Locate the entry point of the reboot routine
      if (!(Reboot = FIS_fwReboot())) return FALSE;
      MemMonitor [0] = 0x27bd0000;                  //addiu $sp,0  Needed to preserve the "addiu $sp" otherwise FindInstructionSequence
                                                    //will hit the previous routine on a consecutive run
      MemMonitor [1] = JAL_CMD | REL_ADDR(Reboot);
      MemMonitor [2] = NOP_CMD;
      break;

    case CRASHMODE_SHUTDOWN:
      //Send the code 0x21 0x01 to the front panel. This will turn off the Toppy immediately.
      if (!(SendFrontPanelString = FIS_fwSendToFP())) return FALSE;

      MemMonitor [0] = 0x27bd0000;
      MemMonitor [1] = 0x24190021;	                                //move  $t9,33=0x0021
      MemMonitor [2] = 0xa3b90024;	                                //sb    $t9,36($sp)
      MemMonitor [3] = 0x24180001;	                                //move  $t8,1
      MemMonitor [4] = 0xa3b80025;	                                //sb    $t8,37($sp)
      MemMonitor [5] = 0x27a40024;	                                //addiu $a0,$sp,36=0x0024
      MemMonitor [6] = JAL_CMD | REL_ADDR (SendFrontPanelString);	//jal   SendFrontPanelString
      break;

    case CRASHMODE_FREEZE:
      //Nothing to do
      break;

    default:
      return FALSE;
  }

  //Whatever we did, flush the data cache and invalidate the instruction cache
  FlushCache (MemMonitor, 16);

  return TRUE;
}
