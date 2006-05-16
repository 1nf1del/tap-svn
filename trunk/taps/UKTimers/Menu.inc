/*
	Copyright (C) 2005-2006 Darkmatter

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	The software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this software; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

//	This is the include file for Menu.c


#ifndef __UkMenu__
#define __UkMenu__

#define MAX_NUMBER_OF_MENU_ITEMS 7


//----------
// external structure
//
typedef enum { MENU_Ok, MENU_Exit } TYPE_MenuProcess;

typedef struct
{
	char	*title;
	char	*description;
} menuItem_Struct;

typedef struct
{
	int		count;
	int		currentLine;
	void	(*CommandProc)(TYPE_MenuProcess, int);
	char	*title;
	menuItem_Struct	*items[MAX_NUMBER_OF_MENU_ITEMS+1];					// an array of pointers
} menu_Struct;



//----------
// external procedures
//
void MenuCreate( menu_Struct*, char*, void (*LocalProc)(TYPE_MenuProcess, int) );
void MenuAdd( menu_Struct*, char*, char* );
void MenuSetCurrentLine( menu_Struct*, int);
void MenuDestroy( menu_Struct* );
void DisplayMenu( menu_Struct* );
dword MenuKeyHandler( menu_Struct*, dword );


#endif
