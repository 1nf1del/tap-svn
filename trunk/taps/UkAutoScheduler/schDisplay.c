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
v0.6 sl8:	08-03-06	Allow the activation key to exit to TV
v0.7 sl8:	11-04-06	Show window added and tidy up.
v0.8 sl8:	19-04-06	Bug fix - Fixed operation of forwrd/rewind/numeric keys
				Filter search option added.
v0.9 sl8:	13-08-06	Copy schedule added

**************************************************************/

#define SCH_DISP_NUMBER_OF_LINES	10
#define SCH_DISP_Y1_OFFSET		(SYS_Y1_OFFSET + 23)

#define SCH_DISP_DIVIDER_X0		53
#define SCH_DISP_DIVIDER_X1		85
#define SCH_DISP_DIVIDER_X2		478
#define SCH_DISP_DIVIDER_X3		541
#define SCH_DISP_DIVIDER_X4		604
#define SCH_DISP_DIVIDER_X5		667

#define ALL_OFFSET	23
#define SET_OFFSET	20

void schDispDrawLegend(void);
void schDispDrawColumnText(void);
void schDispFilterPopulateList(void);
void schDispRefresh(void);

static	bool	schDispWindowShowing = 0;
static	bool	schDispSaveToFile = 0;
static	int	schDispChosenLine = 0;
static	int	schDispPage = 0;
static	int	schDispFilter = 0;
static	byte	schDispFilterListTotal = 0;
static	byte	schDispFilterList[SCH_MAX_SEARCHES];
static	byte	schDispLastTotalValidSearches = 0;

enum
{
	SCH_DISP_FILTER_ALL = 0,
	SCH_DISP_FILTER_RECORD,
	SCH_DISP_FILTER_WATCH,
	SCH_DISP_FILTER_DISABLED
};
	
//------------
//
void schDispWindowCreate(void)
{
	int	i = 0;

	schDispWindowShowing = TRUE;

	sysDrawGraphicBorders();

	schDispDrawColumnText();

	schDispChosenLine = 0;														// default: highlight the 1st timer
	schDispPage = 0;

	schDispFilter = SCH_DISP_FILTER_ALL;
	schDispFilterListTotal = schMainTotalValidSearches;

	for(i = 0; i < schMainTotalValidSearches; i++)
	{
		schDispFilterList[i] = i;
	}
}


void schDispWindowClose( void )
{
	schDispWindowShowing = FALSE;
}



//------------
//
void schDispDrawBackground(void)
{
	char	str[128];

	strcpy(str, "Auto Schedule");

	switch(schDispFilter)
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_DISP_FILTER_ALL:

		strcat(str, " [All]");

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_DISP_FILTER_RECORD:

		strcat(str, " [Record Only]");

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_DISP_FILTER_WATCH:

		strcat(str, " [Watch Only]");

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_DISP_FILTER_DISABLED:
	default:
		strcat(str, " [Disabled Only]");

		break;
	/* ---------------------------------------------------------------------------- */
	}

	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, str, TITLE_COLOUR, COLOR_Black );
}



//------------
//
void schDispDrawText(int line, int dispLine)
{
	char	str[132];
	int	textColour = 0;
	int	index = 0;

	if ( line == 0 ) return;											// bounds check

	index = schDispFilterList[line-1];

// Number
	memset(str,0,132);
	TAP_SPrint(str,"%d", (index + 1));

	PrintCenter(rgn, SCH_DISP_DIVIDER_X0, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, SCH_DISP_DIVIDER_X1, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	memset(str,0,132);
	TAP_SPrint(str,"\"%s\"", schUserData[index].searchTerm);

	if(schUserData[index].searchStatus == SCH_USER_DATA_STATUS_DISABLED)
	{
		if( schDispChosenLine == line )
		{
			textColour = COLOR_DarkGreen;
		}
		else
		{
			textColour = COLOR_DarkBlue;
		}

		TAP_Osd_PutStringAf1622( rgn, 130, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, 370, str, textColour, 0 );
	}
	else
	{

	// Record/Watch Icon
		if(schUserData[index].searchStatus == SCH_USER_DATA_STATUS_RECORD)
		{
			TAP_Osd_PutGd( rgn, 93, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_redcircleGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, 102, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, SCH_DISP_DIVIDER_X2, "R", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutGd( rgn, 93, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_greencircleGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, 100, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, SCH_DISP_DIVIDER_X2, "W", MAIN_TEXT_COLOUR, 0 );
		}

	// Search term
		TAP_Osd_PutStringAf1622( rgn, 130, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, 370, str, MAIN_TEXT_COLOUR, 0 );

	// Channel
		if((schUserData[index].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
		{
			TAP_Osd_PutGd( rgn, SCH_DISP_DIVIDER_X2 + 7, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_greenovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, SCH_DISP_DIVIDER_X2 + ALL_OFFSET, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, 600, "All", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutGd( rgn, SCH_DISP_DIVIDER_X2 + 7, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_redovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, SCH_DISP_DIVIDER_X2 + SET_OFFSET, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, 600, "Set", MAIN_TEXT_COLOUR, 0 );
		}

	// Days
		if(schUserData[index].searchDay == 0x7F)
		{
			TAP_Osd_PutGd( rgn, SCH_DISP_DIVIDER_X3 + 7, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_greenovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, SCH_DISP_DIVIDER_X3 + ALL_OFFSET, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, 600, "All", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutGd( rgn, SCH_DISP_DIVIDER_X3 + 7, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_redovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, SCH_DISP_DIVIDER_X3 + SET_OFFSET, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, 600, "Set", MAIN_TEXT_COLOUR, 0 );
		}

	// Time
		if(schUserData[index].searchStartTime == schUserData[index].searchEndTime)
		{
			TAP_Osd_PutGd( rgn, SCH_DISP_DIVIDER_X4 + 7, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_greenovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, SCH_DISP_DIVIDER_X4 + ALL_OFFSET, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, SCH_DISP_DIVIDER_X5, "All", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutGd( rgn, SCH_DISP_DIVIDER_X4 + 7, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_redovalGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, SCH_DISP_DIVIDER_X4 + SET_OFFSET, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET, SCH_DISP_DIVIDER_X5, "Set", MAIN_TEXT_COLOUR, 0 );
		}
	}
}



//------------
//
void schDispDrawLine(int line)
{
	int	dispLine = 0;
//	int	i = 0;
//	int	foundCount = 0;
//	bool	found = FALSE;

	if ( line == 0 ) return;											// bounds check
	
	dispLine = ((line-1) % SCH_DISP_NUMBER_OF_LINES) + 1;				// calculate position on page

	if ( schDispChosenLine == line )											// highlight the current cursor line
	{
		TAP_Osd_PutGd( rgn, 53, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_highlightGd, FALSE );
	}
	else
	{
		TAP_Osd_PutGd( rgn, 53, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, &_rowaGd, FALSE );
	}

	if
	(
		(schDispFilterListTotal > 0)
		&&
		(line <= schDispFilterListTotal)
	)
	{
		schDispDrawText(line, dispLine);
	}

	TAP_Osd_FillBox( rgn, SCH_DISP_DIVIDER_X1, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );	// draw the column seperators
	TAP_Osd_FillBox( rgn, SCH_DISP_DIVIDER_X2, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );	// temporary code - will eventually place in graphics
	TAP_Osd_FillBox( rgn, SCH_DISP_DIVIDER_X3, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );	// once positions fixed.
	TAP_Osd_FillBox( rgn, SCH_DISP_DIVIDER_X4, (dispLine * SYS_Y1_STEP) + SCH_DISP_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );
}



//------------
//
void schDispDrawList(void)
{
	int i = 0, start = 0, end = 0;

	schDispDrawBackground();

	start = (schDispPage * SCH_DISP_NUMBER_OF_LINES)+1;

	for (i = start; i < (start + SCH_DISP_NUMBER_OF_LINES); i++)
	{
		schDispDrawLine(i);
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
	char	buffer1[128];

	oldPage = schDispPage;
	oldChosenLine = schDispChosenLine;

	schDispLastTotalValidSearches = schMainTotalValidSearches;

	switch ( key )
	{
	/* ---------------------------------------------------------------------------- */
	case RKEY_Exit:

		if(schDispSaveToFile == TRUE)
		{
			schWriteSearchList();
		}

		schServiceSV = SCH_SERVICE_RESET_SEARCH;
		schMoveServiceSV = SCH_MOVE_SERVICE_WAIT_TO_CHECK;

		exitFlag = TRUE;		// signal exit to top level - will clean up, close window,

		break;													// and enter the normal state
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolUp:	// Right

		if
		(
			(schMainTotalValidSearches > 1)
			&&
			(schDispChosenLine < (schMainTotalValidSearches))
			&&
			(schDispChosenLine > 0)
			&&
			(schDispFilter == SCH_DISP_FILTER_ALL)
		)
		{
			schDisplay = schUserData[schDispChosenLine];

			schUserData[schDispChosenLine] = schUserData[schDispChosenLine - 1];

			schUserData[schDispChosenLine - 1] = schDisplay;

			schDispChosenLine++;

			schDispPage = (schDispChosenLine-1) / SCH_DISP_NUMBER_OF_LINES;

			if ( schDispPage == oldPage )			// only redraw what's nessesary
			{
				schDispDrawLine(schDispChosenLine);
				schDispDrawLine(schDispChosenLine - 1);
			}
			else
			{
				schDispDrawList();
			}

			schDispSaveToFile = TRUE;
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolDown:	// Left

		if
		(
			(schMainTotalValidSearches > 1)
			&&
			(schDispChosenLine > 1)
			&&
			(schDispChosenLine <= schMainTotalValidSearches)
			&&
			(schDispFilter == SCH_DISP_FILTER_ALL)
		)
		{
			schDisplay = schUserData[schDispChosenLine - 2];

			schUserData[schDispChosenLine - 2] = schUserData[schDispChosenLine - 1];

			schUserData[schDispChosenLine - 1] = schDisplay;

			schDispChosenLine--;

			schDispPage = (schDispChosenLine-1) / SCH_DISP_NUMBER_OF_LINES;

			if ( schDispPage == oldPage )			// only redraw what's nessesary
			{
				schDispDrawLine(schDispChosenLine);
				schDispDrawLine(schDispChosenLine + 1);
			}
			else
			{
				schDispDrawList();
			}

			schDispSaveToFile = TRUE;
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChDown:

		if ( schDispFilterListTotal > 0)
		{
			if ( schDispChosenLine < schDispFilterListTotal )
			{
				schDispChosenLine++;			// 0=hidden - can't hide once cursor moved
			}
			else
			{
				schDispChosenLine=1;
			}

			schDispPage = (schDispChosenLine-1) / SCH_DISP_NUMBER_OF_LINES;

			if ( schDispPage == oldPage )			// only redraw what's nessesary
			{
				if ( oldChosenLine > 0 )
				{
					schDispDrawLine(oldChosenLine);
				}

				schDispDrawLine(schDispChosenLine);
			}
			else
			{
				schDispDrawList();
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChUp:

		if ( schDispFilterListTotal > 0)
		{
			if ( schDispChosenLine > 1 )
			{
				schDispChosenLine--;
			}
			else
			{
				schDispChosenLine = schDispFilterListTotal;
			}

			schDispPage = (schDispChosenLine-1) / SCH_DISP_NUMBER_OF_LINES;

			if ( schDispPage == oldPage )					// only redraw what's nessesary
			{
				if ( oldChosenLine > 0 ) schDispDrawLine(oldChosenLine);
				schDispDrawLine(schDispChosenLine);
			}
			else
			{
				schDispDrawList();
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Forward:

		if(schDispFilterListTotal > 1)
		{
			if ( schDispPage == ((schDispFilterListTotal - 1) / SCH_DISP_NUMBER_OF_LINES) )			// page down
			{
				schDispChosenLine = schDispFilterListTotal;

				if ( oldChosenLine > 0 )
				{
					schDispDrawLine(oldChosenLine);
				}

				schDispDrawLine(schDispChosenLine);
			}
			else
			{
				schDispPage = schDispPage + 1;
				schDispChosenLine = (schDispPage * SCH_DISP_NUMBER_OF_LINES) + 1;		// will land only on top of page
				schDispDrawList();
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Rewind:

		if ( schDispPage > 0 )							// page up
		{
			schDispPage = schDispPage - 1;
			schDispChosenLine = (schDispPage * SCH_DISP_NUMBER_OF_LINES) + 1;		// will land only on bottom of page
			schDispDrawList();
		}
		else
		{
			schDispPage = 0;
			schDispChosenLine = 1;

			if ( oldChosenLine > 0 )
			{
				schDispDrawLine(oldChosenLine);
			}

			schDispDrawLine(schDispChosenLine);		
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

		if ( schDispFilterListTotal > 0)
		{
			schDispChosenLine = (key - RKEY_0) + (schDispPage * SCH_DISP_NUMBER_OF_LINES);		// direct keyboard selection of any line

			if ( schDispChosenLine > schDispFilterListTotal ) schDispChosenLine = schDispFilterListTotal;

			schDispDrawLine( oldChosenLine );
			schDispDrawLine( schDispChosenLine );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_0:

		if ( schDispFilterListTotal > 0)
		{
			schDispChosenLine = (10) + (schDispPage * SCH_DISP_NUMBER_OF_LINES);			// make "0" select the 10th (last) line
			if ( schDispChosenLine > schDispFilterListTotal ) schDispChosenLine = schDispFilterListTotal;

			schDispDrawLine( oldChosenLine );
			schDispDrawLine( schDispChosenLine );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Ok:

		if
		(
			(schDispChosenLine > 0)
			&&
			(schDispFilterListTotal > 0)
			&&
			(schDispChosenLine <= schDispFilterListTotal)
		)
		{
			schEditWindowActivate(schDispFilterList[schDispChosenLine - 1], SCH_EXISTING_SEARCH);
		}
		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Red:

		if(schMainTotalValidSearches < SCH_MAX_SEARCHES)
		{
			schEditWindowActivate(0,SCH_NEW_SEARCH);
		}
		
		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Green:

		if
		(
			(schDispFilterListTotal > 0)
			&&
			(schDispChosenLine > 0)
		)
		{
			schShowWindowActivate(schDispFilterList[schDispChosenLine - 1], schDispFilterList[schDispChosenLine-1], SCH_DISP_FILTER_ALL);
		}
		
		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Yellow:

		if(schDispFilterListTotal > 0)
		{
			schShowWindowActivate(schDispFilterList[0], schDispFilterList[schDispFilterListTotal - 1], schDispFilter);
		}
		
		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Blue:

		if(schDispFilter == SCH_DISP_FILTER_ALL)
		{
			schDispFilter = SCH_DISP_FILTER_RECORD;
		}
		else if(schDispFilter == SCH_DISP_FILTER_RECORD)
		{
			schDispFilter = SCH_DISP_FILTER_WATCH;
		}
		else if(schDispFilter == SCH_DISP_FILTER_WATCH)
		{
			schDispFilter = SCH_DISP_FILTER_DISABLED;
		}
		else if(schDispFilter == SCH_DISP_FILTER_DISABLED)
		{
			schDispFilter = SCH_DISP_FILTER_ALL;
		}
		else
		{
		}

		schDispFilterPopulateList();

		schDispPage = 0;
		schDispChosenLine = 0;

		schDispDrawList();

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Info:

		/* Reserved key */

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Menu:

		ActivateMenu();

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Slow:

		if(schMainTotalValidSearches < SCH_MAX_SEARCHES)
		{
			schMainTotalValidSearches++;

			schUserData[schMainTotalValidSearches - 1] = schUserData[schDispChosenLine - 1];

			schDispSaveToFile = TRUE;

			schDispRefresh();
		}

		break;
	/* ---------------------------------------------------------------------------- */
	default:

		if (mainActivationKey == key)		// Close on a 2nd press of the activation key.
		{
			if(schDispSaveToFile == TRUE)
			{
				schWriteSearchList();
			}

			schServiceSV = SCH_SERVICE_RESET_SEARCH;
			schMoveServiceSV = SCH_MOVE_SERVICE_WAIT_TO_CHECK;

			exitFlag = TRUE;		// signal exit to top level - will clean up, close window,
		}

		break;
	/* ---------------------------------------------------------------------------- */
	}
}


//---------------------------
//
void schDispWindowActivate( void )
{
	schDispSaveToFile = FALSE;

	schDispWindowCreate();
	schDispDrawList();
	schDispDrawLegend();
	UpdateListClock();
}

void schDispWindowInitialise( void )
{
	schDispWindowShowing = FALSE;
	schDispChosenLine = 0;
	schDispPage = 0;
}

void schDispRefresh( void )
{

	if(schMainTotalValidSearches > schDispLastTotalValidSearches)		// Sched added
	{
		schDispFilter = SCH_DISP_FILTER_ALL;

		schDispFilterPopulateList();

		schDispChosenLine = schDispFilterListTotal;

		schDispPage = (schDispChosenLine-1) / SCH_DISP_NUMBER_OF_LINES;
	}
	else if(schDispLastTotalValidSearches > schMainTotalValidSearches)	// Sched deleted
	{
		schDispFilterPopulateList();

		if(schDispChosenLine > schDispFilterListTotal)
		{
			schDispChosenLine = schDispFilterListTotal;
		}
	}
	else
	{
	}

	sysDrawGraphicBorders();
	schDispDrawColumnText();

	schDispDrawList();
	schDispDrawLegend();
	UpdateListClock();
}

#define LEG_START	52
#define LEG_TEXT_OFFSET	44
#define LEG_SPACING	156

void schDispDrawLegend( void )
{
	TAP_Osd_FillBox( rgn, 53, 514, 614, 62, INFO_FILL_COLOUR );		// clear the bottom portion

	TAP_Osd_PutGd( rgn, LEG_START + (0 * LEG_SPACING), 523, &_redoval38x19Gd, TRUE );
	TAP_Osd_PutStringAf1419( rgn, LEG_START + (0 * LEG_SPACING) + LEG_TEXT_OFFSET, 523, 666, "New Schedule", INFO_COLOUR, 0 );

	TAP_Osd_PutGd( rgn, LEG_START + (1 * LEG_SPACING), 523, &_greenoval38x19Gd, TRUE );
	TAP_Osd_PutStringAf1419( rgn, LEG_START + (1 * LEG_SPACING) + LEG_TEXT_OFFSET, 523, 666, "Search", INFO_COLOUR, 0 );

	TAP_Osd_PutGd( rgn, LEG_START + (2 * LEG_SPACING), 523, &_yellowoval38x19Gd, TRUE );
	TAP_Osd_PutStringAf1419( rgn, LEG_START + (2 * LEG_SPACING) + LEG_TEXT_OFFSET, 523, 666, "Search All", INFO_COLOUR, 0 );

	TAP_Osd_PutGd( rgn, LEG_START + (3 * LEG_SPACING), 523, &_blueoval38x19Gd, TRUE );
	TAP_Osd_PutStringAf1419( rgn, LEG_START + (3 * LEG_SPACING) + LEG_TEXT_OFFSET, 523, 666, "Filter", INFO_COLOUR, 0 );
}


void schDispDrawColumnText(void)
{
	PrintCenter(rgn, SCH_DISP_DIVIDER_X0, 71, SCH_DISP_DIVIDER_X1, "No.", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, SCH_DISP_DIVIDER_X1, 71, SCH_DISP_DIVIDER_X2, "Search Term", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, SCH_DISP_DIVIDER_X2, 71, SCH_DISP_DIVIDER_X3, "Channel", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, SCH_DISP_DIVIDER_X3, 71, SCH_DISP_DIVIDER_X4, "Day", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, SCH_DISP_DIVIDER_X4, 71, SCH_DISP_DIVIDER_X5, "Time", TITLE_COLOUR, 0, FNT_Size_1419 );
}


void schDispFilterPopulateList(void)
{
	int	i = 0;

	schDispFilterListTotal = 0;
	for(i = 0; i < schMainTotalValidSearches; i++)
	{
		schDispFilterList[i] = 0;

		if
		(
			(schDispFilter == SCH_DISP_FILTER_ALL)
			||
			(
				(schDispFilter == SCH_DISP_FILTER_RECORD)
				&&
				(schUserData[i].searchStatus == SCH_USER_DATA_STATUS_RECORD)
			)
			||
			(
				(schDispFilter == SCH_DISP_FILTER_WATCH)
				&&
				(schUserData[i].searchStatus == SCH_USER_DATA_STATUS_WATCH)
			)
			||
			(
				(schDispFilter == SCH_DISP_FILTER_DISABLED)
				&&
				(schUserData[i].searchStatus == SCH_USER_DATA_STATUS_DISABLED)
			)
		)
		{
			schDispFilterList[schDispFilterListTotal] = i;

			schDispFilterListTotal++;
		}
	}
}

