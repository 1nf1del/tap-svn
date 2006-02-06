/************************************************************
			Part of the ukEPG project
		This module displays the Configuration list

Name	: ConfigMenu.c
Author	: Darkmatter
Version	: 0.3
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter:	31-05-05	Inception date
	  v0.1 sl8:		20-11-05	Modified for Auto Scheduler
	  v0.2 sl8:		20-01-06	All variables initialised
	  v0.3 sl8:		06-02-06	Config menu enabled

**************************************************************/

#include "RemoteKeys.c"

//static bool returnFromEdit;
static int chosenConfigLine = 0;
static char configOption = 0;

#define CONFIG_X1	130
#define CONFIG_X2	300
#define CONFIG_E0	113
#define CONFIG_E1	275
#define CONFIG_E2	627

//#define Y	57
//#define Y2	514
//#define Y_STEP 30

#define CONFIG_LINES 10
#define DEFUALT_EXIT_OPTION 0

#define Y1_OFFSET 36
#define Y1_STEP 42					// was 44

static keyCodes_Struct localKeyCodes;
static dword CurrentActivationKey = 0;
static TYPE_ModelType currentModelType;

static bool enterActivateKey = 0;
static byte keyStage = 0;


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
		case 1 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E1, "Model Type", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );


				if ( currentModelType == TF5800 ) TAP_SPrint(str,"TF5800  (UK)");
				else TAP_SPrint(str,"TF5000  (International)");
				
				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;


		case 2 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E1, "Language", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				TAP_SPrint(str, "English");
				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E2, str, COLOR_DarkGray, 0 );
			    break;

				
		case 3 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E1,  "Activation Key", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( keyStage )
				{
				    case 0 : 	text = GetButtonText( &localKeyCodes, CurrentActivationKey );
								strcpy( str, text );
								break;
								
					case 1 : 	TAP_SPrint( str, "Press new key NOW !" );
								break;

					case 2 : 	TAP_SPrint( str, "Invalid choice" );
						    	break;

					default : 	TAP_SPrint( str, "eh?" );
								break;
				}
				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * Y1_STEP + Y1_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
			    break;


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


		default :
			break;
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
	DisplayConfigLine( chosenConfigLine );							// will display the message "Press new key NOW!"
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
		DisplayConfigLine( chosenConfigLine );							// will display the message "invalid choice"
		TAP_Delay( 100 );
	}

	enterActivateKey = FALSE;
	keyStage = 0;
	DisplayConfigLine( chosenConfigLine );								// redraw the key desciption
}



//--------------------------------------------------------
//
void ConfigActionHandler(dword key)
{
	int	count = 0, result = 0;

	switch ( chosenConfigLine )
	{
		case 1 :    switch ( key )										// Model type
					{
						case RKEY_VolUp:	if ( currentModelType == TF5800 )
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

											
						case RKEY_VolDown:	if ( currentModelType == TF5800 )
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


						case RKEY_Ok :		break;

						default :			break;
					}
					break;

		case 2 :														// Language
				    break;

					
		case 3 :	switch ( key )										// Activation key
					{
						case RKEY_VolUp:	CurrentActivationKey = GetNextKey( &localKeyCodes, CurrentActivationKey );
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_VolDown:	CurrentActivationKey = GetPreviousKey( &localKeyCodes, CurrentActivationKey );
											DisplayConfigLine( chosenConfigLine );
											break;


						case RKEY_Ok :		DirectEnterActivateKey();
											break;

						default :			break;
					}
					break;


		case 10 :														// bottom line commands : Save, or Cancel
						switch ( key )
						{
							case RKEY_VolUp:	if ( configOption < 1 ) configOption++;
												else configOption = 0;
												break;

							case RKEY_VolDown:	if ( configOption > 0 ) configOption--;
												else configOption = 1;
												break;


							case RKEY_Ok :		switch ( configOption )
												{
													case 0 :	SaveConfiguration();		// save
																break;
																
													case 1 :	break;						// cancel
													default :	break;
												}

												CloseConfigWindow();		// Close the edit window
												returnFromEdit = TRUE;		// will cause a redraw of timer list
												break;

							default :			break;
						}

						DisplayConfigLine( chosenConfigLine );
					    break;
						
		default :		break;
	}
}


//------------
//
void ConfigKeyHandler(dword key)
{
	int	i = 0, oldEditLine = 0;

	oldEditLine = chosenConfigLine;

	if ( enterActivateKey ) { NewActiveKeyHandler( key ); return; }			// handle our sub processes
	
	switch ( key )
	{
		case RKEY_ChDown :	configOption = 0;
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

							
		case RKEY_ChUp :	configOption = 0;
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

							
		case RKEY_1 :
		case RKEY_2 :
		case RKEY_3 :
		case RKEY_4 :
		case RKEY_5 :
		case RKEY_6 :
		case RKEY_7 :
		case RKEY_8 :
		case RKEY_9 :		chosenConfigLine = key - RKEY_0;				// direct keyboard selection of any line
							DisplayConfigLine( oldEditLine );
							DisplayConfigLine( chosenConfigLine );
							configOption = 0;								// will move the cursor to "Save" when last line selected
							break;

		case RKEY_0 :		chosenConfigLine = 10;							// make "0" select the last line
							DisplayConfigLine( oldEditLine );
							DisplayConfigLine( chosenConfigLine );
							break;

							
		case RKEY_Ok :
		case RKEY_TvRadio :
		case RKEY_VolUp :
		case RKEY_VolDown : ConfigActionHandler( key );
							break;

		case RKEY_Recall:
			//CopyConfiguration();					// reload the original data
//							RedrawTimerEdit();
							break;
							
		case RKEY_Record :	SaveConfiguration();
							CloseConfigWindow();					// Close the edit window
							returnFromEdit = TRUE;					// will cause a redraw of timer list
							break;


		case RKEY_Exit : 	CloseConfigWindow();					// Close the edit window
							returnFromEdit = TRUE;					// will cause a redraw of timer list
							break;
							

		default :
			break;
	}
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
