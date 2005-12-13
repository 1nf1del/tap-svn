/************************************************************
			Part of the ukEPG project
		This module renames the archive entries

Name	: ArchiveRename.c
Author	: kidhazy
Version	: 0.3
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 kidhazy 25-10-05  Inception date

	Last change:  USE   1 Aug 105    8:34 pm
**************************************************************/

#include "Keyboard.c"

#define FILENAME_LENGTH 56

static char  renameCommandOption;
static bool  returnFromRename;
static bool  fileRenamed;
static byte* renameWindowCopy;



//--------------------------------------------------------
// removes the ".rec" from the end of a file name
void StripFileName( char *name )
{
	int 	i, k, len;
	bool 	noREC;
	char	str[10];

	noREC = FALSE;
	len = strlen( name );
	memset( str, '\0', sizeof(str) );									// will catch any duff processing

	if ( len < 4 ) return;												// can't process if string too short
	else
	{
	    k=0;
		
		for ( i = len-4 ; i < len ; i++ )
		{
		    str[k] = name[i];
			k++;
		}

		if ( strcmp( str, ".rec" ) != 0 ) return;						// can't process if the string doesn't end in ".rec"
	}

	for ( i = len-4 ; i < len ; i++ )									// overwrite the ".rec"
	{
	    name[i]='\0';
	}
}



//------------
//
void ReturnFromRenameKeyboard( char *str, bool success)
{
    char str1[200], str2[200];
    int  result;
    
    returnFromRename = TRUE;
    

    if ( success == TRUE)
	{
	    if (currentFile.attr != ATTR_FOLDER) strcat( str, ".rec" );	//ensure the file name is of the correct format
        if (strcmp(currentFile.name,str)!=0) fileRenamed=TRUE; // File name was changed.
        
        if (fileRenamed) // Check that another file doesn't already exist with the new name.
        {
              if (TAP_Hdd_Exist(str))
              {
                  // Message already exists
                  ShowMessageWin( rgn, "Rename Failed.", "A File or Folder with that name", "already exists.", 400 );
              }
              else
              {
              	   // Rename the file.
#ifdef WIN32  // If testing on WIN32 platform don't do rename as not yet implemented in the TAP SDK.
                   result=0;
#else
              	   result=TAP_Hdd_Rename(currentFile.name,str);
#endif          
              	   if (result==0)
                   {
                      // Need to update the playedFiles to update new filename.
                      
                      strcpy( currentFile.name, str );           // Update the display if we were successful.
                      strcpy( myfiles[CurrentDirNumber][chosenLine]->name, str );   // Update the array of files if we were successful.
                      if (IsFileRec(myfiles[CurrentDirNumber][chosenLine]->name, myfiles[CurrentDirNumber][chosenLine]->attr)) FormatSortName(myfiles[CurrentDirNumber][chosenLine]->sortName,str);
                      else 
                      {
                          strcpy( currentFolder.name, str );                     // Update the display if we were successful.
                          strcpy( myfolders[myfiles[CurrentDirNumber][chosenLine]->directoryNumber]->name, str );      // Update the array of folders if we were successful.
                          strcpy( myfiles[CurrentDirNumber][chosenLine]->sortName,str);
                      }
    	              infoCommandOption = INFO_OK_OPTION;                        // Highlight the "OK" Info Window option
    	              fileRenamed = TRUE;
                   }
                   else
                   {
                      TAP_SPrint(str1,"Error renaming: '%s'", currentFile.name);
                      TAP_SPrint(str2,"to: '%s'", str);
                      ShowMessageWin( rgn, "Rename Failed.", str1, str2, 500 ); 
    	              infoCommandOption = INFO_RENAME_OPTION; // Highlight the "RENAME" Info Window option
                   }   
    	           DisplayArchiveInfoWindow();  // Refresh the info window.
              }   
        }
	}
	else
	{
       	infoCommandOption = INFO_RENAME_OPTION; // Highlight the "RENAME" Info Window option
    }

}


void EditFileName( void )
{
	char	str[256];
   
	strncpy( str, currentFile.name, 256 );
	StripFileName( str );
	renameWindowShowing = TRUE;
	returnFromRename = FALSE;
	ActivateKeyboard( str, FILENAME_LENGTH, &ReturnFromRenameKeyboard );
	
}



//------------
//
void initialiseArchiveRename(void)
{
    keyboardWindowShowing = FALSE;
	returnFromRename = FALSE;
	fileRenamed = FALSE;
}



