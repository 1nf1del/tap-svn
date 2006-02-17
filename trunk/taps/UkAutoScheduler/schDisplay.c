/************************************************************
Part of the ukEPG project
This module displays the schedules

v0.0 sl8:	20-11-05	Inception date
v0.1 sl8:	29-11-05	Will not highlight a line when number of schedules equal zero.
v0.2 sl8:	20-01-06	Modified for TAP_SDK. All variables initialised
v0.3 sl8:	06-02-06	Recall key no longer allowed to exit display screen
v0.4 sl8:	06-02-06	Bug fix - Prevent corruption of the searchlist if the right key is pressed when no line highlighted
v0.5 sl8:	15-02-06	Modified for new 'Perform Search' config option.
				Bug fix - No longer possible to edit a deleted search.

**************************************************************/
// mods/features required
//
// must deal with timers firing in the background - timer list can change at any point!!!
//

#define MAX_TIMERS 255
#define MAX_START_TIME 0xffffffff
#define DEFAULT_DAY 7
#define NUMBER_OF_LINES 10
#define SCH_DISP_Y1_OFFSET (36 + 23)
#define Y1_STEP 42					// was 44

#define START_X		53
#define DIVIDER_X1	85
#define DIVIDER_X2	478
#define DIVIDER_X3	541
#define DIVIDER_X4	604
#define END_X		667	//614

#define ALL_OFFSET	23
#define SET_OFFSET	20

void schDisplayShowLegend(void);

static TYPE_Window Win;


static bool schDisplayWindowShowing = 0;
static bool schDisplaySaveToFile = 0;
static int order[MAX_TIMERS];
static char chosenDay = 0;
static int chosenLine = 0;
static int page = 0;
static int maxShown = 0;
static int timerCount = 0;


static byte rowSelection = 0;			// test code


//------------
//
void DrawGraphicBoarders(void)
{
	TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );		// clear the screen
	TAP_Osd_PutGd( rgn, 0, 0, &_topGd, TRUE );			// draw top graphics
	TAP_Osd_PutGd( rgn, 0, 0, &_sideGd, TRUE );			// draw left side graphics
	TAP_Osd_PutGd( rgn, 672, 0, &_sideGd, TRUE );			// draw right side graphics
}


	
//------------
//
void CreateSearchWindow(void)
{
	byte	currentWeekDay = 0, currentMonth = 0, currentDay = 0;
	word	currentYear = 0;
	char	str[80];

	schDisplayWindowShowing = TRUE;
	DrawGraphicBoarders();

	TAP_ExtractMjd( schTimeMjd, &currentYear, &currentMonth, &currentDay, &currentWeekDay) ;
	
	chosenDay = currentWeekDay;											// default: show only today's timers
	chosenLine = 0;														// default: highlight the 1st timer
	page = 0;
}


void CloseTimerWindow( void )
{
	schDisplayWindowShowing = FALSE;
}



//------------
//
void DrawBackground(void)
{
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "Auto Schedule", TITLE_COLOUR, COLOR_Black );

	PrintCenter(rgn, START_X, 71, DIVIDER_X1, "No.", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, DIVIDER_X1, 71, DIVIDER_X2, "Search Term", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, DIVIDER_X2, 71, DIVIDER_X3, "Channel", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, DIVIDER_X3, 71, DIVIDER_X4, "Day", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, DIVIDER_X4, 71, END_X, "Time", TITLE_COLOUR, 0, FNT_Size_1419 );
}



//------------
//
void DisplaySearchText(int line, int dispLine)
{
	char	str[132];
	int	textColour = 0;

	if ( line == 0 ) return;											// bounds check

// Number
	memset(str,0,132);
	TAP_SPrint(str,"%d", line);

	PrintCenter(rgn, START_X, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, DIVIDER_X1, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	memset(str,0,132);
	TAP_SPrint(str,"\"%s\"", schUserData[line-1].searchTerm);

	if(schUserData[line-1].searchStatus == SCH_USER_DATA_STATUS_DISABLED)
	{
		if( chosenLine == line )
		{
			textColour = COLOR_DarkGreen;
		}
		else
		{
			textColour = COLOR_DarkBlue;
		}

		TAP_Osd_PutStringAf1622( rgn, 130, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, 370, str, textColour, 0 );
	}
	else
	{

	// Record/Watch Icon
		if(schUserData[line-1].searchStatus == SCH_USER_DATA_STATUS_RECORD)
		{
			TAP_Osd_PutGd( rgn, 93, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_redcircleGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, 102, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, DIVIDER_X2, "R", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutGd( rgn, 93, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_greencircleGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, 100, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, DIVIDER_X2, "W", MAIN_TEXT_COLOUR, 0 );
		}

	// Search term
		TAP_Osd_PutStringAf1622( rgn, 130, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, 370, str, MAIN_TEXT_COLOUR, 0 );

	// Channel
		if((schUserData[line-1].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
		{
			TAP_Osd_PutGd( rgn, DIVIDER_X2 + 7, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_greenovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, DIVIDER_X2 + ALL_OFFSET, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, 600, "All", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutGd( rgn, DIVIDER_X2 + 7, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_redovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, DIVIDER_X2 + SET_OFFSET, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, 600, "Set", MAIN_TEXT_COLOUR, 0 );
		}

	// Days
		if(schUserData[line-1].searchDay == 0x7F)
		{
			TAP_Osd_PutGd( rgn, DIVIDER_X3 + 7, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_greenovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, DIVIDER_X3 + ALL_OFFSET, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, 600, "All", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutGd( rgn, DIVIDER_X3 + 7, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_redovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, DIVIDER_X3 + SET_OFFSET, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, 600, "Set", MAIN_TEXT_COLOUR, 0 );
		}

	// Time
		if(schUserData[line-1].searchStartTime == schUserData[line-1].searchEndTime)
		{
			TAP_Osd_PutGd( rgn, DIVIDER_X4 + 7, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_greenovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, DIVIDER_X4 + ALL_OFFSET, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, END_X, "All", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutGd( rgn, DIVIDER_X4 + 7, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_redovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, DIVIDER_X4 + SET_OFFSET, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET, END_X, "Set", MAIN_TEXT_COLOUR, 0 );
		}
	}
}



//------------
//
void DisplaySearchLine(int line)
{
	int dispLine = 0;

	if ( line == 0 ) return;											// bounds check
	
	dispLine = ((line-1) % NUMBER_OF_LINES) + 1;				// calculate position on page

	if ( chosenLine == line )											// highlight the current cursor line
	{
		TAP_Osd_PutGd( rgn, 53, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_highlightGd, FALSE );
	}
	else
	{
		TAP_Osd_PutGd( rgn, 53, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_rowaGd, FALSE );
	}

	if
	(
		(schMainTotalValidSearches > 0)
		&&
		(line <= schMainTotalValidSearches)
	)
	{
		DisplaySearchText(line, dispLine);						// anything to display ?
	}

	TAP_Osd_FillBox( rgn, DIVIDER_X1, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, 3, Y1_STEP, FILL_COLOUR );	// draw the column seperators
	TAP_Osd_FillBox( rgn, DIVIDER_X2, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, 3, Y1_STEP, FILL_COLOUR );	// temporary code - will eventually place in graphics
	TAP_Osd_FillBox( rgn, DIVIDER_X3, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, 3, Y1_STEP, FILL_COLOUR );	// once positions fixed.
	TAP_Osd_FillBox( rgn, DIVIDER_X4, (dispLine * Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, 3, Y1_STEP, FILL_COLOUR );
}



//------------
//
void DrawSearchList(void)
{
	int i = 0, start = 0, end = 0;

	DrawBackground();

	start = (page * NUMBER_OF_LINES)+1;

	for ( i=start; i<start+NUMBER_OF_LINES ; i++)
	{
		DisplaySearchLine(i);
	}
}




//------------
//
void UpdateListClock(void)
{
	char	str[80], str2[20], str3[20];
	byte	month = 0, day = 0, weekDay = 0;
	word 	year = 0;
	dword	width = 0;

	TAP_ExtractMjd( schTimeMjd, &year, &month, &day, &weekDay) ;

//	weekDay=2; month=8; day=28; hour=12; min=58;		// use for testing width
	
	switch ( weekDay )
	{
		case 0:	TAP_SPrint(str2,"Mon"); break;
		case 1:	TAP_SPrint(str2,"Tue"); break;
		case 2:	TAP_SPrint(str2,"Wed"); break;
		case 3:	TAP_SPrint(str2,"Thu"); break;
		case 4:	TAP_SPrint(str2,"Fri"); break;
		case 5:	TAP_SPrint(str2,"Sat"); break;
		case 6:	TAP_SPrint(str2,"Sun"); break;
		default : TAP_SPrint(str2,"BAD"); break;
	}

	switch ( month )
	{
		case 1:	TAP_SPrint(str3,"Jan"); break;
		case 2:	TAP_SPrint(str3,"Feb"); break;
		case 3:	TAP_SPrint(str3,"Mar"); break;
		case 4:	TAP_SPrint(str3,"Apr"); break;
		case 5:	TAP_SPrint(str3,"May"); break;
		case 6:	TAP_SPrint(str3,"Jun"); break;
		case 7:	TAP_SPrint(str3,"Jul"); break;
		case 8:	TAP_SPrint(str3,"Aug"); break;
		case 9:	TAP_SPrint(str3,"Sep"); break;
		case 10:TAP_SPrint(str3,"Oct"); break;
		case 11:TAP_SPrint(str3,"Nov"); break;
		case 12:TAP_SPrint(str3,"Dec"); break;
		default : TAP_SPrint(str3,"BAD"); break;
	}
	
	TAP_SPrint( str, "%s %d %s %02d:%02d", str2, day, str3, schTimeHour, schTimeMin);
	width = TAP_Osd_GetW( str, 0, FNT_Size_1622 );

	TAP_Osd_PutGd( rgn, 494, 34, &_timebarGd, TRUE );
	TAP_Osd_PutStringAf1622( rgn, 581-(width/2), 39, 660, str, TIME_COLOUR, 0 );
}



//------------
//
void schDisplayKeyHandler(dword key)
{
	int	oldPage = 0, oldChosenLine = 0;
	struct	schDataTag schDisplay;

	oldPage = page;
	oldChosenLine = chosenLine;

	switch ( key )
	{
	/* ---------------------------------------------------------------------------- */
	case RKEY_Exit:

		if(schDisplaySaveToFile == TRUE)
		{
			schWriteSearchList();
		}

		schServiceSV = SCH_SERVICE_RESET_SEARCH;

		exitFlag = TRUE;		// signal exit to top level - will clean up, close window,

		break;													// and enter the normal state
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolUp:	// Right
		if
		(
			(schMainTotalValidSearches > 1)
			&&
			(chosenLine < (schMainTotalValidSearches))
			&&
			(chosenLine > 0)
		)
		{
			schDisplay = schUserData[chosenLine];

			schUserData[chosenLine] = schUserData[chosenLine - 1];

			schUserData[chosenLine - 1] = schDisplay;

			chosenLine++;

			page = (chosenLine-1) / NUMBER_OF_LINES;

			if ( page == oldPage )			// only redraw what's nessesary
			{
				DisplaySearchLine(chosenLine);
				DisplaySearchLine(chosenLine - 1);
			}
			else
			{
				DrawSearchList();
			}

			schDisplaySaveToFile = TRUE;
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolDown:	// Left
		if
		(
			(schMainTotalValidSearches > 1)
			&&
			(chosenLine > 1)
			&&
			(chosenLine <= schMainTotalValidSearches)
		)
		{
			schDisplay = schUserData[chosenLine - 2];

			schUserData[chosenLine - 2] = schUserData[chosenLine - 1];

			schUserData[chosenLine - 1] = schDisplay;

			chosenLine--;

			page = (chosenLine-1) / NUMBER_OF_LINES;

			if ( page == oldPage )			// only redraw what's nessesary
			{
				DisplaySearchLine(chosenLine);
				DisplaySearchLine(chosenLine + 1);
			}
			else
			{
				DrawSearchList();
			}

			schDisplaySaveToFile = TRUE;
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChDown:

		if ( schMainTotalValidSearches > 0)
		{
			if ( chosenLine < schMainTotalValidSearches )
			{
				chosenLine++;			// 0=hidden - can't hide once cursor moved
			}
			else
			{
				chosenLine=1;
			}

			page = (chosenLine-1) / NUMBER_OF_LINES;

			if ( page == oldPage )			// only redraw what's nessesary
			{
				if ( oldChosenLine > 0 )
				{
					DisplaySearchLine(oldChosenLine);
				}

				DisplaySearchLine(chosenLine);
			}
			else
			{
				DrawSearchList();
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChUp:

		if ( schMainTotalValidSearches > 0)
		{
			if ( chosenLine > 1 )
			{
				chosenLine--;
			}
			else
			{
				chosenLine = schMainTotalValidSearches;
			}

			page = (chosenLine-1) / NUMBER_OF_LINES;

			if ( page == oldPage )					// only redraw what's nessesary
			{
				if ( oldChosenLine > 0 ) DisplaySearchLine(oldChosenLine);
				DisplaySearchLine(chosenLine);
			}
			else
			{
				DrawSearchList();
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Forward:
		if ( page == ((maxShown-1) / NUMBER_OF_LINES) )			// page down
		{
			chosenLine = maxShown;
		}
		else
		{
			page = page + 1;
			chosenLine = (page * NUMBER_OF_LINES) + 1;		// will land only on top of page
			DrawSearchList();
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Rewind:
		if ( page > 0 )							// page up
		{
			page = page - 1;
			chosenLine = (page * NUMBER_OF_LINES) + 1;		// will land only on bottom of page
			DrawSearchList();
		}
		else
		{
			page = 0;
			chosenLine = 1;
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_1:
	case RKEY_2:
	case RKEY_3:
	case RKEY_4:
	case RKEY_5:
	case RKEY_6:
	case RKEY_7:
	case RKEY_8:
	case RKEY_9:

		if ( schMainTotalValidSearches > 0)
		{
			chosenLine = (key - RKEY_0) + (page * NUMBER_OF_LINES);		// direct keyboard selection of any line

			if ( chosenLine > maxShown ) chosenLine = maxShown;

			DisplaySearchLine( oldChosenLine );
			DisplaySearchLine( chosenLine );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_0:
		if ( schMainTotalValidSearches > 0)
		{
			chosenLine = (10) + (page * NUMBER_OF_LINES);			// make "0" select the 10th (last) line
			if ( chosenLine > maxShown ) chosenLine = maxShown;

			DisplaySearchLine( oldChosenLine );
			DisplaySearchLine( chosenLine );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Ok:
		if
		(
			( chosenLine > 0 )
			&&
			( schMainTotalValidSearches > 0)
			&&
			( chosenLine <= schMainTotalValidSearches)
		)
		{
			SearchEdit(chosenLine,SCH_EXISTING_SEARCH);
		}
		break;
	/* ---------------------------------------------------------------------------- */
#ifndef WIN32
	case RKEY_Red:
#else
	case RKEY_F1:
#endif
		if(schMainTotalValidSearches < SCH_MAX_SEARCHES)
		{
			SearchEdit(0,SCH_NEW_SEARCH);
		}
		
		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Menu:
		ActivateMenu();

		break;
	/* ---------------------------------------------------------------------------- */
	default:
		break;
	/* ---------------------------------------------------------------------------- */
	}
}


//---------------------------
//
void ActivateDisplayWindow( void )
{
	schDisplaySaveToFile = FALSE;

	CreateSearchWindow();
	DrawSearchList();
	schDisplayShowLegend();
	UpdateListClock();
}

void initialiseTimerWindow( void )
{
	schDisplayWindowShowing = FALSE;
	chosenDay = DEFAULT_DAY;
	chosenLine = 0;
	page = 0;
	rowSelection = 0;
}

void RefreshSearchWindow( void )
{
	DrawGraphicBoarders();
	DrawSearchList();
	schDisplayShowLegend();
	UpdateListClock();
}

void schDisplayShowLegend( void )
{
	TAP_Osd_FillBox( rgn, 53, 514, 614, 62, FILL_COLOUR );		// clear the bottom portion

	TAP_Osd_PutGd( rgn, 62, 523, &_redoval38x19Gd, TRUE );
	TAP_Osd_PutStringAf1419( rgn, 106, 523, 666, "New Schedule", TITLE_COLOUR, 0 );
}
