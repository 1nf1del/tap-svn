#include "FBLib_flash.h"

//--------------------------------------- FlashProgram --------------------------------
//
void FlashProgram (void)
{
  TYPE_TimerInfo        tTimerInfo;
  int                   Ret, i;

  //Recalculate the CRCs
  for (i = TVServices; i < BLOCKS; i++)
  {
    if (FlashBlockOffset [i] != FlashBlockOffset [i + 1]) *(word *) (FlashBlockOffset [i]) = CRC16 (0, (byte *) (FlashBlockOffset [i] + 4), FlashBlockOffset [i + 1] - FlashBlockOffset [i] - 4);
  }

  //Create and delete a dummy timer entry to write the changes into the flash
  tTimerInfo.isRec = 0;
  tTimerInfo.tuner = 0;
  tTimerInfo.svcType = 0;
  tTimerInfo.svcNum = 0;
  tTimerInfo.reservationType = RESERVE_TYPE_Onetime;
  tTimerInfo.startTime = TIMER_DATE;
  tTimerInfo.duration = 1;
  tTimerInfo.nameFix = 1;
  TAP_SPrint (tTimerInfo.fileName, TIMER_NAME);
  Ret = TAP_Timer_Add (&tTimerInfo);
  if (Ret == 0)
  {
    Ret=TAP_Timer_GetTotalNum ();
    for (i = 0; i < Ret; i++)
    {
      TAP_Timer_GetInfo (i, &tTimerInfo);
      if (tTimerInfo.startTime == TIMER_DATE && strcmp(tTimerInfo.fileName, TIMER_NAME) == 0)
      {
        TAP_Timer_Delete (i);
        break;
      }
    }
#ifdef DEBUG_FIREBIRDLIB
    if (i >= Ret) TAP_Print ("FireBirdLib: FlashProgram failed to delete a timer entry!\n");
#endif
  }
#ifdef DEBUG_FIREBIRDLIB
  else TAP_Print ("FireBirdLib: FlashProgram failed to create a timer entry!\n");
#endif
}
