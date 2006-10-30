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

//static bool  returnFromDelete;
//static bool  fileDeleted;

//------------
//
void DeleteAction(int type)
{
     char str[TS_FILE_NAME_SIZE], title[50], str1[200];

     appendToLogfile("DeleteAction: Started.", INFO);

     switch (type)
     {
           case 0:  TAP_SPrint(title, "Folder Delete Failed.");
                    TAP_SPrint(str1,  "Failed to delete folder:");
                    break;
                
           default: TAP_SPrint(title, "File Delete Failed.");
                    TAP_SPrint(str1,  "Failed to delete file:");
                    break;
     }

     appendStringToLogfile("DeleteAction: Calling TAP_Hdd_Delete for: %s",myfiles[CurrentDirNumber][chosenLine]->name, WARNING);
     TAP_Hdd_Delete(myfiles[CurrentDirNumber][chosenLine]->name);
     appendToLogfile("DeleteAction: TAP_Hdd_Delete finished.", WARNING);
     
     // Check if the delete was successful.
     #ifdef WIN32  // If testing on Windows platform, assume success rather than physically deleting file.
     if (FALSE)
     #else  
     if (TAP_Hdd_Exist(myfiles[CurrentDirNumber][chosenLine]->name))  // Delete didn't work
     #endif
     {
         TAP_SPrint(str,myfiles[CurrentDirNumber][chosenLine]->name);
         ShowMessageWin( rgn, title, str1, str, 400 );
         fileDeleted = FALSE;
     }
     else
         fileDeleted = TRUE;
         
     appendToLogfile("DeleteAction: Finished.", INFO);
         
}     



//------------
//
void DeleteFileFolder(void)
{
     char str[TS_FILE_NAME_SIZE];
     
     appendToLogfile("DeleteFileFolder: Started.", INFO);

     // If the file that has been chosen to be deleted is the current playback, and we're still in playback mode, stop it before deleting it.
     if ((myfiles[CurrentDirNumber][chosenLine]->isPlaying) && (inPlaybackMode))
     {
        appendToLogfile("DeleteFileFolder: File playing.  Calling TAP_Hdd_StopTs.", WARNING);
        TAP_Hdd_StopTs();  // Stop the current playback to allow us to delete the file.
        appendToLogfile("DeleteFileFolder: File playing.  TAP_Hdd_StopTs finished.", WARNING);
     }
        
     //  Check the chosen file attribute to decide if we are deleting a file or a folder.
     switch (myfiles[CurrentDirNumber][chosenLine]->attr)
     {
            case ATTR_FOLDER:   // We have chosen to delete a folder.
                                DeleteAction(0);   // Delete the Folder
                                break;
                                
            default:            // Treat it as a file.
                                switch (recycleBinOption)    // check which recycle-bin option is set.
                                {
                                    case 0:   // No recycle bin, permenantly delete.
                                              DeleteAction(1);  // Delete the File
                                              break;
                                              
                                    case 1:   // Move file to recycle bin.
                                    case 2:   // Move file to recycle bin.
                                              RecycleAction();  // Recycle the File
                                              break;
                                }
     }
     appendToLogfile("DeleteFileFolder: Finished.", INFO);

}
                                           
                                           


//-----------------------------------------------------------------------
//
void ReturnFromDeleteYesNo( bool result)
{
    // Routine if invoked upon return from the "Delete File" confirmation prompt.
     
    char str1[200], str2[200];
    
    appendToLogfile("ReturnFromDeleteYesNo: Started.", INFO);

    // Check the result of the confirmation panel to decide what to do.
	switch (result)
    {
           case TRUE:  // YES
					   DeleteFileFolder();
                       break;
                      
	       case FALSE: // NO
                       fileDeleted = FALSE;
                       break;	
    }   
    returnFromDelete    = TRUE;		//  Set flag that will cause a redraw of file list.
    deleteWindowShowing = FALSE;    //  Clear the flag that show the delete file is showing.
    appendToLogfile("ReturnFromDeleteYesNo: Finished.", INFO);
}


//-----------------------------------------------------------------------
//
void ActivateDeleteWindow(char* filename, dword attr)
{
    char title[50], str1[200], str2[200], str3[200];

    appendToLogfile("ActivateDeleteWindow: Started.", INFO);

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
	   

    fileDeleted         = FALSE;   // Clear the flag to indicate that the file/folder has been deleted.

    // Determine the appropriate window message depending on our recycle bin settings.
    switch (recycleBinOption)
    {
           case 0:           // Normal file delete.
                             strcpy( str3, "delete" );
                             break;
           case 1:
           case 2:           // Recycle bin options.
                             strcpy( str3, "recycle" );
                             break;
    }
    // Determine the appropriate window title and window message depending on whether we are deleing a folder or file.
    switch (attr)
    {
           case ATTR_FOLDER: TAP_SPrint(title, "Folder Delete Confirmation");
                             TAP_SPrint(str1,  "Folder: %s",filename);
                             TAP_SPrint(str2,  "Do you want to delete this folder?");
                             break;
                
           default:          TAP_SPrint(title, "File Delete Confirmation");
                             TAP_SPrint(str1,  "File: %s",filename);
                             TAP_SPrint(str2,  "Do you want to %s this file?", str3);
                             break;
    }
    
    switch (recycleBinOption)
    {
           case 0:           // Normal file delete.
           case 1:           // Recycle bin with confirmation.
                             // Display a confirmation pop-up window.  Invoke "ReturnFromDeleteYesNo" after a choice has been made.
                             deleteWindowShowing = TRUE;    // Set the flag to indicate that the delete confirmation window is showing.
                             DisplayYesNoWindow(title, str1, str2, "Yes", "No", 1, &ReturnFromDeleteYesNo );
                             break;

           case 2:           // Recycle bin wih no confirmation.
                             // Invoke the ReturnFromDeleteYesNo routine as if we selected "Yes" on the confirmation popup.
                             ReturnFromDeleteYesNo(TRUE);
                             break;
    }
    
    appendToLogfile("ActivateDeleteWindow: Finished.", INFO);

}


//------------
//
void initialiseArchiveDelete(void)
{
    deleteWindowShowing = FALSE;
	returnFromDelete    = FALSE;
    fileDeleted         = FALSE;
}



