#include                "FBLib_dialog.h"

void DrawProgressBarBar (int Value, word BarColor)
{
  int                   Perc;

  if (!FBDialogProgressBar || !FBDialogProfile || !FBDialogProgressBar->MemOSDRgn || !FBDialogProgressBar->OSDRgn) return;

  if (FBDialogProgressBar->Max == FBDialogProgressBar->Min) Perc = PROGRESSBARWIDTH;
  else Perc = (Value - FBDialogProgressBar->Min) * PROGRESSBARWIDTH / (FBDialogProgressBar->Max - FBDialogProgressBar->Min);

  if (Perc < 0) Perc = 0;
  if (Perc > PROGRESSBARWIDTH) Perc = PROGRESSBARWIDTH;

  if ((FBDialogProgressBar->Perc == Perc) && (FBDialogProgressBar->BarColor == BarColor)) return;

  if (Perc >= FBDialogProgressBar->Perc)
  {
    if (FBDialogProgressBar->BarColor == BarColor)
    {
      //Just draw the new bar
      TAP_Osd_FillBox (FBDialogProgressBar->MemOSDRgn, PROGRESSBARLEFT + FBDialogProgressBar->Perc, PROGRESSBARTOP, Perc - FBDialogProgressBar->Perc, PROGRESSBARHEIGHT, BarColor ? BarColor : FBDialogProfile->ProgressBarColor);
    }
    else
    {
      //Draw the complete bar
      TAP_Osd_FillBox (FBDialogProgressBar->MemOSDRgn, PROGRESSBARLEFT, PROGRESSBARTOP, Perc, PROGRESSBARHEIGHT, BarColor ? BarColor : FBDialogProfile->ProgressBarColor);
    }
  }
  else
  {
    if (FBDialogProgressBar->BarColor == BarColor)
    {
      //Just erase the delta
      TAP_Osd_FillBox (FBDialogProgressBar->MemOSDRgn, PROGRESSBARLEFT + Perc, PROGRESSBARTOP, FBDialogProgressBar->Perc - Perc, PROGRESSBARHEIGHT, RGB(0, 0, 0));
    }
    else
    {
      //We need to redraw the bar because the color has changed, then erase the delta
      TAP_Osd_FillBox (FBDialogProgressBar->MemOSDRgn, PROGRESSBARLEFT, PROGRESSBARTOP, Perc, PROGRESSBARHEIGHT, BarColor ? BarColor : FBDialogProfile->ProgressBarColor);
      TAP_Osd_FillBox (FBDialogProgressBar->MemOSDRgn, PROGRESSBARLEFT + Perc, PROGRESSBARTOP, FBDialogProgressBar->Perc - Perc, PROGRESSBARHEIGHT, RGB(0, 0, 0));
    }
  }

  TAP_Osd_PutFreeColorGd (FBDialogProgressBar->MemOSDRgn, 0, ProgressBar_Border_Top_Gd->height, ProgressBar_Border_Bottom_Gd, TRUE, FBDialogProfile->BorderColor);
  TAP_Osd_Copy (FBDialogProgressBar->MemOSDRgn, FBDialogProgressBar->OSDRgn, 0, 0, GetOSDRegionWidth(FBDialogProgressBar->MemOSDRgn), GetOSDRegionHeight(FBDialogProgressBar->MemOSDRgn), 0, 0, TRUE);

  FBDialogProgressBar->Value    = Value;
  FBDialogProgressBar->BarColor = BarColor;
  FBDialogProgressBar->Perc     = Perc;
}
