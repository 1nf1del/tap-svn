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

#define PARENT_DIR_TEXT "Move up a directory"   // Define the text to be used for the parent directory.
#define PARENT_DIR_ATTR 250                     // Attribute value to set for the Parent Directory.

#define BUFFER_SIZE 1500

int headerOffset;  // Variable used to cater for offset differences between TF5000 (0 offset) and the TF5800 (4 bytes offset)

#define REC_DURATION_OFFSET 8
#define REC_DURATION_LENGTH 2
unsigned short recDuration;

#define SVC_NUM_OFFSET 10
unsigned short svc_num;

#define SVC_TYPE_OFFSET 12
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
	dword	startCluster;   // Starting disk cluster for the file - used with the filename so that we can locate a unique file.
	dword   totalBlock;     // Stores the total blocks in the file.
	dword   currentBlock;   // Stores the current playback block position.
} TYPE_Played_Files;


TYPE_My_Files myfiles[MAX_FILES], currentFile;  // Establish a global array that will hold the file details for all the files in the current directory.
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
     
     

void LoadHeaderInfo(char filename[ TS_FILE_NAME_SIZE ], int index)
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

    myfiles[index].recDuration = buf[REC_DURATION_OFFSET]*256 + buf[REC_DURATION_OFFSET+1];

    myfiles[index].svcNum = buf[SVC_NUM_OFFSET]*256 + buf[SVC_NUM_OFFSET+1];

    myfiles[index].svcType = buf[SVC_TYPE_OFFSET];

    memcpy(myfiles[index].serviceName,(buf+SERVICE_NAME_OFFSET),SERVICE_NAME_LENGTH);   
    myfiles[index].serviceName[SERVICE_NAME_LENGTH]='\0';

    myfiles[index].parentalRating = buf[PARENTAL_RATING_OFFSET+headerOffset];
    
    myfiles[index].eventNameLength = buf[EVENT_NAME_LENGTH_OFFSET+headerOffset];

    memcpy(myfiles[index].eventName,(buf+EVENT_NAME_OFFSET+headerOffset),myfiles[index].eventNameLength);   
    myfiles[index].eventName[myfiles[index].eventNameLength]='\0';

    memcpy(myfiles[index].eventDescName,(buf+EVENT_NAME_OFFSET+headerOffset+myfiles[index].eventNameLength),EVENT_NAME_MAX_LENGTH);   

    myfiles[index].extInfoLength = buf[EXT_INFO_LENGTH_OFFSET+headerOffset]*256 + buf[EXT_INFO_LENGTH_OFFSET+headerOffset+1];
    //extInfo = "This is the Extended Event Information. I have put more information in here to see what that does to the scrolling.  I was hoping that this could get across multiple pages to see what happens after you do multiple Info key presses. You may find that this goes over multiple lines and wrap around at the word boundary.  The quick brown fox jumps over the lazy dog. You will often find multiple pages particularly when you look at the SBS channel.  The ABC is another channel where you have several pages of extended information.  I still need to look at what happens in the routine if we have a zero length string or a really long string that fills the entire line.";
    
    memcpy(myfiles[index].extInfo,(buf+EXT_INFO_OFFSET+headerOffset), myfiles[index].extInfoLength);   
    myfiles[index].extInfo[myfiles[index].extInfoLength]='\0';

    // I think this is wrong and 1st byte=hours, 2nd byte= minutes
    myfiles[index].eventDuration = buf[EVENT_DURATION_OFFSET+headerOffset]*256 + buf[EVENT_DURATION_OFFSET+headerOffset+1];

    myfiles[index].eventStartMJD = buf[EVENT_STARTMJD_OFFSET+headerOffset]*256 + buf[EVENT_STARTMJD_OFFSET+headerOffset+1];

    myfiles[index].eventStartHour = buf[EVENT_STARTHOUR_OFFSET+headerOffset];
    
    myfiles[index].eventStartMin = buf[EVENT_STARTMIN_OFFSET+headerOffset];
    
    myfiles[index].eventEndMJD = buf[EVENT_ENDMJD_OFFSET+headerOffset]*256 + buf[EVENT_ENDMJD_OFFSET+headerOffset+1];

    myfiles[index].eventEndHour = buf[EVENT_ENDHOUR_OFFSET+headerOffset];
    
    myfiles[index].eventEndMin = buf[EVENT_ENDMIN_OFFSET+headerOffset];
  
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
        if ((myfiles[fileIndex].startCluster == playedFiles[i].startCluster) && (strcmp(myfiles[fileIndex].name,playedFiles[i].name)==0))
        {
              myfiles[fileIndex].hasPlayed = TRUE;
              myfiles[fileIndex].currentBlock = playedFiles[i].currentBlock;
              myfiles[fileIndex].totalBlock = playedFiles[i].totalBlock;
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


void LoadPlaybackInfo( int index )
{   
   TYPE_TapChInfo  chInfo;
   int durHour, durMin;

   appendToLogfile("LoadPlaybackInfo: Started.");

   // We match the current playback file based on disk startcluster and filename. 
   if ((myfiles[index].startCluster == CurrentPlaybackFile->startCluster) && (strncmp(myfiles[index].name,CurrentPlaybackFile->name, TS_FILE_NAME_SIZE)==0))
   {
         appendStringToLogfile("LoadPlaybackInfo: Match on CurrentPlaybackFile->name=%s<<",CurrentPlaybackFile->name);
         appendStringToLogfile("LoadPlaybackInfo: for myfiles.name=%s<<",myfiles[index].name);

         myfiles[index].recDuration = CurrentPlaybackInfo.duration;
         myfiles[index].svcNum      = CurrentPlaybackInfo.svcNum;
         myfiles[index].svcType     = CurrentPlaybackInfo.svcType;

         TAP_Channel_GetInfo( myfiles[index].svcType, myfiles[index].svcNum, &chInfo);
         strcpy(myfiles[index].serviceName,chInfo.chName);
         
         myfiles[index].parentalRating = CurrentPlaybackEvent.parentalRating;
    
         strcpy(myfiles[index].eventName, CurrentPlaybackEvent.eventName);
         strcpy(myfiles[index].eventDescName, CurrentPlaybackEvent.description);

         myfiles[index].extInfoLength = 0;
         myfiles[index].extInfo[0]='\0';

         myfiles[index].eventDuration  = ((CurrentPlaybackEvent.duration>>8)&0xff) + (CurrentPlaybackEvent.duration&0xff);  

         myfiles[index].eventStartMJD  = ((CurrentPlaybackEvent.startTime>>16)&0xffff);
         myfiles[index].eventStartHour = ((CurrentPlaybackEvent.startTime>>8)&0xff);
         myfiles[index].eventStartMin  = ((CurrentPlaybackEvent.startTime)&0xff);

         myfiles[index].eventEndMJD    = ((CurrentPlaybackEvent.endTime>>16)&0xffff);
         myfiles[index].eventEndHour   = ((CurrentPlaybackEvent.endTime>>8)&0xff);
         myfiles[index].eventEndMin    = ((CurrentPlaybackEvent.endTime)&0xff);
         
         myfiles[index].isPlaying      = TRUE;
   }     
        
   appendToLogfile("LoadPlaybackInfo: Finished.");
}


void LoadRecordingInfo(int index)
{
    int i;
    TYPE_TapChInfo  chInfo;
    
    appendToLogfile("LoadRecordingInfo: Checking recordings.");
    
    for (i=0; i <= 1; i += 1)
    {
        if (strncmp(myfiles[index].name, recInfo[i].fileName, TS_FILE_NAME_SIZE)==0) 
        {
                appendStringToLogfile("LoadRecordingInfo: Match on recInfo.filename=%s<<",recInfo[i].fileName);
                appendStringToLogfile("LoadRecordingInfo: for myfiles.name=%s<<",myfiles[index].name);
                myfiles[index].isRecording    = TRUE;  
                myfiles[index].svcNum         = recInfo[i].svcNum;  
                myfiles[index].svcType        = recInfo[i].svcType; 
                
                TAP_Channel_GetInfo( myfiles[index].svcType, myfiles[index].svcNum, &chInfo);
                strcpy(myfiles[index].serviceName,chInfo.chName);
                
                myfiles[index].recDuration   = recInfo[i].duration;  
                myfiles[index].recordedSec   = recInfo[i].recordedSec;  
                
                myfiles[index].eventStartMJD  = ((recInfo[i].startTime>>16)&0xffff);  
                myfiles[index].eventStartHour = ((recInfo[i].startTime>>8)&0xff);  
                myfiles[index].eventStartMin  =  (recInfo[i].startTime&0xff);  
                                 
                myfiles[index].eventEndMJD    = ((recInfo[i].endTime>>16)&0xffff);  
                myfiles[index].eventEndHour   = ((recInfo[i].endTime>>8)&0xff);
                myfiles[index].eventEndMin    =  (recInfo[i].endTime&0xff);    
                break;  // Found our match, so no need to keep checking.
        }
        else
                myfiles[index].isRecording    = FALSE;          
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



void LoadArchiveInfo(void)
{
    int i, cnt;
    char str1[1024];
    appendToLogfile("LoadArchiveInfo: Started.");
   
    memset(&myfiles[0],0,sizeof (myfiles[0]));
    memset(&myfiles[1],0,sizeof (myfiles[1]));
    
    // Set attributes for parent directory as the first file when we're in a subdirectory.
    if (!InDataFilesFolder())
    {
        myfiles[1].attr = PARENT_DIR_ATTR;  // Normal attribute for parent directory is 240.  Make it 250 to allow easy sorting.
        strcpy(myfiles[1].name,PARENT_DIR_TEXT);
        strcpy(myfiles[1].sortName,PARENT_DIR_TEXT);
        numberOfFiles = 1;
        numberOfFolders = 1;
    }    
    else
    {
        numberOfFiles = 0;
        numberOfFolders = 0;
    }    

    //  Clear out variables.    
    memset (&file, 0, sizeof (file));
    
    // Get the names of the recordings that may be running.
    appendToLogfile("LoadArchiveInfo: Checking recordings.");
    GetRecordingInfo();
                   
    // Find all the Files and Folders in the current directory.
    cnt = TAP_Hdd_FindFirst(&file); 
    appendStringToLogfile("LoadArchiveInfo: Started for directory=%s<<",CurrentDir);
    appendIntToLogfile("LoadArchiveInfo: Count=%d",cnt);
    appendIntToLogfile("LoadArchiveInfo: ATTR_FOLDER=%d",ATTR_FOLDER);
    appendIntToLogfile("LoadArchiveInfo: ATTR_TS=%d",ATTR_TS);

    for ( i=1; i <= cnt ; i += 1 )
	{
          appendStringToLogfile("LoadArchiveInfo: file.name=%s",file.name);
          appendIntToLogfile("LoadArchiveInfo: file.attr=%d",file.attr);
      
          // If we've found a folder or TS file (that is not the timeshift temporary file, add it to our list.
          if ( (((IsFileRec(file.name, file.attr)) || (file.attr == ATTR_FOLDER)) && (strcmp(file.name,"__temprec__.ts")!=0))) 
          {
             numberOfFiles += 1;  // Increase count for number of folders/files.

             // Blank out existing variable space.
             memset(&myfiles[numberOfFiles],0,sizeof (myfiles[numberOfFiles]));

             // Set the standard settings.
             myfiles[numberOfFiles].attr         = file.attr;
             myfiles[numberOfFiles].mjd          = file.mjd;
             myfiles[numberOfFiles].hour         = file.hour;
             myfiles[numberOfFiles].min          = file.min;
             myfiles[numberOfFiles].sec          = file.sec;
             myfiles[numberOfFiles].localOffset  = file.localOffset;
             myfiles[numberOfFiles].startCluster = file.startCluster;
             strcpy(myfiles[numberOfFiles].name,file.name);
             if (IsFileRec(file.name, file.attr)) FormatSortName(myfiles[numberOfFiles].sortName,file.name);
             else strcpy(myfiles[numberOfFiles].sortName,file.name);
             myfiles[numberOfFiles].crypt        = file.crypt;
             myfiles[numberOfFiles].size         = file.size;
             myfiles[numberOfFiles].currentPos   = file.currentPos;
             myfiles[numberOfFiles].isRecording  = FALSE;   // Default the isRecording flag to FALSE.
             myfiles[numberOfFiles].isPlaying    = FALSE;   // Default the isPlaying flag to FALSE.
             // If the file is a recording file, we can add additional information.
             if (IsFileRec(file.name, file.attr))
             {
                    appendStringToLogfile("LoadArchiveInfo: File=%s<< is a recording, so adding more info.",file.name);
                  
                    // If we're in the DataFiles directory, check if this file is one of the recording files.
                    appendToLogfile("LoadArchiveInfo: Checking if this is an active recording.");
                    if (InDataFilesFolder() )
                    {
                          // Check if this file is for a current running recording, if so we need to get the information from different locations.
                          appendStringToLogfile("LoadArchiveInfo: Calling LoadRecordingInfo for file #%s",file.name);
                          LoadRecordingInfo(numberOfFiles);
                    }   
             
                    // If the current file is NOT an active recording, we can load the extra information from the header.
                    appendToLogfile("LoadArchiveInfo: Checking if this is an existing recording.");
                    if (!myfiles[numberOfFiles].isRecording)
                    {
                          appendStringToLogfile("LoadArchiveInfo: Calling LoadHeaderInfo for file.name=%s",file.name);
                          LoadHeaderInfo( file.name, numberOfFiles);
                    }   
             }
          }
             
          TAP_Hdd_FindNext (&file);
    }

    appendToLogfile("LoadArchiveInfo: Getting playback info.");
    GetPlaybackInfo();  // Get info about any active playback.
   
	for ( i=1; i<= numberOfFiles; i++)
	{
          if (myfiles[i].attr == ATTR_FOLDER)  // If we're looking at a folder, count the number of files and subfolders in that folder.
          {
                appendToLogfile("LoadArchiveInfo: Counting subfolders.");
                CountFolderFilesAndSubfolders(myfiles[i].name, &myfiles[i].numberOfFiles , &myfiles[i].numberOfFolders );
          }
          else
          {
                appendToLogfile("LoadArchiveInfo: Setting playback status.");
                // Copy any playback status information from the playedFiles array to the matching myFiles entries.
                SetPlaybackStatus( i );  // Match any playback status info with these existing files.
                
                // If there is an existing file being played, load it's information into the matching myFiles entry.
                if (inPlaybackMode) LoadPlaybackInfo(i);
          }      
    }

    maxShown = numberOfFiles;
    appendToLogfile("LoadArchiveInfo: Finished.");
}
