/************************************************************
Part of the ukEPG project
This module edits the schedules

v0.0 sl8:	20-11-05	Inception date
v0.1 sl8	02-12-05	No longer highlights the 'To' cell on the channel and time lines. Also 'Start' and 'End' on padding line.
				Use number keys to enter start/end time and padding.
v0.2: sl8	20-01-06	Modified for TAP_SDK. All variables initialised.
v0.3: sl8	16-02-06	Short cut added. Record key saves search (if valid).
				SearchFolder element added to the main search structure (not used yet)
v0.4: sl8	09-03-06	Allow user to choose a destination folder. Removed debug info. Show selected days info
v0.5: sl8	11-04-06	Show window added and tidy up.
v0.6: sl8	19-04-06	Corrected positoning of screen. Changed colour of info area.

**************************************************************/

#include "ChannelSelection.c"
#include "Keyboard.c"

void schEditCloseWindow(void);
void schEditDrawCellText(int,int,byte,int,bool,char*);
void schEditTimeKeyHandler(dword);
void schEditPaddingKeyHandler(dword);
void schEditReturnFromKeyboard( int, byte, bool);
void schEditDrawDirect(byte, int, int, char*);
bool schEditValidateSearch(void);
void schEditSaveSearch(void);

static int schEditChosenLine = 0;
static int schEditChosenCell = 0;

static byte searchIndex = 0;

static byte schEditCellLimit[11] = {0,0,0,3,2,2,6,1,3,0,2};

static bool enableEditTime = FALSE;
static bool enableEditPadding = FALSE;

static int schEditType = 0;

struct schDataTag schEdit;

static byte channelMode = 0;       // must sort these
static byte timeMode = 0;

static int exitHour = 0;
static int exitMin = 0;

static byte attachPosition[2];
static byte attachType[2];

static byte schDirectTimePos = 0;
static byte schDirectPaddingPos = 0;


#define X1	190
#define X2	300
#define E0	53
#define E1	163
#define E2	627
#define Y	57
#define Y2	514
#define Y_STEP 30
#define SCH_EDIT_NUMBER_OF_LINES 10

#define SCH_CELL_MEDIUM 126
#define SCH_CELL_SMALL 72

#define SCH_EDIT_Y1_OFFSET	36

#define SEARCHTERM_LENGTH	30
#define SEARCHFOLDER_LENGTH	30

enum
{
	SCH_DISPLAY_CHANNEL_ANY = 0,
	SCH_DISPLAY_CHANNEL_ONLY,
	SCH_DISPLAY_CHANNEL_RANGE
};

enum
{
	SCH_DISPLAY_TIME_ANY = 0,
	SCH_DISPLAY_TIME_ONLY,
	SCH_DISPLAY_TIME_RANGE
};

enum
{
	SCH_EDIT_STATUS = 1,
	SCH_EDIT_SEARCH,
	SCH_EDIT_MATCH,
	SCH_EDIT_CHANNEL,
	SCH_EDIT_TIME,
	SCH_EDIT_DAYS,
	SCH_EDIT_PADDING,
	SCH_EDIT_ATTACH,
	SCH_EDIT_FOLDER,
	SCH_EDIT_SAVE = 10
};

enum
{
	SCH_DIRECT_EDIT_HOUR_TENS = 0,
	SCH_DIRECT_EDIT_HOUR_UNITS,
	SCH_DIRECT_EDIT_MIN_TENS,
	SCH_DIRECT_EDIT_MIN_UNITS
};


//-----------------------------------------------------------------------
//
void schEditCreateWindow(void)
{
	schEditWindowShowing = TRUE;
	sysDrawGraphicBorders();
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "Auto Schedule - Edit", TITLE_COLOUR, COLOR_Black );
}


void schEditCloseWindow(void)
{
	schEditWindowShowing = FALSE;
}



//------------
//
void schEditDrawLine(int lineNumber)
{
	char	str[80], str2[80];
	byte	month = 0, day = 0, weekDay = 0;
	byte	modifiedEditCell = 0;
	word	year = 0;
	int 	min = 0, hour = 0, endMin = 0, endHour = 0;
	dword	textColour = 0, backgroundColour = 0, textColour2 = 0;
	int	l = 0;

	TYPE_TapChInfo	currentChInfo;

	if (( lineNumber < 1 ) || ( lineNumber > SCH_EDIT_NUMBER_OF_LINES )) return;		// bound check
	
	TAP_Osd_PutGd( rgn, 53, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_rowaGd, FALSE );

	if( lineNumber != SCH_EDIT_SAVE)
	{
		TAP_Osd_PutGd( rgn, 53, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_rowadarkblueGd, FALSE );
	}

	TAP_Osd_FillBox( rgn, 53, 491, 614, 83, INFO_FILL_COLOUR );		// clear the bottom portion

	switch ( lineNumber )
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_STATUS:

		PrintCenter(rgn, E0, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1, "Status", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if( schEditChosenLine == SCH_EDIT_STATUS )			// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellbigGd, FALSE );
		}

		switch( schEdit.searchStatus )
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_USER_DATA_STATUS_DISABLED:

			PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, "Disabled", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_USER_DATA_STATUS_RECORD:

			PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, "Record", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			TAP_Osd_PutGd( rgn, 272, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_redcircleGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, 281, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E2, "R", MAIN_TEXT_COLOUR, 0 );

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_USER_DATA_STATUS_WATCH:

			PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, "Watch", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			TAP_Osd_PutGd( rgn, 272, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_greencircleGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, 279, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E2, "W", MAIN_TEXT_COLOUR, 0 );

			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_SEARCH:

		if ( schEditChosenLine == SCH_EDIT_SEARCH )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellbigGd, FALSE );
		}

		PrintCenter(rgn, E0, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1, "Search", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		TAP_SPrint(str,"\"%s\"", schEdit.searchTerm);

		TAP_Osd_PutStringAf1622(rgn, X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E2+40, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_MATCH:

		PrintCenter(rgn, E0, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1, "Match", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if( schEditChosenLine == SCH_EDIT_MATCH )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, E1 + (SCH_CELL_MEDIUM * schEditChosenCell), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
		}

		// -------------------------------------------------
		if( (schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EXACT_MATCH) == SCH_USER_DATA_OPTIONS_EXACT_MATCH)
		{
			schEditDrawCellText(lineNumber ,E1 ,0 ,SCH_CELL_MEDIUM ,(schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EVENTNAME), "Exact");
		}
		else
		{
			schEditDrawCellText(lineNumber ,E1 ,0 ,SCH_CELL_MEDIUM ,(schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EVENTNAME), "Partial");
		}

		schEditDrawCellText(lineNumber ,E1 ,1 ,SCH_CELL_MEDIUM ,(schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EVENTNAME), "Title");

		schEditDrawCellText(lineNumber ,E1 ,2 ,SCH_CELL_MEDIUM ,(schEdit.searchOptions & SCH_USER_DATA_OPTIONS_DESCRIPTION), "Description");

		schEditDrawCellText(lineNumber ,E1 ,3 ,SCH_CELL_MEDIUM ,(schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EXT_INFO), "Extended");

		// -------------------------------------------------

		TAP_Osd_FillBox( rgn, E1 + SCH_CELL_MEDIUM, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1 + (3 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_CHANNEL:														// channel name

		PrintCenter(rgn, E0, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1,  "Channel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if(schEditChosenCell == 2)
		{
			modifiedEditCell = 3;
		}
		else
		{
			modifiedEditCell = schEditChosenCell;
		}

		if( schEditChosenLine == SCH_EDIT_CHANNEL )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, E1 + (SCH_CELL_MEDIUM * modifiedEditCell), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
		}

		switch(channelMode)
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_DISPLAY_CHANNEL_RANGE:

			PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, "From", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			DisplayLogo( rgn, E1 + SCH_CELL_MEDIUM + (SCH_CELL_MEDIUM / 2) - 30, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, schEdit.searchStartSvcNum, schEdit.searchTvRadio );

			PrintCenter(rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (3 * SCH_CELL_MEDIUM), "To", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			DisplayLogo( rgn, E1 + (3 * SCH_CELL_MEDIUM) + (SCH_CELL_MEDIUM / 2) - 30, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, schEdit.searchEndSvcNum, schEdit.searchTvRadio);

			TAP_Osd_FillBox( rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
			TAP_Osd_FillBox( rgn, E1 + (3 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DISPLAY_CHANNEL_ONLY:

			PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, "Only", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			DisplayLogo( rgn, E1 + SCH_CELL_MEDIUM + (SCH_CELL_MEDIUM / 2) - 30, (lineNumber*SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, schEdit.searchStartSvcNum, schEdit.searchTvRadio );

			TAP_Osd_FillBox( rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DISPLAY_CHANNEL_ANY:

			PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, "Any", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			if(schEdit.searchTvRadio == SCH_TV)
			{
				PrintCenter(rgn, E1 + SCH_CELL_MEDIUM, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (2 * SCH_CELL_MEDIUM), "TV", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
			}
			else
			{
				PrintCenter(rgn, E1 + SCH_CELL_MEDIUM, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (2 * SCH_CELL_MEDIUM), "Radio", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
			}

			TAP_Osd_FillBox( rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

			break;
		/* ---------------------------------------------------------------------------- */
		default:

			break;
		/* ---------------------------------------------------------------------------- */
		}

		TAP_Osd_FillBox( rgn, E1 + SCH_CELL_MEDIUM, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_TIME:														// time

		PrintCenter(rgn, E0, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1, "Time", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if( schEditChosenLine == SCH_EDIT_TIME )		// highlight the current cursor line
		{
			if(schEditChosenCell == 2)
			{
				modifiedEditCell = 3;
			}
			else
			{
				modifiedEditCell = schEditChosenCell;
			}

			if (enableEditTime == TRUE)
			{
				TAP_Osd_PutGd( rgn, E1 + (SCH_CELL_MEDIUM * modifiedEditCell), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_editcellmedGd, FALSE );
			}
			else
			{
				TAP_Osd_PutGd( rgn, E1 + (SCH_CELL_MEDIUM * modifiedEditCell), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
			}
		}

		if(timeMode == SCH_DISPLAY_TIME_RANGE)
		{
			PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, "From", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			hour = (schEdit.searchStartTime & 0xff00) >> 8;
			min = (schEdit.searchStartTime & 0xff);
			if
			(
				(enableEditTime == TRUE)
				&&
				(modifiedEditCell == 1)
			)
			{
				schEditDrawDirect(schDirectTimePos,hour,min,str);
			}
			else
			{
				TAP_SPrint(str, "%02d:%02d", hour, min);
			}
			
			PrintCenter(rgn, E1 + SCH_CELL_MEDIUM, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (2 * SCH_CELL_MEDIUM), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			PrintCenter(rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (3 * SCH_CELL_MEDIUM), "To", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			hour = (schEdit.searchEndTime & 0xff00) >> 8;
			min = (schEdit.searchEndTime & 0xff);
			if
			(
				(enableEditTime == TRUE)
				&&
				(modifiedEditCell == 3)
			)
			{
				schEditDrawDirect(schDirectTimePos,hour,min,str);
			}
			else
			{
				TAP_SPrint(str, "%02d:%02d", hour, min);
			}
			PrintCenter(rgn, E1 + (3 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (4 * SCH_CELL_MEDIUM), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			TAP_Osd_FillBox( rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
			TAP_Osd_FillBox( rgn, E1 + (3 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		}
		else
		{
			PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, "Any", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
		}

		TAP_Osd_FillBox( rgn, E1 + SCH_CELL_MEDIUM, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_DAYS:														// Days

		PrintCenter(rgn, E0, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1, "Days", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if( schEditChosenLine == SCH_EDIT_DAYS )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, E1 + (SCH_CELL_SMALL * schEditChosenCell), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellsmaGd, FALSE );
		}

		schEditDrawCellText(lineNumber ,E1 ,0 ,SCH_CELL_SMALL ,(schEdit.searchDay & 0x01), "Mon");
		schEditDrawCellText(lineNumber ,E1 ,1 ,SCH_CELL_SMALL ,(schEdit.searchDay & 0x02), "Tue");
		schEditDrawCellText(lineNumber ,E1 ,2 ,SCH_CELL_SMALL ,(schEdit.searchDay & 0x04), "Wed");
		schEditDrawCellText(lineNumber ,E1 ,3 ,SCH_CELL_SMALL ,(schEdit.searchDay & 0x08), "Thu");
		schEditDrawCellText(lineNumber ,E1 ,4 ,SCH_CELL_SMALL ,(schEdit.searchDay & 0x10), "Fri");
		schEditDrawCellText(lineNumber ,E1 ,5 ,SCH_CELL_SMALL ,(schEdit.searchDay & 0x20), "Sat");
		schEditDrawCellText(lineNumber ,E1 ,6 ,SCH_CELL_SMALL ,(schEdit.searchDay & 0x40), "Sun");

		TAP_Osd_FillBox( rgn, E1+SCH_CELL_SMALL, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1+(2*SCH_CELL_SMALL), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1+(3*SCH_CELL_SMALL), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1+(4*SCH_CELL_SMALL), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1+(5*SCH_CELL_SMALL), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1+(6*SCH_CELL_SMALL), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		sprintf(str, "Selected Days:");
		if((schEdit.searchDay & 0x01) == 0x01) strcat(str," Monday");
		if((schEdit.searchDay & 0x02) == 0x02) strcat(str," Tuesday");
		if((schEdit.searchDay & 0x04) == 0x04) strcat(str," Wednesday");
		if((schEdit.searchDay & 0x08) == 0x08) strcat(str," Thursday");
		if((schEdit.searchDay & 0x10) == 0x10) strcat(str," Friday");
		if((schEdit.searchDay & 0x20) == 0x20) strcat(str," Saturday");
		if((schEdit.searchDay & 0x40) == 0x40) strcat(str," Sunday");

		TAP_Osd_PutStringAf1419( rgn, 58, 503, 666, str, INFO_COLOUR, 0 );

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_PADDING:

		PrintCenter(rgn, E0, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1, "Padding", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if( schEditChosenLine == SCH_EDIT_PADDING )		// highlight the current cursor line
		{
			if(schEditChosenCell == 0)
			{
				modifiedEditCell = 1;
			}
			else
			{
				modifiedEditCell = 3;
			}

			if (enableEditPadding == TRUE)
			{
				TAP_Osd_PutGd( rgn, E1 + (SCH_CELL_MEDIUM * modifiedEditCell), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_editcellmedGd, FALSE );
			}
			else
			{
				TAP_Osd_PutGd( rgn, E1 + (SCH_CELL_MEDIUM * modifiedEditCell), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
			}
		}

		PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, "Start", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		hour = (schEdit.searchStartPadding & 0xff00) >> 8;
		min = (schEdit.searchStartPadding & 0xff);
		if
		(
			(enableEditPadding == TRUE)
			&&
			(modifiedEditCell == 1)
		)
		{
			schEditDrawDirect(schDirectPaddingPos,hour,min,str);
		}
		else
		{
			TAP_SPrint(str, "%02d:%02d", hour, min);
		}
		PrintCenter(rgn, E1 + SCH_CELL_MEDIUM, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (2 * SCH_CELL_MEDIUM), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		PrintCenter(rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (3 * SCH_CELL_MEDIUM), "End", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		hour = (schEdit.searchEndPadding & 0xff00) >> 8;
		min = (schEdit.searchEndPadding & 0xff);
		if
		(
			(enableEditPadding == TRUE)
			&&
			(modifiedEditCell == 3)
		)
		{
			schEditDrawDirect(schDirectPaddingPos,hour,min,str);
		}
		else
		{
			TAP_SPrint(str, "%02d:%02d", hour, min);
		}
		PrintCenter(rgn, E1 + (3 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (4 * SCH_CELL_MEDIUM), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		TAP_Osd_FillBox( rgn, E1 + SCH_CELL_MEDIUM, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1 + (3 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_ATTACH:

		PrintCenter(rgn, E0, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1, "Attach", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if( schEditChosenLine == SCH_EDIT_ATTACH )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, E1 + (SCH_CELL_MEDIUM * schEditChosenCell), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
		}

		switch(attachPosition[0])
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_ATTACH_POS_NONE:

			sprintf(str,"None");
			
			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_ATTACH_POS_PREFIX:

			sprintf(str,"Prefix");
			
			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_ATTACH_POS_APPEND:

			sprintf(str,"Append");
			
			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}
		PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
		
		switch(attachType[0])
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_ATTACH_TYPE_NUMBER:

			sprintf(str,"S#");
			
			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_ATTACH_TYPE_DATE:

			sprintf(str,"Date");
			
			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}

		schEditDrawCellText(lineNumber ,E1 ,1 ,SCH_CELL_MEDIUM ,(attachPosition[0] != SCH_ATTACH_POS_NONE), str);
	

		switch(attachPosition[1])
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_ATTACH_POS_NONE:

			sprintf(str,"None");
			
			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_ATTACH_POS_PREFIX:

			sprintf(str,"Prefix");
			
			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_ATTACH_POS_APPEND:

			sprintf(str,"Append");
			
			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}
		PrintCenter(rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (3 * SCH_CELL_MEDIUM), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		switch(attachType[1])
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_ATTACH_TYPE_NUMBER:

			sprintf(str,"S#");
			
			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_ATTACH_TYPE_DATE:

			sprintf(str,"Date");
			
			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}
		schEditDrawCellText(lineNumber ,E1 ,3 ,SCH_CELL_MEDIUM ,(attachPosition[1] != SCH_ATTACH_POS_NONE), str);

		TAP_Osd_FillBox( rgn, E1 + SCH_CELL_MEDIUM, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1 + (3 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_FOLDER:

		textColour = COLOR_DarkBlue;
		if ( schEditChosenLine == SCH_EDIT_FOLDER )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellbigGd, FALSE );
			textColour = COLOR_DarkGreen;
		}

		PrintCenter(rgn, E0, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1, "Folder", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if(FirmwareCallsEnabled == TRUE)
		{
			if( TAP_Hdd_Move_Available == TRUE)
			{
				TAP_SPrint(str,"/%s/", schEdit.searchFolder);

				TAP_Osd_PutStringAf1622(rgn, X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E2+40, str, MAIN_TEXT_COLOUR, 0 );
			}
			else
			{
				TAP_Osd_PutStringAf1622(rgn, X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E2+50, "Not Available", textColour, 0 );
			}
		}
		else
		{
			TAP_Osd_PutStringAf1622(rgn, X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E2+58, "Firmware Calls Disabled", textColour, 0 );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_SAVE:

		TAP_Osd_PutGd( rgn, 53, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_rowaGd, FALSE );		// No highlight for us

		if( schEditChosenLine == SCH_EDIT_SAVE )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, E1 + (SCH_CELL_MEDIUM * schEditChosenCell), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
		}

		PrintCenter(rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + SCH_CELL_MEDIUM, "Save", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		PrintCenter(rgn, E1 + (1 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (2 * SCH_CELL_MEDIUM), "Cancel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		PrintCenter(rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, E1 + (3 * SCH_CELL_MEDIUM), "Delete", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
		
		TAP_Osd_FillBox( rgn, E1 + SCH_CELL_MEDIUM, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1 + (2 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, E1 + (3 * SCH_CELL_MEDIUM), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}

	TAP_Osd_FillBox( rgn, E1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );
}


void schEditDrawDirect(byte position, int hour, int min, char *str)
{
	switch ( position )
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_DIRECT_EDIT_HOUR_TENS:

		TAP_SPrint(str, "%02d:%02d", hour, min);

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_DIRECT_EDIT_HOUR_UNITS:

		TAP_SPrint(str, "%1d-:--", hour/10);

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_DIRECT_EDIT_MIN_TENS:

		TAP_SPrint(str, "%02d:--", hour);

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_DIRECT_EDIT_MIN_UNITS:

		TAP_SPrint(str, "%02d:%1d-", hour, (min/10));

		break;
	/* ---------------------------------------------------------------------------- */
	}		
}

void schEditDrawCellText(int lineNumber ,int cellPosX ,byte cell ,int cellSize ,bool cellEnabled, char *text)
{
	int textColour = 0;

	textColour = MAIN_TEXT_COLOUR;
	if( cellEnabled == FALSE)
	{
		if(( schEditChosenLine == lineNumber ) && ( schEditChosenCell == cell))
		{
			textColour = COLOR_DarkGreen;
		}
		else
		{
			textColour = COLOR_DarkBlue;
		}
	}
	PrintCenter(rgn, cellPosX + (cell * cellSize), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, cellPosX + (cell * cellSize) + cellSize, text, textColour, 0, FNT_Size_1622 );
}




//------------
//
void CopySearchFields(int index)
{
	char buffer1[128];

	schEdit = schUserData[index];

	if(schEdit.searchStartTime == schEdit.searchEndTime)
	{
		timeMode = SCH_DISPLAY_TIME_ANY;

		schEditCellLimit[SCH_EDIT_TIME] = 0;
	}
	else
	{
		timeMode = SCH_DISPLAY_TIME_RANGE;

		schEditCellLimit[SCH_EDIT_TIME] = 2;
	}

	if((schEdit.searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
	{
		channelMode = SCH_DISPLAY_CHANNEL_ANY;

		schEditCellLimit[SCH_EDIT_CHANNEL] = 1;
	}
	else if(schEdit.searchStartSvcNum == schEdit.searchEndSvcNum)
	{
		channelMode = SCH_DISPLAY_CHANNEL_ONLY;

		schEditCellLimit[SCH_EDIT_CHANNEL] = 1;
	}
	else
	{
		channelMode = SCH_DISPLAY_CHANNEL_RANGE;

		schEditCellLimit[SCH_EDIT_CHANNEL] = 2;

	}

	attachPosition[0] = (schEdit.searchAttach >> 6) & 0x03;
	attachType[0] = schEdit.searchAttach & 0x3F;
	attachPosition[1] = (schEdit.searchAttach >> 14) & 0x03;
	attachType[1] = (schEdit.searchAttach >> 8) & 0x3F;
}


//------------
//
bool schEditWindowActivate (int index, int type)
{
	int i = 0;

	schEditType = type;

	schEditChosenLine = SCH_EDIT_SAVE;
	schEditChosenCell = 1;

	schDirectTimePos = SCH_DIRECT_EDIT_HOUR_TENS;
	schDirectPaddingPos = SCH_DIRECT_EDIT_HOUR_TENS;

	switch (schEditType)
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_EXISTING_SEARCH:

		searchIndex = index;
		CopySearchFields(searchIndex);

		schEditCreateWindow();
		UpdateListClock();

		for ( i=1; i <= SCH_EDIT_NUMBER_OF_LINES ; i++)
		{
		    schEditDrawLine(i);
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_NEW_SEARCH:
	default:

		schEdit.searchStatus = 1;

		schEdit.searchOptions = 0;
		schEdit.searchOptions |= SCH_USER_DATA_OPTIONS_EXACT_MATCH;
		schEdit.searchOptions |= SCH_USER_DATA_OPTIONS_EVENTNAME;

		memset(schEdit.searchTerm,0,132);
		memset(schEdit.searchFolder,0,132);

		schEdit.searchStartTime = 0;
		schEdit.searchEndTime = 0;
		timeMode = SCH_DISPLAY_TIME_ANY;
		schEditCellLimit[SCH_EDIT_TIME] = 0;

		schEdit.searchStartSvcNum = 0;
		schEdit.searchEndSvcNum = 0;
		schEdit.searchStartPadding = 0;
		schEdit.searchEndPadding = 0;
		schEdit.searchAttach = 0;
		schEdit.searchDay = 0x7F;

		schEdit.searchOptions |= SCH_USER_DATA_OPTIONS_ANY_CHANNEL;
		channelMode = SCH_DISPLAY_CHANNEL_ANY;
		schEdit.searchTvRadio = SCH_TV;
		schEditCellLimit[SCH_EDIT_CHANNEL] = 1;

		attachPosition[0] = 0;
		attachType[0] = 0;
		attachPosition[1] = 0;
		attachType[1] = 0;

		schEditCreateWindow();
		UpdateListClock();
		
		for ( i = 1; i <= SCH_EDIT_NUMBER_OF_LINES ; i++)
		{
			schEditDrawLine(i);
		}

		break;
	/* ---------------------------------------------------------------------------- */
	}
}


void schEditRefresh( void )
{
	int i = 0;
	
	schEditCreateWindow();
	UpdateListClock();

	for ( i = 1; i <= SCH_EDIT_NUMBER_OF_LINES; i++)
	{
		schEditDrawLine(i);
	}
}


void ReturnFromKeyboardSearchTerm( char *str, bool success)
{
	if ( success == TRUE)
	{
		strcpy( schEdit.searchTerm, str );
	}

	schEditDrawLine( SCH_EDIT_SEARCH );
}																		// no need to update display as keyboard.c has scheduled a global redisplay

void ReturnFromKeyboardSearchFolder( char *str, bool success)
{
	if ( success == TRUE)
	{
		strcpy( schEdit.searchFolder, str );
	}

	schEditDrawLine( SCH_EDIT_FOLDER );
}																		// no need to update display as keyboard.c has scheduled a global redisplay

void schEditReturnFromKeyboard( int selectedLogo, byte selectedSvc, bool success)
{
	if ( success == TRUE)
	{
		if( schEditChosenCell == 1)
		{
			schEdit.searchStartSvcNum = selectedLogo;
			schEdit.searchTvRadio = selectedSvc;
		}
		else
		{
			schEdit.searchEndSvcNum = selectedLogo;
			schEdit.searchTvRadio = selectedSvc;
		}
	}
}																		// no need to update display as keyboard.c has scheduled a global redisplay


//------------
//
void EditLineKeyHandler(dword key)
{
	int	maxTvChannels = 0, maxRadioChannels = 0, max = 0;
	int	newSvcNum = 0;
	int	i = 0, result = 0;
	TYPE_TapChInfo	currentChInfo;
	word 	mjd = 0;
	int 	min = 0, hour = 0, duration = 0;
	byte	chosenDay = 0;
	char	str[256];

	switch ( schEditChosenLine )
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_STATUS:

		switch ( key )
		{
		/* ---------------------------------------------------------------------------- */
		case RKEY_Ok:

			switch( schEdit.searchStatus )
			{
			/* ---------------------------------------------------------------------------- */
			case SCH_USER_DATA_STATUS_DISABLED:

				schEdit.searchStatus = SCH_USER_DATA_STATUS_RECORD;

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_USER_DATA_STATUS_RECORD:

				schEdit.searchStatus = SCH_USER_DATA_STATUS_WATCH;

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_USER_DATA_STATUS_WATCH:
			default:

				schEdit.searchStatus = SCH_USER_DATA_STATUS_DISABLED;

				break;
			/* ---------------------------------------------------------------------------- */
			}

			schEditDrawLine( schEditChosenLine );

			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;				
		/* ---------------------------------------------------------------------------- */
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_SEARCH:

		if( key == RKEY_Ok )
		{
			ActivateKeyboard( schEdit.searchTerm, SEARCHTERM_LENGTH, &ReturnFromKeyboardSearchTerm );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_MATCH:

		switch ( key )
		{
		case RKEY_Ok:
			switch( schEditChosenCell )
			{
			case 0:
				if( (schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EXACT_MATCH) == SCH_USER_DATA_OPTIONS_EXACT_MATCH ) 
				{
					schEdit.searchOptions &= ~SCH_USER_DATA_OPTIONS_EXACT_MATCH;
				}
				else
				{
					schEdit.searchOptions |= SCH_USER_DATA_OPTIONS_EXACT_MATCH;
				}

				break;
			case 1:
				if( (schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EVENTNAME) == SCH_USER_DATA_OPTIONS_EVENTNAME ) 
				{
					schEdit.searchOptions &= ~SCH_USER_DATA_OPTIONS_EVENTNAME;
				}
				else
				{
					schEdit.searchOptions |= SCH_USER_DATA_OPTIONS_EVENTNAME;
				}

				break;
			case 2:
				if( (schEdit.searchOptions & SCH_USER_DATA_OPTIONS_DESCRIPTION) == SCH_USER_DATA_OPTIONS_DESCRIPTION ) 
				{
					schEdit.searchOptions &= ~SCH_USER_DATA_OPTIONS_DESCRIPTION;
				}
				else
				{
					schEdit.searchOptions |= SCH_USER_DATA_OPTIONS_DESCRIPTION;
				}

				break;
			case 3:
				if( (schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EXT_INFO) == SCH_USER_DATA_OPTIONS_EXT_INFO ) 
				{
					schEdit.searchOptions &= ~SCH_USER_DATA_OPTIONS_EXT_INFO;
				}
				else
				{
					schEdit.searchOptions |= SCH_USER_DATA_OPTIONS_EXT_INFO;
				}

				break;
			default:

				break;
			}
			
			schEditDrawLine( schEditChosenLine );

			break;
		default:
			break;
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_CHANNEL:

		switch ( key )
		{
		/* ---------------------------------------------------------------------------- */
		case RKEY_Ok:

			switch( schEditChosenCell )
			{
			/* ---------------------------------------------------------------------------- */
			case 0:
				switch( channelMode )
				{
				/* ---------------------------------------------------------------------------- */
				case SCH_DISPLAY_CHANNEL_ANY:

					channelMode = SCH_DISPLAY_CHANNEL_ONLY;

					schEditCellLimit[SCH_EDIT_CHANNEL] = 1;

					break;
				/* ---------------------------------------------------------------------------- */
				case SCH_DISPLAY_CHANNEL_ONLY:

					channelMode = SCH_DISPLAY_CHANNEL_RANGE;

					schEditCellLimit[SCH_EDIT_CHANNEL] = 2;

					break;
				/* ---------------------------------------------------------------------------- */
				case SCH_DISPLAY_CHANNEL_RANGE:
				default:
					channelMode = SCH_DISPLAY_CHANNEL_ANY;

					schEditCellLimit[SCH_EDIT_CHANNEL] = 1;

					break;
				/* ---------------------------------------------------------------------------- */
				}
				
				schEditDrawLine( schEditChosenLine );

				break;
			/* ---------------------------------------------------------------------------- */
			case 1:
				switch( channelMode )
				{
				/* ---------------------------------------------------------------------------- */
				case SCH_DISPLAY_CHANNEL_ANY:

					if(schEdit.searchTvRadio == SCH_TV)
					{
						schEdit.searchTvRadio = SCH_RADIO;
					}
					else
					{
						schEdit.searchTvRadio = SCH_TV;
					}

					schEditDrawLine( schEditChosenLine );

					break;
				/* ---------------------------------------------------------------------------- */
				default:

					DisplayChannelListWindow(schEdit.searchStartSvcNum, schEdit.searchTvRadio, &schEditReturnFromKeyboard);

					break;
				/* ---------------------------------------------------------------------------- */
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case 2:
				DisplayChannelListWindow(schEdit.searchEndSvcNum, schEdit.searchTvRadio, &schEditReturnFromKeyboard);

				break;
			/* ---------------------------------------------------------------------------- */
			default:

				break;
			/* ---------------------------------------------------------------------------- */
			}

			break;
		/* ---------------------------------------------------------------------------- */
		case RKEY_TvRadio:
// what happens if this is pressed and start/end channel is ok for tv but not applicable for radio??????
/*
			if(schEdit.searchTvRadio == SCH_TV)
			{
				schEdit.searchTvRadio = SCH_RADIO;
			}
			else
			{
				schEdit.searchTvRadio = SCH_TV;
			}
*/
			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_TIME:

		switch ( key )
		{
		/* ---------------------------------------------------------------------------- */
		case RKEY_Ok:

			switch( schEditChosenCell )
			{
			/* ---------------------------------------------------------------------------- */
			case 0:
				if( timeMode == SCH_DISPLAY_TIME_RANGE )
				{
					timeMode = SCH_DISPLAY_TIME_ANY;

					schEditCellLimit[SCH_EDIT_TIME] = 0;
				}
				else
				{
					timeMode = SCH_DISPLAY_TIME_RANGE;

					schEditCellLimit[SCH_EDIT_TIME] = 2;
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case 1:
				exitHour = (schEdit.searchStartTime & 0xff00) >> 8;
				exitMin = (schEdit.searchStartTime & 0xff);

				schDirectTimePos = SCH_DIRECT_EDIT_HOUR_TENS;

				enableEditTime = TRUE;

				break;
			/* ---------------------------------------------------------------------------- */
			case 2:
				exitHour = (schEdit.searchEndTime & 0xff00) >> 8;
				exitMin = (schEdit.searchEndTime & 0xff);

				schDirectTimePos = SCH_DIRECT_EDIT_HOUR_TENS;

				enableEditTime = TRUE;

				break;
			/* ---------------------------------------------------------------------------- */
			default:

				break;
			/* ---------------------------------------------------------------------------- */
			}
			
			schEditDrawLine( schEditChosenLine );

			break;
		/* ---------------------------------------------------------------------------- */
		default:

			break;
		/* ---------------------------------------------------------------------------- */
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_DAYS :														// days

		switch ( key )
		{
		/* ---------------------------------------------------------------------------- */
		case RKEY_Ok:

			chosenDay = 0x01 << schEditChosenCell;

			if((schEdit.searchDay & chosenDay) == chosenDay)
			{
				schEdit.searchDay &= ~chosenDay;
			}
			else
			{
				schEdit.searchDay |= chosenDay;
			}

			schEditDrawLine( schEditChosenLine );

			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_PADDING:

		switch ( key )
		{
		/* ---------------------------------------------------------------------------- */
		case RKEY_Ok:

			switch( schEditChosenCell )
			{
			/* ---------------------------------------------------------------------------- */
			case 0:
				exitHour = (schEdit.searchStartPadding & 0xff00) >> 8;
				exitMin = (schEdit.searchStartPadding & 0xff);

				schDirectPaddingPos = SCH_DIRECT_EDIT_HOUR_TENS;

				enableEditPadding = TRUE;

				break;
			/* ---------------------------------------------------------------------------- */
			case 1:
				exitHour = (schEdit.searchEndPadding & 0xff00) >> 8;
				exitMin = (schEdit.searchEndPadding & 0xff);

				schDirectPaddingPos = SCH_DIRECT_EDIT_HOUR_TENS;

				enableEditPadding = TRUE;

				break;
			/* ---------------------------------------------------------------------------- */
			default:

				break;
			/* ---------------------------------------------------------------------------- */
			}
			
			schEditDrawLine( schEditChosenLine );

			break;
		/* ---------------------------------------------------------------------------- */
		default:

			break;
		/* ---------------------------------------------------------------------------- */
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_ATTACH:

		switch ( key )
		{
		/* ---------------------------------------------------------------------------- */
		case RKEY_Ok:

			switch( schEditChosenCell )
			{
			/* ---------------------------------------------------------------------------- */
			case 0:
				if( attachPosition[0] == SCH_ATTACH_POS_NONE )
				{
					attachPosition[0] = SCH_ATTACH_POS_PREFIX;
				}
				else if( attachPosition[0] == SCH_ATTACH_POS_PREFIX )
				{
					attachPosition[0] = SCH_ATTACH_POS_APPEND;
				}
				else
				{
					attachPosition[0] = SCH_ATTACH_POS_NONE;
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case 1:
				if( attachType[0] == SCH_ATTACH_TYPE_NUMBER )
				{
					attachType[0] = SCH_ATTACH_TYPE_DATE;
				}
				else
				{
					attachType[0] = SCH_ATTACH_TYPE_NUMBER;
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case 2:
				if( attachPosition[1] == SCH_ATTACH_POS_NONE )
				{
					attachPosition[1] = SCH_ATTACH_POS_PREFIX;
				}
				else if( attachPosition[1] == SCH_ATTACH_POS_PREFIX )
				{
					attachPosition[1] = SCH_ATTACH_POS_APPEND;
				}
				else
				{
					attachPosition[1] = SCH_ATTACH_POS_NONE;
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case 3:
				if( attachType[1] == SCH_ATTACH_TYPE_NUMBER )
				{
					attachType[1] = SCH_ATTACH_TYPE_DATE;
				}
				else
				{
					attachType[1] = SCH_ATTACH_TYPE_NUMBER;
				}

				break;
			/* ---------------------------------------------------------------------------- */
			default:

				break;
			/* ---------------------------------------------------------------------------- */
			}
			
			schEditDrawLine( schEditChosenLine );

			break;
		/* ---------------------------------------------------------------------------- */
		default:

			break;
		/* ---------------------------------------------------------------------------- */
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_FOLDER:

		if
		(
			( key == RKEY_Ok )
			&&
			( FirmwareCallsEnabled == TRUE )
			&&
			( TAP_Hdd_Move_Available == TRUE)
		)
		{
			ActivateKeyboard( schEdit.searchFolder, SEARCHFOLDER_LENGTH, &ReturnFromKeyboardSearchFolder );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_SAVE:														// Record or program

		switch ( key )
		{
		/* ---------------------------------------------------------------------------- */
		case RKEY_Ok:

			switch( schEditChosenCell )
			{
			/* ---------------------------------------------------------------------------- */
			case 0:					/* Save */

				if( schEditValidateSearch() == TRUE )
				{
					schEditSaveSearch();
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case 1:					/* Cancel */
				schEditCloseWindow();		// Close the edit window
				returnFromEdit = TRUE;		// will cause a redraw of search list

				break;
			/* ---------------------------------------------------------------------------- */
			case 2:					/* Delete */
				if
				(
					(schMainTotalValidSearches > 0)
					&&
					(schEditType == SCH_EXISTING_SEARCH)
				)
				{
					if(searchIndex != schMainTotalValidSearches)
					{
						for(i = searchIndex; i < (schMainTotalValidSearches - 1); i++)
						{
							schUserData[i] = schUserData[i + 1];
						}
					}

					schMainTotalValidSearches--;

					schDispSaveToFile = TRUE;
				}

				schEditCloseWindow();		// Close the edit window
				returnFromEdit = TRUE;		// will cause a redraw of search list

				break;
			/* ---------------------------------------------------------------------------- */
			default:

				break;
			/* ---------------------------------------------------------------------------- */
			}

			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}

		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}
}



//------------
//
bool schEditValidateSearch(void)
{
	bool valid = TRUE;

	if
	(
		(strlen(schEdit.searchTerm) == 0) 
		||
		(strlen(schEdit.searchTerm) > 128) 
	)
	{
		valid = FALSE;
	}

	if(schEdit.searchDay == 0)
	{
		valid = FALSE;
	}

	if((schEdit.searchOptions & (SCH_USER_DATA_OPTIONS_EVENTNAME + SCH_USER_DATA_OPTIONS_DESCRIPTION + SCH_USER_DATA_OPTIONS_EXT_INFO)) == 0)
	{
		valid = FALSE;
	}
	
	if
	(
		(schEdit.searchStartSvcNum > schEdit.searchEndSvcNum)
		&&
		(channelMode == SCH_DISPLAY_CHANNEL_RANGE)
	)
	{
		valid = FALSE;
	}
	
	return valid;
}



//------------
//
void schEditSaveSearch(void)
{
	if( schEditType == SCH_NEW_SEARCH )
	{
		schMainTotalValidSearches++;

		searchIndex = schMainTotalValidSearches - 1;

		schDispChosenLine = schMainTotalValidSearches;
	}

	schUserData[searchIndex].searchStatus = schEdit.searchStatus;
	strcpy(schUserData[searchIndex].searchTerm, schEdit.searchTerm);
	schUserData[searchIndex].searchDay = schEdit.searchDay;
	schUserData[searchIndex].searchOptions = schEdit.searchOptions;
	strcpy(schUserData[searchIndex].searchFolder, schEdit.searchFolder);

	if(timeMode == SCH_DISPLAY_TIME_ANY)
	{
		schUserData[searchIndex].searchStartTime = 0;
		schUserData[searchIndex].searchEndTime = 0;
	}
	else
	{
		schUserData[searchIndex].searchStartTime = schEdit.searchStartTime;
		schUserData[searchIndex].searchEndTime = schEdit.searchEndTime;
	}

	if(channelMode == SCH_DISPLAY_CHANNEL_ANY)
	{
		schUserData[searchIndex].searchStartSvcNum = 0;
		schUserData[searchIndex].searchEndSvcNum = 0;

		schUserData[searchIndex].searchOptions |= SCH_USER_DATA_OPTIONS_ANY_CHANNEL;
	}
	else if (channelMode == SCH_DISPLAY_CHANNEL_ONLY)
	{
		schUserData[searchIndex].searchStartSvcNum = schEdit.searchStartSvcNum;
		schUserData[searchIndex].searchEndSvcNum = schEdit.searchStartSvcNum;

		schUserData[searchIndex].searchOptions &= ~SCH_USER_DATA_OPTIONS_ANY_CHANNEL;
	}
	else
	{
		schUserData[searchIndex].searchStartSvcNum = schEdit.searchStartSvcNum;
		schUserData[searchIndex].searchEndSvcNum = schEdit.searchEndSvcNum;

		schUserData[searchIndex].searchOptions &= ~SCH_USER_DATA_OPTIONS_ANY_CHANNEL;
	}

	schUserData[searchIndex].searchTvRadio = schEdit.searchTvRadio;

	schUserData[searchIndex].searchStartPadding = schEdit.searchStartPadding;
	schUserData[searchIndex].searchEndPadding = schEdit.searchEndPadding;

	schUserData[searchIndex].searchAttach = 0;
	schUserData[searchIndex].searchAttach |= (attachPosition[0] << 6) & 0xC0;
	schUserData[searchIndex].searchAttach |= attachType[0] & 0x3F;
	schUserData[searchIndex].searchAttach |= (attachPosition[1] << 14) & 0xC000;
	schUserData[searchIndex].searchAttach |= (attachType[1] << 8) & 0x3F00;

	schDispSaveToFile = TRUE;

	schEditCloseWindow();		// Close the edit window
	returnFromEdit = TRUE;		// will cause a redraw of search list
}

//------------
//
void schEditKeyHandler(dword key)
{
	int i = 0, oldEditLine = 0;

	oldEditLine = schEditChosenLine;

	if ( channelListWindowShowing ) { ChannelListKeyHandler( key ); return; }	// handle channel list edit
	if ( enableEditTime ) { schEditTimeKeyHandler( key ); return; }			// handle Time Edit
	if ( enableEditPadding ) { schEditPaddingKeyHandler( key ); return; }		// handle Padding Edit
	if ( keyboardWindowShowing ) { KeyboardKeyHandler( key ); return; }		// handle calendar

	switch ( key )
	{
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChDown:

		if ( schEditChosenLine < SCH_EDIT_NUMBER_OF_LINES )
		{
			schEditChosenLine++;

			if(schEditChosenCell > schEditCellLimit[schEditChosenLine])
			{
				schEditChosenCell = schEditCellLimit[schEditChosenLine];
			}

			schEditDrawLine(oldEditLine);
			schEditDrawLine(schEditChosenLine);
		}
		else
		{
			schEditChosenLine = 1;				// 0=hidden - can't hide once cursor moved
			if(schEditChosenCell > schEditCellLimit[schEditChosenLine])
			{
				schEditChosenCell = schEditCellLimit[schEditChosenLine];
			}

			schEditDrawLine(SCH_EDIT_NUMBER_OF_LINES);
			schEditDrawLine(schEditChosenLine);
		}
		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChUp:
		
		if ( schEditChosenLine > 1 )				// 0=hidden - can't hide once cursor moved
		{
			schEditChosenLine--;

			if(schEditChosenCell > schEditCellLimit[schEditChosenLine])
			{
				schEditChosenCell = schEditCellLimit[schEditChosenLine];
			}
			schEditDrawLine(oldEditLine);
			schEditDrawLine(schEditChosenLine);
		}
		else
		{
			schEditChosenLine = SCH_EDIT_NUMBER_OF_LINES;
			schEditChosenCell = 1;
			schEditDrawLine(1);
			schEditDrawLine(schEditChosenLine);
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Forward:

		if ( searchIndex < schMainTotalValidSearches )				// load next timer
		{
			searchIndex++;
			CopySearchFields(searchIndex);

			for ( i = 1; i <= SCH_EDIT_NUMBER_OF_LINES; i++)		// redraw all fields
			{
				schEditDrawLine(i);
			}
      		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Rewind:
		
		if ( searchIndex > 0 )				// load previous timer
		{
			searchIndex--;
			CopySearchFields(searchIndex);

			for ( i = 1; i <= SCH_EDIT_NUMBER_OF_LINES; i++)	// redraw all fields
			{
				schEditDrawLine(i);
			}
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

		schEditChosenLine = key - RKEY_0;			// direct keyboard selection of any line
		if(schEditChosenCell > schEditCellLimit[schEditChosenLine])
		{
			schEditChosenCell = schEditCellLimit[schEditChosenLine];
		}
		schEditDrawLine( oldEditLine );
		schEditDrawLine( schEditChosenLine );

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_0:

		schEditChosenLine = 10;				// make "0" select the last line
		if(schEditChosenCell > schEditCellLimit[schEditChosenLine])
		{
			schEditChosenCell = schEditCellLimit[schEditChosenLine];
		}
		schEditDrawLine( oldEditLine );
		schEditDrawLine( schEditChosenLine );

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Ok :
	case RKEY_TvRadio :

		EditLineKeyHandler(key);

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolUp :

		if(schEditChosenCell < schEditCellLimit[schEditChosenLine])
		{
			schEditChosenCell++;
		}
		schEditDrawLine( schEditChosenLine );

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolDown : 

		if(schEditChosenCell > 0)
		{
			schEditChosenCell--;
		}
		schEditDrawLine( schEditChosenLine );

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Exit:

		schEditCloseWindow();					// Close the edit window
		returnFromEdit = TRUE;					// will cause a redraw of timer list

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Record:

		if( schEditValidateSearch() == TRUE )
		{
			schEditSaveSearch();
		}

		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}
}


void schEditTimeKeyHandler(dword key)
{
	int	min = 0, hour = 0, temp = 0;

	if(schEditChosenCell == 1)
	{
		hour = (schEdit.searchStartTime & 0xff00) >> 8;		// extract the time
		min = (schEdit.searchStartTime & 0xff);
	}
	else
	{
		hour = (schEdit.searchEndTime & 0xff00) >> 8;		// extract the time
		min = (schEdit.searchEndTime & 0xff);
	}

	switch ( key )
	{
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolUp:					// Time up

		if( schDirectTimePos == SCH_DIRECT_EDIT_HOUR_TENS)
		{
			if ( min < 59 )
			{
				min++;
			}
			else
			{
				if ( hour < 23 )
				{
					min = 0;
					hour++;
				}
				else
				{
					min = 0;
					hour = 0;
				}
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolDown:					// Time down

		if( schDirectTimePos == SCH_DIRECT_EDIT_HOUR_TENS)
		{
			if ( min > 0 )
			{
				min--;
			}
			else
			{
				if ( hour > 0 )
				{
					min = 59;
					hour--;
				}
				else
				{
					min = 59;
					hour = 23;
				}
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChUp:

		if( schDirectTimePos == SCH_DIRECT_EDIT_HOUR_TENS)
		{
			if ( hour < 23 )
			{
				hour++;
			}
			else
			{
				hour = 0;
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChDown:

		if( schDirectTimePos == SCH_DIRECT_EDIT_HOUR_TENS)
		{
			if ( hour > 0 )
			{
				hour--;
			}
			else
			{
				hour = 23;
			}
		}
		
		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_0:
	case RKEY_1:
	case RKEY_2:
	case RKEY_3:
	case RKEY_4:
	case RKEY_5:
	case RKEY_6:
	case RKEY_7:
	case RKEY_8:
	case RKEY_9:

		switch ( schDirectTimePos )
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_DIRECT_EDIT_HOUR_TENS:

			temp = (key - RKEY_0) * 10;

			if(temp <= 20)
			{
				hour = temp;
				min = 0;

				schDirectTimePos = SCH_DIRECT_EDIT_HOUR_UNITS;
			}

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DIRECT_EDIT_HOUR_UNITS:

			temp = hour + (key - RKEY_0);

			if(temp <= 23)
			{
				hour = temp;

				schDirectTimePos = SCH_DIRECT_EDIT_MIN_TENS;
			}

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DIRECT_EDIT_MIN_TENS:

			temp = (key - RKEY_0) * 10;

			if(temp <= 50)
			{
				min = temp;

				schDirectTimePos = SCH_DIRECT_EDIT_MIN_UNITS;
			}

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DIRECT_EDIT_MIN_UNITS:

			temp = min + (key - RKEY_0);

			if(temp <= 59)
			{
				min = temp;

				schDirectTimePos = SCH_DIRECT_EDIT_HOUR_TENS;
			}

			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}
		
		break;	
	/* ---------------------------------------------------------------------------- */
	case RKEY_Ok:

		if( schDirectTimePos != SCH_DIRECT_EDIT_HOUR_TENS)
		{
			hour = exitHour;
			min = exitMin;
		}

		enableEditTime = FALSE;

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Exit:

		hour = exitHour;
		min = exitMin;

		enableEditTime = FALSE;
	
		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}

	if(schEditChosenCell == 1)
	{
		schEdit.searchStartTime = ((hour & 0xff) << 8) | (min & 0xff);
	}
	else
	{
		schEdit.searchEndTime = ((hour & 0xff) << 8) | (min & 0xff);
	}

	schEditDrawLine( schEditChosenLine );
}



void schEditPaddingKeyHandler(dword key)
{
	int 	min = 0, hour = 0, temp = 0;

	if(schEditChosenCell == 0)
	{
		hour = (schEdit.searchStartPadding & 0xff00) >> 8;		// extract the padding
		min = (schEdit.searchStartPadding & 0xff);
	}
	else
	{
		hour = (schEdit.searchEndPadding & 0xff00) >> 8;		// extract the padding
		min = (schEdit.searchEndPadding & 0xff);
	}

	switch ( key )
	{
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolUp:					// Time up

		if( schDirectPaddingPos == SCH_DIRECT_EDIT_HOUR_TENS)
		{
			if
			(
				( min < 59 )
				&&
				( hour == 0)
			)
			{
				min++;
			}
			else
			{
				min = 0;
				hour = 1;
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolDown:					// Time down

		if( schDirectPaddingPos == SCH_DIRECT_EDIT_HOUR_TENS)
		{
			if ( min > 0 )
			{
				min--;
			}
			else
			{
				if ( hour > 0 )
				{
					min = 59;
					hour--;
				}
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChUp:

		if( schDirectPaddingPos == SCH_DIRECT_EDIT_HOUR_TENS)
		{
			if
			(
				( min < 50 )
				&&
				( hour == 0)
			)
			{
				min += 10;
			}
			else
			{
				hour = 1;

				min = 0;
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChDown:

		if( schDirectPaddingPos == SCH_DIRECT_EDIT_HOUR_TENS)
		{
			if ( min >= 10 )
			{
				min -= 10;
			}
			else
			{
				if ( hour > 0)
				{
					hour = 0;
					min = 50;
				}
				else
				{
					min = 0;
				}
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_0:
	case RKEY_1:
	case RKEY_2:
	case RKEY_3:
	case RKEY_4:
	case RKEY_5:
	case RKEY_6:
	case RKEY_7:
	case RKEY_8:
	case RKEY_9:

		switch ( schDirectPaddingPos )
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_DIRECT_EDIT_HOUR_TENS:

			temp = (key - RKEY_0) * 10;

			if(temp <= 0)
			{
				hour = temp;
				min = 0;

				schDirectPaddingPos = SCH_DIRECT_EDIT_HOUR_UNITS;
			}

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DIRECT_EDIT_HOUR_UNITS:

			temp = hour + (key - RKEY_0);

			if(temp <= 1)
			{
				hour = temp;

				schDirectPaddingPos = SCH_DIRECT_EDIT_MIN_TENS;
			}

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DIRECT_EDIT_MIN_TENS:

			temp = (key - RKEY_0) * 10;

			if
			(
				(temp == 0)
				||
				(
					(hour == 0)
					&&
					(temp <= 50)
				)
			)
			{
				min = temp;

				schDirectPaddingPos = SCH_DIRECT_EDIT_MIN_UNITS;
			}

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DIRECT_EDIT_MIN_UNITS:

			temp = min + (key - RKEY_0);

			if
			(
				(temp == 0)
				||
				(
					(hour == 0)
					&&
					(temp <= 59)
				)
			)
			{
				min = temp;

				schDirectPaddingPos = SCH_DIRECT_EDIT_HOUR_TENS;
			}

			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}
		
		break;	
	/* ---------------------------------------------------------------------------- */
	case RKEY_Ok:

		if( schDirectPaddingPos != SCH_DIRECT_EDIT_HOUR_TENS)
		{
			hour = exitHour;
			min = exitMin;
		}

		enableEditPadding = FALSE;

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Exit:

		hour = exitHour;
		min = exitMin;

		enableEditPadding = FALSE;
	
		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}

	if(schEditChosenCell == 0)
	{
		schEdit.searchStartPadding = ((hour & 0xff) << 8) | (min & 0xff);
	}
	else
	{
		schEdit.searchEndPadding = ((hour & 0xff) << 8) | (min & 0xff);
	}

	schEditDrawLine( schEditChosenLine );
}





//------------
//
void schEditWindowInitialise(void)
{
	schEditWindowShowing = FALSE;
	timeEditExitWindowShowing = FALSE;
	
	channelListWindowShowing = FALSE;
	calendarWindowShowing = FALSE;

	schEditChosenLine = 0;
	schEditChosenCell = 0;

	returnFromEdit = FALSE;

	InitialiseKeyboard();
}



