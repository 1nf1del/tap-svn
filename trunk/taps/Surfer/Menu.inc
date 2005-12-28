/************************************************************
			Part of the ukEPG project
	This is the include file for Menu.c

Name	: Menu.inc
Author	: Darkmatter
Version	: 0.1
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 31-07-05	Split from common.c
		  v0.1 Darkmatter

	Last change:  USE   9 Aug 105   11:26 pm
************************************************************/

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
