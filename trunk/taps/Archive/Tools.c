/************************************************************
				Part of the UK TAP Project
		This module contains some generic tools

Name	: Tools.c
Author	: Kidhazy
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Kidhazy: 10-09-05	Inception Date


	Last change:  USE   
**************************************************************/

#define MSG_SCREEN_W	360		// 720 max
#define MSG_SCREEN_H	130		// 576 max
#define MSG_SCREEN_X	((720-MSG_SCREEN_W)/2)
#define MSG_SCREEN_Y	((576-MSG_SCREEN_H)/2)

#define MSG_FOREGROUND_COLOUR RGB(31,31,31)
#define MSG_BACKGROUND_COLOUR RGB(3,5,10)
#define MSG_TITLE_COLOUR RGB(29,29,29)
//#define MSG_TEXT_COLOUR RGB8888(90,104,213)
#define MSG_TEXT_COLOUR RGB(29,29,29)


#include "graphics/msgpopup360x130.GD"


/*
static bool  returnFromMsgWindow;
int          msgWindowDelay;
dword        msgWindowTickTime;


//------------
//
void initialiseMsgWindow(void)
{
    msgWindowShowing    = TRUE;   // Flag that a message window is being displayed.
    returnFromMsgWindow = FALSE;  // Clear flag indicating return from message window.
}


//-----------------------------------------------------------------------
//
void CloseMsgWindow(void)
{
	msgWindowShowing = FALSE;
	TAP_Osd_RestoreBox(msgRgn, MSG_SCREEN_X, MSG_SCREEN_Y, MSG_SCREEN_W, MSG_SCREEN_H, msgWindowCopy);
	TAP_MemFree(msgWindowCopy);

    msgWindowShowing    = FALSE;   // Clear flag that a message window is being displayed.
    returnFromMsgWindow = FALSE;   // Clear flag indicating return from message window.
	
}


dword MsgWindowKeyHandler( dword key )
{
      char *msg1;
    dword currentTime;

	currentTime = TAP_GetTick();			// Get the current time.

  sprintf(msg1,"%d", currentTime);
	TAP_Osd_PutS(rgn, 100, 100, 400, msg1,		
		MSG_TITLE_COLOUR, 0, 0, FNT_Size_1926,
		FALSE, ALIGN_CENTER);											// show 1. message
TAP_Print("in here \r\n");    
	switch ( key )
	{
		case RKEY_Exit :	CloseMsgWindow();
	                        returnFromMsgWindow = TRUE;
                            break;							

		case RKEY_Mute :	return key;
	}
	return 0;
}

*/
//   ShowMessageBox 
//

void    ShowMessageBox( word msgRgn, char* msg1, char* msg2, char* msg3)
{
    word   width, msgY;
    int    msgScrWidth;
    
    msgScrWidth = MSG_SCREEN_W-10;
    msgY = MSG_SCREEN_Y+12;
      
    //  Calculate width of message window text
    width = TAP_Osd_GetW( msg1, 0, FNT_Size_1926 );     
    if ( TAP_Osd_GetW( msg2, 0, FNT_Size_1622 ) + 10 > width ) width = TAP_Osd_GetW( msg2, 0, FNT_Size_1622 ) + 10;
    if ( TAP_Osd_GetW( msg3, 0, FNT_Size_1622 ) + 10 > width ) width = TAP_Osd_GetW( msg3, 0, FNT_Size_1622 ) + 10;

    if ( width > msgScrWidth ) width = msgScrWidth;

    // Display the pop-up window.
    TAP_Osd_PutGd( msgRgn, MSG_SCREEN_X, MSG_SCREEN_Y, &_msgpopup360x130Gd, TRUE );
		
	TAP_Osd_PutS(msgRgn, MSG_SCREEN_X+(msgScrWidth-width)/2, msgY, MSG_SCREEN_X+(msgScrWidth+width)/2, msg1,		
		MSG_TITLE_COLOUR, 0, 0, FNT_Size_1926,
		FALSE, ALIGN_CENTER);											// show 1. message
		
	TAP_Osd_PutS(msgRgn, MSG_SCREEN_X+(msgScrWidth-width)/2, msgY+50, MSG_SCREEN_X+(msgScrWidth+width)/2, msg2,	
		MSG_TEXT_COLOUR, 0, 0, FNT_Size_1622,
		FALSE, ALIGN_CENTER);											// show 2. message

	TAP_Osd_PutS(msgRgn, MSG_SCREEN_X+(msgScrWidth-width)/2, msgY+80, MSG_SCREEN_X+(msgScrWidth+width)/2, msg3,	
		MSG_TEXT_COLOUR, 0, 0, FNT_Size_1622,
		FALSE, ALIGN_CENTER);											// show 3. message


}


//   ShowMessageWin 
//

void    ShowMessageWin( word msgRgn, char* msg1, char* msg2, char* msg3, int delay )
{
    byte*        msgWindowCopy;
    
    // Save the region where we are about to display the pop-up.
    msgWindowCopy = TAP_Osd_SaveBox(msgRgn, MSG_SCREEN_X, MSG_SCREEN_Y, MSG_SCREEN_W, MSG_SCREEN_H);

    ShowMessageBox(msgRgn, msg1, msg2, msg3 );

    TAP_Delay(delay);  // Wait for the specified time.

	TAP_Osd_RestoreBox(msgRgn, MSG_SCREEN_X, MSG_SCREEN_Y, MSG_SCREEN_W, MSG_SCREEN_H, msgWindowCopy);
	TAP_MemFree(msgWindowCopy);

}




//--------------------------------------------------------------------
//    D I R E C T O R Y    Utilities
//--------------------------------------------------------------------

char _tapDir[512];
char *_tapDirPtr = NULL;

char _currentDir[512];
char *_currentDirPtr = NULL;

//char PROGRAMFILES[] = "ProgramFiles";
//char AUTOSTART[] = "ProgramFiles\\Auto Start";


//------------------------------ InDataFilesFolder --------------------------------------
//
bool	InDataFilesFolder(char* currentDir)
{
    if (strncmp(currentDir,"/DataFiles",TS_FILE_NAME_SIZE)==0) return TRUE;
    else return FALSE;
}


//------------------------------ InDataFilesFolderSubDir --------------------------------------
//
bool	InDataFilesSubFolder(char* currentDir)
{
    if (strncmp(currentDir,"/DataFiles/",11)==0) return TRUE;
    else return FALSE;
}


//------------------------------ ChangeDirRoot --------------------------------------
//
void	ChangeDirRoot()
{
	TYPE_File	fp;
	int		iNdx;

	iNdx = 0;
	TAP_Hdd_FindFirst( &fp );

    //	Loop until Root found allow maximum of 20 levels

	while ( ( strcmp( fp.name, "__ROOT__" ) != 0 ) && ( iNdx < 20 ) )
	{
		TAP_Hdd_ChangeDir( ".." );
		TAP_Hdd_FindFirst( &fp );
	}

}

//------------------------------ FindTapDir --------------------------------------
// Run once to locate the TAP's directory and stored it in the global _tapDir
//--------------------------------------------------------------------------------------

char* FindTapDir()
{
	TYPE_File	fp;

	if ( _tapDirPtr != NULL )
		return _tapDirPtr;

	TAP_Hdd_ChangeDir("..");
	TAP_Hdd_FindFirst( &fp );

	// TAP_Print("Tapdir: fp.name = %s\r\n", fp.name);

	if ( strcmp( fp.name, "__ROOT__" ) == 0 )
	{
		strcpy(_tapDir, "/ProgramFiles");
		TAP_Hdd_ChangeDir("ProgramFiles");
	} else {
		strcpy(_tapDir, "/ProgramFiles/Auto Start");
		TAP_Hdd_ChangeDir("Auto Start");
	}

	_tapDirPtr = _tapDir;
	// TAP_Print("Tapdir: _tapDirPtr = %s\r\n", _tapDirPtr);

	return _tapDirPtr;	
}


//----------------  DIRECTORY TOOLS        ---------------------------------------------
// 
//--------------------------------------------------------------------------------------
//
void	ExtractLastField ( char* path, char* result )
{
	int	i, lastpos, startpos, fieldLength;
	char	temp[512];

	memset( result, 0, sizeof result );
	memset( temp, 0, sizeof temp );

	lastpos = strlen( path );
	startpos = 0;
	
	for ( i = lastpos; i >= 0; i -= 1 )				
	{
		if ( path[i] == 47 )  // Find the "/" character.
		{
             startpos = i;
             break;
		}
	}

    fieldLength = lastpos - startpos -1;   // Calculate final length of field.  (exclude the leading "/" )
	strncpy(temp, path+startpos+1, fieldLength); // copy approx chars per line to new string
	temp[fieldLength]= '\0'; // clean up the end of the new line.

	strcpy( result, temp );
	
}

//------------------------------ isFileRec --------------------------------------
//
bool	IsFileRec( char *recFile, 	dword	attr )
{
    //
    // Checks if a file is a valid recording.
    //
    // 2 options available via "recCheckOption"
    // recCheckOption=0 - filename ends with ".rec" AND file attribute must equal ATTR_TS 
    // recCheckOption=1 - filename ends with ".rec" 
    //
    
	char buff[128];

	strcpy( buff, recFile );       // Copy the filename into the buffer for checking.
	strlwr( buff );                // Convert the filename into all lowercase for easy comparison.
	
#ifdef WIN32
    if ( strcmp( buff + strlen( buff ) - 4, ".rec" ) == 0 )
        	return TRUE;
	else
        	return FALSE;
#else
    if ((recCheckOption==0) && ( strcmp( buff + strlen( buff ) - 4, ".rec" ) == 0 ) && (attr == ATTR_TS) )
        	return TRUE;
    else     
    if ((recCheckOption==1) && ( strcmp( buff + strlen( buff ) - 4, ".rec" ) == 0 ))
        	return TRUE;
	else
        	return FALSE;
#endif
}


//----------------  DIRECTORY TOOLS        ---------------------------------------------
// 
//--------------------------------------------------------------------------------------

bool GotoPath(char *path){
	char *startPos;
	char *endPos;
	bool ready;

	ChangeDirRoot();

	startPos=path;
	if ((*startPos)!='/'){
		// TAP_Print("%s Doesn't start with a /\r\n",startPos);
		return FALSE;
	}
	startPos=startPos+1;
	ready=FALSE;
	while (ready==FALSE){
		endPos=startPos;
		while (((*endPos)!=0) && ((*endPos)!='/')){
			endPos=endPos+1;
		}
		if ((*endPos)==0){
			ready=TRUE;
			// TAP_Print("GotoPath1: going to %s...\r\n", startPos);
			TAP_Hdd_ChangeDir(startPos);
		} else {
			(*endPos)=0;
			// TAP_Print("GotoPath2: going to %s...\r\n", startPos);
			TAP_Hdd_ChangeDir(startPos);
			(*endPos)='/';
			startPos=endPos+1;
		}
	}
	strcpy(_currentDir, path);

	return TRUE;
}	
	
void GotoTapDir(){
	char *tdir = FindTapDir();
	GotoPath(tdir);
	strcpy(_currentDir, tdir);
}

bool GotoProgramFiles(){
	return GotoPath("/ProgramFiles");
}

bool GotoDataFiles(){
	return GotoPath("/DataFiles");
}

void GotoRoot(){
	TAP_Hdd_ChangeDir("/");
	_currentDir[0] = '/';
	_currentDir[1] = '\0';
}


bool CurrentDirStartCluster (dword* cluster)
{
    TYPE_File file;

    // '.' should be the first entry, so find it
    memset (&file, 0, sizeof (file));
    if (TAP_Hdd_FindFirst (&file))
    {
       while (strcmp (file.name, ".") && TAP_Hdd_FindNext (&file)) {};
    }

    // return starting cluster of this directory
    *cluster = file.startCluster;

    return (strcmp (file.name, ".") == 0);
}

char* GetCurrentDir(void)
{
    dword cluster;
    char* path;
    TYPE_File file;
    char* temp;
                    
    appendToLogfile("GetCurrentDir: started.", INFO);

    path = TAP_MemAlloc (2);

    if (path == NULL) 
    {
             appendToLogfile("GetCurrentDir: TAP_MemAlloc (2) failed.", ERR);
             return NULL;
    }         
    appendToLogfile("GetCurrentDir: after memalloc.", INFO);

    strcpy (path, "");
    path[0]='\0';

    // while we have a '.' entry we work up the tree matching starting clusters
    while (CurrentDirStartCluster (&cluster))
    {
//        TYPE_File file;
        // move into parent directory and look for a starting cluster match
        TAP_Hdd_ChangeDir ("..");
        memset (&file, 0, sizeof (file));
        if (TAP_Hdd_FindFirst (&file))
        {
            while ((cluster != file.startCluster) && TAP_Hdd_FindNext (&file)) {};
        }
        // if we have a match prepend it to the path
        if (cluster == file.startCluster)
        {
//            char* temp;

            temp = TAP_MemAlloc (strlen (path) + strlen (file.name) + 2);

            // no memory - back to starting directory and return NULL
            if (temp == NULL)
            {
                appendToLogfile("GetCurrentDir: TAP_MemAlloc (strlen path) failed.", ERR);
                TAP_Hdd_ChangeDir (file.name);
                if (strlen (path)) TAP_Hdd_ChangeDir (&path[1]);
                TAP_MemFree (path);
                return NULL;
            }

            appendStringToLogfile("GetCurrentDir: Match on file=%s",file.name, WARNING);

            // There's an issue where we may find the "." directory entry instead of the subdir -
            // so for now, let's ignore it.   5 Nov 2005
            if (strcmp(file.name,".")!=0)
            {
               strcpy (temp, "/");
               strcat (temp, file.name);
               strcat (temp, path);
               TAP_MemFree (path);
               path = temp;
               appendStringToLogfile("GetCurrentDir: Path now=%s",path, WARNING);
            }
        }
        else
        {
            // directory structure inconsistent, shouldn't get here
            // problem - we can't get back to our starting directory
            TAP_MemFree (path);
            appendStringToLogfile("GetCurrentDir: Bombed out.",file.name, WARNING);
            return NULL;
        }
    }
    if (strlen (path))
    {
        // finally we put ourselves back in our starting directory
        //TAP_Hdd_ChangeDir (&path[1]);
        appendToLogfile("GetCurrentDir: Found current directory.", WARNING);
        appendStringToLogfile("GetCurrentDir: It's %s.",path, WARNING);
        GotoPath(path);
    }
    else
    {
        // We're at the root
        strcpy (path, "/");
        appendStringToLogfile("GetCurrentDir: Found at ROOT.",file.name, WARNING);
    }

    return (path);
}


/*
// Return the Topfield model number
TYPE_ModelType GetModel()
{
/*
406 TF5000/5500
416 TF5000t
436 TF5010/TF5510
446 TF5000 Black Panther/White Polar
456 TF5800t MHEG
466 TF5000t Black Panther/White Polar
486 TF5010-SE
501 TF5000CI (ME)
1416 TF5000 Masterpiece
1426 TF5000t Masterpiece
1456 TF5010 Masterpiece
1501 TF5000CI (EUR)
10446 TF5200c
12406 TF5100c
13406 TF5100
*/
/*
#ifdef WIN32
return TF5000_BP_WP;
#endif               
        appendIntToLogfile("GetModel: sysID =%d",*sysID, WARNING);

        switch ( *sysID )
        {
        case 406:	return TF5000_5500;
        case 416:	return TF5000t;
        case 436:	return TF5010_5510;
        case 446:	return TF5000_BP_WP;
        case 456:	return TF5800t;
        case 466:	return TF5000t_BP_WP;
        case 486:	return TF5010_SE;
        case 501:	return TF5000CI_ME;
        case 1416:	return TF5000_MP;
        case 1426:	return TF5000t_MP;
        case 1456:	return TF5010_MP;
        case 1501:	return TF5000CI_EUR;
        case 10416:	return PC5101c_5102c;
        case 10426:	return PC5101t_5102t;
        case 10446:	return TF5200c;
        case 12406:	return TF5100c;
        case 13406:	return TF5100;
        }
        return TFOther;
}
*/

bool OsdActive (int startCol, int startRow, int endCol, int endRow)
{
    TYPE_OsdBaseInfo osdBaseInfo;
    dword* wScrn;
    dword iRow, iCol;

    TAP_Osd_GetBaseInfo( &osdBaseInfo );

    for ( iRow = startRow; iRow < endRow; iRow += 4 )  //every 4th line
    {
        wScrn = osdBaseInfo.eAddr + 720 * iRow;

        for ( iCol = startCol; iCol < endCol; iCol += 6 ) //may as well only scan every 6 pixels to save time and resource, stop at 350 as new f/w puts channel details to the right.
        {
            if ( (*(wScrn + iCol)) != 0 )
                return TRUE; // Do not allow Key Action
        }
    }
    return FALSE;
}


/*
// Finds the location of the INI file.
void LocateINIfileDir( void )
{
    char* currentPath;
    
    // Find the current path that we're in.
    currentPath = GetCurrentDir();
    
    //
    // FIRST - try /ProgramFiles/Settings
    //
    // Got to the /ProgramFiles directory
    GotoProgramFiles();
    // Check if the /ProgramFiles/Settings directory exists.  
    if (TAP_Hdd_Exist("Settings"))
    {
        TAP_Hdd_ChangeDir("Settings");               // Change to the "/ProgramFiles/Settings" directory.
        TAP_MemFree( currentPath );
        TAPIniDir = GetCurrentDir();                 // Store the directory location of the INI file.	
        return;    
    }
   


    //
    // SECOND - try <CurrentDir>/UK TAP Project
    //
    // Got back to the original directory
    GotoPath( currentPath );
    // Attempt to change to the "UK TAP Project" directory.
	TAP_Hdd_ChangeDir(PROJECT_DIRECTORY);  // Change to the UK TAP Project SubDirectory.

    // If the above fails, we fall through into the current directory, so use that as our location.
    TAPIniDir = GetCurrentDir();           // Store the directory location of the INI file.	

    // Free up any allocated memory.
    TAP_MemFree( currentPath );
}  



void DeterminePIN(char *pin)
{
  word       SysID = *((volatile word*)0xa3fffffa);
  extern int    _appl_version;
  dword      eEPROMOffset=0;
  word      nPin;

  if ((SysID == 1416) && (_appl_version == 0x1203)) eEPROMOffset=0x81b0ee2c;  // (5000 FW 5.12.03)
  if ((sysID == 456)  && (_appl_version == 0x1204)) eEPROMOffset=0x81bc3c8c;
  if ((sysID == 456)  && (_appl_version == 0x1205)) eEPROMOffset=0x81bc3e0c;
  if ((sysID == 456)  && (_appl_version == 0x1209)) eEPROMOffset=0x81c0e76c;
      
  //to be continued with more systems / versions

  if (eEPROMOffset == 0)
  {
    TAP_Print ("Unsupported firmware version.\n");
    pin[0]='\0'; 
    return;
  }


// Now simply read the pin from the EEPROM into a variable…

  nPin =  *((word*) (eEPROMOffset)+0x12);
  TAP_sPrint (pin,"%4.4d", nPin);
}
*/

