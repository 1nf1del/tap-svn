/************************************************************
				Part of the UK TAP Project
		This module handles display of  the channel logos

Name	: DisplayLogos.c
Author	: Darkmatter
Version	: 0.2
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 02-08-05	Inception Date
		  v0.1 Darkmatter: 02-08-05	Turned logo display in to a caurosel
		  v0.2 Darkmatter: 02-08-05	Made Caurosel/Linear optional
		  v0.3 Kidhazy:    22-08-05 Added functions to handle playback virtual channel.

	Last change:  USE   7 Aug 105   11:01 pm
**************************************************************/

static int oldRadioService;
static int oldTvService;

#include "logo.c"
#include "graphics/Row80.gd"
//#include "graphics/Highlight80.gd"
//#include "graphics/NarrowHighlight.GD"
//#include "graphics/NarrowHighlight80x22.GD"  // Included in logo.c
#include "graphics/SmallHighlight1.gd"
#include "graphics/SmallYellowBar.gd"
#include "graphics/SmallYellowArrows.gd"
#include "graphics/SmallHighlight4.gd"
#include "graphics/SmallHighlight5.gd"
#include "graphics/SmallHighlight1s80x39.gd"
#include "graphics/SmallHighlight4s80x39.gd"
#include "graphics/SmallHighlight5s80x39.gd"

//----------------------------------------------
// test code - will be remove once test complete
void SelectNextHighlight( byte *selection )
{
	if ( *selection < 6) *selection = *selection + 1; else *selection = 0;
}

void SelectPreviousHighlight( byte *selection )
{
	if ( *selection > 0)*selection = *selection - 1; else *selection = 6;
}


//----------------------------------------------
//
void LogoPosition( dword *x, dword *y, int line )
{
    int newLine;
	
	switch ( displayOption )
	{
		case 0 :	if ( line <= MID_POINT )							// carousel
					{
					    *y = (line * LOGO_SPACE_CAROUSEL) + BASE_Y;
						*x = BASE_X-10;
					}
					else
					{
					    *y = (line * LOGO_SPACE_CAROUSEL) + BASE_Y + 8;
						*x = BASE_X-10;
					}
					break;

	    case 1 :	*y = (line * LOGO_SPACE_LINEAR) + BASE_Y;
					*x = BASE_X-10;
					break;					
					
		default : 	break;
	}
}


//----------------------------------------------
//
void PrintLogo( int svcType, int svcNum, int line, bool highlight )
{
    int countTvSvc, countRadioSvc;
	dword x_coord, y_coord;

	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	countTvSvc++;            // Increase TV Service count to accommodate virtual playback channel.
    	
	LogoPosition( &x_coord, &y_coord, line );



	switch ( displayOption )
	{																							// Carousel
	    case 0 :	if ( ( svcType == SVC_TYPE_Tv ) && ( svcNum >= countTvSvc )) return;		// bounds check
					if ( ( svcType == SVC_TYPE_Radio ) && ( svcNum >= countRadioSvc )) return;

					if ( line == MID_POINT ) y_coord+=4;
					DisplayLogo( rgn, x_coord+10, y_coord, svcNum, svcType );					// draw the logo
					break;

																								// Linear
		case 1 :	if ( highlight == TRUE )
                    {
                         TAP_Osd_FillBox( rgn, x_coord, y_coord, 10+LOGO_W+10, LOGO_H, COLOR_Black );	// Clear the background
			             switch ( highlightChoice  )			
			             {										
				            case 1 : TAP_Osd_PutGd(   rgn, x_coord, y_coord, &_smallyellowbarGd, TRUE );        break;
				            case 2 : TAP_Osd_PutGd(   rgn, x_coord, y_coord, &_smallyellowarrowsGd, TRUE );     break;
				            case 3 : TAP_Osd_PutGd(   rgn, x_coord, y_coord, &_smallhighlight5s80x39Gd, TRUE ); break;
				            case 4 : TAP_Osd_FillBox( rgn, x_coord, y_coord, 10+LOGO_W+10, LOGO_H, COLOR_Yellow ); break;
				            case 5 : TAP_Osd_PutGd(   rgn, x_coord, y_coord, &_smallhighlight1s80x39Gd, TRUE ); break;
				            case 6 : TAP_Osd_PutGd(   rgn, x_coord, y_coord, &_smallhighlight4s80x39Gd, TRUE ); break;

				            default : break;	// default is no background, just black
			             }
                    }
					else
					{
//						TAP_Osd_FillBox( rgn, x_coord, y_coord-2, 80, 2, LEFT_BAR_COLOUR );		// clear the highlight
//						TAP_Osd_FillBox( rgn, x_coord, y_coord+39, 80, 2, LEFT_BAR_COLOUR );
					    TAP_Osd_PutGd( rgn, x_coord, y_coord, &_row80Gd, TRUE );
					}
						
					if ( ( svcType == SVC_TYPE_Tv ) && ( svcNum >= countTvSvc )) return;		// bounds check
					if ( ( svcType == SVC_TYPE_Radio ) && ( svcNum >= countRadioSvc )) return;
					
					DisplayLogo( rgn, x_coord+10, y_coord, svcNum, svcType );					// draw the logo
					break;

		default : 	break;
	}
	if ((highlight == TRUE) || ((displayOption==0) && (line == MID_POINT)) ) // We're printing the selected channel, so print the channel name as well.
	{
          DisplaySvcName( rgn, BASE_X-10, BASE_Y-HEADER_H, 10+LOGO_W+10, svcNum, svcType );		// Print the highlighted channel name at the top.    
    }
}



//----------------------------------------------
//
void DrawLogoBackgroundLinear( int svcType, int svcNum )
{
	int i;
	int service;
	bool highlight;

	service = svcNum / NUM_LOGO_ON_Y_AXIS;								// find the 1st channel on this page
	service = service * NUM_LOGO_ON_Y_AXIS;

	for ( i=0; i<NUM_LOGO_ON_Y_AXIS; i++ )
	{
		if ( service == svcNum ) highlight = TRUE;
		else highlight = FALSE;
		
		PrintLogo( svcType, service, i, highlight );					// draw the logo
		service++;
	}
}


//----------------------------------------------
//
void DrawLogoBackgroundCarousel( void )
{
	int i;
	dword x_coord, y_coord;
	
	for ( i=0; i<NUM_LOGO_ON_Y_AXIS; i++ )
	{
		LogoPosition( &x_coord, &y_coord, i );

		if ( i != MID_POINT ) TAP_Osd_PutGd( rgn, x_coord, y_coord, &_row80Gd, TRUE );
		else
		{																			// middle cursor position
		    TAP_Osd_FillBox( rgn, x_coord, y_coord, 10+LOGO_W+10, 47, COLOR_Black );			// clear the background
			
			switch ( highlightChoice  )												// in this test version, the user get to choose the highlight
			{																		// by pressing the red button
				case 1 : TAP_Osd_PutGd(   rgn, x_coord, y_coord+4, &_smallyellowbarGd, TRUE ); break;
				case 2 : TAP_Osd_PutGd(   rgn, x_coord, y_coord+4, &_smallyellowarrowsGd, TRUE ); break;
				case 3 : TAP_Osd_PutGd(   rgn, x_coord, y_coord,   &_smallhighlight5Gd, TRUE ); break;
				case 4 : TAP_Osd_FillBox( rgn, x_coord, y_coord,   10+LOGO_W+10, 47, COLOR_Yellow ); break;
				case 5 : TAP_Osd_PutGd(   rgn, x_coord, y_coord,   &_smallhighlight1Gd, TRUE ); break;
				case 6 : TAP_Osd_PutGd(   rgn, x_coord, y_coord,   &_smallhighlight4Gd, TRUE ); break;
				default : break;			// default is no background, just black
			}
		}
	}
}


//----------------------------------------------
//
void RedrawAllLogos( int svcType, int svcNum, bool withBackground )
{
	int i;
	int service;
	bool highlight;
    int countTvSvc, countRadioSvc, maxSvc;

	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	countTvSvc++;            //KH Increase TV Service count to accommodate virtual playback channel.
	
	if ( svcType == SVC_TYPE_Tv ) maxSvc = countTvSvc - 1;
	else maxSvc = countRadioSvc - 1;



	switch ( displayOption )
	{																				// Carousel
	    case 0 : 	if ( withBackground ) DrawLogoBackgroundCarousel();

					if ( svcNum >= MID_POINT ) service = svcNum - MID_POINT;		// find 1st channel on the carousel
					else service = maxSvc - ( MID_POINT - svcNum - 1);

//					service = svcNum - 2;

					for ( i=0; i<NUM_LOGO_ON_Y_AXIS; i++ )
					{
						PrintLogo( svcType, service, i, FALSE );					// draw the logo

						if ( service < maxSvc ) service++;
						else service = 0;
					}
					break;

																					// Linear
	    case 1 :	if ( withBackground ) DrawLogoBackgroundLinear( svcType, svcNum );

					service = svcNum / NUM_LOGO_ON_Y_AXIS;							// find the 1st channel on this page
					service = service * NUM_LOGO_ON_Y_AXIS;

					for ( i=0; i<NUM_LOGO_ON_Y_AXIS; i++ )
					{
						if ( service == svcNum ) highlight = TRUE;
						else highlight = FALSE;
		
						PrintLogo( svcType, service, i, highlight );				// draw the logo
						service++;
					}
		
		default : break;
	}
}


void ActivateLogos( int svcType, int svcNum )
{
	TAP_Osd_FillBox( rgn, 0, 0, LOGO_WINDOW_W, LOGO_WINDOW_H, LEFT_BAR_COLOUR );			// was 130, 345 clear the left hand portion of the screen
	
//	TAP_Osd_FillBox( rgn, 0, 451, 720, 120, LEFT_BAR_COLOUR );			// clear the bottom portion of the screen

    TAP_Osd_PutGd( rgn, BASE_X-10, BASE_Y-HEADER_H, &_narrowhighlight80x22Gd, TRUE );	// draw the green bar
    
    // To assist other TAPs determine when Surfer is active we put two coloured pixels in the upper left hand corner.
    // Hopefully these pixels would normally be in the overscan area of most TV displays.
    TAP_Osd_PutPixel( rgn, 0,0,RGB(1,2,3));  // Put a pixel value that we can detect.  Value = 0x8443
    TAP_Osd_PutPixel( rgn, 0,1,RGB(1,2,3));  // Put a pixel value that we can detect.  Value = 0x8443

	RedrawAllLogos( svcType, svcNum, TRUE );
}


//----------------------------------------------
// Swap between showing TV logos and those for Radio
//
void SwapTvRadio( int *svcType, int *svcNum )
{
    int countTvSvc, countRadioSvc;
	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	countTvSvc++;            //KH Increase TV Service count to accommodate virtual playback channel.
	
	if ( *svcType == SVC_TYPE_Tv )
	{
	    *svcType = SVC_TYPE_Radio;
		
		oldTvService = *svcNum;
		*svcNum = oldRadioService;

		if ( *svcNum > countRadioSvc ) *svcNum = 0;						// bounds check
	}
	else
	{
		*svcType = SVC_TYPE_Tv;

		oldRadioService = *svcNum;
		*svcNum = oldTvService;

		if ( *svcNum > countTvSvc ) *svcNum = 0;						// bounds check
	}

	RedrawAllLogos( *svcType, *svcNum, FALSE );
}



void SelectNextLogo( int *svcType, int *svcNum )
{
    int countTvSvc, countRadioSvc;
	int oldPage, newPage;
	int oldSvcNum;
	int line, oldLine;

	
	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	countTvSvc++;            //KH Increase TV Service count to accommodate virtual playback channel.

	oldSvcNum = *svcNum;
	oldPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
	oldLine = *svcNum % NUM_LOGO_ON_Y_AXIS;


	if ( *svcType == SVC_TYPE_Tv )
	{
		if ( *svcNum < countTvSvc-1 ) *svcNum = *svcNum + 1;
		else *svcNum = 0;
	}
	else
	{
		if ( *svcNum < countRadioSvc-1 ) *svcNum = *svcNum + 1;
		else *svcNum = 0;
	}

	switch ( displayOption )
	{
	    case 0 : RedrawAllLogos( *svcType, *svcNum, FALSE );
		    	 break;
			
		case 1 : PrintLogo( *svcType, oldSvcNum, oldLine, FALSE );
				 newPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
				 line = *svcNum % NUM_LOGO_ON_Y_AXIS;

				 if ( oldPage == newPage ) PrintLogo( *svcType, *svcNum, line, TRUE );
				 else RedrawAllLogos( *svcType, *svcNum, TRUE );

			     break;

		default : break;
	}
	if (*svcNum == Playback_svcnum) Playback_Selected = TRUE;
	else Playback_Selected = FALSE;
}



void SelectPreviousLogo( int *svcType, int *svcNum )
{
    int countTvSvc, countRadioSvc;
	int oldPage, newPage;
	int oldSvcNum;
	int line, oldLine;


	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	countTvSvc++;            //KH Increase TV Service count to accommodate virtual playback channel.
	
	oldSvcNum = *svcNum;
	oldPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
	oldLine = *svcNum % NUM_LOGO_ON_Y_AXIS;
	

	if ( *svcType == SVC_TYPE_Tv )
	{
		if ( *svcNum > 0 ) *svcNum = *svcNum - 1;
		else *svcNum = countTvSvc-1;
	}
	else
	{
		if ( *svcNum > 0 ) *svcNum = *svcNum - 1;
		else *svcNum = countRadioSvc-1;
	}

	switch ( displayOption )
	{
	    case 0 : RedrawAllLogos( *svcType, *svcNum, FALSE );
		    	 break;
			
		case 1 : PrintLogo( *svcType, oldSvcNum, oldLine, FALSE );
				 newPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
				 line = *svcNum % NUM_LOGO_ON_Y_AXIS;

				 if ( oldPage == newPage ) PrintLogo( *svcType, *svcNum, line, TRUE );
				 else RedrawAllLogos( *svcType, *svcNum, TRUE );

			     break;

		default : break;
	}
	if (*svcNum == Playback_svcnum) Playback_Selected = TRUE;
	else Playback_Selected = FALSE;
}

void SelectNextLogoPage( int *svcType, int *svcNum )
{
    int countTvSvc, countRadioSvc;
	int oldPage, newPage, lastPage,tempPage;
	int oldSvcNum;
	int line, oldLine;

	
	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	countTvSvc++;            //KH Increase TV Service count to accommodate virtual playback channel.

	oldSvcNum = *svcNum;
	oldPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
	oldLine = *svcNum % NUM_LOGO_ON_Y_AXIS;
	lastPage = (countTvSvc-1) / NUM_LOGO_ON_Y_AXIS;

	if ( *svcType == SVC_TYPE_Tv )
	{
	    lastPage = (countTvSvc-1) / NUM_LOGO_ON_Y_AXIS;
	    *svcNum  = *svcNum + NUM_LOGO_ON_Y_AXIS;  // Move to the next page of logos.
	    tempPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
	    if ( *svcNum > countTvSvc-1 )
	    {
             if (displayOption == 0) *svcNum = *svcNum - countTvSvc;
             else 
             if (tempPage == lastPage) *svcNum = countTvSvc - 1;  // Move to last service.
             else *svcNum = min(6,countTvSvc-1);  // Move to last line on 1st page.
        }
	}
	else
	{
	    lastPage = (countRadioSvc-1) / NUM_LOGO_ON_Y_AXIS;
	    *svcNum  = *svcNum + NUM_LOGO_ON_Y_AXIS;  // Move to the next page of logos.
	    tempPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
	    if ( *svcNum > countRadioSvc-1 )
	    {
             if (displayOption == 0) *svcNum = *svcNum - countRadioSvc;
             else 
             if (tempPage == lastPage) *svcNum = countRadioSvc - 1;  // Move to last service.
             else *svcNum = min(6,countRadioSvc-1);  // Move to last line on 1st page.
        }
	}

	switch ( displayOption )
	{
	    case 0 : RedrawAllLogos( *svcType, *svcNum, FALSE );
		    	 break;
			
		case 1 : PrintLogo( *svcType, oldSvcNum, oldLine, FALSE );
				 newPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
				 line = *svcNum % NUM_LOGO_ON_Y_AXIS;

				 if ( oldPage == newPage ) PrintLogo( *svcType, *svcNum, line, TRUE );
				 else RedrawAllLogos( *svcType, *svcNum, TRUE );

			     break;

		default : break;
	}
	if (*svcNum == Playback_svcnum) Playback_Selected = TRUE;
	else Playback_Selected = FALSE;
}



void SelectPreviousLogoPage( int *svcType, int *svcNum )
{
    int countTvSvc, countRadioSvc;
	int oldPage, newPage, lastPage, tempPage;
	int oldSvcNum;
	int line, oldLine;


	TAP_Channel_GetTotalNum( &countTvSvc, &countRadioSvc );
	countTvSvc++;            //KH Increase TV Service count to accommodate virtual playback channel.
	
	oldSvcNum = *svcNum;
	oldPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
	oldLine = *svcNum % NUM_LOGO_ON_Y_AXIS;
	

	if ( *svcType == SVC_TYPE_Tv )
	{
	    lastPage = (countTvSvc-1) / NUM_LOGO_ON_Y_AXIS;
	    *svcNum  = *svcNum - NUM_LOGO_ON_Y_AXIS;  // Move to the previous page of logos.
	    tempPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
	    if ( *svcNum < 0 )
	    {
             if (displayOption == 0) *svcNum = countTvSvc + *svcNum;
             else *svcNum = lastPage * NUM_LOGO_ON_Y_AXIS;  // Move to first service on the last page.
        }
	}
	else
	{
	    lastPage = (countRadioSvc-1) / NUM_LOGO_ON_Y_AXIS;
	    *svcNum  = *svcNum - NUM_LOGO_ON_Y_AXIS;  // Move to the previous page of logos.
	    tempPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
	    if ( *svcNum < 0 )
	    {
             if (displayOption == 0) *svcNum = countRadioSvc - *svcNum;
             else *svcNum = lastPage * NUM_LOGO_ON_Y_AXIS;  // Move to first service on the last page.
        }
	}

	switch ( displayOption )
	{
	    case 0 : RedrawAllLogos( *svcType, *svcNum, FALSE );
		    	 break;
			
		case 1 : PrintLogo( *svcType, oldSvcNum, oldLine, FALSE );
				 newPage = *svcNum / NUM_LOGO_ON_Y_AXIS;
				 line = *svcNum % NUM_LOGO_ON_Y_AXIS;

				 if ( oldPage == newPage ) PrintLogo( *svcType, *svcNum, line, TRUE );
				 else RedrawAllLogos( *svcType, *svcNum, TRUE );

			     break;

		default : break;
	}
	if (*svcNum == Playback_svcnum) Playback_Selected = TRUE;
	else Playback_Selected = FALSE;
}





//----------------------------------------------
//
void InitialiseLogos( void )
{
	CacheLogos();
	
	oldRadioService = 0;
	oldTvService = 0;
}

void TerminateLogos( void )
{
	ReleaseLogoMemory();

}
