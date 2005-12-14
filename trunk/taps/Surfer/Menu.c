/************************************************************
			Part of the ukEPG project
		Generic code for handling menus
		
Name	: Menu.c
Author	: Darkmatter
Version	: 0.2
For		: Topfield TF5x00 series PVRs
Descr.	:
Licence	:
Usage	:
History	: v0.0 Darkmatter:	04-07-05	Inception date
		  v0.1 Darkmatter:	30-07-05	Changed significantly to make code generic. Now supports multiple lists.
										Code specific to the main main spilt in to MainMenu.c
		  v0.2 Darkmatter

	Last change:  USE   9 Aug 105   11:25 pm
************************************************************/

#define MENU_Y		80
#define MENU_X		210
#define MENU_ROW_W	300
#define MENU_STEP	42

#include "Menu.inc"



//------------------
//
void MenuCreate( menu_Struct *menu, char *menuTitle, void (*LocalProc)(TYPE_MenuProcess, int) )
{
	menu->count = 0;
	menu->currentLine = 0;
	menu->CommandProc = LocalProc;
	menu->title = menuTitle;
}


void MenuAdd( menu_Struct *menu, char *title, char *description )
{
	menuItem_Struct	*newItem;
    
	if ( menu->count >= MAX_NUMBER_OF_MENU_ITEMS ) return;				// bounds check
    menu->count++;

	newItem = TAP_MemAlloc( sizeof( menuItem_Struct ) );				// allocate some memory to store the pointers to the strings
	newItem->title = title;												// note, the strings are not copied, they remain in the source code.
	newItem->description = description;
	menu->items[ menu->count ] = newItem;								// point to the newly allocated memory
}


void MenuSetCurrentLine( menu_Struct *menu, int lineNumber)
{
	if (lineNumber <= (menu->count)) menu->currentLine = lineNumber;
}


void MenuDestroy( menu_Struct *menu )
{
    int i;
	
	if ( menu->count == 0 ) return;										// bounds check

	for ( i=1; i <= (menu->count) ; i++ )								// for all members of the structure
	{
		TAP_MemFree( menu->items[i] );									// release memory back to the heap
	}

	menu->count = 0;													// tidy things up - set number of elements to 0
}



//------------------
//
void DisplayMenuLine( menu_Struct *menu, int lineNumber )
{
    char	str[80];
	dword	width, x_coord, exTextX, exTextY;
	int 	i;
	int 	currentMenuLine;
    menuItem_Struct	*currentItem;

	currentMenuLine = menu->currentLine;

	if ( lineNumber > menu->count ) return;								// bounds check
	if ( lineNumber == 0 ) return;										// bounds check

	if ( currentMenuLine == lineNumber )								// highlight the current cursor line
	    TAP_Osd_PutGd( rgn, MENU_X, (lineNumber * MENU_STEP) + MENU_Y - 8, &_menurow_highlightGd, FALSE );
	else
	    TAP_Osd_PutGd( rgn, MENU_X, (lineNumber * MENU_STEP) + MENU_Y - 8, &_menurowGd, FALSE );


	currentItem = menu->items[ lineNumber ];
	strcpy( str, currentItem->title );

	width = TAP_Osd_GetW( str, 0, FNT_Size_1622 );

	if ( width <= MENU_ROW_W ) x_coord = MENU_X + ((MENU_ROW_W)/2) - (width/2);	// centralise text
	else x_coord = MENU_X;														// too wide - fill width
	
																				// print the text
	TAP_Osd_PutStringAf1622( rgn, x_coord, (lineNumber * MENU_STEP) + MENU_Y, (MENU_X + MENU_ROW_W), str, MAIN_TEXT_COLOUR, 0 );

	
	exTextX = MENU_X;
	exTextY = ((MAX_NUMBER_OF_MENU_ITEMS+1) * MENU_STEP) + MENU_Y;
	
	if ( lineNumber == currentMenuLine )
	{
		strcpy( str, currentItem->description );
																				// blank the extended text area before redrawing
		TAP_Osd_FillBox( rgn, exTextX, (exTextY-8), MENU_ROW_W, MENU_STEP, FILL_COLOUR );
		TAP_Osd_PutStringAf1419( rgn, exTextX, (exTextY+15), (exTextX + MENU_ROW_W), str, INFO_COLOUR, 0 );
	}
}





void DisplayMenu( menu_Struct *menu )
{
	int i;
	word h1, v1, x1, x2, y1, y2;
	char str[256];

    TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );				// Clear the screen
    TAP_Osd_PutGd( rgn, (MENU_X - 11), MENU_Y - 8, &_menu_titleGd, TRUE );

																		// Draw boarders - calculate coords first:
	h1 = MENU_ROW_W + 16;												// horizontal width
	v1 = ((MAX_NUMBER_OF_MENU_ITEMS+1) * MENU_STEP) + 8;				// vertical width - num of items + extended text line

	x1 = MENU_X - 10;													// top right
	y1 = MENU_Y + MENU_STEP - 12;

	x2 = x1 + h1;														// bottom left
	y2 = y1 + v1;

	
    TAP_Osd_FillBox( rgn, x1, y1, 4, v1, TITLE_COLOUR );				// left boarder line
    TAP_Osd_FillBox( rgn, x2, y1, 4, v1, TITLE_COLOUR );				// right boarder line
    TAP_Osd_FillBox( rgn, x1, y2, h1+4, 4, TITLE_COLOUR );				// bottom boarder line

	TAP_Osd_PutStringAf1926( rgn, MENU_X+20, MENU_Y+2, MENU_X + 100, menu->title, TITLE_COLOUR, 0 );	// Print the title

	for ( i=1 ; i <= (menu->count) ; i++ )								// for each row on the menu
	{
		DisplayMenuLine( menu, i );										// display the text
	}
}




dword MenuKeyHandler( menu_Struct *menu, dword key )
{
	int	oldChosenLine;
    void (*LocalProc)(TYPE_MenuProcess, int);

	if (( (menu->count) == 0 ) && (key != RKEY_Exit)) return 0;				// bounds check
 
	oldChosenLine = menu->currentLine;
	LocalProc = menu->CommandProc;
	
	
	switch ( key )
	{
		case RKEY_ChDown :	if ( (menu->currentLine) < (menu->count) )
		    					(menu->currentLine)++;
							else
							    (menu->currentLine) = 1;

							DisplayMenuLine( menu, oldChosenLine );			// re-draw the old line in normal
							DisplayMenuLine( menu, (menu->currentLine) );	// draw the new selection with a highlight bar
							break;

		case RKEY_ChUp :	if ( (menu->currentLine) > 1 )
		    					(menu->currentLine)--;
							else
							    (menu->currentLine) = (menu->count);

							DisplayMenuLine( menu, oldChosenLine );			// re-draw the old line in normal
							DisplayMenuLine( menu, (menu->currentLine) );	// draw the new selection with a highlight bar
							break;

		case RKEY_Ok :		LocalProc( MENU_Ok, (menu->currentLine) );
							break;

		case RKEY_Exit :	LocalProc( MENU_Exit, 0);
						    break;
	}

	return  0;																// we're not interested in passing on the keys
}
