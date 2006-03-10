/************************************************************
			Part of the ukEPG project
		This module displays the Configuration list

Name	: ConfigMenu.c
Author	: Darkmatter
Version	: 0.5
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter:	31-05-05	Inception date
	  v0.1 sl8:		20-11-05	Modified for Auto Scheduler
	  v0.2 sl8:		20-01-06	All variables initialised
	  v0.3 sl8:		06-02-06	Config menu enabled
	  v0.4 sl8		16-02-06	Activation key enabled. 'Perform Search' option added
	  v0.5 sl8		09-03-06	Option added to enable/disable firmware calls

**************************************************************/

#include "RemoteKeys.c"

void configTimeKeyHandler(dword);

#define CONFIG_X1	130
#define CONFIG_X2	300
#define CONFIG_E0	113
#define CONFIG_E1	275
#define CONFIG_E2	627
#define CONFIG_LINES 10
#define DEFUALT_EXIT_OPTION 0
#define Y1_OFFSET 36
#define Y1_STEP 42					// was 44

static keyCodes_Struct localKeyCodes;
static dword CurrentActivationKey = 0;
static TYPE_ModelType currentModelType;
static bool enterActivateKey = 0;
static byte keyStage = 0;
static int chosenConfigLine = 0;
static char configOption = 0;
static bool configEnableEditTime = 0;


//--------------
//
void DisplayConfigLine(char lineNumber)
{
	char	str[80], str2[80];
	char	*text = NULL;

	if (( lineNumber < 1 ) || ( lineNumber > CONFIG_LINES )) return;		// bound check
	
	if (( chosenConfigLine == lineNumber ) && ( lineNumber != 10 ))		// highlight the current cursor line
	{																	// save, cancel, delete looks after itself
		TAP_Osd_PutGd( rgn, 53, lineNumber*Y1_STEP+Y1_OFFSET-8, &_highlightGd, FALSE );
	}
	else
	{
		TAP_Osd_PutGd( rgn, 53, lineNumber * Y1_STEP + Y1_OFFSET - 8, &_rowaGd, FALSE );
	}

	switch ( lineNumber )
	{
	/*--------------------------------------------------*/
	case 1 :
		PrintCenter(rgn, CONFIG_E0, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E1, "Model Type", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if ( currentModelType == TF5800 )
		{
			TAP_SPrint(str,"TF5800  (UK)");
		}
		else
		{
			TAP_SPrint(str,"TF5000  (International)");
		}

		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/*--------------------------------------------------*/
	case 2 :
		PrintCenter(rgn, CONFIG_E0, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E1, "Language", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		TAP_SPrint(str, "English");
		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E2, str, COLOR_DarkGray, 0 );

		break;
	/*--------------------------------------------------*/
	case 3 :
		PrintCenter(rgn, CONFIG_E0, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E1,  "Activation Key", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		switch ( keyStage )
		{
		/*--------------------------------------------------*/
		case 0 :
			text = GetButtonText( &localKeyCodes, CurrentActivationKey );

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

		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/*--------------------------------------------------*/
	case 4 :
		PrintCenter(rgn, CONFIG_E0, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E1,  "Perform Search", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		switch ( schMainPerformSearchMode )
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
				schDisplayDirectEdit(schDirectTimePos, ((schMainPerformSearchTime & 0xff00) >> 8), (schMainPerformSearchTime & 0xff), str2);
				strcat(str, str2);
			}

			break;
		/*--------------------------------------------------*/
		default :
			TAP_SPrint( str, "eh?" );

			break;
		/*--------------------------------------------------*/
		}

		TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );

		break;
	/*--------------------------------------------------*/
	case 5 :
		PrintCenter(rgn, CONFIG_E0, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E1,  "Firmware Calls", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if (FirmwareCallsEnabled == FALSE)
		{
			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E2, "Disabled", MAIN_TEXT_COLOUR, 0 );
		}
		else
		{
			TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E2, "Enabled", MAIN_TEXT_COLOUR, 0 );
		}

		break;
	/*--------------------------------------------------*/
	case 10 :
		TAP_Osd_PutGd( rgn, 53, lineNumber * Y1_STEP + Y1_OFFSET - 8, &_rowaGd, FALSE );		// No highlight for us

		if (( configOption == 0 ) && ( chosenConfigLine == 10 ))
		{
			TAP_Osd_PutGd( rgn, 116, lineNumber * Y1_STEP + Y1_OFFSET - 8, &_smallgreenbarGd, FALSE );
			PrintCenter(rgn, 144, lineNumber * Y1_STEP + Y1_OFFSET, 244, "Save", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
		}
		else PrintCenter(rgn, 144, lineNumber * Y1_STEP + Y1_OFFSET, 244, "Save", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		if (( configOption == 1 ) && ( chosenConfigLine == 10 ))
		{
			TAP_Osd_PutGd( rgn, 278, lineNumber * Y1_STEP + Y1_OFFSET - 8, &_smallgreenbarGd, FALSE );
			PrintCenter(rgn, 306, lineNumber * Y1_STEP + Y1_OFFSET, 406, "Cancel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
		}
		else PrintCenter(rgn, 306, lineNumber * Y1_STEP + Y1_OFFSET, 406, "Cancel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

		TAP_Osd_FillBox( rgn, 437, lineNumber*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );		// one extra column seperator for us

		break;
	/*--------------------------------------------------*/
	default :
		break;
	/*--------------------------------------------------*/
	}

	TAP_Osd_FillBox( rgn, CONFIG_E0, lineNumber*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );		// draw the column seperators
	TAP_Osd_FillBox( rgn, CONFIG_E1, lineNumber*Y1_STEP+Y1_OFFSET-8, 3, Y1_STEP, FILL_COLOUR );
}



//-----------------------------------------------------------------------
//
void CopyConfiguration( void )
{
	CurrentActivationKey = mainActivationKey;
	currentModelType = unitModelType;
}

void SaveConfiguration( void )
{
	mainActivationKey = CurrentActivationKey;
	unitModelType = currentModelType;

	SaveConfigurationToFile();
}



//------------------
//
void CreateConfigWindow(void)
{
	configWindowShowing = TRUE;
	DrawGraphicBoarders();
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

	for ( i=1; i<=TIMER_LINES ; i++)
	{
		DisplayConfigLine(i);
	}

	TAP_Osd_PutStringAf1419( rgn, 58, 503, 666, "Quick keys", COLOR_DarkGray, 0 );
	TAP_Osd_PutStringAf1419( rgn, 58, 528, 666, "EXIT = quit without saving, RECORD = save and exit", COLOR_DarkGray, 0 );
}


void DisplayConfigWindow( void )
{
	chosenConfigLine = 10;
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
		CurrentActivationKey = key;
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
	case 2 :								// Language

		break;
	/*--------------------------------------------------*/
	case 3 :
		switch ( key )						// Activation key
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:

			CurrentActivationKey = GetNextKey( &localKeyCodes, CurrentActivationKey );
			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:
			CurrentActivationKey = GetPreviousKey( &localKeyCodes, CurrentActivationKey );
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

			if(schMainPerformSearchMode < 2)
			{
				schMainPerformSearchMode++;
			}
			else
			{
				schMainPerformSearchMode = 0;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:

			if(schMainPerformSearchMode > 0)
			{
				schMainPerformSearchMode--;
			}
			else
			{
				schMainPerformSearchMode = 2;
			}

			DisplayConfigLine( chosenConfigLine );

			break;
		/*--------------------------------------------------*/
		case RKEY_Ok :

			if(schMainPerformSearchMode == SCH_CONFIG_SEARCH_PERIOD_SPECIFIED)
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

			if ( FirmwareCallsEnabled == FALSE )
			{
				FirmwareCallsEnabled = TRUE;
			}
			else
			{
				FirmwareCallsEnabled = FALSE;
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
	case 10 :														// bottom line commands : Save, or Cancel
		switch ( key )
		{
		/*--------------------------------------------------*/
		case RKEY_VolUp:
			if ( configOption < 1 )
			{
				configOption++;
			}
			else
			{
				configOption = 0;
			}
		
			break;
		/*--------------------------------------------------*/
		case RKEY_VolDown:
			if ( configOption > 0 )
			{
				configOption--;
			}
			else
			{
				configOption = 1;
			}

			break;
		/*--------------------------------------------------*/
		case RKEY_Ok :
			switch ( configOption )
			{
			/*--------------------------------------------------*/
			case 0 :
				SaveConfiguration();		// save
			
				break;
			/*--------------------------------------------------*/
			case 1 :
				break;				// cancel
			/*--------------------------------------------------*/
			default :
				break;
			/*--------------------------------------------------*/
			}

			CloseConfigWindow();		// Close the edit window
			returnFromEdit = TRUE;		// will cause a redraw of timer list

			break;
		/*--------------------------------------------------*/
		default :
			break;
		/*--------------------------------------------------*/
		}

		DisplayConfigLine( chosenConfigLine );

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
	int	i = 0, oldEditLine = 0;

	oldEditLine = chosenConfigLine;

	if ( enterActivateKey ) { NewActiveKeyHandler( key ); return; }			// handle our sub processes
	if ( configEnableEditTime ) { configTimeKeyHandler( key ); return; }			// handle Time Edit

	switch ( key )
	{
	/*--------------------------------------------------*/
	case RKEY_ChDown :
		configOption = 0;

		if ( chosenConfigLine < CONFIG_LINES )
		{
			chosenConfigLine++;
			DisplayConfigLine( chosenConfigLine - 1);
			DisplayConfigLine( chosenConfigLine );
		}
		else
		{
			chosenConfigLine = 1;						// 0=hidden - can't hide once cursor moved
			DisplayConfigLine( CONFIG_LINES );
			DisplayConfigLine( chosenConfigLine );
		}
		break;
	/*--------------------------------------------------*/
	case RKEY_ChUp :
		configOption = 0;

		if ( chosenConfigLine > 1 )					// 0=hidden - can't hide once cursor moved
		{
			chosenConfigLine--;
			DisplayConfigLine( chosenConfigLine + 1 );
			DisplayConfigLine( chosenConfigLine );
		}
		else
		{
			chosenConfigLine = CONFIG_LINES;
			DisplayConfigLine( 1 );
			DisplayConfigLine( chosenConfigLine );
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
		DisplayConfigLine( oldEditLine );
		DisplayConfigLine( chosenConfigLine );
		configOption = 0;						// will move the cursor to "Save" when last line selected

		break;
	/*--------------------------------------------------*/
	case RKEY_0 :
		chosenConfigLine = 10;						// make "0" select the last line

		DisplayConfigLine( oldEditLine );
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

//TAP_Print("Key pressed\r\n");		

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
	configOption = 0;
	keyStage = 0;

	CreateTF5800Keys( &localKeyCodes );
}


void TerminateConfigMenu( void )
{
	KeyDestory( &localKeyCodes );
}
