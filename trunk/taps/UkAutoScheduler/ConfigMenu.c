/************************************************************
			Part of the ukEPG project
		This module displays the Configuration list

Name	: ConfigMenu.c
Author	: Darkmatter
Version	: 0.12
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: 
	  v0.0 Darkmatter:	31-05-05	Inception date
	  v0.1 sl8:		20-11-05	Modified for Auto Scheduler
	  v0.2 sl8:		20-01-06	All variables initialised
	  v0.3 sl8:		06-02-06	Config menu enabled
	  v0.4 sl8:		16-02-06	Activation key enabled. 'Perform Search' option added
	  v0.5 sl8:		09-03-06	Option added to enable/disable firmware calls
	  v0.6 sl8:		11-04-06	Show window added and tidy up.
	  v0.7 sl8:		19-04-06	TRC option added.
	  v0.8 sl8:		05-08-06	Search ahead, date and time format added.
	  v0.9 sl8:		28-08-06	Keyboard types.
	  v0.10 sl8:		28-09-06	Recall bug fix. Options 4 - 9 would not revert to default.
	  v0.11 sl8:		28-09-06	Conflict handler option.
	  v0.12 sl8:		23-10-06	Added legend.
	  v0.13 janilxx:	03-11.06	Added SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING conflict handler option.

**************************************************************/

#include "RemoteKeys.c"

void configTimeKeyHandler(dword);
void configDrawLegend(void);

#define CONFIG_X1	130
#define CONFIG_X2	282	// 300

#define CONFIG_E0	53
#define CONFIG_E1	95	// 113
#define CONFIG_E2	257	// 275
#define CONFIG_E3	627

#define Y1_OFFSET	36

#define CONFIG_MENU_NUMBER_OF_DISPLAYED_OPTIONS		10
#define CONFIG_MENU_TOTAL_NUMBER_OF_OPTIONS		11

static	keyCodes_Struct	localKeyCodes;

static	dword	currentActivationKey = 0;
static	TYPE_ModelType	currentModelType;
static	byte	currentKeyboardLanguage = 0;
static	byte	currentPerformSearchMode = 0;
static	bool	currentFirmwareCallsEnabled = FALSE;
static	bool	currentTRCEnabled = TRUE;
static	int	currentPerformSearchDays = 0;
static	byte	currentDateFormat = 0;
static	byte	currentTimeFormat = 0;
static	byte	currentConflictOption = 0;
static	bool	currentAlreadyRecordedEnabled = TRUE;

static	bool	enterActivateKey = 0;
static	byte	keyStage = 0;

static	int	chosenConfigLine = 0;
static	int	configTopLine = 1;

static	char	configOption = 0;
static	bool	configEnableEditTime = 0;


//--------------
//
void DisplayConfigLine(int option)
{
	char	str[80], str2[80];
	char	*text = NULL;
	int	lineNumber = 0;

	if (( option < 1 ) || ( option > CONFIG_MENU_TOTAL_NUMBER_OF_OPTIONS )) return;		// bound check

	lineNumber = option - configTopLine + 1;

	if ( chosenConfigLine == option )			// highlight the current cursor line
	{																	// save, cancel, delete looks after itself
		TAP_Osd_PutGd( rgn, 53, lineNumber*SYS_Y1_STEP+Y1_OFFSET-8, &_highlightGd, FALSE );
	}
	else
	{
		TAP_Osd_PutGd( rgn, 53, lineNumber * SYS_Y1_STEP + Y1_OFFSET - 8, &_rowaGd, FALSE );
	}

	memset(str,0,80);
	TAP_SPrint(str,"%d", option);
	PrintCenter(rgn, 56, (lineNumber * SYS_Y1_STEP) + Y1_OFFSET, CONFIG_E1, str, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

	switch ( option )
	{
	/*--------------------------------------------------*/
	case 1 :
		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2, "Model Type", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if ( currentModelType == TF5800 )
		{
			TAP_SPrint(str,"TF5800  (UK)");
		}
		else
		{
			TAP_SPrint(str,"TF5000  (International)");
		}

		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/*--------------------------------------------------*/
	case 2 :
		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2, "Keyboard", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		switch ( currentKeyboardLanguage )
		{
		/*--------------------------------------------------*/
		case KEYBOARD_ENGLISH:

			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, "English", MAIN_TEXT_COLOUR, 0 );
		
			break;
		/*--------------------------------------------------*/
		case KEYBOARD_FINNISH:

			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, "Finnish", MAIN_TEXT_COLOUR, 0 );

			break;
		/*--------------------------------------------------*/
		case KEYBOARD_GERMAN:

			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, "German", MAIN_TEXT_COLOUR, 0 );

			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 3 :
		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2,  "Activation Key", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		switch ( keyStage )
		{
		/*--------------------------------------------------*/
		case 0 :
			text = GetButtonText( &localKeyCodes, currentActivationKey );

			strcpy( str, text );
		
			break;
		/*--------------------------------------------------*/
		case 1 :
			TAP_SPrint( str, "Press new key NOW !" );

			break;
		/*--------------------------------------------------*/
		case 2 :
			TAP_SPrint( str, "Invalid choice" );
		
			break;
		/*--------------------------------------------------*/
		default :
			TAP_SPrint( str, "eh?" );

			break;
		/*--------------------------------------------------*/
		}

		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/*--------------------------------------------------*/
	case 4 :
		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2,  "Perform Search", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		switch ( currentPerformSearchMode )
		{
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_TEN_MINS:

			TAP_SPrint( str, "Every 10 Minutes" );

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_ONE_HOUR:

			TAP_SPrint( str, "Once Every Hour" );

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_SPECIFIED:

			if (configEnableEditTime == FALSE)
			{
				TAP_SPrint( str, "Once Every Day at " );
				TAP_SPrint( str2,"%02d:%02d", ((schMainPerformSearchTime & 0xff00) >> 8), (schMainPerformSearchTime & 0xff));
				strcat(str, str2);
			}
			else
			{
				TAP_SPrint( str, "Enter Time - " );
				schEditDrawDirect(schDirectTimePos, ((schMainPerformSearchTime & 0xff00) >> 8), (schMainPerformSearchTime & 0xff), str2);
				strcat(str, str2);
			}

			break;
		/*--------------------------------------------------*/
		default :
			TAP_SPrint( str, "eh?" );

			break;
		/*--------------------------------------------------*/
		}

		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/*--------------------------------------------------*/
	case 5 :
		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2,  "Firmware Calls", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if (currentFirmwareCallsEnabled == FALSE)
		{
			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, "Disabled", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, "Enabled", MAIN_TEXT_COLOUR, 0 );
		}

		break;
	/*--------------------------------------------------*/
	case 6 :
		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2,  "TRC", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if (currentTRCEnabled == FALSE)
		{
			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, "Disabled", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, "Enabled", MAIN_TEXT_COLOUR, 0 );
		}

		break;
	/*--------------------------------------------------*/
	case 7 :
		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2,  "Search Ahead", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		switch(currentPerformSearchDays)
		{
		/*--------------------------------------------------*/
		case 1:
			TAP_SPrint( str,"%d Day", currentPerformSearchDays);

			break;
		/*--------------------------------------------------*/
		case 14:
			TAP_SPrint( str,"Unlimited");

			break;
		/*--------------------------------------------------*/
		default:
			TAP_SPrint( str,"%d Days", currentPerformSearchDays);

			break;
		/*--------------------------------------------------*/
		}

		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/*--------------------------------------------------*/
	case 8 :
		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2,  "Date Format", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		switch(currentDateFormat)
		{
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_DD_DOT_MM_DOT_YY:

			TAP_SPrint( str,"DD.MM.YY");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_DD_DOT_MM_DOT_YYYY:

			TAP_SPrint( str,"DD.MM.YYYY");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_YY_DOT_MM_DOT_DD:

			TAP_SPrint( str,"YY.MM.DD");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_YYYY_DOT_MM_DOT_DD:

			TAP_SPrint( str,"YYYY.MM.DD");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_DDMMYY:

			TAP_SPrint( str,"DDMMYY");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_DDMMYYYY:

			TAP_SPrint( str,"DDMMYYYY");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_YYMMDD:

			TAP_SPrint( str,"YYMMDD");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_YYYYMMDD:

			TAP_SPrint( str,"YYYYMMDD");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_DD_SLASH_MM_SLASH_YY:

			TAP_SPrint( str,"DD/MM/YY");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_DD_SLASH_MM_SLASH_YYYY:

			TAP_SPrint( str,"DD/MM/YYYY");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_YY_SLASH_MM_SLASH_DD:

			TAP_SPrint( str,"YY/MM/DD");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_YYYY_SLASH_MM_SLASH_DD:

			TAP_SPrint( str,"YYYY/MM/DD");

			break;
		/*--------------------------------------------------*/			
		case SCH_CONFIG_DATE_FORMAT_DD_DOT_MM:

			TAP_SPrint( str,"DD.MM");

			break;

		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_MM_DOT_DD:

			TAP_SPrint( str,"MM.DD");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_DDMM:

			TAP_SPrint( str,"DDMM");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_MMDD:

			TAP_SPrint( str,"MMDD");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_DD_SLASH_MM:

			TAP_SPrint( str,"DD/MM");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_DATE_FORMAT_MM_SLASH_DD:
			
			TAP_SPrint( str,"MM/DD");

			break;
		/*--------------------------------------------------*/
		default:

			break;
		/*--------------------------------------------------*/
		}

		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/*--------------------------------------------------*/		
	case 9 :
		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2,  "Time Format", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		switch(currentTimeFormat)
		{
		/*--------------------------------------------------*/
		case SCH_CONFIG_TIME_FORMAT_HH_COLON_MM:

			TAP_SPrint( str,"HH:MM");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_TIME_FORMAT_HH_DOT_MM:

			TAP_SPrint( str,"HH.MM");

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_TIME_FORMAT_HHMM:

			TAP_SPrint( str,"HHMM");

			break;
		/*--------------------------------------------------*/
		default:

			break;
		/*--------------------------------------------------*/
		}

		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/*--------------------------------------------------*/
	case 10 :

		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2,  "Conflict Handling", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		switch(currentConflictOption)
		{
		/*--------------------------------------------------*/
		case SCH_MAIN_CONFLICT_DISABLED:

			TAP_SPrint( str,"Disabled");

			break;
		/*--------------------------------------------------*/
		case SCH_MAIN_CONFLICT_COMBINE:

			TAP_SPrint( str,"Combine Timers");

			break;
		/*--------------------------------------------------*/
		case SCH_MAIN_CONFLICT_SEPARATE:

			TAP_SPrint( str,"Separate Timers");

			break;
		/*--------------------------------------------------*/
		case SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING:

			TAP_SPrint( str,"Separate Timers - Keep End Padding");

			break;
		/*--------------------------------------------------*/
		default:

			break;
		/*--------------------------------------------------*/
		}

		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/*--------------------------------------------------*/
	case 11 :

		PrintCenter(rgn, CONFIG_E1, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E2,  "Already Recorded", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if (currentAlreadyRecordedEnabled == FALSE)
		{
			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, "Disabled", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * SYS_Y1_STEP + Y1_OFFSET), CONFIG_E3, "Enabled", MAIN_TEXT_COLOUR, 0 );
		}

		break;
	/*--------------------------------------------------*/
	default :
		break;
	/*--------------------------------------------------*/
	}

	TAP_Osd_FillBox( rgn, CONFIG_E1, lineNumber*SYS_Y1_STEP+Y1_OFFSET-8, 3, SYS_Y1_STEP, FILL_COLOUR );		// draw the column seperators
	TAP_Osd_FillBox( rgn, CONFIG_E2, lineNumber*SYS_Y1_STEP+Y1_OFFSET-8, 3, SYS_Y1_STEP, FILL_COLOUR );
}



//-----------------------------------------------------------------------
//
void CopyConfiguration( void )
{
	currentActivationKey = mainActivationKey;
	currentModelType = unitModelType;
	currentKeyboardLanguage = keyboardLanguage;
	currentPerformSearchMode = schMainPerformSearchMode;
	currentFirmwareCallsEnabled = FirmwareCallsEnabled;
	currentTRCEnabled = schMainTRCEnabled;
	currentPerformSearchDays = schMainPerformSearchDays;
	currentDateFormat = schMainDateFormat;
	currentTimeFormat = schMainTimeFormat;
	currentConflictOption = schMainConflictOption;
	currentAlreadyRecordedEnabled = schMainAlreadyRecordedEnabled;
}

void SaveConfiguration( void )
{
	mainActivationKey = currentActivationKey;
	unitModelType = currentModelType;
	keyboardLanguage = currentKeyboardLanguage;
	schMainPerformSearchMode = currentPerformSearchMode;
	FirmwareCallsEnabled = currentFirmwareCallsEnabled;
	schMainTRCEnabled = currentTRCEnabled;
	schMainPerformSearchDays = currentPerformSearchDays;
	schMainDateFormat = currentDateFormat;
	schMainTimeFormat = currentTimeFormat;
	schMainConflictOption = currentConflictOption;
	schMainAlreadyRecordedEnabled = currentAlreadyRecordedEnabled;

	SaveConfigurationToFile();
}



//------------------
//
void CreateConfigWindow(void)
{
	configWindowShowing = TRUE;
	sysDrawGraphicBorders();
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, "Configuration", TITLE_COLOUR, COLOR_Black );
}


void CloseConfigWindow(void)
{
	configWindowShowing = FALSE;
}


void RedrawConfigWindow( void )
{
	int i = 0;
	
	CreateConfigWindow();
	UpdateListClock();
	CopyConfiguration();

	for (i = 1; i <= CONFIG_MENU_NUMBER_OF_DISPLAYED_OPTIONS; i++)
	{
		DisplayConfigLine(i);
	}

	configDrawLegend();
}


void DisplayConfigWindow( void )
{
	chosenConfigLine = 0;
	configTopLine = 1;
	configOption = 1;
	
	RedrawConfigWindow();
}



//--------------------------------------------------------
//
void DirectEnterActivateKey( void )
{
	enterActivateKey = TRUE;
	keyStage = 1;
	DisplayConfigLine( chosenConfigLine );				// will display the message "Press new key NOW!"
}


void NewActiveKeyHandler( dword key )
{
	if ( IsKeyValid( &localKeyCodes, key ) )
	{
		currentActivationKey = key;
	}
	else
	{
		keyStage = 2;
		DisplayConfigLine( chosenConfigLine );			// will display the message "invalid choice"
		TAP_Delay( 100 );
	}

	enterActivateKey = FALSE;
	keyStage = 0;
	DisplayConfigLine( chosenConfigLine );				// redraw the key desciption
}



//--------------------------------------------------------
//
void ConfigActionHandler(dword key)
{
	int	count = 0, result = 0;

	switch ( chosenConfigLine )
	{
	/*--------------------------------------------------*/
	case 1 :
		switch ( key )						// Model type
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:
			if ( currentModelType == TF5800 )
			{
				currentModelType = TF5000;
				CreateTF5000Keys( &localKeyCodes );
			}
			else
			{
				currentModelType = TF5800;
				CreateTF5800Keys( &localKeyCodes );
			}

			DisplayConfigLine( chosenConfigLine );
			DisplayConfigLine( 3 );					// update the activation key code

			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:
			if ( currentModelType == TF5800 )
			{
				currentModelType = TF5000;
				CreateTF5000Keys( &localKeyCodes );
			}
			else
			{
				currentModelType = TF5800;
				CreateTF5800Keys( &localKeyCodes );
			}

			DisplayConfigLine( chosenConfigLine );
			DisplayConfigLine( 3 );					// update the activation key code

			break;
		/*--------------------------------------------------*/
		case RKEY_Ok :

			break;
		/*--------------------------------------------------*/
		default :

			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 2 :								// Keyboard

		switch ( key )
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:

			if(currentKeyboardLanguage < KEYBOARD_GERMAN)
			{
				currentKeyboardLanguage++;
			}
			else
			{
				currentKeyboardLanguage = KEYBOARD_ENGLISH;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:

			if(currentKeyboardLanguage > KEYBOARD_ENGLISH)
			{
				currentKeyboardLanguage--;
			}
			else
			{
				currentKeyboardLanguage = KEYBOARD_GERMAN;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		default :
			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 3 :
		switch ( key )						// Activation key
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:

			currentActivationKey = GetNextKey( &localKeyCodes, currentActivationKey );
			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:
			currentActivationKey = GetPreviousKey( &localKeyCodes, currentActivationKey );
			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_Ok :
			DirectEnterActivateKey();

			break;
		/*--------------------------------------------------*/
		default :
			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 4 :
		switch ( key )						// Perform Search
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:

			if(currentPerformSearchMode < 2)
			{
				currentPerformSearchMode++;
			}
			else
			{
				currentPerformSearchMode = 0;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:

			if(currentPerformSearchMode > 0)
			{
				currentPerformSearchMode--;
			}
			else
			{
				currentPerformSearchMode = 2;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_Ok :

			if(currentPerformSearchMode == SCH_CONFIG_SEARCH_PERIOD_SPECIFIED)
			{
				exitHour = (schMainPerformSearchTime >> 8) & 0xFF;
				exitMin = schMainPerformSearchTime & 0xFF;

				configEnableEditTime = TRUE;
				schDirectTimePos = SCH_DIRECT_EDIT_HOUR_TENS;

				DisplayConfigLine( chosenConfigLine );
			}

			break;
		/*--------------------------------------------------*/
		default :
			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 5 :
		switch ( key )						// Firware Calls
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:
		case RKEY_VolDown:
		case RKEY_Ok :

			if ( currentFirmwareCallsEnabled == FALSE )
			{
				currentFirmwareCallsEnabled = TRUE;
			}
			else
			{
				currentFirmwareCallsEnabled = FALSE;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		default :
			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 6 :
		switch ( key )						// TRC
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:
		case RKEY_VolDown:
		case RKEY_Ok :

			if ( currentTRCEnabled == FALSE )
			{
				currentTRCEnabled = TRUE;
			}
			else
			{
				currentTRCEnabled = FALSE;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		default :
			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 7 :
		switch ( key )						// Search Days
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:

			if(currentPerformSearchDays < 14)
			{
				currentPerformSearchDays++;
			}
			else
			{
				currentPerformSearchDays = 1;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:

			if(currentPerformSearchDays > 1)
			{
				currentPerformSearchDays--;
			}
			else
			{
				currentPerformSearchDays = 14;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		default :
			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 8 :
		switch ( key )						// Date Format
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:

			if(currentDateFormat < SCH_CONFIG_DATE_FORMAT_MM_SLASH_DD)
			{
				currentDateFormat++;
			}
			else
			{
				currentDateFormat = SCH_CONFIG_DATE_FORMAT_DD_DOT_MM_DOT_YY;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:

			if(currentDateFormat > SCH_CONFIG_DATE_FORMAT_DD_DOT_MM_DOT_YY)
			{
				currentDateFormat--;
			}
			else
			{
				currentDateFormat = SCH_CONFIG_DATE_FORMAT_MM_SLASH_DD;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		default :
			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 9 :
		switch ( key )						// Time Format
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:

			if(currentTimeFormat < SCH_CONFIG_TIME_FORMAT_HHMM)
			{
				currentTimeFormat++;
			}
			else
			{
				currentTimeFormat = SCH_CONFIG_TIME_FORMAT_HH_COLON_MM;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:

			if(currentTimeFormat > SCH_CONFIG_TIME_FORMAT_HH_COLON_MM)
			{
				currentTimeFormat--;
			}
			else
			{
				currentTimeFormat = SCH_CONFIG_TIME_FORMAT_HHMM;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		default :
			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 10 :
		switch ( key )						// Conflict handler
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:

			if(currentConflictOption < SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING)
			{
				currentConflictOption++;
			}
			else
			{
				currentConflictOption = SCH_MAIN_CONFLICT_DISABLED;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:

			if(currentConflictOption > SCH_MAIN_CONFLICT_DISABLED)
			{
				currentConflictOption--;
			}
			else
			{
				currentConflictOption = SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	case 11 :
		switch ( key )						// Already Recorded
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:
		case RKEY_VolDown:
		case RKEY_Ok :

			if ( currentAlreadyRecordedEnabled == FALSE )
			{
				currentAlreadyRecordedEnabled = TRUE;
			}
			else
			{
				currentAlreadyRecordedEnabled = FALSE;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		default :
			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	default :
		break;
	/*--------------------------------------------------*/
	}
}


//------------
//
void ConfigKeyHandler(dword key)
{
	int	i = 0, oldChosenConfigLine = 0;

	oldChosenConfigLine = chosenConfigLine;

	if ( enterActivateKey ) { NewActiveKeyHandler( key ); return; }			// handle our sub processes
	if ( configEnableEditTime ) { configTimeKeyHandler( key ); return; }			// handle Time Edit

	switch ( key )
	{
	/*--------------------------------------------------*/
	case RKEY_ChDown :
		configOption = 0;

		if ( chosenConfigLine < CONFIG_MENU_TOTAL_NUMBER_OF_OPTIONS )
		{
			chosenConfigLine++;

			if ( chosenConfigLine > (configTopLine + CONFIG_MENU_NUMBER_OF_DISPLAYED_OPTIONS - 1) )
			{
				configTopLine++;

				for ( i = configTopLine; i <= chosenConfigLine; i++)
				{
					DisplayConfigLine(i);
				}
			}
			else
			{
				DisplayConfigLine( oldChosenConfigLine );
				DisplayConfigLine( chosenConfigLine );
			}
		}
		else		// wrap around
		{
			chosenConfigLine = 1;				// 0=hidden - can't hide once cursor moved

			if(configTopLine != 1)
			{
				configTopLine = 1;

				for ( i = 1; i <= CONFIG_MENU_NUMBER_OF_DISPLAYED_OPTIONS; i++)
				{
					DisplayConfigLine(i);
				}
			}
			else
			{
				DisplayConfigLine( oldChosenConfigLine );
				DisplayConfigLine( chosenConfigLine );
			}
		}

		break;
	/*--------------------------------------------------*/
	case RKEY_ChUp :

		if ( chosenConfigLine > 1 )				// 0=hidden - can't hide once cursor moved
		{
			chosenConfigLine--;

			if(chosenConfigLine < configTopLine)
			{
				configTopLine--;

				for ( i = configTopLine; i <= (configTopLine + CONFIG_MENU_NUMBER_OF_DISPLAYED_OPTIONS - 1) ; i++)
				{
					DisplayConfigLine(i);
				}
			}
			else
			{
				DisplayConfigLine( oldChosenConfigLine );
				DisplayConfigLine( chosenConfigLine );
			}
		}
		else
		{
			chosenConfigLine = CONFIG_MENU_TOTAL_NUMBER_OF_OPTIONS;

			configTopLine = CONFIG_MENU_TOTAL_NUMBER_OF_OPTIONS + 1 - CONFIG_MENU_NUMBER_OF_DISPLAYED_OPTIONS;

			if(configTopLine != 1)
			{
				for ( i = configTopLine; i <= chosenConfigLine; i++)
				{
					DisplayConfigLine(i);
				}			
			}
			else
			{
				DisplayConfigLine( oldChosenConfigLine );
				DisplayConfigLine( chosenConfigLine );
			}
		}

		break;
	/*--------------------------------------------------*/
	case RKEY_1 :
	case RKEY_2 :
	case RKEY_3 :
	case RKEY_4 :
	case RKEY_5 :
	case RKEY_6 :
	case RKEY_7 :
	case RKEY_8 :
	case RKEY_9 :
		chosenConfigLine = key - RKEY_0;				// direct keyboard selection of any line
		DisplayConfigLine( oldChosenConfigLine );
		DisplayConfigLine( chosenConfigLine );
		configOption = 0;						// will move the cursor to "Save" when last line selected

		break;
	/*--------------------------------------------------*/
	case RKEY_0 :
		chosenConfigLine = 10;						// make "0" select the last line

		DisplayConfigLine( oldChosenConfigLine );
		DisplayConfigLine( chosenConfigLine );

		break;
	/*--------------------------------------------------*/
	case RKEY_Ok :
	case RKEY_TvRadio :
	case RKEY_VolUp :
	case RKEY_VolDown :
		ConfigActionHandler( key );

		break;
	/*--------------------------------------------------*/
	case RKEY_Recall:
		//CopyConfiguration();					// reload the original data
//		RedrawTimerEdit();

		break;
	/*--------------------------------------------------*/
	case RKEY_Record :
		SaveConfiguration();
		CloseConfigWindow();					// Close the edit window
		returnFromEdit = TRUE;					// will cause a redraw of timer list

		break;
	/*--------------------------------------------------*/
	case RKEY_Exit :
		CloseConfigWindow();					// Close the edit window
		returnFromEdit = TRUE;					// will cause a redraw of timer list

		break;
	/*--------------------------------------------------*/
	default :
		break;
	/*--------------------------------------------------*/
	}
}


void configTimeKeyHandler(dword key)
{
	int	min = 0, hour = 0, temp = 0;

	hour = (schMainPerformSearchTime & 0xff00) >> 8;		// extract the time
	min = schMainPerformSearchTime & 0xff;

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

		configEnableEditTime = FALSE;

		break;
	/* ---------------------------------------------------------------------------- */
	case RKEY_Exit:

		hour = exitHour;
		min = exitMin;

		configEnableEditTime = FALSE;
	
		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}

	schMainPerformSearchTime = ((hour & 0xff) << 8) | (min & 0xff);

	DisplayConfigLine( chosenConfigLine );				// will display the message "Press new key NOW!"
}



//------------
//
void InitialiseConfigRoutines(void)
{
	configWindowShowing = FALSE;
	enterActivateKey = FALSE;
	chosenConfigLine = 0;
	configTopLine = 1;
	configOption = 0;
	keyStage = 0;

	CreateTF5800Keys( &localKeyCodes );
}


void TerminateConfigMenu( void )
{
	KeyDestory( &localKeyCodes );
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


void configDrawLegend(void)
{
	TAP_Osd_FillBox( rgn, INFO_AREA_X, INFO_AREA_Y, INFO_AREA_W, INFO_AREA_H, INFO_FILL_COLOUR );		// clear the bottom portion

	TAP_Osd_PutGd( rgn, INSTR_AREA_X, INSTR_AREA_Y + 1, &_exitoval38x19Gd, TRUE );	
	TAP_Osd_PutStringAf1419( rgn, INSTR_AREA_X + 50, INSTR_AREA_Y + 2, INSTR_AREA_X + INSTR_AREA_W, "Cancel", INFO_COLOUR, INFO_FILL_COLOUR );

	TAP_Osd_PutGd( rgn, INSTR_AREA_X, INSTR_AREA_Y + 21, &_recordoval38x19Gd, TRUE );	
	TAP_Osd_PutStringAf1419( rgn, INSTR_AREA_X + 50, INSTR_AREA_Y + 22, INSTR_AREA_X + INSTR_AREA_W, "Save", INFO_COLOUR, INFO_FILL_COLOUR );
}	

