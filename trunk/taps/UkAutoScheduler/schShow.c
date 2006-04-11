/************************************************************
Part of the ukEPG project
This module displays the schedules

  v0.0 sl8:	11-04-06	Inception date

**************************************************************/

void schShowDrawLegend(void);
void schShowDrawList(void);
void schShowDrawInfo(int);
void schShowDrawProgress(int,int,int,int,int,int);
void schShowCheckTimer(int,int);
void schShowSortResults(int);

#ifndef WIN32
	#define SCH_SHOW_MAX_RESULTS		100
#else
	#define SCH_SHOW_MAX_RESULTS		20
#endif

#define SCH_SHOW_NUMBER_OF_LINES	8
#define SCH_SHOW_Y1_OFFSET		(SYS_Y1_OFFSET + 23)

#define SCH_SHOW_DIVIDER_WIDTH		3
#define SCH_SHOW_DIVIDER_X0		53
#define SCH_SHOW_DIVIDER_X1		85
#define SCH_SHOW_DIVIDER_X2		127	// icon / programme name border
#define SCH_SHOW_DIVIDER_X3		416	// programme / time border
#define SCH_SHOW_DIVIDER_X4		530	// time / day border 
#define SCH_SHOW_DIVIDER_X5		604	// day / logo border
#define SCH_SHOW_DIVIDER_X6		667

// Position of the progess pop-up Screen
#define SCH_SHOW_PROGRESS_WINDOW_W	360
#define SCH_SHOW_PROGRESS_WINDOW_H	130
#define SCH_SHOW_PROGRESS_WINDOW_X	((MAX_SCREEN_X - SCH_SHOW_PROGRESS_WINDOW_W) / 2)
#define SCH_SHOW_PROGRESS_WINDOW_Y	((MAX_SCREEN_Y - SCH_SHOW_PROGRESS_WINDOW_H) / 2)

#define SCH_SHOW_OFFSET			8

#define SCH_SHOW_DESCRIPTION_SIZE	256

enum
{
	SCH_SHOW_SORT_ORDER_TIME = 0,
	SCH_SHOW_SORT_ORDER_CHANNEL,
	SCH_SHOW_SORT_ORDER_NAME
};

typedef struct
{
	char	name[128];
	char	description[SCH_SHOW_DESCRIPTION_SIZE];
	dword	startTime;
	dword	endTime;
	word	duration;
	int	svcNum;
	int	svcType;
	bool	timerSet;
	byte	isRec;
}schShowResultsStruct;


static	int	schShowNumberOfResults = 0;
static	int	schShowSearchIndex = 0;
static	int	schShowStartSearchIndex = 0;
static	int	schShowEndSearchIndex = 0;
static	byte	schShowServiceSV = SCH_SHOW_SERVICE_INITIALISE;
static	int	schShowChosenLine = 0;
static	int	schShowPage = 0;
static	int	schShowMaxShown = 0;
static	int	schShowSortOrder = 0;

static	schShowResultsStruct	*schShowResults = NULL;
static	schShowResultsStruct	*schShowResultsPtr[SCH_SHOW_MAX_RESULTS];

//------------
//
void schShowWindowCreate(void)
{
	sysDrawGraphicBorders();

	PrintCenter(rgn, SCH_SHOW_DIVIDER_X0, 71, SCH_SHOW_DIVIDER_X1, "No.", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, SCH_SHOW_DIVIDER_X1, 71, SCH_SHOW_DIVIDER_X2, "Set", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, SCH_SHOW_DIVIDER_X2, 71, SCH_SHOW_DIVIDER_X3, "Programme Name", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, SCH_SHOW_DIVIDER_X3, 71, SCH_SHOW_DIVIDER_X4, "Time", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, SCH_SHOW_DIVIDER_X4, 71, SCH_SHOW_DIVIDER_X5, "Day", TITLE_COLOUR, 0, FNT_Size_1419 );
	PrintCenter(rgn, SCH_SHOW_DIVIDER_X5, 71, SCH_SHOW_DIVIDER_X6, "Channel", TITLE_COLOUR, 0, FNT_Size_1419 );
	
	schShowChosenLine = 0;														// default: highlight the 1st timer
	schShowPage = 0;
}


void schShowWindowClose( void )
{
	schShowWindowShowing = FALSE;
}



//------------
//
void schShowDrawBackground(void)
{
	char	str[128];

	strcpy(str, "Auto Schedule - Show");

	switch(schShowSortOrder)
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_SHOW_SORT_ORDER_TIME:

		strcat(str, " [by time]");

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_SHOW_SORT_ORDER_CHANNEL:

		strcat(str, " [by chan]");

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_SHOW_SORT_ORDER_NAME:
	default:
		strcat(str, " [by name]");

		break;
	/* ---------------------------------------------------------------------------- */
	}

	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, str, TITLE_COLOUR, COLOR_Black );


//	PrintCenter(rgn, SCH_SHOW_DIVIDER_X0, 71, SCH_SHOW_DIVIDER_X1, "No.", TITLE_COLOUR, 0, FNT_Size_1419 );
//	PrintCenter(rgn, SCH_SHOW_DIVIDER_X1, 71, SCH_SHOW_DIVIDER_X2, "Set", TITLE_COLOUR, 0, FNT_Size_1419 );
//	PrintCenter(rgn, SCH_SHOW_DIVIDER_X2, 71, SCH_SHOW_DIVIDER_X3, "Programme Name", TITLE_COLOUR, 0, FNT_Size_1419 );
//	PrintCenter(rgn, SCH_SHOW_DIVIDER_X3, 71, SCH_SHOW_DIVIDER_X4, "Time", TITLE_COLOUR, 0, FNT_Size_1419 );
//	PrintCenter(rgn, SCH_SHOW_DIVIDER_X4, 71, SCH_SHOW_DIVIDER_X5, "Day", TITLE_COLOUR, 0, FNT_Size_1419 );
//	PrintCenter(rgn, SCH_SHOW_DIVIDER_X5, 71, SCH_SHOW_DIVIDER_X6, "Channel", TITLE_COLOUR, 0, FNT_Size_1419 );
}



//------------
//
void schShowDrawText(int line, int dispLine)
{
	char	str[132];
	char	str2[132];
	int	textColour = 0;
	byte	startHour = 0, startMin = 0;
	byte	endHour = 0, endMin = 0;
	word	year = 0;
	byte	month = 0, day = 0, weekDay = 0;

	if ( line == 0 ) return;											// bounds check

// Number
	memset(str,0,132);
	TAP_SPrint(str,"%d", line);

	PrintCenter(rgn, SCH_SHOW_DIVIDER_X0, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET, SCH_SHOW_DIVIDER_X1, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	memset(str,0,132);
	TAP_SPrint(str,"\"%s\"", schShowResultsPtr[line-1]->name);

// Record/Watch Icon
	if(schShowResultsPtr[line-1]->timerSet == TRUE)
	{
		if(schShowResultsPtr[line-1]->isRec == 1)
		{
			TAP_Osd_PutGd( rgn, 93, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET - 8, &_redcircleGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, 102, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET, SCH_SHOW_DIVIDER_X2, "R", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutGd( rgn, 93, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET - 8, &_greencircleGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, 100, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET, SCH_SHOW_DIVIDER_X2, "W", MAIN_TEXT_COLOUR, 0 );
		}
	}


// Programme Name
	TAP_Osd_PutStringAf1622( rgn, (SCH_SHOW_DIVIDER_X2 + SCH_SHOW_OFFSET + SCH_SHOW_DIVIDER_WIDTH), (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET, SCH_SHOW_DIVIDER_X3 - SCH_SHOW_OFFSET, schShowResultsPtr[line-1]->name, MAIN_TEXT_COLOUR, 0 );

// Start and end time
	startHour = (schShowResultsPtr[line-1]->startTime & 0xff00) >> 8;
	startMin = (schShowResultsPtr[line-1]->startTime & 0xff);
	endHour = (schShowResultsPtr[line-1]->endTime & 0xff00) >> 8;
	endMin = (schShowResultsPtr[line-1]->endTime & 0xff);
				
	TAP_SPrint(str, "%02d:%02d ~ %02d:%02d", startHour, startMin, endHour, endMin);
	PrintCenter( rgn, (SCH_SHOW_DIVIDER_X3 + SCH_SHOW_DIVIDER_WIDTH), (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET, SCH_SHOW_DIVIDER_X4, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );




// Day

	TAP_ExtractMjd( ((schShowResultsPtr[line-1]->startTime & 0xffff0000) >> 16), &year, &month, &day, &weekDay) ;
	
	switch ( weekDay )
	{
		case 0:	TAP_SPrint(str2,"Mon"); break;
		case 1:	TAP_SPrint(str2,"Tue"); break;
		case 2:	TAP_SPrint(str2,"Wed"); break;
		case 3:	TAP_SPrint(str2,"Thu"); break;
		case 4:	TAP_SPrint(str2,"Fri"); break;
		case 5:	TAP_SPrint(str2,"Sat"); break;
		case 6:	TAP_SPrint(str2,"Sun"); break;
		default : TAP_SPrint(str2,"OT BAD"); break;
	}

	sprintf( str, "%s %d", str2, day);

	PrintCenter( rgn, (SCH_SHOW_DIVIDER_X4 + SCH_SHOW_DIVIDER_WIDTH), (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET, SCH_SHOW_DIVIDER_X5, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

// Logo 
	DisplayLogo( rgn, 607, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET-8, schShowResultsPtr[line-1]->svcNum, schShowResultsPtr[line-1]->svcType );

}



//------------
//
void schShowDrawLine(int line)
{
	int dispLine = 0;

	if ( line == 0 ) return;											// bounds check
	
	dispLine = ((line-1) % SCH_SHOW_NUMBER_OF_LINES) + 1;				// calculate position on page

	if ( schShowChosenLine == line )											// highlight the current cursor line
	{
		TAP_Osd_PutGd( rgn, 53, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET - 8, &_highlightGd, FALSE );
	}
	else
	{
		TAP_Osd_PutGd( rgn, 53, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET - 8, &_rowaGd, FALSE );
	}

	if
	(
		(schShowNumberOfResults > 0)
		&&
		(line <= schShowNumberOfResults)
	)
	{
		schShowDrawText(line, dispLine);						// anything to display ?
	}

	TAP_Osd_FillBox( rgn, SCH_SHOW_DIVIDER_X1, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET - 8, SCH_SHOW_DIVIDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );	// draw the column seperators
	TAP_Osd_FillBox( rgn, SCH_SHOW_DIVIDER_X2, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET - 8, SCH_SHOW_DIVIDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );	// temporary code - will eventually place in graphics
	TAP_Osd_FillBox( rgn, SCH_SHOW_DIVIDER_X3, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET - 8, SCH_SHOW_DIVIDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );	// once positions fixed.
	TAP_Osd_FillBox( rgn, SCH_SHOW_DIVIDER_X4, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET - 8, SCH_SHOW_DIVIDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );
	TAP_Osd_FillBox( rgn, SCH_SHOW_DIVIDER_X5, (dispLine * SYS_Y1_STEP) + SCH_SHOW_Y1_OFFSET - 8, SCH_SHOW_DIVIDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );

	if ( schShowChosenLine == line )											// highlight the current cursor line
	{
		schShowDrawInfo(line);
	}
}



void schShowDrawInfo(int line)
{
	int	schIndex = 0;
	byte	startHour = 0, startMin = 0;
	byte	endHour = 0, endMin = 0;
	byte	durHour = 0, durMin = 0;
	char	str[512];
	char	str2[128];

	if ( line == 0 ) return;

	schIndex = line - 1;

	TAP_Osd_FillBox( rgn, 53, 430, 614, 91, INFO_FILL_COLOUR );		// clear the bottom portion

	startHour = (schShowResultsPtr[line-1]->startTime & 0xff00) >> 8;
	startMin = (schShowResultsPtr[line-1]->startTime & 0xff);
	endHour = (schShowResultsPtr[line-1]->endTime & 0xff00) >> 8;
	endMin = (schShowResultsPtr[line-1]->endTime & 0xff);
	durHour = (schShowResultsPtr[line-1]->duration & 0xff00) >> 8;
	durMin = (schShowResultsPtr[line-1]->duration & 0xff);
				
	sprintf(str, "%s %02d:%02d ~ %02d:%02d (", schShowResultsPtr[schIndex]->name, startHour, startMin, endHour, endMin);
	
	if (durHour > 0)
	{	
		sprintf( str2, "%d hour", durHour);
		strcat(str, str2);

		if (durHour > 1)
		{
			strcat(str, "s");
		}
	}

	if (durMin > 0)
	{	
		if (durHour > 0)
		{
			strcat(str, " ");
		}

		sprintf( str2, "%d minute", durMin);
		strcat(str, str2);

		if (durMin > 1)
		{
			strcat(str, "s");
		}
	}

	strcat(str, ")");

	WrapPutStr( rgn, str, 60, 434, 602, INFO_COLOUR, INFO_FILL_COLOUR, 1, FNT_Size_1622, 0);

//	strcpy(str, schShowResults[schIndex].description);
//	strcat(str, schShowResults[schIndex].description);

//	WrapPutStr( rgn, str, 60, 460, 602, TITLE_COLOUR, COLOR_Black, 3, FNT_Size_1419, 0);
	WrapPutStr( rgn, schShowResultsPtr[schIndex]->description, 60, 460, 602, INFO_COLOUR, INFO_FILL_COLOUR, 3, FNT_Size_1419, 0);

//	TAP_Osd_PutStringAf1419( rgn, 106, 523, 666, "???????", TITLE_COLOUR, 0 );

/*
	ExtInfoRows = ((EVENT_Y + EVENT_H) - LastWrapPutStr_Y ) / extInfoRowSize ;   //KH Calculate how many lines are left available for Extended Info with a font height of 19.
	if( extInfo )
	{
		WrapPutStr_StartPos = LastWrapPutStr_P; //KH Set the start position of the string to where we got up to last time.
		WrapPutStr( MemRgn, extInfo, EVENT_X+8, LastWrapPutStr_Y, EVENT_W-8, MAIN_TEXT_COLOUR, EVENT_FILL_COLOUR, ExtInfoRows, extInfoFontSize, 0);
		TAP_MemFree( extInfo );
	}
*/



}



//------------
//
void schShowDrawList(void)
{
	int i = 0, start = 0, end = 0;

	schShowDrawBackground();

	start = (schShowPage * SCH_SHOW_NUMBER_OF_LINES)+1;

	for ( i=start; i < (start + SCH_SHOW_NUMBER_OF_LINES) ; i++)
	{
		schShowDrawLine(i);
	}
}





//------------
//
void schShowKeyHandler(dword key)
{
	int	oldPage = 0, oldChosenLine = 0;
	char buffer1[256];

	oldPage = schShowPage;
	oldChosenLine = schShowChosenLine;

	switch ( key )
	{
	/* ---------------------------------------------------------------------------- */
	case RKEY_Exit:

		if( schShowResults )
		{
			TAP_MemFree( schShowResults );
			schShowResults = NULL;
		}

		schShowWindowClose();					// Close the show window
		returnFromEdit = TRUE;					// will cause a redraw of timer list

		break;													// and enter the normal state
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChDown:

		if ( schShowNumberOfResults > 0)
		{
			if ( schShowChosenLine < schShowNumberOfResults )
			{
				schShowChosenLine++;			// 0=hidden - can't hide once cursor moved
			}
			else
			{
				schShowChosenLine = 1;
			}

			schShowPage = (schShowChosenLine - 1) / SCH_SHOW_NUMBER_OF_LINES;

			if ( schShowPage == oldPage )			// only redraw what's nessesary
			{
				if ( oldChosenLine > 0 )
				{
					schShowDrawLine(oldChosenLine);
				}

				schShowDrawLine(schShowChosenLine);
			}
			else
			{
				schShowDrawList();
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChUp:

		if ( schShowNumberOfResults > 0)
		{
			if ( schShowChosenLine > 1 )
			{
				schShowChosenLine--;
			}
			else
			{
				schShowChosenLine = schShowNumberOfResults;
			}

			schShowPage = (schShowChosenLine - 1) / SCH_SHOW_NUMBER_OF_LINES;

			if ( schShowPage == oldPage )					// only redraw what's nessesary
			{
				if ( oldChosenLine > 0 ) schShowDrawLine(oldChosenLine);
				schShowDrawLine(schShowChosenLine);
			}
			else
			{
				schShowDrawList();
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Forward:

		if ( schShowPage == ((schShowMaxShown-1) / SCH_SHOW_NUMBER_OF_LINES) )			// page down
		{
			schShowChosenLine = schShowMaxShown;
		}
		else
		{
			schShowPage = schShowPage + 1;
			schShowChosenLine = (schShowPage * SCH_SHOW_NUMBER_OF_LINES) + 1;		// will land only on top of page
			schShowDrawList();
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Rewind:

		if ( schShowPage > 0 )							// page up
		{
			schShowPage = schShowPage - 1;
			schShowChosenLine = (schShowPage * SCH_SHOW_NUMBER_OF_LINES) + 1;		// will land only on bottom of page
			schShowDrawList();
		}
		else
		{
			schShowPage = 0;
			schShowChosenLine = 1;
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

		if ( schShowNumberOfResults > 0)
		{
			schShowChosenLine = (key - RKEY_0) + (schShowPage * SCH_SHOW_NUMBER_OF_LINES);		// direct keyboard selection of any line

			if ( schShowChosenLine > schShowMaxShown ) schShowChosenLine = schShowMaxShown;

			schShowDrawLine( oldChosenLine );
			schShowDrawLine( schShowChosenLine );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Ok:

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Red:

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Yellow:
	case RKEY_Green:
	case RKEY_Info:
		
		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Blue:

		if(schShowSortOrder == SCH_SHOW_SORT_ORDER_TIME)
		{
			schShowSortOrder = SCH_SHOW_SORT_ORDER_CHANNEL;
		}
		else if(schShowSortOrder == SCH_SHOW_SORT_ORDER_CHANNEL)
		{
			schShowSortOrder = SCH_SHOW_SORT_ORDER_NAME;
		}
		else if(schShowSortOrder == SCH_SHOW_SORT_ORDER_NAME)
		{
			schShowSortOrder = SCH_SHOW_SORT_ORDER_TIME;
		}
		else
		{
		}
		
		schShowSortResults(schShowSortOrder);

		schShowDrawList();
	
		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Menu:

		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}
}







//---------------------------
//
void schShowWindowActivate( int startSearch, int endSearch )
{
	schShowStartSearchIndex = startSearch - 1;
	schShowEndSearchIndex = endSearch;

	schShowSearchIndex = schShowStartSearchIndex;

	schShowServiceSV = SCH_SHOW_SERVICE_INITIALISE;
	schShowWindowShowing = TRUE;
}

void schShowWindowInitialise( void )
{
	schShowWindowShowing = FALSE;
	schShowChosenLine = 0;
	schShowPage = 0;
}

void schShowRefresh( void )
{
	sysDrawGraphicBorders();
	schShowDrawList();
	schShowDrawLegend();
	UpdateListClock();
}

void schShowDrawLegend( void )
{
	TAP_Osd_FillBox( rgn, 53, 430, 614, 140, INFO_FILL_COLOUR );		// clear the bottom portion

	TAP_Osd_PutGd( rgn, LEG_START + (0 * LEG_SPACING), 523, &_redoval38x19Gd, TRUE );
//	TAP_Osd_PutStringAf1419( rgn, LEG_START + (0 * LEG_SPACING) + LEG_TEXT_OFFSET, 523, 666, "Spare", INFO_COLOUR, 0 );

	TAP_Osd_PutGd( rgn, LEG_START + (1 * LEG_SPACING), 523, &_greenoval38x19Gd, TRUE );
//	TAP_Osd_PutStringAf1419( rgn, LEG_START + (1 * LEG_SPACING) + LEG_TEXT_OFFSET, 523, 666, "Spare", INFO_COLOUR, 0 );

	TAP_Osd_PutGd( rgn, LEG_START + (2 * LEG_SPACING), 523, &_yellowoval38x19Gd, TRUE );
	TAP_Osd_PutStringAf1419( rgn, LEG_START + (2 * LEG_SPACING) + LEG_TEXT_OFFSET, 523, 666, "Sellect All", INFO_COLOUR, 0 );

	TAP_Osd_PutGd( rgn, LEG_START + (3 * LEG_SPACING), 523, &_blueoval38x19Gd, TRUE );
	TAP_Osd_PutStringAf1419( rgn, LEG_START + (3 * LEG_SPACING) + LEG_TEXT_OFFSET, 523, 666, "Sort", INFO_COLOUR, 0 );
}


void schShowService( void )
{
	static word schShowChannelIndex = 0;
	static word schShowStartChannelIndex = 0;
	static word schShowEndChannelIndex = 0;
	static int schShowEpgIndex = 0;
	static int schShowEpgTotalEvents = 0;
	char str[128];
	char buffer1[128];


	switch(schShowServiceSV)
	{
	/*--------------------------------------------------*/
	case SCH_SHOW_SERVICE_INITIALISE:

		TAP_Osd_PutGd( rgn, SCH_SHOW_PROGRESS_WINDOW_X, SCH_SHOW_PROGRESS_WINDOW_Y, &_popup360x130Gd, TRUE );
		PrintCenter( rgn, SCH_SHOW_PROGRESS_WINDOW_X + 5, SCH_SHOW_PROGRESS_WINDOW_Y +  13, SCH_SHOW_PROGRESS_WINDOW_X + SCH_SHOW_PROGRESS_WINDOW_W - 5, "Searching...", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
		PrintCenter( rgn, SCH_SHOW_PROGRESS_WINDOW_X + 5, SCH_SHOW_PROGRESS_WINDOW_Y +  58, SCH_SHOW_PROGRESS_WINDOW_X + SCH_SHOW_PROGRESS_WINDOW_W - 5, "0 %", MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );
		PrintCenter( rgn, SCH_SHOW_PROGRESS_WINDOW_X + 5, SCH_SHOW_PROGRESS_WINDOW_Y +  104, SCH_SHOW_PROGRESS_WINDOW_X + SCH_SHOW_PROGRESS_WINDOW_W - 5, "(Press EXIT to cancel search)", MAIN_TEXT_COLOUR, 0, FNT_Size_1419 );

		schShowResults = (schShowResultsStruct*)TAP_MemAlloc(sizeof(schShowResultsStruct) * SCH_SHOW_MAX_RESULTS);

		schShowNumberOfResults = 0;

		schShowServiceSV = SCH_SHOW_SERVICE_BEGIN_SEARCH;
TAP_Print("Show - Init\r\n");
		break;
	/*-------------------------------------------------*/
	case SCH_SHOW_SERVICE_BEGIN_SEARCH:

TAP_Print("Show - Begin\r\n");

		schShowEpgIndex = 0;

		if((schUserData[schShowSearchIndex].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
		{
			schShowStartChannelIndex = 0;

			if(schUserData[schShowSearchIndex].searchTvRadio == SCH_TV)
			{
				schShowEndChannelIndex = schTotalTvSvc;
			}
			else
			{
				schShowEndChannelIndex = schTotalRadioSvc;
			}
		}
		else
		{
			schShowStartChannelIndex = schUserData[schShowSearchIndex].searchStartSvcNum;
			schShowEndChannelIndex = schUserData[schShowSearchIndex].searchEndSvcNum;
		}

		schShowChannelIndex = schShowStartChannelIndex;

		schShowServiceSV = SCH_SHOW_SERVICE_INITIALISE_EPG_DATA;

		break;
	/*--------------------------------------------------*/
	case SCH_SHOW_SERVICE_INITIALISE_EPG_DATA:
TAP_Print("Show - Init epg data\r\n");

#ifndef WIN32
		if( schEpgData )
		{
			TAP_MemFree( schEpgData );
			schEpgData = 0;
		}

		schEpgData = TAP_GetEvent( schUserData[schShowSearchIndex].searchTvRadio, schShowChannelIndex, &schShowEpgTotalEvents );
#else
		schEpgData = TAP_GetEvent_SDK( schUserData[schShowSearchIndex].searchTvRadio, schShowChannelIndex, &schShowEpgTotalEvents );
#endif
		schShowServiceSV = SCH_SHOW_SERVICE_PERFORM_SEARCH;

		break;	
	/*--------------------------------------------------*/
	case SCH_SHOW_SERVICE_PERFORM_SEARCH:
//TAP_Print("Show - Perform Search\r\n");

//		schShowDrawProgress(schShowEpgIndex, schShowEpgTotalEvents);

		schShowDrawProgress(schShowSearchIndex, schShowStartSearchIndex, schShowEndSearchIndex, schShowChannelIndex, schShowStartChannelIndex, schShowEndChannelIndex);

		if
		(
			(schEpgData)
			&&
			(schShowEpgTotalEvents > 0)
			&&
			(schShowEpgIndex < schShowEpgTotalEvents)
			&&
			(schShowNumberOfResults < SCH_SHOW_MAX_RESULTS)
		)
		{
			if((schPerformSearch(schEpgData, schShowEpgIndex, schShowSearchIndex)) == TRUE)
//			if(0)
			{
TAP_Print("Show - Found\r\n");

			strcpy(schShowResults[schShowNumberOfResults].name, schEpgData[schShowEpgIndex].eventName);

			if( schEpgDataExtendedInfo )
			{
				TAP_MemFree( schEpgDataExtendedInfo );
				schEpgDataExtendedInfo = 0;
			}

			schEpgDataExtendedInfo = TAP_EPG_GetExtInfo(&schEpgData[schShowEpgIndex]);

			if
			(
				(schEpgDataExtendedInfo)
				&&
				(strncmp(schEpgData[schShowEpgIndex].description, schEpgDataExtendedInfo, 127) == 0)
			)
			{
				strncpy(schShowResults[schShowNumberOfResults].description, schEpgDataExtendedInfo, SCH_SHOW_DESCRIPTION_SIZE);
			}
			else
			{
				strcpy(schShowResults[schShowNumberOfResults].description, schEpgData[schShowEpgIndex].description);
			}

			schShowResults[schShowNumberOfResults].startTime = schEpgData[schShowEpgIndex].startTime;
			schShowResults[schShowNumberOfResults].endTime = schEpgData[schShowEpgIndex].endTime;
			schShowResults[schShowNumberOfResults].duration = schEpgData[schShowEpgIndex].duration;
			schShowResults[schShowNumberOfResults].svcNum = schShowChannelIndex;
			schShowResults[schShowNumberOfResults].svcType = schUserData[schShowSearchIndex].searchTvRadio;

			schShowCheckTimer(schShowNumberOfResults, schShowSearchIndex);

			schShowResultsPtr[schShowNumberOfResults] = &schShowResults[schShowNumberOfResults];


			schShowNumberOfResults++;

//	eventMjd = ((epgData[epgDataIndex].startTime >> 16) & 0xFFFF);
//	eventHour = (( >> 8) & 0xFF);
//	eventMin = (epgData[epgDataIndex].startTime & 0xFF);


//				schSetTimer(schEpgData, schShowEpgIndex, schShowSearchIndex, schShowChannelIndex);
			}

			schShowEpgIndex++;
		}
		else
		{
			schShowServiceSV = SCH_SHOW_SERVICE_NEXT_CHANNEL;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SHOW_SERVICE_NEXT_CHANNEL:

TAP_Print("Show - Next Channel\r\n");
		schShowChannelIndex++;
		schShowEpgIndex = 0;

		if(schShowNumberOfResults >= SCH_SHOW_MAX_RESULTS)
		{
			schShowServiceSV = SCH_SHOW_SERVICE_DISPLAY_RESULTS;
		}
		else if((schUserData[schShowSearchIndex].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
		{
			if
			(
				(
					(schShowChannelIndex >= schTotalTvSvc)
					&&
					(schUserData[schShowSearchIndex].searchTvRadio == SCH_TV)
				)
				||
				(
					(schShowChannelIndex >= schTotalRadioSvc)
					&&
					(schUserData[schShowSearchIndex].searchTvRadio == SCH_RADIO)
				)
			)
			{
				schShowServiceSV = SCH_SHOW_SERVICE_NEXT_SEARCH;
			}
			else
			{
				schShowServiceSV = SCH_SHOW_SERVICE_INITIALISE_EPG_DATA;
			}
		}
		else
		{
			if(schShowChannelIndex > schUserData[schShowSearchIndex].searchEndSvcNum)
			{
				schShowServiceSV = SCH_SHOW_SERVICE_NEXT_SEARCH;
			}
			else
			{
				schShowServiceSV = SCH_SHOW_SERVICE_INITIALISE_EPG_DATA;
			}
		}

		break;	
	/*--------------------------------------------------*/
	case SCH_SHOW_SERVICE_NEXT_SEARCH:

TAP_Print("Show - Next Search\r\n");

		schShowSearchIndex++;

		schShowEpgIndex = 0;

		if(schShowSearchIndex >= schShowEndSearchIndex)
		{
			schShowServiceSV = SCH_SHOW_SERVICE_DISPLAY_RESULTS;
		}
		else if(schUserData[schShowSearchIndex].searchStatus != SCH_USER_DATA_STATUS_DISABLED)
		{
			if((schUserData[schShowSearchIndex].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
			{
				schShowChannelIndex = 0;
			}
			else
			{
				schShowChannelIndex = schUserData[schShowSearchIndex].searchStartSvcNum;
			}

			schShowServiceSV = SCH_SHOW_SERVICE_INITIALISE_EPG_DATA;
		}
		else
		{
			schShowServiceSV = SCH_SHOW_SERVICE_NEXT_SEARCH;
		}

		break;	
	/*--------------------------------------------------*/
	case SCH_SHOW_SERVICE_DISPLAY_RESULTS:

TAP_Print("Show - Display\r\n");

		if(schShowNumberOfResults > 1)
		{
			schShowSortResults(SCH_SHOW_SORT_ORDER_TIME);
		}

		schShowWindowCreate();
		schShowDrawList();
		schShowDrawLegend();

		sprintf(str, "Found %d Matches", schShowNumberOfResults);
		PrintCenter( rgn, SCH_SHOW_DIVIDER_X1, 450, SCH_SHOW_DIVIDER_X6, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
//		WrapPutStr( rgn, str, 60, 434, 602, INFO_COLOUR, INFO_FILL_COLOUR, 1, FNT_Size_1622, 0);

		UpdateListClock();

		schShowServiceSV = SCH_SHOW_SERVICE_COMPLETE;

		break;
	/*--------------------------------------------------*/
	case SCH_SHOW_SERVICE_COMPLETE:
	default:
	
		break;
	/*--------------------------------------------------*/
	}
}


void schShowDrawProgress(int schShowSearchIndex, int  schShowStartSearchIndex, int  schShowEndSearchIndex, int schShowChannelIndex, int schShowStartChannelIndex, int schShowEndChannelIndex)
{
	int	progress = 0;
	char	str[128];
	word	mjd = 0;
	byte	hour = 0, min = 0, sec = 0;
	static	byte	lastSec = 0;

	if(schShowStartSearchIndex >= schShowEndSearchIndex)
	{
		return;
	}

	TAP_GetTime( &mjd, &hour, &min, &sec);

	if(sec != lastSec)
	{
		progress = ((schShowSearchIndex - schShowStartSearchIndex) * 100) / (schShowEndSearchIndex - schShowStartSearchIndex);

//		progress = ((schShowSearchIndex - schShowStartSearchIndex) * 100 * 10) / (schShowEndSearchIndex - schShowStartSearchIndex);

//		progress += ((schShowChannelIndex - schShowStartChannelIndex) * 100) / (schShowEndChannelIndex - schShowStartChannelIndex);

//		progress /= 10;

		TAP_Osd_FillBox( rgn, SCH_SHOW_PROGRESS_WINDOW_X + 40, SCH_SHOW_PROGRESS_WINDOW_Y + 54, SCH_SHOW_PROGRESS_WINDOW_W - 80, 36, POPUP_FILL_COLOUR );

		sprintf(str, "%d", progress);
		strcat(str," %");

		PrintCenter( rgn, SCH_SHOW_PROGRESS_WINDOW_X + 5, SCH_SHOW_PROGRESS_WINDOW_Y + 58, SCH_SHOW_PROGRESS_WINDOW_X + SCH_SHOW_PROGRESS_WINDOW_W - 5, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1926 );

		lastSec = sec;
	}
}

void schShowCheckTimer(int schShowResultIndex, int schShowSearchIndex)
{
	dword	resultStartTimeInMins = 0, resultDurationTimeInMins = 0;
	dword	searchStartPaddingInMins = 0, searchEndPaddingInMins = 0;
	dword	timerStartTimeInMins = 0, timerDurationTimeInMins = 0;
	int	numberOfTimers = 0;
	int	i = 0;
	bool	found = FALSE;
	TYPE_TimerInfo	timerInfo;

	schShowResults[schShowResultIndex].timerSet = FALSE;

	numberOfTimers = TAP_Timer_GetTotalNum();

	resultStartTimeInMins = (((schShowResults[schShowResultIndex].startTime >> 16) & 0xFFFF) * 24 * 60) + (((schShowResults[schShowResultIndex].startTime >> 8) & 0xFF) * 60) + (schShowResults[schShowResultIndex].startTime & 0xFF);
	resultDurationTimeInMins = (((schShowResults[schShowResultIndex].duration >> 8) & 0xFF) * 60) + (schShowResults[schShowResultIndex].duration & 0xFF);

	searchStartPaddingInMins = (((schUserData[schShowSearchIndex].searchStartPadding >> 8) & 0xFF) * 60) + (schUserData[schShowSearchIndex].searchStartPadding & 0xFF);
	searchEndPaddingInMins = (((schUserData[schShowSearchIndex].searchEndPadding >> 8) & 0xFF) * 60) + (schUserData[schShowSearchIndex].searchEndPadding & 0xFF);

	for(i = 0; ((i < numberOfTimers) && (found == FALSE)); i++ )
	{
		if(TAP_Timer_GetInfo(i, &timerInfo ))
		{
			timerStartTimeInMins = (((timerInfo.startTime >> 16) & 0xFFFF) * 24 * 60) + (((timerInfo.startTime >> 8) & 0xFF) * 60) + (timerInfo.startTime & 0xFF);
			timerDurationTimeInMins = (((timerInfo.duration >> 8) & 0xFF) * 60) + (timerInfo.duration & 0xFF);

			if
			(
				(timerStartTimeInMins == (resultStartTimeInMins - searchStartPaddingInMins))
				&&
				(timerDurationTimeInMins == (resultDurationTimeInMins + searchStartPaddingInMins + searchEndPaddingInMins))
				&&
				(timerInfo.svcNum == schShowResults[schShowResultIndex].svcNum)
//				&&
// must enable this !!!!!	(timerInfo.svcType == schShowResults[schShowResultIndex].svcType)
			)
			{
				schShowResults[schShowResultIndex].timerSet = TRUE;

				schShowResults[schShowResultIndex].isRec = timerInfo.isRec;

				found = TRUE;
			}
		}
	}
}



void schShowSortResults(int sortOrder)
{
	int	i = 0, j = 0;
	schShowResultsStruct	*tempResult = NULL;

	for(i = 0; i < (schShowNumberOfResults - 1) ; i++)
	{
		for(j = (schShowNumberOfResults - 1); j > i; j--)
		{
			switch(sortOrder)
			{
			/*--------------------------------------------------*/
			case SCH_SHOW_SORT_ORDER_TIME:
//schShowResultsPtr
				if(schShowResultsPtr[j]->startTime < schShowResultsPtr[j-1]->startTime)
				{
					tempResult = schShowResultsPtr[j];
					schShowResultsPtr[j] = schShowResultsPtr[j-1];
					schShowResultsPtr[j-1] = tempResult;
				}
				else if(schShowResultsPtr[j]->startTime == schShowResultsPtr[j-1]->startTime)
				{
					if(schShowResultsPtr[j]->svcNum < schShowResultsPtr[j-1]->svcNum)
					{
						tempResult = schShowResultsPtr[j];
						schShowResultsPtr[j] = schShowResultsPtr[j-1];
						schShowResultsPtr[j-1] = tempResult;
					}
				}
				else
				{
				}

				break;
			/*--------------------------------------------------*/
			case SCH_SHOW_SORT_ORDER_CHANNEL:

				if(schShowResultsPtr[j]->svcNum < schShowResultsPtr[j-1]->svcNum)
				{
					tempResult = schShowResultsPtr[j];
					schShowResultsPtr[j] = schShowResultsPtr[j-1];
					schShowResultsPtr[j-1] = tempResult;
				}
				else if(schShowResultsPtr[j]->svcNum == schShowResultsPtr[j-1]->svcNum)
				{
					if(schShowResultsPtr[j]->startTime < schShowResultsPtr[j-1]->startTime)
					{
						tempResult = schShowResultsPtr[j];
						schShowResultsPtr[j] = schShowResultsPtr[j-1];
						schShowResultsPtr[j-1] = tempResult;
					}
				}
				else
				{
				}

				break;
			/*--------------------------------------------------*/
			case SCH_SHOW_SORT_ORDER_NAME:

				if(strcmp(schShowResultsPtr[j]->name, schShowResultsPtr[j-1]->name) < 0)
				{
					tempResult = schShowResultsPtr[j];
					schShowResultsPtr[j] = schShowResultsPtr[j-1];
					schShowResultsPtr[j-1] = tempResult;
				}

				break;
			/*--------------------------------------------------*/
			default:

				break;
			/*--------------------------------------------------*/
			}
		}
	}
}


