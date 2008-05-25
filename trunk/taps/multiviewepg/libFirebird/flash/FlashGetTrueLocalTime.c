#include "FBLib_flash.h"

//--------------------------------------- FlashGetTrueLocalTime --------------------------------
//
dword FlashGetTrueLocalTime (dword ToppyLocalDateTime, int StandardOffsetMin)
{
  short*                DST_addr;
  word                  year;
  byte                  month, day, weekDay, lsM, lsO;
  dword                 lsMDateTime, lsODateTime, UTCDateTime, LocalDateTime;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return 0;

  if (FlashOffset == 0)
  {
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: Flash library hasn't been initialized\n");
#endif
    return 0;
  }
  //Get the address of the local time offset value
  DST_addr = (short *) (FlashGetBlockStartAddress (ClockSetup) + 4);


  //Get the current date and time and calculate if DST is active
  UTCDateTime = AddTime (ToppyLocalDateTime, -*DST_addr);            // convert to GMT
  TAP_ExtractMjd (UTCDateTime >> 16, &year, &month, &day, &weekDay);

  lsM = (byte) (31 - ((int) (5 * year / 4) + 4) % 7);             // last Sunday in March
  lsO = (byte) (31 - ((int) (5 * year / 4) + 1) % 7);             // last Sunday in October

  lsMDateTime = (TAP_MakeMjd (year, 3, lsM) << 16) | 0x0200;
  lsODateTime = (TAP_MakeMjd (year, 10, lsO) << 16) | 0x0200;

  LocalDateTime = AddTime (UTCDateTime, StandardOffsetMin);            // convert to standard time zone

  if ((UTCDateTime > lsMDateTime) && (UTCDateTime < lsODateTime)) LocalDateTime = AddTime (LocalDateTime, 60); //Add one hour if DST is active

  return LocalDateTime;
}
