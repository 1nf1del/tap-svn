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

#include "skin.h"
#include "display.h"


// All this is from Media Manager and its gradient creation routines

// adjust the red, green and blue values according the severity of the gradient
void adjustRGB(byte prominent_colour, dword increment, word *r, word *g, word *b, word orig_r, word orig_g, word orig_b)
{
	switch (prominent_colour) {
		case RED:
			*r = orig_r < 128 ? ((*r + increment) > 255 ? 255 : *r + increment) : ((*r < increment) ? 0 : *r - increment);
			*g = orig_r < 128 ? ((*g + increment) > 255 ? 255 : *g + increment) : ((*g < increment) ? 0 : *g - increment);
			*b = orig_r < 128 ? ((*b + increment) > 255 ? 255 : *b + increment) : ((*b < increment) ? 0 : *b - increment);
			break;
		case GREEN:
			*r = orig_g < 128 ? ((*r + increment) > 255 ? 255 : *r + increment) : ((*r < increment) ? 0 : *r - increment);
			*g = orig_g < 128 ? ((*g + increment) > 255 ? 255 : *g + increment) : ((*g < increment) ? 0 : *g - increment);
			*b = orig_g < 128 ? ((*b + increment) > 255 ? 255 : *b + increment) : ((*b < increment) ? 0 : *b - increment);
			break;
		case BLUE:
			*r = orig_b < 128 ? ((*r + increment) > 255 ? 255 : *r + increment) : ((*r < increment) ? 0 : *r - increment);
			*g = orig_b < 128 ? ((*g + increment) > 255 ? 255 : *g + increment) : ((*g < increment) ? 0 : *g - increment);
			*b = orig_b < 128 ? ((*b + increment) > 255 ? 255 : *b + increment) : ((*b < increment) ? 0 : *b - increment);
			break;
	}
}

// create a gradient block where orig_r, orig_g & orig_b represent the base skin colour
void createGradient(word region, int width, int height, dword gradient_increment, dword colour )
{
	word r, g, b, y;
	
	int prominent_bg_colour;
	word orig_r, orig_g, orig_b;
	
	orig_r = extractRGBValue(RED, colour);
	orig_g = extractRGBValue(GREEN, colour);
	orig_b = extractRGBValue(BLUE, colour);	
	
	prominent_bg_colour	= getProminentColour( colour );

	r = orig_r; g = orig_g; b = orig_b;

	// save a line at a time, adjusting the colour as we go
	for (y = 0; y < height; y++) {
		TAP_Osd_FillBox(region, 0, y, width+10+1, 1, RGB8888(r,g,b));
		adjustRGB(prominent_bg_colour, gradient_increment, &r, &g, &b, orig_r, orig_g, orig_b);
	}
}

int getProminentColour( dword colour ){
	if( ((colour>>10&0x1F)*8) > ((colour>>5&0x1F)*8) &&
		((colour>>10&0x1F)*8) > ((colour>>0&0x1F)*8) ){
		return RED;
	}
	if( ((colour>>5&0x1F)*8) > ((colour>>10&0x1F)*8) &&
		((colour>>5&0x1F)*8) > ((colour>>0&0x1F)*8) ){
		return GREEN;
	}
	return BLUE;
}

word extractRGBValue( int type, dword colour ){
	switch( type ){
		case RED:
			return (colour>>10&0x1F)*8;
		case GREEN:
			return (colour>>5&0x1F)*8;
		case BLUE:
			return (colour>>0&0x1F)*8;
	}
	return 0;
}


//#define RGB8888(r,g,b)	RGB( (r>>3), (g>>3), (b>>3) )

void skinTestStartup( void ){
	char text[1024];
return;
	memset(text, 0, sizeof(text) );
	
	sprintf(text, "Main item skin colour = %d, r = %d, g = %d, b = %\0", DISPLAY_ITEM, DISPLAY_ITEM<<5, DISPLAY_ITEM<<10, DISPLAY_ITEM<<15 );
	
	Log("JimsGuide.log", "JimsGuide", FALSE, TIMESTAMP_NONE, text);
}


// Function used to load options from the saved file
void Skin_LoadFile( char * fileName ){
	TYPE_File* fp;
	char sIniFile[1024];
	char sResult[256];
	char sSection[128];
	char sBuff[256];
	memset( sIniFile, 0, sizeof sIniFile );
   	memset( sResult, 0, sizeof sResult );
	memset( sSection, 0, sizeof sSection );
	memset( sBuff, 0, sizeof sBuff );
	ChangeDirRoot();
	TAP_Hdd_ChangeDir("ProgramFiles");
	if( !TAP_Hdd_Exist("Settings") ) return;
	TAP_Hdd_ChangeDir("Settings");
	if( !TAP_Hdd_Exist("Skins") ) return;
	TAP_Hdd_ChangeDir("Skins");

	if ((fp = TAP_Hdd_Fopen (OPTIONS_FILENAME))){
		TAP_Hdd_Fread( sIniFile, sizeof( sIniFile ), 1, fp );
		TAP_Hdd_Fclose( fp );
		GetSetting( sIniFile, "[General]", "ShowTimes", sResult );
		Settings_LinearEPGTimes  = ( strcmp ( sResult, "True" ) == 0 );

		GetSetting( sIniFile, "[General]", "GridEPGExcludes", sResult );
		strcpy( Settings_ChannelExcludes, sResult );
	} else {

	}
}
