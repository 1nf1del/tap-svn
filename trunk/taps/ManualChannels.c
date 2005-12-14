/************************************************************
			Part of the ukEPG project
		This module handles entering of Manual Channels

Name	: ManualChannels.c
Author	: kidhazy
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy: 12-09-05	Inception date

	Last change:  USE   
**************************************************************/

static bool manualChannelEntered=FALSE;
int enteredChannelNumber = 0;
int digitsEntered = 0;

dword manualChannelTick;
#define maxDigitsEntered 4   // How many digits to allow.
#define manualChannelWait 250 // Time to wait between pressing a channel number and automatically selecting that channel.

bool FindMatchingService( int LCN, word *svcNum, byte *svcType)
{
    int i;
	TYPE_logoArray	*localPtr;
    int maxTvSvc, maxRadioSvc;
	
    TAP_Channel_GetTotalNum( &maxTvSvc, &maxRadioSvc );

	for ( i=0; i<maxTvSvc; i++)				// first compare all the TV services
	{
		if (logoArrayTv[i].svcLCN == LCN )	//We have a matching LCN
		{
			*svcNum  = i;
			*svcType = logoArrayTv[i].svcType;
			return TRUE;
		}
	}
	for ( i=0; i<maxRadioSvc; i++)										// then compare all the Radio services
	{
		if (logoArrayRadio[i].svcLCN == LCN )	//We have a matching LCN
		{
			*svcNum  = i;
			*svcType = logoArrayRadio[i].svcType;
			return TRUE;
		}
	}

	return FALSE;														// didn't find any service name that match
     
}

void ResetManualChannelNumber()
{
     manualChannelEntered = FALSE; 
     enteredChannelNumber = 0;    
     TAP_Osd_PutGd( rgn, BASE_X-10, BASE_Y-HEADER_H, &_narrowhighlight80x22Gd, TRUE );	// Blank out the channel entry field.
}


void ChangeToManualChannel(int channelNumber)
{
     word svcNum;
     byte svcType;

     ResetManualChannelNumber();
     if (!FindMatchingService( channelNumber, &svcNum, &svcType) ) // If the LCN entered doesn't match one of our channels.
     {
	    RedrawAllLogos( currentSvcType, currentSvcNum, FALSE );  // Redraw the logos to reprint the channel name text.
        return;   // And return without changing channels.
     }   
     
     currentSvcNum  = svcNum;
     currentSvcType = svcType;
     selectionChanged = TRUE;  
     Playback_Selected = FALSE;  // We don't allow to manually select Playback channel, so reset it to false.   

     ChangePipChannel( currentSvcType, currentSvcNum );			// change the PIP window
	 currentGuideIndex = 0;
     if (guideWindowDisplayed)  // Redraw Guide Window in case new PiP channel was on another page.
     {
	      LoadGuideEventInfo( currentSvcType, currentSvcNum );
          DisplayGuideWindow( currentSvcType, currentSvcNum );
     }
     RedrawAllLogos( currentSvcType, currentSvcNum, FALSE );  // Redraw the logos as the new logo may have been on a new page.
	 DisplayEventInfo( currentSvcType, currentSvcNum );

}

void SelectManualChannel(int pressedKey)
{
     int digit;
     char channelStr[10];
     
     digit = pressedKey - RKEY_0;  // Calculate the actual digit value of the key pressed.
     enteredChannelNumber =  enteredChannelNumber * 10 + digit;
     manualChannelTick = TAP_GetTick();
     if (!manualChannelEntered) digitsEntered = 1; // This is our first digit
     else digitsEntered++;  // Increase the number of digits entered.
     
     
     sprintf( channelStr, "%-4i", enteredChannelNumber);  // Take the 1st 4 characters to display.
     TAP_Osd_PutGd( rgn, BASE_X-10, BASE_Y-HEADER_H, &_narrowhighlight80x22Gd, TRUE );	// draw the green bar
     PrintCenter( rgn, BASE_X, BASE_Y-HEADER_H-2, BASE_X+LOGO_W, channelStr, COLOR_White, 0, FNT_Size_1622);

     if (digitsEntered == maxDigitsEntered)    // If we've entered the maximum number of digits, select channel and reset.
     {
         ChangeToManualChannel( enteredChannelNumber );
     }
     else manualChannelEntered = TRUE;

}
