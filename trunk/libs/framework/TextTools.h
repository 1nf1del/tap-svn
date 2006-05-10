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

#ifndef __TEXTTOOLS_H
#define __TEXTTOOLS_H


void PrintCenter( word windowRgn, word x, word y, word maxX, const char *str, word fcolor, word bcolor, byte fntSize);
void PrintRight( word windowRgn, word x, word y, word maxX, const char *str, word fcolor, word bcolor, byte fntSize);
void PrintLeft( word windowRgn, word x, word y, word maxX, const char *str, word fcolor, word bcolor, byte fntSize);
void WrapPutStr(word windowRgn, char *str, short int x, short int y, short int w, word fgCol, word bgCol, int maxRows, byte fntSize, int lineSep);


#endif
