/************************************************************
	    Part of the ukEPG project
	This module displays text

Name	: TextTools.c
Author	: kidhazy
Version	: 0.2
For	: Topfield TF5x00 series PVRs
Descr.	:
Licence	:
Usage	:
History	: v0.0 kidhazy:
	  v0.1 sl8:	20-01-06	All variables initialised
	  v0.2 sl8:	11-04-06	Tidy up

	
**************************************************************/
int     WrapPutStr_StartPos = 0;  	//KH Starting position to print string from
int     LastWrapPutStr_Y = 0;			//KH Tracks the last coord from the WrapPutStr routine.
int 	LastWrapPutStr_P = 0;  			//KH Tracks the last string position from the WrapPutStr routine.
int 	LastWrapPutStr_Start = 0;       //KH Tracks the start position of the last line from the WrapPutStr routine.
int     ExtInfoRows = 1;  			//KH Keep track of how many lines available for extended info. Initially assume at least 1.;

//-------------------------------------------
//
/******************************************************************************
Converts the Parental Rating from number to textual Australian Based ratings.
******************************************************************************/
void GetOzParentalRating(int parentalRating,char *rating)
{
     // FTA: G=9  PG=11 M=13 MA=15 R=18
     // TED: G=10 PG=12 M=13 MA=15 R=18
     switch ( parentalRating )
     {
            case 9: 
            case 10: strcpy(rating, "G");  rating[1]= '\0'; break;
            case 11: 
            case 12: strcpy(rating, "PG"); rating[2]= '\0'; break;
            case 13: strcpy(rating, "M");  rating[1]= '\0'; break;
            case 15: strcpy(rating, "MA"); rating[2]= '\0'; break;
            case 18: strcpy(rating, "R");  rating[1]= '\0'; break;
            default: strcpy(rating, "");  rating[0]= '\0'; break;
     }
}
     
//------------
//
void PrintCenter( word windowRgn, dword x, dword y, dword maxX, const char *str, dword fcolor, dword bcolor, byte fntSize)
{
	dword width = 0, offset = 0;

	width = TAP_Osd_GetW( str, 0, fntSize );

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
void PrintRight( word windowRgn, dword x, dword y, dword maxX, const char *str, dword fcolor, dword bcolor, byte fntSize)
{
	dword width = 0, offset = 0;

	width = TAP_Osd_GetW( str, 0, fntSize );

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
void PrintLeft( word windowRgn, dword x, dword y, dword maxX, const char *str, dword fcolor, dword bcolor, byte fntSize)
{
	dword width = 0, offset = 0;

	width = TAP_Osd_GetW( str, 0, fntSize );

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
void WrapPutStr(word windowRgn, char *str, int x, int y, int w, int fgCol, int bgCol, int maxRows, byte fntSize, int lineSep)
{
	int row = 0, c = 0, p = 0;
	int done = 0, try = 5;
	int fontH = 0;
	int avgFontW = 0;
	int len = 0;
	int newStrW = 0;
	char newStr[512]; //100 should be more than enough for a line [!!!????]

	switch (fntSize)
	{
		case FNT_Size_1419 : fontH = 19; avgFontW = 6; break;
		case FNT_Size_1622 : fontH = 22; avgFontW = 8; break;
		case FNT_Size_1926 : fontH = 26; avgFontW = 10; break;
		default : fontH = 19; break;
	}

	len = strlen(str);
	
	p = WrapPutStr_StartPos;			//KH Set the starting position to print string from.
	if (len == 0)  
	{
		LastWrapPutStr_P = p;			//KH Keep track of the last string position.
		return;
	}

	for (row=0; row<maxRows;row++)   
	{
		LastWrapPutStr_Start = p;		//KH Keep track of the position of the first character on each line.
		c = w/avgFontW;				// guess approx chars in line
		if(p + c > len) c = len - p;
		if(c > 99) c = 99;

		strncpy(newStr,str+p, c);		// copy approx chars per line to new string
		newStr[c]= '\0';			//KH clean up the end of the new line.

		while(!done)
		{
			newStrW = TAP_Osd_GetW(newStr,0,fntSize);	// see if it fits
			if (newStrW > w && c > 0)			// Too big
			{						//remove some chars
				if(c > 4) c = c - 4;
				else c--;

				newStr[c]= '\0';
			}
			else
			{
				done = 1;				// string short enough
			}
		}

		done = 0;

		while (TAP_Osd_GetW(newStr,0,fntSize)<w-avgFontW)	//while the width of the text is less than a lines length
		{
			if (p + c + 1 > len)				//if the counter is larger than the string length of the orig string, we have really finished here 
			{
				done=1;					
				break;
			}
			strncat(newStr,str+p+c,1);			//copy 1 char from str at a time.
			c++;						//get next char
		}

		if (!done )						//KH find wrap position unless finished
		{
			c--;						//reduce c by 1 as it is one too much from last while statement.
			while (!isspace(newStr[c])&&!(newStr[c]=='-')&&(c>0))
		 	{						//look for a space or hyphen (or underflow).
				c--;
			}
			c++;						//add 1 to keep hyphen on right line
		}
		newStr[c]='\0';						//terminate str (early at wrap point if not done).

		LastWrapPutStr_Y = y+((fontH+lineSep)*row);		//KH Keep track of the Y coordinate of where we're printing.
		TAP_Osd_PutStringAf(windowRgn, x, LastWrapPutStr_Y, x+w, newStr, fgCol, bgCol, 0, fntSize, 0);  //put multiple lines down if required.
		p = p + c;   //store current position
		LastWrapPutStr_P = p;					//KH Keep track of the last string position.
		newStr[0] = '\0';					//reset string.

		if (done)						//KH if we've printed all the text, break out of loop.
		{
			LastWrapPutStr_P = 0;				//KH Reset to start of string.
			break;
		}
	}    
	LastWrapPutStr_Y = LastWrapPutStr_Y + fontH + lineSep;		//KH Store the last Y position.
	WrapPutStr_StartPos = 0;					//KH Reset starting position to print string from
}

