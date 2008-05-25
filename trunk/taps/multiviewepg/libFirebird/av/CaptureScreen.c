#include "FBLib_av.h"
#include "../libFireBird.h"

// ------------------------------ CaptureScreen ------------------------------------
//
bool CaptureScreen ( byte mainSub, int width, int height, byte *pBuffer, bool bOSD, int alpha)
{
  int                   nR, nG, nB, n1 = 0, n2 = 0, i;
  dword                 color;
  byte                  *p;
  TYPE_VideoBank        videoBank;
  byte                  *picAddr, *chrAddr;
  int                   x, y, xx, yy, xOSD, yOSD;
  byte                  r, g, b;
  int                   ypos, ypos2, xpos, yPos, cPos;
  byte                  *_vAddr1b[576]; //OSD Plane 1 (8 bit)
  word                  *_vAddr1w[576]; //OSD Plane 1 (16 bit)
  byte                  *_vAddr2b[576]; //OSD Plane 2 (8 bit)
  word                  *_vAddr2w[576]; //OSD Plane 2 (16 bit)
  bool                  Plane1UsesLUT = FALSE, Plane2UsesLUT = FALSE;
  bool                  nRet = TRUE;

  dword                 *pOSD1Info = NULL, *pOSD2Info = NULL;
  dword                 pOSD1e, pOSD1o, pOSD2e, pOSD2o;
  dword                 OSDWidth = 0, OSDHeight = 0;

  typedef struct
  {
    byte                Alpha;
    byte                Y;
    byte                V;
    byte                U;
  } tLUT;

  //The Masterpiece uses this RGB lookup table for his 8 bit plane 1
  tLUT     *Plane1LUT = NULL, *Plane2LUT = NULL;

  if ( bOSD )
  {
    pOSD1Info = (dword *) ((*EMMA_DGRP_OSD1CTR & 0x0fffffff) | 0x80000000);  // pointer to Plane1
    pOSD2Info = (dword *) ((*EMMA_DGRP_OSD2CTR & 0x0fffffff) | 0x80000000);  // pointer to Plane2

    if ((dword) pOSD1Info != 0x80000000)           // Plane 1
    {
      pOSD1e = (*(pOSD1Info + 6) & 0x0fffffff) | 0x80000000;
      pOSD1o = (*(pOSD1Info + 7) & 0x0fffffff) | 0x80000000;
      for( i=0; i<288; i++ )
      {
        _vAddr1b[2*i] = (byte *) (pOSD1e + 720 * i);
        _vAddr1b[2*i+1] = (byte *) (pOSD1o + 720 * i);
        _vAddr1w[2*i] = (word *) (pOSD1e + 720 * i * 2);
        _vAddr1w[2*i+1] = (word *) (pOSD1o + 720 * i * 2);
      }

      //Check if plane 1 uses a 8 bit LUT (e.g. the Masterpiece)
      if ((*pOSD1Info & 0x00003000) == 0x00002000)
      {
        Plane1UsesLUT = TRUE;
        Plane1LUT = (tLUT*) ((*(pOSD1Info + 4) & 0x0fffffff) | 0x80000000);
      }
    }

    if ((dword) pOSD2Info != 0x80000000)           // Plane2
    {
      pOSD2e = (*(pOSD2Info + 6) & 0x0fffffff) | 0x80000000;
      pOSD2o = (*(pOSD2Info + 7) & 0x0fffffff) | 0x80000000;
      for( i=0; i<288; i++ )
      {
        _vAddr2b[2*i] = (byte *) (pOSD2e + 720 * i);
        _vAddr2b[2*i+1] = (byte *) (pOSD2o + 720 * i);
        _vAddr2w[2*i] = (word *) (pOSD2e + 720 * i * 2);
        _vAddr2w[2*i+1] = (word *) (pOSD2o + 720 * i * 2);
      }
      //Check if plane 2 uses a 8 bit LUT (e.g. deep in the menu structure)
      if ((*pOSD2Info & 0x00003000) == 0x00002000)
      {
        Plane2UsesLUT = TRUE;
        Plane2LUT = (tLUT*) ((*(pOSD2Info + 4) & 0x0fffffff) | 0x80000000);
      }
    }

    OSDHeight = *(pOSD1Info + 2);
    OSDWidth = (OSDHeight & 0xffff) + 1;
    OSDHeight = (OSDHeight >> 16) + 2;

    // get alpha for osd
    n1 = (alpha * 3) / 5; // Korrektur wegen Unterschied zum Topf Mischer!
    n2 = 100 - n1;
  }

  // clear videoBank
  memset( &videoBank, 0, sizeof(videoBank) );

  // capture screen and set adress
  if ( TAP_CaptureScreen( mainSub, &videoBank ) != 0 ) nRet = FALSE;
  if ( nRet )
  {
    picAddr = videoBank.yAddress;
    chrAddr = videoBank.cAddress;

    // convert from uyvy to rgb mode and scale image
    for( yy = 0; yy < height; yy++ )
    {
      y = (yy * videoBank.height) / height;
      yOSD = (yy * OSDHeight) / height;
      ypos = (( y  & 0xffffffe0) * 736) + ((y  & 0x1f) << 5);
      ypos2 = (((y / 2) & 0xffffffe0) * 736) + (((y / 2) & 0x1f) << 5);

      for( xx = 0; xx < width; xx++ )
      {
        x = (xx * videoBank.width) / width;
        xOSD = (xx * OSDWidth) / width;
        xpos = ((x & 0xffffffe0) << 5) + (x & 0x1f);
        yPos = ypos + xpos;
        cPos = (ypos2 + xpos) & 0xfffffffe;

        // get rgb screen value
        YUV2RGB( picAddr[yPos], chrAddr[cPos + 1], chrAddr[cPos], &r, &g, &b );

        // get osd value
        if ( bOSD )
        {
          //First check plane 1
          if ((dword) pOSD1Info != 0x80000000) //Check if the plane is in use
          {
            if (Plane1UsesLUT) color = *(_vAddr1b[yOSD] + xOSD);
            else color = *(_vAddr1w[yOSD] + xOSD);
          }
          else
          {
            color = 0;
          }
          if ( color != 0 )
          {
            // don't use transparence
            //  r = ( (color >> 10) & 0x1f ) << 3;  g = ( (color >> 5 ) & 0x1f ) << 3;  b = ( (color  ) & 0x1f ) << 3;
            // use transparence
            if (Plane1UsesLUT)
            {
              YUV2RGB2 (Plane1LUT [color].Y, Plane1LUT [color].U, Plane1LUT [color].V, &nR, &nG, &nB);
            }
            else
            {
              nR = ( (color >> 10) & 0x1f ) << 3;
              nG = ( (color >> 5 ) & 0x1f ) << 3;
              nB = ( (color  ) & 0x1f ) << 3;
            }
            nR = (n1*r + n2*nR) / 100;
            nG = (n1*g + n2*nG) / 100;
            nB = (n1*b + n2*nB) / 100;
            r = _Clip(nR);
            g = _Clip(nG);
            b = _Clip(nB);
          } else

          //Else check plane 2
          {
            if ((dword) pOSD2Info != 0x80000000) //Check if the plane is in use
            {
              if (Plane2UsesLUT) color = *(_vAddr2b[yOSD] + xOSD);
              else color = *(_vAddr2w[yOSD] + xOSD);
            }
            else
            {
              color = 0;
            }
            if ( color != 0 )
            {
              //Convert the index to a RGB color.
              if (Plane2UsesLUT)
              {
                YUV2RGB2 (Plane2LUT [color].Y, Plane2LUT [color].U, Plane2LUT [color].V, &nR, &nG, &nB);
              }
              else
              {
                nR = ( (color >> 10) & 0x1f ) << 3;
                nG = ( (color >> 5 ) & 0x1f ) << 3;
                nB = ( (color  ) & 0x1f ) << 3;
              }
              nR = (n1*r + n2*nR) / 100;
              nG = (n1*g + n2*nG) / 100;
              nB = (n1*b + n2*nB) / 100;
              r = _Clip(nR);
              g = _Clip(nG);
              b = _Clip(nB);
            }
          }
        } // end if osd

        // set pixel value in buffer bottom -> top
        p = pBuffer + 3*( ((height - yy -1)* width) + xx);
        p[0] = b;
        p[1] = g;
        p[2] = r;
      } // end for x
    } // end for y
  } // if nRet

  // free resources
  TAP_MemFree( videoBank.cAddress );
  TAP_MemFree( videoBank.yAddress );

  // all done
  return nRet;
}
