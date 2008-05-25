#include "../libFireBird.h"

int TAP_Osd_PutFreeColorGd (word rgn, int x, int y, TYPE_GrData * gd, bool sprite, dword FilterColor)
{
  int                   ret;
  int                   R, G, B;
  dword                 FR, FG, FB;
  word                  *PixelData, TempRgn;
  register dword        oy, cx, cy, p, P;

  //Use a temporary memory region
  TempRgn = TAP_Osd_Create (0, 0, gd->width, gd->height, 0, OSD_Flag_MemRgn);

  //Copy Gd data into OSD region
  ret = TAP_Osd_PutGd(TempRgn, 0, 0, gd, sprite);

  //Save the OSD so we get a memory pointer to the pixel data
  if (!ret) PixelData = (word *) TAP_Osd_SaveBox (TempRgn, 0, 0, gd->width, gd->height);

  if (ret || !PixelData)
  {
    TAP_Osd_Delete (TempRgn);
    return (ret ? ret : -1);
  }

  FR = (FilterColor >> 10) & 0x1f;
  FG = (FilterColor >>  5) & 0x1f;
  FB =  FilterColor        & 0x1f;

  //This routine manipulates the RGB data and assumes the 1555 format
  for (oy = 0, cy = 0; cy < gd->height; cy++)
  {
    for (cx = 0; cx < gd->width; cx++)
    {
      //Calculate the pixel index out of the X/Y coordinates
      p = cx + oy;

      //Extract the colors and apply the filter function
      P = PixelData [p];

      //This one uses a multiplication algorithm
      //R = (((P >> 10) & 0x1f) * FR) >> 5;
      //G = (((P >>  5) & 0x1f) * FG) >> 5;
      //B = (( P        & 0x1f) * FB) >> 5;

      //This one subtracts the complementary color
      B = (P & 0x1f) - 31 + FB; if (B < 0) B = 0;
      P >>= 5;
      G = (P & 0x1f) - 31 + FG; if (G < 0) G = 0;
      P >>= 5;
      R = (P & 0x1f) - 31 + FR; if (R < 0) R = 0;
      P >>= 5;

      PixelData [p] = ARGB(P, R, G, B);
    }

    oy += gd->width;
  }

  //Restore the modified OSD
  TAP_Osd_RestoreBox (TempRgn, 0, 0, gd->width, gd->height, PixelData);
  TAP_Osd_Copy (TempRgn, rgn, 0, 0, gd->width, gd->height, x, y, TRUE);
  TAP_MemFree (PixelData);
  TAP_Osd_Delete (TempRgn);

  return 0;
}
