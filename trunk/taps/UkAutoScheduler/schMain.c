/************************************************************
Part of the ukEPG project
This module performs the search

v0.0 sl8:	20-11-05	Inception date
v0.1 sl8:	20-01-06	All variables initialised
v0.2 sl8:	06-02-06	Moved schInitLcnToSvcNumMap to ukauto.c
v0.3 sl8:	16-02-06	Bug fix - Searches were not performed correctly on current day. 30 minute window incorrectly calculated.
				Modified for 'Perform Search' config option
				Delay searches for 2 minutes when TAP starts.
				Removed - 30 minute window where timers would not set.
				Added - Timers can be set up to within 2 minutes of current time (start padding permitting)
v0.4 sl8:	09-03-06	Destination folder and remote search file mods
v0.5 sl8:	22-03-06	Bug fix - Move failed if programme spanned midnight
v0.6 sl8:	11-04-06	Show window added and tidy up.
v0.7 sl8:	19-04-06	TRC option added. More work added for the Show window.
v0.8 sl8:	07-06-06	Bug fix - 1 hour padding caused incorrect start/end times.
v0.9 sl8:	05-08-06	Search ahead, Date, Time format. Keep feature.
v0.10 SgtWilko:	28-08-06	Added conflict handling code.
v0.11 sl8:	28-08-06	EPG event qualifier. Record check and more Date formats added.
v0.12 sl8	29-08-06	Ignore character table code in event name is present
v0.13 sl8	03-09-06	Max chars changed to 40 for single and multi timers. TAP_Timer_Add_SDK, TAP_Timer_Modifiy_SDK added.
v0.14 sl8	28-09-06	TRC bug fix. Check for remote file if no schedules set.
v0.15 sl8	29-09-06	Added separate conflict handler.
v0.16 sl8	11-10-06	Separate conflict bug fix. Changes made so that icons can update in 'Show' screen.
v0.17 sl8	13-10-06	Update move information when timer modified due to conflict.
v0.18 sl8	23-10-06	Incorrectly indicating a timer success when timer fails due to different channel conflict.
v0.19 janilxx:	03-11-06	Added ability to keep end padding between consecutive timers.
v0.20 Harvey    22-11-06	Added Already Recorded feature
v0.21 Harvey	03-12-06	Added Searching features (and,or,not) and wildcards
v0.22 sl8	15-12-06	Settings/UkAuto folder

**************************************************************/

#if DEBUG == 0
#define SCH_MAIN_DELAY_SEARCH_ALARM		120	// Search not allowed within 2 Minutes of TAP starting
#else
#define SCH_MAIN_DELAY_SEARCH_ALARM		1
#endif

#define SCH_MAX_FILENAME_LENGTH			40	// Max Length of filename - 4 (.rec)

#define STRING_LENGTH 255
#define MAX_STACK_VAL  50

#define RECORDED_DB_DIR "UKAuto_db"
#define REC_TYPE_EMPTY 0
#define REC_TYPE_CRC32 1

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

#ifndef SEEK_CUR
#define SEEK_CUR 1
#endif

#ifndef SEEK_END
#define SEEK_END 2
#endif

bool schMainCompareStrings(char *, char *,int);
bool schMainPerformSearch(TYPE_TapEvent *, int, int);
byte schMainSetTimer(char*, dword, dword, int, word, byte);
void schMainService(void);
void schMainInitLcnToSvcNumMap(void);
void schMainUpdateSearchList(void);
void schMainDetermineChangeDirType(void);
int schMainFindMultipleConflictCount(char *);
dword schMainConvertTimeToMins(dword time);
dword schMainConvertMinsToTime(dword time);
bool schProcessConflict(TYPE_TimerInfo*, dword, dword, int);
void schConflictUpdateMoveList(TYPE_TimerInfo*, dword);

static struct schDataTag schUserData[SCH_MAX_SEARCHES];
static struct schDataTag schRemoteData[SCH_MAX_SEARCHES];

static struct schMoveTag schMoveData[SCH_MAX_MOVES];

static int schTotalTvSvc = 0;
static int schTotalRadioSvc = 0;
static byte schMainTotalValidSearches = 0;
static byte schMainTotalValidRemoteSearches = 0;
static byte schMainPerformSearchMode = 0;
static int schMainPerformSearchTime = 0;
static int schMainPerformSearchDays = 14;
static byte schMainTotalValidMoves = 0;
static bool schMainTRCEnabled = TRUE;
static byte schMainDateFormat = 0;
static byte schMainTimeFormat = 0;
static byte schMainConflictOption = SCH_MAIN_CONFLICT_DISABLED;

word* schLcnToServiceTv = NULL;
word* schLcnToServiceRadio = NULL;

byte* schEpgDataExtendedInfo = NULL;
TYPE_TapEvent* schEpgData = NULL;

static unsigned long crc32_tab[] = {
 0x00000000L, 0x77073096L, 0xee0e612cL, 0x990951baL, 0x076dc419L,
 0x706af48fL, 0xe963a535L, 0x9e6495a3L, 0x0edb8832L, 0x79dcb8a4L,
 0xe0d5e91eL, 0x97d2d988L, 0x09b64c2bL, 0x7eb17cbdL, 0xe7b82d07L,
 0x90bf1d91L, 0x1db71064L, 0x6ab020f2L, 0xf3b97148L, 0x84be41deL,
 0x1adad47dL, 0x6ddde4ebL, 0xf4d4b551L, 0x83d385c7L, 0x136c9856L,
 0x646ba8c0L, 0xfd62f97aL, 0x8a65c9ecL, 0x14015c4fL, 0x63066cd9L,
 0xfa0f3d63L, 0x8d080df5L, 0x3b6e20c8L, 0x4c69105eL, 0xd56041e4L,
 0xa2677172L, 0x3c03e4d1L, 0x4b04d447L, 0xd20d85fdL, 0xa50ab56bL,
 0x35b5a8faL, 0x42b2986cL, 0xdbbbc9d6L, 0xacbcf940L, 0x32d86ce3L,
 0x45df5c75L, 0xdcd60dcfL, 0xabd13d59L, 0x26d930acL, 0x51de003aL,
 0xc8d75180L, 0xbfd06116L, 0x21b4f4b5L, 0x56b3c423L, 0xcfba9599L,
 0xb8bda50fL, 0x2802b89eL, 0x5f058808L, 0xc60cd9b2L, 0xb10be924L,
 0x2f6f7c87L, 0x58684c11L, 0xc1611dabL, 0xb6662d3dL, 0x76dc4190L,
 0x01db7106L, 0x98d220bcL, 0xefd5102aL, 0x71b18589L, 0x06b6b51fL,
 0x9fbfe4a5L, 0xe8b8d433L, 0x7807c9a2L, 0x0f00f934L, 0x9609a88eL,
 0xe10e9818L, 0x7f6a0dbbL, 0x086d3d2dL, 0x91646c97L, 0xe6635c01L,
 0x6b6b51f4L, 0x1c6c6162L, 0x856530d8L, 0xf262004eL, 0x6c0695edL,
 0x1b01a57bL, 0x8208f4c1L, 0xf50fc457L, 0x65b0d9c6L, 0x12b7e950L,
 0x8bbeb8eaL, 0xfcb9887cL, 0x62dd1ddfL, 0x15da2d49L, 0x8cd37cf3L,
 0xfbd44c65L, 0x4db26158L, 0x3ab551ceL, 0xa3bc0074L, 0xd4bb30e2L,
 0x4adfa541L, 0x3dd895d7L, 0xa4d1c46dL, 0xd3d6f4fbL, 0x4369e96aL,
 0x346ed9fcL, 0xad678846L, 0xda60b8d0L, 0x44042d73L, 0x33031de5L,
 0xaa0a4c5fL, 0xdd0d7cc9L, 0x5005713cL, 0x270241aaL, 0xbe0b1010L,
 0xc90c2086L, 0x5768b525L, 0x206f85b3L, 0xb966d409L, 0xce61e49fL,
 0x5edef90eL, 0x29d9c998L, 0xb0d09822L, 0xc7d7a8b4L, 0x59b33d17L,
 0x2eb40d81L, 0xb7bd5c3bL, 0xc0ba6cadL, 0xedb88320L, 0x9abfb3b6L,
 0x03b6e20cL, 0x74b1d29aL, 0xead54739L, 0x9dd277afL, 0x04db2615L,
 0x73dc1683L, 0xe3630b12L, 0x94643b84L, 0x0d6d6a3eL, 0x7a6a5aa8L,
 0xe40ecf0bL, 0x9309ff9dL, 0x0a00ae27L, 0x7d079eb1L, 0xf00f9344L,
 0x8708a3d2L, 0x1e01f268L, 0x6906c2feL, 0xf762575dL, 0x806567cbL,
 0x196c3671L, 0x6e6b06e7L, 0xfed41b76L, 0x89d32be0L, 0x10da7a5aL,
 0x67dd4accL, 0xf9b9df6fL, 0x8ebeeff9L, 0x17b7be43L, 0x60b08ed5L,
 0xd6d6a3e8L, 0xa1d1937eL, 0x38d8c2c4L, 0x4fdff252L, 0xd1bb67f1L,
 0xa6bc5767L, 0x3fb506ddL, 0x48b2364bL, 0xd80d2bdaL, 0xaf0a1b4cL,
 0x36034af6L, 0x41047a60L, 0xdf60efc3L, 0xa867df55L, 0x316e8eefL,
 0x4669be79L, 0xcb61b38cL, 0xbc66831aL, 0x256fd2a0L, 0x5268e236L,
 0xcc0c7795L, 0xbb0b4703L, 0x220216b9L, 0x5505262fL, 0xc5ba3bbeL,
 0xb2bd0b28L, 0x2bb45a92L, 0x5cb36a04L, 0xc2d7ffa7L, 0xb5d0cf31L,
 0x2cd99e8bL, 0x5bdeae1dL, 0x9b64c2b0L, 0xec63f226L, 0x756aa39cL,
 0x026d930aL, 0x9c0906a9L, 0xeb0e363fL, 0x72076785L, 0x05005713L,
 0x95bf4a82L, 0xe2b87a14L, 0x7bb12baeL, 0x0cb61b38L, 0x92d28e9bL,
 0xe5d5be0dL, 0x7cdcefb7L, 0x0bdbdf21L, 0x86d3d2d4L, 0xf1d4e242L,
 0x68ddb3f8L, 0x1fda836eL, 0x81be16cdL, 0xf6b9265bL, 0x6fb077e1L,
 0x18b74777L, 0x88085ae6L, 0xff0f6a70L, 0x66063bcaL, 0x11010b5cL,
 0x8f659effL, 0xf862ae69L, 0x616bffd3L, 0x166ccf45L, 0xa00ae278L,
 0xd70dd2eeL, 0x4e048354L, 0x3903b3c2L, 0xa7672661L, 0xd06016f7L,
 0x4969474dL, 0x3e6e77dbL, 0xaed16a4aL, 0xd9d65adcL, 0x40df0b66L,
 0x37d83bf0L, 0xa9bcae53L, 0xdebb9ec5L, 0x47b2cf7fL, 0x30b5ffe9L,
 0xbdbdf21cL, 0xcabac28aL, 0x53b39330L, 0x24b4a3a6L, 0xbad03605L,
 0xcdd70693L, 0x54de5729L, 0x23d967bfL, 0xb3667a2eL, 0xc4614ab8L,
 0x5d681b02L, 0x2a6f2b94L, 0xb40bbe37L, 0xc30c8ea1L, 0x5a05df1bL,
 0x2d02ef8dL
 };


typedef struct
{
   // Note - for ease make 512 divisable by the size of this structure
   int type;
   unsigned long hash;
} TYPE_Hash_Record;

#define HASH_RECORDS_PER_BLOCK 512/sizeof(TYPE_Hash_Record)

typedef enum {
Or,
And,
Not,
LParen,
RParen,
EndOfLine,

/* How may operators are there? */
T_Max,

/* None operators */
Word,

} Token_val;

enum {
S,                 /* shift */
R,                 /* reduce */
A,                 /* accept */
E1,                /* error: missing right parenthesis */
E2,                /* error: missing operator */
E3,                /* error: unbalanced right parenthesis */
};

int parseTable[T_Max][T_Max] = {
/* stack  input ------------------------ */
/*          |   +   !   (   )  EOL  */
/*         --  --  --  --  --  ---  */
/* | */   { R,  S,  S,  S,  R,  R },
/* + */   { R,  R,  S,  S,  R,  R },
/* ! */   { R,  R,  S,  S,  R,  R },
/* ( */   { S,  S,  S,  S,  S,  E1},
/* ) */   { R,  R,  S,  E2, R,  R },
/* EOL*/  { S,  S,  S,  S,  E3, A }
} ;

typedef struct _search_stack
{
   char value[MAX_STACK_VAL];
   int top;
} SEARCH_STACK;

typedef struct _search
{
   Token_val T_LookAhead;
   char description[STRING_LENGTH];
   char expr[STRING_LENGTH];
   char string_buf[STRING_LENGTH];

   char search_len;
   int search_ptr;

   char token_buf[STRING_LENGTH];
   int token_ptr;

   SEARCH_STACK operator_stack;
   SEARCH_STACK value_stack;

} SEARCH;


unsigned long schMainGenerateHash(unsigned char *buf)
{
   unsigned long crc32=0;

   while ( *buf != '\0' )
      crc32 = crc32_tab[(crc32 ^ *buf++) & 0xff] ^ (crc32 >> 8);

   return crc32;
}

bool schMainAlreadyRecorded(TYPE_TapEvent *event)
{
   TYPE_File *file;
   unsigned long hash;
   TYPE_Hash_Record *hash_record;
   bool found=FALSE;
   char buf[512];
   int blocks,current,i;

   //TAP_Print("schMainAlreadyRecorded(%s)\n",event->description);

   // Given the eventName and the description, does this exist in the recorded database?
   //

   GotoPath(SETTINGS_FOLDER);
   TAP_Hdd_ChangeDir(RECORDED_DB_DIR);

   // Generate a hash for the description
   //
   hash=schMainGenerateHash(event->description);

   //TAP_Print("   Hash generated %d\n",hash);

   // Open up the database for this program
   //
   if ( (file=TAP_Hdd_Fopen(event->eventName)) == 0 )
   {
      //TAP_Print("   Creating New file %s\n",event->eventName);

      // Database File does not exist - create it
      //
      TAP_Hdd_Create(event->eventName, ATTR_NORMAL);
   
      // Open the file
      //
      if ( (file=TAP_Hdd_Fopen(event->eventName)) == 0 )
         return FALSE;

      // Now write out a blank 512 byte record
      //
      memset(buf,0,512);
      TAP_Hdd_Fwrite(buf,512,1,file);
  
      // Rewind to the begining
      //
      TAP_Hdd_Fseek(file,0,SEEK_SET);
   }

   blocks=(TAP_Hdd_Flen(file)/512);// do I need +1?

   //TAP_Print("   there are %d blocks in this file\n",blocks);

   current=0;

   while (current < blocks)
   {
      //TAP_Print("   Reading block %d\n",current);
	  
      TAP_Hdd_Fread(buf,512,1,file);

      hash_record=(TYPE_Hash_Record *)buf;

      for(i=0;i<HASH_RECORDS_PER_BLOCK;i++)
      {
         // Have we gone beyond the entry in this block?
         //
         if ( hash_record->type == REC_TYPE_EMPTY )
            break;

         //TAP_Print("   Checking against Hash %d\n",hash_record->hash);

         if ( hash == hash_record->hash )
         {
            //TAP_Print("   match found\n");

            // Close the file - and return match
            //
            TAP_Hdd_Fclose(file);
            return TRUE;
         }

         // Move on to the next record
         //
         hash_record++;
      }

      // We have fell out the loop - why?
      //
      if ( i != HASH_RECORDS_PER_BLOCK )
         break;

      current++;
   }

   //TAP_Print("   No Match Found\n");
  
   // Do we need to create a new block?
   //
   if ( current == blocks ) 
   {
      memset(buf,0,512);
      hash_record=(TYPE_Hash_Record *)buf;
   }
    
   hash_record->hash = hash;
   hash_record->type = REC_TYPE_CRC32;

   //TAP_Print("   writing block %d\n",current);

   TAP_Hdd_Fseek(file,512 * current, SEEK_SET);
   TAP_Hdd_Fwrite(buf,512,1,file);
 
   // Close the file - and return no match
   //
   TAP_Hdd_Fclose(file);
   return FALSE;
}

void schMainAlreadyRecordedInitialise()
{
   GotoPath(SETTINGS_FOLDER);
   TAP_Hdd_Create(RECORDED_DB_DIR,ATTR_FOLDER);
}

void schMainService(void)
{
	static word schChannel = 0;
	static byte schUserSearchIndex = 0;
	static dword schEpgIndex = 0;
	static dword schLastEpgIndex = 0;
	static int schEpgTotalEvents = 0;
	dword schMainCurrentTime = 0;

	char buffer1[128];

	switch(schServiceSV)
	{
	/*--------------------------------------------------*/
	case SCH_SERVICE_INITIALISE:

		if(schFileRetreiveSearchData() > 0)
		{
//			sprintf(buffer1, "%d Valid Searches",schMainTotalValidSearches);
//			ShowMessageWin(buffer1,"Loaded");
		}
		else
		{
//			ShowMessageWin("Error loading","SearchList.txt");
//			TAP_Exit();
		}

		schServiceSV = SCH_SERVICE_RESET_SEARCH;

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_RESET_SEARCH:

		schServiceSV = SCH_SERVICE_WAIT_TO_SEARCH;

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_WAIT_TO_SEARCH:

		switch(schMainPerformSearchMode)
		{
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_ONE_HOUR:

			if(schTimeMin == 0)
			{
				schServiceSV = SCH_SERVICE_CHECK_FOR_REMOTE_SEARCHES;
			}

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_SPECIFIED:

			if
			(
				(schTimeHour == ((schMainPerformSearchTime >> 8) & 0xFF))
				&&
				(schTimeMin == (schMainPerformSearchTime & 0xFF))
			)
			{
				schServiceSV = SCH_SERVICE_CHECK_FOR_REMOTE_SEARCHES;
			}

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_TEN_MINS:
		default:

			if((schTimeMin % 10) == 0)
			{
				schServiceSV = SCH_SERVICE_CHECK_FOR_REMOTE_SEARCHES;
			}

			break;
		/*--------------------------------------------------*/
		}

#if DEBUG == 1
	schServiceSV = SCH_SERVICE_CHECK_FOR_REMOTE_SEARCHES;
#endif

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_CHECK_FOR_REMOTE_SEARCHES:

		if(schMainTRCEnabled == TRUE)
		{
			if(schFileRetreiveRemoteData() > 0)
			{
				schServiceSV = SCH_SERVICE_UPDATE_SEARCH_LIST;
			}
			else if(schMainTotalValidSearches > 0)
			{
				schServiceSV = SCH_SERVICE_BEGIN_SEARCH;
			}
			else
			{
				schServiceSV = SCH_SERVICE_COMPLETE;
			}
		}
		else if(schMainTotalValidSearches > 0)
		{
			schServiceSV = SCH_SERVICE_BEGIN_SEARCH;
		}
		else
		{
			schServiceSV = SCH_SERVICE_COMPLETE;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_UPDATE_SEARCH_LIST:

		schMainUpdateSearchList();

		if(schMainTotalValidSearches > 0)
		{
			schServiceSV = SCH_SERVICE_BEGIN_SEARCH;
		}
		else
		{
			schServiceSV = SCH_SERVICE_COMPLETE;
		}

		break;	
	/*-------------------------------------------------*/
	case SCH_SERVICE_BEGIN_SEARCH:

		if(schStartUpCounter > SCH_MAIN_DELAY_SEARCH_ALARM)
		{
//			logStoreEvent("Performing Search");

			schUserSearchIndex = 0;		
			schEpgIndex = 0;

			if((schUserData[schUserSearchIndex].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
			{
				schChannel = 0;
			}
			else
			{
				schChannel = schUserData[schUserSearchIndex].searchStartSvcNum;
			}

			if(schUserData[schUserSearchIndex].searchStatus != SCH_USER_DATA_STATUS_DISABLED)
			{
				schServiceSV = SCH_SERVICE_INITIALISE_EPG_DATA;
			}
			else
			{
				schServiceSV = SCH_SERVICE_NEXT_USER_SEARCH;
			}
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_INITIALISE_EPG_DATA:

#ifndef WIN32
		if( schEpgData )
		{
			TAP_MemFree( schEpgData );
			schEpgData = 0;
		}

		schEpgData = TAP_GetEvent( schUserData[schUserSearchIndex].searchTvRadio, schChannel, &schEpgTotalEvents );
#else
		schEpgData = TAP_GetEvent_SDK( schUserData[schUserSearchIndex].searchTvRadio, schChannel, &schEpgTotalEvents );
#endif
		schServiceSV = SCH_SERVICE_PERFORM_SEARCH;
		
		break;	
	/*--------------------------------------------------*/
	case SCH_SERVICE_PERFORM_SEARCH:

		schMainCurrentTime = (schTimeMjd << 16) + (schTimeHour << 8) + schTimeMin;

		if
		(
			(schEpgData)
			&&
			(schEpgTotalEvents > 0)
			&&
			(schEpgIndex < schEpgTotalEvents)
			&&
			(
				((schMainCurrentTime + (schMainPerformSearchDays << 16)) > schEpgData[schEpgIndex].startTime)
				||
				(schMainPerformSearchDays == 14)
			)
		)
		{
			if((schMainPerformSearch(schEpgData, schEpgIndex, schUserSearchIndex)) == TRUE)
			{
				if(schUserData[schUserSearchIndex].searchStatus != SCH_USER_DATA_STATUS_RECORD_NEW)
				{
					schMainSetTimer(schEpgData[schEpgIndex].eventName, schEpgData[schEpgIndex].startTime, schEpgData[schEpgIndex].endTime, schUserSearchIndex, schChannel, (schUserData[schUserSearchIndex].searchStatus == SCH_USER_DATA_STATUS_RECORD_ALL));
				}
				else
				{
					if( schMainAlreadyRecorded(&schEpgData[schEpgIndex]) == FALSE )
					{
						schMainSetTimer(schEpgData[schEpgIndex].eventName, schEpgData[schEpgIndex].startTime, schEpgData[schEpgIndex].endTime, schUserSearchIndex, schChannel, 	(schUserData[schUserSearchIndex].searchStatus == SCH_USER_DATA_STATUS_RECORD_NEW));
					}
				}
			}

			schLastEpgIndex = schEpgIndex;

			schServiceSV = SCH_SERVICE_NEXT_EVENT;
		}
		else
		{
			schServiceSV = SCH_SERVICE_NEXT_CHANNEL;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_NEXT_EVENT:

		schEpgIndex++;

		if(schEpgIndex < schEpgTotalEvents)
		{
			if(schEpgData[schEpgIndex].startTime != schEpgData[schLastEpgIndex].startTime)
			{
				schServiceSV = SCH_SERVICE_PERFORM_SEARCH;
			}
		}
		else
		{
			schServiceSV = SCH_SERVICE_NEXT_CHANNEL;
		}
		
		break;	
	/*--------------------------------------------------*/
	case SCH_SERVICE_NEXT_CHANNEL:

		schChannel++;
		schEpgIndex = 0;

		if((schUserData[schUserSearchIndex].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
		{
			if
			(
				(
					(schChannel >= schTotalTvSvc)
					&&
					(schUserData[schUserSearchIndex].searchTvRadio == SCH_TV)
				)
				||
				(
					(schChannel >= schTotalRadioSvc)
					&&
					(schUserData[schUserSearchIndex].searchTvRadio == SCH_RADIO)
				)
			)
			{
				schServiceSV = SCH_SERVICE_NEXT_USER_SEARCH;
			}
			else
			{
				schServiceSV = SCH_SERVICE_INITIALISE_EPG_DATA;
			}
		}
		else
		{
			if(schChannel > schUserData[schUserSearchIndex].searchEndSvcNum)
			{
				schServiceSV = SCH_SERVICE_NEXT_USER_SEARCH;
			}
			else
			{
				schServiceSV = SCH_SERVICE_INITIALISE_EPG_DATA;
			}
		}

		break;	
	/*--------------------------------------------------*/
	case SCH_SERVICE_NEXT_USER_SEARCH:

		schUserSearchIndex++;

		schEpgIndex = 0;

		if(schUserSearchIndex >= schMainTotalValidSearches)
		{
			schServiceSV = SCH_SERVICE_COMPLETE;
		}
		else if(schUserData[schUserSearchIndex].searchStatus != SCH_USER_DATA_STATUS_DISABLED)
		{
			if((schUserData[schUserSearchIndex].searchOptions & SCH_USER_DATA_OPTIONS_ANY_CHANNEL) == SCH_USER_DATA_OPTIONS_ANY_CHANNEL)
			{
				schChannel = 0;
			}
			else
			{
				schChannel = schUserData[schUserSearchIndex].searchStartSvcNum;
			}

			schServiceSV = SCH_SERVICE_INITIALISE_EPG_DATA;
		}
		else
		{
			schServiceSV = SCH_SERVICE_NEXT_USER_SEARCH;
		}

		break;
	/*--------------------------------------------------*/
	case SCH_SERVICE_COMPLETE:

		switch(schMainPerformSearchMode)
		{
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_ONE_HOUR:

			if(schTimeMin != 0)
			{
//				logStoreEvent("Search Finished");

				schServiceSV = SCH_SERVICE_RESET_SEARCH;
			}

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_SPECIFIED:

			if(schTimeMin != (schMainPerformSearchTime & 0xFF))
			{
//				logStoreEvent("Search Finished");

				schServiceSV = SCH_SERVICE_RESET_SEARCH;
			}

			break;
		/*--------------------------------------------------*/
		case SCH_CONFIG_SEARCH_PERIOD_TEN_MINS:
		default:

			if((schTimeMin % 10) != 0)
			{
//				logStoreEvent("Search Finished");

				schServiceSV = SCH_SERVICE_RESET_SEARCH;
			}

			break;
		/*--------------------------------------------------*/
		}

		break;
	/*--------------------------------------------------*/
	}
}


bool schMainPerformSearch(TYPE_TapEvent *epgData, int epgDataIndex, int schSearch)
{
	int eventNameIndex = 0;
	bool foundSearchTerm = FALSE;
	word eventMjd = 0, eventYear = 0;
	byte eventHour = 0, eventMin = 0;
	byte eventMonth = 0, eventDay = 0, eventWeekDay = 0, eventWeekDayBit = 0;
	dword eventStartInMins = 0, currentTimeInMins = 0, startPaddingInMins = 0;
	TYPE_RecInfo CurrentRecordInfo0;
	char rec0InfoOverRun[512];
	TYPE_RecInfo CurrentRecordInfo1;
	char rec1InfoOverRun[512];

	eventMjd = ((epgData[epgDataIndex].startTime >> 16) & 0xFFFF);
	eventHour = ((epgData[epgDataIndex].startTime >> 8) & 0xFF);
	eventMin = (epgData[epgDataIndex].startTime & 0xFF);

	TAP_ExtractMjd( eventMjd, &eventYear, &eventMonth, &eventDay, &eventWeekDay);
	eventWeekDayBit = 0x01 << eventWeekDay;

	eventStartInMins = (eventMjd * 24 * 60) + (eventHour * 60) + eventMin;
	currentTimeInMins = (schTimeMjd * 24 * 60) + (schTimeHour * 60) + schTimeMin;
	startPaddingInMins = (((schUserData[schSearch].searchStartPadding & 0xff00) >> 8) * 60) + (schUserData[schSearch].searchStartPadding & 0xff);

	TAP_Hdd_GetRecInfo (0, &CurrentRecordInfo0);
	TAP_Hdd_GetRecInfo (1, &CurrentRecordInfo1);

	if
	(
		(
			(CurrentRecordInfo0.recType != RECTYPE_Normal)
			||
			(
				(CurrentRecordInfo0.recType == RECTYPE_Normal)
				&&
				(epgData[epgDataIndex].startTime >= CurrentRecordInfo0.endTime)
			)
		)
		&&
		(
			(CurrentRecordInfo1.recType != RECTYPE_Normal)
			||
			(
				(CurrentRecordInfo1.recType == RECTYPE_Normal)
				&&
				(epgData[epgDataIndex].startTime >= CurrentRecordInfo1.endTime)
			)
		)
		&&
		(
			(eventStartInMins > (startPaddingInMins + 2))
			&&
			((eventStartInMins - (startPaddingInMins + 2)) > currentTimeInMins)
		)
		&&
		(
			(schUserData[schSearch].searchStartTime == schUserData[schSearch].searchEndTime)
			||
			(
				(schUserData[schSearch].searchStartTime < schUserData[schSearch].searchEndTime)
				&&
				((epgData[epgDataIndex].startTime & 0xFFFF) >= schUserData[schSearch].searchStartTime)
				&&
				((epgData[epgDataIndex].startTime & 0xFFFF) <= schUserData[schSearch].searchEndTime)
			)
			||
			(
				(schUserData[schSearch].searchStartTime > schUserData[schSearch].searchEndTime)
				&&
				(
					((epgData[epgDataIndex].startTime & 0xFFFF) >= schUserData[schSearch].searchStartTime)
					||
					((epgData[epgDataIndex].startTime & 0xFFFF) <= schUserData[schSearch].searchEndTime)
				)
			)
		)
		&&
		((schUserData[schSearch].searchDay & eventWeekDayBit) == eventWeekDayBit)
	)
	{
		if
		(
			((schUserData[schSearch].searchOptions & SCH_USER_DATA_OPTIONS_EVENTNAME) == SCH_USER_DATA_OPTIONS_EVENTNAME)
			&&
			(
				((schUserData[schSearch].searchOptions & SCH_USER_DATA_OPTIONS_EXACT_MATCH) != SCH_USER_DATA_OPTIONS_EXACT_MATCH)
				||
				(strlen(epgData[epgDataIndex].eventName) == strlen(schUserData[schSearch].searchTerm))
				||
				(
					(epgData[epgDataIndex].eventName[0] <= 0x1F)	// Ignore character table code
					&&
					((strlen(epgData[epgDataIndex].eventName) - 1) == strlen(schUserData[schSearch].searchTerm))
				)
			)
		)
		{
			foundSearchTerm = schMainCompareStrings(epgData[epgDataIndex].eventName, schUserData[schSearch].searchTerm,schUserData[schSearch].searchOptions);
		}

		if
		(
			(foundSearchTerm == FALSE)
			&&
			((schUserData[schSearch].searchOptions & SCH_USER_DATA_OPTIONS_DESCRIPTION) == SCH_USER_DATA_OPTIONS_DESCRIPTION)
		)
		{
			foundSearchTerm = schMainCompareStrings(epgData[epgDataIndex].description, schUserData[schSearch].searchTerm,schUserData[schSearch].searchOptions);
		}

		if
		(
			(foundSearchTerm == FALSE)
			&&
			((schUserData[schSearch].searchOptions & SCH_USER_DATA_OPTIONS_EXT_INFO) == SCH_USER_DATA_OPTIONS_EXT_INFO)
		)
		{
			if( schEpgDataExtendedInfo )
			{
				TAP_MemFree( schEpgDataExtendedInfo );
				schEpgDataExtendedInfo = 0;
			}
			schEpgDataExtendedInfo = TAP_EPG_GetExtInfo(&epgData[epgDataIndex]);

			if(schEpgDataExtendedInfo)
			{
				foundSearchTerm = schMainCompareStrings((char*)schEpgDataExtendedInfo, schUserData[schSearch].searchTerm,schUserData[schSearch].searchOptions);
			}
		}
	}

	return foundSearchTerm;
}



byte schMainSetTimer(char *eventName, dword eventStartTime, dword eventEndTime, int searchIndex, word svcNum, byte isRec)
{
	TYPE_TimerInfo schTimerInfo, conflictTimerInfo, conflictTimerInfoA, conflictTimerInfoB;
	dword schEventStartInMins = 0, schEventEndInMins = 0;
	word schPaddingStartInMins = 0, schPaddingEndInMins = 0;
	dword schTimerStartInMins = 0, schTimerEndInMins = 0;
	dword endConflictMJD=0, endTimerMJD=0;
	word schTimerStartMjd = 0, schTimerEndMjd = 0;
	byte schTimerStartHour = 0, schTimerEndHour = 0;
	byte  schTimerStartMin = 0, schTimerEndMin = 0;
	word mjd = 0,year = 0;
	byte month = 0, day = 0, weekDay = 0, weekDayBit = 0;
	byte attachPosition[2];
	byte attachType[2];
	char prefixStr[64];
	char appendStr[64];
	char dateStr[64];
	char numbStr[64];
	char timeStr[64];
	char fileNameStr[132], fileNameStrConflict[132];
	char logBuffer[LOG_BUFFER_SIZE];
	int fileNameLen = 0;
	int timerError = 0, timerErrorA = 0, timerErrorB = 0;
	int i = 0;
	int conflictStatus=0;
	char multipleConflictCount[3];
	bool longName = FALSE;
	byte result = SCH_MAIN_TIMER_FAILED;
	dword temp = 0;
	bool schMoveListModified = FALSE;

	// ------------- Attachments ----------------

	memset(fileNameStr,0,132);

	attachPosition[0] = (schUserData[searchIndex].searchAttach >> 6) & 0x03;
	attachType[0] = schUserData[searchIndex].searchAttach & 0x3F;
	attachPosition[1] = (schUserData[searchIndex].searchAttach >> 14) & 0x03;
	attachType[1] = (schUserData[searchIndex].searchAttach >> 8) & 0x3F;

	fileNameLen = strlen(eventName);

	mjd = ((eventStartTime >> 16) & 0xFFFF);

	TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay);

	memset(dateStr,0,64);
	switch(schMainDateFormat)
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_DD_DOT_MM_DOT_YY:

		sprintf(dateStr,"%02d.%02d.%02d", day, month, (year % 10));
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_DD_DOT_MM_DOT_YYYY:

		sprintf(dateStr,"%02d.%02d.%04d", day, month, year);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_YY_DOT_MM_DOT_DD:

		sprintf(dateStr,"%02d.%02d.%02d", (year % 10), month, day);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_YYYY_DOT_MM_DOT_DD:

		sprintf(dateStr,"%04d.%02d.%02d", year, month, day);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_DDMMYY:

		sprintf(dateStr,"%02d%02d%02d", day, month, (year % 10));
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_DDMMYYYY:

		sprintf(dateStr,"%02d%02d%04d", day, month, year);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_YYMMDD:

		sprintf(dateStr,"%02d%02d%02d", (year % 10), month, day);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_YYYYMMDD:

		sprintf(dateStr,"%04d%02d%02d", year, month, day);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_DD_SLASH_MM_SLASH_YY:

		sprintf(dateStr,"%02d/%02d/%02d", day, month, (year % 10));
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_DD_SLASH_MM_SLASH_YYYY:

		sprintf(dateStr,"%02d/%02d/%04d", day, month, year);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_YY_SLASH_MM_SLASH_DD:

		sprintf(dateStr,"%02d/%02d/%02d", (year % 10), month, day);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_YYYY_SLASH_MM_SLASH_DD:

		sprintf(dateStr,"%04d/%02d/%02d", year, month, day);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_DD_DOT_MM:

		sprintf(dateStr,"%02d.%02d", day, month);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_MM_DOT_DD:

		sprintf(dateStr,"%02d.%02d", month, day);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_DDMM:

		sprintf(dateStr,"%02d%02d", day, month);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_MMDD:
		
		sprintf(dateStr,"%02d%02d", month, day);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_DD_SLASH_MM:
		
		sprintf(dateStr,"%02d/%02d", day, month);
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_DATE_FORMAT_MM_SLASH_DD:

		sprintf(dateStr,"%02d/%02d", month, day);
		
		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}

	memset(timeStr,0,64);
	switch(schMainTimeFormat)
	{
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_TIME_FORMAT_HH_COLON_MM:

		sprintf(timeStr,"%02d:%02d", ((eventStartTime >> 8) & 0xFF), (eventStartTime & 0xFF));
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_TIME_FORMAT_HH_DOT_MM:

		sprintf(timeStr,"%02d.%02d", ((eventStartTime >> 8) & 0xFF), (eventStartTime & 0xFF));
		
		break;
	/* ---------------------------------------------------------------------------- */
	case SCH_CONFIG_TIME_FORMAT_HHMM:

		sprintf(timeStr,"%02d%02d", ((eventStartTime >> 8) & 0xFF), (eventStartTime & 0xFF));
		
		break;
	/* ---------------------------------------------------------------------------- */
	default:

		break;
	/* ---------------------------------------------------------------------------- */
	}

	memset(numbStr,0,64);
	sprintf(numbStr,"S#%d", (searchIndex + 1));

	memset(prefixStr,0,64);
	memset(appendStr,0,64);

	for(i = 0; i < 2; i++)
	{
		if(attachPosition[i] == SCH_ATTACH_POS_PREFIX)
		{
			switch(attachType[i])
			{
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_NUMBER:

				if((strlen(eventName) + (strlen(numbStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(prefixStr,numbStr);
					strcat(prefixStr,"_");
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_DATE:

				if((strlen(eventName) + (strlen(dateStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(prefixStr,dateStr);
					strcat(prefixStr,"_");
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_TIME:

				if((strlen(eventName) + (strlen(timeStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(prefixStr,timeStr);
					strcat(prefixStr,"_");
				}

				break;
			/* ---------------------------------------------------------------------------- */
			default:
				break;
			/* ---------------------------------------------------------------------------- */
			}
		}	
	}

	for(i = 0; i < 2; i++)
	{
		if(attachPosition[i] == SCH_ATTACH_POS_APPEND)
		{
			switch(attachType[i])
			{
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_NUMBER:

				if((strlen(eventName) + (strlen(numbStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(appendStr,"_");
					strcat(appendStr,numbStr);
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_DATE:

				if((strlen(eventName) + (strlen(dateStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(appendStr,"_");
					strcat(appendStr,dateStr);
				}

				break;
			/* ---------------------------------------------------------------------------- */
			case SCH_ATTACH_TYPE_TIME:

				if((strlen(eventName) + (strlen(timeStr)+1) + strlen(fileNameStr)) < (128 - 6))
				{
					strcat(appendStr,"_");
					strcat(appendStr,timeStr);
				}

				break;
			/* ---------------------------------------------------------------------------- */
			default:
				break;
			/* ---------------------------------------------------------------------------- */
			}
		}	
	}

	strcat(fileNameStr,prefixStr);

	longName = FALSE;
	if((strlen(prefixStr) + strlen(eventName) + strlen(appendStr)) > SCH_MAX_FILENAME_LENGTH)
	{
		strncat(fileNameStr, eventName, (SCH_MAX_FILENAME_LENGTH - strlen(prefixStr) - strlen(appendStr)));

		longName = TRUE;
	}
	else
	{
		strcat(fileNameStr, eventName);
	}

	strcat(fileNameStr,appendStr);

	strcat(fileNameStr,".rec");

	// ------------- Add padding to start time ----------------

	schEventStartInMins = ((((eventStartTime >> 16) & 0xFFFF) * 24 * 60) + (((eventStartTime >> 8) & 0xFF) * 60) + (eventStartTime & 0xFF));
	schPaddingStartInMins = (((schUserData[searchIndex].searchStartPadding >> 8) & 0xFF) * 60) + (schUserData[searchIndex].searchStartPadding & 0xFF);
	schTimerStartInMins = schEventStartInMins - schPaddingStartInMins;

	schTimerStartMjd = (schTimerStartInMins / (24 * 60)) & 0xFFFF;
	schTimerStartHour = ((schTimerStartInMins % (24 * 60)) / 60) & 0xFF;
	schTimerStartMin = ((schTimerStartInMins % (24 * 60)) % 60) & 0xFF;

	// ------------- Add padding to end time ----------------

	schEventEndInMins = ((((eventEndTime >> 16) & 0xFFFF) * 24 * 60) + (((eventEndTime >> 8) & 0xFF) * 60) + (eventEndTime & 0xFF));
	schPaddingEndInMins = (((schUserData[searchIndex].searchEndPadding >> 8) & 0xFF) * 60) + (schUserData[searchIndex].searchEndPadding & 0xFF);
	schTimerEndInMins = schEventEndInMins + schPaddingEndInMins;

	schTimerEndMjd = (schTimerEndInMins / (24 * 60)) & 0xFFFF;
	schTimerEndHour = ((schTimerEndInMins % (24 * 60)) / 60) & 0xFF;
	schTimerEndMin = ((schTimerEndInMins % (24 * 60)) % 60) & 0xFF;

	// -------------------------------------------------------

	schTimerInfo.isRec = isRec;
	schTimerInfo.tuner = 3;							// Next available tuner
	schTimerInfo.svcType = schUserData[searchIndex].searchTvRadio;		// 0 TV, 1 Radio
	schTimerInfo.reserved = 0;	
	schTimerInfo.svcNum = svcNum;						// Channel number
	schTimerInfo.reservationType = 0;					// one time
	schTimerInfo.nameFix = 1;
	schTimerInfo.duration = (schTimerEndInMins - schTimerStartInMins);
	schTimerInfo.startTime = (schTimerStartMjd << 16) + (schTimerStartHour << 8) + schTimerStartMin;
	strcpy(schTimerInfo.fileName, fileNameStr);

#ifndef WIN32
	timerError = TAP_Timer_Add(&schTimerInfo);
#else
	timerError = TAP_Timer_Add_SDK(&schTimerInfo);
#endif
	if(timerError == 0)
	{
		result = SCH_MAIN_TIMER_SUCCESS;
	}
	ef ((timerError==1) || (timerError==2)){
		// 1 - Can't add
		// 2 - Invalid Tuner
	}
	ef (timerError>=0xffff0000)
	{//Timer error indicates the timer that overlaps.

		if(schMainConflictOption == SCH_MAIN_CONFLICT_COMBINE)
		{
			memset(logBuffer,0,LOG_BUFFER_SIZE);
			TAP_Timer_GetInfo( (timerError&0x0000ffff), &conflictTimerInfo );
			/*sprintf( logBuffer, "fileName = %s", conflictTimerInfo.fileName );
			logStoreEvent(logBuffer);*/
			memset(fileNameStrConflict, 0, 132);
			memset(multipleConflictCount, 0 ,3);
			strncpy(fileNameStrConflict, conflictTimerInfo.fileName, strlen(conflictTimerInfo.fileName)-4);
			sprintf(multipleConflictCount,"%d", schMainFindMultipleConflictCount(fileNameStrConflict)+1);
			/*sprintf( logBuffer, "fileNameStriped = %s\nChar:%s\n", fileNameStrConflict , multipleConflictCount);
			logStoreEvent(logBuffer);*/

			//End MJDtime is calculated by... startMJD + minutes + (hours bitshift left 1 byte)
			endConflictMJD=conflictTimerInfo.startTime + (conflictTimerInfo.duration % 60) +((conflictTimerInfo.duration/60)<<8);
			if (((endConflictMJD) & 0xff)> 59) {  //minutes >=1 hour (can't be more than 1 hour of rollover to add), add 196 as this is a BCD rep and we need to rollover into the next byte (256-60=)...
				endConflictMJD+=196;
			}
			if (((endConflictMJD>>8) & 0xff)> 23) { //hours>=1 day, so as BCD can store >10days in hours, add number of days-1 onto days in mjd ((hours/24)-1)*256, then add remaining hours onto a entire day 256+(hours%24) so it rolls over, then bitshift left into the hours & days bytes.  So for normal cases, if hours=25 hours/24=1, 1-1=0, 0*256=0, hours%24=1, 0+1+256=257 = 1 day, 1hour.
				endConflictMJD+=(((((endConflictMJD>>8)&0xff)/24)-1)*256)+(256+(((endConflictMJD>>8)&0xff) % 24))<<8;  //endConflictMJD+=(((endConflictMJD>>8)&0xff)-24)<<16;  //this is wrong, it forgets about hours.
			}
			endTimerMJD=schTimerInfo.startTime + (schTimerInfo.duration % 60) +((schTimerInfo.duration/60)<<8);
			if (((endTimerMJD>>8) & 0xff)> 23){
				endTimerMJD+=(((((endTimerMJD>>8)&0xff)/24)-1)*256)+(256+(((endTimerMJD>>8)&0xff) % 24))<<8;
			}
			if (((endTimerMJD) & 0xff)> 59) {
				endTimerMJD+=196;
			}
			/*sprintf( logBuffer, "conflictDuration = %d", conflictTimerInfo.duration );
			logStoreEvent(logBuffer);
			TAP_ExtractMjd( conflictTimerInfo.startTime>>16, &year, &month, &day, &weekDay) ;
			sprintf( logBuffer, "conflictStartTime = %d/%d/%d %02d:%02d", day, month, year, (conflictTimerInfo.startTime&0xff00)>>8, (conflictTimerInfo.startTime&0xff) );
			logStoreEvent(logBuffer);		
			TAP_ExtractMjd( endConflictMJD>>16, &year, &month, &day, &weekDay) ;
			sprintf( logBuffer, "conflictEndTime = %d/%d/%d %02d:%02d", day, month, year, (endConflictMJD&0xff00)>>8, (endConflictMJD&0xff) );
			logStoreEvent(logBuffer);
			
			sprintf( logBuffer, "newduration = %d", schTimerInfo.duration );
			logStoreEvent(logBuffer);		
			TAP_ExtractMjd( schTimerInfo.startTime>>16, &year, &month, &day, &weekDay) ;
			sprintf( logBuffer, "newstartTime = %d/%d/%d %02d:%02d", day, month, year, (schTimerInfo.startTime&0xff00)>>8, (schTimerInfo.startTime&0xff) );
			logStoreEvent(logBuffer);
			TAP_ExtractMjd( endTimerMJD>>16, &year, &month, &day, &weekDay) ;
			sprintf( logBuffer, "newEndTime = %d/%d/%d %02d:%02d", day, month, year,(endTimerMJD&0xff00)>>8,(endTimerMJD&0xff));
			logStoreEvent(logBuffer);

			sprintf( logBuffer, "--Conflict with:  %s",schTimerInfo.fileName);
			logStoreEvent(logBuffer);
			sprintf( logBuffer, "svcType = %d", conflictTimerInfo.svcType );
			logStoreEvent(logBuffer);		
			sprintf( logBuffer, "svcNum = %d", conflictTimerInfo.svcNum );
			logStoreEvent(logBuffer);		
			sprintf( logBuffer, "reservationType = %d", conflictTimerInfo.reservationType );
			logStoreEvent(logBuffer);		
			sprintf( logBuffer, "duration = %d", conflictTimerInfo.duration );
			logStoreEvent(logBuffer);		
			sprintf( logBuffer, "fileName = %s", conflictTimerInfo.fileName );
			logStoreEvent(logBuffer);
							
			sprintf( logBuffer, "//Do nothing.");*/
			//If conflict is on the same channel and we are looking at a once only timer.
			if ((schTimerInfo.svcNum==conflictTimerInfo.svcNum) && (conflictTimerInfo.reservationType==RESERVE_TYPE_Onetime)){
				//Start of my timer is <= start time of conflict and end time is >= end, so new timer will include all of the old timer.
			if ((schTimerInfo.startTime<=conflictTimerInfo.startTime) && (endTimerMJD>=endConflictMJD)){
					//check for exact match
				if ((schTimerInfo.startTime==conflictTimerInfo.startTime) && (endTimerMJD==endConflictMJD)){
						//New Timer is old timer, do nothing.
						//sprintf( logBuffer, "//New Timer is old timer, do nothing.");
					} else {
						//New Timer includes all of old timer, remove old timer and add new timer.
						//We could edit existing timer rather than remove, but new timer must be acceptable to be set otherwise there wouldn't be a search looking for this, so new name is acceptable.
						
						//sprintf( logBuffer, "//New Timer includes all of old timer, remove old timer and add new timer.");
						if (TAP_Timer_Delete((timerError&0x0000ffff))) {
#ifndef WIN32
							timerError = TAP_Timer_Add(&schTimerInfo);
#else
							timerError = TAP_Timer_Add_SDK(&schTimerInfo);
#endif
							//Did it add OK?
							if (timerError!=0) { //No it didn't restore old timer (Run Away, Run Away!!!)
#ifndef WIN32
								if (TAP_Timer_Add(&conflictTimerInfo)!=0) {  //Failed to restore old timer.
#else
								if (TAP_Timer_Add_SDK(&conflictTimerInfo)!=0) {  //Failed to restore old timer.
#endif
									sprintf( logBuffer, "//bugger, unable to recreate deleted timer.");
									logStoreEvent(logBuffer);
								}
							} else {
								conflictStatus=1;
							}
						} else {  //Unsure if this can happen, API spec doesn't specify
							sprintf( logBuffer, "//Unable to delete old timer.");
							logStoreEvent(logBuffer);
						}
					}
				}
				//We simply check to see if the start time of the conflicting (existing) timer is between the start and end of the new timer we are trying to create.  We don't check they are <= or >= as they would be adjacent, not overlapping.
				if((conflictTimerInfo.startTime>schTimerInfo.startTime) && (conflictTimerInfo.startTime<endTimerMJD)){
					//New timer overlaps the start of the old timer
					//sprintf( logBuffer, "//New timer overlaps the start of the old timer");
					
					//Edit existing timer, set start time to be earlier start time, then duration to be new existingEnd-newStart time.
					conflictTimerInfo.startTime=schTimerInfo.startTime;
					//If the MJD (days) differ then the hours/mins will provide negative values, we compensate for this by checking that the days are the same and if they arn't adding 1440 minutes onto it, ie start 2300 finish 0100 is -1320 minutes add numMinsInDay (1440) = 120.. Ta Da!!!
					conflictTimerInfo.duration=(((endConflictMJD>>16)-(conflictTimerInfo.startTime>>16))*1440)+ ((((endConflictMJD>>8)&0xff)-((conflictTimerInfo.startTime>>8)&0xff))*60)+ ((endConflictMJD&0xff)-(conflictTimerInfo.startTime&0xff));
					memset(fileNameStr,0,132);
					strcat(fileNameStr,prefixStr);
					strcat(fileNameStr,"[M");
					strcat(fileNameStr,multipleConflictCount);
					strcat(fileNameStr,"]");
					i=(SCH_MAX_FILENAME_LENGTH-(strlen(prefixStr) + strlen(appendStr) + 7))/2;
					//If the second name in the string is less than it's allowed, increase the first string by the amount the second string is less.
					strncat(fileNameStr, eventName, (strlen(fileNameStrConflict)>i ? i : (i*2)-strlen(fileNameStrConflict)));
					strcat(fileNameStr," & ");
					strncat(fileNameStr, fileNameStrConflict, (SCH_MAX_FILENAME_LENGTH-(strlen(appendStr) + strlen(fileNameStr))));
					strcat(fileNameStr,appendStr);
					strcat(fileNameStr,".rec");
					strcpy(conflictTimerInfo.fileName, fileNameStr);
					timerError=TAP_Timer_Modify((timerError&0x0000ffff), &conflictTimerInfo);
					if (timerError!=0){
						sprintf( logBuffer, "Modify timer error %d", timerError);
						logStoreEvent(logBuffer);
					} else {
						conflictStatus=2;
						result = SCH_MAIN_TIMER_SUCCESS;
					}
				}
				//We simply check to see if the end time of the conflicting (existing) timer is between the start and end of the new timer we are trying to create.  We don't check they are <= or >= as they would be adjacent, not overlapping.
				if((endConflictMJD>schTimerInfo.startTime) && (endConflictMJD<endTimerMJD)){
					//New timer overlaps the end of the old timer
					//sprintf( logBuffer, "//New timer overlaps the end of the old timer");
					
					//Edit existing timer, start time OK, end time not, so change duration to be newEnd-existingStart time.
					
					//If the MJD (days) differ then the hours/mins will provide negative values, we compensate for this by checking that the days are the same and if they arn't adding 1440 minutes onto it, ie start 2300 finish 0100 is -1320 minutes add numMinsInDay (1440) = 120.. Ta Da!!!
					conflictTimerInfo.duration=(((endTimerMJD>>16)-(conflictTimerInfo.startTime>>16))*1440)+ ((((endTimerMJD>>8)&0xff)-((conflictTimerInfo.startTime>>8)&0xff))*60)+ ((endTimerMJD&0xff)-(conflictTimerInfo.startTime&0xff));
					memset(fileNameStr,0,132);
					strcat(fileNameStr,prefixStr);
					strcat(fileNameStr,"[M");
					strcat(fileNameStr,multipleConflictCount);
					strcat(fileNameStr,"]");
					i=(SCH_MAX_FILENAME_LENGTH-(strlen(prefixStr) + strlen(appendStr) + 7))/2;
					//If the second name in the string is less than it's allowed, increase the first string by the amount the second string is less.
					strncat(fileNameStr, fileNameStrConflict, (strlen(fileNameStrConflict)>i ? i : (i*2)-strlen(fileNameStrConflict)));
					strcat(fileNameStr," & ");
					strncat(fileNameStr, eventName, (SCH_MAX_FILENAME_LENGTH-(strlen(appendStr) + strlen(fileNameStr))));
					strcat(fileNameStr,appendStr);
					strcat(fileNameStr,".rec");
					strcpy(conflictTimerInfo.fileName, fileNameStr);
#ifndef WIN32
					timerError=TAP_Timer_Modify((timerError&0x0000ffff), &conflictTimerInfo);
#else
					timerError=TAP_Timer_Modify_SDK((timerError&0x0000ffff), &conflictTimerInfo);
#endif
					if (timerError!=0){
						sprintf( logBuffer, "Modify timer error %d", timerError);
						logStoreEvent(logBuffer);
					} else {
						conflictStatus=4;
						result = SCH_MAIN_TIMER_SUCCESS;
					}
				}
			} else {
				//sprintf( logBuffer, "//two different channels or wrong timer type, need more code for this problem.");
			}
			//logStoreEvent(logBuffer);
	/*
			sprintf( logBuffer, "--Conflict with:  %s",schTimerInfo.fileName);
			logStoreEvent(logBuffer);
			TAP_ExtractMjd( conflictTimerInfo.startTime>>16, &year, &month, &day, &weekDay) ;
			sprintf( logBuffer, "startTime = %d/%d/%d %02d:%02d", day, month, year, (conflictTimerInfo.startTime&0xff00)>>8, (conflictTimerInfo.startTime&0xff) );
			logStoreEvent(logBuffer);		
			sprintf( logBuffer, "duration = %d", conflictTimerInfo.duration );
			logStoreEvent(logBuffer);		
			sprintf( logBuffer, "fileName = %s", conflictTimerInfo.fileName );
			logStoreEvent(logBuffer);
			
			sprintf( logBuffer, "--Done timer error : %d\r\n", timerError & 0x0000ffff);
			logStoreEvent(logBuffer);*/
		}
		else if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE || schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING)
		{
			timerErrorA = timerError;

			TAP_Timer_GetInfo( (timerError & 0x0000ffff), &conflictTimerInfoA );
			if(schProcessConflict(&schTimerInfo, eventStartTime, eventEndTime, timerError) == TRUE)
			{
//TAP_Print("Process OK\r\n");

#ifndef WIN32
				timerError = TAP_Timer_Add(&schTimerInfo);
#else
				timerError = TAP_Timer_Add_SDK(&schTimerInfo);
#endif
				if
				(
					(timerError == 1)
					||
					(timerError == 2)
					||
					(timerError == timerErrorA)
				)
				{
//TAP_Print("Invalid\r\n");
					// Recover Conflict A

					TAP_Timer_Modify((timerErrorA & 0x0000ffff), &conflictTimerInfoA);
				}
				else if (timerError >= 0xffff0000)
				{
//TAP_Print("New conflict\r\n");
					timerErrorB = timerError;

					TAP_Timer_GetInfo( (timerError & 0x0000ffff), &conflictTimerInfoB );

					if(schProcessConflict(&schTimerInfo, eventStartTime, eventEndTime, timerError) == TRUE)
					{
#ifndef WIN32
						timerError = TAP_Timer_Add(&schTimerInfo);
#else
						timerError = TAP_Timer_Add_SDK(&schTimerInfo);
#endif
						if(timerError != 0)
						{
//TAP_Print("Second conflict error\r\n");
							// Recover Conflict A and B

							TAP_Timer_Modify((timerErrorA & 0x0000ffff), &conflictTimerInfoA);

							TAP_Timer_Modify((timerErrorB & 0x0000ffff), &conflictTimerInfoB);
						}
						else
						{
//TAP_Print("Success B\r\n");
							// Success

							schConflictUpdateMoveList(&conflictTimerInfoA, timerErrorA);

							schConflictUpdateMoveList(&conflictTimerInfoB, timerErrorB);

							schMoveListModified = TRUE;

							result = SCH_MAIN_TIMER_SUCCESS_WITH_MODIFICATIONS;
						}
					}
				}
				else
				{
//TAP_Print("Success A\r\n");
					// Success

					schConflictUpdateMoveList(&conflictTimerInfoA, timerErrorA);

					schMoveListModified = TRUE;

					result = SCH_MAIN_TIMER_SUCCESS_WITH_MODIFICATIONS;
				}
			}
		}
		else
		{
		}
	}
	else
	{
	}

	if (( timerError == 0) && ((conflictStatus & 6)==0)) {  
		if
		(
			(strlen(schUserData[searchIndex].searchFolder) > 0)
			&&
			(schMainTotalValidMoves < SCH_MAX_MOVES)
		)
		{
			schMoveData[schMainTotalValidMoves].moveEnabled = TRUE;
			strcpy(schMoveData[schMainTotalValidMoves].moveFileName, fileNameStr);
			strcpy(schMoveData[schMainTotalValidMoves].moveFolder, schUserData[searchIndex].searchFolder);
			schMoveData[schMainTotalValidMoves].moveStartTime = schTimerInfo.startTime;
			temp = schMainConvertTimeToMins(schTimerInfo.startTime) + schTimerInfo.duration;
			schMoveData[schMainTotalValidMoves].moveEndTime = schMainConvertMinsToTime(temp);
			schMoveData[schMainTotalValidMoves].moveFailedCount = 0;

			schMainTotalValidMoves++;

			schWriteMoveList();
		}
		else if(schMoveListModified == TRUE)
		{
			schWriteMoveList();
		}
		else
		{
		}
	}
	else
	{
	}

	return result;
}



char *schMainstrstrwc_i(char *text,char *pattern)
{
   char *pptr,*tptr;

   pptr=pattern;
   tptr=text;

   // Sanity check
   //
   if ( *tptr == '\0' )
      return text;

   // Enter search loop while we have a pattern to match
   //
   while ( *pptr != '\0' )
   {
      // We have more to match - if we are at the end of text string then no match found
      // (unless the next char is $
      //
      if ( *tptr ==  '\0' )
      {
         if ( *pptr == '$' )
            return text;
         else
            return NULL;
      }

      switch(*pptr)
      {
      case '_':
         break;

      case '*':
         // Recursivly search
         //
         if ( schMainstrstrwc_i(tptr,pptr+1) != NULL )
            return text;

         // No match here - move on to next char - but pattern pos stays the same
         //
         tptr++;
         continue;

      default:
         // Just compare characters
         //
         if ( *pptr != *tptr )
            return NULL;
         break;
      }
   
      // Advance both pattern and search pos
      //
      pptr++;
      tptr++;
   }

   return text;
}

char *schMainstrstrwc(char *text,char *pattern)
{
   char upattern[255];
   char *val;

   if ( text == NULL || pattern == NULL )
      return NULL;

   // Mangle the search pattern 
   //
   switch(*pattern)
   {
   case '*':
      strcpy(upattern,pattern); 
      break;

   case '^':
      strcpy(upattern,pattern+1); 
      break;

   default:
      // Add a * to the beginning of the pattern
      //
      strcpy(upattern,"*");
      strcat(upattern,pattern); 
      break;
   }

   val=schMainstrstrwc_i(text,upattern);
   return val;
}

void schMainGetNextChar(SEARCH *search,char *c)
{
   search->search_ptr+=1;
   *c=search->expr[search->search_ptr];
}

void schMainBufTok(SEARCH *search, char c)
{
   int i;
   char inspect;

   search->token_buf[search->token_ptr++]=c;
   schMainGetNextChar(search,&inspect);

   while ( (inspect >= 'a' && inspect <='z' ) ||
           (inspect >= 'A' && inspect <='Z' ) ||
           (inspect >= '0' && inspect <='9' ) ||
           (inspect == '*' ) || 
           (inspect == ' ' ) || 
           (inspect == '$' ) || 
           (inspect == '^' ) || 
           (inspect == '_' ) )
   {
      search->token_buf[search->token_ptr++]=inspect;
      schMainGetNextChar(search,&inspect);
   }

   // Put the last character back
   //
   search->search_ptr-=1;

   // Terminate the string
   //
   search->token_buf[search->token_ptr++]='\0';

   // Trim the string of extra spaces at the end
   //
   i=search->token_ptr-2;

   while (i && search->token_buf[i] == ' ' )
      search->token_buf[i--]='\0';

}

int schMainGetNextTok(SEARCH *search)
{
   char c;

   search->token_buf[0]='\0';
   search->token_ptr=0;

   // Trim the string of extra spaces at the beginning
   //
   while( search->search_ptr + 1 <= search->search_len )
   {
      schMainGetNextChar(search,&c);

      if ( c != ' ' )
         break;
   }

   switch(c)
   {
   case '(':
      return LParen;

   case ')':
      return RParen;

   case '+':
      return And;

   case '|':
      return Or;

   case '!':
      return Not;

   case '\0':
      return EndOfLine;

   default:
      // Default - its a word
      //
      schMainBufTok(search,c);
      return Word;
   }
}

int schMainCompare(char *string,char *what)
{
   if ( schMainstrstrwc(string,what) )
      return -1;

   return 0;
}

void schMainSearchStackInit(SEARCH_STACK *stack)
{
   stack->top=-1;
}

int schMainSearchPush(SEARCH_STACK *stack, int value)
{
   stack->top++;

   if ( stack->top == MAX_STACK_VAL )
      return -1;

   stack->value[stack->top]=value;

   return 0;
}

int schMainSearchReduce(SEARCH *search)
{
   int top=search->value_stack.top;

   switch(search->operator_stack.value[search->operator_stack.top])
   {
   case And:
      if ( top < 1 )
         return -1;

      search->value_stack.value[top-1]=search->value_stack.value[top-1] && 
                                       search->value_stack.value[top];

      search->value_stack.top--;
      break;

   case Or:
      if ( top < 1 )
         return -1;

      search->value_stack.value[top-1]=search->value_stack.value[top-1] ||
                                       search->value_stack.value[top];

      search->value_stack.top--;
      break;

   case Not:
      if ( top < 0 )
         return -1;

      search->value_stack.value[top]=!search->value_stack.value[top];
      break;

   case RParen:
      search->operator_stack.top--;
      break;
   }

   search->operator_stack.top--;
   return 0;
}

int schMainSearchAdvanced(SEARCH *search)
{
   int token,val;

   search->search_len=strlen(search->expr);
   search->search_ptr=-1;

   schMainSearchStackInit(&search->operator_stack);
   schMainSearchStackInit(&search->value_stack);

   schMainSearchPush(&search->operator_stack,EndOfLine);
 
   token=schMainGetNextTok(search);

   for(;;)
   {
      if ( token == Word )
      {
         val=schMainCompare(search->description, search->token_buf);

         if ( schMainSearchPush(&search->value_stack,val) != 0 )
            return -1;

         token=schMainGetNextTok(search);
         continue;
      }

      // Token is an operator 
      //
      switch(parseTable[search->operator_stack.value[search->operator_stack.top]][token])
      {
      case R:
         if ( schMainSearchReduce(search) != 0 )
            return -1;

         break;

      case S:
         if ( schMainSearchPush(&search->operator_stack,token) != 0 )
            return -1;

         token=schMainGetNextTok(search);
         break;

      case A:
         if ( search->value_stack.top != 0 )
            return -999;
   
         if ( search->value_stack.value[0] )
            val=1;
         else
            val=0;

         return val;

      case E1:
         return -2;

      case E2:
         return -3;
 
      case E3:
         return -4;

      default:
         return -998;
         return -5;
 
      }
   }
}

int schMainSearchNormal(SEARCH *search)
{
   if ( strstr(search->description, search->expr) )
      return 1;

   return 0;
}

int schMainSearch(SEARCH *search,char *expr)
{
   char *src,*tgt;
   int advanced=0;

   src=expr;
   tgt=search->expr;

   // Convert to lowercase and determin normal or advanced search
   //
   while (*src != '\0')
   {
      if ( *src == '(' || *src == ')' ||
           *src == '^' || *src == '&' ||
           *src == '+' || *src == '!' ||
           *src == '$' || *src == '|' ||
           *src == '_' || *src == '*'  )
         advanced=-1;
   
      *tgt++=tolower(*src++);
   }

   *tgt='\0';

   if ( advanced )
      return schMainSearchAdvanced(search);

   // Default its a normal search
   //
   return schMainSearchNormal(search);
}

int schMainSearchInit(SEARCH *search,char *description)
{
   char *src,*tgt;

   if ( search == NULL || description == NULL )
      return -1;

   memset(search,0,sizeof(SEARCH));

   // Convert description to lower case
   //
   src=description;
   tgt=search->description;

   while (*src != '\0')
      *tgt++=tolower(*src++);

   *tgt='\0';

   return 0;
}


bool schMainCompareStrings(char *eventName, char *searchTerm,int options)
{
	int eventNameIndex = 0;
	bool foundSearchTerm = FALSE;
	int eventNameLength = 0;
	int searchTermLength = 0;

	eventNameLength = strlen(eventName);
	searchTermLength = strlen(searchTerm);

	// Sanity checks
	//
	if ( (eventNameLength == 0) || (searchTermLength == 0) )
		return FALSE;

	if(strlen(eventName) < strlen(searchTerm))
		return FALSE;

	// Are we doing advanced searching ( +, | and ! with wildcards?)
	//
	if ( options & SCH_USER_DATA_OPTIONS_ADVANCED_SEARCH )
	{
		SEARCH mysearch;

		if ( schMainSearchInit(&mysearch,eventName) != 0 )
			return FALSE;

                switch ( schMainSearch(&mysearch,searchTerm) )
                {
                case 0: // search not found
			return FALSE;
 
                case 1: // search found
			return TRUE;

                default: // error
			break;
		}

		return FALSE;
	}

	// Default - existing search
	//
	for( eventNameIndex=0; ((eventNameIndex <= (eventNameLength - searchTermLength)) && (foundSearchTerm == FALSE)); eventNameIndex++ )
	{
//		if(eventName[eventNameIndex] == searchTerm[0])
//		{
			if(strncasecmp(&eventName[eventNameIndex],searchTerm,searchTermLength) == 0)
			{
				foundSearchTerm = TRUE;
		}
//		}
	}

	return foundSearchTerm;
}


void schMainInitLcnToSvcNumMap(void)
{
	int i = 0;
	TYPE_TapChInfo chInfo;

#ifndef WIN32
	TAP_Channel_GetTotalNum( &schTotalTvSvc, &schTotalRadioSvc );
#else
	TAP_Channel_GetTotalNum_SDK( &schTotalTvSvc, &schTotalRadioSvc );
#endif

	schLcnToServiceTv = (word*)TAP_MemAlloc(schTotalTvSvc * 2);
	for (i = 0; i < schTotalTvSvc; i++)
	{
		TAP_Channel_GetInfo(SCH_TV,i,&chInfo);

		schLcnToServiceTv[i] = chInfo.logicalChNum; 
	}

	schLcnToServiceRadio = (word*)TAP_MemAlloc(schTotalRadioSvc * 2);
	for (i = 0; i < schTotalRadioSvc; i++)
	{
		TAP_Channel_GetInfo(SCH_RADIO,i,&chInfo);

		schLcnToServiceRadio[i] = chInfo.logicalChNum; 
	}
}


void schMainUpdateSearchList(void)
{
	byte i = 0;

	for(i = 0; i < schMainTotalValidRemoteSearches; i++)
	{
		if(schMainTotalValidSearches < SCH_MAX_SEARCHES)
		{
			schUserData[schMainTotalValidSearches] = schRemoteData[i];

			schMainTotalValidSearches++;
		}
	}

	schWriteSearchList();
}


void schMainDetermineChangeDirType(void)
{
	int	totalFileCount = 0;
	TYPE_File	tempFile;

	schMainChangeDirAvailable = FALSE;

	if(GotoRoot() == TRUE)
	{
		schMainChangeDirSuccess = TAP_Hdd_ChangeDir("DataFiles");

		totalFileCount = TAP_Hdd_FindFirst(&tempFile); 

		if(strcmp(tempFile.name, "__ROOT__") != 0)
		{
			if(GotoRoot() == TRUE)
			{
				schMainChangeDirFail = TAP_Hdd_ChangeDir("ThisDirectoryDoesNotExist");

				totalFileCount = TAP_Hdd_FindFirst(&tempFile); 

				if
				(
					(schMainChangeDirSuccess != schMainChangeDirFail)
					&&
					(strcmp(tempFile.name, "__ROOT__") == 0)
				)
				{
					schMainChangeDirAvailable = TRUE;
				}
			}
		}
	}
}

int schMainFindMultipleConflictCount(char *toClean){
//Needs cleaning...
	char * sub_text;
	char output[132];
	int i;
	i=1;
	if((sub_text=(char *)(strstr(toClean, "[M")))!=NULL){
		if(strncmp(sub_text+2,"]",1)){
			//output =(char *) malloc(sizeof(toClean));
			//memset(output,0,strlen(toClean));
			memset(output,0,132);
			i=toClean[(sub_text-toClean)+2]-48;
			i=(i>9 ? 1 : i);
			strncat(output, toClean, (sub_text-toClean));
			strcat(output, (sub_text+4));
			strcpy(toClean, output);
		}
	}
	return(i);
}


dword schMainConvertTimeToMins(dword time)
{
	return ((((time >> 16) & 0xFFFF) * 24 * 60) + (((time >> 8) & 0xFF) * 60) + (time & 0xFF));
}


dword schMainConvertMinsToTime(dword timeInMins)
{
	return ((((timeInMins / (24 * 60)) & 0xFFFF) << 16) + ((((timeInMins % (24 * 60)) / 60) & 0xFF) << 8) + (((timeInMins % (24 * 60)) % 60) & 0xFF));
}


bool schProcessConflict(TYPE_TimerInfo* schTimerInfo, dword eventStartTime, dword eventEndTime, int timerError)
{
	TYPE_TimerInfo	conflictTimerInfo;
	dword	endConflictInMins = 0, endTimerInMins = 0, endEventInMins = 0, currentTimeInMins = 0;
	bool	result = FALSE;

char buffer1[256];

	currentTimeInMins = (schTimeMjd * 24 * 60) + (schTimeHour * 60) + schTimeMin;

	TAP_Timer_GetInfo( (timerError & 0x0000ffff), &conflictTimerInfo );

	if(schMainConvertTimeToMins(conflictTimerInfo.startTime) > (currentTimeInMins + 2))
	{
		endConflictInMins = schMainConvertTimeToMins(conflictTimerInfo.startTime) + schMainConvertTimeToMins(conflictTimerInfo.duration);
		endTimerInMins = schMainConvertTimeToMins(schTimerInfo->startTime) + schMainConvertTimeToMins(schTimerInfo->duration);
		endEventInMins = schMainConvertTimeToMins(eventEndTime);

//sprintf(buffer1,"Event %02d:%02d - %02d:%02d\r\n",((eventStartTime >> 8) & 0xFF), (eventStartTime & 0xFF), ((eventEndTime >> 8) & 0xFF), (eventEndTime & 0xFF));
//TAP_Print(buffer1);

//sprintf(buffer1,"Original Timer %02d:%02d - %02d:%02d\r\n",((schTimerInfo->startTime >> 8) & 0xFF), (schTimerInfo->startTime & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);

//sprintf(buffer1,"Original Conflict %02d:%02d - %02d:%02d\r\n",((conflictTimerInfo.startTime >> 8) & 0xFF), (conflictTimerInfo.startTime & 0xFF), ((((schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);

		if
		(
			(conflictTimerInfo.startTime <= eventStartTime)
			&&
			(endConflictInMins >= endEventInMins)
		)
		{
//TAP_Print("No modification required. Event already covered\r\n");

			/* Do Nothing - Event already covered */

		}
		else if
		(
			(conflictTimerInfo.startTime <= schTimerInfo->startTime)
			&&
			(endConflictInMins > schMainConvertTimeToMins(schTimerInfo->startTime))
			&&
			(endConflictInMins < endTimerInMins)
		)
		{
//TAP_Print("Conflict Before New Event\r\n");

			/* Conflict before new event */

			if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE) //Discard paddings between timers
			{
				if(conflictTimerInfo.reservationType == RESERVE_TYPE_Onetime)
				{
					if(schTimerInfo->svcNum == conflictTimerInfo.svcNum)
					{
						schTimerInfo->startTime = eventStartTime;
						schTimerInfo->duration = endTimerInMins - schMainConvertTimeToMins(eventStartTime);
	
						if(endConflictInMins > schMainConvertTimeToMins(eventStartTime))
						{
							// Remove end padding from conflict
	
							conflictTimerInfo.duration -= (endConflictInMins - schMainConvertTimeToMins(eventStartTime));
	
							if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
							{
								result = TRUE;
							}
						}
						else
						{
							result = TRUE;
						}
					}
					else //conflict with timer in another channel
					{
//janilxx: This should work but for some reason this code is not called at all
						schTimerInfo->startTime = schMainConvertMinsToTime(schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration);
						schTimerInfo->duration = endTimerInMins - schMainConvertTimeToMins(schTimerInfo->startTime);
		
						result = TRUE;
					}
				}
			}
			else if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING) //Keep end padding between timers
			{
//janilxx: This works for one channel timers. 
//This should work also for multi-channel timers but for some reason this code is not called at all
				schTimerInfo->startTime = schMainConvertMinsToTime(schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration);
				schTimerInfo->duration = endTimerInMins - schMainConvertTimeToMins(schTimerInfo->startTime);

				result = TRUE;
			}	
		}
		else if
		(
			(endConflictInMins >= endTimerInMins)
			&&
			(schMainConvertTimeToMins(conflictTimerInfo.startTime) < endTimerInMins)
			&&
			(conflictTimerInfo.startTime > schTimerInfo->startTime)
		)
		{
//TAP_Print("Conflict After New Event\r\n");

			/* Conflict after new event */
			
			if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE) //Discard paddings between timers
			{
				if(conflictTimerInfo.reservationType == RESERVE_TYPE_Onetime)
				{
					if(schTimerInfo->svcNum == conflictTimerInfo.svcNum)
					{
						schTimerInfo->duration = endEventInMins - schMainConvertTimeToMins(schTimerInfo->startTime);
	
						if(schMainConvertTimeToMins(conflictTimerInfo.startTime) < endEventInMins)
						{
							// Remove start padding from conflict
	
							conflictTimerInfo.duration -= (endEventInMins - schMainConvertTimeToMins(conflictTimerInfo.startTime));
							conflictTimerInfo.startTime = eventEndTime;
	
							if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
							{
								result = TRUE;
							}
						}
						else
						{
							result = TRUE;
						}
					}
					else //conflict with timer in another channel
					{
//janilxx: This should work but I suppose this is not called either (because codes above are not run either (codes with "janilxx comment))
						//Let's make an assumption:
						//Usually start padding is smaller than end padding and usually used paddings are quite same size.
						//-->
						//If old timer's (conflict timer) start padding (difference to new timer program's end time)
						//is smaller or equal than new timer's end padding,
						//let's assume that instead of timering two or more programs with one timer, 
						//it is only a start padding for one timered program.
						//-->
						//And because of this, we can safely remove the start padding.

						//old timer's start padding: endEventInMins - schMainConvertTimeToMins(conflictTimerInfo.startTime)
						//new timer's end padding: endTimerInMins - endEventInMins
						if 
						( 							
							(endEventInMins - schMainConvertTimeToMins(conflictTimerInfo.startTime)) 
							<= 
							(endTimerInMins - endEventInMins)
						)
						{
//janilxx: Timer change code missing here
						}

					}
				}
			}
			else if(schMainConflictOption == SCH_MAIN_CONFLICT_SEPARATE_KEEP_END_PADDING) //Keep end padding between timers
			{
				if(schTimerInfo->svcNum == conflictTimerInfo.svcNum)
				{
					conflictTimerInfo.startTime = schMainConvertMinsToTime(schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration);
					conflictTimerInfo.duration = endConflictInMins - schMainConvertTimeToMins(conflictTimerInfo.startTime);
	
					if(TAP_Timer_Modify((timerError & 0x0000ffff), &conflictTimerInfo) == 0)
					{
						result = TRUE;
					}
				}
				else //conflict with timer in another channel
				{
//janilxx: This is not coded yet. The same idea than above was designed to be used
					//Let's make an assumption:
					//Usually start padding is smaller than end padding and usually used paddings are quite same size.
					//-->
					//If old timer's (conflict timer) start padding (difference to new timer program's end time)
					//is smaller or equal than new timer's end padding,
					//let's assume that instead of timering two or more programs with one timer, 
					//it is only a start padding for one timered program.
					//-->
					//And because of this, we can safely remove the start padding.

//janilxx: If missing here
					//old timer's start padding: [[[something]]]
					//new timer's end padding: [[[something]]]
					if ( TRUE )
					{
//janilxx: Timer change code missing here						
					}		
				}
			}	
		}
		else
		{
		}
	}

//sprintf(buffer1,"Modified Timer %02d:%02d - %02d:%02d\r\n",((schTimerInfo->startTime >> 8) & 0xFF), (schTimerInfo->startTime & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(schTimerInfo->startTime) + schTimerInfo->duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);

//sprintf(buffer1,"Modified Conflict %02d:%02d - %02d:%02d\r\n",((conflictTimerInfo.startTime >> 8) & 0xFF), (conflictTimerInfo.startTime & 0xFF), ((((schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration) % (24 * 60)) / 60) & 0xFF), ((((schMainConvertTimeToMins(conflictTimerInfo.startTime) + conflictTimerInfo.duration) % (24 * 60)) % 60) & 0xFF));
//TAP_Print(buffer1);

	return result;
}


void schConflictUpdateMoveList(TYPE_TimerInfo* origConflictTimerInfo, dword modTimerError)
{
	TYPE_TimerInfo	modConflictTimerInfo;
	dword origConflictTimerEndTime = 0, temp = 0;
	int i = 0;
	
	temp = schMainConvertTimeToMins(origConflictTimerInfo->startTime) + origConflictTimerInfo->duration;
	origConflictTimerEndTime = schMainConvertMinsToTime(temp);

	for ( i = 0; i < schMainTotalValidMoves; i++)
	{
		if
		(
			(schMoveData[i].moveEnabled == TRUE)
			&&
			(strcmp(schMoveData[i].moveFileName, origConflictTimerInfo->fileName) == 0)
			&&
			(schMoveData[i].moveStartTime == origConflictTimerInfo->startTime)
			&&
			(schMoveData[i].moveEndTime == origConflictTimerEndTime)
		)
		{
			TAP_Timer_GetInfo( (modTimerError & 0x0000ffff), &modConflictTimerInfo );

			schMoveData[i].moveStartTime = modConflictTimerInfo.startTime;

			temp = schMainConvertTimeToMins(modConflictTimerInfo.startTime) + modConflictTimerInfo.duration;
			schMoveData[i].moveEndTime = schMainConvertMinsToTime(temp);
		}
	}
}

