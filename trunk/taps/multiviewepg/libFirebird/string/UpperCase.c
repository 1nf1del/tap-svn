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
      case 'ä':
        string[i] = 'Ä';
        break;

      case 'ö':
        string[i] = 'Ö';
        break;

      case 'ü':
        string[i] = 'Ü';
        break;

      default:
        string[i] = toupper (string[i]);
    }

    i++;
  }
}
