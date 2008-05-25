//
//	  MultiViewEPG jim16 Tue 1 Apr 12:12:00
//
//	  Copyright (C) 2008 Jim16
//
//	  This is free software; you can redistribute it and/or modify
//	  it under the terms of the GNU General Public License as published by
//	  the Free Software Foundation; either version 2 of the License, or
//	  (at your option) any later version.
//
//	  The software is distributed in the hope that it will be useful,
//	  but WITHOUT ANY WARRANTY; without even the implied warranty of
//	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//	  GNU General Public License for more details.
//
//	  You should have received a copy of the GNU General Public License
//	  along with this software; if not, write to the Free Software
//	  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

/*
GfxLIB - A utility library to assist in doing graphics on the OSD on Topfield 5000.
Author : Kjetil Naess, July 2005
Version: 1.0

You may copy and/or use GfxLIB in either source or compiled form as you wish. Any suggestions and/or
improvements are welcome so we can make this a good public resource.
*/

#include "tap.h"
#include "gfxlib.h"

word *_vAddr[576];
//#include "bitmapfont2.h"

byte currentROP = blt_SRCCOPY;

void setupGraphics()
{
   int i;
   word *ep, *op;

   TYPE_OsdBaseInfo osdBaseInfo;
   TAP_Osd_GetBaseInfo( &osdBaseInfo );

   ep = osdBaseInfo.eAddr;
   op = osdBaseInfo.oAddr ;
  
   // set vertical address
   for( i=0; i<576; i += 2 )
   {
       _vAddr[i] = ep; ep+= 720;
       _vAddr[i+1] = op; op+= 720;
    }
}

/*void setupGraphics()
{
   int i;

   TYPE_OsdBaseInfo osdBaseInfo;
   TAP_Osd_GetBaseInfo( &osdBaseInfo );

   // set vertical address
   for( i=0; i<VideoResY; i+=2 )
   {
       _vAddr[i] = (word *) osdBaseInfo.eAddr + 720*i;
       _vAddr[i+1] = (word *) osdBaseInfo.oAddr + 720*i;
   }

      
   setStockLUT(2);
}*/

void endGraphics()
{
}
  
void setPixel(word region, int x, int y, word color)
{
	if (x <= xLimit && y <= yLimit && (x >= 0 && y >= 0))   
	{
//		if (masterpiece_mode == 2) {
//			byte bPixel = color;
		
			// Playing games alters the offset of the default region
			/*if (region == 0) {
				iCol += uGbl.iGameCol;
				iRow += uGbl.iGameRow;
			}*/

//			TAP_Osd_PutPixel(region, x, y, bPixel);
//		}
//		else {
			*(_vAddr[y]+x) = color;
			//TAP_Osd_PutPixel(region, x, y, color);
//		}
	}
}

word getPixel(word region, int x, int y)
{
//    byte   bLocalPixel;
//    word   wLocalPixel;

//	if (masterpiece_mode == 2) {
//	    TAP_Osd_GetPixel(region, x, y, &bLocalPixel);
// 	    return (word) bLocalPixel;
//	}
//	else {
		return *( _vAddr[y]+x);
		//TAP_Osd_GetPixel(region, x, y, &wLocalPixel);
        //return wLocalPixel; 
//	}
}

//Bresenham circle with center at (xc,yc) with radius and red green blue color
bool drawCircle(word region, int xc, int yc, int radius, word color)
{
    int x = 0;
    int y = radius;
    int p = 3 - (radius << 1);
    int a, b, c, d, e, f, g, h;
    if(xc - radius < 0 || xc + radius >= VideoResX || yc - radius < 0 || yc + radius >= VideoResY) return 0;
    while (x <= y)
    {
         a = xc + x; //8 pixels can be calculated at once thanks to the symmetry
         b = yc + y;
         c = xc - x;
         d = yc - y;
         e = xc + y;
         f = yc + x;
         g = xc - y;
         h = yc - x;
         setPixel(region, a, b, color);
         setPixel(region, c, d, color);
         setPixel(region, e, f, color);
         setPixel(region, g, f, color);
         if(x>0) //avoid drawing pixels at same position as the other ones
         {
             setPixel(region, a, d, color);
             setPixel(region, c, b, color);
             setPixel(region, e, h, color);
             setPixel(region, g, h, color);
         }
         if(p < 0) p += (x++ << 2) + 6;
         else p += ((x++ - y--) << 2) + 10;
  }
  
  return 1;
}

//Filled bresenham circle with center at (xc,yc) with radius and red green blue color
void drawDisk(word region, int xc, int yc, int radius, word color)
{
    int x = 0;
    int y = radius;
    int p = 3 - (radius << 1);
    int a, b, c, d, e, f, g, h;
    int pb = 0;
	int pd = 0; //previous values: to avoid drawing horizontal lines multiple times    

	if(xc + radius < 0 || xc - radius >= VideoResX || yc + radius < 0 || yc - radius >= VideoResY) return ; //every single pixel outside screen, so don't waste time on it
    while (x <= y)
    {
         // write data
         a = xc + x;
         b = yc + y;
         c = xc - x;
         d = yc - y;
         e = xc + y;
         f = yc + x;
         g = xc - y;
         h = yc - x;
         if(b != pb) drawLine(region, a, b, c, b, color);
         if(d != pd) drawLine(region, a, d, c, d, color);
         if(f != b)  drawLine(region, e, f, g, f, color);
         if(h != d && h != f) drawLine(region, e, h, g, h,color);
         pb = b;
         pd = d;         
         if(p < 0) p += (x++ << 2) + 6;
         else p += ((x++ - y--) << 2) + 10;
  }
}

void drawLine(word region, int x1, int y1, int x2, int y2, word color)
{
int  x, y;
 int  dx, dy;
 int  incx, incy;
 int  balance;
 
 if (x2 >= x1)
 {
    dx = x2 - x1;
    incx = 1;
 }
 else
 {
    dx = x1 - x2;
    incx = -1;
 }

 if (y2 >= y1)
 {
    dy = y2 - y1;
    incy = 1;
 }
 else
 {
    dy = y1 - y2;
    incy = -1;
 }

 x = x1;
 y = y1;

 if (dx >= dy)
 {
  dy <<= 1;
  balance = dy - dx;
  dx <<= 1;

  while (x != x2)
  {
   setPixel(region, x, y,color);
   if (balance >= 0)
   {
    y += incy;
    balance -= dx;
   }
   balance += dy;
   x += incx;
  } setPixel(region, x, y,color);
 }
 else
 {
  dx <<= 1;
  balance = dx - dy;
  dy <<= 1;

  while (y != y2)
  {
   setPixel(region, x, y,color);
   if (balance >= 0)
   {
    x += incx;
    balance -= dy;
   }
   balance += dx;
   y += incy;
  } setPixel(region, x, y,color);
 }
}

void DuffsDevice(register unsigned char *to, register unsigned char *from, register unsigned int count)
{
   register unsigned int n = (count + 7) / 8;
   switch(count%8)
   {
   case 0: do
           {
           *to++ = *from++;
   case 7: *to++ = *from++;
   case 6: *to++ = *from++;
   case 5: *to++ = *from++;
   case 4: *to++ = *from++;
   case 3: *to++ = *from++;
   case 2: *to++ = *from++;
   case 1: *to++ = *from++;
           } while (--n > 0);
   }  
}
void DuffsDeviceA(register unsigned char *to, register unsigned char *from, register unsigned int count)
{
   register unsigned int n = (count + 7) / 8;
   switch(count%8)
   {
   case 0: do
           {
           *to++ &= *from++;
   case 7: *to++ &= *from++;
   case 6: *to++ &= *from++;
   case 5: *to++ &= *from++;
   case 4: *to++ &= *from++;
   case 3: *to++ &= *from++;
   case 2: *to++ &= *from++;
   case 1: *to++ &= *from++;
           } while (--n > 0);
   }  
}
void DuffsDeviceX(register unsigned char *to, register unsigned char *from, register unsigned int count)
{
   register unsigned int n = (count + 7) / 8;
   switch(count%8)
   {
   case 0: do
           {
           *to++ ^= *from++;
   case 7: *to++ ^= *from++;
   case 6: *to++ ^= *from++;
   case 5: *to++ ^= *from++;
   case 4: *to++ ^= *from++;
   case 3: *to++ ^= *from++;
   case 2: *to++ ^= *from++;
   case 1: *to++ ^= *from++;
           } while (--n > 0);
   }  
}
void DuffsDeviceO(register unsigned char *to, register unsigned char *from, register unsigned int count)
{
   register unsigned int n = (count + 7) / 8;
   switch(count%8)
   {
   case 0: do
           {
           *to++ |= *from++;
   case 7: *to++ |= *from++;
   case 6: *to++ |= *from++;
   case 5: *to++ |= *from++;
   case 4: *to++ |= *from++;
   case 3: *to++ |= *from++;
   case 2: *to++ |= *from++;
   case 1: *to++ |= *from++;
           } while (--n > 0);
   }  
}

void DuffsDeviceF(register unsigned char *to, register unsigned char *from, register unsigned int count)
{
   register unsigned int n = (count + 7) / 8;
   switch(count%8)
   {
   case 0: do
           {
           *to++ = *from;
   case 7: *to++ = *from;
   case 6: *to++ = *from;
   case 5: *to++ = *from;
   case 4: *to++ = *from;
   case 3: *to++ = *from;
   case 2: *to++ = *from;
   case 1: *to++ = *from;
           } while (--n > 0);
   }  
}

void drawHorizLine(word region, int x1, int y, int w, word color)
{
   int i;
   for (i = x1; i < x1+w; i++)
   {
       setPixel(region, i, y, color);
   }
}

void drawArea(word region, int x1, int y1, int w, int h, word color)
{
   int i;
   for (i = y1; i < y1+h; i++)
	   drawHorizLine(region, x1, i, w, color);
}

void drawRectangle(word region, int x1, int y1, int x2, int y2, word color, byte Filled)
{
   if (Filled == FALSE)
   {
      drawLine(region, x1, y1, x2, y1, color);
	  drawLine(region, x1, y1, x1, y2, color);
	  drawLine(region, x1, y2, x2, y2, color);
	  drawLine(region, x2, y2, x2, y1, color);
   }
   else
   {
      drawArea(region, x1, y1, x2-x1, y2-y1, color);
   }
}

/*void bitBlt(int x, int y, int w, int h, word *Data, unsigned char  Direction, unsigned char Operation)
{
   int i;
   int width=sizeof(word) * w;
   unsigned char *Destination;
   unsigned char *Source;
   for (i =0; i < h; i++)
   {
	   if (Direction == blt_ToScreen)
	   {
	      Destination = (unsigned char *)(_vAddr[i+y]+x);
		  Source = (unsigned char *)Data + (i * width);
	   }
	   else
	   {
	      Source = (unsigned char *)(_vAddr[i+y]+x);
		  Destination = (unsigned char *)Data + (i * width);
	   }
	   switch(Operation)
	   {
	   case blt_SRCCOPY : DuffsDevice((unsigned char *)Destination, (unsigned char *)Source, width); break;
	   case blt_SRCAND  : DuffsDeviceA((unsigned char *)Destination, (unsigned char *)Source, width); break;
	   case blt_SRCINVERT:DuffsDeviceX((unsigned char *)Destination, (unsigned char *)Source, width); break;
	   case blt_SRCPAINT: DuffsDeviceO((unsigned char *)Destination, (unsigned char *)Source, width); break;
	   }
   }
}*/

/*void drawStringCentre(word region, int x1, int y1, int w, char *s)
{
   int xo, xc;

   xc = (w / 2) + x1;
   xo = (strlen(s) * 16) / 2;

   drawString(region, xc - xo,y1,s);
}*/

/*byte setROP(byte ROP)
{
   byte b = currentROP;
   currentROP = ROP;
   return b;
}*/

/*
 * This function will draw a quadrant of a circle. By using the Bresenham algorithm which
 * devides a circle into octets, this is easily achived.
 * quadrant=1 UpperLeft
 * quadrant=2 UpperRight
 * quadrant=3 LowerLeft
 * quadrant=4 LowerRight
 */
void gxCircleQuadrant(word region, int xc, int yc, int radius, word color, int quadrant, byte Filled)
{
    int x = 0;
    int y = radius;
    int p = 3 - (radius << 1);
    int a, b, c, d, e, f, g, h;
    
	while (x <= y)
    {
         a = xc + x; //8 pixels can be calculated at once thanks to the symmetry
         b = yc + y;
         c = xc - x;
         d = yc - y;
         e = xc + y;
         f = yc + x;
         g = xc - y;
         h = yc - x;
         if (quadrant == 4) 
		 {
			 if (Filled)
				 drawLine(region, a, b, xc, b, color);
		     else
				 setPixel(region, a, b, color);
		 }
         if (quadrant == 1) 
		 {
			 if (Filled)
				 drawLine(region, c, d, xc,d,color);
			 else
				setPixel(region, c, d, color);
		 }
		 if (quadrant == 4) 
		 {
			 if (Filled)
				 drawLine(region, e, f, xc, f, color);
		     else
				 setPixel(region, e, f, color);
		 }
         if (quadrant == 3) 
		 {
			 if (Filled)
				 drawLine(region, g, f, xc, f, color);
		     else
				 setPixel(region, g, f, color);
		 }
         if(x>0) //avoid drawing pixels at same position as the other ones
         {
             if (quadrant == 2) 
			 {
				 if (Filled)
					 drawLine(region, a, d, xc, d, color);
			     else
					 setPixel(region, a, d, color);
			 }
             if (quadrant == 3) 
			 {
				 if (Filled)
					 drawLine(region, c, b, xc, b, color);
			     else
					 setPixel(region, c, b, color);
			 }
             if (quadrant == 2) 
			 {
				 if (Filled)
					 drawLine(region, e, h, xc, h, color);
			     else
					 setPixel(region, e, h, color);
			 }
             if (quadrant == 1) 
			 {
				 if (Filled)
					drawLine(region, g, h, xc,h, color);
			     else
   				    setPixel(region, g, h, color);
			 }
         }

		 if (p < 0) 
            p += (x++ << 2) + 6;
         else 
            p += ((x++ - y--) << 2) + 10;
	}

	/*while (x <= y)
    {
         a = xc + x; //8 pixels can be calculated at once thanks to the symmetry
         b = yc + y;
         c = xc - x;
         d = yc - y;
         e = xc + y;
         f = yc + x;
         g = xc - y;
         h = yc - x;
         if (quadrant == 4)  setPixel(a, b, color);
         if (quadrant == 1) setPixel(c, d, color);
         if (quadrant == 4) setPixel(e, f, color);
         if (quadrant == 3) setPixel(g, f, color);
         if(x>0) //avoid drawing pixels at same position as the other ones
         {
             if (quadrant == 2) setPixel(a, d, color);
             if (quadrant == 3) setPixel(c, b, color);
             if (quadrant == 2) setPixel(e, h, color);
             if (quadrant == 1) setPixel(g, h, color);
         }
         if (p < 0) 
            p += (x++ << 2) + 6;
         else 
            p += ((x++ - y--) << 2) + 10;
  }*/
}

#define stackSize 16384
static long stack[stackSize];
static long stackPointer = 0;

#define maxheight  600
#define maxwidth 800

byte pop(int *x, int *y) 
{ 
    if(stackPointer > 0) 
    { 
        int p = stack[stackPointer]; 
        *x = p / maxheight; 
        *y = p % maxheight; 
        stackPointer--; 
        return TRUE; 
    }     
    else 
    { 
        return FALSE; 
    }    
}    

byte push(int x, int y) 
{ 
    if(stackPointer < stackSize - 1) 
    { 
        stackPointer++; 
        stack[stackPointer] = maxheight * x + y; 
        return TRUE; 
    }     
    else 
    { 
        return FALSE; 
    }    
}     

void emptyStack() 
{ 
    int x, y; 
    while(pop(&x, &y)); 
}
/*
 * This implements a stackbased scanline floodfill. newColor is the color to check against, ie.
 * the boundries. oldColor is the color to be replaced. So, for a red shape on an empty
 * background call gxFloodFill(x, y, COLOR_Red, COLOR_None);
 */
void gxFloodFill(word region, int x, int y, word newColor, word oldColor)
{
    int y1; 
    byte spanLeft, spanRight;

	if(oldColor == newColor) return;
    emptyStack();
    
    
    if(!push(x, y)) return;
    
    while(pop(&x, &y))
    {    
        y1 = y;
        while (getPixel(region, x, y1) == oldColor && y1 >= 0) 
			   y1--;
        y1++;
        spanLeft = spanRight = 0;
        while(getPixel(region, x, y1) == oldColor && y1 < maxheight)
        {
            setPixel(region, x, y1, newColor);
            if(!spanLeft && x > 0 && getPixel(region, x - 1, y1) == oldColor) 
            {
                if(!push(x - 1, y1)) return;
                spanLeft = 1;
            }
            else if(spanLeft && x > 0 && getPixel(region, x - 1, y1) != oldColor)
            {
                spanLeft = 0;
            }
            if(!spanRight && x < maxwidth - 1 && getPixel(region, x + 1, y1) == oldColor) 
            {
                if(!push(x + 1, y1)) return;
                spanRight = 1;
            }
            else if(spanRight && x < maxwidth - 1 && getPixel(region, x + 1, y1) != oldColor)
            {
                spanRight = 0;
            } 
            y1++;                   
        }
    }        
}

/*
 * Draws a rectangle with rounded edges. (x1,x2)-(x2-y2) is the extend of the rectangle,
 * r is the radius to draw the rounded corners with, color is color and if Filled==TRUE
 * it is drawn filled 
 */
void gxRoundRectangle(word region, int x1, int y1, int x2, int y2, word color, byte Filled, int r)
{
//   int X1, X2, X3, Y1, Y2, Y3;

   drawLine(region, x1,y1+r,x1,y2-r,color);
   drawLine(region, x1+r,y1,x2-r,y1,color);
   drawLine(region, x2,y1+r,x2,y2-r,color);
   drawLine(region, x1+r,y2,x2-r,y2,color);

   gxCircleQuadrant(region, x1+r,y1+r,r,color, 1, FALSE); // Upper left corner
   gxCircleQuadrant(region, x2-r,y1+r,r,color,2, FALSE);  // Upper right corner
   gxCircleQuadrant(region, x1+r,y2-r,r,color,3, FALSE);  // Lower left corner
   gxCircleQuadrant(region, x2-r,y2-r,r,color,4, FALSE);  // Lower right corner
  
   if (Filled) {
      //gxFloodFill(x1+r,y1+r, color, COLOR_None);
	    drawArea(region, x1,y1+r,x2-x1,(y2-y1)-(r<<1),color); // Draw center block as filled area
		drawArea(region, x1+r,y1,(x2-x1)-(r<<1),r, color);    // Top rectangle
		drawArea(region, x1+r,y2-r,(x2-x1)-(r<<1),r, color);  // Bottom rectangle
   }
}
