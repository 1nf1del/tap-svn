/************************************************************
				Part of the UK TAP Project
		This module handles display of  the channel logos

Name	: Picture.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 06-08-05	Inception Date


	Last change:  USE   9 Aug 105    9:25 pm
**************************************************************/

#define DWELL_TIME	40													// 0.3 second



int	savedWidescreen;
int savedLetterBox;
int savedPipType;
int savedPipSvcNum;
bool selectionChanged;
dword changeTime;



void ShrinkPictureWindows( void )
{
    TAP_Channel_Scale( CHANNEL_Main, SCREEN_X, SCREEN_Y, SCREEN_W, SCREEN_H, TRUE );
	TAP_Channel_Scale( CHANNEL_Sub,  PIP_SCRX, PIP_SCRY, PIP_SCRW, PIP_SCRH, TRUE );			// define the PIP window
}



void ChangePipChannel( int svcType, int svcNum )
{
	savedPipType = svcType;
	savedPipSvcNum = svcNum;

//	TAP_Channel_Start( 0, svcType, svcNum );							// change the PIP window

	changeTime = TAP_GetTick();											// mark for change later
	selectionChanged = TRUE;
}



void ChangeMainChannel( int svcType, int svcNum )
{
    int result;
    if (!Playback_Selected) 
    {     
          SaveCurrentPlaybackPosition();
          InPlaybackMode = FALSE;
	      TAP_Channel_Start( CHANNEL_Main, svcType, svcNum );
    }
    else
    {
        StartPlayback( Playback_Lastfilename, Playback_StartCluster );
//        StartPlayback( Playback_Lastfilename );
    }
    
    ChangePipChannel( currentSvcType, currentSvcNum );					// must try the PIP window again - can fail to start under
	ShrinkPictureWindows();												// some record conditions.
}




//-----------------------------------------------
// Unit will crash if a radio channel is attempted in the small PIP window
// Which means PIP can't obtain now & next info.
//
void HandleRadioPip( int isMainPlayable, int isPipPlayable )
{
	TAP_Osd_FillBox( rgn, PIP_X, PIP_Y, PIP_W, PIP_H, COLOR_Black );		// hide the picture & clear any old overlay graphics
    TAP_Osd_PutGd(   rgn, PIP_X_GRAPHIC_OFFSET, PIP_MSG_Y+4, &_radioservicebarGd, TRUE );	// display "Radio Service"

	if ( isMainPlayable != 2 )												// is this channel viewable in the main window
	{
        PrintCenter(  rgn, PIP_X_TEXT_OFFSET, PIP_MSG_Y+50, PIP_W,  "Press OK to listen",  MAIN_TEXT_COLOUR, 0 , FNT_Size_1419);
        PrintCenter(  rgn, PIP_X_TEXT_OFFSET, PIP_MSG_Y+70, PIP_W, "from the main window", MAIN_TEXT_COLOUR, 0 , FNT_Size_1419);
	}
}

//-----------------------------------------------
//
void DelayedUpdate( dword currentTickTime )
{
	int isMainPlayable, isPipPlayable;

    if ( selectionChanged == FALSE ) return;
	if ( (currentTickTime > (changeTime + DWELL_TIME)) || (currentTickTime < changeTime) )
	{
		selectionChanged = FALSE;
		ShowDelayedEvent();

		isMainPlayable = TAP_Channel_IsPlayable( CHANNEL_Main, savedPipType, savedPipSvcNum );
		isPipPlayable  = TAP_Channel_IsPlayable( CHANNEL_Sub,  savedPipType, savedPipSvcNum );

        if (Playback_Selected) // Are we pointing to the playback virtual channel?
        {
            HandlePlaybackPip( Playback_Lastfilename);
        }
        else 
        if ( isPipPlayable != 2 ) 
		{
			if ( savedPipType == SVC_TYPE_Tv  )
			{
				TAP_Osd_FillBox( rgn, PIP_X, PIP_Y, PIP_W, PIP_H, 0 );				// clear any overlay graphics
				TAP_Channel_Start( CHANNEL_Sub, savedPipType, savedPipSvcNum );		// change the PIP window
			}
			else
			    HandleRadioPip( isMainPlayable, isPipPlayable );

			ShrinkPictureWindows();
		}
		else
		{
			TAP_Osd_FillBox( rgn, PIP_X,    PIP_Y, PIP_W, PIP_H, COLOR_Black );		// hide the picture & clear any old overlay graphics
		    TAP_Osd_PutGd(   rgn, PIP_X_GRAPHIC_OFFSET, PIP_MSG_Y+4, &_notavailablebarGd, TRUE );	// display "Not Available" if channel can't start

			if ( isMainPlayable != 2 )												// is this channel viewable in the main window
			{
                PrintCenter(  rgn, PIP_X_TEXT_OFFSET, PIP_MSG_Y+50, PIP_W,  "Press OK to view",  MAIN_TEXT_COLOUR, 0 , FNT_Size_1419);
                PrintCenter(  rgn, PIP_X_TEXT_OFFSET, PIP_MSG_Y+70, PIP_W, "in the main window", MAIN_TEXT_COLOUR, 0 , FNT_Size_1419);
			}
		}
		if ((CLOCK_X-CLOCK_W < PIP_W) && ( CLOCK_Y > EVENT_Y))  UpdateClock( rgn, CLOCK_X, CLOCK_Y ); // Then clock is within PiP Region
	}
}



void DelayPipChange( dword currentTickTime )						// called when a new key press is detected
{
	changeTime = currentTickTime;									// record the time of this event
}



//-------------------------------------------
//
void ActivatePicture( void )
{
	savedWidescreen = (*TAP_GetSystemVar)( SYSVAR_TvRatio );
	savedLetterBox  = (*TAP_GetSystemVar)( SYSVAR_16_9_Display );
																		// display both screen as 16:9
//	if ( savedWidescreen == SCREEN_RATIO_4_3 ) (*TAP_SetSystemVar)( SYSVAR_16_9_Display, DISPLAY_MODE_LetterBox );
//	if ( savedWidescreen == SCREEN_RATIO_16_9 )  SetWideScreenSizes(); // Change the screen size variables to suit the Widescreen proportions.
//    else Set43ScreenSizes();
//	if ( TvRatioOption == 1 )  SetWideScreenSizes(); // Change the screen size variables to suit the Widescreen proportions.
//    else 
//    {
//         (*TAP_SetSystemVar)( SYSVAR_16_9_Display, DISPLAY_MODE_LetterBox );  // display both screen as 16:9
//         Set43ScreenSizes();
//   }
	if ( TvRatioOption == 0 )   // Standard display option
         (*TAP_SetSystemVar)( SYSVAR_16_9_Display, DISPLAY_MODE_LetterBox );  // display both screen as 16:9

// *********
// 31 July
// savedPipSvcNum is not initially set !
// and there hasn't been any scaling yet.
//	
    if ( savedPipType == SVC_TYPE_Tv  )  // Activate the PIP Window if it is in TV mode.
	{
				TAP_Osd_FillBox( rgn, PIP_X, PIP_Y, PIP_W, PIP_H, 0 );				// clear any overlay graphics
				TAP_Channel_Start( CHANNEL_Sub, savedPipType, savedPipSvcNum );		// open the PIP window
	}

}



void ClosePicture( void )
{
//	if ( savedWidescreen == SCREEN_RATIO_4_3 ) (*TAP_SetSystemVar)( SYSVAR_16_9_Display, savedLetterBox );
	if ( TvRatioOption == 0 )  (*TAP_SetSystemVar)( SYSVAR_16_9_Display, savedLetterBox );   // For 4:3 screens, set the display style back.
    
	TAP_Channel_Stop( CHANNEL_Sub );									// close the small PIP window
    TAP_Channel_Scale( CHANNEL_Main, 0, 0, 720, 576, TRUE );			// default TV picture to full screen
}



void InitialisePicture( void )
{
	changeTime = 0;
	selectionChanged = FALSE;

//    TAP_Channel_GetCurrent( &savedPipType, &savedPipSvcNum );			// load up the initial PIP window settings - just use the current window
 																		// v1.22 API will permit more.
//280805 force svcnum
savedPipType=SVC_TYPE_Tv;
savedPipSvcNum=0;
} 																		


