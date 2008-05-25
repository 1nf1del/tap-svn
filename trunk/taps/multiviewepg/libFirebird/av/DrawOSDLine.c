#include                "../libFireBird.h"

void DrawOSDLine (word OSDRgn, dword Ax, dword Ay, dword Bx, dword By, dword Color)
{
  int dX, dY, Xincr, Yincr, dist, Xerr, Yerr, d;

  // distances
  dX = Bx - Ax;
  dY = By - Ay;

  // increments
  if (dX < 0)
  {
    dX = -dX;
    Xincr = -1;
  }
  else Xincr = (dX > 0 ? 1 : 0);

  if (dY < 0)
  {
    dY = -dY;
    Yincr = -1;
  }
  else Yincr = (dY > 0 ? 1 : 0);

  // longest distance
  dist = (dX > dY ? dX : dY);

  // initialize error terms
  Xerr = dX;
  Yerr = dY;

  // draw line

  for (d = 0; d < dist; d++)
  {
    TAP_Osd_PutPixel(OSDRgn, Ax, Ay, Color);   // plot the pixel

    Xerr += dX;
    Yerr += dY;

    if (Xerr > dist)
    {
      Xerr -= dist;
      Ax += Xincr;
    }

    if (Yerr > dist)
    {
      Yerr -= dist;
      Ay += Yincr;
    }
  }

  TAP_Osd_PutPixel(OSDRgn, Bx, By, Color);   // plot the pixel
}
