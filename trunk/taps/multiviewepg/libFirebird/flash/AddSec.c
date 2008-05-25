#include "../libFireBird.h"

dword AddSec (dword date, byte dateSec, int add)
{
  word                  mjd;
  int                   hour, min, sec;

  mjd = MJD(date);
  hour= HOUR(date);
  min = MINUTE(date);
  sec = dateSec;

  sec += add % 60;

  if (sec < 0) min -= 1;
  else if (sec > 59) min += 1;

  add /= 60;

  min += add % 60;

  if (min < 0)
  {
    hour -= 1;
    min += 60;
  }
  else if (min > 59)
  {
    hour += 1;
    min -= 60;
  }

  hour += add / 60;

  if (hour < 0)
  {
    mjd -= 1;
    hour += 24;
  }
  else if (hour > 23)
  {
    mjd += 1;
    hour -= 24;
  }

  return DATE(mjd, hour, min);
}
