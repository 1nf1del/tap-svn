#include <string.h>
#include <ctype.h>
#include "FBLib_string.h"

// case-insensitive version of strstr()
char *stricstr (const char *s1, const char *s2)
{
  bool found = FALSE;
  char start[3];
  char *str = NULL;

  if (*s2)
  {
    size_t len = strlen(s2) - 1;

    start[0] = toupper(*s2);
    start[1] = tolower(*s2);
    start[2] = '\0';

    s2++;

    do
    {
      if ((str = strpbrk(s1, start)))
      {
        s1 = str + 1;
        found = (strncasecmp(s1, s2, len) == 0);
      }
    }
    while (str && !found && *s1);
  }

  return (found ? str : NULL);
}
