#include "../libFireBird.h"

dword                   fbl_rgn = 0;

//--------------------------------------- ShowMessageWindow --------------------------------
//
void ShowMessageWindow (char **content, dword pos_x, dword pos_y, byte fntSize, byte align, dword bdcolor, dword titlecolor, dword msgcolor, dword bgcolor, dword delay)
{
  dword                 extra_x, h_y, w = 0, h = 0, w1, i, j, extra, rahmen, y;
  const dword           extra_y = 10;
  word                  fbl_memrgn;

  if (content[0] == NULL && content[1] == NULL) content[0] = "";

  if (fntSize > FNT_Size_1926) fntSize = FNT_Size_1926;

  if (fntSize == FNT_Size_1419)
  {
    extra_x = 14;
    h_y = 19;
  }
  else if (fntSize == FNT_Size_1622)
  {
    extra_x = 16;
    h_y = 22;
  }
  else
  {
    extra_x = 20;
    h_y = 26;
  }

  h = extra_y;

  if (content[0])
  {
    w = TAP_Osd_GetW(content[0], 0, fntSize) + extra_x;
    h += h_y;
  }

  for (i = 1; (pos_y >= 576 ? 0 : pos_y) + h + h_y < 576 && content[i] != NULL; i++)
  {
    w1 = TAP_Osd_GetW(content[i], 0, fntSize) + extra_x;
    h += h_y;
    if (w1 > w) w = w1;
  }

  if (w > 720 - (pos_x >= 720 ? 0 : pos_x)) w = 720 - (pos_x >= 720 ? 0 : pos_x);

  if (pos_x >= 720) pos_x = (720 - w) / 2;
  if (pos_y >= 576) pos_y = (576 - h) / 2;

  extra = extra_x / (align == ALIGN_CENTER ? 4 : 2);
  rahmen = extra_x / 4;
  y = 0;

  fbl_memrgn = TAP_Osd_Create(pos_x, pos_y, w, h, 0, OSD_Flag_MemRgn);

  // draw border box
  TAP_Osd_FillBox(fbl_memrgn, 0, 0, w, h, bdcolor);

  // draw background box
  TAP_Osd_FillBox(fbl_memrgn, rahmen, rahmen, w - rahmen * 2, h - rahmen * 2, bgcolor);

  // show title
  if (content[0])
  {
    TAP_Osd_PutS(fbl_memrgn, extra_x / 4, y + extra_y / 2, w - extra_x / 4, content[0], titlecolor, bgcolor, 0, fntSize, (byte) FALSE, ALIGN_CENTER);
    y += h_y;
  }

  // show message lines
  for (j = 1; j < i; j++)
  {
    TAP_Osd_PutS(fbl_memrgn, extra, y + extra_y / 2, w - extra, content[j], msgcolor, bgcolor, 0, fntSize, (byte) FALSE, align);
    y += h_y;
  }

  if (fbl_rgn) TAP_Osd_Delete (fbl_rgn);
  fbl_rgn = TAP_Osd_Create(pos_x, pos_y, w, h, 0, 0);
  TAP_Osd_Copy (fbl_memrgn, fbl_rgn, 0, 0, w, h, 0, 0, FALSE);
  TAP_Osd_Delete (fbl_memrgn);

  if (delay)
  {
    TAP_Delay(delay);
    EndMessageWin();   // release rgn handle
  }
}
