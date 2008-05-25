#include "../libFireBird.h"

char *TimeFormat(dword DateTime, byte Sec, eTimeStampFormat TimeStampFormat)
{
  static char           sTimeStamp [25];      //Log adds a SPACE to sTimeStamp. Do not make it too small!
  word                  Mjd, Year;
  byte                  Month, Day, WeekDay, Hour, Min;

  Mjd  = MJD    (DateTime);
  Hour = HOUR   (DateTime);
  Min  = MINUTE (DateTime);
  TAP_ExtractMjd (Mjd, &Year, &Month, &Day, &WeekDay);

  switch (TimeStampFormat)
  {
    case TIMESTAMP_HM     : TAP_SPrint (sTimeStamp, "%02d:%02d", Hour, Min); break;
    case TIMESTAMP_HMS    : TAP_SPrint (sTimeStamp, "%02d:%02d:%02d", Hour, Min, Sec); break;
    case TIMESTAMP_YMD    : TAP_SPrint (sTimeStamp, "%02d-%02d-%02d", Year, Month, Day); break;
    case TIMESTAMP_YMDHM  : TAP_SPrint (sTimeStamp, "%04d-%02d-%02d %02d:%02d", Year, Month, Day, Hour, Min); break;
    case TIMESTAMP_YMDHMS : TAP_SPrint (sTimeStamp, "%04d-%02d-%02d %02d:%02d:%02d", Year, Month, Day, Hour, Min, Sec); break;
    case TIMESTAMP_FNYMDHM: TAP_SPrint (sTimeStamp, "%04d%02d%02d_%02d%02d", Year, Month, Day, Hour, Min); break;
    default               : sTimeStamp [0] = '\0';
  }

  return sTimeStamp;
}
