#include <ctype.h>
#include "../libFireBird.h"

//--------------------------------------- LowerCase --------------------------------
//
void LowerCase (char *string)
{
  dword                 i = 0;

  while (string [i])
  {
    switch (string[i])
    {
      case '�':
        string[i] = '�';
        break;

      case '�':
        string[i] = '�';
        break;

      case '�':
        string[i] = '�';
        break;

      default:
        string[i] = tolower (string[i]);
    }

    i++;
  }
}
