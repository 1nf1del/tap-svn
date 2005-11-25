/************************************************************
			Part of the ukEPG project
		This module stops the archive entries

Name	: ArchiveStop.c
Author	: kidhazy
Version	: 0.3
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy 25-10-05  Inception date

	Last change:  USE   1 Aug 105    8:34 pm
**************************************************************/

static bool  returnFromStop;
static bool  fileStopped;


//------------
//
void StopFile(void)
{
    char str[TS_FILE_NAME_SIZE];
    bool result;
    int intResult;
     
    if (myfiles[chosenLine].isRecording)
    {
       result=FALSE;
       if (strncmp(recInfo[0].fileName, myfiles[chosenLine].name,TS_FILE_NAME_SIZE)==0) result=TAP_Hdd_StopRecord(0);  // Stop the recording in slot 1.
       if (strncmp(recInfo[1].fileName, myfiles[chosenLine].name,TS_FILE_NAME_SIZE)==0) result=TAP_Hdd_StopRecord(1);  // Stop the recording in slot 1.
       if (result==TRUE)
          fileStopped = TRUE;
       else
       {
          TAP_SPrint(str,myfiles[chosenLine].name);
          ShowMessageWin( rgn, "Stop Recording Failed.", "Failed to stop recording file:", str, 400 );
          fileStopped = FALSE;
       }
    }

    if (myfiles[chosenLine].isPlaying)
    {
       intResult = TAP_Hdd_StopTs();  // Stop the current playback.
       if (intResult == 0)
          fileStopped = TRUE;
       else
       {
          TAP_SPrint(str,myfiles[chosenLine].name);
          ShowMessageWin( rgn, "Stop Playback Failed.", "Failed to stop playing file:", str, 400 );
          fileStopped = FALSE;
       }
    }
}
                                           
                                           


//-----------------------------------------------------------------------
//
void ReturnFromStopYesNo( bool result)
{
    char str1[200], str2[200];
	switch (result)
    {
           case TRUE:
					  StopFile();
                      break;	// YES
                      
	       case FALSE:
                      fileStopped = FALSE;
                      break;	// NO
    }   
    returnFromStop = TRUE;		// will cause a redraw of file list
    stopWindowShowing = FALSE;
}


//-----------------------------------------------------------------------
//
void ActivateStopWindow(char* filename)
{
    char title[50], str1[200], str2[200], str3[200];

    stopWindowShowing = TRUE;
    fileStopped = FALSE;

    if (myfiles[chosenLine].isRecording)
    {
       TAP_SPrint(title, "Recording Stop Confirmation");
       TAP_SPrint(str1, "Do you want to stop this recording?");
       TAP_SPrint(str2, "File: %s",filename);
    }

    if (myfiles[chosenLine].isPlaying)
    {
       TAP_SPrint(title, "Playback Stop Confirmation");
       TAP_SPrint(str1, "Do you want to stop this playback?");
       TAP_SPrint(str2, "File: %s",filename);
    }

    DisplayYesNoWindow(title, str1, str2, "Yes", "No", 1, &ReturnFromStopYesNo );

}


//------------
//
void initialiseStopDelete(void)
{
    stopWindowShowing = FALSE;
	returnFromStop = FALSE;
    fileStopped = FALSE;
}



