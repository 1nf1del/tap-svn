/************************************************************
			Part of the ukEPG project
		This module deletes the archive entries

Name	: ArchiveDelete.c
Author	: kidhazy
Version	: 0.3
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy 25-10-05  Inception date

	Last change:  USE   1 Aug 105    8:34 pm
**************************************************************/

static bool  returnFromDelete;
static bool  fileDeleted;


//------------
//
void DeleteFileFolder(void)
{
     char str[TS_FILE_NAME_SIZE];
     
     if (myfiles[CurrentDirNumber][chosenLine]->isPlaying)
        TAP_Hdd_StopTs();  // Stop the current playback to allow us to delete the file.
     TAP_Hdd_Delete(myfiles[CurrentDirNumber][chosenLine]->name);
     
     // Check if the delete was successful.
#ifdef WIN32
     if (FALSE)
#else  
     if (TAP_Hdd_Exist(myfiles[CurrentDirNumber][chosenLine]->name))  // Delete didn't work
#endif
     {
         TAP_SPrint(str,myfiles[CurrentDirNumber][chosenLine]->name);
         ShowMessageWin( rgn, "File Deletion Failed.", "Failed to delete file:", str, 400 );
         fileDeleted = FALSE;
     }
     else
         fileDeleted = TRUE;
     
}
                                           
                                           


//-----------------------------------------------------------------------
//
void ReturnFromDeleteYesNo( bool result)
{
    char str1[200], str2[200];
    
	switch (result)
    {
           case TRUE:
					  DeleteFileFolder();
                      break;	// YES
                      
	       case FALSE:
                      fileDeleted = FALSE;
                      break;	// NO
    }   
    returnFromDelete = TRUE;		// will cause a redraw of file list
    deleteWindowShowing = FALSE;
}


//-----------------------------------------------------------------------
//
void ActivateDeleteWindow(char* filename, dword attr)
{
    char title[50], str1[200], str2[200], str3[200];

    switch (attr)
    {
           case ATTR_FOLDER: 
	                         if ((folderDeleteOption == 0) &&
                             ((myfolders[myfiles[CurrentDirNumber][chosenLine]->directoryNumber]->numberOfRecordings > 0) || (myfolders[myfiles[CurrentDirNumber][chosenLine]->directoryNumber]->numberOfFolders > 0)))  // We cannot delete a non-empty folder
	                         {
                                ShowMessageWin( rgn, "Folder Delete Not Allowed.", "You cannot delete a folder that still", "contains recordings or subfolders.", 400 );
                                return;
                             }     
                             break;
                
           default:          if (myfiles[CurrentDirNumber][chosenLine]->isRecording)  // We cannot delete a recording file.
	                         {
                                 ShowMessageWin( rgn, "File Delete Not Allowed.", "You cannot delete a file that is", "currently recording.", 400 );
                                 return;
                             }  
                             break;
    }
	   

    deleteWindowShowing = TRUE;
    fileDeleted = FALSE;

    switch (attr)
    {
           case ATTR_FOLDER: TAP_SPrint(title, "Folder Delete Confirmation");
                             TAP_SPrint(str1,  "Folder: %s",filename);
                             TAP_SPrint(str2, "Do you want to delete this folder?");
                             break;
                
           default:          TAP_SPrint(title, "File Delete Confirmation");
                             TAP_SPrint(str1, "File: %s",filename);
                             TAP_SPrint(str2, "Do you want to delete this file?");
                             break;
    }
    

    DisplayYesNoWindow(title, str1, str2, "Yes", "No", 1, &ReturnFromDeleteYesNo );

}


//------------
//
void initialiseArchiveDelete(void)
{
    deleteWindowShowing = FALSE;
	returnFromDelete = FALSE;
    fileDeleted = FALSE;
}



