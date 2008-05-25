#include "../libFireBird.h"

// ------------------------------ MPMenuCheck ------------------------------------
//
bool isMPMenu (void)
{
  dword                 *pOSD1Info, *pOSD2Info;

  pOSD1Info = (dword *) ((*EMMA_DGRP_OSD1CTR & 0x0fffffff) | 0x80000000);
  if ((dword) pOSD1Info != 0x80000000)
  {
    if ((*pOSD1Info & 0x00003000) != 0x00002000) return FALSE;
  }

  pOSD2Info = (dword *) ((*EMMA_DGRP_OSD2CTR & 0x0fffffff) | 0x80000000);
  if ((dword) pOSD2Info != 0x80000000)
  {
    if ((*pOSD2Info & 0x00003000) != 0x00002000) return FALSE;
  }

  return TRUE;
}
