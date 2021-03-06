/************************************************************
				Part of the UK TAP Project
		This module loads the Archive information

Name	: LoadArchiveInfo.c
Author	: Kidhazy
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Kidhazy: 18-10-05	Inception Date


	Last change:  USE   
**************************************************************/

#define MAX_FILES 200  // Define how many files in a single directory we will handle.
#define MAX_DIRS  100  // Define how many directories we will handle.
//#define MAX_FULL_DIR_NAME_LENGTH 200 // Define the maximum length of the full directory name.

#define PARENT_DIR_TEXT "Move up a directory"   // Define the text to be used for the parent directory.
#define PARENT_DIR_ATTR 250                     // Attribute value to set for the Parent Directory.
#define CURRENT_DIR_ATTR 240                    // Attribute value for the Current Directory.
#define NORMAL_PARENT_DIR_ATTR 242              // Standard attribute value for the Parent Directory.

#define BUFFER_SIZE 1500

int headerOffset;   // Variable used to cater for offset differences between TF5000 (0 offset) and the TF5800 (4 bytes offset)
int headerOffset2;  // Variable used to cater for offset differences between TF5000 (0 offset) and the TF5100 (1 bytes offset)

#define REC_DURATION_OFFSET 8
#define REC_DURATION_LENGTH 2
unsigned short recDuration;

#define SVC_NUM_OFFSET 10
unsigned short svc_num;

#define SVC_TYPE_OFFSET 13
unsigned short svc_type;

#define SERVICE_NAME_OFFSET 28 //TF5100 29
#define SERVICE_NAME_LENGTH 24
char serviceName[MAX_SvcName];

#define EVENT_DESC_MAX_LENGTH 256
char eventDescName[EVENT_DESC_MAX_LENGTH];

#define EVENT_DURATION_OFFSET 70
#define EVENT_DURATION_LENGTH 2
unsigned short eventDuration;

#define EVENT_STARTMJD_OFFSET 76
#define EVENT_STARTMJD_LENGTH 2
unsigned short eventStartMJD;

#define EVENT_STARTHOUR_OFFSET 78
unsigned short eventStartHour;

#define EVENT_STARTMIN_OFFSET 79
unsigned short eventStartMin;

#define EVENT_ENDMJD_OFFSET 80
#define EVENT_ENDMJD_LENGTH 2
unsigned short eventEndMJD;

#define EVENT_ENDHOUR_OFFSET 82
unsigned short eventEndHour;

#define EVENT_ENDMIN_OFFSET 83
unsigned short eventEndMin;

#define EVENT_NAME_LENGTH_OFFSET 85 //TF5800 89  TF5100 (subtract 1 from the length as event name has byte x05 and both fornt and end.
#define EVENT_NAME_LENGTH_LENGTH 1
unsigned short eventNameLength;

#define PARENTAL_RATING_OFFSET 86  // TF5800 90
#define PARENTAL_RATING_LENGTH 1
unsigned short parentalRating;

#define EVENT_NAME_OFFSET 87  // TF5800 91  TF5100 88
#define EVENT_NAME_MAX_LENGTH 256
char eventName[EVENT_NAME_MAX_LENGTH];

#define EXT_INFO_LENGTH_OFFSET 362
#define EXT_INFO_LENGTH_LENGTH 2
unsigned short extInfoLength;

#define EXT_INFO_OFFSET 368
#define EXT_INFO_MAX_LENGTH 1024
char extInfo[EXT_INFO_MAX_LENGTH];

// Define character values.
//  32=" "  35="#"  45="-"   46="."  47="/"  48="0"  57="9"  95="_"
#define CHAR_SPACE   32
#define CHAR_HASH    35
#define CHAR_ASTERIX 42
#define CHAR_PLUS    43
#define CHAR_DASH    45
#define CHAR_DOT     46
#define CHAR_SLASH   47
#define CHAR_ZERO    48
#define CHAR_NINE    57
#define CHAR_SCORE   95

typedef struct 
{
	dword	attr:8;
	dword	mjd:16;
	dword	hour:8;
	
	dword	min :8;
	dword	sec :8;
	dword	localOffset:16;
	
	dword	startCluster;
	dword	totalCluster;
	dword	unusedByte;
	char	name[ TS_FILE_NAME_SIZE ];
	char	sortName[ TS_FILE_NAME_SIZE ];
	char    directory[ MAX_FULL_DIR_NAME_LENGTH ];
	int     directoryNumber;
    byte    skip;
    byte    crypt;
    byte	playLst;
    byte    reserved[5];

	void	*handle;
	ulong64	size;
	dword 	currentPos;
	
    bool    isPlaying;      // Flag to indicate that this file is actually being played now.
    bool    hasPlayed;      // Flag to indicate whether file has been played or not.
    bool    isNew;          // Flag to indicate if the file is a new file since the last time Archive was activated.
	dword   totalBlock;     // Stores the total blocks in the file.
	dword   currentBlock;   // Stores the current playback block position.
	int     playIndex;      // Index entry to point to matching playedFiles array
	 
	int     svcNum;
	int     svcType;
	int     numberOfFiles;
	int     numberOfFolders;
	int     recDuration;
	dword   recordedSec;
    char    serviceName[MAX_SvcName];
	int     eventNameLength;
	int     parentalRating;
    char    eventName[EVENT_NAME_MAX_LENGTH];
    char    eventDescName[EVENT_DESC_MAX_LENGTH];
    int     extInfoLength;
    char    extInfo[EXT_INFO_MAX_LENGTH];
    int     eventDuration;
    int     eventStartMJD;
    int     eventStartHour;
    int     eventStartMin;
    int     eventEndMJD;
    int     eventEndHour;
    int     eventEndMin;
    bool    isRecording;  // Flag to indicate if this file is actually being recorded now.
    bool    present;      // Flag to indicate if the file is present when we activate the TAP - if not it may have been deleted outside of the TAP.
} TYPE_My_Files;

typedef struct 
{
    char	name[ TS_FILE_NAME_SIZE ];
    int     directoryNumber;
    int     numberOfFiles;
    int     numberOfFolders;
    int     numberOfRecordings;
    int     parentDirNumber;
    int     newRecs;             // Counter to indicate how many new files this folder holds since the last time Archive was activated.
    int     newRecDirs;          // Counter to indicate how many subfolders from this folder holds new recordings since the last time Archive was activated.
	dword	lastViewMjd:16;      // Store the variables that indicate when this folder was last viewed.
	dword	lastViewHour:8;
	dword	lastViewMin :8;
	dword	lastViewSec :8;
    
    
} TYPE_Dir_List;

typedef struct 
{
	char	name[ TS_FILE_NAME_SIZE ];
	dword	startCluster;   // Starting disk cluster for the file - used with the filename so that we can locate a unique file.
	dword   totalBlock;     // Stores the total blocks in the file.
	dword   currentBlock;   // Stores the current playback block position.
} TYPE_Played_Files;


TYPE_My_Files *myfiles[MAX_DIRS][MAX_FILES], currentFile;  // Establish a global array that will hold the file details for all the files in the current directory.
TYPE_Dir_List *myfolders[MAX_DIRS], currentFolder, folderLastViewInfo[MAX_DIRS];
TYPE_File     file, blankFile;
TYPE_RecInfo  recInfo[2];
TYPE_Played_Files *playedFiles[MAX_FILES];

void CreateBlankFile(void)
{
     // Set the standard settings.
/*     
     blankfile.attr         = file.attr;
     blankFile.mjd          = file.mjd;
     blankFile.hour         = file.hour;
     blankFile.min          = file.min;
     blankFile.sec          = file.sec;
     blankFile.localOffset  = file.localOffset;
     blankfile.startCluster = file.startCluster;
     blankfile.name,file.name);
     blankfile.directory, directory);
     blankfile.directoryNumber = dirNumber;
     blankfile.crypt        = file.crypt;
     blankFile.size         = file.size;
     blankFile.currentPos   = file.currentPos;
     blankfile.isRecording  = FALSE;   // Default the isRecording flag to FALSE.
     blankfile.isPlaying    = FALSE;   // Default the isPlaying flag to FALSE.
*/
     memset(&blankFile,0,sizeof (blankFile));   // Make blankFile or zeroes.
}


void CountFolderFilesAndSubfolders(char *NewDir, int *files, int *subfolders)
{
     int   totalCnt, i;
     TYPE_File tempFile;
     
     *files = 0;
     *subfolders = 0;
     
     TAP_Hdd_ChangeDir(NewDir);  // change to the directory that is to be counted.
     totalCnt = TAP_Hdd_FindFirst(&tempFile); 
    
     for ( i=1; i <= totalCnt ; i += 1 )
	 {
          if (IsFileRec(tempFile.name, tempFile.attr))  *files += 1;   // Increase the count of files.
          if (tempFile.attr == ATTR_FOLDER) *subfolders += 1;   // Increase the count of subfolders.
          TAP_Hdd_FindNext(&tempFile); 
     }

     TAP_Hdd_ChangeDir("..");  // return to the parent directory 
//     GotoPath(currentDir);  // Return to the directory that we started from.
}
     
     

void LoadHeaderInfo(char filename[ TS_FILE_NAME_SIZE ], int dir, int index)
{

    TYPE_File   *fp;
    byte        *buf;

    char  str[259];
    char  str1[2500];
    word year;
    byte  month, day, weekDay;

    appendToLogfile("LoadHeaderInfo: Started.", INFO);

    fp = TAP_Hdd_Fopen( filename ); 
    if( fp==NULL) 
    {
            appendStringToLogfile("LoadHeaderInfo: Failure opening filename=%s<<",filename, ERR);
            appendToLogfile("LoadHeaderInfo: Exiting.", ERR);
            return;
    }        

    buf = (byte *)TAP_MemAlloc( BUFFER_SIZE );
    if( buf == 0 ) { TAP_Hdd_Fclose( fp ); return ; }
    TAP_Hdd_Fread( buf, BUFFER_SIZE, 1, fp );
    TAP_Hdd_Fclose( fp );

    myfiles[dir][index]->recDuration = buf[REC_DURATION_OFFSET]*256 + buf[REC_DURATION_OFFSET+1];

    myfiles[dir][index]->svcNum = buf[SVC_NUM_OFFSET]*256 + buf[SVC_NUM_OFFSET+1];

    myfiles[dir][index]->svcType = buf[SVC_TYPE_OFFSET];

    memcpy(myfiles[dir][index]->serviceName,(buf+SERVICE_NAME_OFFSET+headerOffset2),SERVICE_NAME_LENGTH);   
    myfiles[dir][index]->serviceName[SERVICE_NAME_LENGTH]='\0';

    myfiles[dir][index]->parentalRating = buf[PARENTAL_RATING_OFFSET+headerOffset];
    
    myfiles[dir][index]->eventNameLength = buf[EVENT_NAME_LENGTH_OFFSET+headerOffset]-headerOffset2;
    myfiles[dir][index]->eventNameLength = min( myfiles[dir][index]->eventNameLength, EVENT_NAME_MAX_LENGTH );  //Make sure we don't exceed our limit.

    memcpy(myfiles[dir][index]->eventName,(buf+EVENT_NAME_OFFSET+headerOffset+headerOffset2),myfiles[dir][index]->eventNameLength);   
    myfiles[dir][index]->eventName[myfiles[dir][index]->eventNameLength]='\0';

    memcpy(myfiles[dir][index]->eventDescName,(buf+EVENT_NAME_OFFSET+headerOffset+headerOffset2+headerOffset2+myfiles[dir][index]->eventNameLength),EVENT_NAME_MAX_LENGTH);   

    myfiles[dir][index]->extInfoLength = buf[EXT_INFO_LENGTH_OFFSET+headerOffset]*256 + buf[EXT_INFO_LENGTH_OFFSET+headerOffset+1];
    myfiles[dir][index]->extInfoLength = min( myfiles[dir][index]->extInfoLength, EXT_INFO_MAX_LENGTH );  //Make sure we don't exceed our limit.
    //extInfo = "This is the Extended Event Information. I have put more information in here to see what that does to the scrolling.  I was hoping that this could get across multiple pages to see what happens after you do multiple Info key presses. You may find that this goes over multiple lines and wrap around at the word boundary.  The quick brown fox jumps over the lazy dog. You will often find multiple pages particularly when you look at the SBS channel.  The ABC is another channel where you have several pages of extended information.  I still need to look at what happens in the routine if we have a zero length string or a really long string that fills the entire line.";
    
    memcpy(myfiles[dir][index]->extInfo,(buf+EXT_INFO_OFFSET+headerOffset), myfiles[dir][index]->extInfoLength);   
    myfiles[dir][index]->extInfo[myfiles[dir][index]->extInfoLength]='\0';

    // I think this is wrong and 1st byte=hours, 2nd byte= minutes
    myfiles[dir][index]->eventDuration = buf[EVENT_DURATION_OFFSET+headerOffset]*256 + buf[EVENT_DURATION_OFFSET+headerOffset+1];

    myfiles[dir][index]->eventStartMJD = buf[EVENT_STARTMJD_OFFSET+headerOffset]*256 + buf[EVENT_STARTMJD_OFFSET+headerOffset+1];

    myfiles[dir][index]->eventStartHour = buf[EVENT_STARTHOUR_OFFSET+headerOffset];
    
    myfiles[dir][index]->eventStartMin = buf[EVENT_STARTMIN_OFFSET+headerOffset];
    
    myfiles[dir][index]->eventEndMJD = buf[EVENT_ENDMJD_OFFSET+headerOffset]*256 + buf[EVENT_ENDMJD_OFFSET+headerOffset+1];

    myfiles[dir][index]->eventEndHour = buf[EVENT_ENDHOUR_OFFSET+headerOffset];
    
    myfiles[dir][index]->eventEndMin = buf[EVENT_ENDMIN_OFFSET+headerOffset];
  
    appendToLogfile("LoadHeaderInfo: Finished.", INFO);

}




void SetPlaybackStatus(int dirNumber, int fileIndex)
{
    int i;
    
    for (i=1; i <= numberOfPlayedFiles; i += 1)
    {
        // If the file has the same filename and the same disk startcluster we consider it a match.
        if ((myfiles[dirNumber][fileIndex]->startCluster == playedFiles[i]->startCluster) && (strcmp(myfiles[dirNumber][fileIndex]->name,playedFiles[i]->name)==0))
        {
              myfiles[dirNumber][fileIndex]->hasPlayed = TRUE;
              myfiles[dirNumber][fileIndex]->currentBlock = playedFiles[i]->currentBlock;
              myfiles[dirNumber][fileIndex]->totalBlock = playedFiles[i]->totalBlock;
              break; // We've found a match, so don't bother checking other playback entries.
        }              
    }   
}


void GetPlaybackInfo( void )
{
   // Reset playback flag and current playback file information.  
   inPlaybackMode      = FALSE;
   CurrentPlaybackFile = &blankFile;
  
   // Check for Playbackmode
   iOverRunCheck = ApiOverRunCheck;   // Set flag to catch any buffer overrun from API call.
   TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo);

   if ( iOverRunCheck != ApiOverRunCheck )  // TAP_Hdd_GetPlayInfo V1.22 has a buffer overrun issue, so check if it has overrun by more than 128 bytes.
   {
      ShowMessageWin( rgn, "Archive:  Error retrieving Playback Info", "TAP_Hdd_GetPlayInfo buffer overrun", "in 'GetPlaybackInfo'.", 400 );
      return;
   }

   // Check the various playmodes to see if we are playing a file back.
   // PLAYMODE_None               Nothing playing
   // PLAYMODE_Playing            Playing a file
   // PLAYMODE_RecPlaying         Playing a currently recording file
   //
   // If we're not in playback then just return.
   if (CurrentPlaybackInfo.playMode != PLAYMODE_Playing)  return;
   
   // Assign variable to the file entry for easier use.
   CurrentPlaybackFile = CurrentPlaybackInfo.file;
   CurrentPlaybackEvent = CurrentPlaybackInfo.evtInfo;
   
   inPlaybackMode = TRUE;
}


void LoadPlaybackInfo( int dir, int index )
{   
   TYPE_TapChInfo  chInfo;
   int durHour, durMin;

   appendToLogfile("LoadPlaybackInfo: Started.", INFO);

   // We match the current playback file based on disk startcluster and filename. 
   if ((myfiles[dir][index]->startCluster == CurrentPlaybackFile->startCluster) && (strncmp(myfiles[dir][index]->name,CurrentPlaybackFile->name, TS_FILE_NAME_SIZE)==0))
   {
         appendStringToLogfile("LoadPlaybackInfo: Match on CurrentPlaybackFile.name=%s<<",CurrentPlaybackFile->name, WARNING);
         appendStringToLogfile("LoadPlaybackInfo: for myfiles->name=%s<<",myfiles[dir][index]->name, WARNING);

         myfiles[dir][index]->recDuration = CurrentPlaybackInfo.duration;
         myfiles[dir][index]->svcNum      = CurrentPlaybackInfo.svcNum;
         myfiles[dir][index]->svcType     = CurrentPlaybackInfo.svcType;
/*
         TAP_Channel_GetInfo( myfiles[dir][index]->svcType, myfiles[dir][index]->svcNum, &chInfo);
         strcpy(myfiles[dir][index]->serviceName,chInfo.chName);
         
         myfiles[dir][index]->parentalRating = CurrentPlaybackEvent.parentalRating;
    
//         strcpy(myfiles[dir][index]->eventName, CurrentPlaybackEvent.eventName);
//         strcpy(myfiles[dir][index]->eventDescName, CurrentPlaybackEvent.description);

//         myfiles[dir][index]->extInfoLength = 0;
//         myfiles[dir][index]->extInfo[0]='\0';

         myfiles[dir][index]->eventDuration  = ((CurrentPlaybackEvent.duration>>8)&0xff) + (CurrentPlaybackEvent.duration&0xff);  

         myfiles[dir][index]->eventStartMJD  = ((CurrentPlaybackEvent.startTime>>16)&0xffff);
         myfiles[dir][index]->eventStartHour = ((CurrentPlaybackEvent.startTime>>8)&0xff);
         myfiles[dir][index]->eventStartMin  = ((CurrentPlaybackEvent.startTime)&0xff);

         myfiles[dir][index]->eventEndMJD    = ((CurrentPlaybackEvent.endTime>>16)&0xffff);
         myfiles[dir][index]->eventEndHour   = ((CurrentPlaybackEvent.endTime>>8)&0xff);
         myfiles[dir][index]->eventEndMin    = ((CurrentPlaybackEvent.endTime)&0xff);
*/         
         myfiles[dir][index]->isPlaying      = TRUE;
   }     
   else
         myfiles[dir][index]->isPlaying      = FALSE;
        
   appendToLogfile("LoadPlaybackInfo: Finished.", INFO);
}


void LoadRecordingInfo(int dir, int index)
{
    // 
    // Compares the information from the two recording slots and if they match the specified 'myfiles' entry, loads that
    // information into 'myfiles'. 
    //
    int i;
    TYPE_TapChInfo  chInfo;
    
    appendToLogfile("LoadRecordingInfo: Checking recordings.", INFO);
    
    for (i=0; i <= 1; i += 1)
    {
        if (strncmp(myfiles[dir][index]->name, recInfo[i].fileName, TS_FILE_NAME_SIZE)==0) 
        {
                appendStringToLogfile("LoadRecordingInfo: Match on recInfo.filename=%s<<",recInfo[i].fileName, WARNING);
                appendStringToLogfile("LoadRecordingInfo: for myfiles->name=%s<<",myfiles[dir][index]->name, WARNING);
                myfiles[dir][index]->isRecording    = TRUE;  
                myfiles[dir][index]->svcNum         = recInfo[i].svcNum;  
                myfiles[dir][index]->svcType        = recInfo[i].svcType; 
                
                TAP_Channel_GetInfo( myfiles[dir][index]->svcType, myfiles[dir][index]->svcNum, &chInfo);
                strcpy(myfiles[dir][index]->serviceName,chInfo.chName);
                
                myfiles[dir][index]->recDuration   = recInfo[i].duration;  
                myfiles[dir][index]->recordedSec   = recInfo[i].recordedSec;  
                
                myfiles[dir][index]->eventStartMJD  = ((recInfo[i].startTime>>16)&0xffff);  
                myfiles[dir][index]->eventStartHour = ((recInfo[i].startTime>>8)&0xff);  
                myfiles[dir][index]->eventStartMin  =  (recInfo[i].startTime&0xff);  
                                 
                myfiles[dir][index]->eventEndMJD    = ((recInfo[i].endTime>>16)&0xffff);  
                myfiles[dir][index]->eventEndHour   = ((recInfo[i].endTime>>8)&0xff);
                myfiles[dir][index]->eventEndMin    =  (recInfo[i].endTime&0xff);    
                break;  // Found our match, so no need to keep checking.
        }
        else
                myfiles[dir][index]->isRecording    = FALSE;          
    }   
    appendToLogfile("LoadRecordingInfo: Finished checking recordings.", INFO);

}

void GetRecordingInfo()
{
    int i;
     
    // Get the names of the recordings that may be running.
    appendToLogfile("GetRecordingInfo: Started.", INFO);
    for ( i=0; i <=1 ; i += 1)
    {
        memset (&recInfo[i],0,sizeof (recInfo[i]));
        if (TAP_Hdd_GetRecInfo (i, &recInfo[i]) && ((recInfo[i].recType == RECTYPE_Normal) || (recInfo[i].recType == RECTYPE_Copy)))
        {
            // Keep the recording information as it is.
            appendIntToLogfile("GetRecordingInfo: Recording slot #%d is running.",i, WARNING);
        }
        else
        {
            appendIntToLogfile("GetRecordingInfo: Recording slot #%d is NOT running.",i, WARNING);
            TAP_SPrint(recInfo[i].fileName,"");   // Blank out the name.
        }
    }    
    appendToLogfile("GetRecordingInfo: Finished.", INFO);
}



bool MatchFileStamp( char* stamp, char* str)
{
     int d, l, start, stampLen, fileLen;
     char weekDay[12];
     
     stampLen = strlen( stamp );
     fileLen  = strlen( str ); // Length of filename.
     start    = fileLen - stampLen; // starting position to check in filename.
         
     // Handle the " DDD" date stamp.
     if ( strcmp( stamp, " DDD" ) == 0 )
     {
        for ( d = 0; d < 7; d++ )               
        {
            WeekdayToAlpha(d, weekDay);
            if ( strncmp( str + start + 1, weekDay, 3 ) == 0 )
                return TRUE;
        }
        return FALSE;
     }
     else
     // Handle the "- DDD" date stamp.
     if ( strcmp( stamp, "- DDD" ) == 0 )
     {
        for ( d = 0; d < 7; d++ )               
        {
            WeekdayToAlpha(d, weekDay);
            if ( strncmp( str + start + 2, weekDay, 3 ) == 0 )
                return TRUE;
        }
        return FALSE;
     }
     else
     {          
        // Handle all other date stamps.          
        for (l=0; l<stampLen; l++)
        {
            switch ( stamp[l] )
            {
                   case '*':   
                               if ( (str[start+l] != CHAR_DASH ) && (str[start+l] != CHAR_SCORE ) && ( str[start+l] != CHAR_DOT) && ( str[start+l] != CHAR_SLASH))
                                  return FALSE;
                               break;
                   case ' ':
                               if ( (str[start+l] != CHAR_SPACE ) && (str[start+l] != CHAR_SCORE ))
                                  return FALSE;
                               break;
                   case '+':
                               if ( str[start+l] != CHAR_PLUS )
                                  return FALSE;
                               break;
                   case '#':
                               if ( str[start+l] != CHAR_HASH )
                                  return FALSE;
                               break;
                   case '-':
                               if ( str[start+l] != CHAR_DASH )
                                  return FALSE;
                               break;
                   case '.':
                               if ( str[start+l] != CHAR_DOT )
                                  return FALSE;
                               break;
                   case '/':
                               if ( str[start+l] != CHAR_SLASH )
                                  return FALSE;
                               break;
                   case '_':
                               if ( str[start+l] != CHAR_SCORE )
                                  return FALSE;
                               break;
                   case '9':   
                               if (( str[start+l] < CHAR_ZERO ) || ( str[start+l] > CHAR_NINE))
                                  return FALSE;
                               break;
                   case 'D':
                   case 'M':
                   case 'Y':
                               if ((str[start+l] < CHAR_ZERO) || (str[start+l] > CHAR_NINE)) 
                                  return FALSE;
                               break;
                }
            }
     }  
     // TAP_Print("%s< matched on %s<\r\n",str,stamp);           
     return TRUE;            
}



void FormatSortName(char* target, char* source)
{
#define NUMBER_OF_STAMPS 17   
  
/*
* = -_./ space
9 = 0-9

*99*99 YYYY*MM*DD
*YYYYMMDD_9999
*YYYYMMDD
YYYYMMDD
*YYYY*MM*DD
YYYY*MM*DD
*YY*MM*DD (also matchs *DD*MM*YY )
YY*MM*DD
*DDMMYY
DDMMYY
*DD*MM
DD*MM
- DDD
*DDD
DDD

-99
_99
-9
_9

_+
-+
_#
-#



*/
//     char stamps[NUMBER_OF_STAMPS][10] = {"_99-99 YYYY-MM-DD", "_#", "-#", "-9", "-99", "_+", "-+", " DD/MM/YY", " DD-MM-YY", "DD.MM.YY", " DD/MM", " DD-MM", " MMDD", "- DDD", " DDD", " YYYYMMDD", "DDMMYY"};
//     char stamps[NUMBER_OF_STAMPS][10] = {"_99-99 YYYY-MM-DD", " DD/MM/YY", " DD-MM-YY", " DD.MM.YY", " YYYYMMDD", " DDMMYY", "DD/MM/YY", "DD-MM-YY", "DD.MM.YY", "YYYYMMDD", " DDMMYY", " DD/MM", " DD-MM", " MMDD", "- DDD", " DDD", "_#", "-#", "-9", "-99", "_+", "-+"};
     char stamps[NUMBER_OF_STAMPS][20] = {"_#", "-#", "_+", "-+", " YYYY*MM*DD", "*99*99 YYYY*MM*DD", "*YYYYMMDD_9999", "*YYYYMMDD", "*YYYY*MM*DD",  "*YY*MM*DD",  "*DDMMYY",  "*DD*MM",  "*MMDD",  "- DDD", " DDD", "-99", "-9"};
     char stampStr[10];
     int i, start, stampLen, fileLen;

     strcpy( target, source);                   // Copy the whole filename by default.
     fileLen = strlen(target);                  // Get the length of the filename.
     target[fileLen - tagLength]='\0';          // Chop off the ".rec" or ".rec.del" from the filename.
                
     for (i=0; i < NUMBER_OF_STAMPS; i++)       // Go through each date/time stamp and see if we have a matching filename
     {
         if (MatchFileStamp(stamps[i], target)) // We've found a match.
         {
            stampLen = strlen( stamps[i] );     // Get the length of the date/time stamp.
            fileLen  = strlen( target );        // Get the length of filename.
            start    = fileLen - stampLen;      // Work out where the real file name ends and date/time stamp begins.
            target[start]='\0';                 // Terminate the filename at the date/time stamp beginning.
           // break;
         } 
     }
}


void AddCommonInfo(char* directory, int dirNumber, int index, TYPE_File file )
{
        // Set the standard settings.
        myfiles[dirNumber][index]->attr         = file.attr;
        myfiles[dirNumber][index]->mjd          = file.mjd;
        myfiles[dirNumber][index]->hour         = file.hour;
        myfiles[dirNumber][index]->min          = file.min;
        myfiles[dirNumber][index]->sec          = file.sec;
        myfiles[dirNumber][index]->localOffset  = file.localOffset;
        myfiles[dirNumber][index]->startCluster = file.startCluster;
        strcpy(myfiles[dirNumber][index]->name,file.name);
        strcpy(myfiles[dirNumber][index]->directory, directory);
        myfiles[dirNumber][index]->directoryNumber = dirNumber;
        myfiles[dirNumber][index]->crypt        = file.crypt;
        myfiles[dirNumber][index]->size         = file.size;
        myfiles[dirNumber][index]->currentPos   = file.currentPos;
        myfiles[dirNumber][index]->isRecording  = FALSE;   // Default the isRecording flag to FALSE.
        myfiles[dirNumber][index]->isPlaying    = FALSE;   // Default the isPlaying flag to FALSE.
        myfiles[dirNumber][index]->present      = TRUE;    // Mark it as being present
}

bool AfterLastView(char* directory, TYPE_File file)
{
     // Routine will check if the specified file has a date/time after the Last View date/time of the specified directory.
     
     int i;
     
     for (i = 0; i <= numberOfFolders; i++)
     {
         // See if this is a matching directory name.
         if (strncmp( directory, folderLastViewInfo[i].name, MAX_FULL_DIR_NAME_LENGTH ) == 0) 
         {
//TAP_Print("Dir match i=%d %s fileMJD=%d folderMJD=%d\r\n",i,folderLastViewInfo[i].name,file.mjd, folderLastViewInfo[i].lastViewMjd);        
//TAP_Print("%d %d %d %d \r\n",folderLastViewInfo[i].lastViewMjd,folderLastViewInfo[i].lastViewHour,folderLastViewInfo[i].lastViewMin,folderLastViewInfo[i].lastViewSec);
//TAP_Print("%d %d %d %d \r\n",file.mjd,file.hour,file.min,file.sec);
              // Check the MJD and time of the file to see if it is after the Last View date/time.        
              if ( file.mjd  > folderLastViewInfo[i].lastViewMjd  )  return TRUE;   // The file date is after  the Last View Date.
              if ( file.mjd  < folderLastViewInfo[i].lastViewMjd  )  return FALSE;  // The file date is before the Last View Date.
              // At this point the file DATE = Last view DATE
              if ( file.hour > folderLastViewInfo[i].lastViewHour )  return TRUE;   // The file hour is after  the Last View Date.
              if ( file.hour < folderLastViewInfo[i].lastViewHour )  return FALSE;  // The file hour is before the Last View Date.
              // At this point the file HOUR = Last view HOUR
              if ( file.min  > folderLastViewInfo[i].lastViewMin  )  return TRUE;   // The file min  is after  the Last View Date.
              if ( file.min  < folderLastViewInfo[i].lastViewMin  )  return FALSE;  // The file min  is before the Last View Date.
              if ( file.sec  > folderLastViewInfo[i].lastViewSec  )  return TRUE;   // The file sec  is after  the Last View Date.
              if ( file.sec  < folderLastViewInfo[i].lastViewSec  )  return FALSE;  // The file sec  is before the Last View Date.
              
              return FALSE;   // Return FALSE if the file date/time is the same as the directory Last View.
         }
                            
     }
     
     // No match found, then we assume this is a new file, so return TRUE.
     return TRUE;
}

void CascadeFolderHasNewChange( int dirNumber, int amount )
{
     // Routine will recursively climb up the myfolders directory structure, changing the "hasNew" recordings flag in each of the parent directories.
     
     while (dirNumber != 0)
     {
           
             myfolders[dirNumber]->newRecDirs = myfolders[dirNumber]->newRecDirs + amount;   // Modifiy the number of subfolders holding new recordings. 
             dirNumber = myfolders[dirNumber]->parentDirNumber;
     }
     
}


void AddNewFile(char* directory, int dirNumber, int index, TYPE_File file)
{

        // Blank out existing variable space.
        myfiles[dirNumber][index] = TAP_MemAlloc(sizeof (*myfiles[dirNumber][index]));
        if (myfiles[dirNumber][index] == NULL)
        {
               ShowMessageWin( rgn, "TAP Memory Allocation Failure", "Unable to allocate enough memory", "for 'myfiles' in 'AddNewFile'", 500);
               return;
        }
        memset(myfiles[dirNumber][index],0,sizeof (*myfiles[dirNumber][index]));

        AddCommonInfo( directory, dirNumber, index, file);

        #ifdef WIN32   // If testing on WIN32 platform set some file date/time attributes as not yet implemented in the TAP SDK.
             file.mjd  = 54042;
             file.hour = 8;
             file.min  = 0;
             file.sec  = 0;
        #endif
        // If we are checking at an initial load of Archive data, check if this file has a date after the Last Viewed date/time for this directory.                                   
        if (!recycleWindowMode)
        {
            if (AfterLastView(directory, file) )
            {
                if (myfolders[dirNumber]->newRecs == 0)   // This is the first 'new' recording for this folder, so increase count up parent directories.
                    CascadeFolderHasNewChange(myfolders[dirNumber]->parentDirNumber, +1);
                myfiles[dirNumber][index]->isNew = TRUE;         // Mark it as being a new file since Archive was last activated.
                myfolders[dirNumber]->newRecs++;                 // Increase the number of new recordings in this folder.
            }
        }
/*      if (initialLoad) 
        {
            if (AfterLastView(directory, file) )
            {
               if (myfolders[dirNumber]->newRecs == 0) // This is the first 'new' recording for this folder, so increase count up parent directories.
                  CascadeFolderHasNewChange(myfolders[dirNumber]->parentDirNumber, +1);
               myfiles[dirNumber][index]->isNew = TRUE;         // Mark it as being a new file since Archive was last activated.
               myfolders[dirNumber]->newRecs++;                 // Increase the number of new recordings in this folder.
            }   
        }
        else   // Assume TAP has already started, so this must be a new file - if we are not in Recycle Bin mode.
        if (!recycleWindowMode)
        {
//            if (AfterLastView(directory, file) )
//            {
            if (myfolders[dirNumber]->newRecs == 0)   // This is the first 'new' recording for this folder, so increase count up parent directories.
                CascadeFolderHasNewChange(myfolders[dirNumber]->parentDirNumber, +1);
            myfiles[dirNumber][index]->isNew = TRUE;         // Mark it as being a new file since Archive was last activated.
            myfolders[dirNumber]->newRecs++;                 // Increase the number of new recordings in this folder.
//            }
        }
*/              
        if (recCheckOption ==1)   // If option to ignore Attribute in recording check is set, then force the TS Attribute in case this recording doesn't have it set.
           myfiles[dirNumber][index]->attr = ATTR_TS;   
        FormatSortName(myfiles[dirNumber][index]->sortName,file.name);
}

void SetLastView(char* directoryName, int dirNumber)
{
     // Routine that will set the Last View date/time for a new folder based on saved information.
     
     int i;
     
     for (i = 0; i <= numberOfFolders; i++)
     {
         // See if this is a matching directory name.
         if (strncmp( directoryName, folderLastViewInfo[i].name, MAX_FULL_DIR_NAME_LENGTH ) == 0) 
         {
                      
//TAP_Print("Dir match i=%d %s fileMJD=%d folderMJD=%d\r\n",i,folderLastViewInfo[i].name,file.mjd, folderLastViewInfo[i].lastViewMjd);        
//TAP_Print("%d %d %d %d \r\n",folderLastViewInfo[i].lastViewMjd,folderLastViewInfo[i].lastViewHour,folderLastViewInfo[i].lastViewMin,folderLastViewInfo[i].lastViewSec);
//TAP_Print("%d %d %d %d \r\n",file.mjd,file.hour,file.min,file.sec);
//TAP_Print("Dir match i=%d dir=%s< %s Match=%s\r\n",i,directoryName,folderLastViewInfo[i].name,myfolders[dirNumber]->name);
              myfolders[dirNumber]->lastViewMjd  = folderLastViewInfo[i].lastViewMjd;
              myfolders[dirNumber]->lastViewHour = folderLastViewInfo[i].lastViewHour;
              myfolders[dirNumber]->lastViewMin  = folderLastViewInfo[i].lastViewMin;
              myfolders[dirNumber]->lastViewSec  = folderLastViewInfo[i].lastViewSec;
              return;   // We found our match, so no need to check any more.
         }
                            
     }
}



void AddNewFolder(char* parentDirectoryName, int dirNumber, int index, TYPE_File file, int newFolderNumber)
{
        char newDirectoryName[MAX_FULL_DIR_NAME_LENGTH] ;
        

        // Allocate & blank out existing new 'myfiles' entry.
        myfiles[dirNumber][index] = TAP_MemAlloc(sizeof (*myfiles[dirNumber][index]));
        if (myfiles[dirNumber][index] == NULL)
        {
               ShowMessageWin( rgn, "TAP Memory Allocation Failure", "Unable to allocate enough memory", "for 'myfiles' in 'AddNewFolder'", 500);
               return;
        }
        memset(myfiles[dirNumber][index],0,sizeof (*myfiles[dirNumber][index]));

        AddCommonInfo( parentDirectoryName, dirNumber, index, file);
        myfiles[dirNumber][index]->size = 0;
        strcpy(myfiles[dirNumber][index]->sortName,file.name);

        myfiles[dirNumber][index]->directoryNumber = newFolderNumber;  // Give each new directory a new number.
             
        // Allocate & blank out existing new 'myfolders' entry.
        myfolders[newFolderNumber] = TAP_MemAlloc( sizeof  (*myfolders[newFolderNumber])); 
        if (myfolders[newFolderNumber]  == NULL)
        {
               ShowMessageWin( rgn, "TAP Memory Allocation Failure", "Unable to allocate enough memory", "for 'myfolders' in 'AddNewFolder'", 500);
               return;
        }
        memset(myfolders[newFolderNumber],0,sizeof (*myfolders[newFolderNumber]));

        // Make new folder entry in the "myfolders" array.
        strcpy(myfolders[newFolderNumber]->name, file.name);                   // Save the partial directory name.
        myfolders[newFolderNumber]->directoryNumber    = newFolderNumber;      // Save the new directory number.
        myfolders[newFolderNumber]->parentDirNumber    = dirNumber;            // Save the parent directory number.
        myfolders[newFolderNumber]->numberOfFiles      = 0;
        myfolders[newFolderNumber]->numberOfFolders    = 0;
        myfolders[newFolderNumber]->numberOfRecordings = 0;
        
        TAP_SPrint(newDirectoryName, "%s/%s",parentDirectoryName,file.name);   // Create the new fully qualified directory name.
        if (tapStartup) SetLastView( newDirectoryName, newFolderNumber);       // If this is adding a folder during TAP startup, set any Last View Date/Times.
}

void AddNewParentFolder(char* directory, int dirNumber, int index, int parentDirNumber)
{
        // Blank out existing variable space.
        myfiles[dirNumber][index] = TAP_MemAlloc(sizeof (*myfiles[dirNumber][index]));
        if (myfiles[dirNumber][index] == NULL)
        {
               ShowMessageWin( rgn, "TAP Memory Allocation Failure", "Unable to allocate enough memory", "for 'myfiles' in 'AddNewParentFolder'", 500);
               return;
        }
        memset(myfiles[dirNumber][index],0,sizeof (*myfiles[dirNumber][index]));

        myfiles[dirNumber][index]->attr             = PARENT_DIR_ATTR;  // Normal attribute for parent directory is 240.  Make it 250 to allow easy sorting.
        myfiles[dirNumber][index]->directoryNumber  = parentDirNumber;
        myfiles[dirNumber][index]->present          = TRUE;             // Mark it as being present
        myfiles[dirNumber][index]->mjd              = 0;                // Don't give it a date.
        myfiles[dirNumber][index]->hour             = 0;                // Don't give it a hour.
        myfiles[dirNumber][index]->min              = 0;                // Don't give it a minute.        
        myfiles[dirNumber][index]->size             = 0;                // Don't give it a size.        
        strcpy( myfiles[dirNumber][index]->name,      PARENT_DIR_TEXT );
        strcpy( myfiles[dirNumber][index]->sortName,  PARENT_DIR_TEXT );
        strcpy( myfiles[dirNumber][index]->directory, directory );
}


bool FileExists(char* directory, TYPE_File file, int dirNumber, int fileIndex)
{
//TAP_Print("FE Comparing to %s< dn=%d, fi=%d\r\n",myfiles[dirNumber][fileIndex]->name,dirNumber,fileIndex);
//    TAP_Print("IN FE\r\n");
 
    if (myfiles[dirNumber][fileIndex] == NULL)           
    {
        myfiles[dirNumber][fileIndex] = TAP_MemAlloc(sizeof (*myfiles[dirNumber][fileIndex]));
        if (myfiles[dirNumber][fileIndex] == NULL)
        {
               ShowMessageWin( rgn, "TAP Memory Allocation Failure", "Unable to allocate enough memory", "for 'myfiles' in 'FileExists'", 500);
               return;
        }
        memset(myfiles[dirNumber][fileIndex],0,sizeof (*myfiles[dirNumber][fileIndex]));
    }
    // If the directory name is different it's not a match.
    if (strncmp( directory, myfiles[dirNumber][fileIndex]->directory, MAX_FULL_DIR_NAME_LENGTH ) != 0) return FALSE;
    
    // Check for the existance of the file/folder.
    switch (file.attr)
    {
           case ATTR_FOLDER:   // We're looking for a folder, so only match on name.
                               if  (strncmp( file.name,  myfiles[dirNumber][fileIndex]->name, TS_FILE_NAME_SIZE) == 0)
                               {
                                   myfiles[dirNumber][fileIndex]->present = TRUE;                  // Mark it as being present
                                   return TRUE;
                               }        
                               break;
           
           default:            // We're looking for a file, so match on name, date, time and size.
                               if ((strncmp( file.name,  myfiles[dirNumber][fileIndex]->name, TS_FILE_NAME_SIZE) == 0) &&
                                   (file.mjd  == myfiles[dirNumber][fileIndex]->mjd  ) &&
                                   (file.hour == myfiles[dirNumber][fileIndex]->hour ) &&
                                   (file.min  == myfiles[dirNumber][fileIndex]->min  ) &&
                                   (file.size == myfiles[dirNumber][fileIndex]->size ))
                               {
                                   myfiles[dirNumber][fileIndex]->present = TRUE;                  // Mark it as being present
                                   return TRUE;
                               }        
                               break;
    }
    
    
    return FALSE;
}
        
           
bool FileExistsInList(char* directory, TYPE_File file, int dirNumber, int *foundIndex)
{
    int fileIndex;
//TAP_Print("FEIL Comparing %s< to list dn=%d\r\n",file.name,dirNumber);
//TAP_Print("FEIL dir %s< \r\n",directory);
//TAP_Print("FEIL myfd %s< \r\n",myfiles[dirNumber][1]->directory);
    if (myfiles[dirNumber][0] == NULL)           
    {
        myfiles[dirNumber][0] = TAP_MemAlloc(sizeof (*myfiles[dirNumber][0]));
        if (myfiles[dirNumber][0] == NULL)
        {
               ShowMessageWin( rgn, "TAP Memory Allocation Failure", "Unable to allocate enough memory", "for 'myfiles' in 'FileExistsInList'", 500);
               return -1;
        }
        memset(myfiles[dirNumber][0],0,sizeof (*myfiles[dirNumber][0]));
    }
    if (myfiles[dirNumber][1] == NULL)           
    {
        myfiles[dirNumber][1] = TAP_MemAlloc(sizeof (*myfiles[dirNumber][1]));
        if (myfiles[dirNumber][1] == NULL)
        {
               ShowMessageWin( rgn, "TAP Memory Allocation Failure", "Unable to allocate enough memory", "for 'myfiles' in 'FileExistsInList'", 500);
               return -1;
        }
        memset(myfiles[dirNumber][1],0,sizeof (*myfiles[dirNumber][1]));
    }
    
    if (strncmp( directory, myfiles[dirNumber][1]->directory, MAX_FULL_DIR_NAME_LENGTH ) != 0) return FALSE;
    
    for ( fileIndex=1; fileIndex<= myfolders[dirNumber]->numberOfFiles; fileIndex++)
	{
          if (FileExists( directory, file, dirNumber, fileIndex) ) 
          {
              *foundIndex = fileIndex;
              return TRUE;
          }
    }
    
    return FALSE;
}
 
     
void SetDirFilesToNotPresent(int dirNumber)
{
    int fileIndex;
    
    for ( fileIndex=1; fileIndex<= myfolders[dirNumber]->numberOfFiles; fileIndex++)
    {
            myfiles[dirNumber][fileIndex]->present     = FALSE;   // Mark it as being NOT present
            myfiles[dirNumber][fileIndex]->isRecording = FALSE;   // Default the isRecording flag to FALSE.
            myfiles[dirNumber][fileIndex]->isPlaying   = FALSE;   // Default the isPlaying   flag to FALSE.
//            myfiles[dirNumber][fileIndex]->isNew       = FALSE;   // Default the isNew       flag to FALSE.
    }
}

          
     
void SetAllFilesToNotPresent(void)
{
    int dirNumber, fileIndex;
    
	for ( dirNumber=0; dirNumber<= numberOfFolders; dirNumber++)
	{
	    SetDirFilesToNotPresent( dirNumber );    // Set the files in this directory to not present.
	    myfolders[dirNumber]->newRecs    = 0;
	    myfolders[dirNumber]->newRecDirs = 0;
	    
    }
}


void DeleteMyfilesEntry(int dirNumber, int fileIndex)
{
     int i;
     appendToLogfile("DeleteMyfilesEntry: started.", INFO);
     
//     if (IsFileRec(myfiles[dirNumber][fileIndex]->name, myfiles[dirNumber][fileIndex]->attr))   //If this is a recording
     if ( myfiles[dirNumber][fileIndex]->attr == ATTR_FOLDER)     // It's a folder
     {
        myfolders[dirNumber]->numberOfFolders--;       // Decrease the number of folders in this directory.    
     }
     else                                              // Assume it's a recording.
     {
        if (!recycleWindowMode) DeleteProgressInfo( dirNumber, fileIndex, FALSE);   // Delete any playback progress information for this file if we're in normal view
        myfolders[dirNumber]->numberOfRecordings--;  // Decrease the number of recordings in this directory.    
     }   
     
     appendToLogfile("DeleteMyfilesEntry: TAP_MemFree for myfiles[dirNumber][fileIndex].", WARNING);
     appendIntToLogfile("DeleteMyfilesEntry: TAP_MemFree dirNumber=%d", dirNumber,WARNING);
     appendIntToLogfile("DeleteMyfilesEntry: TAP_MemFree fileIndex=%d", fileIndex,WARNING);
     TAP_MemFree( myfiles[dirNumber][fileIndex] );   // Free the allocated memory of this entry as we repoint the pointers next.
     appendToLogfile("DeleteMyfilesEntry: After TAP_MemFree.", WARNING);
     // Shuffle each of the remaining entries down one.
     appendIntToLogfile("DeleteMyfilesEntry: Starting shuffle loop fileIndex=%d.",fileIndex, WARNING);
     appendIntToLogfile("DeleteMyfilesEntry: Starting shuffle loop myfolders[dirNumber]->numberOfFiles=%d.",myfolders[dirNumber]->numberOfFiles, WARNING);
     for ( i=fileIndex; i < myfolders[dirNumber]->numberOfFiles; i++)
     {
         myfiles[dirNumber][i] = myfiles[dirNumber][i+1];  
     }
     
     myfolders[dirNumber]->numberOfFiles--;       // Decrease the number of files/folders in this directory.    
     appendToLogfile("DeleteMyfilesEntry: finished.", INFO);

}


void DeleteMyfilesFolderEntry(int dirNumber)
{
     int i;
     
     // emove any 'new'file indicators for any parent directories.
     if (myfolders[dirNumber]->newRecDirs > 0)
         CascadeFolderHasNewChange(myfolders[dirNumber]->parentDirNumber, -myfolders[dirNumber]->newRecDirs);
         
     for ( i=1; i <= myfolders[dirNumber]->numberOfFiles; i++)
     {
         if (myfiles[dirNumber][i]->attr == ATTR_FOLDER)  // This is a subfolder that we need to recursively delete.
         {
            DeleteMyfilesFolderEntry( myfiles[dirNumber][i]->directoryNumber );
         }   
         else
         {
            if (!recycleWindowMode) DeleteProgressInfo( dirNumber, i, FALSE);   // Delete any playback progress information for this file if in normal view mode
         }
         TAP_MemFree(myfiles[dirNumber][i]);                               // Free up the memory.
     }

//     memset(myfolders[dirNumber],0,sizeof (*myfolders[dirNumber]));   // Clear out the *myfolders entry.
     myfolders[dirNumber]->numberOfFiles      = 0;  // Set number of files to 0
     myfolders[dirNumber]->numberOfFolders    = 0;  // Set number of folders to 0
     myfolders[dirNumber]->numberOfRecordings = 0;  // Set number of recordings to 0
//     TAP_MemFree(myfolders[dirNumber]);                               // Free up the memory.
}


void DeleteDirFilesNotPresent(int dirNumber)
{
    int fileIndex;
    
    for ( fileIndex=1; fileIndex<= myfolders[dirNumber]->numberOfFiles; fileIndex++)
    {
         if (!myfiles[dirNumber][fileIndex]->present)               // File was not present when we reinvoked Archive
         {
               if (myfiles[dirNumber][fileIndex]->attr == ATTR_FOLDER)  // Delete any subfolders first.
               {
                   DeleteMyfilesFolderEntry( myfiles[dirNumber][fileIndex]->directoryNumber);
                }    
               DeleteMyfilesEntry( dirNumber, fileIndex);
               fileIndex--;  // Move pointer back as we need to recheck the file that has just been shuffled down.
         }
    }
}

void DeleteAllFilesNotPresent(void)
{
    int dirNumber, fileIndex;
    
	for ( dirNumber=0; dirNumber<= numberOfFolders; dirNumber++)
	{
	    DeleteDirFilesNotPresent( dirNumber );  // Delete the files not present in this directory.      
    }
}



void LoadArchiveInfo(char* directory, int dirNumber, int parentDirNumber, int recursiveDepth)
{

    int i, cnt, foundIndex;
    char str[200], str1[1024], subdir[1024];
    int numberOfDirFiles, numberOfDirFolders, numberOfDirRecordings, subFolderCount;
    TYPE_Dir_List subfolders[MAX_DIRS];

    appendToLogfile("LoadArchiveInfo: Started.", INFO);
  
    // Set the local directory pointers.  If this is the first time through for this directory they will all equal 0, otherwise they will 
    // have the previous local totals.   
    numberOfDirFiles      = myfolders[dirNumber]->numberOfFiles;
    numberOfDirFolders    = myfolders[dirNumber]->numberOfFolders;
    numberOfDirRecordings = myfolders[dirNumber]->numberOfRecordings;

    subFolderCount        = 0;  // Tracks the number of subfolders to recursively check.

    // Create a parent directory entry as the first file when we're in a subdirectory - but not in /DataFiles.
    if (!InDataFilesFolder( directory ))
    {
        //  Clear out variables.    
        memset (&file, 0, sizeof (file));
        strcat(file.name, PARENT_DIR_TEXT);
        if (!FileExists(directory, file, dirNumber, 1))  // If the parent directory doesn't already exist, create a new entry.
        {
           numberOfDirFiles++;  // Increase count for number of folders/files.
           AddNewParentFolder(directory, dirNumber, numberOfDirFiles, parentDirNumber);     // Create parent folder entry in the "myfiles" array.
        }
    }    

    //  Clear out variables.    
    memset (&file, 0, sizeof (file));

    // Find all the Files and Folders in the current directory.
    cnt = TAP_Hdd_FindFirst(&file); 
    #ifdef WIN32    // If testing on WIN32 platform set some file date/time attributes as not yet implemented in the TAP SDK.
       file.mjd  = 54042;  // 3rd November 2006
       file.hour = 8;
       file.min  = 0;
       file.sec  = 0;
    #endif
    

    appendStringToLogfile("LoadArchiveInfo: Started for directory=%s<<",directory, WARNING);
    appendIntToLogfile("LoadArchiveInfo: Count=%d",cnt, WARNING);
    appendIntToLogfile("LoadArchiveInfo: ATTR_FOLDER=%d",ATTR_FOLDER, WARNING);
    appendIntToLogfile("LoadArchiveInfo: ATTR_TS=%d",ATTR_TS, WARNING);

    for ( i=1; i <= cnt ; i += 1 )
	{
         appendStringToLogfile("LoadArchiveInfo: file.name=%s",file.name, WARNING);
          appendIntToLogfile("LoadArchiveInfo: file.attr=%d",file.attr, WARNING);
      
      
          //
          // RECYCLED RECORDINGS - handle the automatic delete of recycled recordings at TAP startup.
          //
          if ((IsFileRecycledRec(file.name, file.attr)) && (recycleBinCleanoutOption==1) && (tapStartup))
          {
               TAP_Hdd_Delete(file.name);
               // Check if the delete was successful.
               #ifdef WIN32  // If testing on Windows platform, assume success rather than physically deleting file.
               if (FALSE)
               #else  
               if (TAP_Hdd_Exist(file.name))  // Delete didn't work
               #endif
               {
                     ShowMessageWin( rgn, "Archive Startup", "Recycle Bin Cleanout Failed to Delete:", file.name, 400 );
               }
          }            
      

      
          //
          // RECORDING
          //
          if (IsFileRec(file.name, file.attr))
          {
             if (!FileExistsInList(directory, file, dirNumber, &foundIndex))    // If the file doesn't already exist in this directory,create a new entry.
             {
                numberOfDirFiles++;           // Increase local directory count for number of folders/files (count starts at 0, so first file/folder is #1).
                numberOfDirRecordings++;      // Increase local directory count for the number of recordings (count starts at 0).

                AddNewFile(directory, dirNumber, numberOfDirFiles, file);    // Create file entry in the "myfiles" array.

                // If the file is a recording file, we can add additional information.
                appendToLogfile("LoadArchiveInfo: Checking if this is an active recording.", WARNING);
                if (InDataFilesFolder(directory) )
                {
                   // Check if this file is for a current running recording, if so we need to get the information from different locations.
                   appendStringToLogfile("LoadArchiveInfo: Calling LoadRecordingInfo for file #%s",file.name, WARNING);
                   LoadRecordingInfo(dirNumber, numberOfDirFiles);
                }   
             
                // If the current file is NOT an active recording, we can load the extra information from the header.
                appendToLogfile("LoadArchiveInfo: Checking if this is an existing recording.", WARNING);
                if (!myfiles[dirNumber][numberOfDirFiles]->isRecording)
                {
                    appendStringToLogfile("LoadArchiveInfo: Calling LoadHeaderInfo for file.name=%s",file.name, WARNING);
                    LoadHeaderInfo( file.name, dirNumber, numberOfDirFiles);
                }   
             }  
             else   // The file already exists.
             { 
                // If the file is a recording file, we can add additional information.
                appendToLogfile("LoadArchiveInfo: Checking if this is an active recording.", WARNING);
                if (InDataFilesFolder(directory) )
                {
                   // Check if this file is for a current running recording, if so we need to get the information from different locations.
                   appendStringToLogfile("LoadArchiveInfo: Calling LoadRecordingInfo for file #%s",file.name, WARNING);
                   LoadRecordingInfo(dirNumber, foundIndex);
                }   
             
                // If the current file is NOT an active recording, AND the header information hasn't already been loaded, read the header.
                appendToLogfile("LoadArchiveInfo: Checking if this is an existing recording.", WARNING);
                if ((!myfiles[dirNumber][foundIndex]->isRecording) && ((myfiles[dirNumber][foundIndex]->eventNameLength==0)||(myfiles[dirNumber][foundIndex]->recDuration==0)))
                {
                    appendStringToLogfile("LoadArchiveInfo: Calling LoadHeaderInfo for file.name=%s",file.name, WARNING);
                    LoadHeaderInfo( file.name, dirNumber, foundIndex);
                }   
             }   
                    
          }

          //
          // FOLDER
          //
          if (file.attr == ATTR_FOLDER)    // It's a subfolder, so store it's name for checking later.
          {
             if (!FileExistsInList(directory, file, dirNumber, &foundIndex))    // If the file doesn't exist in this directory,create a new entry.
             {
                numberOfDirFiles++;           // Increase local directory count for number of folders/files (count starts at 0, so first file/folder is #1).
                numberOfDirFolders++;         // Increase local directory count for the number of subfolders (count starts at 0).
                numberOfFolders++;            // Increase global count for number of folders (/Datafiles is #0).
                AddNewFolder(directory, dirNumber, numberOfDirFiles, file, numberOfFolders);     // Create subfolder entry in the "myfiles" array.
                // Add the name of this folder to the list of files to recursively check at the end.  Index 1 is the first subfolder to check.
                strcpy( subfolders[numberOfDirFolders].name, file.name );
                subfolders[numberOfDirFolders].directoryNumber = numberOfFolders;     // Save the directory number for this new folder.
             }   
             else
             {  // Subfolder previously existed, but...     
                // Add the name of this folder to the list of files to recursively check at the end.  Index 1 is the first subfolder to check.
                subFolderCount++;
                strcpy( subfolders[subFolderCount].name, file.name );
                subfolders[subFolderCount].directoryNumber = myfiles[dirNumber][foundIndex]->directoryNumber;     // Save the current directory number for this existing folder.
             }   
          }
             
          TAP_Hdd_FindNext (&file);
          #ifdef WIN32
             file.mjd  = 54042;  // 3rd November 2006
             file.hour = 8;
             file.min  = 0;
             file.sec  = 0;
          #endif
          
    }
   
    // Save new or updated information on file and sub-folder counts for THIS directory.
    myfolders[dirNumber]->numberOfFiles      = numberOfDirFiles;
    myfolders[dirNumber]->numberOfFolders    = numberOfDirFolders;
    myfolders[dirNumber]->numberOfRecordings = numberOfDirRecordings;
    myfolders[dirNumber]->parentDirNumber    = parentDirNumber;

    if (recursiveDepth > 0)       // If we have asked to recursively load files, call the additional subfolders.
    { 
	   for ( i=1; i<= numberOfDirFolders; i++)
	   {
          if (strcmp(subfolders[i].name, "..")==0) break;   // Don't do a recursive on a parent directory.
          if (strcmp(subfolders[i].name, ".")==0) break;    // Don't do a recursive on a same directory.
          // Create the full directory name of the subfolder.
          subdir[0]='\0';                   // Blank out existing subdir.
          strcpy(subdir, directory);        // Add in our starting directory.
          strcat(subdir,"/");               // Add a slash.
          strcat(subdir, subfolders[i].name);     // Add the subfolder name.

          // Change directory to the subfolder.
          TAP_Hdd_ChangeDir(subfolders[i].name);  // Change to the sub directory.
          // Recurcively call the LoadArchiveInfo to read/load the information for the subfolder.
//          LoadArchiveInfo(subdir, subfolders[i].directoryNumber, dirNumber, TRUE);   
          if (recursiveDepth == 1)  // Then we only want to go 1 sub-directory deep, so load one more sub-directory, but no more recursive levels.        
             LoadArchiveInfo(subdir, subfolders[i].directoryNumber, dirNumber, 0);          // Only go recursive 1 directory.
          else   // Keep calling recursive until no more sub-directories.
             LoadArchiveInfo(subdir, subfolders[i].directoryNumber, dirNumber, 99);          
          // Go back to our starting directory, ready for anymore sub directories.
          GotoPath(directory);              
       }
    }   

    appendToLogfile("LoadArchiveInfo: Finished.", INFO);
}



void LoadPlaybackStatusInfo(void)
{
    int dirNumber, fileIndex;
    
     
	for ( dirNumber=0; dirNumber<= numberOfFolders; dirNumber++)
	{
	    for ( fileIndex=1; fileIndex<= myfolders[dirNumber]->numberOfFiles; fileIndex++)
	    {
          if (IsFileRec(myfiles[dirNumber][fileIndex]->name, myfiles[dirNumber][fileIndex]->attr))  // If we're looking at a recorded show, match up any existing playback status info.
          {
                appendToLogfile("LoadArchiveInfo: Setting playback status.", WARNING);
                // Copy any playback status information from the playedFiles array to the matching myFiles entries.
                SetPlaybackStatus( dirNumber, fileIndex );  // Match any playback status info with these existing files.
                
                // If there is an existing file being played, load it's information into the matching myFiles entry.
                if (inPlaybackMode) LoadPlaybackInfo(dirNumber, fileIndex);
          } 
        }       
    }
}

void loadInitialArchiveInfo(bool clearProgressInfo, int recursiveLevel)
{
    // Routine to do an initial load of file information from the hard-drive.
    
    GotoDataFiles();                                // Change directory to the root /DataFiles directory.
    GetRecordingInfo();
    SetAllFilesToNotPresent();                      // Flag all of the files/folders in our myfiles list as not present.
    initialLoad = TRUE;
    LoadArchiveInfo("/DataFiles", 0, 0, recursiveLevel);        // Check all of the files/folders again to see if there are any new files/folders.
    initialLoad = FALSE;
    DeleteAllFilesNotPresent();                     // Delete any of the files/folders that are no longer on the disk
  
    GotoPath(CurrentDir);                           // Change directory back to the directory that we were last in.
    
    numberOfFiles = myfolders[CurrentDirNumber]->numberOfFiles;  // Set the number of files for this directory.
    maxShown      = myfolders[CurrentDirNumber]->numberOfFiles;  // Set the number of files shown for this directory.

    GetPlaybackInfo();                              // Get info about any active playback.
    LoadPlaybackStatusInfo();                       // Load the latest playback status information into the file entries.

	sortOrder = sortOrderOption;                    // Default to default sort order.
	SortList(sortOrder);		                    // Sort the list.

}

void loadSubsequentArchiveInfo(bool clearProgressInfo, int recursiveLevel)
{
    // Routine to do a subsequent load of file information from the hard-drive starting from the current directory.
    
    GetRecordingInfo();
    SetDirFilesToNotPresent(CurrentDirNumber);            // Flag all of the files/folders in our myfiles list as not present.
    initialLoad = FALSE;
    LoadArchiveInfo(CurrentDir, CurrentDirNumber,  myfolders[CurrentDirNumber]->parentDirNumber, recursiveLevel);    // Check all of the files/folders again to see if there are any new files/folders.
    DeleteDirFilesNotPresent(CurrentDirNumber);           // Delete any of the files/folders that are no longer on the disk
    
    numberOfFiles = myfolders[CurrentDirNumber]->numberOfFiles;  // Set the number of files for this directory.
    maxShown      = myfolders[CurrentDirNumber]->numberOfFiles;  // Set the number of files shown for this directory.

    GetPlaybackInfo();                              // Get info about any active playback.
    LoadPlaybackStatusInfo();                       // Load the latest playback status information into the file entries.

	sortOrder = sortOrderOption;                    // Default to default sort order.
	SortList(sortOrder);		                    // Sort the list.
	

}
