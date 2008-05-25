#include <string.h>
#include "../libFireBird.h"

// take off spaces from a string's end
char *RTrim (char *s)
{
  char *p;

  if (s)
  {
    if ((p = strrchr(s, '\0')))
    {
      p--;

      while (p > s && *p == ' ') p--;

      *++p = '\0';
    }
  }

  return s;
}
