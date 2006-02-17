/************************************************************
				Part of the UK TAP Project
		This module contains some generic tools

Name	: Tools.c
Author	: Kidhazy
Version	: 0.2
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Kidhazy: 10-09-05	Inception Date
	  v0.1 sl8:	20-01-06	All variable initialised
	  v0.2 sl8:	16-02-06	sysID removed

**************************************************************/



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
	int		iNdx = 0;

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

//TAP_Print("Tapdir: fp.name = %s\r\n", fp.name);

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
	int	i = 0, lastpos = 0, startpos = 0, fieldLength = 0;
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
    
	char buff[128];

	strcpy( buff, recFile );       // Copy the filename into the buffer for checking.
	strlwr( buff );                // Convert the filename into all lowercase for easy comparison.
	
	if (( strcmp( buff + strlen( buff ) - 4, ".rec" ) == 0 ) && (attr == ATTR_TS) )
        	return TRUE;
	else
        	return FALSE;
}


//----------------  DIRECTORY TOOLS        ---------------------------------------------
// 
//--------------------------------------------------------------------------------------

bool GotoPath(char *path){
	char *startPos = NULL;
	char *endPos = NULL;
	bool ready = 0;

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



bool OsdActive (int startCol, int startRow, int endCol, int endRow)
{
    TYPE_OsdBaseInfo osdBaseInfo;
    dword* wScrn = NULL;
    dword iRow = 0, iCol = 0;

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
    char* currentPath = NULL;
    
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
//    TAP_Print ("Unsupported firmware version.\n");
    pin[0]='\0'; 
    return;
  }


// Now simply read the pin from the EEPROM into a variable…

  nPin =  *((word*) (eEPROMOffset)+0x12);
  TAP_sPrint (pin,"%4.4d", nPin);
}
*/

