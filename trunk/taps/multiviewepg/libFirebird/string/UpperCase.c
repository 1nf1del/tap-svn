#include <ctype.h>
#include "../libFireBird.h"

//--------------------------------------- UpperCase --------------------------------
//
void UpperCase (char *string)
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
        string[i] = toupper (string[i]);
    }

    i++;
  }
}
