/*
Copyright (C) 2005 Robin Glover

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
#ifndef _colors_18CE3693_FC3F_4d4b_A9E0_66CD4F632AE1_h
#define _colors_18CE3693_FC3F_4d4b_A9E0_66CD4F632AE1_h

enum colorSets
{
	normalColors,
	highlightColors,
	headerColors,
	footerColors,
	scrollBarColors
};

typedef struct _ListColors
{
	word bgColor;
	word textColor;
	word frameColor;
} ListColors;

#endif