/************************************************************
				Part of the UK TAP Project
		This module displays the Configuration list

Name	: ConfigMenu.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 31-05-05	Inception date
		  v0.1 Kidhazy:    17-08-05 added option for Now/Next/Later tag
		                            added ProgressInfo option
          v0.2 Kidhazy:    13-09-05 Provided support for 2 pages in the Config Window.
          v0.2 Kidhazy:    14-09-05 Provided support for TSRCommander calling the Config Window.
          

	Last change:  USE   7 Aug 105   11:01 pm
**************************************************************/

#include "RemoteKeys.c"

//static bool returnFromEdit;
static int chosenConfigLine;
static char configOption;

//#define CONFIG_X1	130
#define CONFIG_X2	300
#define CONFIG_E0	113
#define CONFIG_E1	275
#define CONFIG_E2	627
//#define CONFIG_Y	57
//#define CONFIG_Y	514
//#define CONFIG_Y_STEP 30

#define CONFIG_Y_OFFSET 36
#define CONFIG_Y_STEP 42					// was 44

#define CONFIG_LINES 20
#define LINES_PER_PAGE 10
#define TOTAL_PAGES 2
#define DEFUALT_EXIT_OPTION 0


static keyCodes_Struct localKeyCodes;
static bool enterActivateKey;
static byte keyStage;
static byte oldLineNumber;

static bool configWindowShowing;
static bool CalledByTSRCommander=FALSE;



//--------------
//
void DisplayConfigLine(char lineNumber)
{
    char	str[80], str2[80];
	char	*text;
	int     optionNumber, page;


	if (( lineNumber < 1 ) || ( lineNumber > CONFIG_LINES )) return;		// bound check

    optionNumber = lineNumber;
    page = ((optionNumber-1)/LINES_PER_PAGE)+1;
    lineNumber = lineNumber % LINES_PER_PAGE;
    if (lineNumber == 0) lineNumber = 10;  // Put the Save/Cancel options always on line 10.	

	if (( chosenConfigLine == optionNumber ) && ( optionNumber != 10 ))		// highlight the current cursor line
    {																	// save, cancel, delete looks after itself
		TAP_Osd_PutGd( rgn, 53, lineNumber*CONFIG_Y_STEP+CONFIG_Y_OFFSET-8, &_highlightGd, FALSE );
	}
	else
	{
		TAP_Osd_PutGd( rgn, 53, lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET - 8, &_rowaGd, FALSE );
	}
	TAP_SPrint(str,"%d.",optionNumber);
	TAP_Osd_PutStringAf1622(rgn, 75, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), 700, str, MAIN_TEXT_COLOUR, 0 );
	

	switch ( optionNumber )
	{
		case 1 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Model Type", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );


				if ( currentModelType == TF5800 ) TAP_SPrint(str,"TF5800  (UK)");
				else TAP_SPrint(str,"TF5000  (International)");
				
				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;


		case 2 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Language", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				TAP_SPrint(str, "English");
				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, COLOR_DarkGray, 0 );
			    break;

				
		case 3 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1,  "Activation Key", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

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
				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
			    break;


		case 4 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Highlight", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );


				TAP_SPrint(str,"%d", currentHighlight );
				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

				
		case 5 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Selection Style", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( displayOption )
				{
				    case 0 : 	TAP_SPrint( str, "Carousel" );
								break;
								
					case 1 : 	TAP_SPrint( str, "Linear" );
								break;

					default : 	TAP_SPrint( str, "BAD" );
								break;
				}
				
				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

		case 6 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Now/Next Tag", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentNowNextTag )
				{
				    case 0 : 	TAP_SPrint( str, "Off" );
								break;
								
					case 1 : 	TAP_SPrint( str, "On" );
								break;

					default : 	TAP_SPrint( str, "BAD" );
								break;
				}

				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

				
				
		case 7 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Progress Info", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentProgressInfo )
				{
				    case 0 : 	TAP_SPrint( str, "Off" );
								break;
								
					case 1 : 	TAP_SPrint( str, "Text" );
								break;

					default : 	TAP_SPrint( str, "BAD" );
								break;
				}

				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

				
				
		case 8 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Recall/PiP-Swap Key", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentRecallKey )
				{
				    case 0 : 	TAP_SPrint( str, "First Channel" );
								break;
								
					case 1 : 	TAP_SPrint( str, "Swap Channels" );
								break;

					default : 	TAP_SPrint( str, "BAD" );
								break;
				}

				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

				
				
		case 9 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Screen Fading", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentScreenFade)
				{
				    case 0 : 	TAP_SPrint( str, "Off - no screen fading" );
								break;
								
					case 1 : 	TAP_SPrint( str, "Manual - use Blue key" );
								break;

					default : 	TAP_SPrint( str, "Automatic after %d seconds", currentScreenFade  );
								break;
				}

				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

				
		case 11 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Start Channel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentStartChannel)
				{
				    case 0 : 	TAP_SPrint( str, "Channel Last Highlighted" );
								break;
								
					case 1 : 	TAP_SPrint( str, "Current Channel" );
								break;

					default : 	TAP_SPrint( str, "BAD"  );
								break;
				}

				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

		case 12 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Timer Start Offset", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentStartOffset)
				{
				    default : 	TAP_SPrint( str, "%d minutes",currentStartOffset );
								break;
				}

				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

		case 13 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Timer End Offset", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentEndOffset)
				{
				    default : 	TAP_SPrint( str, "%d minutes",currentEndOffset );
								break;
				}

				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;


		case 14 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Guide At Startup", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentGuideStart )
				{
				    case 0 : 	TAP_SPrint( str, "Start hidden" );
								break;
								
					case 1 : 	TAP_SPrint( str, "Start displayed" );
								break;

					case 2 : 	TAP_SPrint( str, "Start as it was when last exited" );
								break;

					default : 	TAP_SPrint( str, "BAD"  );
								break;
				}

				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

				
				
		case 15 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "TV Screen Ratio", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentTvRatio)
				{
				    case 0 : 	TAP_SPrint( str, "4:3 Standard" );
								break;
								
					case 1 : 	TAP_SPrint( str, "16:9 Widescreen" );
								break;

					default : 	TAP_SPrint( str, "BAD"  );
								break;
				}

				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

				
				
		case 16 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1, "Main/Guide window", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				switch ( currentGuideWindowOption)
				{
				    case 0 : 	TAP_SPrint( str, "Normal size" );
								break;
								
					case 1 : 	TAP_SPrint( str, "Shrink to fit" );
								break;

					default : 	TAP_SPrint( str, "BAD"  );
								break;
				}

				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
				break;

				
		case 17 :
				PrintCenter(rgn, CONFIG_E0, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E1,  "Progress Bars", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
				switch ( currentProgressBarOption )
				{
				    case 0 : 	TAP_SPrint( str, "Multi Coloured" );
								break;
								
					case 1 : 	sprintf( str, "Single colour, %% filled" );
						    	break;

					case 2 : 	TAP_SPrint( str, "Single colour, all filled" );
								break;

					case 3 : 	sprintf( str, "White colour, %% filled" );
								break;

					default : 	TAP_SPrint( str, "[Invalid value]" );
								break;
				}
				TAP_Osd_PutStringAf1622(rgn, CONFIG_X2, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), CONFIG_E2, str, MAIN_TEXT_COLOUR, 0 );
			    break;
				
				
		case 10 :		
		case 20 :
				TAP_Osd_PutGd( rgn, 53, lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET - 8, &_rowaGd, FALSE );		// No highlight for us
	            TAP_SPrint(str,"%d.",optionNumber);
                TAP_Osd_PutStringAf1622(rgn, 75, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), 700, str, MAIN_TEXT_COLOUR, 0 );

				if (( configOption == 0 ) && ( chosenConfigLine%10 == 0 ))
				{
					TAP_Osd_PutGd( rgn, 116, lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET - 8, &_smallgreenbarGd, FALSE );
				    PrintCenter(rgn, 144, lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET, 244, "Save", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
				}
				else PrintCenter(rgn, 144, lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET, 244, "Save", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

				if (( configOption == 1 ) && ( chosenConfigLine%10 == 0 ))
				{
					TAP_Osd_PutGd( rgn, 278, lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET - 8, &_smallgreenbarGd, FALSE );
					PrintCenter(rgn, 306, lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET, 406, "Cancel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );
				}
				else PrintCenter(rgn, 306, lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET, 406, "Cancel", MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

			    TAP_Osd_FillBox( rgn, 437, lineNumber*CONFIG_Y_STEP+CONFIG_Y_OFFSET-8, 3, CONFIG_Y_STEP, FILL_COLOUR );		// one extra column seperator for us
			    
	            TAP_SPrint(str,"Page %d of %d",page,TOTAL_PAGES);
	            TAP_Osd_PutStringAf1622(rgn, 470, (lineNumber * CONFIG_Y_STEP + CONFIG_Y_OFFSET), 700, str, MAIN_TEXT_COLOUR, 0 );
			    
				break;


		default :
			break;
	}


    TAP_Osd_FillBox( rgn, CONFIG_E0, lineNumber*CONFIG_Y_STEP+CONFIG_Y_OFFSET-8, 3, CONFIG_Y_STEP, FILL_COLOUR );		// draw the column seperators
    TAP_Osd_FillBox( rgn, CONFIG_E1, lineNumber*CONFIG_Y_STEP+CONFIG_Y_OFFSET-8, 3, CONFIG_Y_STEP, FILL_COLOUR );

}



//-----------------------------------------------------------------------
//
void CopyConfiguration( void )
{
	CurrentActivationKey = mainActivationKey;
	currentModelType     = unitModelType;
	currentHighlight     = highlightChoice;
	currentNowNextTag    = NowNextTagOption;
	currentProgressInfo  = ProgressInfoOption;
	currentRecallKey     = RecallKeyOption;
	currentScreenFade    = screenFadeOption;
	currentStartChannel  = StartChannelOption;
	currentStartOffset   = optionStartOffset;
	currentEndOffset     = optionEndOffset;
	currentGuideStart    = GuideStartOption;
	currentTvRatio       = TvRatioOption;
	currentGuideWindowOption = GuideWindowOption;
	currentProgressBarOption = progressBarOption;
}

void SaveConfiguration( void )
{
	mainActivationKey  = CurrentActivationKey;
	unitModelType      = currentModelType;
	highlightChoice    = currentHighlight;
	NowNextTagOption   = currentNowNextTag;
	ProgressInfoOption = currentProgressInfo;
	RecallKeyOption    = currentRecallKey;
	screenFadeOption   = currentScreenFade;
	StartChannelOption = currentStartChannel;
	optionStartOffset  = currentStartOffset;
	optionEndOffset    = currentEndOffset;
	GuideStartOption   = currentGuideStart;
	TvRatioOption      = currentTvRatio;
	GuideWindowOption  = currentGuideWindowOption;
	progressBarOption  = currentProgressBarOption;

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
	if ((guideWindowDisplayed) && (GuideWindowOption == 1)) SwitchScreenToGuideWindow();
    else SwitchScreenToNormal();  
	
    if (CalledByTSRCommander)   // If the Config Window was called by TSR Commander, then we need to do some extra cleanup.
    {
       CalledByTSRCommander=FALSE;
	   TAP_Osd_FillBox( rgn, 0, 0, 720, 576, 0 );							// clear the screen.  Added for TSR Commander support.
	   TAP_EnterNormal();                                                  // Added for TSR Commander support.  Needed to return to normal.
    }

	
}

void RedrawConfigPage(int lineNumber)
{
	int i, currentPage, oldPage;
    currentPage = (lineNumber-1)    / LINES_PER_PAGE;   // Calculate page #.  Lines 1-10=page 0   11-20= page 1, etc.
    oldPage     = (oldLineNumber-1) / LINES_PER_PAGE;   // Calculate page #.  Lines 1-10=page 0   11-20= page 1, etc.
    if ( currentPage != oldPage )  // We're on a new page so redraw all lines.
    {
	    for ( i=1+(currentPage*LINES_PER_PAGE); i<=((currentPage+1)*LINES_PER_PAGE) ; i++)
	    {
	        DisplayConfigLine(i);
        }
    }
    oldLineNumber = lineNumber;
	
}

void RedrawConfigWindow()
{
	
	CreateConfigWindow();
	CopyConfiguration();
    
    RedrawConfigPage( chosenConfigLine );
    
	TAP_Osd_PutStringAf1419( rgn, 58, 503, 666, "Quick keys", COLOR_Gray, 0 );
	TAP_Osd_PutStringAf1419( rgn, 58, 528, 666, "EXIT = quit without saving, RECORD = save and exit", COLOR_Gray, 0 );
}


void DisplayConfigWindow( void )
{
	chosenConfigLine = 10;
	configOption = 1;
	oldLineNumber = LINES_PER_PAGE * 2;   // Initialise the oldLineNumber to force a page redisplay on first pass.
	
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
	int		count, result;

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


		case 4 :	switch ( key )										// Logo highlight
					{
						case RKEY_VolUp:	SelectNextHighlight( &currentHighlight );
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_VolDown:	SelectPreviousHighlight( &currentHighlight );
											DisplayConfigLine( chosenConfigLine );
											break;

						default :			break;
					}
					break;



		case 5 :	switch ( key )										// Selection sytle
					{
						case RKEY_VolUp:	if ( displayOption < 1 ) displayOption++; else displayOption = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_VolDown:	if ( displayOption > 0 ) displayOption--; else displayOption = 1;
											DisplayConfigLine( chosenConfigLine );
											break;

						default :			break;
					}
					break;

		case 6 :	switch ( key )										// NowNext Tag
					{
						case RKEY_VolUp:	if ( currentNowNextTag < 1 ) currentNowNextTag++; else currentNowNextTag = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_VolDown:	if ( currentNowNextTag > 0 ) currentNowNextTag--; else currentNowNextTag = 1;
											DisplayConfigLine( chosenConfigLine );
											break;

						default :			break;
					}
					break;
					
					
		case 7 :	switch ( key )										// Progress Information
					{
						case RKEY_VolUp:	if ( currentProgressInfo < 1 ) currentProgressInfo++; else currentProgressInfo = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_VolDown:	if ( currentProgressInfo > 0 ) currentProgressInfo--; else currentProgressInfo = 1;
											DisplayConfigLine( chosenConfigLine );
											break;

						default :			break;
					}
					break;
					
		case 8 :	switch ( key )										// Recall Key Option
					{
						case RKEY_VolUp:	if ( currentRecallKey < 1 ) currentRecallKey++; else currentRecallKey = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_VolDown:	if ( currentRecallKey > 0 ) currentRecallKey--; else currentRecallKey = 1;
											DisplayConfigLine( chosenConfigLine );
											break;

						default :			break;
					}
					break;
					
		case 9 :	switch ( key )										// Screen Fade Options  0 - 600seconds
					{
						case RKEY_VolUp:	if ( currentScreenFade < 600 ) currentScreenFade++; else currentScreenFade = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_Forward:	if (currentScreenFade > 2)
                                            { if ( currentScreenFade < 591 ) currentScreenFade=currentScreenFade+10; 
                                              else currentScreenFade = 600;
                                            }
                                            else currentScreenFade = 10;
				                            DisplayConfigLine( chosenConfigLine );
                                            break;

						case RKEY_VolDown:	if ( currentScreenFade > 0 ) currentScreenFade--; else currentScreenFade = 600;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_Rewind:	if (currentScreenFade > 2) 
                                            { if (currentScreenFade >11) currentScreenFade=currentScreenFade-10; 
                                              else currentScreenFade = 2; 
											  DisplayConfigLine( chosenConfigLine );
                                            }  
											break;

						default :			break;
					}
					break;

		case 11 :	switch ( key )										// Channel Start Option
					{
						case RKEY_VolUp:	if ( currentStartChannel < 1 ) currentStartChannel++; else currentStartChannel = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_VolDown:	if ( currentStartChannel > 0 ) currentStartChannel--; else currentStartChannel = 1;
											DisplayConfigLine( chosenConfigLine );
											break;

						default :			break;
					}
					break;

		case 12 :	switch ( key )										// Timer Start Offset  0 - 60minutes
					{
						case RKEY_VolUp:	if ( currentStartOffset < 60 ) currentStartOffset++; else currentStartOffset = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_Forward:	if ( currentStartOffset < 55 ) currentStartOffset=currentStartOffset+5; else currentStartOffset = 60;
				                            DisplayConfigLine( chosenConfigLine );
                                            break;

						case RKEY_VolDown:	if ( currentStartOffset > 0 ) currentStartOffset--; else currentStartOffset = 60;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_Rewind:	if (currentStartOffset >6) currentStartOffset=currentStartOffset-5; else currentStartOffset = 0; 
				                            DisplayConfigLine( chosenConfigLine );
                                            break;

						default :			break;
					}
					break;

		case 13 :	switch ( key )										// Timer End Offset  0 - 60minutes
					{
						case RKEY_VolUp:	if ( currentEndOffset < 60 ) currentEndOffset++; else currentEndOffset = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_Forward:	if ( currentEndOffset < 55 ) currentEndOffset=currentEndOffset+5; else currentEndOffset = 60;
				                            DisplayConfigLine( chosenConfigLine );
                                            break;

						case RKEY_VolDown:	if ( currentEndOffset > 0 ) currentEndOffset--; else currentEndOffset = 60;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_Rewind:	if (currentEndOffset >6) currentEndOffset=currentEndOffset-5; else currentEndOffset = 0; 
				                            DisplayConfigLine( chosenConfigLine );
                                            break;

						default :			break;
					}
					break;

		case 14 :	switch ( key )										// Guide Startup Option
					{
						case RKEY_VolUp:	if ( currentGuideStart < 2 ) currentGuideStart++; else currentGuideStart = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_VolDown:	if ( currentGuideStart > 0 ) currentGuideStart--; else currentGuideStart = 2;
											DisplayConfigLine( chosenConfigLine );
											break;

						default :			break;
					}
					break;
					
		case 15 :	switch ( key )										// TV Ratio Option
					{
						case RKEY_VolUp:	if ( currentTvRatio < 1 ) currentTvRatio++; else currentTvRatio = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_VolDown:	if ( currentTvRatio > 0 ) currentTvRatio--; else currentTvRatio = 1;
											DisplayConfigLine( chosenConfigLine );
											break;

						default :			break;
					}
					break;

		case 16 :	switch ( key )										// Guide Window Option
					{
						case RKEY_VolUp:	if ( currentGuideWindowOption < 1 ) currentGuideWindowOption++; else currentGuideWindowOption = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

						case RKEY_VolDown:	if ( currentGuideWindowOption > 0 ) currentGuideWindowOption--; else currentGuideWindowOption = 1;
											DisplayConfigLine( chosenConfigLine );
											break;

						default :			break;
					}
					break;

		case 17 :	switch ( key )										// Progress Bar colours
					{
		            	case RKEY_VolUp:	if (currentProgressBarOption < 3) currentProgressBarOption = currentProgressBarOption + 1;
		            	                    else currentProgressBarOption = 0;
											DisplayConfigLine( chosenConfigLine );
											break;

											
						case RKEY_VolDown:	if (currentProgressBarOption > 0) currentProgressBarOption = currentProgressBarOption - 1;
		            	                    else currentProgressBarOption = 3;
											DisplayConfigLine( chosenConfigLine );
											break;

						default :			break;
					}
					break;

		case 10 :
        case 20 :														// bottom line commands : Save, or Cancel
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
												redrawWindow = TRUE;		// will cause a redraw of timer list
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
	int i, oldEditLine;

	oldEditLine = chosenConfigLine;

	if ( enterActivateKey ) { NewActiveKeyHandler( key ); return; }			// handle our sub processes
	
	switch ( key )
	{
		case RKEY_ChDown :	configOption = 0;
							if ( chosenConfigLine < CONFIG_LINES )
		    				{
							    chosenConfigLine++;
								DisplayConfigLine( chosenConfigLine - 1);
								RedrawConfigPage( chosenConfigLine );
                                DisplayConfigLine( chosenConfigLine );
							}
							else
							{
							    chosenConfigLine = 1;						// 0=hidden - can't hide once cursor moved
								DisplayConfigLine( CONFIG_LINES );
								RedrawConfigPage( chosenConfigLine );
								DisplayConfigLine( chosenConfigLine );
							}
							break;

							
		case RKEY_ChUp :	configOption = 0;
							if ( chosenConfigLine > 1 )					// 0=hidden - can't hide once cursor moved
		    				{
							    chosenConfigLine--;
								DisplayConfigLine( chosenConfigLine + 1 );
								RedrawConfigPage( chosenConfigLine );
								DisplayConfigLine( chosenConfigLine );
							}
							else
							{
							    chosenConfigLine = CONFIG_LINES;
								DisplayConfigLine( 1 );
								RedrawConfigPage( chosenConfigLine );
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
		case RKEY_9 :		chosenConfigLine = (key - RKEY_0) + (((oldEditLine-1)/LINES_PER_PAGE)*LINES_PER_PAGE);	// direct keyboard selection of any line
							DisplayConfigLine( oldEditLine );
							DisplayConfigLine( chosenConfigLine );
							configOption = 0;								// will move the cursor to "Save" when last line selected
							break;

		case RKEY_0 :		chosenConfigLine = 10+ (((oldEditLine-1)/LINES_PER_PAGE)*LINES_PER_PAGE);		// make "0" select the last line
							DisplayConfigLine( oldEditLine );
							DisplayConfigLine( chosenConfigLine );
							break;

							
		case RKEY_Ok :
		case RKEY_TvRadio :
        case RKEY_Forward :
        case RKEY_Rewind :
		case RKEY_VolUp :
		case RKEY_VolDown : ConfigActionHandler( key );
							break;

		case RKEY_Recall :	CopyConfiguration();					// reload the original data
	                        oldLineNumber = LINES_PER_PAGE * 3;     // Initialise the oldLineNumber to force a page redisplay.
							RedrawConfigWindow();
							break;
							
		case RKEY_Record :	if (TvRatioOption != currentTvRatio) // If we changed the TV Ratio, apply the defaults to save
	                        {
                                 if ( currentTvRatio == 1 )  SetWideScreenSizes(); // Change the screen size variables to suit the Widescreen proportions.
                                 else Set43ScreenSizes();
                                 // And if the Guide Window was displayed when we called the menu we need to close it and reactivate it
                                 // with the new cordinates that would have been set here.
                                 if (guideWindowDisplayed) 
                                 {
                                    CloseGuideWindow();    // Close current Guide window
                                    ActivateGuideWindow(); // Reactivate with new coordinates.
                                 }
                            }
                            SaveConfiguration();
							CloseConfigWindow();					// Close the edit window
							redrawWindow = TRUE;					// will cause a redraw of timer list
							break;


		case RKEY_Exit : 	CloseConfigWindow();					// Close the edit window
							redrawWindow = TRUE;					// will cause a redraw of timer list
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
	enterActivateKey    = FALSE;
	chosenConfigLine    = 0;
	oldLineNumber       = LINES_PER_PAGE * 2;   // Initialise the oldLineNumber to force a page redisplay on first pass.
	configOption        = 0;
	keyStage            = 0;

	CreateTF5800Keys( &localKeyCodes );
}


void TerminateConfigMenu( void )
{
	KeyDestory( &localKeyCodes );
}
