#include "../libFireBird.h"

//--------------------------------------- ShowMessageWin --------------------------------
//
void ShowMessageWin (char *title, char *lpMessage1, char *lpMessage2, dword dwDelay)
{
  char *content[4];

  content[0] = title;
  content[1] = lpMessage1;
  content[2] = lpMessage2;
  content[3] = NULL;

  ShowMessageWindow(content, 720, 576, FNT_Size_1926, ALIGN_CENTER, RGB(19,19,19), RGB(31,31,10), RGB(28,31,28), RGB(3,5,10), dwDelay);
}
