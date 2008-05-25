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

#ifndef __SKINHEADER__
  #define __SKINHEADER__

#define __SKINHEADER_VERSION__ "2008-03-07"

#define RED 0
#define GREEN 1
#define BLUE 2

// Add more later but determine a base and use in the timerlist and favourites list
//static byte* Skin_GradientBase = NULL;
//static byte* Skin_GradientHigh = NULL;
//static byte* Skin_GradientBaseLin = NULL;
//static byte* Skin_GradientHighLin = NULL;
//static byte* Skin_GradientBaseRight = NULL;

void adjustRGB(byte, dword, word *, word *, word *, word, word, word);
void createGradient(word region, int width, int height, dword gradient_increment, dword colour );
void skinTestStartup( void );
int getProminentColour( dword colour );
word extractRGBValue( int type, dword colour );

#endif
