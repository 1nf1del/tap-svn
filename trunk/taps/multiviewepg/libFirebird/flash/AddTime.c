#include "../libFireBird.h"

//--------------------------------------- AddTime --------------------------------
//
dword AddTime (dword date, int add) //add minutes to the day
{
  word                  day;
  short                 hour, min;

  day = (date >> 16) & 0xffff;
  hour= (date >> 8) & 0xff;
  min = (date) & 0xff;

  min += add % 60;
  if (min < 0)
  {
    hour-=1;
    min+=60;
  }
  else if (min > 59)
  {
    hour+=1;
    min-=60;
  }

  hour += add / 60;

  if (hour < 0)
  {
    day-=1;
    hour+=24;
  }
  else if (hour > 23)
  {
    day+=1;
    hour-=24;
  }

  return ((day<<16)|(hour<<8)|min);
}
