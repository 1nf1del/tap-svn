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
      case 'Ä':
        string[i] = 'ä';
        break;

      case 'Ö':
        string[i] = 'ö';
        break;

      case 'Ü':
        string[i] = 'ü';
        break;

      default:
        string[i] = tolower (string[i]);
    }

    i++;
  }
}
