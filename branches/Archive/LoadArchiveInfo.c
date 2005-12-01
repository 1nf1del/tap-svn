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

#define MAX_FILES 512  // Define how many files in a single directory we will handle.
#define MAX_DIRS  100  // Define how many directories we will handle.

#define PARENT_DIR_TEXT "Move up a directory"   // Define the text to be used for the parent directory.
#define PARENT_DIR_ATTR 250                     // Attribute value to set for the Parent Directory.
#define CURRENT_DIR_ATTR 240                    // Attribute value for the Current Directory.
#define NORMAL_PARENT_DIR_ATTR 242              // Standard attribute value for the Parent Directory.

#define BUFFER_SIZE 1500

int headerOffset;  // Variable used to cater for offset differences between TF5000 (0 offset) and the TF5800 (4 bytes offset)

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

#define EVENT_NAME_LENGTH_OFFSET 85 //89  TF5100 (subtract 1 from the length)
#define EVENT_NAME_LENGTH_LENGTH 1
unsigned short eventNameLength;

#define PARENTAL_RATING_OFFSET 86  // 90
#define PARENTAL_RATING_LENGTH 1
unsigned short parentalRating;

#define EVENT_NAME_OFFSET 87  //91  TF5100 88
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
	char    directory[ TS_FILE_NAME_SIZE ];
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
} TYPE_My_Files;

typedef struct 
{
    char	name[ TS_FILE_NAME_SIZE ];
    int     directoryNumber;
    int     numberOfFiles;
    int     numberOfFolders;
    int     numberOfRecordings;
    int     parentDirNumber;
} TYPE_Dir_List;

typedef struct 
{
	char	name[ TS_FILE_NAME_SIZE ];
	dword	startCluster;   // Starting disk cluster for the file - used with the filename so that we can locate a unique file.
	dword   totalBlock;     // Stores the total blocks in the file.
	dword   currentBlock;   // Stores the current playback block position.
} TYPE_Played_Files;


TYPE_My_Files myfiles[MAX_DIRS][MAX_FILES], currentFile;  // Establish a global array that will hold the file details for all the files in the current directory.
TYPE_Dir_List myfolders[MAX_DIRS];
TYPE_File     file;
TYPE_RecInfo  recInfo[2];
TYPE_Played_Files playedFiles[MAX_FILES];


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

    appendToLogfile("LoadHeaderInfo: Started.");

    fp = TAP_Hdd_Fopen( filename ); 
    if( fp==NULL) 
    {
            appendStringToLogfile("LoadHeaderInfo: Failure opening filename=%s<<",filename);
            appendToLogfile("LoadHeaderInfo: Exiting.");
            return;
    }        

    buf = (byte *)TAP_MemAlloc( BUFFER_SIZE );
    if( buf == 0 ) { TAP_Hdd_Fclose( fp ); return ; }
    TAP_Hdd_Fread( buf, BUFFER_SIZE, 1, fp );
    TAP_Hdd_Fclose( fp );

    myfiles[dir][index].recDuration = buf[REC_DURATION_OFFSET]*256 + buf[REC_DURATION_OFFSET+1];

    myfiles[dir][index].svcNum = buf[SVC_NUM_OFFSET]*256 + buf[SVC_NUM_OFFSET+1];

    myfiles[dir][index].svcType = buf[SVC_TYPE_OFFSET];

    memcpy(myfiles[dir][index].serviceName,(buf+SERVICE_NAME_OFFSET),SERVICE_NAME_LENGTH);   
    myfiles[dir][index].serviceName[SERVICE_NAME_LENGTH]='\0';

    myfiles[dir][index].parentalRating = buf[PARENTAL_RATING_OFFSET+headerOffset];
    
    myfiles[dir][index].eventNameLength = buf[EVENT_NAME_LENGTH_OFFSET+headerOffset];

    memcpy(myfiles[dir][index].eventName,(buf+EVENT_NAME_OFFSET+headerOffset),myfiles[dir][index].eventNameLength);   
    myfiles[dir][index].eventName[myfiles[dir][index].eventNameLength]='\0';

    memcpy(myfiles[dir][index].eventDescName,(buf+EVENT_NAME_OFFSET+headerOffset+myfiles[dir][index].eventNameLength),EVENT_NAME_MAX_LENGTH);   

    myfiles[dir][index].extInfoLength = buf[EXT_INFO_LENGTH_OFFSET+headerOffset]*256 + buf[EXT_INFO_LENGTH_OFFSET+headerOffset+1];
    //extInfo = "This is the Extended Event Information. I have put more information in here to see what that does to the scrolling.  I was hoping that this could get across multiple pages to see what happens after you do multiple Info key presses. You may find that this goes over multiple lines and wrap around at the word boundary.  The quick brown fox jumps over the lazy dog. You will often find multiple pages particularly when you look at the SBS channel.  The ABC is another channel where you have several pages of extended information.  I still need to look at what happens in the routine if we have a zero length string or a really long string that fills the entire line.";
    
    memcpy(myfiles[dir][index].extInfo,(buf+EXT_INFO_OFFSET+headerOffset), myfiles[dir][index].extInfoLength);   
    myfiles[dir][index].extInfo[myfiles[dir][index].extInfoLength]='\0';

    // I think this is wrong and 1st byte=hours, 2nd byte= minutes
    myfiles[dir][index].eventDuration = buf[EVENT_DURATION_OFFSET+headerOffset]*256 + buf[EVENT_DURATION_OFFSET+headerOffset+1];

    myfiles[dir][index].eventStartMJD = buf[EVENT_STARTMJD_OFFSET+headerOffset]*256 + buf[EVENT_STARTMJD_OFFSET+headerOffset+1];

    myfiles[dir][index].eventStartHour = buf[EVENT_STARTHOUR_OFFSET+headerOffset];
    
    myfiles[dir][index].eventStartMin = buf[EVENT_STARTMIN_OFFSET+headerOffset];
    
    myfiles[dir][index].eventEndMJD = buf[EVENT_ENDMJD_OFFSET+headerOffset]*256 + buf[EVENT_ENDMJD_OFFSET+headerOffset+1];

    myfiles[dir][index].eventEndHour = buf[EVENT_ENDHOUR_OFFSET+headerOffset];
    
    myfiles[dir][index].eventEndMin = buf[EVENT_ENDMIN_OFFSET+headerOffset];
  
    appendToLogfile("LoadHeaderInfo: Finished.");

}

void ReadPlaybackInfo()
{
    int i;
    
    for (i=1; i <= MAX_FILES; i += 1)
    {
        memset(&playedFiles[i],0,sizeof (playedFiles[i]));
        
        // Read the playback info from the playback ini file.
       
    }   

    numberOfPlayedFiles = 0;
}


void SetPlaybackStatus(int fileIndex)
{
    int i;
    
    for (i=1; i <= numberOfPlayedFiles; i += 1)
    {
        // If the file has the same filename and the same disk startcluster we consider it a match.
        if ((myfiles[0][fileIndex].startCluster == playedFiles[i].startCluster) && (strcmp(myfiles[0][fileIndex].name,playedFiles[i].name)==0))
        {
              myfiles[0][fileIndex].hasPlayed = TRUE;
              myfiles[0][fileIndex].currentBlock = playedFiles[i].currentBlock;
              myfiles[0][fileIndex].totalBlock = playedFiles[i].totalBlock;
              break; // We've found a match, so don't bother checking other playback entries.
        }              
    }   
}


void GetPlaybackInfo( void )
{
   inPlaybackMode = FALSE;
   

   // Check for Playbackmode
   iOverRunCheck = ApiOverRunCheck;   // Set flag to catch any buffer overrun from API call.
   TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo);

   if ( iOverRunCheck != ApiOverRunCheck )  // TAP_Hdd_GetPlayInfo V1.22 has a buffer overrun issue, so check if it has overrun by more than 128 bytes.
   {
      ShowMessageWin( rgn, "Archive:  Error retrieving Playback Info", "TAP_Hdd_GetPlayInfo buffer overrun", "in 'CheckPlaybackStatus'.", 400 );
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

   appendToLogfile("LoadPlaybackInfo: Started.");

   // We match the current playback file based on disk startcluster and filename. 
   if ((myfiles[dir][index].startCluster == CurrentPlaybackFile->startCluster) && (strncmp(myfiles[dir][index].name,CurrentPlaybackFile->name, TS_FILE_NAME_SIZE)==0))
   {
         appendStringToLogfile("LoadPlaybackInfo: Match on CurrentPlaybackFile->name=%s<<",CurrentPlaybackFile->name);
         appendStringToLogfile("LoadPlaybackInfo: for myfiles.name=%s<<",myfiles[dir][index].name);

         myfiles[dir][index].recDuration = CurrentPlaybackInfo.duration;
         myfiles[dir][index].svcNum      = CurrentPlaybackInfo.svcNum;
         myfiles[dir][index].svcType     = CurrentPlaybackInfo.svcType;

         TAP_Channel_GetInfo( myfiles[dir][index].svcType, myfiles[dir][index].svcNum, &chInfo);
         strcpy(myfiles[dir][index].serviceName,chInfo.chName);
         
         myfiles[dir][index].parentalRating = CurrentPlaybackEvent.parentalRating;
    
         strcpy(myfiles[dir][index].eventName, CurrentPlaybackEvent.eventName);
         strcpy(myfiles[dir][index].eventDescName, CurrentPlaybackEvent.description);

         myfiles[dir][index].extInfoLength = 0;
         myfiles[dir][index].extInfo[0]='\0';

         myfiles[dir][index].eventDuration  = ((CurrentPlaybackEvent.duration>>8)&0xff) + (CurrentPlaybackEvent.duration&0xff);  

         myfiles[dir][index].eventStartMJD  = ((CurrentPlaybackEvent.startTime>>16)&0xffff);
         myfiles[dir][index].eventStartHour = ((CurrentPlaybackEvent.startTime>>8)&0xff);
         myfiles[dir][index].eventStartMin  = ((CurrentPlaybackEvent.startTime)&0xff);

         myfiles[dir][index].eventEndMJD    = ((CurrentPlaybackEvent.endTime>>16)&0xffff);
         myfiles[dir][index].eventEndHour   = ((CurrentPlaybackEvent.endTime>>8)&0xff);
         myfiles[dir][index].eventEndMin    = ((CurrentPlaybackEvent.endTime)&0xff);
         
         myfiles[dir][index].isPlaying      = TRUE;
   }     
        
   appendToLogfile("LoadPlaybackInfo: Finished.");
}


void LoadRecordingInfo(int dir, int index)
{
    int i;
    TYPE_TapChInfo  chInfo;
    
    appendToLogfile("LoadRecordingInfo: Checking recordings.");
    
    for (i=0; i <= 1; i += 1)
    {
        if (strncmp(myfiles[dir][index].name, recInfo[i].fileName, TS_FILE_NAME_SIZE)==0) 
        {
                appendStringToLogfile("LoadRecordingInfo: Match on recInfo.filename=%s<<",recInfo[i].fileName);
                appendStringToLogfile("LoadRecordingInfo: for myfiles.name=%s<<",myfiles[dir][index].name);
                myfiles[dir][index].isRecording    = TRUE;  
                myfiles[dir][index].svcNum         = recInfo[i].svcNum;  
                myfiles[dir][index].svcType        = recInfo[i].svcType; 
                
                TAP_Channel_GetInfo( myfiles[dir][index].svcType, myfiles[dir][index].svcNum, &chInfo);
                strcpy(myfiles[dir][index].serviceName,chInfo.chName);
                
                myfiles[dir][index].recDuration   = recInfo[i].duration;  
                myfiles[dir][index].recordedSec   = recInfo[i].recordedSec;  
                
                myfiles[dir][index].eventStartMJD  = ((recInfo[i].startTime>>16)&0xffff);  
                myfiles[dir][index].eventStartHour = ((recInfo[i].startTime>>8)&0xff);  
                myfiles[dir][index].eventStartMin  =  (recInfo[i].startTime&0xff);  
                                 
                myfiles[dir][index].eventEndMJD    = ((recInfo[i].endTime>>16)&0xffff);  
                myfiles[dir][index].eventEndHour   = ((recInfo[i].endTime>>8)&0xff);
                myfiles[dir][index].eventEndMin    =  (recInfo[i].endTime&0xff);    
                break;  // Found our match, so no need to keep checking.
        }
        else
                myfiles[dir][index].isRecording    = FALSE;          
    }   
    appendToLogfile("LoadRecordingInfo: Finished checking recordings.");

}

void GetRecordingInfo()
{
    int i;
     
    // Get the names of the recordings that may be running.
    appendToLogfile("GetRecordingInfo: Started.");
    for ( i=0; i <=1 ; i += 1)
    {
        memset (&recInfo[i],0,sizeof (recInfo[i]));
        if (TAP_Hdd_GetRecInfo (i, &recInfo[i]) && ((recInfo[i].recType == RECTYPE_Normal) || (recInfo[i].recType == RECTYPE_Copy)))
        {
            // Keep the recording information as it is.
            appendIntToLogfile("GetRecordingInfo: Recording slot #%d is running.",i);
        }
        else
        {
            appendIntToLogfile("GetRecordingInfo: Recording slot #%d is NOT running.",i);
            TAP_SPrint(recInfo[i].fileName,"");   // Blank out the name.
        }
    }    
    appendToLogfile("GetRecordingInfo: Finished.");
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
            if ( stamp[l] == CHAR_ASTERIX )  // * is a wildcard for -_./ space
            {   
               if ( (str[start+l] != CHAR_DASH ) && (str[start+l] != CHAR_SCORE ) && ( str[start+l] != CHAR_DOT) && ( str[start+l] != CHAR_SLASH)&& ( str[start+l] != CHAR_SPACE))
                  return FALSE;
            }
            else 
            if ( stamp[l] == CHAR_SPACE ) 
            {   
               if ( (str[start+l] != CHAR_SPACE ) && (str[start+l] != CHAR_SCORE ))
                  return FALSE;
            }
            else 
            if ( stamp[l] == CHAR_PLUS ) 
            {   
               if ( str[start+l] != CHAR_PLUS )
                  return FALSE;
            }
            else 
            if ( stamp[l] == CHAR_HASH ) 
            {
                 if ( str[start+l] != CHAR_HASH)
                    return FALSE;
            }
            else 
            if ( stamp[l] == CHAR_DASH ) 
            {
                 if ( str[start+l] != CHAR_DASH )
                    return FALSE;
            }
            else 
            if ( stamp[l] == CHAR_DOT ) 
            {
                 if ( str[start+l] != CHAR_DOT )
                    return FALSE;
            }
            else 
            if ( stamp[l] == CHAR_SLASH ) 
            {
                 if ( str[start+l] != CHAR_SLASH )
                    return FALSE;
            }
            else 
            if ( stamp[l] == CHAR_SCORE ) 
            {
                 if ( str[start+l] != CHAR_SCORE )
                    return FALSE;
            }
            else 
            if ( stamp[l] == CHAR_NINE ) 
            {
                 if (( str[start+l] < CHAR_ZERO ) || ( str[start+l] > CHAR_NINE))
                    return FALSE;
            }
            else 
            if ((str[start+l] < CHAR_ZERO) || (str[start+l] > CHAR_NINE)) 
               return FALSE;
        } 
     }  
     // TAP_Print("%s< matched on %s<\r\n",str,stamp);           
     return TRUE;            
}



void FormatSortName(char* target, char* source)
{
#define NUMBER_OF_STAMPS 16   
  
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
     char stamps[NUMBER_OF_STAMPS][20] = {"_#", "-#", "_+", "-+", "*99*99 YYYY*MM*DD", "*YYYYMMDD_9999", "*YYYYMMDD", "*YYYY*MM*DD",  "*YY*MM*DD",  "*DDMMYY",  "*DD*MM",  "*MMDD",  "- DDD", " DDD", "-99", "-9"};
     char stampStr[10];
     int i, start, stampLen, fileLen;

     strcpy( target, source);                   // Copy the whole filename by default.
     fileLen = strlen(target);                  // Get the length of the filename.
     target[fileLen - 4]='\0';                  // Chop off the ".rec" from the filename.
                
     for (i=0; i < NUMBER_OF_STAMPS; i++)  // Go through each date/time stamp and see if we have a matching filename
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



void LoadArchiveInfo(char* directory, int dirNumber, int parentDirNumber)
{

    int i, cnt;
    char str1[1024];
    int numberOfDirFiles, numberOfDirFolders, numberOfDirRecordings;
    TYPE_Dir_List subfolders[MAX_DIRS];
    char subdir[1024];
TAP_Print("started for %s< d=%d p=%d\r\n",directory,dirNumber, parentDirNumber);    
    appendToLogfile("LoadArchiveInfo: Started.");
   
    numberOfDirFiles      = 0;
    numberOfDirFolders    = 0;
    numberOfDirRecordings = 0;
    
//    memset(&myfiles[0][1],0,sizeof (myfiles[0][1]));
    
    // Set attributes for parent directory as the first file when we're in a subdirectory - but not in /DataFiles.
    if (!InDataFilesFolder( directory ))
    {
        numberOfDirFiles++;  // Increase count for number of folders/files.
        memset(&myfiles[dirNumber][numberOfDirFiles],0,sizeof (myfiles[dirNumber][numberOfDirFiles]));
        myfiles[dirNumber][numberOfDirFiles].attr = PARENT_DIR_ATTR;  // Normal attribute for parent directory is 240.  Make it 250 to allow easy sorting.
        strcpy(myfiles[dirNumber][numberOfDirFiles].name,PARENT_DIR_TEXT);
        strcpy(myfiles[dirNumber][numberOfDirFiles].sortName,PARENT_DIR_TEXT);
        strcpy(myfiles[dirNumber][numberOfDirFiles].directory,directory);
        myfiles[dirNumber][numberOfDirFiles].directoryNumber = parentDirNumber;
    }    


    //  Clear out variables.    
    memset (&file, 0, sizeof (file));
    
    // Get the names of the recordings that may be running.
//    appendToLogfile("LoadArchiveInfo: Checking recordings.");
//    GetRecordingInfo();
                   
    // Find all the Files and Folders in the current directory.
    cnt = TAP_Hdd_FindFirst(&file); 
    appendStringToLogfile("LoadArchiveInfo: Started for directory=%s<<",directory);
    appendIntToLogfile("LoadArchiveInfo: Count=%d",cnt);
    appendIntToLogfile("LoadArchiveInfo: ATTR_FOLDER=%d",ATTR_FOLDER);
    appendIntToLogfile("LoadArchiveInfo: ATTR_TS=%d",ATTR_TS);

    for ( i=1; i <= cnt ; i += 1 )
	{
          appendStringToLogfile("LoadArchiveInfo: file.name=%s",file.name);
          appendIntToLogfile("LoadArchiveInfo: file.attr=%d",file.attr);
TAP_Print("%d %s<\r\n",i,file.name);    
      
          // If we've found a folder or TS file , add it to our list.
          if ((IsFileRec(file.name, file.attr)) || (file.attr == ATTR_FOLDER)) 
          {
             numberOfFiles++;  // Increase count for number of folders/files.
             numberOfDirFiles++;  // Increase count for number of folders/files.

             // Blank out existing variable space.
             memset(&myfiles[dirNumber][numberOfDirFiles],0,sizeof (myfiles[0][numberOfDirFiles]));

             // Set the standard settings.
             myfiles[dirNumber][numberOfDirFiles].attr         = file.attr;
             myfiles[dirNumber][numberOfDirFiles].mjd          = file.mjd;
             myfiles[dirNumber][numberOfDirFiles].hour         = file.hour;
             myfiles[dirNumber][numberOfDirFiles].min          = file.min;
             myfiles[dirNumber][numberOfDirFiles].sec          = file.sec;
             myfiles[dirNumber][numberOfDirFiles].localOffset  = file.localOffset;
             myfiles[dirNumber][numberOfDirFiles].startCluster = file.startCluster;
             strcpy(myfiles[dirNumber][numberOfDirFiles].name,file.name);
             if (IsFileRec(file.name, file.attr)) FormatSortName(myfiles[dirNumber][numberOfDirFiles].sortName,file.name);
             else strcpy(myfiles[dirNumber][numberOfDirFiles].sortName,file.name);
             strcpy(myfiles[dirNumber][numberOfDirFiles].directory, directory);
             myfiles[dirNumber][numberOfDirFiles].directoryNumber = dirNumber;
             myfiles[dirNumber][numberOfDirFiles].crypt        = file.crypt;
             myfiles[dirNumber][numberOfDirFiles].size         = file.size;
             myfiles[dirNumber][numberOfDirFiles].currentPos   = file.currentPos;
             myfiles[dirNumber][numberOfDirFiles].isRecording  = FALSE;   // Default the isRecording flag to FALSE.
             myfiles[dirNumber][numberOfDirFiles].isPlaying    = FALSE;   // Default the isPlaying flag to FALSE.
             // If the file is a recording file, we can add additional information.
             if (IsFileRec(file.name, file.attr))
             {
                    appendStringToLogfile("LoadArchiveInfo: File=%s<< is a recording, so adding more info.",file.name);
                    numberOfDirRecordings++;                // Increase the number of recordings count for this folder.
                  
                    // If we're in the DataFiles directory, check if this file is one of the recording files.
                    appendToLogfile("LoadArchiveInfo: Checking if this is an active recording.");
//                    if (InDataFilesFolder() )
                    if (strcmp(directory,"/DataFiles")==0)
                    {
                          // Check if this file is for a current running recording, if so we need to get the information from different locations.
                          appendStringToLogfile("LoadArchiveInfo: Calling LoadRecordingInfo for file #%s",file.name);
                          LoadRecordingInfo(dirNumber, numberOfDirFiles);
                    }   
             
                    // If the current file is NOT an active recording, we can load the extra information from the header.
                    appendToLogfile("LoadArchiveInfo: Checking if this is an existing recording.");
                    if (!myfiles[dirNumber][numberOfDirFiles].isRecording)
                    {
                          appendStringToLogfile("LoadArchiveInfo: Calling LoadHeaderInfo for file.name=%s",file.name);
                          LoadHeaderInfo( file.name, dirNumber, numberOfDirFiles);
                    }   
                    
             }
             if (file.attr == ATTR_FOLDER)   // It's a subfolder, so store it's name for checking later.
             {
                 strcpy(subfolders[numberOfDirFolders].name, file.name);
                 strcpy(subfolders[numberOfDirFolders].name, file.name);
                 numberOfFolders++;
                 myfiles[dirNumber][numberOfDirFiles].directoryNumber = numberOfFolders;  // Give each new directory a new number.
                 subfolders[numberOfDirFolders].directoryNumber = numberOfFolders;
TAP_Print("DF=%d OF=%d sf=%d\r\n", numberOfDirFolders,  numberOfFolders, subfolders[numberOfDirFolders].directoryNumber );
TAP_Delay(000);
                 numberOfDirFolders++;
         }
          }
             
          TAP_Hdd_FindNext (&file);
    }
TAP_Print("Finished dir\r\n");
    appendToLogfile("LoadArchiveInfo: Getting playback info.");
//    GetPlaybackInfo();  // Get info about any active playback.
   
    // Save information on file and sub-folder counts for this directory.
    strcpy(myfolders[dirNumber].name,directory);
    myfolders[dirNumber].numberOfFiles      = numberOfDirFiles;
    myfolders[dirNumber].numberOfFolders    = numberOfDirFolders;
    myfolders[dirNumber].numberOfRecordings = numberOfDirRecordings;
    myfolders[dirNumber].parentDirNumber    = parentDirNumber;
TAP_Print("Dir #%d >%s<\r\n",dirNumber,directory);
TAP_Print("dirfiles=%d dirfolders=%d recs=%d \r\n",    numberOfDirFiles,numberOfDirFolders,numberOfDirRecordings);
TAP_Delay(000);
       
	for ( i=0; i< numberOfDirFolders; i++)
	{
          subdir[0]='\0';                   // Blank out existing subdir.
          strcat(subdir, directory);        // Add in our starting directory.
          strcat(subdir,"/");               // Add a slash.
          strcat(subdir, subfolders[i].name);     // Add the subfolder name.
          TAP_Hdd_ChangeDir(subfolders[i].name);  // Change to the sub directory.
          LoadArchiveInfo(subdir, subfolders[i].directoryNumber, dirNumber );          // Recursive call to load archive information from new subdirectory.
          GotoPath(directory);              // Go back to out starting directory, ready for anymore sub directories.
    }

    maxShown = numberOfDirFiles;
    appendToLogfile("LoadArchiveInfo: Finished.");
}



void LoadPlaybackStatusInfo(void)
{
    int i;
     
	for ( i=1; i<= numberOfFiles; i++)
	{
          if (IsFileRec(myfiles[0][i].name, myfiles[0][i].attr))  // If we're looking at a recorded show, match up any existing playback status info.
          {
                appendToLogfile("LoadArchiveInfo: Setting playback status.");
                // Copy any playback status information from the playedFiles array to the matching myFiles entries.
                SetPlaybackStatus( i );  // Match any playback status info with these existing files.
                
                // If there is an existing file being played, load it's information into the matching myFiles entry.
                if (inPlaybackMode) LoadPlaybackInfo(0, i);
          }      
    }
}
