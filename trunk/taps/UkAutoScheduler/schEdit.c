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
v0.7: sl8	08-05-06	API move added.
v0.8: sl8	05-08-06	Keep added, Yes/No Box used.
v0.9: sl8	28-09-06	Do not allow 'move' and 'keep' if unable to determine 'changeDir' type.
v0.10: sl8	11-10-06	Copy search term to folder. Allow user to create folder if it doesn't exist.

**************************************************************/

#include "ChannelSelection.c"
#include "Keyboard.c"

void schEditCloseWindow(void);
void schEditDrawCellText(int,int,byte,int,bool,char*);
void schEditTimeKeyHandler(dword);
void schEditPaddingKeyHandler(dword);
void schEditKeepKeyHandler(dword);
void schEditReturnFromKeyboard( int, byte, bool);
void schEditDrawDirect(byte, int, int, char*);
bool schEditValidateSearch(void);
bool schEditValidateFolder(void);
void schEditSaveSearch(void);
void schEditDrawLegend(void);
void schEditReturnFromExitYesNo(bool);
void schEditReturnFromDeleteYesNo(bool);
void schEditReturnFromFolderWarningYesNo(bool);

static int schEditChosenLine = 0;
static int schEditChosenCell = 0;
static int schEditTopLine = 1;

static byte searchIndex = 0;

static byte schEditCellLimit[14] = {0,0,0,3,2,2,6,1,3,0,0,0,0,2};

static bool enableEditTime = FALSE;
static bool enableEditPadding = FALSE;
static bool enableEditKeep = FALSE;

static int schEditType = 0;

struct schDataTag schEdit;

static byte channelMode = 0;       // must sort these
static byte timeMode = 0;

static int exitHour = 0;
static int exitMin = 0;
static int exitKeep = 0;

static byte attachPosition[2];
static byte attachType[2];

static byte schDirectTimePos = 0;
static byte schDirectPaddingPos = 0;
static byte schDirectKeepPos = 0;

static bool schEditModified = FALSE;

#define SCH_EDIT_DIVIDER_X0	53
#define SCH_EDIT_DIVIDER_X1	85
#define SCH_EDIT_DIVIDER_X2	191
#define SCH_EDIT_DIVIDER_X3	627

#define Y_STEP 30

#define SCH_EDIT_NUMBER_OF_DISPLAYED_OPTIONS	10
#define SCH_EDIT_TOTAL_NUMBER_OF_OPTIONS	10

#define SCH_EDIT_CELL_MEDIUM		117
#define SCH_EDIT_CELL_SMALL		66
#define SCH_EDIT_CELL_BORDER_WIDTH		2
#define SCH_EDIT_X2_BORDER_WIDTH	3

#define SCH_EDIT_Y1_OFFSET	36

#define SEARCHTERM_LENGTH	30
#define SEARCHFOLDER_LENGTH	30

#define SCH_EDIT_KEEP_VALUE_DEFAULT	10

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
	SCH_DISPLAY_KEEP_ALL = 0,
	SCH_DISPLAY_KEEP_LAST,
	SCH_DISPLAY_KEEP_SMALLEST,
	SCH_DISPLAY_KEEP_LARGEST,
	SCH_DISPLAY_KEEP_DAYS
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
	SCH_EDIT_KEEP
};

enum
{
	SCH_DIRECT_EDIT_HOUR_TENS = 0,
	SCH_DIRECT_EDIT_HOUR_UNITS,
	SCH_DIRECT_EDIT_MIN_TENS,
	SCH_DIRECT_EDIT_MIN_UNITS
};

enum
{
	SCH_DIRECT_EDIT_KEEP_TENS = 0,
	SCH_DIRECT_EDIT_KEEP_UNITS
};

//-----------------------------------------------------------------------
//
void schEditCreateWindow(void)
{
	schEditWindowShowing = TRUE;
	sysDrawGraphicBorders();
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "Edit Search", TITLE_COLOUR, COLOR_Black );
}


void schEditCloseWindow(void)
{
	schEditWindowShowing = FALSE;
}



//------------
//
void schEditDrawLine(int option)
{
	char	str[80], str2[80];
	byte	month = 0, day = 0, weekDay = 0;
	byte	modifiedEditCell = 0;
	word	year = 0;
	int 	min = 0, hour = 0, endMin = 0, endHour = 0;
	dword	textColour = 0, backgroundColour = 0, textColour2 = 0;
	int	i = 0;
	int	lineNumber;
	TYPE_TapChInfo	currentChInfo;

	lineNumber = option - schEditTopLine + 1;

	if( lineNumber < 1 )
	{
		return;		// bound check
	}

	TAP_Osd_PutGd( rgn, 53, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_rowaGd, FALSE );

	TAP_Osd_PutGd( rgn, 53, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_rowadarkblueGd, FALSE );

	memset(str,0,80);
	TAP_SPrint(str,"%d", option);
	PrintCenter(rgn, SCH_EDIT_DIVIDER_X0, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X1, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	TAP_Osd_FillBox( rgn, 53, 490, 497, 83, INFO_FILL_COLOUR );		// clear the bottom portion

	switch ( option )
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_STATUS:

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2, "Status", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if( schEditChosenLine == SCH_EDIT_STATUS )			// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_X2_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellbigGd, FALSE );
		}

		switch( schEdit.searchStatus )
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_USER_DATA_STATUS_DISABLED:

			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM + 40, "Search Only", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			if( schEditChosenLine == SCH_EDIT_STATUS )
			{
				sprintf(str, "Search Only: Timers will not be automatically set.");
				TAP_Osd_PutStringAf1419( rgn, 58, 503, 666, str, INFO_COLOUR, 0 );
			}

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_USER_DATA_STATUS_RECORD:

			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM, "Record", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + 101, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_redcircleGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, SCH_EDIT_DIVIDER_X2 + 111, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X3, "R", MAIN_TEXT_COLOUR, 0 );

			if( schEditChosenLine == SCH_EDIT_STATUS )
			{
				sprintf(str, "Record: Automatically set 'Record' timers.");
				TAP_Osd_PutStringAf1419( rgn, 58, 503, 666, str, INFO_COLOUR, 0 );
			}

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_USER_DATA_STATUS_WATCH:

			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2  + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM, "Watch", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + 101, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_greencircleGd, TRUE );
			TAP_Osd_PutStringAf1622( rgn, SCH_EDIT_DIVIDER_X2 + 109, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X3, "W", MAIN_TEXT_COLOUR, 0 );

			if( schEditChosenLine == SCH_EDIT_STATUS )
			{
				sprintf(str, "Watch: Automatically set 'Watch' timers. ");
				TAP_Osd_PutStringAf1419( rgn, 58, 503, 666, str, INFO_COLOUR, 0 );
			}

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
			TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_X2_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellbigGd, FALSE );
		}

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2, "Search", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		TAP_SPrint(str,"\"%s\"", schEdit.searchTerm);

		TAP_Osd_PutStringAf1622(rgn, SCH_EDIT_DIVIDER_X2 + 27, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X3+40, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_MATCH:

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2, "Match", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if( schEditChosenLine == SCH_EDIT_MATCH )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + ((SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH) * schEditChosenCell) + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
		}

		// -------------------------------------------------
		if( (schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EXACT_MATCH) == SCH_USER_DATA_OPTIONS_EXACT_MATCH)
		{
			schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH ,0 ,SCH_EDIT_CELL_MEDIUM ,(schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EVENTNAME), "Exact");
		}
		else
		{
			schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH ,0 ,SCH_EDIT_CELL_MEDIUM ,(schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EVENTNAME), "Partial");
		}

		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH,1 ,SCH_EDIT_CELL_MEDIUM ,(schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EVENTNAME), "Title");

		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH,2 ,SCH_EDIT_CELL_MEDIUM ,(schEdit.searchOptions & SCH_USER_DATA_OPTIONS_DESCRIPTION), "Description");

		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH,3 ,SCH_EDIT_CELL_MEDIUM ,(schEdit.searchOptions & SCH_USER_DATA_OPTIONS_EXT_INFO), "Extended");

		// -------------------------------------------------

		for(i = 1; i < 4; i++)
		{
			TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (i * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_CHANNEL:														// channel name

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2,  "Channel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

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
			TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + ((SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH) * modifiedEditCell) + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
		}

		switch(channelMode)
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_DISPLAY_CHANNEL_RANGE:

			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (0 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "From", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			DisplayLogo( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)) + (SCH_EDIT_CELL_MEDIUM / 2) - 30, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, schEdit.searchStartSvcNum, schEdit.searchTvRadio );

			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "To", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			DisplayLogo( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)) + (SCH_EDIT_CELL_MEDIUM / 2) - 30, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, schEdit.searchEndSvcNum, schEdit.searchTvRadio);

			TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
			TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DISPLAY_CHANNEL_ONLY:

			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (0 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "Only", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			DisplayLogo( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)) + (SCH_EDIT_CELL_MEDIUM / 2) - 30, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, schEdit.searchStartSvcNum, schEdit.searchTvRadio );

			TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DISPLAY_CHANNEL_ANY:

			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (0 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "Any", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			if(schEdit.searchTvRadio == SCH_TV)
			{
				PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "TV", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
			}
			else
			{
				PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "Radio", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
			}

			TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

			break;
		/* ---------------------------------------------------------------------------- */
		default:

			break;
		/* ---------------------------------------------------------------------------- */
		}

		TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_TIME:														// time

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2, "Time", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

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
				TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + ((SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH) * modifiedEditCell) + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_editcellmedGd, FALSE );
			}
			else
			{
				TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + ((SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH) * modifiedEditCell) + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
			}
		}

		if(timeMode == SCH_DISPLAY_TIME_RANGE)
		{
			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (0 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "From", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

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
			
			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "To", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

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
			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (4 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
			TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		}
		else
		{
			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (0 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "Any", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
		}

		TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_DAYS:														// Days

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2, "Days", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if( schEditChosenLine == SCH_EDIT_DAYS )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + ((SCH_EDIT_CELL_SMALL + SCH_EDIT_CELL_BORDER_WIDTH) * schEditChosenCell) + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellsmaGd, FALSE );
		}

		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH,0 ,SCH_EDIT_CELL_SMALL ,(schEdit.searchDay & 0x01), "Mon");
		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH,1 ,SCH_EDIT_CELL_SMALL ,(schEdit.searchDay & 0x02), "Tue");
		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH,2 ,SCH_EDIT_CELL_SMALL ,(schEdit.searchDay & 0x04), "Wed");
		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH,3 ,SCH_EDIT_CELL_SMALL ,(schEdit.searchDay & 0x08), "Thu");
		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH,4 ,SCH_EDIT_CELL_SMALL ,(schEdit.searchDay & 0x10), "Fri");
		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH,5 ,SCH_EDIT_CELL_SMALL ,(schEdit.searchDay & 0x20), "Sat");
		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH,6 ,SCH_EDIT_CELL_SMALL ,(schEdit.searchDay & 0x40), "Sun");

		for(i = 1; i < 7; i++)
		{
			TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (i * (SCH_EDIT_CELL_SMALL + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		}

		if( schEditChosenLine == SCH_EDIT_DAYS )
		{
			sprintf(str, "Selected Days:");
			if((schEdit.searchDay & 0x01) == 0x01) strcat(str," Mon");
			if((schEdit.searchDay & 0x02) == 0x02) strcat(str," Tue");
			if((schEdit.searchDay & 0x04) == 0x04) strcat(str," Wed");
			if((schEdit.searchDay & 0x08) == 0x08) strcat(str," Thu");
			if((schEdit.searchDay & 0x10) == 0x10) strcat(str," Fri");
			if((schEdit.searchDay & 0x20) == 0x20) strcat(str," Sat");
			if((schEdit.searchDay & 0x40) == 0x40) strcat(str," Sun");

			TAP_Osd_PutStringAf1419( rgn, 58, 503, 666, str, INFO_COLOUR, 0 );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_PADDING:

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2, "Padding", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

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
				TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + ((SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH) * modifiedEditCell) + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_editcellmedGd, FALSE );
			}
			else
			{
				TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + ((SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH) * modifiedEditCell) + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
			}
		}

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (0 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "Start", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

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
		PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "End", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

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
		PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (4 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_ATTACH:

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2, "Attach", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if( schEditChosenLine == SCH_EDIT_ATTACH )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + ((SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH) * schEditChosenCell) + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
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
		PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
		
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
		case SCH_ATTACH_TYPE_TIME:

			sprintf(str,"Time");
			
			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}

		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 ,1 ,SCH_EDIT_CELL_MEDIUM ,(attachPosition[0] != SCH_ATTACH_POS_NONE), str);
	

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
		PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

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
		case SCH_ATTACH_TYPE_TIME:

			sprintf(str,"Time");
			
			break;
		/* ---------------------------------------------------------------------------- */
		default:
			break;
		/* ---------------------------------------------------------------------------- */
		}
		schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 ,3 ,SCH_EDIT_CELL_MEDIUM ,(attachPosition[1] != SCH_ATTACH_POS_NONE), str);

		TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_FOLDER:

		textColour = COLOR_DarkBlue;
		if ( schEditChosenLine == SCH_EDIT_FOLDER )		// highlight the current cursor line
		{
			TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_X2_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellbigGd, FALSE );
			textColour = COLOR_DarkGreen;
		}

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2, "Folder", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if(FirmwareCallsEnabled == TRUE)
		{
			if
			(
				(
					( schMainApiMoveAvailable == TRUE)
					||
					( schMainDebugMoveAvailable == TRUE)
				)
				&&
				(schMainChangeDirAvailable == TRUE)
			)
			{
				TAP_SPrint(str,"/%s/", schEdit.searchFolder);

				TAP_Osd_PutStringAf1622(rgn, SCH_EDIT_DIVIDER_X2 + 27, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X3, str, MAIN_TEXT_COLOUR, 0 );
			}
			else
			{
				TAP_Osd_PutStringAf1622(rgn, SCH_EDIT_DIVIDER_X2 + 27, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X3, "Not Available", textColour, 0 );
			}
		}
		else
		{
			TAP_Osd_PutStringAf1622(rgn, SCH_EDIT_DIVIDER_X2 + 27, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X3, "Firmware Calls Disabled", textColour, 0 );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_KEEP:

		PrintCenter(rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2, "Keep", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if(schEditChosenCell == 2)
		{
			modifiedEditCell = 3;
		}
		else
		{
			modifiedEditCell = schEditChosenCell;
		}

		if( schEditChosenLine == SCH_EDIT_KEEP )		// highlight the current cursor line
		{
			if (enableEditKeep == TRUE)
			{
				TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + ((SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH) * modifiedEditCell) + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_editcellmedGd, FALSE );
			}
			else
			{
				TAP_Osd_PutGd( rgn, SCH_EDIT_DIVIDER_X2 + ((SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH) * modifiedEditCell) + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, &_highlightcellmedGd, FALSE );
			}
		}


		if
		(
			(schEdit.searchKeepMode != SCH_DISPLAY_KEEP_ALL)
			&&
			(schMainChangeDirAvailable == TRUE)
		)
		{
			switch(schEdit.searchKeepMode)
			{
			/* ---------------------------------------------------------------------------- */
			case SCH_DISPLAY_KEEP_LAST:

				PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH, "Last", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_DISPLAY_KEEP_SMALLEST:

				PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH, "Smallest", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_DISPLAY_KEEP_LARGEST:

				PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH, "Largest", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_DISPLAY_KEEP_DAYS:
			default:

				PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH, "For", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				break;
			/* ---------------------------------------------------------------------------- */
			}

			TAP_SPrint(str, "%d", schEdit.searchKeepValue);
			
			PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (1 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			if(schEdit.searchKeepValue == 1)
			{
				if(schEdit.searchKeepMode == SCH_DISPLAY_KEEP_DAYS)
				{
					PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "Day", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
				}
				else
				{
					PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "Programme", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
				}
			}
			else
			{
				if(schEdit.searchKeepMode == SCH_DISPLAY_KEEP_DAYS)
				{
					PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "Days", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
				}
				else
				{
					PrintCenter(rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_BORDER_WIDTH + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), "Programmes", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
				}
			}

			TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (2 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
			TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + (3 * (SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH)), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
		}
		else
		{
			schEditDrawCellText(lineNumber ,SCH_EDIT_DIVIDER_X2 ,0 ,SCH_EDIT_CELL_MEDIUM, ((strlen(schEdit.searchFolder) > 0) && (schMainChangeDirAvailable == TRUE)), "All");
		}

		TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2 + SCH_EDIT_CELL_MEDIUM + SCH_EDIT_CELL_BORDER_WIDTH, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, SCH_EDIT_CELL_BORDER_WIDTH, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators

		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}

	TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X1, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );	// draw the column seperators
	TAP_Osd_FillBox( rgn, SCH_EDIT_DIVIDER_X2, (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET - 8, 3, SYS_Y1_STEP, FILL_COLOUR );
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
	PrintCenter(rgn, cellPosX + (cell * cellSize) + (cell * SCH_EDIT_CELL_BORDER_WIDTH), (lineNumber * SYS_Y1_STEP) + SCH_EDIT_Y1_OFFSET, cellPosX + (cell * cellSize) + (cell * SCH_EDIT_CELL_BORDER_WIDTH) + cellSize, text, textColour, 0, FNT_Size_1622 );
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

	if(schEdit.searchKeepMode == SCH_DISPLAY_KEEP_ALL)
	{
		schEditCellLimit[SCH_EDIT_KEEP] = 0;
	}
	else
	{
		schEditCellLimit[SCH_EDIT_KEEP] = 1;
	}
}


//------------
//
bool schEditWindowActivate (int index, int type)
{
	int i = 0;

	schEditType = type;

	schEditChosenLine = 0;
	schEditTopLine = 1;
	schEditChosenCell = 0;

	schDirectTimePos = SCH_DIRECT_EDIT_HOUR_TENS;
	schDirectPaddingPos = SCH_DIRECT_EDIT_HOUR_TENS;

	switch (schEditType)
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_EXISTING_SEARCH:

		schEditModified = FALSE;

		searchIndex = index;
		CopySearchFields(searchIndex);

		schEditCreateWindow();
		UpdateListClock();
		schEditDrawLegend();

		for ( i=1; i <= SCH_EDIT_NUMBER_OF_DISPLAYED_OPTIONS ; i++)
		{
		    schEditDrawLine(i);
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_NEW_SEARCH:
	default:
		schEditModified = TRUE;

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

		schEdit.searchKeepMode = SCH_DISPLAY_KEEP_ALL;
		schEdit.searchKeepValue = SCH_EDIT_KEEP_VALUE_DEFAULT;

		schEditCreateWindow();
		UpdateListClock();
		schEditDrawLegend();
		
		for ( i = 1; i <= SCH_EDIT_NUMBER_OF_DISPLAYED_OPTIONS ; i++)
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
	schEditDrawLegend();

	for ( i = 1; i <= SCH_EDIT_NUMBER_OF_DISPLAYED_OPTIONS; i++)
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

		if(strlen(str) == 0)
		{
			schEdit.searchKeepMode = SCH_DISPLAY_KEEP_ALL;
			schEdit.searchKeepValue = SCH_EDIT_KEEP_VALUE_DEFAULT;
			schEditCellLimit[SCH_EDIT_KEEP] = 0;
		}
	}

	schEditDrawLine( SCH_EDIT_FOLDER );
	schEditDrawLine( SCH_EDIT_KEEP );
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
}															// no need to update display as keyboard.c has scheduled a global redisplay


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
				if( attachPosition[0] != SCH_ATTACH_POS_NONE )
				{
					if( attachType[0] == SCH_ATTACH_TYPE_NUMBER )
					{
						attachType[0] = SCH_ATTACH_TYPE_DATE;
					}
					else if ( attachType[0] == SCH_ATTACH_TYPE_DATE )
					{
						attachType[0] = SCH_ATTACH_TYPE_TIME;
					}
					else
					{
						attachType[0] = SCH_ATTACH_TYPE_NUMBER;
					}
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
				if( attachPosition[1] != SCH_ATTACH_POS_NONE )
				{
					if( attachType[1] == SCH_ATTACH_TYPE_NUMBER )
					{
						attachType[1] = SCH_ATTACH_TYPE_DATE;
					}
					else if ( attachType[1] == SCH_ATTACH_TYPE_DATE )
					{
						attachType[1] = SCH_ATTACH_TYPE_TIME;
					}
					else
					{
						attachType[1] = SCH_ATTACH_TYPE_NUMBER;
					}
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
			(
				( schMainApiMoveAvailable == TRUE)
				||
				( schMainDebugMoveAvailable == TRUE)
			)
			&&
			( schMainChangeDirAvailable == TRUE )
		)
		{
			ActivateKeyboard( schEdit.searchFolder, SEARCHFOLDER_LENGTH, &ReturnFromKeyboardSearchFolder );
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_EDIT_KEEP:

		if(strlen(schEdit.searchFolder) > 0)
		{
			switch ( key )
			{
			/* ---------------------------------------------------------------------------- */
			case RKEY_Ok:

				switch( schEditChosenCell )
				{
				/* ---------------------------------------------------------------------------- */
				case 0:
					switch( schEdit.searchKeepMode )
					{
					/* ---------------------------------------------------------------------------- */
					case SCH_DISPLAY_KEEP_ALL:

						schEdit.searchKeepMode = SCH_DISPLAY_KEEP_LAST;

						schEditCellLimit[SCH_EDIT_KEEP] = 1;

						break;
					/* ---------------------------------------------------------------------------- */
					case SCH_DISPLAY_KEEP_LAST:

						schEdit.searchKeepMode = SCH_DISPLAY_KEEP_SMALLEST;

						schEditCellLimit[SCH_EDIT_KEEP] = 1;

						break;
					/* ---------------------------------------------------------------------------- */
					case SCH_DISPLAY_KEEP_SMALLEST:

						schEdit.searchKeepMode = SCH_DISPLAY_KEEP_LARGEST;

						schEditCellLimit[SCH_EDIT_KEEP] = 1;

						break;
					/* ---------------------------------------------------------------------------- */
					case SCH_DISPLAY_KEEP_LARGEST:
						schEdit.searchKeepMode = SCH_DISPLAY_KEEP_DAYS;

						schEditCellLimit[SCH_EDIT_KEEP] = 1;

						break;
					/* ---------------------------------------------------------------------------- */
					case SCH_DISPLAY_KEEP_DAYS:
					default:
						schEdit.searchKeepMode = SCH_DISPLAY_KEEP_ALL;

						schEditCellLimit[SCH_EDIT_KEEP] = 0;

						break;
					/* ---------------------------------------------------------------------------- */
					}
					
					break;
				/* ---------------------------------------------------------------------------- */
				case 1:
					exitKeep = schEdit.searchKeepValue;

					schDirectKeepPos = SCH_DIRECT_EDIT_KEEP_TENS;

					enableEditKeep = TRUE;

					break;
				/* ---------------------------------------------------------------------------- */
				default:

					break;
				/* ---------------------------------------------------------------------------- */
				}

				schEditDrawLine( schEditChosenLine );

				break;
			}
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


bool schEditValidateFolder(void)
{
	char folderStr[132];
	bool valid = TRUE;

	if
	(
		( FirmwareCallsEnabled == TRUE )
		&&
		(
			( schMainApiMoveAvailable == TRUE)
			||
			( schMainDebugMoveAvailable == TRUE)
		)
		&&
		(schMainChangeDirAvailable == TRUE)
	)
	{
		if(strlen(schEdit.searchFolder) > 0)
		{
			sprintf( folderStr, "DataFiles/%s", schEdit.searchFolder );
			valid = GotoPath(folderStr);
		}
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

	schUserData[searchIndex].searchKeepMode = schEdit.searchKeepMode;
	schUserData[searchIndex].searchKeepValue = schEdit.searchKeepValue;

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
	if ( enableEditKeep ) { schEditKeepKeyHandler( key ); return; }			// handle Keep Edit
	if ( keyboardWindowShowing ) { KeyboardKeyHandler( key ); return; }		// handle calendar

	switch ( key )
	{
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChDown:

		if ( schEditChosenLine < SCH_EDIT_TOTAL_NUMBER_OF_OPTIONS )
		{
			schEditChosenLine++;

			if(schEditChosenCell > schEditCellLimit[schEditChosenLine])
			{
				schEditChosenCell = schEditCellLimit[schEditChosenLine];
			}

			if ( schEditChosenLine > (schEditTopLine + SCH_EDIT_NUMBER_OF_DISPLAYED_OPTIONS - 1) )
			{
				schEditTopLine++;

				for ( i = schEditTopLine; i <= schEditChosenLine; i++)
				{
					schEditDrawLine(i);
				}
			}
			else
			{
				schEditDrawLine(oldEditLine);
				schEditDrawLine(schEditChosenLine);
			}
		}
		else		// wrap around
		{
			schEditChosenLine = 1;				// 0=hidden - can't hide once cursor moved
			if(schEditChosenCell > schEditCellLimit[schEditChosenLine])
			{
				schEditChosenCell = schEditCellLimit[schEditChosenLine];
			}

			if(schEditTopLine != 1)
			{
				schEditTopLine = 1;

				for ( i = 1; i <= SCH_EDIT_NUMBER_OF_DISPLAYED_OPTIONS; i++)
				{
					schEditDrawLine(i);
				}
			}
			else
			{
				schEditDrawLine(oldEditLine);
				schEditDrawLine(schEditChosenLine);
			}
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

			if(schEditChosenLine < schEditTopLine)
			{
				schEditTopLine--;

				for ( i = schEditTopLine; i <= (schEditTopLine + SCH_EDIT_NUMBER_OF_DISPLAYED_OPTIONS - 1) ; i++)
				{
					schEditDrawLine(i);
				}
			}
			else
			{
				schEditDrawLine(oldEditLine);
				schEditDrawLine(schEditChosenLine);
			}
		}
		else
		{
			schEditChosenLine = SCH_EDIT_TOTAL_NUMBER_OF_OPTIONS;

			if(schEditChosenCell > schEditCellLimit[schEditChosenLine])
			{
				schEditChosenCell = schEditCellLimit[schEditChosenLine];
			}

			if(schEditTopLine != 1)
			{
				schEditTopLine = SCH_EDIT_TOTAL_NUMBER_OF_OPTIONS + 1 - SCH_EDIT_NUMBER_OF_DISPLAYED_OPTIONS;

				for ( i = schEditTopLine; i <= schEditChosenLine; i++)
				{
					schEditDrawLine(i);
				}			
			}
			else
			{
				schEditDrawLine(oldEditLine);
				schEditDrawLine(schEditChosenLine);
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Forward:

		if ( searchIndex < schMainTotalValidSearches )				// load next timer
		{
			searchIndex++;
			CopySearchFields(searchIndex);

			for ( i = 1; i <= SCH_EDIT_NUMBER_OF_DISPLAYED_OPTIONS; i++)		// redraw all fields
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

			for ( i = 1; i <= SCH_EDIT_NUMBER_OF_DISPLAYED_OPTIONS; i++)	// redraw all fields
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

		schEditModified = TRUE;

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

		if(schEditModified == TRUE)
		{
			DisplayYesNoWindow("Exit without saving", "Schedule has changed", "Do you want to exit without saving?", "Yes", "Cancel", 0, &schEditReturnFromExitYesNo );
		}
		else
		{
			schEditCloseWindow();					// Close the edit window
			returnFromEdit = TRUE;					// will cause a redraw of timer list
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Record:

		if(schEditValidateFolder() == FALSE)
		{
			DisplayYesNoWindow("Warning!", "Destination folder does not exist", "Would you like to create the folder?", "Yes", "No", 0, &schEditReturnFromFolderWarningYesNo );
		}
		else if( schEditValidateSearch() == TRUE )
		{
			schEditSaveSearch();
		}
		else
		{
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_White:					/* Delete */

		if
		(
			(schMainTotalValidSearches > 0)
			&&
			(schEditType == SCH_EXISTING_SEARCH)
		)
		{
			DisplayYesNoWindow("Schedule Delete Confirmation", "Do you want to delete this schedule?", "", "Yes", "No", 1, &schEditReturnFromDeleteYesNo );
		}
		else
		{
			schEditCloseWindow();		// Close the edit window
			returnFromEdit = TRUE;		// will cause a redraw of search list
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Slow:					/* Copy */

		if
		(
			( schEditChosenLine == SCH_EDIT_FOLDER )
			&&
			(strlen(schEdit.searchTerm) > 0)
			&&
			( FirmwareCallsEnabled == TRUE )
			&&
			(
				( schMainApiMoveAvailable == TRUE)
				||
				( schMainDebugMoveAvailable == TRUE)
			)
								&&
			(schMainChangeDirAvailable == TRUE)

		)
		{
			memset(schEdit.searchFolder,0,132);

			strcpy(schEdit.searchFolder, schEdit.searchTerm);

			schEditDrawLine( SCH_EDIT_FOLDER );
			schEditDrawLine( SCH_EDIT_KEEP );
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


void schEditKeepKeyHandler(dword key)
{
	int	editKeep = 1;

	editKeep = schEdit.searchKeepValue;

	switch ( key )
	{
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolUp:					// Time up

		if( schDirectKeepPos == SCH_DIRECT_EDIT_KEEP_TENS)
		{
			if ( editKeep < 99 )
			{
				editKeep++;
			}
			else
			{
				editKeep = 1;
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_VolDown:					// Time down

		if( schDirectKeepPos == SCH_DIRECT_EDIT_KEEP_TENS)
		{
			if ( editKeep > 1 )
			{
				editKeep--;
			}
			else
			{
				editKeep = 99;
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChUp:

		if( schDirectKeepPos == SCH_DIRECT_EDIT_KEEP_TENS)
		{
			if((editKeep + 10) < 100)
			{
				editKeep += 10;
			}
			else
			{
				editKeep = editKeep + 9 - 99;
			}
		}

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_ChDown:

		if( schDirectKeepPos == SCH_DIRECT_EDIT_KEEP_TENS)
		{
			if ( editKeep >= 11 )
			{
				editKeep -= 10;
			}
			else
			{
				editKeep = editKeep + 99 - 9;

				if(editKeep > 99)
				{
					editKeep = 99;
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

		switch ( schDirectKeepPos )
		{
		/* ---------------------------------------------------------------------------- */
		case SCH_DIRECT_EDIT_KEEP_TENS:

			editKeep = (key - RKEY_0) * 10;

			schDirectKeepPos = SCH_DIRECT_EDIT_KEEP_UNITS;

			break;
		/* ---------------------------------------------------------------------------- */
		case SCH_DIRECT_EDIT_KEEP_UNITS:

			editKeep += (key - RKEY_0);

			schDirectKeepPos = SCH_DIRECT_EDIT_KEEP_TENS;

			if(editKeep == 0)
			{
				editKeep = 1;
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

		if( schDirectKeepPos != SCH_DIRECT_EDIT_KEEP_TENS)
		{
			editKeep = exitKeep;
		}

		enableEditKeep = FALSE;

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Exit:

		editKeep = exitKeep;

		enableEditKeep = FALSE;
	
		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}

	schEdit.searchKeepValue = editKeep;

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

// Dimensions of Information Window
#define INFO_AREA_X 53
#define INFO_AREA_Y 490
#define INFO_AREA_W 614  // Total width of list window.
#define INFO_AREA_H 83

// Dimensions for instructions on Info Window
#define INSTR_AREA_W 116
#define INSTR_AREA_X 550
#define INSTR_AREA_Y INFO_AREA_Y
#define INSTR_AREA_H 83

// Coordinates for the text in the Information Window
#define INFO_TEXT_X (INFO_AREA_X + 5)
#define INFO_TEXT_Y (INFO_AREA_Y + 3)
#define INFO_TEXT_W (INFO_AREA_W - INSTR_AREA_W)  //666
#define INFO_TEXT_H 25  // Height of a single text row.


void schEditDrawLegend(void)
{
	TAP_Osd_FillBox( rgn, INFO_AREA_X, INFO_AREA_Y, INFO_AREA_W, INFO_AREA_H, INFO_FILL_COLOUR );		// clear the bottom portion

	TAP_Osd_PutGd( rgn, INSTR_AREA_X, INSTR_AREA_Y + 1, &_exitoval38x19Gd, TRUE );	
	TAP_Osd_PutStringAf1419( rgn, INSTR_AREA_X + 50, INSTR_AREA_Y + 2, INSTR_AREA_X + INSTR_AREA_W, "Cancel", INFO_COLOUR, INFO_FILL_COLOUR );

	TAP_Osd_PutGd( rgn, INSTR_AREA_X, INSTR_AREA_Y + 21, &_recordoval38x19Gd, TRUE );	
	TAP_Osd_PutStringAf1419( rgn, INSTR_AREA_X + 50, INSTR_AREA_Y + 22, INSTR_AREA_X + INSTR_AREA_W, "Save", INFO_COLOUR, INFO_FILL_COLOUR );

	TAP_Osd_PutGd( rgn, INSTR_AREA_X, INSTR_AREA_Y + 41, &_whiteoval38x19Gd, TRUE );
	TAP_Osd_PutStringAf1419( rgn, INSTR_AREA_X + 50, INSTR_AREA_Y + 42, INSTR_AREA_X + INSTR_AREA_W, "Delete", INFO_COLOUR, INFO_FILL_COLOUR );

}	


void schEditReturnFromExitYesNo( bool result)
{
	switch (result)
	{
	/* ---------------------------------------------------------------------------- */
	case TRUE:
		schEditCloseWindow();					// Close the edit window
		returnFromEdit = TRUE;					// will cause a redraw of timer list

		break;	// YES
	/* ---------------------------------------------------------------------------- */
	default:
	case FALSE:
	
		break;	// NO
	/* ---------------------------------------------------------------------------- */
	}   
}


void schEditReturnFromDeleteYesNo( bool result)
{
	int i = 0;

	switch (result)
	{
	/* ---------------------------------------------------------------------------- */
	case TRUE:
		if(searchIndex != schMainTotalValidSearches)
		{
			for(i = searchIndex; i < (schMainTotalValidSearches - 1); i++)
			{
				schUserData[i] = schUserData[i + 1];
			}
		}

		schMainTotalValidSearches--;

		schDispSaveToFile = TRUE;

		schEditCloseWindow();		// Close the edit window
		returnFromEdit = TRUE;		// will cause a redraw of search list

		break;	// YES
	/* ---------------------------------------------------------------------------- */
	default:
	case FALSE:
	
		break;	// NO
	/* ---------------------------------------------------------------------------- */
	}   
}


void schEditReturnFromFolderWarningYesNo( bool result)
{
	char singlePath[132];
	int startPos = 0, i = 0;
	bool createResult = TRUE;

	switch (result)
	{
	/* ---------------------------------------------------------------------------- */
	case TRUE:

		if
		(
			(GotoDataFiles() == TRUE)
			&&
			(schEdit.searchFolder[0] != '/')
			&&
			(strlen(schEdit.searchFolder) > 0)
		)
		{
			for(i = 0; ((i < strlen(schEdit.searchFolder)) && (createResult == TRUE)); i++)
			{
				if(schEdit.searchFolder[i] == '/')
				{
					memset(singlePath, 0, sizeof(singlePath));
					strncpy(singlePath, &schEdit.searchFolder[startPos], (i - startPos));
					if
					(
						(singlePath[0] != '/')
						&&
						(strlen(singlePath) > 0)
					)
					{
						if(TAP_Hdd_ChangeDir(singlePath) != schMainChangeDirSuccess)
						{
							TAP_Hdd_Create( singlePath, ATTR_FOLDER );

							if(TAP_Hdd_ChangeDir(singlePath) != schMainChangeDirSuccess)
							{
								createResult = FALSE;
							}
						}
					}
					else
					{
						createResult = FALSE;
					}

					startPos = i + 1;
				}
			}

			memset(singlePath, 0 , sizeof(singlePath));
			strncpy(singlePath, &schEdit.searchFolder[startPos], (i - startPos));
			if
			(
				(createResult == TRUE)
				&&
				(singlePath[0] != '/')
				&&
				(strlen(singlePath) > 0)
			)
			{
				if(TAP_Hdd_ChangeDir(singlePath) != schMainChangeDirSuccess)
				{
					TAP_Hdd_Create( singlePath, ATTR_FOLDER );

					if(TAP_Hdd_ChangeDir(singlePath) != schMainChangeDirSuccess)
					{
						createResult = FALSE;
					}
				}
			}
			else
			{
				createResult = FALSE;
			}
		}

		break;	// YES
	/* ---------------------------------------------------------------------------- */
	default:
	case FALSE:

		break;	// NO
	/* ---------------------------------------------------------------------------- */
	}   

	if( schEditValidateSearch() == TRUE )
	{
		schEditSaveSearch();
	}
}

