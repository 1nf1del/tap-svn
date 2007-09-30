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
#define MSG_TEXT_COLOUR RGB8888(90,104,213)

#include "graphics/msgpopup360x130.GD"



//   ShowMessageWin 
//

//void    ShowMessageWin( char* msg1, char* msg2, int delay )
void    ShowMessageWin( word msgRgn, char* msg1, char* msg2, char* msg3, int delay )
{

    word   width, msgY;
    int    msgScrWidth;
    byte*  msgWindowCopy;
    
    msgScrWidth = MSG_SCREEN_W-10;
    msgY = MSG_SCREEN_Y+12;
      
    //  Calculate width of message window
    width = TAP_Osd_GetW( msg1, 0, FNT_Size_1926 );     
    if ( TAP_Osd_GetW( msg2, 0, FNT_Size_1622 ) + 10 > width ) width = TAP_Osd_GetW( msg2, 0, FNT_Size_1622 );
    if ( TAP_Osd_GetW( msg3, 0, FNT_Size_1622 ) + 10 > width ) width = TAP_Osd_GetW( msg3, 0, FNT_Size_1622 );

    if ( width > msgScrWidth ) width = msgScrWidth;

    // Save the region where we are about to display the pop-up.
    msgWindowCopy = TAP_Osd_SaveBox(msgRgn, MSG_SCREEN_X, MSG_SCREEN_Y, MSG_SCREEN_W, MSG_SCREEN_H);
#ifdef WIN32  // If testing on WIN32 platform 
TAP_Osd_FillBox( msgRgn,MSG_SCREEN_X, MSG_SCREEN_Y, MSG_SCREEN_W, MSG_SCREEN_H, FILL_COLOUR );				// clear the screen
#endif          

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


    TAP_Delay( delay );    

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

bool GotoPath(char *path){
	char *startPos;
	char *endPos;
	bool ready;

	// TAP_Print("GotoPath: going to root...\r\n");
	ChangeDirRoot();
	//GotoRoot();

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

    path = TAP_MemAlloc (2);

    if (path == NULL) return NULL;

    strcpy (path, "");

    // while we have a '.' entry we work up the tree matching starting clusters
    while (CurrentDirStartCluster (&cluster))
    {
        TYPE_File file;
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
            char* temp;

            temp = TAP_MemAlloc (strlen (path) + strlen (file.name) + 2);

            // no memory - back to starting directory and return NULL
            if (temp == NULL)
            {
                TAP_Hdd_ChangeDir (file.name);
                if (strlen (path)) TAP_Hdd_ChangeDir (&path[1]);
                TAP_MemFree (path);
                return NULL;
            }

            strcpy (temp, "/");
            strcat (temp, file.name);
            strcat (temp, path);
            TAP_MemFree (path);
            path = temp;
        }
        else
        {
            // directory structure inconsistent, shouldn't get here
            // problem - we can't get back to our starting directory
            TAP_MemFree (path);
            return NULL;
        }
    }
    if (strlen (path))
    {
        // finally we put ourselves back in our starting directory
        //TAP_Hdd_ChangeDir (&path[1]);
        GotoPath(path);
    }
    else
    {
        // We're at the root
        strcpy (path, "/");
    }

    return (path);
}


