/************************************************************
				Part of the ukEPG project
		This module displays the timer list

Name	: ChannelSelection.c
Author	: Darkmatter
Version	: 0.2
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter:	27-07-05	Split from TimerEdit.c
	  v0.1 sl8:		20-11-05	More generic
	  v0.2 sl8		20-01-06	Modified for TAP_SDK. All variables initialised.

**************************************************************/

#define NUM_LOGO_ON_X_AXIS	9
#define LOGO_SPACE_X	65
#define BASE_X			70
#define LOGO_SPACE_Y	45
#define BASE_Y			80

void (*CallbackChannelSelect)( int, byte, bool );			// points to the procedure to be called when we've done

static int selectedLogo = 0;
static byte selectedSvc = 0;

//---------------------------------------------------------------
//
void DisplayChannelLogo( int svcNum, byte svcType )
{
	dword x_coord = 0, y_coord = 0;

	x_coord = ((svcNum % NUM_LOGO_ON_X_AXIS) * LOGO_SPACE_X) + BASE_X;			// calculate x, and y coord for the logo
	y_coord = ((svcNum / NUM_LOGO_ON_X_AXIS) * LOGO_SPACE_Y) + BASE_Y;

	if ( svcNum == selectedLogo ) TAP_Osd_FillBox( rgn, x_coord-5, y_coord-5, 70, 49, COLOR_Yellow );	// highlight in yellow
	else TAP_Osd_FillBox( rgn, x_coord-5, y_coord-5, 70, 49, FILL_COLOUR );		// otherwise clear the space around the logo (remove any highlight)

	DisplayLogo( rgn, x_coord, y_coord, svcNum, svcType );						// draw the logo
}


void DisplayChannelSelection( void )
{
	char str[256];
	TYPE_TapChInfo	chInfo;	
	
	TAP_Channel_GetInfo( selectedSvc, selectedLogo, &chInfo );
	TAP_SPrint( str, "%d : %s", chInfo.logicalChNum, chInfo.chName );

	TAP_Osd_FillBox( rgn, 53, 490, 614, 86, FILL_COLOUR );				// clear the bottom portion
	TAP_Osd_PutStringAf1622( rgn, 58, 518, 610, str, INFO_COLOUR, FILL_COLOUR );
}

void ReDrawAllLogos( void )
{
	int countTvSvc = 0, countRadioSvc = 0;
	int svcCount = 0, i = 0;

	TAP_Osd_FillBox( rgn, 53, 70, 614, 420, FILL_COLOUR );				// Clear the main portion of the screen
	
	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	if ( selectedSvc == 0 ) svcCount = countTvSvc; else svcCount = countRadioSvc;
	
	for ( i=0; i<svcCount ; i++)										// for each channel (service)
	{
	    DisplayChannelLogo( i, selectedSvc );							// display the logo
	}
	
	DisplayChannelLogo( selectedLogo, selectedSvc );					// redraw the highlight (makes things a little tidier)
	DisplayChannelSelection();
}

void DisplayChannelListWindow( int svcNum, byte svcType, void (*ReturningProcedure)( int, byte, bool ) )
{
	char str[256];

	CallbackChannelSelect = ReturningProcedure;

	channelListWindowShowing = TRUE;
	selectedLogo = svcNum;
	selectedSvc = svcType;

	DrawGraphicBoarders();												// Draw the pretty graphical surround

	TAP_SPrint( str, "Channel Selection" );
	TAP_Osd_PutStringAf1926( rgn, 58, 40, 390, str, TITLE_COLOUR, COLOR_Black );

	ReDrawAllLogos();
}



void CloseChannelListWindow( bool successOrFail )
{
	channelListWindowShowing = FALSE;
	returnFromEdit = TRUE;												// will cause a redraw of timer edit window

	CallbackChannelSelect( selectedLogo, selectedSvc, successOrFail );
}




void ChannelListKeyHandler( dword key )
{
	int svcCount = 0, i = 0;
	int countTvSvc = 0, countRadioSvc = 0;
	int oldSelection = 0;
	
	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	if ( selectedSvc == 0 ) svcCount = countTvSvc; else svcCount = countRadioSvc;
	
	switch ( key )
	{
		case RKEY_VolUp :	oldSelection = selectedLogo;
							selectedLogo++;										// move the cursor across one logo
							
							if ( selectedLogo >= svcCount) selectedLogo = 0;

						    DisplayChannelLogo( oldSelection, selectedSvc);		// redraw the old logo without the highlight
						    DisplayChannelLogo( selectedLogo, selectedSvc);		// and redraw the new selection with the highlight
							DisplayChannelSelection();
							
							break;

		case RKEY_VolDown :	oldSelection = selectedLogo;

							if ( selectedLogo > 0 ) selectedLogo--;				// move the cursor left one logo
							else selectedLogo = svcCount-1;

						    DisplayChannelLogo( oldSelection, selectedSvc);		// redraw the old logo without the highlight
						    DisplayChannelLogo( selectedLogo, selectedSvc);		// and redraw the new selection with the highlight
							DisplayChannelSelection();

							break;
							
							
		case RKEY_ChDown :	oldSelection = selectedLogo;
							selectedLogo += NUM_LOGO_ON_X_AXIS;					// move the cursor down one line
							
							if ( selectedLogo >= svcCount) selectedLogo = (selectedLogo % NUM_LOGO_ON_X_AXIS);	// keep in same column when scrolling off bottom

						    DisplayChannelLogo( oldSelection, selectedSvc);		// redraw the old logo without the highlight
						    DisplayChannelLogo( selectedLogo, selectedSvc);		// and redraw the new selection with the highlight
							DisplayChannelSelection();
							
							break;

							
		case RKEY_ChUp :	oldSelection = selectedLogo;

							if ( selectedLogo >= NUM_LOGO_ON_X_AXIS ) selectedLogo -= NUM_LOGO_ON_X_AXIS;	// move the cursor up one line
							else
							{													// keep the same column when scroll off top
								while (selectedLogo <= svcCount-1) { selectedLogo += NUM_LOGO_ON_X_AXIS; }
								selectedLogo -= NUM_LOGO_ON_X_AXIS;
							}

						    DisplayChannelLogo( oldSelection, selectedSvc);		// redraw the old logo without the highlight
						    DisplayChannelLogo( selectedLogo, selectedSvc);		// and redraw the new selection with the highlight
							DisplayChannelSelection();
							
							break;

							
		case RKEY_Forward :
		case RKEY_Rewind :                                          			// swap between TV and Radio
		case RKEY_TvRadio :	if ( selectedSvc == 0 )
							{
								selectedSvc= 1;
								if ( selectedLogo >= countRadioSvc ) selectedLogo = 0;
							}
							else
							{
							    selectedSvc = 0;
								if ( selectedLogo >= countTvSvc ) selectedLogo = 0;
							}

							ReDrawAllLogos();
							break;
				
		case RKEY_Record :							
		case RKEY_Ok :
							CloseChannelListWindow( TRUE );							// go back to the timer edit screen
							break;

							
		case RKEY_Exit :	CloseChannelListWindow( TRUE );							// go back to the timer edit screen
							break;


		default :
			break;
	}
}
