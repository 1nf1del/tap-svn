/*
	Copyright (C) 2005 kidhazy

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <tap.h>
#include <string.h>
#include <ctype.h>

int     WrapPutStr_StartPos = 0;  	// Starting position to print string from
int     LastWrapPutStr_Y;			// Tracks the last coord from the WrapPutStr routine.
int 	LastWrapPutStr_P;  			// Tracks the last string position from the WrapPutStr routine.
int 	LastWrapPutStr_Start;       // Tracks the start position of the last line from the WrapPutStr routine.
int     ExtInfoRows = 1;  			// Keep track of how many lines available for extended info. Initially assume at least 1.;

//------------
//
void PrintCenter( word windowRgn, word x, word y, word maxX, const char *str, word fcolor, word bcolor, byte fntSize)
{
	word width, offset;

	width = (word)TAP_Osd_GetW( str, 0, fntSize );

	if ( width <= (maxX-x) ) offset = (maxX - x - width)/2;				// centralise text
	else offset = 5;													// too wide - fill width

	switch ( fntSize )
	{
		case FNT_Size_1419 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 21, bcolor );
							 TAP_Osd_PutStringAf1419( windowRgn, x+offset, y, maxX, str, fcolor, bcolor);
							 break;
							
		case FNT_Size_1622 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 24, bcolor );
							 TAP_Osd_PutStringAf1622( windowRgn, x+offset, y, maxX, str, fcolor, bcolor);
							 break;
							 
		case FNT_Size_1926 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 28, bcolor );
							 TAP_Osd_PutStringAf1926( windowRgn, x+offset, y, maxX, str, fcolor, bcolor);
							 break;
							 
		default : break;
	}

	
}

//------------
//
void PrintRight( word windowRgn, word x, word y, word maxX, const char *str, word fcolor, word bcolor, byte fntSize)
{
	word width, offset;

	width = (word)TAP_Osd_GetW( str, 0, fntSize );

	if ( width <= (maxX-x) ) offset = (maxX - width);				// right justify text
	else offset = x;												// too wide - fill width

	switch ( fntSize )
	{
		case FNT_Size_1419 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 21, bcolor );
	                         TAP_Osd_PutS(windowRgn, x, y, maxX, str, fcolor, bcolor, 0, fntSize, FALSE, ALIGN_RIGHT);
							 break;
							 
							
		case FNT_Size_1622 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 24, bcolor );
	                         TAP_Osd_PutS(windowRgn, x, y, maxX, str, fcolor, bcolor, 0, fntSize, FALSE, ALIGN_RIGHT);
							 break;
							 
		case FNT_Size_1926 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 28, bcolor );
	                         TAP_Osd_PutS(windowRgn, x, y, maxX, str, fcolor, bcolor, 0, fntSize, FALSE, ALIGN_RIGHT);
							 break;
							 
		default : break;
	}
	
}

//------------
//
void PrintLeft( word windowRgn, word x, word y, word maxX, const char *str, word fcolor, word bcolor, byte fntSize)
{
	word width, offset;

	width = (word) TAP_Osd_GetW( str, 0, fntSize );

	if ( width <= (maxX-x) ) offset = (maxX - width);				// right justify text
	else offset = x;												// too wide - fill width

	switch ( fntSize )
	{
		case FNT_Size_1419 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 21, bcolor );
	                         TAP_Osd_PutS(windowRgn, x, y, maxX, str, fcolor, bcolor, 0, fntSize, FALSE, ALIGN_LEFT);
							 break;
							 
							
		case FNT_Size_1622 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 24, bcolor );
	                         TAP_Osd_PutS(windowRgn, x, y, maxX, str, fcolor, bcolor, 0, fntSize, FALSE, ALIGN_LEFT);
							 break;
							 
		case FNT_Size_1926 : if ( bcolor > 0 ) TAP_Osd_FillBox( windowRgn, x, y-1, maxX-x, 28, bcolor );
	                         TAP_Osd_PutS(windowRgn, x, y, maxX, str, fcolor, bcolor, 0, fntSize, FALSE, ALIGN_LEFT);
							 break;
							 
		default : break;
	}
	
}

/******************************************************************************
Word Wrap string output.

str: The string to be printed.
x, y, w: print area starts at x pixels in, y pixels down and is w pixels wide.
fgCol, bgCol: foreground and background colours.
maxRows: defines the hight of the print area in rows of characters.
fntSize: one of FNT_Size_1419, FNT_Size_1622 and FNT_Size_1926.
lineSep: The extra pixels between rows (can be negative with bgCol = COLOR_None
for overprinting).

Control characters (cariage return, new line etc.) are not supported.
******************************************************************************/
void WrapPutStr(word windowRgn, char *str, short int x, short int y, short int w, word fgCol, word bgCol, int maxRows, byte fntSize, int lineSep)
{
	int row=0,c=0,p=0;
	int done = 0;
	int fontH;
	int avgFontW;
	int len;
	int newStrW;
	char newStr[512]; //100 should be more than enough for a line [!!!????]

	switch (fntSize)
	{
		case FNT_Size_1419 : fontH = 19; avgFontW = 6; break;
		case FNT_Size_1622 : fontH = 22; avgFontW = 8; break;
		case FNT_Size_1926 : fontH = 26; avgFontW = 10; break;
		default : fontH = 19; avgFontW=6; break;
	}

	len = strlen(str);
	if (len == 0)  return;
	
	p = WrapPutStr_StartPos;  // Set the starting position to print string from.

	for (row=0; row<maxRows;row++)   
	{
    	LastWrapPutStr_Start = p;  // Keep track of the position of the first character on each line.
		c = w/avgFontW;	// guess approx chars in line
		if(p + c > len) c = len - p;
		if(c > 99) c = 99;

		strncpy(newStr,str+p, c); // copy approx chars per line to new string
		newStr[c]= '\0'; // clean up the end of the new line.

		while(!done)
		{
			newStrW = TAP_Osd_GetW(newStr,0,fntSize); // see if it fits
			if (newStrW > w && c > 0)  // Too big
			{		 //remove some chars
				if(c > 4) c = c - 4;
				else c--;

				newStr[c]= '\0';
			}
			else
			{
				done = 1; // string short enough
			}
			
		}

		done = 0;

		while (TAP_Osd_GetW(newStr,0,fntSize)<w-avgFontW)    //while the width of the text is less than a lines length
		{
			if (p + c + 1 > len)   //if the counter is larger than the string length of the orig string, we have really finished here 
			{
				done=1;					
				break;
			}
			strncat(newStr,str+p+c,1); //copy 1 char from str at a time.
			c++;                 //get next char
		}


		if (!done ) // find wrap position unless finished
		{
			c--; //reduce c by 1 as it is one too much from last while statement.
			while (!isspace(newStr[c])&&!(newStr[c]=='-')&&(c>0))
		 	{   //look for a space or hyphen (or underflow).
				c--;
			}
			c++; //add 1 to keep hyphen on right line
		}
		newStr[c]='\0';   //terminate str (early at wrap point if not done).

		LastWrapPutStr_Y = y+((fontH+lineSep)*row); // Keep track of the Y coordinate of where we're printing.
		TAP_Osd_PutStringAf(windowRgn, x, LastWrapPutStr_Y, x+w, newStr, fgCol, bgCol, 0, fntSize, 0);  //put multiple lines down if required.
		p = p + c;   //store current position
		LastWrapPutStr_P = p; // Keep track of the last string position.
		newStr[0] = '\0';  //reset string.
    	if (done) // if we've printed all the text, break out of loop.
        {
            LastWrapPutStr_P = 0;  // Reset to start of string.
            break;
        }
	}    
	LastWrapPutStr_Y = LastWrapPutStr_Y + fontH + lineSep;  // Store the last Y position.
	WrapPutStr_StartPos = 0;  // Reset starting position to print string from
}

