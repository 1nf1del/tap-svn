#include "../libFireBird.h"

//--------------------------------------- isAnyOSDVisible --------------------------------
//
bool isAnyOSDVisible (dword checkX, dword checkY, dword checkW, dword checkH)
{
  dword                 *pOSDInfo[2];
  dword                 Plane, pOSDe;
  bool                  _8bit;
  void                  *pAddr;   // speichert die vertikale Adresse
  dword                 x, y;     // Schleifenzähler für Bildschirm-Scan


  // Parameter prüfen
  if (checkX >= 720) checkX = 719;
  if (checkY >= 576) checkY = 575;
  if (checkX + checkW > 720) checkW = 720 - checkX;
  if (checkY + checkH > 576) checkH = 576 - checkY;

  pOSDInfo[0] = (dword *) ((*EMMA_DGRP_OSD1CTR & 0x0fffffff) | 0x80000000);   // Zeiger auf Plane 1
  pOSDInfo[1] = (dword *) ((*EMMA_DGRP_OSD2CTR & 0x0fffffff) | 0x80000000);   // Zeiger auf Plane 2

  // nur die geraden Punkte prüfen
  checkY /= 2;
  checkH /= 2;

  for (Plane = 0; Plane < 2; Plane++)
  {
    if ((dword) pOSDInfo[Plane] != 0x80000000)   // prüfen auf "0" Plane 1
    {
      pOSDe = (*(pOSDInfo[Plane] + 6) & 0x0fffffff) | 0x80000000;
      _8bit = ((*pOSDInfo[Plane] & 0x00003000) == 0x00002000);

      for (y = checkY; y < (checkY + checkH); y += 2)  // check only even plane, every 2nd line (every 4th line per frame)
      {
        // vertikale Adressen für 1. Halbbild
        if (_8bit) pAddr = (byte *) (pOSDe + 720 * y);
        else pAddr = (word *) (pOSDe + 720 * y * 2);

        for (x = checkX; x < (checkX + checkW); x += 4) // Schleife für die Spalten
          if ((_8bit ? ((byte *) pAddr)[x] : ((word *) pAddr)[x]) != 0) return TRUE;   // wenn ein Wert zurückgemeldet wird, ist irgend etwas auf den Bildschirm gezeichnet - dann TRUE zurückmelden
      }
    }
  }

  return FALSE;
}
