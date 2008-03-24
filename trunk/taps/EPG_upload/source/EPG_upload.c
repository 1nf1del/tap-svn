#define ID_SIGNAL 0x80000a0B
#include <time.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
//#include <ctype.h> //for isspace

#include <tap.h>
#include "firmware.h"
#include "Flash.h"
#include "exTap.h"

#include "EPG_upload.h"


//#include "ok.c"
//#include "okLow.c"

#include "email.gd"
#include "cog1.gd"
#include "cog2.gd"
#include "cog3.gd"
#include "yellowbutton.gd"

#define DEBUG 0

#define VERSION "3.23"

#if DEBUG
#define _PROGRAM_NAME_ "EPGUploader"VERSION""__DATE__""
#define _TAP_PROGRAM_NAME_ "EPG_upload"VERSION""__DATE__".tap"
#else
#define _PROGRAM_NAME_ "EPG Uploader "VERSION""
#define _TAP_PROGRAM_NAME_ "EPG_upload"VERSION".tap"
#endif


#define INI_FILE "epg_upload.ini"
#define LOG_FILE "epg_upload.log"
#define TIMER_FILE "epg_timers.tsv"
#define TIMER_DELETE_FILE "epg_dtimer.tsv"
#define ICE_INI_FILE "epg_uploadice.ini"
#define GMT_OFFSET_FILE "GMT_Offset.ini"
#define TOPPBIFFLOW_FILE "toppybifflow.wav"
#define TOPPBIFF_FILE "toppybiff.wav"
#define RECPLAYBACKFILE "epg_recplay.tsv"
#define RECORDINGS_FILE "epg_recordings.tsv"

TAP_ID( ID_SIGNAL );
TAP_PROGRAM_NAME(_PROGRAM_NAME_);
TAP_AUTHOR_NAME("TonyMY01");
TAP_DESCRIPTION("EPG & timer uploader, now with limited Toppybiff support");
TAP_ETCINFO(__DATE__);

#include "TSRCommander.inc"

//position the blue button
#define EBOX_X		35
#define EBOX_Y		35
#define EBOX_W		(_emailGd.width)
#define EBOX_H		(_emailGd.height)

#define CBOX_X		0
#define CBOX_Y		0
#define CBOX_W		(_cog1Gd.width)
#define CBOX_H		(_cog1Gd.height)

//position the yellow button
#define YBOX_X		510
#define YBOX_Y		476
#define YBOX_W		(_yellowbuttonGd.width)
#define YBOX_H		(_yellowbuttonGd.height)

#define printf TAP_Print

#define MAX_INI_SVCS 60


enum option
{
    USEGMTFILE = 0 ,
    GMTOFFSET , 
    NOWNEXTUPD,
    NEWFILCHQFREQNT,        
	NEWFILCHQFREQTS,
	EMAILVOLUM,
    LOADINDIV,
	LOADALL,
	BUILDTSV,
	AUTLOADTGD,
	DELETESTART,
	RELOADHOUR,
    LOADNUMDAYS,
	SHOWOSD,
	LOGLEVL,
	EITBLOCK,
	EPGLOADWAIT,
	MANUALSTRT,
	MANUALPRWF,
	MANUALGMTFX,
	ADJUSTGMTLD,
	IGNOREEXIT,
	LCNTGDLOAD,
	ICELOAD,
	TEDCREATE,
	RELOADTAP,
	SERVICDETL,
	SHOWINIFIL,
	SHOWLOGF,
	EXITTAP,
	NEXTTAP 
};




enum timeshift
{
	NTS,
	TSH
};

enum volume
{
	VOLOFF,
	LOW,
	HIGH
};

enum GMToption
{
	GMTFILE,
	GMTTOPPY,
	GMTTAP
};

enum osdisplay
{
	DISOFF,
	SMALL,
	LARGE
};

enum LoadIceIni
{
	NOICE,
	LOADNORMALINI,
	LOADICEINI
};

typedef enum
{
	FIRSTTIME,
	START,
	LOADING,
	FINISHED,
	NOMORE
} TYPE_LoadState;


typedef enum
{
	FULL_LOAD,
	NEW_LOAD,
	NO_MORE
} TYPE_SatLoadState;

//struct _reent *_impure_ptr;

typedef struct _EPG_INFO EPG_INFO;

typedef struct
{
  byte  OpCode;
  dword LocalDateTime;
  byte seconds;
} __attribute__((packed)) tFPSetTime;


//static TYPE_TimerRAM *g_timerPtr;

static TYPE_IniSvcDetails *g_IniSvcPtr;

//const int power[5]={1,10,100,1000,10000};
static word gtestEvtId=0;
static dword gbytestransferred = 0;
static int g_svcType,g_chNum,g_eventNum; //currentEvtNumber
static int g_total_tvSvc=0,g_total_radSvc=0; 
static TYPE_TapEvent *g_eventinfo;
static bool g_bchannelchange=FALSE;

static bool g_buseExitKey=TRUE;
static bool g_bLCNTGDload=TRUE;

//static bool gload9999epgevts=TRUE;

static bool g_bOptsWinShown=FALSE; //used to track options window.
//static bool g_bFileWinShown=FALSE;
static bool g_bSvcWinShown=FALSE;
static bool g_bepgFileWinShown=FALSE;
static bool g_bIniWinShown=FALSE;
static bool g_bLogWinShown = FALSE;

struct LCNStore {
					bool LCNfound;
					int  IntChannelNum;
				  };
static struct LCNStore gLCNfound[1000];				  
static bool gLCNinTGD=FALSE;
//static bool g_logfileopen = FALSE;
//static TYPE_File* g_filepointer;
//static unsigned char*    g_sprintfile;

static bool g_bMailOptsWinShown =FALSE;

static bool g_beit_uploaded=FALSE;
static bool gEITload=FALSE;  //track whether to load EIT or not.

static bool gloading=FALSE;

static bool g_bloadevts=TRUE; //no events loaded yet.

static bool g_btimer_create_delay=FALSE; //flag to track whether to delay between timers.

static bool g_scanALLevents=FALSE; //used to scan just recent events or all events

static bool gTedRecordingsfile=FALSE; //used to create a recordings file for every recording started.

static word _gFullRegion=0; //global region handle
static TYPE_Window g_winOpts; //global window handle.


//static int gfileNum=0; //tracks selected TS file
//static int gtimerNum=0; //tracks selected TS timer
static int gepgfileNum=0; //tracks selected EPG file

static bool gautoloadepg=TRUE;
static bool gloadallepg=FALSE;
static bool gautodeleteepg=FALSE;

static int g_GMToffst=0; //number of minutes of GMToffset

static int  g_NumServices=0;
static bool g_bLoadSettings=TRUE;   //loadsettings variable tha
static int g_loadtimewait=3;
static int gLoadDay=7; //number of days to load

//static bool gbfiledetailsdisplayed=FALSE;
//static bool gbtimerdetailsdisplayed=FALSE;
//static bool gbepgfiledetailsdisplayed=FALSE;

//static unsigned char gtapDirName[64];  //used to remember where tap was started from
static unsigned char gTGDdirectory[64]=""; //for storing EPG data
//static unsigned char gcurrentDir[64];
static bool g_binifiledirectoryknown=FALSE;

static unsigned char g_satname[MAX_SatName];
//static bool loadedSatOnce=FALSE;


static	dword gfileNames[40];
static	int gfilenames_count=0;

static int g_nselected; //options window selection

static int g_NextUpdateTime=-1;

static int gReloadHour=24;
static bool g_bReloaded=FALSE;

static int g_freqMin[2]={360,2}; //filecheck frequency.
//static int timeshiftmode=0;

static dword g_nlastfile=0x00000000;
static dword g_nlastsatfile=0;

static int gtimer_error_counter=0;
static int gtimer_succ_counter=0;
static int gtimer_del_counter=0;

static int gVolume=HIGH; //volume level for beep

static dword g_deleteregion=0;	//used to delete OSD messages after 4 secs in idle period.

static bool g_btapdirinifile=FALSE; //to flag if inifile found with tap (new versions will put in programfiles).

static int g_ShowLoadingOSD=LARGE;
static int g_loglevel=4;

static dword gTGDlastEITStartTime=0;
static bool gbSystemProc=FALSE;  //used to avoid running idle/event routine if calling systemproc

static int g_timerdelay=350;   //3.5second delay for setting and deleting timers.

static int g_LoadIceIni=NOICE;  //0=none, 1=load normal ini, 2=load ice ini
static bool g_brestarttap=FALSE; //hack because I can't think of an easier way...

static int g_Mailposition=0;
static int gemailtransparency=255;

static TYPE_LoadState g_loadstate=NOMORE;
static TYPE_SatLoadState g_loadedSatOnce=NO_MORE;

static bool g_bemailIconexists=FALSE;

static word _emailIconRAM;
static word _emailIconRegion=0;
static word _cogIconRAM1;
static word _cogIconRAM2;
static word _cogIconRAM3;


static word _timersUploadedRegion=0;
static word _rotateUploadedRegion=0;
static word _emailWindowRegion=0;
static word _emailOptionsRegion=0;

static char * emailarray[50];
static int g_emailcounter=0;
static bool g_bEmailTextWinShown=FALSE;

static word gTodayMJDdate;

static int g_useGMT=GMTTAP;
static word gDstStart=0, gDstEnd=0;
static bool gDstActive=FALSE;

static char *g_mailramfree=NULL;
static bool g_bcanclearRAM=FALSE;
static dword g_tick=0;

static TYPE_File *gLogFPointer=NULL;
static bool g_bcancloseLOG=FALSE;

static dword gtick_lastloaded=0;

static bool g_loadshortevents=FALSE;
static bool g_startupTimeSet=TRUE;
static bool g_powerFailTime=TRUE;
static bool g_manualGMTTime=TRUE;
static bool g_GMTloadAdjust=TRUE;

static int geventnumberarray[5000];


dword  SysID, ApplID, BIOSAddress, EventDispatcherAddress, TAPgp, FWgp, TAP_TableAddress, TAP_IdentAddress;
static dword gFlashlocation=0;
static TYPE_ClockSetup  *gClockSetting;
static dword	gGMTLocation=0;
static int g_tdtUTCoffset=0;										
static dword	gGmt_updateblock=0;
static dword  gGmt_updateblockbackup=0;
static bool gGMTPatched=FALSE;
static bool g_setTime1min=FALSE;

static char g_manualLCN[100];

static	int exTAPret;
static byte g_fileplaying=0;

static	char g_LCNinfo[0x3];

static	word gexTAPday=0;
static	byte gexTAPhour,gexTAPmin;

static char gclockSetDSTflag=0;
static bool gbpowerfailed=FALSE;
static int gwatchdog=0;
	
//static bool gbIsWaiting=FALSE;

//--------- Function Prototypes -------------------
void logf(int log,unsigned char *fmt, ...);
void createtimerTSVfile(void);
void InterpretEPGbuff( EPG_INFO *epg, const unsigned char *buf );
void clearEPG( EPG_INFO *epg );
int LoadEPG(unsigned char *filename,int filedate);
int put_chanServiceDetails_TGD(EPG_INFO *epg, int count);
//bool lookup_svcid_forNN(int topfieldchannelNum, int number, EPG_INFO *srvc);
bool blookupchannel_svcid_forNandN(word svcID, unsigned char  chName[ MAX_SvcName ]) ;

bool OsdActive (void);
void ShowMessageWin (unsigned char* lpMessage, unsigned char* lpMessage1, int time, bool logerror);
void ShowOptionsWindow (void);
void HideWindow ();
dword GetCrc32(const void *data, dword n);
void Put32bit( void *p, dword data );
byte BCD( byte value );
void showTimersUploaded(void);
void *MakeEitTable(word service_id, 
				   word transport_stream_id,
				   word original_network_id,
				   word eventId, 
				   dword startTime, 
				   dword duration,
				   word runningStatus,
				   unsigned char *evtname,
				   unsigned char *shortevt,
				   unsigned char *evtdesc, 
				   byte parental);

void bubble_sort(dword a[], int size);
void normalTAP_delay(int time);
void rotateTAP_delay(int time);
void change_chkFreq(int param,int timeshift);
bool LoadSettings(bool IceSettings);
void SaveSettings(int IceSettings);
int convert_parental(const unsigned char *buf);
void ShowTSTimerList( dword selNum );
void FindTapDir(unsigned char *tapDirName);
void ChangeDir(unsigned char *DirName);
void ChangeDirRoot();
void ShowEPGFileList( dword selNum);
void delete_old_tgd(void);
void show_timer_error(int retval, int i);
void erase_current_epg(void);
int record_timer_evt(EPG_INFO *epg);
TYPE_LoadState load_new_tgd(bool firsttime);
unsigned char *lookupReservtype(TYPE_ReservationType reserve);
dword addtime(dword date, int add);  //add minutes to the day
TYPE_LoadState load_TGD_stateMachine(TYPE_LoadState loadstate);
int WrapPutStr(dword rgn, char *str, int offset, int x, int y, int w, int fgCol, int bgCol, int maxRows, byte fntSize, int lineSep);
unsigned char *loadwav(char * filename, long * size, int * realrate, int * rate);
int delete_timer_evt(EPG_INFO *epg);

#include "settings.c"


//-------------------------------------------------
void show_timer_error(int retval, int i)
{
	unsigned char msg[300];
	char tmp[64];
	char tmp1[64];
	char tmp2[64];
	TYPE_TimerInfo timerinfo;
	TYPE_TimerInfo timerinfo1,timerinfo2;

	logf(4,"Timer error retval:0x%x",retval);

	if (i!=999)
	{
		TAP_Timer_GetInfo(i,&timerinfo);
		memset(tmp,0,sizeof tmp);
		strncpy(tmp,timerinfo.fileName,60);
	}
	switch (TAP_GetSystemVar(SYSVAR_OsdLan))
	{
	case LAN_German:
		if (retval == 0xffffffff)
			TAP_SPrint(msg,"Fehler beim Verändern von Timer %s! (ungültiger Eintrag)",tmp);
		else if (retval == 0xfffffffe)
			TAP_SPrint(msg,"Fehler beim Verändern von Timer %s! (ungültiger Tuner)",tmp);
		else if (retval >= 0xFFFE0000)
		{
			TAP_Timer_GetInfo((retval&0x000000FF),&timerinfo1);
			memset(tmp1,0,sizeof tmp1);
			strncpy(tmp1,timerinfo1.fileName,60);
			if ((retval&0xFFFF0000) == 0xFFFE0000)
			{
				TAP_Timer_GetInfo((retval&0x0000FF00)>>8,&timerinfo2);
				memset(tmp2,0,sizeof tmp2);
				strncpy(tmp2,timerinfo2.fileName,60);
			}
			if (i==999)
				TAP_SPrint(msg,"Störungen mit neuem Timer des Baus! (Konflikt mit %s %s)",tmp1,((retval&0xFFFF0000) == 0xFFFE0000)?tmp2:"");
			else
				TAP_SPrint(msg,"Fehler beim Verändern %s! (Konflikt mit %s %s)",tmp,tmp1,((retval&0xFFFF0000) == 0xFFFE0000)?tmp2:"");
		}
		else
		{
			TAP_SPrint(msg,"Fehler beim Verändern von Timer! (Unknown API return-0x%x)",retval);
		}
		//else if (retval==4)
		//	TAP_SPrint(msg,"Störungen mit Timer! (Konflikt mit mehrfachen Timern)");
		//else
		//	TAP_SPrint(msg,"Fehler beim Verändern von Timer %s! (unbekannter Fehler)",strncpy(tmp,timerinfo.fileName,60));
		logf(3,"Timer Fehler, %s",msg);
		//ShowMessageWin("Timer Fehler",msg,240);
		break;
	case 15:  //finish language
		if (retval == 0xffffffff)
			TAP_SPrint(msg,"Virhe muokattaessa ajastusta %s. (Virheellinen ajastus)",tmp);
		else if (retval == 0xfffffffe )
			TAP_SPrint(msg,"Virhe muokattaessa ajastusta %s. (Virheellinen viritin)",tmp);
		else if (retval >= 0xFFFE0000)
		{
			TAP_Timer_GetInfo((retval&0x000000FF),&timerinfo1);
			memset(tmp1,0,sizeof tmp1);
			strncpy(tmp1,timerinfo1.fileName,60);
			if ((retval&0xFFFF0000) == 0xFFFE0000)
			{
				TAP_Timer_GetInfo((retval&0x0000FF00)>>8,&timerinfo2);
				memset(tmp2,0,sizeof tmp2);
				strncpy(tmp2,timerinfo2.fileName,60);
			}
			if (i==999)
				TAP_SPrint(msg,"Virhe ajastuksen luonnissa. (Päällekkäisyys ajastuksen %s %s kanssa)",tmp1,((retval&0xFFFF0000) == 0xFFFE0000)?tmp2:"");
			else
				TAP_SPrint(msg,"Virhe muokattaessa ajastusta %s. (Päällekkäisyys ajastuksen %s %s kanssa)",tmp,tmp1,((retval&0xFFFF0000) == 0xFFFE0000)?tmp2:"");
		}
		else
		{
			TAP_SPrint(msg,"Virhe ajastuksen/muokattaessa ajastusta. (Unknown API return-0x%x)",retval);
		}
		//else if (retval==4)
		//	TAP_SPrint(msg,"Virhe muokattaessa ajastusta/ajastuksen luonnissa. (Päällekkäisyys monen ajastuksen kanssa)");
		//else
		//	TAP_SPrint(msg,"Virhe muokattaessa ajastusta %s. (Tuntematon virhe)",i+1);
		logf(3,"Ajastuksen luontivirhe %s",msg);
		//ShowMessageWin("Ajastuksen luontivirhe",msg,240);
		break;
	default:
		if (retval == 0xffffffff)
			TAP_SPrint(msg,"Error Modding Entry %s! (Invalid Entry)",tmp);
		else if (retval == 0xfffffffe )
			TAP_SPrint(msg,"Error Modding Entry %s! (Invalid Tuner)",tmp);
		else if (retval >= 0xFFFE0000)   //if ((retval&0xFFFF0000) == 0xFFFE0000)
		{
			TAP_Timer_GetInfo((retval&0x000000FF),&timerinfo1);
			memset(tmp1,0,sizeof tmp1);
			strncpy(tmp1,timerinfo1.fileName,60);
			if ((retval&0xFFFF0000) == 0xFFFE0000)
			{
				TAP_Timer_GetInfo((retval&0x0000FF00)>>8,&timerinfo2);
				memset(tmp2,0,sizeof tmp2);
				strncpy(tmp2,timerinfo2.fileName,60);
			}
			if (i==999)
				TAP_SPrint(msg,"Error Creating New Entry! (Conflicts with %s %s)",tmp1,((retval&0xFFFF0000) == 0xFFFE0000)?tmp2:"");
			else
				TAP_SPrint(msg,"Error Modding %s! (Conflicts with %s %s)",tmp,tmp1,((retval&0xFFFF0000) == 0xFFFE0000)?tmp2:"");
		}
		else
		{
			TAP_SPrint(msg,"Error Creating/Modding Entry! (Unknown API return-0x%x)",retval);
		}
		//else if (retval==4)
		//	TAP_SPrint(msg,"Error Modding/Creating New Entry! (Conflicts with multiple timers)");
		//else
		//	TAP_SPrint(msg,"Error Modding Entry %s! (Unknown Error)",i+1);
		logf(3,"Timer create FAILURE, %s",msg);
		//ShowMessageWin("Timer create FAILURE",msg,240);
	}
} //show_timer_error



void logf(int log,unsigned char *fmt, ...)
{
    word mjd;
    byte hour,min,sec;
    word year;
    byte month, day, weekDay;
//	dword heapSize,freeHeapSize,availHeapSize;
    va_list args;
    char *p,*sval;
    int ival;
    double dval;
    static char sprintfile[10240]; //since it is such a large chunk, will make static
    //char  sprintfile[2048];
    char str1[300]; //,str2[50],str3[60],str4[50];
    char str2[300];
    //gLogFPointerchar currentdir[64];
//    int i,j;
//    int idx=0;
//    int num_items=0;
    int filelength;
    //TYPE_File* gLogFPointer;
//    bool changeddir=FALSE;

    if (log<=g_loglevel)
    {
       // strcpy(currentdir,gcurrentDir);
       //if (!strstr(gTGDdirectory,currentdir)) //!strstr==0 if equal
       // {
       //     changeddir=TRUE;
       //     ChangeDir(gTGDdirectory);
       // }
        memset(sprintfile,0,sizeof sprintfile);

//        if (!TAP_Hdd_Exist(LOG_FILE)) {
//            TAP_Hdd_Create(LOG_FILE,ATTR_NORMAL);
//        }

        if (!g_bcancloseLOG) gLogFPointer = TAP_Hdd_Fopen(LOG_FILE);
		if (gLogFPointer!=0)
        {
			filelength=TAP_Hdd_Flen( gLogFPointer );
            if (filelength!=0 && (filelength%512)==0) filelength+=512;  //FRIGGIN TOPFIELD BUG! flen is 512bytes out if the filesize is a multiple of 512.
            if (filelength>sizeof sprintfile) filelength=sizeof sprintfile;
			TAP_Hdd_Fseek(gLogFPointer, 0, SEEK_SET);
            TAP_Hdd_Fread( sprintfile, filelength, 1, gLogFPointer );
		}

            TAP_GetTime( &mjd, &hour, &min, &sec);
            TAP_ExtractMjd(mjd, &year, &month, &day, &weekDay );

            sprintf(str1,"%04d/%02d/%02d %02d:%02d:%02d-",year,month,day,hour,min,sec);
            va_start(args,fmt);
            for (p=fmt;*p;p++){
                if(*p!='%'){
                    sprintf(str2,"%c",*p);
                    strcat(str1,str2);
                    continue;
                }
                switch(*++p){
            case 'd':
                ival=va_arg(args,int);
                sprintf(str2,"%d",ival);
                strcat(str1,str2);
                break;
            case 'f':
                dval=va_arg(args,double);
                sprintf(str2,"%f",dval);
                strcat(str1,str2);
                break;
            case 's':
                for (sval = va_arg(args,char *); *sval; sval++)
                {
                    sprintf(str2,"%c",*sval);
                    strcat(str1,str2);
                }
                break;
            case 'x':
                ival=va_arg(args,int);
                sprintf(str2,"%x",ival);
                strcat(str1,str2);
                break;
            case '0':  //works for %02/04d
                ival=va_arg(args,int);
                if (*++p=='4')   sprintf(str2,"%04d",ival);
                else sprintf(str2,"%02d",ival);
                *++p;
                strcat(str1,str2);
                break;

            default:
                sprintf(str2,"%c",*p);
                strcat(str1,str2);
                break;
                }
            }
            va_end(args);
            sprintf(str2,"\r\n");
            strcat(str1,str2);
            printf("%s",str1);
			
		if (gLogFPointer!=0)
		{
            if ((strlen(str1)+strlen(sprintfile))>sizeof sprintfile)
            {
                //printf("log file greater now\n");
                memmove(sprintfile,sprintfile+strlen(str1),sizeof sprintfile-strlen(str1));
                memset(sprintfile+sizeof sprintfile-strlen(str1),0,strlen(str1));
            }
            strcat(sprintfile,str1);
            TAP_Hdd_Fseek(gLogFPointer, 0, SEEK_SET);
            TAP_Hdd_Fwrite( sprintfile,sizeof sprintfile,1,gLogFPointer);
			g_bcancloseLOG=TRUE;
            //TAP_Hdd_Fclose( gLogFPointer );
        }
        else //logfile can't be opened
            if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
                ShowMessageWin("kann nicht geöffnet werden file", LOG_FILE,150,FALSE);
            else
                ShowMessageWin("can't open file", LOG_FILE,150,FALSE);

        //if (changeddir) ChangeDir(currentdir);
    }
}



unsigned char *loadwav(char * filename, long * size, int * realrate, int * rate)
{
    //char currentdir[64];
    long filelength;
    TYPE_File* fp;
//    bool changeddir=FALSE;
	dword heapSize,freeHeapSize,availHeapSize;
	unsigned char *ramspot=0;
	unsigned char wFormatTag;
	unsigned char wChannels;
	word  dwSamplesPerSec;
	unsigned char wBitsPerSample;
	unsigned char fmtchunksize;
	long datachunksize=0;
	word wtemp;
	dword dwtemp;
	unsigned char bytetemp;
	int n;
	long m;


        //memset(sprintfile,0,sizeof sprintfile);

        if (!( fp = TAP_Hdd_Fopen( filename) ))
		{
		    if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
                ShowMessageWin("kann nicht geöffnet werden file", filename,150,TRUE);
            else
                ShowMessageWin("can't open file", filename ,150,TRUE);
			*size=(long)NULL;	
			return NULL;
		}
        //else
		filelength=TAP_Hdd_Flen( fp );
		if (filelength!=0 && (filelength%512)==0) filelength+=512;  //FRIGGIN TOPFIELD BUG! flen is 512bytes out if the filesize is a multiple of 512.
		TAP_MemInfo(&heapSize, &freeHeapSize, &availHeapSize);
		if (availHeapSize<filelength-0x2b-0x31)
		{
			ShowMessageWin("Not enough ram to open and process", filename ,150,TRUE);
			TAP_Hdd_Fclose( fp );
			*size=(long)NULL;
			return NULL;
		}
		
		TAP_Hdd_Fseek(fp, 0x10, SEEK_SET);
		TAP_Hdd_Fread( &fmtchunksize, 1, 1, fp );
		
		TAP_Hdd_Fseek(fp, 0x3, SEEK_CUR);
		TAP_Hdd_Fread( &wFormatTag, 1, 1, fp );
		
		TAP_Hdd_Fseek(fp, 1, SEEK_CUR);
		TAP_Hdd_Fread( &wChannels, 1, 1, fp );
		
		TAP_Hdd_Fseek(fp, 1, SEEK_CUR);
		TAP_Hdd_Fread( &wtemp, 2, 1, fp );
		
		dwSamplesPerSec=((wtemp>>8)&0xff)|((wtemp&0xff)<<8);
		
		*realrate=dwSamplesPerSec;
		
		if (dwSamplesPerSec==16000)
			*rate=FREQ_16K;
		else if (dwSamplesPerSec==22050)
			*rate=FREQ_22K05;
		else if (dwSamplesPerSec==24000)
			*rate=FREQ_24K;
		else if (dwSamplesPerSec==32000)
			*rate=FREQ_32K;
		else if (dwSamplesPerSec==44100)
			*rate=FREQ_44K1;
		else if (dwSamplesPerSec==48000)
			*rate=FREQ_48K;
		else 
			{
				*rate=FREQ_16K;
				*realrate=16000;
			}
		
		
		TAP_Hdd_Fseek(fp,0x8,SEEK_CUR); 
		TAP_Hdd_Fread( &wBitsPerSample, 1, 1, fp );
		
		TAP_Hdd_Fseek(fp,fmtchunksize+0x14,SEEK_SET); 
		TAP_Hdd_Fread( &dwtemp, 4, 1, fp );
		if (dwtemp==0x66616374) //="fact" ascii
			TAP_Hdd_Fseek(fp, 0xc, SEEK_CUR); //silly windows recorder adds "fact" section even though uncompressed.
		//else //must be data section so at the correct position
			
		//TAP_Hdd_Fseek(fp,fmtchunksize+0x14+4,SEEK_SET); //move to indicated end of fmt, past data flag to beginning of data
		for (n=0;n<4;n++)
		{
			TAP_Hdd_Fread( &bytetemp, 1, 1, fp );
			datachunksize=datachunksize|((dword)bytetemp<<(8*n));
		}
		logf(4,"Audio format %d, channels %d, samples/sec %d, datachunksize %d, bits/sample %d",wFormatTag,wChannels,dwSamplesPerSec,datachunksize,wBitsPerSample);
		if (wFormatTag!=1 || wChannels!=2 ||wBitsPerSample!=16)
		{
			ShowMessageWin("WAV file not compat with Toppy PCM playback", "require >=16K rate, 2 channels, 16bit",200,TRUE);
			*size=0;
			TAP_Hdd_Fclose( fp );
			return NULL;
		}
		
		*size=datachunksize;
		ramspot=TAP_MemAlloc(datachunksize);
		TAP_Hdd_Fread(ramspot, 1, datachunksize, fp );
		TAP_Hdd_Fclose( fp );
		
		for (m=0;m<datachunksize;m+=2)
		{
			bytetemp= *(byte *)(ramspot+m);  //have to do this cludge to turn ass about intel format to normal PCM 16bit.
			*(byte *)(ramspot+m)=*(byte *)(ramspot+m+1);
			*(byte *)(ramspot+m+1)=bytetemp;
		}
		return ramspot;
}

void ShowMessageWin (unsigned char* lpMessage, unsigned char* lpMessage1, int time, bool logerror)
{
	bool createreg=FALSE;							// stores rgn-handle for osd
	dword w;							// stores width of message-text

	if (!_gFullRegion)
	{
		TAP_ExitNormal();
		createreg=TRUE;
		_gFullRegion = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );		// create rgn-handle
	}
	//g_deleteregion=TAP_GetTick();
	w = TAP_Osd_GetW( lpMessage, 0, FNT_Size_1926 ) + 10;		// calculate width of message
	if (TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10>w)
	{
		w = TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10;	// calculate width of message
	}
	if (w > 700) w = 700;						// if message is to long
	TAP_Osd_FillBox(_gFullRegion, (720-w)/2-5, 265, w+10, 62, RGB(19,19,19) );	// draw background-box for border
	TAP_Osd_PutS(_gFullRegion, (720-w)/2, 270, (720+w)/2, lpMessage,		// show message
		RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_CENTER);
	TAP_Osd_PutS(_gFullRegion, (720-w)/2, 270+26, (720+w)/2, lpMessage1,		// show message
		RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926,FALSE, ALIGN_CENTER);
	if (g_binifiledirectoryknown && logerror) //if inifile directory loaded and asked to log-
		logf(2,"%s,%s",lpMessage,lpMessage1);
	normalTAP_delay(time);							// show it for whatever tenths of seconds
	if (createreg)
	{
		TAP_Osd_Delete(_gFullRegion);					// release rgn-handle
		_gFullRegion=0;
		TAP_EnterNormal();
	}
}

int convert_parental(const unsigned char *buf)
{
	//logf(4,"parental:%s\n",buf);
	//rating, where rating=min_age-3   I found these ages take2 (after a few reports back): 15=MA,13=M,11=PG,9=G,6=P?,18=R
	if (buf[0]=='G') return 6;
	if (buf[0]=='M' && buf[1]=='A') return 12;
	if (buf[0]=='R') return 15;
	if (buf[0]=='P' && buf[1]=='G') return 8;
	if (buf[0]=='P') return 2;
	if (buf[0]=='M') return 10;
	if (buf[0]=='C') return 4;  //Lets try a "C" rating.
	//else
	return 0;
}


/*void InterpretEPGbuff2( EPG_INFO *epg, const unsigned char *buf )
{
unsigned char *tmp_field;//[2048];
int  field_no, i;
unsigned char field_start[]="tony test\twas\there\t\n";
unsigned char	tmp_date[5];

logf(4,"tmp_fieldb4malloc %d",tmp_field);
tmp_field= TAP_MemAlloc(strlen(field_start)+1);
logf(4,"malloc worked %d",tmp_field);
tmp_field = strtok (field_start,"\t");
logf(4,"strtok worked");
while (tmp_field != NULL)
{
logf(4,"%s",tmp_field);
tmp_field = strtok (NULL,"\t");
}
//TAP_MemFree(field_start);
}
*/

int subtracttimes(dword todaydatetime1, dword todaydatetime2)
{
	int day1,hour1,min1,day2,hour2,min2;

	day1=(todaydatetime1>>16)&0xffff;
	hour1=(todaydatetime1>>8)&0xff;
	min1=(todaydatetime1)&0xff;

	day2=(todaydatetime2>>16)&0xffff;
	hour2=(todaydatetime2>>8)&0xff;
	min2=(todaydatetime2&0x0ff);

	return (day1-day2)*24*60+(hour1-hour2)*60 +(min1-min2);
}

void InterpretEPGbuff( EPG_INFO *epg, const unsigned char *buf )
{
	unsigned char tmp_field[5000];
	//unsigned char tmp[256];
	int  field_no, i;
	unsigned char *field_start;
	unsigned char	tmp_date[5];
	int len;

	//TAP_Print("InterpretEPGbuff: buf(%s)", buf);
	field_no = 0;
	field_start = (unsigned char *)buf;

	memset(tmp_field, 0x0, 1);
	//memset(tmp,0x0,256);

	for(i=0; i<strlen(buf); i++)
	{
		if ( *(buf+i) == '\t' )
		{
			strncat( tmp_field, field_start, ( (buf+i) - field_start ));
			field_no++;
			//logf(4,"field_no%02d %s", field_no, strncpy(tmp,tmp_field,115));
			//memset(tmp,0x0,1);
			switch ( field_no )
			{
			case 1: // Service details
				//put_chanServiceDetails_TGD(tmp_field,epg);  // store name from file
				if ((len=strlen(tmp_field))>255)
					len=255;
				strncat( epg->service_name, tmp_field, len);
				break;
			case 2: // Date
				memset( tmp_date, 0x0, 5 );
				//strncpy( tmp_date, tmp_field, 4 );
				epg->year = atoi( strncpy( tmp_date, tmp_field, 4 ) );

				memset( tmp_date, 0x0, 3 );
				//strncpy( tmp_date, tmp_field+5, 2 );
				epg->month = atoi( strncpy( tmp_date, tmp_field+5, 2 ) );

				//memset( tmp_date, 0x0, 5 );
				//strncpy( tmp_date, tmp_field+8, 2 );
				epg->day = atoi( strncpy( tmp_date, tmp_field+8, 2 ) );

				//memset( tmp_date, 0x0, 5 );
				//strncpy( tmp_date, tmp_field+11, 2 );
				epg->hour = atoi( strncpy( tmp_date, tmp_field+11, 2 ) );

				//memset( tmp_date, 0x0, 5 );
				//strncpy( tmp_date, tmp_field+14, 2 );
				epg->min = atoi( strncpy( tmp_date, tmp_field+14, 2 ) );
				
				if (strlen(tmp_field)>16) //+HHMM format
				{
					memset( tmp_date, 0x0, 4 );
					epg->offsetminutes=atoi(strncpy(tmp_date, tmp_field+19, 2)) + atoi(strncpy( tmp_date, tmp_field+16, 3 ))*60; //MM+HH*60
				}
				else
					epg->offsetminutes=1440; //use 1440 to indicate "no offset indicated".
				break;
			case 3: // Duration
				epg->duration = atoi(tmp_field);
				break;

			case 4: // Title
				if ((len=strlen(tmp_field))>254)
					len=254;
				strncat( epg->title, tmp_field, len);
				break;
			case 5: //short event desc
				if ((len=strlen(tmp_field))>254)
					len=254;
				strncat( epg->shortevt, tmp_field, len);
				break;

			case 6: // Description
				if ((len=strlen(tmp_field))>3984)
					len=3984;
				strncat( epg->description,tmp_field, len);
				break;
			case 7: // Parental
				epg->parental= convert_parental(tmp_field);
				break;

			case 8: // Recording Status
				strncat(epg->recordingStatus, tmp_field, strlen(tmp_field));
				if (tmp_field[0]=='D') delete_timer_evt((EPG_INFO *)epg);//delete epg
				else
					if (tmp_field[0]=='Y'||tmp_field[0]=='+'||tmp_field[0]=='-' || tmp_field[0]=='W'|| tmp_field[0]=='E') record_timer_evt((EPG_INFO *)epg); //set recording for item
				break;

			case 9: 
				epg->eventid = atoi(tmp_field);
				break;
			default:  // Any more fields will currently not do anything.
				break;
			}
			memset(tmp_field, 0x0, 1);
			field_start = (unsigned char *)(buf + i + 1);
		}
	}
}


int record_timer_evt(EPG_INFO *epg)
{
	TYPE_TimerInfo timerinfo, toppytimers;
//	TYPE_TapChInfo chInfo;

	//TYPE_TimerRAM tempbuff;
	TYPE_RecInfo recInfo;

//	int tvSvc,radSvc;
	int retval;
	dword startTime;
	word day;
	unsigned char tmp[256];
	int durationadd=0;
	int startadd=0;
	int i,j;
	word todaydate;
	byte nowhour,nowmin,dummy;
	word year;
	byte month, day2, weekDay;
	int temp=0,temp2;

	TYPE_TapChInfo chInfo;

		
	//bool bailsearch=FALSE;


	// "N" do not record, the default value
	// "-10 +15" - start 10 minutes early and go 15 minutes extra
	// "Y" - record as specified by EPG entry.
	// "D" - delete record


	logf(1,"Setting recording: \"%s\"",epg->title);
	timerinfo.svcNum=0;//start with 0 as it may not get filled 

	for (i=0;i<g_NumServices;i++)
	{
		temp=put_chanServiceDetails_TGD(epg,temp);
		if (epg->topfieldinternalchannel!=9999 || temp==0) break;  //if epg details for *epg is found
	}
	
	if (temp==0 || (timerinfo.svcNum= epg->topfieldinternalchannel)==9999)
	{
			logf(3,"ERROR:Timer \"%s\" not created due to Topfield channel number not found, setup ini file correctly. temp:%d,svcnum:%d",epg->title,temp,timerinfo.svcNum);
			gtimer_error_counter++;
			return 0;
	}
	TAP_Channel_GetInfo( g_svcType,  timerinfo.svcNum, &chInfo );
	logf(3,"Toppy Internal Channel Number: %d LCN: %d",timerinfo.svcNum,chInfo.logicalChNum);  //
			
	day=TAP_MakeMjd((word)epg->year,(byte)epg->month,(byte)epg->day);
    timerinfo.reservationType=RESERVE_TYPE_Onetime; //set to one time, unless below is set to different
    if (epg->recordingStatus[0]=='E' && epg->recordingStatus[1]=='D')//everyday
    {
        timerinfo.reservationType=RESERVE_TYPE_Everyday;
    }
    else if (epg->recordingStatus[0]=='W' && epg->recordingStatus[1]=='D')//weekday
    {
        timerinfo.reservationType=RESERVE_TYPE_WeekDay;
    }
    else if (epg->recordingStatus[0]=='W' && epg->recordingStatus[1]=='E')//weekend
    {
        timerinfo.reservationType=RESERVE_TYPE_EveryWeekend;
    }
    else if (epg->recordingStatus[0]=='W' && epg->recordingStatus[1]=='L')//weekly
    {
        timerinfo.reservationType=RESERVE_TYPE_Weekly;
    }
    else if (epg->recordingStatus[0]!='Y')
	{
		startadd=(epg->recordingStatus[1]-'0')*10+(epg->recordingStatus[2]-'0');
		if (epg->recordingStatus[0]=='-')
			startadd=-startadd;
		durationadd=(epg->recordingStatus[5]-'0')*10+(epg->recordingStatus[6]-'0');
		if (epg->recordingStatus[4]=='-')
			durationadd=-durationadd;

		durationadd=durationadd-startadd;//included extra minutes on the duration for the start.
        
	}

	startTime=addtime((day<<16)|((epg->hour)<<8)|(epg->min),startadd);

	timerinfo.duration=epg->duration+durationadd;

	TAP_GetTime(&todaydate, &nowhour, &nowmin, &dummy);
	memset(tmp,0x0,128);

	if ( subtracttimes( (todaydate<<16)|(nowhour<<8)|nowmin , addtime(startTime,timerinfo.duration) ) >0 ) //we are trying to set a one time timer that has well and truely passed, past the prog duration.
	{
		strncpy(tmp,epg->title,59);
		logf(1,"ERROR:Timer FAILED: Trying to set a timer for prog already past, not creating \"%s\" at %02d:%02d",tmp,(startTime>>8)&0xff,startTime&0xff);
		//bailsearch=TRUE;
		gtimer_error_counter++;
		return 0;
	}
	//scan for exact match, if so, bail.
	for (i=0;i<TAP_Timer_GetTotalNum();i++)
	{
		TAP_Timer_GetInfo(i,&toppytimers);
		if ( (toppytimers.startTime==startTime) && (toppytimers.svcNum==timerinfo.svcNum))
		{
			logf(1,"ERROR:Timer FAILED: Found timer with same channel&time, timername:\"%s\"",strncpy(tmp,toppytimers.fileName,64));
			gtimer_error_counter++;
			return 0;
		}
	}
	if ( (temp2=subtracttimes((todaydate<<16)|(nowhour<<8)|nowmin,startTime)) >=0)  //temp2 is number of minutes that now is greater than the intended starttime
	{
		for (j=0;j<=1;j++)
		{
			if (TAP_Hdd_GetRecInfo( j, &recInfo ))
			{
				if (recInfo.recType==RECTYPE_Normal && recInfo.svcNum==timerinfo.svcNum)
				{
					logf(1,"Recording on this channel in progress, not setting timer");
					gtimer_error_counter++;
					return 0;
				}
				else
					printf("SvcNum not same, timer ok to set\n");
			}
		}
		timerinfo.startTime=addtime((todaydate<<16)|(nowhour<<8)|nowmin,1);
		if ((timerinfo.duration-temp2-1) <= 0)
			timerinfo.duration=0;
		else timerinfo.duration=timerinfo.duration-temp2-1;
	}
	else
		timerinfo.startTime=startTime;

	strncpy(tmp,epg->title,59);
	strcpy(timerinfo.fileName,strcat(tmp,".rec"));

	if (timerinfo.duration>1440 || timerinfo.duration==0) 
	{
		logf(1,"ERROR:Timer duration %dmins, could be a corrupt TGD file, bailing timer create routine",timerinfo.duration);
		gtimer_error_counter++;
		return 0;
	}
	timerinfo.isRec=1;    //recording rather than reservation/wakeup
	timerinfo.svcType=0;  //tv

	
	timerinfo.nameFix=1; //  
	timerinfo.tuner=3;   //the "catch all" tuner.

	TAP_ExtractMjd(day, &year, &month, &day2, &weekDay );

	if (g_btimer_create_delay) {
		rotateTAP_delay(g_timerdelay);
	}
	//printf("Before timer add\n");
	retval=TAP_Timer_Add(&timerinfo);
	//printf("After timer add\n");
	memset(tmp,0x0,128);
	strncpy(tmp,timerinfo.fileName,40);
	//printf("timerinfo filename %s\n",tmp);
	if (retval!=0)
	{
		//printf("retval is a number\n");
		gtimer_error_counter++;
		show_timer_error(retval,999);
		logf(1,"ERROR:Timer FAILED:\"%s\" at %04d/%02d/%02d %02d:%02d for %dmins IntChannel:%d LCN:%d", tmp, year,month,day2,(timerinfo.startTime>>8)&0xff,(timerinfo.startTime)&0xff,timerinfo.duration,timerinfo.svcNum,chInfo.logicalChNum);
	}
	else {
		//printf("retval not true\n");
		gtimer_succ_counter++;
		logf(1,"Timer SUCCESSFUL:\"%s\" at %04d/%02d/%02d %02d:%02d for %dmins type:%d IntChannel:%d LCN:%d", tmp, year,month,day2,(timerinfo.startTime>>8)&0xff,(timerinfo.startTime)&0xff,timerinfo.duration, timerinfo.reservationType,timerinfo.svcNum,chInfo.logicalChNum);
		g_btimer_create_delay=TRUE;
	}
	return 1;
}//record_timer_evt(EPG_INFO *epg)



int delete_timer_evt(EPG_INFO *epg)
{
	TYPE_TimerInfo timerinfo, toppytimers;
	dword startTime;
	unsigned char tmp[128];
	int i;
	int temp=0;
	bool timerfound=FALSE;
	TYPE_TapChInfo chInfo;

	logf(1,"Deleting recording: \"%s\"",epg->title);
	timerinfo.svcNum=0;//start with 0 as it may not get filled

	for (i=0;i<g_NumServices;i++)
	{
		temp=put_chanServiceDetails_TGD(epg,temp);
		if (epg->topfieldinternalchannel!=9999 || temp==0) break;  //if epg details for *epg is found
	}
	//if (temp==0 || (timerinfo.svcNum= g_IniSvcPtr[temp-1].topfieldinternalchannel)==9999)
	if (temp==0 || (timerinfo.svcNum= epg->topfieldinternalchannel)==9999)
	{
		logf(3,"ERROR:Timer \"%s\" not deleted due to Topfield channel number not found, setup ini file correctly. temp:%d,svcnum:%d",epg->title,temp,timerinfo.svcNum);
		gtimer_error_counter++;
		return 0;
	}

	startTime=(TAP_MakeMjd((word)epg->year,(byte)epg->month,(byte)epg->day)<<16)|((epg->hour)<<8)|(epg->min);
	TAP_Channel_GetInfo( g_svcType,  timerinfo.svcNum, &chInfo );
	logf(3,"Toppy Internal Channel Number: %d LCN:%d",timerinfo.svcNum,chInfo.logicalChNum);

	//scan for exact match
	for (i=0;i<TAP_Timer_GetTotalNum();i++)
	{
		TAP_Timer_GetInfo(i,&toppytimers);
		if ( (toppytimers.startTime==startTime) && (toppytimers.svcNum==timerinfo.svcNum))
		{
			if (TAP_Timer_Delete(i))
			{
				logf(1,"Timer Delete OK: timername:\"%s\" svcNum:%d LCN:%d",strncpy(tmp,toppytimers.fileName,64),timerinfo.svcNum,chInfo.logicalChNum);
				timerfound=TRUE;
				g_btimer_create_delay=TRUE;
				gtimer_del_counter++;

				ChangeDir("ProgramFiles");
				if (!TAP_Hdd_Exist(TIMER_DELETE_FILE)) {
					TAP_Hdd_Create(TIMER_DELETE_FILE,ATTR_NORMAL);
				}
				ChangeDir(gTGDdirectory);
				break;
			}
			else
			{
				logf(1,"ERROR:Timer Delete Failed-API: timername:\"%s\" svcNum:%d LCN:%d",strncpy(tmp,epg->title,64),timerinfo.svcNum,chInfo.logicalChNum);
				gtimer_error_counter++;
				return 0;
			}
		}
	}
	if (!timerfound)  
	{
		logf(1,"ERROR:Timer to delete not found??: timername:\"%s\" svcNum:%d LCN:%d",strncpy(tmp,epg->title,64),timerinfo.svcNum,chInfo.logicalChNum);
		gtimer_error_counter++;
		return 0;
	}

	if (g_btimer_create_delay) 
	{
		rotateTAP_delay(g_timerdelay);
	}
	return 1;
}//delete_timer_evt


void rotateTAP_delay(int delay)
{
	int i;
	int OFFSX=55,OFFSY=40;
	static int loadingstr=0;
//	unsigned char loadingstrchr[2];
	if (g_ShowLoadingOSD!=DISOFF)
	{
			if (_rotateUploadedRegion==0) _rotateUploadedRegion = TAP_Osd_Create( CBOX_X, CBOX_Y, CBOX_W, CBOX_H, 0, 0 );
			OFFSX=0;
			OFFSY=0;
	}
	gbSystemProc=TRUE;
	for (i=0;i<=delay;i+=30)
	{
		if (g_ShowLoadingOSD!=DISOFF)
		{
			switch ((++loadingstr)%3){
				case 0:
					TAP_Osd_Copy( _cogIconRAM1, _rotateUploadedRegion, 0, 0, CBOX_W, CBOX_H, 0, 0, TRUE ); //TAP_Osd_Copy( word srcRgnNum, word dstRgnNum, dword srcX, dword srcY, dword w, dword h, dword dstX, dword dstY, bool sprite)
					break;
				case 1:
					TAP_Osd_Copy( _cogIconRAM2, _rotateUploadedRegion, 0, 0, CBOX_W, CBOX_H, 0, 0, TRUE ); //TAP_Osd_Copy( word srcRgnNum, word dstRgnNum, dword srcX, dword srcY, dword w, dword h, dword dstX, dword dstY, bool sprite)
					break;
				case 2:
					TAP_Osd_Copy( _cogIconRAM3, _rotateUploadedRegion, 0, 0, CBOX_W, CBOX_H, 0, 0, TRUE ); //TAP_Osd_Copy( word srcRgnNum, word dstRgnNum, dword srcX, dword srcY, dword w, dword h, dword dstX, dword dstY, bool sprite)
					break;
			}
		}
		TAP_Delay(30);
		TAP_SystemProc();
	}
	gbSystemProc=FALSE;
}


void normalTAP_delay(int delay)
{
	int i;
	gbSystemProc=TRUE;
	TAP_SystemProc();
	for (i=0;i<delay;i+=20)
	{
		TAP_Delay(20);
		TAP_SystemProc();
	}
	gbSystemProc=FALSE;
}



void showTimersUploaded(void)
{
	unsigned char str[256];
	int OFFSX=75;
	int OFFSY=40;

	g_btimer_create_delay=FALSE; //set flag back to false so future timer uploading can occur

	if (g_ShowLoadingOSD!=DISOFF)// && !(g_bLoadSettings && !g_deleteregion && OsdActive())) 
	{
		if (!_gFullRegion)
		{
			//TAP_ExitNormal();
			_timersUploadedRegion = TAP_Osd_Create( OFFSX, OFFSY, 420, 100, 0, FALSE );		// create rgn-handle
			OFFSX=0;
			OFFSY=0;
		}
		else
			_timersUploadedRegion=_gFullRegion;

		g_deleteregion=TAP_GetTick();
		if (g_ShowLoadingOSD==SMALL)
		{
			sprintf(str,"C%d F%d D%d", gtimer_succ_counter, gtimer_error_counter, gtimer_del_counter); 
			TAP_Osd_FillBox(_timersUploadedRegion, OFFSX, OFFSY, TAP_Osd_GetW( str, 0, FNT_Size_1926 )+8, 32, RGB(19,19,19) );	// draw background-box for border x,y,w,h
			TAP_Osd_PutS(_timersUploadedRegion, OFFSX+3, OFFSY+4, TAP_Osd_GetW( str, 0, FNT_Size_1926 )+OFFSX+8-3,	str,	// show message x,y,maxX
				RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_RIGHT);
		}
		else
		{
			if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
				sprintf(str,"Timer OK:%d Fehlgeschlagen:%d Del:%d", gtimer_succ_counter, gtimer_error_counter, gtimer_del_counter); //one last print.
			else
				sprintf(str," Timers OK:%d FAILED:%d Del:%d", gtimer_succ_counter, gtimer_error_counter, gtimer_del_counter); //one last print.

			TAP_Osd_FillBox(_timersUploadedRegion, OFFSX, OFFSY+33, 400, 32, RGB(19,19,19) );	// draw background-box for border x,y,w,h
			TAP_Osd_PutS(_timersUploadedRegion, OFFSX+5, OFFSY+38, 394+OFFSX, str,		// show message x,y,maxX
				RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_LEFT);
		}
	}
	if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
		logf(1,"Timer geladen OK:%d Fehlgeschlagen:%d Deleted:%d", gtimer_succ_counter, gtimer_error_counter, gtimer_del_counter);
	else
		logf(1,"Timers Loaded OK:%d FAILED:%d Deleted:%d", gtimer_succ_counter, gtimer_error_counter, gtimer_del_counter);

	createtimerTSVfile();
	gtimer_error_counter=0;
	gtimer_succ_counter=0;
	gtimer_del_counter=0;
} //showTimersUploaded


void createtimerTSVfile(void)
{
	TYPE_File*	fp;
	TYPE_TimerInfo timerinfo;
	unsigned char sprintfile[10240];
	unsigned char sBuff[256];
	word year;
	byte month,day, weekDay;
//	int i,retval;
	int i;
	//bool changeddir=FALSE;
	TYPE_TapChInfo chInfo;
//	int temp;

	if (!TAP_Hdd_Exist(TIMER_FILE)) {
		TAP_Hdd_Create(TIMER_FILE,ATTR_NORMAL);
	}
	
	if (( fp = TAP_Hdd_Fopen( TIMER_FILE) ))
	{
		memset(sprintfile,0,sizeof sprintfile);
		sprintf(sBuff,"fileName\tchanName\tchanNum\tstartTime\tduration\treservationType\tsvcType\ttuner\tnameFix\tisRec\treserved\tlcn\r\n"); 

		for (i=0;i<TAP_Timer_GetTotalNum();++i)
		{
			TAP_Timer_GetInfo(i,&timerinfo);
			TAP_ExtractMjd((timerinfo.startTime>>16), &year, &month, &day, &weekDay );
			strcat(sprintfile,sBuff);
			TAP_Channel_GetInfo( g_svcType,  timerinfo.svcNum, &chInfo );
			sprintf(sBuff,"%s\t%s\t%d\t%04d%02d%02d %02d:%02d\t%d\t%s\t%s\t%d\t%d\t%s\t%d\t%d\r\n",
				timerinfo.fileName,chInfo.chName,timerinfo.svcNum,year,month,day, (timerinfo.startTime>>8)&0xff,timerinfo.startTime&0xff,
				timerinfo.duration,lookupReservtype(timerinfo.reservationType),
				(timerinfo.svcType)?"Radio":"TV",timerinfo.tuner,	timerinfo.nameFix,(timerinfo.isRec)?"Recording":"VCR Timer",
				timerinfo.reserved, chInfo.logicalChNum);
		}
		strcat(sprintfile,sBuff);

		TAP_Hdd_Fwrite( sprintfile, sizeof( sprintfile ), 1, fp);
		TAP_Hdd_Fclose( fp );
	}
	else
	{
		//logf("can't open "INI_FILE);
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			ShowMessageWin("ERROR:kann nicht geöffnet werden "TIMER_FILE, "",150,TRUE);
		else
			ShowMessageWin("ERROR:can't open "TIMER_FILE, "",150,TRUE);
	}
}

byte createRecPlaybackFile(byte timeshift)
{
	TYPE_File*	fp;
	TYPE_File*	recfp;
	unsigned char * recbuffer;
	//TYPE_TimerInfo timerinfo;
	static char sprintfile[2048];
	unsigned char sBuff[256];
	static TYPE_RecInfo CurrentRecInfo0;
	static TYPE_RecInfo CurrentRecInfo1;
	static TYPE_PlayInfo CurrentPlaybackInfo; 
	static char sOverRun[1024]; //TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo); has a buffer overrun issue
	static byte gRec0Mode=99,gRec1Mode=99,gPlayMode=99;
	static word gduration0=0,gduration1=0,gplayduration=0,grecsvcNum0=9999,grecsvcNum1=9999,gplaysvcNum=9999;
	word year;
	byte month,day, weekDay;
//	int i,retval;
	//bool changeddir=FALSE;
	TYPE_TapChInfo chInfo;
//	int temp;
	int recfilelength=512;
	
	bool updateFile=FALSE;
	bool updateRecFile0=FALSE;
	bool updateRecFile1=FALSE;
	byte timeshiftreturn=timeshift;  //so if 1, i.e. timeshift on always, then will return 1, but if 0, will return 1 if HDD playback or recording

	//if (checkplaystatusonly)
	//{
	
		if (TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo))
		{
			if (gPlayMode!=CurrentPlaybackInfo.playMode || gplayduration!=CurrentPlaybackInfo.duration ||gplaysvcNum!=CurrentPlaybackInfo.svcNum)
			{
				updateFile=TRUE;
				gPlayMode=CurrentPlaybackInfo.playMode;
				gplayduration=CurrentPlaybackInfo.duration;
				gplaysvcNum=CurrentPlaybackInfo.svcNum;
	
			}
			if (CurrentPlaybackInfo.playMode!=PLAYMODE_None)
			{
#if DEBUG
				logf(4,"NFFC Playback happening playmode:%d",CurrentPlaybackInfo.playMode);
#endif		
				timeshiftreturn=timeshiftreturn|2;
			}
			else
				timeshiftreturn=timeshiftreturn&~2;
		}
		if (TAP_Hdd_GetRecInfo (0,&CurrentRecInfo0))
		{
			if (CurrentRecInfo0.recType==RECTYPE_Timeshift)
				CurrentRecInfo0.recType=RECTYPE_None;//work around to ditch the silly mode it gets when pausing
			if (gRec0Mode!=CurrentRecInfo0.recType || gduration0!=CurrentRecInfo0.duration || grecsvcNum0!=CurrentRecInfo0.svcNum)
			{
				updateFile=TRUE;
				if (CurrentRecInfo0.recType==RECTYPE_Normal && gTedRecordingsfile)
					updateRecFile0=TRUE;
				gRec0Mode=CurrentRecInfo0.recType;
				gduration0=CurrentRecInfo0.duration;
				grecsvcNum0=CurrentRecInfo0.svcNum;
			}
			if (CurrentRecInfo0.recType!=RECTYPE_None) 
//have to exclude pause-timeshift (but doesn't exclude rec-timeshift)
//unfort too, as if pause is hit, even for a second, this mode gets stuck until channel is changed
			{
#if DEBUG	
				logf(4,"NFFC rec in slot 0, rectype:%d",CurrentRecInfo0.recType);
#endif	
				timeshiftreturn=timeshiftreturn|4;
			}
			else 
				timeshiftreturn=timeshiftreturn&~4;
		}	
		if (TAP_Hdd_GetRecInfo (1,&CurrentRecInfo1))
		{
			if (CurrentRecInfo1.recType==RECTYPE_Timeshift)
				CurrentRecInfo1.recType=RECTYPE_None;//work around to ditch the silly mode it gets when pausing
			if (gRec1Mode!=CurrentRecInfo1.recType || gduration1!=CurrentRecInfo1.duration || grecsvcNum1!=CurrentRecInfo1.svcNum )
			{
				updateFile=TRUE;
				if (CurrentRecInfo1.recType==RECTYPE_Normal && gTedRecordingsfile)
					updateRecFile1=TRUE;
				gRec1Mode=CurrentRecInfo1.recType;
				gduration1=CurrentRecInfo1.duration;
				grecsvcNum1=CurrentRecInfo1.svcNum;
			}
			if (CurrentRecInfo1.recType!=RECTYPE_None)
			{
#if DEBUG
				logf(4,"NFFC rec in slot 1 rectype:%d",CurrentRecInfo1.recType);
#endif						
				timeshiftreturn=timeshiftreturn|8;
			}
			else 
				timeshiftreturn=timeshiftreturn&~8;
		}
	//}
	
	if (!updateFile)  //if we don't have to update the file, bail at this point
		return timeshiftreturn;
		
#if DEBUG	
				logf(4,"rec-slot0rectype:%d slot1rectype:%d playbacktype:%d",CurrentRecInfo0.recType,CurrentRecInfo1.recType,CurrentPlaybackInfo.playMode);
#endif	
	if (updateRecFile0||updateRecFile1)
		logf(4,"Updating "RECPLAYBACKFILE" and "RECORDINGS_FILE" file");
	else	
		logf(4,"Updating "RECPLAYBACKFILE" file");
	
		if (!TAP_Hdd_Exist(RECPLAYBACKFILE)) {
		TAP_Hdd_Create(RECPLAYBACKFILE,ATTR_NORMAL);
	}
	
	if (updateRecFile0||updateRecFile1)
	{	
		if (!TAP_Hdd_Exist(RECORDINGS_FILE))
		{
			TAP_Hdd_Create(RECORDINGS_FILE,ATTR_NORMAL);
			recfilelength=0;
		}
		
		if (( recfp = TAP_Hdd_Fopen( RECORDINGS_FILE) ))
		{
			if (recfilelength) //if not 0...
			{
				recfilelength=TAP_Hdd_Flen( recfp );
				if (!(recfilelength%512)) recfilelength+=512;  //FRIGGIN TOPFIELD BUG! flen is 512bytes out if the filesize is a multiple of 512.
			}
			if ((recbuffer=(unsigned char *)TAP_MemAlloc(recfilelength+512))==NULL)
			{
				logf(1,"ERROR:Not enough memory to load \"%s\"",RECORDINGS_FILE);
				TAP_Hdd_Fclose (recfp);
			}
			else
			{
				memset(recbuffer,0,recfilelength+512);
				TAP_Hdd_Fread (recbuffer, 1, recfilelength, recfp);
	            //TAP_Hdd_Fclose (fp);
			}
		}
	}
#if DEBUG	
				printf("gothere");
#endif		
	
	if (( fp = TAP_Hdd_Fopen( RECPLAYBACKFILE) ))
	{
		memset(sprintfile,0,sizeof sprintfile);
#if DEBUG	
				printf("gothere2");
#endif	
		//sprintf(sBuff,"fileName\tchanName\tchanNum\tstartTime\tduration\treservationType\tsvcType\ttuner\tnameFix\tisRec\treserved\r\n"); 

		if (CurrentRecInfo0.recType!=RECTYPE_None && CurrentRecInfo0.recType!=RECTYPE_Timeshift)
		{
			TAP_ExtractMjd((CurrentRecInfo0.startTime>>16), &year, &month, &day, &weekDay );
			TAP_Channel_GetInfo( CurrentRecInfo0.svcType,  CurrentRecInfo0.svcNum, &chInfo );
			sprintf(sBuff,"REC1\t%s\t%s\t%04d%02d%02d %02d:%02d\t%d\t%d\r\n",
				CurrentRecInfo0.fileName,chInfo.chName,year,month,day, (CurrentRecInfo0.startTime>>8)&0xff,CurrentRecInfo0.startTime&0xff,
				CurrentRecInfo0.duration,chInfo.logicalChNum);
			if (updateRecFile0 && recbuffer)
			{
					logf(3,"Recording 1: %s duration:%d on %02dLCN",CurrentRecInfo0.fileName,CurrentRecInfo0.duration,chInfo.logicalChNum);
					strcat(recbuffer,sBuff); //update recordings file	
			}		
		}
		else
			sprintf(sBuff,"REC1\t\t\t\t\r\n");
			
		strcat(sprintfile,sBuff);
		
		if (CurrentRecInfo1.recType!=RECTYPE_None && CurrentRecInfo1.recType!=RECTYPE_Timeshift)
		{
			TAP_ExtractMjd((CurrentRecInfo1.startTime>>16), &year, &month, &day, &weekDay );
			TAP_Channel_GetInfo( CurrentRecInfo1.svcType,  CurrentRecInfo1.svcNum, &chInfo );
								
			sprintf(sBuff,"REC2\t%s\t%s\t%04d%02d%02d %02d:%02d\t%d\t%d\r\n",
				CurrentRecInfo1.fileName,chInfo.chName,year,month,day, (CurrentRecInfo1.startTime>>8)&0xff,CurrentRecInfo1.startTime&0xff,
				CurrentRecInfo1.duration,chInfo.logicalChNum);
				
			if (updateRecFile1 && recbuffer)
			{
				logf(3,"Recording 2: %s duration:%d on %02dLCN",CurrentRecInfo1.fileName,CurrentRecInfo1.duration,chInfo.logicalChNum);
				strcat(recbuffer,sBuff); //update recordings file	
			}
		}
		else
			sprintf(sBuff,"REC2\t\t\t\t\r\n");
			
		strcat(sprintfile,sBuff);
		
#if DEBUG	
				printf("gothere3");
#endif			
	
		if (CurrentPlaybackInfo.playMode!=PLAYMODE_None)
		{
			//TAP_Hdd_GetPlayInfo (&CurrentPlaybackInfo); //resetting variable again as get exception when cutting files sometimes, when playmode=3 momentarily during cut or save.
			memset(sBuff,0,sizeof sBuff);
#if DEBUG	
				printf("gothere4");
#endif	
			sprintf(sBuff,"PLAY\t");
			if (CurrentPlaybackInfo.file) 
			{
				strncat(sBuff, CurrentPlaybackInfo.file->name, TS_FILE_NAME_SIZE);
				logf(3,"Playback %s",sBuff);
			}
			else 
				strcat(sBuff, "No file name available");
#if DEBUG	
				printf("gothere5");
#endif				
			strcat(sBuff,"\r\n");
		}
		else
			sprintf(sBuff,"PLAY\t\r\n");

#if DEBUG	
				printf("gothere6");
#endif	
			
		strcat(sprintfile,sBuff);
		TAP_Hdd_Fwrite( sprintfile, sizeof( sprintfile ), 1, fp);
		TAP_Hdd_Fclose( fp );
		if ((updateRecFile0||updateRecFile1) && recbuffer)
		{
			int temp=strlen(recbuffer);
#if DEBUG
			printf("strlen of recbuffer: %d",temp);
#endif			
			if (temp%512!=0) temp=(temp/512 + 1)*512;
			TAP_Hdd_Fseek( recfp, 0, SEEK_SET );
			TAP_Hdd_Fwrite( recbuffer, temp, 1, recfp);
			TAP_Hdd_Fclose( recfp );
			TAP_MemFree(recbuffer);
		}
	}
	else
	{
		//logf("can't open "INI_FILE);
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			ShowMessageWin("ERROR:kann nicht geöffnet werden "RECPLAYBACKFILE, "",150,TRUE);
		else
			ShowMessageWin("ERROR:can't open "RECPLAYBACKFILE, "",150,TRUE);
	}
	return timeshiftreturn;
}



void clearEPG( EPG_INFO *epg )
{
	memset(epg->toppysatName,0x0,MAX_SatName);
	memset(epg->service_name,0x0,256);
	epg->topfieldinternalchannel=9999;
	epg->service_id = 0;
	epg->transport_stream_id=0;
	epg->original_network_id=0;
	epg->year = 0;
	epg->month = 0;
	epg->day = 0;
	epg->hour = 0;
	epg->min = 0;
	epg->duration = 0;
	memset( epg->title, 0x0, 257 );
	memset( epg->shortevt,0x0, 257);
	memset( epg->description, 0x0, 4000);
	epg->parental =0;
	memset( epg->recordingStatus, 0x0, 20 );
	epg->eventid =0;
}

dword GetCrc32(const void *data, dword n)
{
	dword	crc = -1;

	while (n--)
	{
		crc = (crc << 8) ^ _mpegCrc32Tbl[((crc >> 24) ^ *(byte*)data) & 0xff];
		data = (byte*)data + 1;
	}
	return crc;
} 

void Put32bit( void *p, dword data )
{
	*((byte*)p + 0) = (data >> 24)&0xff;
	*((byte*)p + 1) = (data >> 16)&0xff;
	*((byte*)p + 2) = (data >>  8)&0xff;
	*((byte*)p + 3) = (data      )&0xff;
}

byte BCD( byte value )
{
	return ((value/10)<<4) | ((value%10));
}

void *MakeEitTable( word service_id,
				   word transport_stream_id,
				   word original_network_id,
				   word eventId, 
				   dword startTime, 
				   dword duration,
				   word runningStatus,
				   unsigned char *evtname,
				   unsigned char *shortevt,
				   unsigned char *evtdesc,
				   byte parental)
{
	byte *eitData, *desc;
	TYPE_Eit *eit;
	dword descLen=0,shortLen=0,extendLen=0,parentalLen=0;
	//,chunkLen=0;
	int i,j;
	dword crc32;
	int len;
	int NumberExtended=0;
//	int temp;

	if ((eitData = (byte *)TAP_MemAlloc( 5500 ))==NULL)
	{
		logf(1,"ERROR: Not enough memory to load 5500byte EIT table-MakeEitTable");
		return 0;
	}
	else
	{
		eit = (TYPE_Eit *)eitData;

		eit->transport_stream_id	= transport_stream_id;
		eit->original_network_id	= original_network_id;

		eit->tableId				= 0x60;

		eit->section_syntax_indicator = 1;
		eit->reserved				= 0;
		eit->service_id_h			= (service_id>>8) & 0xff;

		eit->service_id_l			= service_id & 0xff;
		eit->reserved2				= 0;
		eit->version_number			= 0;
		eit->current_next_indicator	= 0;
		eit->section_number			= 0;
		eit->lastSectionNumber		= 0;

		eit->segment_last_section_number	=  0;
		eit->last_table_id					= 0;
		eit->event_id=(eventId)?eventId:((startTime>>4)&0xf000) +  ((startTime>>8)&0xff)*60+ (startTime&0xff);//last 4bits of date, plus number of minutes for 12bits, unless exists in file
		//logf(4,"event_id:%d",eit->event_id);

		eit->start_time_mjd			= (startTime >> 16 );
		eit->start_time_hour		= BCD( (startTime >> 8) & 0xff);
		eit->start_time_min			= BCD( (startTime) & 0xff);

		eit->start_time_sec			= 0;
		eit->duration				= (BCD(duration/60) << 16) | (BCD(duration%60) << 8);

		eit->runningStatus			= runningStatus;
		eit->free_ca_mode			= 0;

		desc = &eitData[26];	// 26 = Header Size.  desc=address of eit 26th byte.

		//extended event description
		len=strlen(evtdesc);
		if ((len<60)&& (strlen(shortevt)==0) && g_loadshortevents) //for less than 40unsigned chars, will just feed info to shortevt description.
		{
			strcpy(shortevt,evtdesc);
			memset(evtdesc,0x0,1);
			len=0;
		}

		if (len!=0)
		{
			if (len>3735) len=3735; //TONY HAS TESTED, THIS IS DEFINITELY THE MAX, I WOULD HAVE THOUGHT 16x249, but I was wrong, it is 15x249.
			if ((len%249)==0)
			{
				NumberExtended=(len/249)-1;  //if exactly 249unsigned chars or multiple
			}
			else
			{
				NumberExtended=len/249; //if not 249, 
			}

			for (j=0; j<=NumberExtended;++j)
			{
				desc[descLen+(extendLen++)] = 0x4e;	// Extended Event Descriptor in 1st byte
				desc[descLen+(extendLen++)] = descLen; //descriptor_length
				//logf(4,"descnumb:%d lastdesc:%d",j&0x0F,NumberExtended&0x0F);
				desc[descLen+(extendLen++)] = ((j&0x0F)<<4)|(NumberExtended&0x0F); //descriptor_number & last_descriptor_number
				desc[descLen+(extendLen++)] = 'e';   //ISO 639-2 [3]_language_code
				desc[descLen+(extendLen++)] = 'n';
				desc[descLen+(extendLen++)] = 'g';
				desc[descLen+(extendLen++)] =  0; //length_of_items, the toppy shows them as item_description:item_unsigned char

				/*
				temp=extendLen;
				desc[descLen+(extendLen++)] = strlen( s ); //item_description_length
				for( i = 0; i< strlen( s ); i++ )
				{
				desc[descLen+(extendLen++)] = s[i];  //item_description_unsigned char
				}
				desc[descLen+(extendLen++)] = strlen( s );  //item_length
				for( i = 0; i< strlen( s ); i++ )
				{
				desc[descLen+(extendLen++)] = s[i];   //item_unsigned char
				}
				desc[descLen+6] = extendLen-temp;      //setting item_length to the real size..
				*/

				len=strlen(evtdesc+j*249);
				if (len>249)
					len=249;

				desc[descLen+(extendLen++)] = len;  //text_length
				//strncpy(tony,evtdesc+chunkLen,100);
				//logf(4,"evtdeschunked: %s",tony);
				for( i = 0; i< len; i++ )
				{
					desc[descLen+(extendLen++)] = evtdesc[i+j*249];   //text_unsigned char
				}
				desc[descLen+1] = extendLen-2;      //setting descriptor_length to the real size..
				descLen=descLen+extendLen;  //move descLen on.
				extendLen=0;
				//desc[descLen+6] = extendLen-7;    //length_of_items
			}//extended event description
		}

		//short event description
		desc[descLen+(shortLen++)] = 0x4d;	// Short Event Descriptor in 1st byte
		desc[descLen+(shortLen++)] = descLen; //descriptor_length
		desc[descLen+(shortLen++)] = 'e';   //ISO 639-2 [3]_language_code
		desc[descLen+(shortLen++)] = 'n';
		desc[descLen+(shortLen++)] = 'g';
		if ((len=strlen(evtname))>128) 
			len=128;                      //truncate event name to 128unsigned chars
		desc[descLen+(shortLen++)] = len; //event_name_length
		for( i = 0; i< len; i++ )
		{
			desc[descLen+(shortLen++)] = evtname[i];  //event_name_unsigned char
		}
		//sprintf(shortevt,"%s %d",shortevt,eventId);

		if ((len=strlen(shortevt)+len)>=250) 
			len=250-desc[descLen+5]; //desc[descLen+5] is the length of the evtname.
		desc[descLen+(shortLen++)] = len;  //text_length
		for( i = 0; i< len; i++ )
		{
			desc[descLen+(shortLen++)] = shortevt[i];   //text_unsigned char
		}
		desc[descLen+1] = shortLen-2;   //setting the descriptor_length to the real size..
		descLen=descLen+shortLen;
		//short event description end

		//parental event description
		desc[descLen+ (parentalLen++)] = 0x55;	// Parental Descriptor tag in 1st byte
		desc[descLen+ (parentalLen++)] = descLen; //descriptor_length
		desc[descLen+ (parentalLen++)] = 'A';   //country_code
		desc[descLen+ (parentalLen++)] = 'U';
		desc[descLen+ (parentalLen++)] = 'S';
		desc[descLen+ (parentalLen++)] = parental;  //rating, where rating=min_age-3   15=MA,13=M,12=PG,10=G,6=P?,18=R
		desc[descLen+1] = parentalLen-2;   //setting the descriptor_length to the real size

		//end of parental

		descLen=descLen+parentalLen;

		eit->descriptor_loop_length	= descLen;
		eit->sectionLength			= 26 + descLen + 4 - 3;  //4 is CRC, 26 is header, 3 is??

		crc32 = GetCrc32( eitData, 26 + descLen);
		Put32bit( &eitData[26 + descLen], crc32 );

		return eitData;
	}
}//MakeEitTable


/*  
bool OsdActive (void)
{
	TYPE_OsdBaseInfo osdBaseInfo;
	dword * wScrn;
	dword iRow, iCol;

	TAP_Osd_GetBaseInfo( &osdBaseInfo );

	for ( iRow = 100; iRow < 500; iRow += 4 )  //every 4th line
	{
		wScrn = osdBaseInfo.eAddr + 720 * iRow;

		for ( iCol = 180; iCol < 300; iCol += 6 ) //may as well only scan every 6 pixels to save time and resource, stop at 350 as new f/w puts channel details to the right.
		{
			if ( (*(wScrn + iCol)) != 0 )
				return TRUE; // Do not allow Key Action
		}
	}
	return FALSE;
}
*/
/*-----------------28.05.2006 23:08-----------------
 * This function checks, if there is anything active on the OSD
 * The original function was replaced, because the OSD of the
 * 5000 MP behaves differently from the other 5000 PVRs
 * We use hardware registers of the EMMA chip to get the base addresses
 * of the two OSD planes.
 * --------------------------------------------------*/
bool OsdActive (void)
{
  volatile dword        * DGRP_OSD1CTR       = (dword*) 0xb0004104;       // Register mit Zeiger auf die OSD Strukturen
  volatile dword        * DGRP_OSD2CTR       = (dword*) 0xb0004108;       // Register mit Zeiger auf die OSD Strukturen

  dword                 *pOSD1Info, *pOSD2Info;
  dword                 pOSD1e, pOSD1o, pOSD2e, pOSD2o;
  word                  *pwAddr[576];                                      // Speichert die vertikale Adresse für 16 bit Planes
  byte                  *pbAddr[576];                                      // Speichert die vertikale Adresse für 8 bit Planes
  dword                 i, x, y;                                          // Schleifenzähler für Bildschirm-Scan
  bool			Plane1UsesLUT, Plane2UsesLUT;

  pOSD1Info = (dword *) ((*DGRP_OSD1CTR & 0x0fffffff) | 0x80000000);      // Zeiger auf Plane1
  pOSD2Info = (dword *) ((*DGRP_OSD2CTR & 0x0fffffff) | 0x80000000);      // Zeiger auf Plane2

  Plane1UsesLUT = ((*pOSD1Info & 0x00003000) == 0x00002000);
  if ((dword) pOSD1Info != 0x80000000)                                    // prüfen auf "0" Plane1
  {
    pOSD1e = (*(pOSD1Info + 6) & 0x0fffffff) | 0x80000000;
    pOSD1o = (*(pOSD1Info + 7) & 0x0fffffff) | 0x80000000;
    for( i=0; i<288; i++ )                                                // Schleife zum Speichern der vertikalen Adressen
    {
      if (Plane1UsesLUT)
      {
        pbAddr[2*i] = (byte *) (pOSD1e + 720 * i);                        // Vertikale Adressen auslesen für 1. und...
        pbAddr[2*i+1] = (byte *) (pOSD1o + 720 * i);                      // ... 2. Halbbild
      }
      else
      {
        pwAddr[2*i] = (word *) (pOSD1e + 720 * i * 2);                    // Vertikale Adressen auslesen für 1. und...
        pwAddr[2*i+1] = (word *) (pOSD1o + 720 * i * 2);                  // ... 2. Halbbild
      }
    }
    for( y=100; y<500; y+=4 )                                               // Schleife zum scannen der Zeilen des Screenbuffers
    {
      for ( x=180; x<300; x+=4 )                                            // Schleife für die Spalten
      {
      	if (Plane1UsesLUT)						  // Wenn ein Wert zurückgemeldet wird, ist irgendetwas auf den Bildschirm gezeichnet - dann TRUE zurückmelden
      	{
      	  if (*(pbAddr[y]+x) != 0) return TRUE;
        }
        else
        {
      	  if (*(pwAddr[y]+x) != 0) return TRUE;
      	}
      }
    }
  }

  Plane2UsesLUT = ((*pOSD2Info & 0x00003000) == 0x00002000);
  if ((dword) pOSD2Info != 0x80000000)                                    // Plane2
  {
    pOSD2e = (*(pOSD2Info + 6) & 0x0fffffff) | 0x80000000;
    pOSD2o = (*(pOSD2Info + 7) & 0x0fffffff) | 0x80000000;
    for( i=0; i<288; i++ )                                                // Schleife zum Speichern der vertikalen Adressen
    {
      if (Plane2UsesLUT)
      {
        pbAddr[2*i] = (byte *) (pOSD2e + 720 * i);                        // Vertikale Adressen auslesen für 1. und...
        pbAddr[2*i+1] = (byte *) (pOSD2o + 720 * i);                      // ... 2. Halbbild
      }
      else
      {
        pwAddr[2*i] = (word *) (pOSD2e + 720 * i * 2);                    // Vertikale Adressen auslesen für 1. und...
        pwAddr[2*i+1] = (word *) (pOSD2o + 720 * i * 2);                  // ... 2. Halbbild
      }
    }
    for( y=100; y<500; y+=4 )                                               // Schleife zum scannen der Zeilen des Screenbuffers
    {
      for ( x=180; x<300; x+=4 )                                            // Schleife für die Spalten
      {
      	if (Plane2UsesLUT)						  // Wenn ein Wert zurückgemeldet wird, ist irgendetwas auf den Bildschirm gezeichnet - dann TRUE zurückmelden
      	{
      	  if (*(pbAddr[y]+x) != 0) return TRUE;
        }
        else
        {
      	  if (*(pwAddr[y]+x) != 0) return TRUE;
      	}
      }
    }
  }
  return FALSE;
}

void change_chkFreq(int param, int tshift)
{
	int values[]={2,5,10,15,20,30,60,120,180,240,360,480,720};
	int i;

	for (i=0;i<=12;i++)
	{
		if (values[i]==g_freqMin[tshift])
			break;
		if ((i>0)&&(values[i]>g_freqMin[tshift]) && (values[i-1]<g_freqMin[tshift]))
			break;
	}

	if (param==RKEY_VolUp)
	{
		if (i==12) i=-1;
		g_freqMin[tshift]=values[i+1];
	}
	else if (param==RKEY_VolDown)
	{
		if (i==0) i=13;
		g_freqMin[tshift]=values[i-1];
	}

}


void updateINIwithLCN(void) 
{
	int i,j;
	TYPE_IniSvcDetails *NewIniStore,*OldIniStore;
	bool LCNinINI=FALSE,added=FALSE;

	for (i=0;i<1000;i++)
	{
		if (gLCNfound[i].LCNfound)
		{
			for (j=0;j<g_NumServices; j++) //go thru the existing number of ini services to find this LCN
			{
				if (gLCNfound[i].IntChannelNum==g_IniSvcPtr[j].topfieldinternalchannel)//already in the inifile
				{
					LCNinINI=TRUE;
					break;
				}
			}
			if (!LCNinINI) //if it isn't already in the ini, have to malloc some more memory and add
			{
				NewIniStore=(TYPE_IniSvcDetails *)TAP_MemAlloc((g_NumServices+1)*sizeof(*g_IniSvcPtr));
				if (NewIniStore) //if we got the memory
				{
					memcpy(NewIniStore,g_IniSvcPtr,g_NumServices*sizeof(*g_IniSvcPtr));
					OldIniStore=g_IniSvcPtr;
					g_IniSvcPtr=NewIniStore;
					TAP_MemFree(OldIniStore);
					g_IniSvcPtr[g_NumServices].toppysatName[0]='\0';
					sprintf(g_IniSvcPtr[g_NumServices].tgdservice_name,"%d",i);
					g_IniSvcPtr[g_NumServices].topfieldinternalchannel=gLCNfound[i].IntChannelNum;
					g_IniSvcPtr[g_NumServices++].LCN_channel_id=i;
					added=TRUE;
				}
				
			}
			else LCNinINI=FALSE; //reset to FALSE at this point
		}
	}
	if (added)
	{
		logf(3,"Saving new found TGD LCNS to the inifile");
		SaveSettings(NOICE);
	}
	gLCNinTGD=FALSE;
}

int put_chanServiceDetails_TGD(EPG_INFO *epg,int count) 
{
	bool lcnfound=FALSE;
	int i,j,k=0;
	char LCNStore[256];//static so each iteration reads ok.
	int SingleLCN=0;
	char *ptr; //*p=LCNStore,*ptr;
	TYPE_TapChInfo chInfo;
	//logf(4,"buffer:%s",buf);
	if (g_bLCNTGDload && epg->service_name[0]>='0' && epg->service_name[0]<='9') //numeric service name.. well, the first char!
	{
		if (count<9999) //silly trick so routine can indicate ok, then return 0 next time called.
		{
			strcpy(LCNStore,epg->service_name);
			for (j=0;j<=count;j++) //have to keep getting next comma seperated LCN from the last iteration when this routine is called
			{
				if ((ptr = strchr(LCNStore,',')) == NULL) //no more comma's for null
				{
					k=9999; //indicate g_NumServices so put_chanServiceDetails_TGD routine won't be called again
					SingleLCN=atoi(LCNStore);
					break;
				}
				else  //pointer will be set to point to the comma at this point
				{
					k=count+1;
					*ptr='\0';
					SingleLCN=atoi(LCNStore);
					strcpy(LCNStore,ptr+1);
					//pointer++; //move to after the comma
				}
			}	
		
			for ( j = 0; j < g_total_tvSvc; j++ )		//search for toppy LCN		
			{
				TAP_Channel_GetInfo( SVC_TYPE_Tv,  j, &chInfo );
				if (chInfo.logicalChNum==SingleLCN)
				{
//#if DEBUG					
//						logf(4,"%d",SingleLCN);
//#endif					
						gLCNfound[SingleLCN].LCNfound=TRUE;     //setting up an LCN array for easy lookup of channel
						gLCNfound[SingleLCN].IntChannelNum=j;
						gLCNinTGD=TRUE;
						epg->service_id=chInfo.svcId;
						epg->transport_stream_id=chInfo.tsId;
						epg->original_network_id=chInfo.orgNetId;
						epg->topfieldinternalchannel=j; 
						strcpy(epg->toppysatName,"");
						lcnfound=TRUE;
						break;
				}
			}
			if (!lcnfound) return 0;
			return k;
		}
		else return 0;
	} //numeric in TGD else-
	else for (i=count;i<g_NumServices; i++) //else not a numeric TGD, so use inifile method
	{
		//logf(4,"buf %s, servicename %s",buf,service_name+i);
		if (g_IniSvcPtr[i].topfieldinternalchannel!=9999 && strncmp(g_IniSvcPtr[i].tgdservice_name,epg->service_name,128)==0) //if servicename and tgdname the same 
		{
			strcpy(epg->toppysatName,g_IniSvcPtr[i].toppysatName);
			epg->service_id				= g_IniSvcPtr[i].service_id ;
			epg->transport_stream_id	= g_IniSvcPtr[i].transport_stream_id ;
			epg->original_network_id	= g_IniSvcPtr[i].original_network_id ;
			epg->topfieldinternalchannel= g_IniSvcPtr[i].topfieldinternalchannel;
			//logf(4,"returned count %d",i+1);
			return i+1;  //return the next ini entry along (for multiple ini entries for the one TGD entry)
		}
	}
	//logf(4,"returned count 0");
	return 0;
	//logf(4,"srv%d trans%d orig%d",srvc->service_id,srvc->transport_stream_id,srvc->original_network_id);
}


int LoadEPG(unsigned char *TGDfilename,int filedate)
{
	unsigned char * in_buf;
	//unsigned char * memory;
	static unsigned char buf[5000];
	static unsigned char sBuff[64];
	TYPE_File* fp;
	void *eitData;
	dword startTime;
//	dword duration;
	EPG_INFO	epg;
	int idx=0, i;
//	int nWidth, nHeight, xStart, yStart;
	//dword lasttick;
	//int rgn=0;		// stores rgn-handle for osd
	word epgMJDdate;	//var to store date created with epg entry.
	unsigned char str1[512];
	int filelength=0;
	int servicenumberfound=0;
	int channelNum;
	bool crfound=FALSE;
	int OFFSX=75;
	int OFFSY=40;
	TYPE_TapChInfo chInfo;
	//int filenum=0;
    int dstOffset=0;
    word currentmjd;
    byte currenthour,currentmin,currentsec;
    dword start,end;
    int runningstatus;
	int offset;
    
	if (!(fp = TAP_Hdd_Fopen (TGDfilename)))
	{
		logf(1,"ERROR:Can't open file!!: \"%s\"",TGDfilename);
		return 1;
	}
	if (!g_bloadevts) //if we haven't loaded any events yet
	{
		g_bloadevts=TRUE;
		logf(4,"EIT%s return:%d",(gEITload)?"LOAD":"BLOCK",TAP_ControlEit(gEITload));
	}
	logf(1,"Loading EPG file \"%s\"",TGDfilename);
	//lasttick=TAP_GetTick()-210;
	filelength=TAP_Hdd_Flen( fp );
	if (!(filelength%512)) filelength+=512;  //FRIGGIN TOPFIELD BUG! flen is 512bytes out if the filesize is a multiple of 512.

	if ((in_buf=(unsigned char *)TAP_MemAlloc(filelength+2))==NULL)
	{
		logf(1,"ERROR:Not enough memory to load \"%s\"",TGDfilename);
		TAP_Hdd_Fclose (fp);
		return 1;
	}
		
	//in_buf=memory;
	//logf(4,"filelength:%d",filelength);
	clearEPG( &epg );
	//logf(4,"bytes_in:%d",bytes_in);
	TAP_Hdd_Fread (in_buf, 1, filelength, fp);
	TAP_Hdd_Fclose (fp);
	if(filedate>=99990000) 
	{
		logf(2,"File loaded to ram, now deleting: %s",TGDfilename);
		TAP_Hdd_Delete( TGDfilename );
	}
	*(in_buf+filelength)=0x0;
	*(in_buf+filelength+1)=0x0;

	//memset( buf, 0x0, 2048 );

	for(i=0; i<filelength; i++)
	{
		// If we don't have a \n add the unsigned character to the current buffer
		if (idx<=5000-3) //3000 byte max record size.
		{
			buf[idx++] = *in_buf++;
			gbytestransferred++;  //countbytes
		}
		else
		{
			while ( *in_buf != '\n' && *in_buf != '\r') //if record too large, will scan to the end of the line anyway
			{
				*in_buf++;
				gbytestransferred++;  //event count carriage returns to make the byte count identical to file size
				i++; 
			}
			crfound=TRUE;  
		}
		while ( *in_buf == '\n' || *in_buf == '\r'|| *in_buf == '\0') //scan thru all CR/LF and EOF
		{
			crfound=TRUE;
			if (*in_buf == '\0') break; //end of file
			*in_buf++;
			gbytestransferred++;  //event count carriage returns to make the byte count identical to file size
			i++;
			//TAP_SystemProc();
		}
		if (crfound) 
		{
			crfound=FALSE;
			buf[idx++]='\t';  //add a tab to the end to make the InterpretEPGbuff easier to populate
			buf[idx]='\0';
			// Fill the EPG record
			InterpretEPGbuff( &epg, (const unsigned char*)buf );  //use new buffer to fill epg

			memset(sBuff,0,64);
			if ( filedate <99990000) //if NOT an info file, load into EIT
			{
				epgMJDdate=TAP_MakeMjd((word)epg.year,(byte)epg.month,(byte)epg.day);
				TAP_GetTime( &currentmjd, &currenthour, &currentmin, &currentsec );

				//code used to manipulate epg that started on DST date, but date hadn't reached DST yet.  So if a timer was created from the
				//future epg, the timer would have the correct start time.
				if (g_GMTloadAdjust)
				{
					dstOffset=0;
					if ((epgMJDdate>gDstStart && epgMJDdate<gDstEnd) || (epgMJDdate==gDstStart && (byte)epg.hour>=2) || (epgMJDdate==gDstEnd && (byte)epg.hour<3))
					{
						if (!gDstActive)  //if daylight savings isn't already active,
							dstOffset=60; //add daylight offset to future entries if not quite dst yet 
					}
					else
					{
						if (gDstActive)  //if daylight savings is currently active offset will be more than it should be, so
							dstOffset=-60;
					}
				}
//#if DEBUG
//              printf("Current Date/Time %x %02d:%02d %02d\n", currentmjd, currenthour, currentmin, currentsec);
// #endif
				 start=addtime(((epgMJDdate<<16) | ((byte)epg.hour<<8) | (byte)epg.min),g_NextUpdateTime);
				 end=addtime(start,epg.duration);
				 
				 if(( start <= (currentmjd<<16|currenthour<<8|currentmin) ) && ( end > (currentmjd<<16|currenthour<<8|currentmin)))  //if now between start and end
					runningstatus=4;
				 else 
					runningstatus=1;
																				   
				if (epg.offsetminutes==1440) //1440 used to indicate no offset exists in parser
				{
					if (g_useGMT==GMTTOPPY)
							offset=g_tdtUTCoffset;
					else
							offset=g_GMToffst;
					startTime = addtime(((epgMJDdate<<16) | ((byte)epg.hour<<8) | (byte)epg.min),-(offset+dstOffset));	//((MJD << 16) | (Hour << 8) | Min)
				}
				else
				{
#if DEBUG
                    printf("epg.offsetminutes:%d\n",epg.offsetminutes);
#endif				
					startTime = addtime(((epgMJDdate<<16) | ((byte)epg.hour<<8) | (byte)epg.min), -epg.offsetminutes);	//((MJD << 16) | (Hour << 8) | Min)
				}

				if (startTime>gTGDlastEITStartTime) gTGDlastEITStartTime=startTime; //storing the last EIT starttime loaded
				do{
					if ((servicenumberfound=put_chanServiceDetails_TGD(&epg, servicenumberfound)))  //if we get number back from put_chanServiceDetails_TGD
					{
						//logf(4,"Servicenumberfound:%d",servicenumberfound);
						if (strlen(epg.toppysatName)>0)
						{
							//logf(4,"epg toppysatname found %s",epg.toppysatName);
							TAP_Channel_GetCurrent( &g_svcType, &channelNum);
							TAP_Channel_GetInfo( g_svcType, channelNum, &chInfo );
							if (strcmp(chInfo.satName,epg.toppysatName)==0) //if chInfo.satName,epg.toppysatName equal
							{
								strcpy(g_satname,epg.toppysatName);
								eitData = MakeEitTable( epg.service_id, 
									epg.transport_stream_id,
									epg.original_network_id,
									epg.eventid, startTime, epg.duration, runningstatus, //1=runningstatus 
									epg.title, epg.shortevt, epg.description, epg.parental );
								if (eitData)
								{
									gtestEvtId++;  //count events uploaded
									TAP_PutEvent( eitData );
									TAP_MemFree((unsigned char *)  eitData );
								}
								else i=filelength; //finish loading file
							}
							//else don't upload
						} 
						else   //no satellitename defined in epg
						{ 
							eitData = MakeEitTable( epg.service_id, 
							epg.transport_stream_id,
							epg.original_network_id,
							epg.eventid, startTime, epg.duration, runningstatus, //1=runningstatus 
							epg.title, epg.shortevt, epg.description, epg.parental );
							if (eitData)
							{
								gtestEvtId++;  //count events uploaded
								TAP_PutEvent( eitData );
								TAP_MemFree((unsigned char *)  eitData );
							}
							else i=filelength; //finish loading file
						}
					}
				}
				while (servicenumberfound);
			} //if (atoi(strncpy(sBuff,TGDfilename,strlen(TGDfilename)-4))<99990000) //if NOT an info file, load into EIT
			else if (filedate==99999997)
			{
				sprintf(str1,"%02d:%02d %s frm: %s",epg.hour,epg.min,epg.title,epg.shortevt);
				TAP_Win_AddItem (&g_winOpts, str1);
				emailarray[g_emailcounter]=TAP_MemAlloc(strlen(epg.description)+1);
				memset(emailarray[g_emailcounter],0,strlen(epg.description)+1);
				strncpy(emailarray[g_emailcounter++],epg.description,strlen(epg.description));
				//memset(emailarray[g_emailcounter++]+strlen(epg.description)+1,0,1);
			}
			clearEPG( &epg );  // Clear the record and move on
			idx = 0;
			gbSystemProc=TRUE;
			TAP_SystemProc();//let toppy do other things (such as channel changing)
			gbSystemProc=FALSE;
		}
	} //end while that counts thru the inbuf
	TAP_MemFree((unsigned char *) in_buf);

	//following will put OSD if set in the menu, and if starting up with nothing onscreen (including but banner if loading mult files is ok)
	//logf(4,"OSD active:%s !(g_bLoadSettings && !g_deleteregion && OsdActive()):%d",OsdActive()?"YES":"NO",!(g_bLoadSettings && !g_deleteregion && OsdActive(50,500)));
	memset(sBuff,0,64);
	if (filedate < 99990000)  //if NOT a info file, show load
	{
		//log events
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			logf(1,"Sendungen geladen:%d bytes:%d", gtestEvtId, gbytestransferred);
		else
			logf(1,"Events Loaded:%d bytes:%d", gtestEvtId, gbytestransferred);
		//and display them	
		if (g_ShowLoadingOSD!=DISOFF)
		{
			if (!_gFullRegion)
			{
				_timersUploadedRegion = TAP_Osd_Create( OFFSX, OFFSY, 420, 100, 0, FALSE );		// create rgn-handle
				OFFSX=0;
				OFFSY=0;
			}
			else
				_timersUploadedRegion=_gFullRegion;
	
			g_deleteregion=TAP_GetTick();
	
			if (g_ShowLoadingOSD==LARGE)
			{
				if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
					sprintf(str1," %d Sendungen geladen %dbytes", gtestEvtId, gbytestransferred); //one last print.
				else
					sprintf(str1," %d Events Loaded %dbytes", gtestEvtId, gbytestransferred); //one last print.
	
				TAP_Osd_FillBox(_timersUploadedRegion, OFFSX, OFFSY, 400, 32, RGB(19,19,19) );	// draw background-box for border x,y,w,h
				TAP_Osd_PutS(_timersUploadedRegion, OFFSX+5, OFFSY+4, 394+OFFSX, str1,		// show message x,y,maxX
					RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_LEFT);
			}
			else //must be minimal setting
			{
				sprintf(str1,"%d", gtestEvtId); 
				TAP_Osd_FillBox(_timersUploadedRegion, OFFSX, OFFSY, 80, 32, RGB(19,19,19) );	// draw background-box for border x,y,w,h
				TAP_Osd_PutS(_timersUploadedRegion, OFFSX+3, OFFSY+4, 77+OFFSX, str1,		// show message x,y,maxX
					RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_CENTER);
			}
		}
	}
	return 0;
}



void HideWindow ()  //generic hide routine
{
	int i;
	g_bOptsWinShown=FALSE;
	g_bMailOptsWinShown=FALSE;
	//g_bFileWinShown=FALSE;
	g_bSvcWinShown=FALSE;
	//g_bepgFileWinShown=FALSE;
	g_bIniWinShown=FALSE;
	g_bLogWinShown=FALSE;
	//g_deleteregion=0;  //just in case it was set, shouldn't be though...

    TAP_Win_Delete (&g_winOpts);
	if (_gFullRegion>0)
	{
		TAP_Osd_Delete (_gFullRegion);
		if (_timersUploadedRegion>0) _timersUploadedRegion=0;
	}
	if (_timersUploadedRegion>0)
		TAP_Osd_Delete (_timersUploadedRegion);
	if (_rotateUploadedRegion>0)
		TAP_Osd_Delete (_rotateUploadedRegion);
	if (_emailWindowRegion>0)
		TAP_Osd_Delete (_emailWindowRegion);
	if (_emailOptionsRegion>0)
		TAP_Osd_Delete (_emailOptionsRegion);

	_emailOptionsRegion=0;
	_emailWindowRegion=0;
	_timersUploadedRegion=0;
	_rotateUploadedRegion=0;
	_gFullRegion=0;
	if (g_emailcounter>0)
	{
		for (i=g_emailcounter-1;i>=0;i--)
		{
			TAP_MemFree(emailarray[i]);
		}
		g_emailcounter=0;

	}
	TAP_EnterNormal();      // re-entering normal receiver operation
	//g_bOptsWinShown = FALSE;
}

void HideOptions () //only used to hide options
{
	g_deleteregion=0; //just in case it was set, shouldn't be though...
	g_bOptsWinShown=FALSE;

	TAP_Win_Delete (&g_winOpts);
	TAP_Osd_Delete (_gFullRegion);
	_gFullRegion=0;
	SaveSettings(NOICE);
}

void ChangeDirRoot()
{
	TYPE_File	fp;
	int		iNdx;

	iNdx = 0;
	TAP_Hdd_FindFirst( &fp );
	//	Loop until Root found allow maximum of 20 levels
	while ( !strstr( fp.name, "__ROOT__" ) &&  iNdx < 20 )
	{
		TAP_Hdd_ChangeDir( ".." );
		TAP_Hdd_FindFirst( &fp );
	}
}



void ChangeDir(unsigned char *DirName)
{
	//strcpy(gcurrentDir,DirName);
	ChangeDirRoot();
	if (strstr(DirName,"Auto Start")) //!strstr==0 implies they are equal
	{
		TAP_Hdd_ChangeDir("ProgramFiles");
		TAP_Hdd_ChangeDir("Auto Start");
	}
	else if (strstr(DirName,"ProgramFiles"))
	{
		TAP_Hdd_ChangeDir("ProgramFiles");
	}
	else TAP_Hdd_ChangeDir(DirName);
}

void TSRCommanderConfigDialog()
{	
	g_nselected=NEXTTAP;
	//ChangeDir(gtapDirName); //just in case
	LoadSettings(FALSE);   
	ShowOptionsWindow ();
}

bool TSRCommanderExitTAP (void)
{
//	int i;

	HideWindow();
	//ChangeDir(gtapDirName);
	SaveSettings(NOICE);
	if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
		ShowMessageWin("wird beendet",_PROGRAM_NAME_,100,TRUE);
	else
		ShowMessageWin("Quitting",_PROGRAM_NAME_,100,TRUE);
	logf(3,"Quitting...");
	logf(3,"ControlEITLOADreturn:%d",TAP_ControlEit(TRUE));
	
	if (g_bcanclearRAM)	TAP_MemFree(g_mailramfree);
	TAP_MemFree((unsigned char *)  g_eventinfo );
	TAP_MemFree((unsigned char *) g_IniSvcPtr );
	TAP_Osd_Delete(_cogIconRAM3);
	TAP_Osd_Delete(_cogIconRAM2);
	TAP_Osd_Delete(_cogIconRAM1);
	TAP_Osd_Delete(_emailIconRAM);
	
	if (g_bcancloseLOG)	TAP_Hdd_Fclose( gLogFPointer );
	if (gGmt_updateblock)	*(dword *)gGmt_updateblock = gGmt_updateblockbackup;  //orig af8284c4  //appears to be regular updater.
	return TRUE;
}


/******************************************************************************
Word Wrap string output.... largely modified to support paging by tony

str: The string to be printed.
x, y, w: print area starts at x pixels in, y pixels down and is w pixels wide.
fgCol, bgCol: foreground and background colours.
maxRows: defines the hight of the print area in rows of characters.
fntSize: one of FNT_Size_1419, FNT_Size_1622 and FNT_Size_1926.
lineSep: The extra pixels between rows (can be negative with bgCol = COLOR_None
for overprinting).

Control characters (cariage return, new line etc.) are not supported.
******************************************************************************/
int WrapPutStr(dword rgn, char *str, int offset, int x, int y, int w, int fgCol, int bgCol, int maxRows, byte fntSize, int lineSep)
{
	int row=0,c=0,p=0,c2=0;
	int done = 0; //, try = 5;
	int fontH;
	int avgFontW;
	int len;
	int newStrW;
	bool breakfound=FALSE;
	char newStr[200]; //100 should be more than enough for a line

	switch (fntSize)
	{
		case FNT_Size_1419 : fontH = 19; avgFontW = 6; break;
		case FNT_Size_1622 : fontH = 22; avgFontW = 8; break;
		case FNT_Size_1926 : fontH = 26; avgFontW = 10; break;
		default : fontH = 19; avgFontW = 6; break;
	}

	if ((len = strlen(str+offset))!=0)
	{
	logf(4,"Stringlength %d, offset %d",len,offset);
	for (row=0; row<maxRows;row++)   
	{
		c = w/avgFontW;	// guess approx chars in line
		if(p + c > len) c = len - p;
		if(c > 199) c = 199;
		c2=0;
		//strncpy(newStr,str+p, c); // copy approx chars per line to new string
		breakfound=FALSE;
		
		while(!done)
		{
			if(c2>=c) break;
			strncat(newStr,str+p+c2+offset,1);
			if (c2>=3 && str[p+c2+offset]=='>' && str[p+c2-1+offset]=='r' && str[p+c2-2+offset]=='b' && str[p+c2-3+offset]=='<')
			{
				//logf(4,"found <br>. c is %d. text %s",c,newStr);
				if (c2==3) //a <br> is by itself on a line.. may as well skip multiple <br>s.
				{
					newStr[0]='\0';
					if (p+4<len)
					{
						p+=4; //multiple br is found, so move onto next line
						c2=-1; //as c2 is incremented after the if, want it to be zerod.
					}
					else break;
				}
				else  //a <br> in a line, move the C counter back 3 and terminate the string and indicate a break found
				{ 
					c=c2-3;
					newStr[c]='\0';
					breakfound=TRUE;
					break;					
				}
			}
			c2++;
		}
		
		while(!done)
		{
			newStrW = TAP_Osd_GetW(newStr,0,fntSize); // see if it fits
			if (newStrW > w && c > 0)  // Too big
			{		 //remove some chars
				if(c > 4) c = c - 4;
				else c--;
				newStr[c]= '\0';
			}
			else
			{
				done = 1; // string short enough
			}
		}

		done = 0;

		while (TAP_Osd_GetW(newStr,0,fntSize)<w && !breakfound)    //while the width of the text is less than a lines length
		{
			if (p + c + 1 > len)   //if the counter is larger than the string length of the orig string, we have really finished here 
			{
				done=1;					
				break;
			}
			strncat(newStr,str+p+c+offset,1); //copy 1 char from str at a time.
			c++;                 //get next char
		}

		if (!breakfound && !done && (maxRows - row) > 1) // find wrap position unless finished or last row
		{
			c--; //reduce c by 1 as it is one too much from last while statement.
			while (!isspace(newStr[c])&&!(newStr[c]=='-')&&(c>0))
		 	{   //look for a space or hyphen (or underflow).
				c--;
			}
			c++; //add 1 to keep hyphen on right line
		}
		newStr[c]='\0';   //terminate str (early at wrap point if not done).
		TAP_Osd_PutStringAf(rgn, x, y+((fontH+lineSep)*row), -1, newStr, fgCol, bgCol, 0, fntSize, 0);  //put multiple lines down if required.
		//TAP_Osd_PutStringAf(rgn, x, y+((fontH+lineSep)*row), x+w, newStr, fgCol, bgCol, 0, fntSize, 0);  //put multiple lines down if required.
		if (breakfound)
			p = p + c + 4;   //store current position
		else
			p = p + c;   //store current position
			
		newStr[0] = '\0';  //reset string.
	}  
	}
	//printf("p:%d, len:%d \n",p,len);
	if (p<len) 
		return p+offset;
	else
		return 0;
	
	
}


void ShowEmailText (int messagenumber)
{
	if (_gFullRegion==0)
	{
		TAP_ExitNormal();      // leaving normal receiver operation, receiver hides its OSDs
		if (_emailWindowRegion==0)
			//TAP_Win_Create( &g_winOpts, _emailOptionsRegion, 40, 30, 640, 100, FALSE, TRUE );
			_emailWindowRegion= TAP_Osd_Create (40, 220, 640, 336, 0, FALSE);
	}
	else _emailWindowRegion=_gFullRegion;

	//dword TAP_Osd_GetW( const char *str, byte fntType, byte fntSize )
	TAP_Osd_FillBox(_emailWindowRegion, 0, 0, 636, 336, RGB(3,5,10) ); //TAP_Osd_FillBox( rgn, x, y, w, h, color )
#if DEBUG		
	logf(4,"gmailposn before WrapPutStr %d",g_Mailposition);
#endif
	g_Mailposition=WrapPutStr(_emailWindowRegion, emailarray[messagenumber],g_Mailposition, 5, 5, 616, COLOR_White, COLOR_None, 16, FNT_Size_1622, -2);
#if DEBUG
	logf(4,"gmailposn after WrapPutStr %d",g_Mailposition);
#endif		
	g_bEmailTextWinShown = TRUE;

} //ShowEmailText


void ShowLogFile (void)
{
	unsigned char	sprintfile[10240];
	unsigned char str1[300]; //,str2[50],str3[60],str4[50];
	int i,j;
	int idx=0;
	int num_items=0;
	int filelength;
//	TYPE_File* fp;

	memset(sprintfile,0,sizeof sprintfile);

	//ChangeDir(gTGDdirectory);
	if (!g_bcancloseLOG) gLogFPointer = TAP_Hdd_Fopen( LOG_FILE);
	if (gLogFPointer!=0)
	{
		filelength=TAP_Hdd_Flen( gLogFPointer );
		if (!(filelength%512)) filelength+=512;  //FRIGGIN TOPFIELD BUG! flen is 512bytes out if the filesize is a multiple of 512.
		if (filelength>sizeof sprintfile) filelength=sizeof sprintfile-1;
		TAP_Hdd_Fseek(gLogFPointer, 0, SEEK_SET);
		TAP_Hdd_Fread( sprintfile, filelength, 1, gLogFPointer );
		//TAP_Hdd_Fclose(gLogFPointer );
		g_bcancloseLOG=TRUE;
		if (!_gFullRegion)
		{
			TAP_ExitNormal();      // leaving normal receiver operation, receiver hides its OSDs
			_gFullRegion= TAP_Osd_Create (0, 0, 720, 576, 0, FALSE);
		}
		TAP_Win_SetDefaultColor (&g_winOpts);
		TAP_Win_Create (&g_winOpts, _gFullRegion, 30, 30, 600, 410, FALSE, TRUE);
		TAP_Win_SetTitle (&g_winOpts, "Log file:"LOG_FILE"", 0, FNT_Size_1622);

		for (i=0;i<filelength; i++)
		{
			if (sprintfile[i] == '\0')
				break;
			else if (sprintfile[i]!='\n' && sprintfile[i]!='\r' && idx<sizeof str1 && TAP_Osd_GetW1622( str1 )<575)
			{
				str1[idx++] = sprintfile[i];
				if (sprintfile[i+1] == '\n' || sprintfile[i+1] == '\r') i++;
			}
			else
			{
				for (j=0;j<strlen(str1);j++)
				{
					if (str1[j]=='\n' || str1[j]=='\r' || str1[j]=='\t' || str1[j]=='\v') str1[j]=' ';
					else if (str1[j]== '\\' || str1[j]=='\'' || str1[j]=='\"' || str1[j]=='\?' ) str1[j]='_';
				}
				if ((idx==sizeof str1) || TAP_Osd_GetW1622( str1 )>=575) i--;
				idx=0;
				TAP_Win_AddItem (&g_winOpts, str1);
				memset( str1, 0, sizeof str1 );
				num_items++;
			}
		}
		TAP_Win_SetSelection (&g_winOpts, num_items);
		g_bLogWinShown = TRUE;
	}
	else //logfile can't be opened
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			ShowMessageWin("kann nicht geöffnet werden file", LOG_FILE,150,FALSE);
		else
			ShowMessageWin("can't open file", LOG_FILE,150,FALSE);

} //ShowLogWindow

void show_service_details(void)
{
	TYPE_TapChInfo chInfo;
	unsigned char str1[90]; //,str2[50],str3[60],str4[50];
	int i;
	//int num_items;
	//int nTvSvc,nRadioSvc;

	//TAP_Channel_GetTotalNum( &nTvSvc, &nRadioSvc );
	if (!_gFullRegion)
	{
		TAP_ExitNormal();      // leaving normal receiver operation, receiver hides its OSDs
		_gFullRegion= TAP_Osd_Create (0, 0, 720, 576, 0, FALSE);
	}
	TAP_Win_SetDefaultColor (&g_winOpts);
	//TAP_Win_Create (&g_winOpts, _gFullRegion, 60, ((278-(num_items+2)*10)<30)?30:(278-(num_items+2)*10), 550, (((num_items+2)*18)>546)?546:(num_items+2)*18, FALSE, FALSE);
	TAP_Win_Create (&g_winOpts, _gFullRegion, 40, ((241-g_total_tvSvc*11)<20)?20:(241-g_total_tvSvc*11), 590, ((g_total_tvSvc*22)>440)?440:g_total_tvSvc*22, FALSE, TRUE);

	TAP_Win_SetTitle (&g_winOpts, "Topfield Channel Details", 0, FNT_Size_1622);

	for ( i = 0; i < g_total_tvSvc; i += 1 )				
	{
		TAP_Channel_GetInfo( SVC_TYPE_Tv,  i, &chInfo );
		if (strcmp(g_satname,"DVBT")==0)
		{
			sprintf( str1, "%03d LCN:%04d srvcId:%04d tsId:%04d orignetId:%04d %s",
				i, chInfo.logicalChNum, chInfo.svcId,chInfo.tsId, chInfo.orgNetId, chInfo.chName);
		}
		else
		{
			sprintf( str1, "%03d srvId:%04d tsId:%04d orgnetId:%04d Sat%d:%s %s",
				i+1, chInfo.svcId,chInfo.tsId, chInfo.orgNetId, chInfo.satIdx, chInfo.satName, chInfo.chName);
		}
		//logf("%s-%s",service_name[i],toppyservice_name[i]);
		TAP_Win_AddItem (&g_winOpts, str1);
	}
	g_bSvcWinShown = TRUE;
}//show_service_details

void delete_old_tgd(void)
{
	word mjd;
	byte hour,min,sec;
	word year;
	byte month, day, weekDay;
	int totalEntry;
	TYPE_File file;
	dword filedate;
	unsigned char sBuff[128];
	//unsigned char sBuff2[128];
    unsigned char sdate[9];
    //byte tempmonth,tempday;
    //word tempyear;
	

	TAP_GetTime( &mjd, &hour, &min, &sec);
    gTodayMJDdate=mjd;
	logf(3,"entered file delete: %02d:%02dH",hour,min);

	if (hour>=6)  //epg looks to start about 6am, lets take 2 day old file data out.
		mjd=mjd-1;
	else 
		mjd=mjd-2;

	TAP_ExtractMjd(mjd, &year, &month, &day, &weekDay );

	sprintf(sdate,"%04d%02d%02d",year,month,day);
	logf(4,"date to del<= %02d%02d%02d",year,month,day);

	//ChangeDir(gTGDdirectory);

	totalEntry = TAP_Hdd_FindFirst( &file );
	while( totalEntry-- )
	{
		//memset(sBuff,0x0,128);

		strcpy( sBuff, file.name );
		strlwr( sBuff );  //lowercase
		//logf(4,"filename:%s",sBuff);

		if (strstr( sBuff + strlen( sBuff ) - 4, ".tgd" ))
		{
			memset(sBuff,0x0,128);
			strncpy(sBuff,file.name,strlen(file.name)-4);
			filedate= atoi( sBuff );
			if (filedate<=atoi(sdate)) //if filedate<=todaydate-1 or 2
			{
				logf(2,"Deleting File: %s",file.name);
				TAP_Hdd_Delete( file.name );
			}
		}	
		if( totalEntry==0 || TAP_Hdd_FindNext( &file ) == 0 )
		{
			break;
		}
	}
}

void bubble_sort(dword a[], int size)
{
	int nswaps,i;
	dword temp;
	do {
		for (nswaps = i = 0 ; i < size - 1 ; i++)
			if (a[i] > a[i+1]) {
				temp=a[i];  //fill buffer 
				a[i]=a[i+1];  //swap 
				a[i+1]=temp;
				++nswaps;
			}
	} while (nswaps > 0);
}

TYPE_LoadState load_new_tgd(bool firsttime)
{
	int totalEntry;//,i;
	TYPE_File file;
	//dword filedate;
	unsigned char sBuff[128];
//	bool loaded=FALSE;
	//dword fileNames[40];
	//int count=0;

	//ChangeDir(gTGDdirectory);

	gfilenames_count=0;
	
	totalEntry = TAP_Hdd_FindFirst( &file );
	while( totalEntry-- )
	{
		strcpy( sBuff, file.name );
		strlwr( sBuff );  //lowercase

		if (strstr( sBuff + strlen( sBuff ) - 4, ".tgd" ))
		{
			memset(sBuff,0,128);
			gfileNames[gfilenames_count++]=atoi(strncpy(sBuff,file.name,strlen(file.name)-4));
		}
		if( totalEntry==0 || TAP_Hdd_FindNext( &file ) == 0 )
		{
			break;
		}
	}
	if (gfilenames_count)
	{
		bubble_sort(&gfileNames[0],gfilenames_count);
		if (firsttime)
			return FIRSTTIME;
		else
			return START;
	}
	else
		return NOMORE;
}

void memfreecallback(void)
{
#if DEBUG	
	logf(4,"memfreecallback called");
#endif
	g_bcanclearRAM=TRUE;

}               


TYPE_LoadState load_TGD_stateMachine(TYPE_LoadState state)
{
	static bool loaded=FALSE;
	static int fileposn;
	static bool firstime;
	unsigned char sBuff[128];
    word tempyear;
    byte tempmonth;
    byte tempday;
    byte tempweekday;
    dword intMAXdate;
    unsigned char tempdates[9];
	unsigned char *waveram=NULL;
	long wavesize=0;
	int waverate=0;
	int realwaverate=0;
	gloading=TRUE;  //use this as a lock to stop idle routine from doing everything again
	
	switch (state){

	case START:
	case FIRSTTIME:
		loaded=FALSE;
		fileposn=0;
		gloading=FALSE;
		if (state==FIRSTTIME)
			firstime=TRUE; 
		else 
			firstime=FALSE;
		return LOADING;

	case LOADING:
        //logf(4,"loading %d.tgd",gfileNames[fileposn]);
       
		if (gloadallepg || !(gfileNames[fileposn]<99990000 && !gautoloadepg ))  //a bloody cludge to stop anything loading that isn't a 9999 file if autoloadepg is off.
		{
			TAP_ExtractMjd( gTodayMJDdate+gLoadDay, &tempyear, &tempmonth, &tempday, &tempweekday );
			memset(tempdates,0,9);
			sprintf(tempdates,"%04d%02d%02d",tempyear,tempmonth,tempday);
			memset(sBuff,0,128);
			sprintf(sBuff,"%d.tgd",gfileNames[fileposn]);
		  //  printf("%04d%02d%02d\n",tempyear,tempmonth,tempday);
			intMAXdate=atoi(tempdates);
#if DEBUG		
			logf(4,"Tempdate %d",intMAXdate);
#endif  
			if ((firstime || gfileNames[fileposn]>g_nlastfile) && 
			   (gfileNames[fileposn]<intMAXdate||gfileNames[fileposn]>=99990000||gloadallepg) )
			{
				logf(4,"Checking TGD File: %s",sBuff);
				if(gfileNames[fileposn]==99999997)
				{
					logf(4,"email icon");				
					if (_emailIconRegion==0) _emailIconRegion = TAP_Osd_Create( EBOX_X, EBOX_Y, EBOX_W, EBOX_H, 0, 0 );
					TAP_Osd_Copy( _emailIconRAM, _emailIconRegion, 0, 0, EBOX_W, EBOX_H, 0, 0, TRUE ); //TAP_Osd_Copy( word srcRgnNum, word dstRgnNum, dword srcX, dword srcY, dword w, dword h, dword dstX, dword dstY, bool sprite)
					if (!g_bemailIconexists && !g_bcanclearRAM) 
					{
						if (gVolume==LOW)
						{
							waveram=loadwav(TOPPBIFFLOW_FILE, &wavesize, &realwaverate, &waverate);
						}
						else if (gVolume==HIGH)
						{   
							waveram=loadwav(TOPPBIFF_FILE, &wavesize, &realwaverate, &waverate);
						}
						//else don't play a sound
						if (wavesize) //now if we have some ram setup, play the wave
						{
							logf(4,"wavesize:%d,waveramspot:%x, waverate:%d",wavesize,waveram,waverate);
							g_mailramfree=waveram;
							g_bcanclearRAM=FALSE;
							TAP_PlayPCM( waveram, wavesize, waverate, NULL); //friggin callback crashes sometimes, very dodgy.
							g_tick=TAP_GetTick()+wavesize*100/(realwaverate*4); //x2 for 2channels x2for 16bits/samp
	#if DEBUG
							logf(4,"g_tick %d",g_tick);
	#endif			
							g_bcanclearRAM=TRUE;
						}
					}
					g_bemailIconexists=TRUE;
				}
				else //not a 99999997 file.
				{
					loaded=TRUE;
					if (gautoloadepg || gloadallepg || gfileNames[fileposn]>=99990000) 
					{
						if (LoadEPG(sBuff,gfileNames[fileposn]))
						{
							if (gwatchdog++==10) 
							{
								ShowMessageWin("ERROR:Can't open file",sBuff,150,TRUE);
								gwatchdog=0;
							}
							else
							{
								normalTAP_delay(200);
								fileposn--; //try to reload same file again (fileposn is incremented a bit later before loading again)
							}	
						}
					}	
					//if(gfileNames[fileposn]>=99990000) 
					//{
						//logf(2,"Deleting File: %s",sBuff);
						//TAP_Hdd_Delete( sBuff );
						//g_beit_uploaded=FALSE;
					//}
					//else
					if ((gautoloadepg || gloadallepg) && gfileNames[fileposn]<99990000) //must be a real epg file, and if we have asked to autoload them
					{
						g_beit_uploaded=TRUE;
						gtick_lastloaded=TAP_GetTick();
						if (gfileNames[fileposn]>g_nlastfile) 
						{
							g_nlastsatfile=g_nlastfile;  //saving previous last file for satfile... why, i can't remember!??!
							g_nlastfile=gfileNames[fileposn];
						}
					}
				}
			}

//			if (!firstime && !gloadallepg && gfileNames[fileposn]<g_nlastfile) return NOMORE;
		}
		gloading=FALSE;  //use this as a lock to stop idle routine from doing everything again

		if (++fileposn >=gfilenames_count) //if we have loaded all files (well, last file).
		{
			if (loaded)	showTimersUploaded();
			if (gLCNinTGD) updateINIwithLCN();
	//		lastloadedTGDEPGfile=fileposn-1;
			gtestEvtId=0;
			gbytestransferred=0;
			//g_beit_uploaded=TRUE; //set flag so idle event EIT entry checking can start
			gloadallepg=FALSE;
			gloading=FALSE;
#if DEBUG
			logf(4,"load_TGD_stateMachine returning NOMORE");
#endif			
			return NOMORE;    //flag used to indicate that no more loading is due to happen.
		}
		else
		{
#if DEBUG
			logf(4,"load_TGD_stateMachine returning LOADING");
#endif	
			return LOADING;
		}
		case FINISHED:   //added to shut compiler up a bit, shouldn't get here
		case NOMORE:
			return NOMORE;
	}
	return NOMORE; //shouldn't get here!
}

void ShowEPGFileList( dword selNum )
{
	int totalEntry,i;
	int count=0;
	TYPE_File file;
	unsigned char sBuff[128];
//	unsigned char fileName[128];
	dword fileNames[40];

	if (!_gFullRegion)
	{
		TAP_ExitNormal();
		_gFullRegion = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
	}

	TAP_Win_SetDefaultColor( &g_winOpts );
	TAP_Win_Create( &g_winOpts, _gFullRegion, 70, 100, 180, 280, FALSE, TRUE );
	if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
		TAP_Win_SetTitle( &g_winOpts, "EPG Dateien", 0, FNT_Size_1622 );
	else
		TAP_Win_SetTitle( &g_winOpts, "EPG files", 0, FNT_Size_1622 );


	//ChangeDir(gTGDdirectory); 
	// find all files...
	totalEntry = TAP_Hdd_FindFirst( &file );
	while( totalEntry-- )
	{	
		strcpy( sBuff, file.name );
		strlwr( sBuff );

		if (strstr( sBuff + strlen( sBuff ) - 4, ".tgd" ))
		{
			memset(sBuff,0,128);
			fileNames[count++]=atoi(strncpy(sBuff,file.name,strlen(file.name)-4));
		}

		if( totalEntry==0 || TAP_Hdd_FindNext( &file ) == 0 )
		{
			break;
		}
	}

	if (count!=0)
	{

		bubble_sort(&fileNames[0],count);
		for (i=0;i<count;i++)
		{
			memset(sBuff,0,128);
			sprintf(sBuff,"%d.tgd",fileNames[i]);
			TAP_Win_AddItem( &g_winOpts, sBuff );
		}
	}
	else
	{
		TAP_Win_AddItem( &g_winOpts, "No TGD files");
	}
	TAP_Win_SetSelection( &g_winOpts, selNum );

	g_bepgFileWinShown=TRUE;

	gtestEvtId=0;
	gbytestransferred=0;
}

void erase_current_epg(void)
{
	int channelNum;
	int eventNum;
	int OFFSX=75;
	int OFFSY=40;
	int i,j;	
	TYPE_TapEvent *eventinfo;


	logf(3,"EPG EIT Erase entered");

	if (g_ShowLoadingOSD!=DISOFF)
	{
		if (!_gFullRegion)
		{
			_timersUploadedRegion = TAP_Osd_Create( OFFSX, OFFSY, 420, 100, 0, FALSE );		// create rgn-handle
			OFFSX=0;
			OFFSY=0;
		}
		else
			_timersUploadedRegion=_gFullRegion;

		g_deleteregion=TAP_GetTick();

		if (g_ShowLoadingOSD==LARGE)
		{
			TAP_Osd_FillBox(_timersUploadedRegion, OFFSX, OFFSY, 380, 32, RGB(19,19,19) );	// draw background-box for border x,y,w,h
			TAP_Osd_PutS(_timersUploadedRegion, OFFSX+5, OFFSY+4, 374+OFFSX, " Erasing EIT, please wait..",		// show message x,y,maxX
				RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_LEFT);
		}
		else //must be minimal setting
		{
			TAP_Osd_FillBox(_timersUploadedRegion, OFFSX, OFFSY, 80, 32, RGB(19,19,19) );	// draw background-box for border x,y,w,h
			TAP_Osd_PutS(_timersUploadedRegion, OFFSX+3, OFFSY+4, 77+OFFSX,"EIT Del",		// show message x,y,maxX
				RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_CENTER);
		}
	}
	
	if (exTAPret==0) //if exTAP supported
	{
		exTAP_EPG_DeleteAllEvents();  //use faster method to delete
	}
	else 
	{
		TAP_Channel_GetCurrent( &g_svcType, &channelNum);
		for (j=0;j<g_NumServices;j++)
		{
			g_deleteregion=TAP_GetTick();
			//printf("gothere1\n");
			if (g_IniSvcPtr[j].topfieldinternalchannel!=9999)
			{
				eventinfo = TAP_GetEvent( g_svcType, g_IniSvcPtr[j].topfieldinternalchannel, &eventNum );
				//printf("gothere2\n");
				//TAP_MemInfo(&heapSize, &freeHeapSize, &availHeapSize);
				if (eventNum>0) 
				{
					if (g_loglevel==4)
					{
						TYPE_TapChInfo chInfo;
						TAP_Channel_GetInfo( g_svcType, g_IniSvcPtr[j].topfieldinternalchannel, &chInfo );
						logf(4,"Erase %d events on infile entry:%d Channel:%d LCN:%d",eventNum,j+1,g_IniSvcPtr[j].topfieldinternalchannel,chInfo.logicalChNum);
					}
					for( i=eventNum-1; i>=0; i-- ) 
					{
						//printf("gothere4\n");
						TAP_EPG_DeleteEvent( g_svcType, g_IniSvcPtr[j].topfieldinternalchannel, eventinfo[i].evtId);  //crashes with 1.11 api and older f/w.
						gbSystemProc=TRUE;
						TAP_SystemProc();
						gbSystemProc=FALSE;
					}//for loop
					
					geventnumberarray[g_IniSvcPtr[j].topfieldinternalchannel]=0;
					
				} //if have events
				if( eventinfo )
				{
					TAP_MemFree((unsigned char *)  eventinfo );
					eventinfo = 0;
				}
			}
		}
	 }
     g_beit_uploaded=FALSE;
     g_loadstate=NOMORE;
}

unsigned char *lookupReservtype(TYPE_ReservationType reserve)
{

	switch (reserve){
	case RESERVE_TYPE_Onetime: return "Onetime";
	case RESERVE_TYPE_Everyday: return "Everyday";
	case RESERVE_TYPE_EveryWeekend: return "EveryWeekend";
	case RESERVE_TYPE_Weekly: return "Weekly";
	case RESERVE_TYPE_WeekDay: return "WeekDay";
	case N_ReserveType: return "N_ReserveType";
	}
	return "Huh?";
}

dword addtime(dword date, int add)  //add minutes to the day
{
	word day;
	short hour,min;

	day = (date>>16)&0xffff;
	hour= (date>>8)&0xff;
	min = (date)&0xff;

	min+= add%60;
	if (min<0)
	{
		hour-=1;
		min+=60;
	}
	else if (min>59)
	{
		hour+=1;
		min-=60;
	}

	hour+=add/60;

	if (hour<0){
		day-=1;
		hour+=24;
	} 
	else if (hour>23){
		day+=1;
		hour-=24;
	}
	return ((day<<16)|(hour<<8)|min);
}

void update_nowandnext(int topfieldchannelNum, dword NNnowtime)
{
	int i;	
	word startevent=0;
	dword start,end;//,startnext;

	if( g_eventinfo )
	{
		TAP_MemFree((unsigned char *)  g_eventinfo );
		g_eventinfo = 0;
	}
	
	if (topfieldchannelNum!=9999)
	{
		if (g_scanALLevents || geventnumberarray[topfieldchannelNum]<=2)  //lets sweep thru less epg entries
			startevent=0;
		else
			startevent=geventnumberarray[topfieldchannelNum]-2;
	
		g_eventinfo = TAP_GetEvent( g_svcType, topfieldchannelNum, &g_eventNum );
		
#if DEBUG
		logf(4,"g_eventnum:%d mjd:%x startevent:%d ",g_eventNum,NNnowtime,startevent);
#endif	
		if (g_eventNum) 
		{
			for( i=startevent; i<g_eventNum; i++ )  //scanning through events from current event, or from 0 if g_scanALLevents true.
			{
				start=addtime(g_eventinfo[i].startTime,g_NextUpdateTime); //setup N&N update time to be a bit less than now
				end=addtime(g_eventinfo[i].endTime,g_NextUpdateTime);
#if DEBUG				
				logf(4,"S%x E%x %s",start,end,g_eventinfo[i].eventName);
#endif				
				if (end<NNnowtime && g_eventinfo[i].runningStatus==4) //if end is before now and RS=4, i.e. old program
				{
					//if (i<g_eventNum-1)  //check next event is due to start now too.. if not, then we won't change status yet
					//{
					//	startnext=addtime(g_eventinfo[i+1].startTime,g_NextUpdateTime);
					//	if (startnext == NNnowtime) //if the next program is due to start now, lets set this one to runningstat=1
					//	{
#if DEBUG						
							logf(4,"start%x end%x mjd%x, ",start,end, NNnowtime);  //just changed to less than or equal, as this would never get done because end of one prog is the same minute as start of next
							logf(4,"end is before now and rs==4 ");
#endif							
							TAP_EPG_UpdateEvent( g_svcType, topfieldchannelNum, g_eventinfo[i].evtId, 1 );
					//	}
					//}	
				}
				if(start<=NNnowtime && end>NNnowtime)  //if now between start and end, i.e. current program
				{
					geventnumberarray[topfieldchannelNum]=i; //set the channel event array to this event
					if (g_eventinfo[i].runningStatus==1)  //this is the running status set originally by the tap, needs to be 4 while current,
					{
#if DEBUG					
						logf(4,"start %x, end %x, mjd %x ",start,end, NNnowtime);
						logf(4,"start less now and end greater now and RS==1 ");
#endif						
						if (i>0) TAP_EPG_UpdateEvent( g_svcType, topfieldchannelNum, g_eventinfo[i-1].evtId, 1 ); //set previous event to not running
						TAP_EPG_UpdateEvent( g_svcType, topfieldchannelNum, g_eventinfo[i].evtId, 4 );
						if (g_loglevel==4)
						{
							TYPE_TapChInfo chInfo;
							TAP_Channel_GetInfo( g_svcType, topfieldchannelNum, &chInfo );
							logf(4,"runningstat topfchNm:%d logChNm:%d evtId:%d starttm:0x%x-%02d:%02d dur:%d offs:%d %s", 
							topfieldchannelNum,chInfo.logicalChNum,g_eventinfo[i].evtId, (g_eventinfo[i].startTime>>16)&0xffff,
							(g_eventinfo[i].startTime>>8)&0xff,g_eventinfo[i].startTime&0xff,  
							(g_eventinfo[i].duration>>8)*60+(g_eventinfo[i].duration&0xff), g_NextUpdateTime, g_eventinfo[i].eventName);
						}
						if (i<g_eventNum-1) //accomodate overlapping and buggered events
						{	
							TAP_EPG_UpdateEvent( g_svcType, topfieldchannelNum, g_eventinfo[i+1].evtId, 1 ); //set next to not running
							start=addtime(g_eventinfo[i+1].startTime,g_NextUpdateTime); //setup N&N update time to be a bit less than now
							end=addtime(g_eventinfo[i+1].endTime,g_NextUpdateTime);
							if(( start <= NNnowtime ) && ( end > NNnowtime)) // then we have 2 overlapping events!
							{
#if DEBUG
						logf(4,"OverlapDetect- evt1id:%d evt2id:%d Evt1Start:%x Evt2Start:%x Evt1End:%x Evt2End:%x",
						g_eventinfo[i].evtId,g_eventinfo[i+1].evtId,
						g_eventinfo[i].startTime,g_eventinfo[i+1].startTime,g_eventinfo[i].endTime,g_eventinfo[i+1].endTime);
#endif	
								i++;
							}
						}
						
					}//end if running=1.
#if DEBUG					
					logf(4,"For loop is being breaked");
#endif					
					break; //CAN break for loop, as epg entries are in chronological order, no need to check anymore entries.
				}
			}//for loop
		} //if have events
	} //if channel#!=9999
}//update_nowandnext


void ShowMailOptionsWindow (void)
{
	if (_gFullRegion==0)
	{
		TAP_ExitNormal();      // leaving normal receiver operation, receiver hides its OSDs
		if (_emailOptionsRegion==0)
			_emailOptionsRegion= TAP_Osd_Create (0, 0, 720, 576, 0, FALSE);
	}
	else _emailOptionsRegion=_gFullRegion;
	
	//ChangeDir(gtapDirName);  //change tap directory
	if (g_bOptsWinShown)
	{
		TAP_Win_Delete (&g_winOpts);
	}
	TAP_Win_Create( &g_winOpts, _emailOptionsRegion, 40, 30, 580, 90, FALSE, TRUE );
	TAP_Osd_SetTransparency( _emailOptionsRegion, gemailtransparency);
#if DEBUG		
		printf("gemailtransparency:%d\n",gemailtransparency);
#endif	
	TAP_Win_SetDefaultColor (&g_winOpts);  //fonts are 22pixels high, so the below will make sure window is correct height for entries
	g_bMailOptsWinShown = TRUE;
	TAP_Win_SetTitle (&g_winOpts, "Unread Messages", 0, FNT_Size_1622);
} //ShowMailOptionsWindow

void ShowOptionsWindow (void)
{
	unsigned char sBuff[100]; //,str2[50],str3[60],str4[50];
    unsigned char dststring[5];
    word currentmjd;
    byte hour,minute,secs;
    
	if (!_gFullRegion)
	{
		TAP_ExitNormal();      // leaving normal receiver operation, receiver hides its OSDs
		_gFullRegion= TAP_Osd_Create (0, 0, 720, 576, 0, FALSE);
	}
	//ChangeDir(gtapDirName);  //change tap directory
	if (g_bOptsWinShown)
	{
		TAP_Win_Delete (&g_winOpts);
	}

#define X 190
#define W 720-2*X

	TAP_Win_Create (&g_winOpts, _gFullRegion, X, ((241-NEXTTAP*11)<20)?20:(241-NEXTTAP*11), W, ((NEXTTAP*22)>440)?440:NEXTTAP*22, FALSE, TRUE); //x,y,w,h
	TAP_Win_SetDefaultColor (&g_winOpts);  //fonts are 22pixels high, so the below will make sure window is correct height for entries
	g_bOptsWinShown = TRUE;

	TAP_Win_SetTitle (&g_winOpts, _PROGRAM_NAME_, 0, FNT_Size_1622);
    
    
    TAP_GetTime( &currentmjd, &hour, &minute, &secs );
    if (g_useGMT!=GMTFILE) 
	{
        TAP_SPrint(dststring,"");
		gDstActive=FALSE;  //set to false in case user decides not to use the file any more
		gDstStart=0; 
		gDstEnd=0;
	}		//else if ((currentmjd>gDstStart && currentmjd<gDstEnd) || (currentmjd==gDstStart && hour>=2) || (currentmjd==gDstEnd && hour<2))
	else
	{
		ReadGMTini(); //better re-read the file at this point, as it isn't read if the option is not set when tap starts up
		ChangeDir(gTGDdirectory);  //change to normal logging directory
		if (gDstActive)
		{
			if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
				 TAP_SPrint(dststring,"DST: Ja");
			else
				 TAP_SPrint(dststring,"DST: Yes");
				 
		}
		else
		{
			if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)	
				TAP_SPrint(dststring,"DST:Nein");
			else
				TAP_SPrint(dststring,"DST:No");
		}
    }
	if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
	{
        TAP_Win_AddItem (&g_winOpts, (g_useGMT==GMTFILE)?"GMT:  "GMT_OFFSET_FILE"":(g_useGMT==GMTTOPPY)?"GMT:  Broadcaster":"GMT:  TAP Offset");
		if (g_useGMT==GMTTOPPY)
			sprintf( sBuff, "GMT offset:  %s%02d:%02d %s", (g_tdtUTCoffset<0)?"-":"+", abs(g_tdtUTCoffset/60),abs(g_tdtUTCoffset%60),dststring);
		else
			sprintf( sBuff, "GMT offset:  %s%02d:%02d %s", (g_GMToffst<0)?"-":"+", abs(g_GMToffst/60),abs(g_GMToffst%60),dststring);
        TAP_Win_AddItem (&g_winOpts, sBuff);
		if (g_NextUpdateTime==31)
			TAP_Win_AddItem (&g_winOpts, "Jetzt/Nächste:  Nein");
		else
		{
			sprintf( sBuff, "Jetzt/Nächste:  %02d mins\0", g_NextUpdateTime);
			TAP_Win_AddItem (&g_winOpts, sBuff);
		}
		sprintf( sBuff, "Datei Check Intervall TS weg von:  %02d:%02d\0", (g_freqMin[NTS])/60,(g_freqMin[NTS])%60);
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( sBuff, "Datei Check Intervall TS auf:  %02d:%02d\0", (g_freqMin[TSH])/60,(g_freqMin[TSH])%60);
		TAP_Win_AddItem (&g_winOpts, sBuff);
		TAP_Win_AddItem (&g_winOpts, (gVolume==VOLOFF)?"Email Volume:  OFF":(gVolume==LOW)?"Email Volume:  LOW":"Email Volume:  HIGH");
		TAP_Win_AddItem (&g_winOpts, "Einzelne EPG Datei laden");
		TAP_Win_AddItem (&g_winOpts, "Alle EPG Dateien laden");
		TAP_Win_AddItem (&g_winOpts, "Erzeuge "TIMER_FILE"");
		TAP_Win_AddItem (&g_winOpts, (gautoloadepg)?"EPG Dateien beim laden:  Ja":"EPG Dateien beim laden:  Nein");
		TAP_Win_AddItem (&g_winOpts, (gautodeleteepg)?"EPG Daten beim Start löschen: Ja":"EPG Daten beim Start löschen: Nein");
		if (gReloadHour<24)
               sprintf( sBuff, "Umladenstunde : %02d:00\0", gReloadHour);
        else
               sprintf( sBuff, "Umladenstunde : None\0", gReloadHour);
		TAP_Win_AddItem (&g_winOpts, sBuff);
        sprintf( sBuff, "Load days: %d\0", gLoadDay);
        TAP_Win_AddItem (&g_winOpts, sBuff);   
		TAP_Win_AddItem (&g_winOpts, (g_ShowLoadingOSD==DISOFF)?"Lade Infos Anzeigen: Nein":(g_ShowLoadingOSD==SMALL)?"Lade Infos Anzeigen: Minimal":"Lade Infos Anzeigen: Voll");
		sprintf( sBuff, "Logging Level: %d\0", g_loglevel);
		TAP_Win_AddItem (&g_winOpts, sBuff);
		TAP_Win_AddItem (&g_winOpts, (gEITload)?"Sender EIT: Laden":"Sender EIT: Blockieren");
		sprintf( sBuff, "EPG laden Wartezeit: %ds\0", g_loadtimewait);
		TAP_Win_AddItem (&g_winOpts, sBuff);
		
		sprintf( sBuff,"Set Manual Time at Startup: %s", (g_startupTimeSet)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( sBuff,"Set Manual Time at power fail: %s", (g_powerFailTime)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( sBuff,"Manual Time GMT FIX: %s", (g_manualGMTTime)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( sBuff,"Adjust GMT loading TGD: %s", (g_GMTloadAdjust)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( &sBuff,"Exit Key for Menu: %s\r\n", (g_buseExitKey)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( &sBuff,"LCN TGD load: %s\r\n", (g_bLCNTGDload)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		//TAP_Win_AddItem (&g_winOpts, "Aufnahme TS anzeigen");
		TAP_Win_AddItem (&g_winOpts, (g_LoadIceIni==NOICE)?"ICE INI laden: keine":(g_LoadIceIni==LOADNORMALINI)?"ICE INI laden: Nein":"ICE INI laden: Ja");
		TAP_Win_AddItem( &g_winOpts, (gTedRecordingsfile)?"TEDS Recordings File: Ja":"TEDS Recordings File: Nein");
		TAP_Win_AddItem (&g_winOpts, "Neustart TAP");
		TAP_Win_AddItem (&g_winOpts, "Senderinfos anzeigen");
		TAP_Win_AddItem (&g_winOpts, "IniFile interpretiert anzeigen");
		TAP_Win_AddItem (&g_winOpts, "Logfile anzeigen");
		TAP_Win_AddItem (&g_winOpts, "Ausgang "_PROGRAM_NAME_);
		TAP_Win_AddItem (&g_winOpts, "Folgende TAP");
	}
	else  //use english
	{
        TAP_Win_AddItem (&g_winOpts, (g_useGMT==GMTFILE)?"GMT: "GMT_OFFSET_FILE"":(g_useGMT==GMTTOPPY)?"GMT: Broadcaster":"GMT: TAP Offset");
		if (g_useGMT==GMTTOPPY)
			sprintf( sBuff, "GMT offset: %s%02d:%02d %s", (g_tdtUTCoffset<0)?"-":"+", abs(g_tdtUTCoffset/60),abs(g_tdtUTCoffset%60),dststring);
		else
			sprintf( sBuff, "GMT offset: %s%02d:%02d %s", (g_GMToffst<0)?"-":"+", abs(g_GMToffst/60),abs(g_GMToffst%60),dststring);
  		TAP_Win_AddItem (&g_winOpts, sBuff);
		if (g_NextUpdateTime>=31)
			TAP_Win_AddItem (&g_winOpts, "Now/Next update: NO");
		else
		{
			sprintf( sBuff, "Now/Next update: %02d mins\0", g_NextUpdateTime);
			TAP_Win_AddItem (&g_winOpts, sBuff);
		}
		sprintf( sBuff, "New file check freq TS off: %02d:%02d\0", (g_freqMin[NTS])/60,(g_freqMin[NTS])%60);
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( sBuff, "New file check freq TS on: %02d:%02d\0", (g_freqMin[TSH])/60,(g_freqMin[TSH])%60);
		TAP_Win_AddItem (&g_winOpts, sBuff);
		TAP_Win_AddItem (&g_winOpts, (gVolume==VOLOFF)?"Email Volume: OFF":(gVolume==LOW)?"Email Volume: LOW":"Email Volume: HIGH");
		TAP_Win_AddItem (&g_winOpts, "Load individual EPG files");
		TAP_Win_AddItem (&g_winOpts, "Load ALL EPG files");
		TAP_Win_AddItem (&g_winOpts, "Build "TIMER_FILE" file");
		TAP_Win_AddItem (&g_winOpts, (gautoloadepg)?"AutoLoad EPG: YES":"AutoLoad EPG: NO");
		TAP_Win_AddItem (&g_winOpts, (gautodeleteepg)?"Delete Existing EPG at startup: YES":"Delete Existing EPG at startup: NO");
        if (gReloadHour>=24)
        	TAP_Win_AddItem (&g_winOpts, "Reload Hour: None");   
        else
		{
			sprintf( sBuff, "Reload Hour: %02d:00\0", gReloadHour);
			TAP_Win_AddItem (&g_winOpts, sBuff);
		}
		
        sprintf( sBuff, "Load days: %d\0", gLoadDay);
        TAP_Win_AddItem (&g_winOpts, sBuff);       
		TAP_Win_AddItem (&g_winOpts, (g_ShowLoadingOSD==DISOFF)?"Show Loading Info: NO":(g_ShowLoadingOSD==SMALL)?"Show Loading Info: MINIMAL":"Show Loading Info: FULL");
		
		sprintf( sBuff, "Logging Level: %d\0", g_loglevel);
		TAP_Win_AddItem (&g_winOpts, sBuff);
		TAP_Win_AddItem (&g_winOpts, (gEITload)?"Broadcaster EIT: LOAD":"Broadcaster EIT: BLOCK");
		sprintf( sBuff, "EPG load wait: %ds\0", g_loadtimewait);
		TAP_Win_AddItem (&g_winOpts, sBuff);


		sprintf( sBuff,"Set Manual Time at Startup: %s", (g_startupTimeSet)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( sBuff,"Set Manual Time at power fail: %s", (g_powerFailTime)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( sBuff,"Manual Time GMT FIX: %s", (g_manualGMTTime)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( sBuff,"Adjust GMT loading TGD: %s", (g_GMTloadAdjust)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( &sBuff,"Exit Key for Menu: %s\r\n", (g_buseExitKey)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		sprintf( &sBuff,"LCN TGD load: %s\r\n", (g_bLCNTGDload)?"YES":"NO");
		TAP_Win_AddItem (&g_winOpts, sBuff);
		//TAP_Win_AddItem (&g_winOpts, "Recording TS reader");
		TAP_Win_AddItem (&g_winOpts, (g_LoadIceIni==NOICE)?"Load ICE INI: NONE":(g_LoadIceIni==LOADNORMALINI)?"Load ICE INI: NO":"Load ICE INI: YES");
		TAP_Win_AddItem( &g_winOpts, (gTedRecordingsfile)?"TEDS Recordings File: YES":"TEDS Recordings File: NO");
		TAP_Win_AddItem (&g_winOpts, "Reload TAP");
		TAP_Win_AddItem (&g_winOpts, "Channel service details");
		TAP_Win_AddItem (&g_winOpts, "IniFile Interpreted Viewer");
		TAP_Win_AddItem (&g_winOpts, "LogFile Viewer");
		TAP_Win_AddItem (&g_winOpts, "Exit "_PROGRAM_NAME_);
		TAP_Win_AddItem (&g_winOpts, "Next TAP");
	}

	TAP_Win_SetSelection (&g_winOpts, g_nselected);
} //ShowOptionsWindow

void epgKeyHandler(dword param1)  //used to load a individual file
{
	unsigned char fileName[256];  //used to store filename of selected TSreader output.
	if (param1 == RKEY_Ok) // ok-key confirms the file-entry
	{
		dword filedate=0;
		gepgfileNum = TAP_Win_GetSelection (&g_winOpts);
		memset(fileName,0x0,128);
		strncpy(fileName,g_winOpts.item[gepgfileNum],strlen(g_winOpts.item[gepgfileNum])-4);
		filedate= atoi( fileName );
		//if ((filedate>g_nlastfile)&& (filedate!=99999999))
		//	g_nlastfile=filedate;
		strcpy(fileName,g_winOpts.item[gepgfileNum]);

		//ChangeDir(gTGDdirectory);
		if(g_ShowLoadingOSD!=DISOFF)  //cludge to show the info regardless when doing single loading.
		{
			int temploading=g_ShowLoadingOSD;
			g_ShowLoadingOSD=LARGE;
			LoadEPG((unsigned char *)fileName,filedate);
			showTimersUploaded();
			g_ShowLoadingOSD=temploading;
		}
		else
		{
			LoadEPG((unsigned char *)fileName,filedate);
			showTimersUploaded();
		}
		if(filedate<99990000) 
		{
			gtick_lastloaded=TAP_GetTick();
			g_beit_uploaded=TRUE;
		}
		//g_deleteregion=0;  //don't want region deleted for manual upload, unless person pressed exit already.
		gtestEvtId=0;
		gbytestransferred=0;
	}
	else if (param1 == RKEY_Exit) // exit-key hides the options window
	{
		g_bepgFileWinShown=FALSE;
		HideWindow();
	}
	else if (param1 == RKEY_Menu)
	{
		//ChangeDir(gtapDirName);  //change back to tap directory so we can save settings to correct spot
		HideWindow ();
		ShowOptionsWindow();
	}
	else
	{
		TAP_Win_Action (&g_winOpts, param1); // send all other key's to menu-Win
	}
}

void WinKeyHandler(dword param1)
{
			if (param1 == RKEY_Exit) // ok-key confirms the file-entry
			{
				HideWindow ();
			}
			else if (param1 == RKEY_Menu)
			{
				HideWindow ();
				ShowOptionsWindow();
			}
			else
			{
				TAP_Win_Action (&g_winOpts, param1); // send all other key's to menu-Win
			}
}

int emailIconKeyHandler(dword param1)
{
	if (param1 == RKEY_Exit || param1 == RKEY_Ok)
	{
		if (_emailIconRegion>0) {
			TAP_Osd_Delete(_emailIconRegion);
			_emailIconRegion=0;
		}
		g_bemailIconexists=FALSE;
	}
	if (param1 == RKEY_Ok) // ok-key confirms the file-entry
	{
		g_emailcounter=0;
		ShowMailOptionsWindow();
		LoadEPG("99999997.tgd",99999997);
		//g_bemailIconexists=TRUE;  //used to stop new email from coming while browsing existing
	}
	if (param1 == RKEY_Exit)// ||param1 == RKEY_Ok)
	{
		TAP_Hdd_Delete("99999997.tgd");
		return 0;
	}
	return param1;	
}

void EmailOptsKeyHandler(dword param1)
{
	//static	int setupOSDtransparency;
	if (param1 == RKEY_Ok) // ok-key confirms the file-entry
	{
		//setupOSDtransparency=TAP_GetSystemVar(SYSVAR_OsdAlpha);
		//TAP_SetSystemVar(SYSVAR_OsdAlpha,gemailtransparency);
		ShowEmailText(TAP_Win_GetSelection (&g_winOpts));
	}
	else if (param1 == RKEY_Exit) // 
	{
		g_Mailposition=0;
		HideWindow ();
		//TAP_SetSystemVar(SYSVAR_OsdAlpha,setupOSDtransparency);
	}
	else if (param1 == RKEY_Menu)
	{
		g_Mailposition=0;
		HideWindow ();
		//TAP_SetSystemVar(SYSVAR_OsdAlpha,setupOSDtransparency);
		ShowOptionsWindow();
	}
	else if (param1 == RKEY_Prev || param1 == RKEY_Next)
	{
		(param1==RKEY_Next)?(gemailtransparency=(gemailtransparency+8)%256): //else
		((gemailtransparency<8)?gemailtransparency=255:(gemailtransparency=(gemailtransparency-8)%256));
#if DEBUG		
		printf("gemailtransparency:%d\n",gemailtransparency);
#endif		
		SaveSettings(NOICE);
		TAP_Osd_SetTransparency( _emailOptionsRegion, gemailtransparency);
	}
	else
	{
		g_Mailposition=0;
		TAP_Win_Action (&g_winOpts, param1); // send all other key's to menu-Win
	}
}


dword optsWinKeyHandler(dword param1)
{
	int temp=0;
//	int i;
//	bool autoloadflag=FALSE;
	if (param1 == RKEY_Ok ||param1==RKEY_VolUp ||param1==RKEY_VolDown) // ok-key confirms the g_nselected list-entry
			{
				g_nselected=TAP_Win_GetSelection (&g_winOpts);
				switch (g_nselected)
				{
                case USEGMTFILE: g_useGMT=(param1==RKEY_VolDown)?(g_useGMT=(--g_useGMT%3<0)?GMTTAP:g_useGMT):++g_useGMT%3;; ShowOptionsWindow(); break;
				case GMTOFFSET: 
                if (g_useGMT==GMTTAP)
                {
                     g_GMToffst=(param1==RKEY_VolDown)? (g_GMToffst-=15)%1455 :(g_GMToffst+=15)%1455;
                }
                 ShowOptionsWindow(); 
                 break;
				case NOWNEXTUPD: g_NextUpdateTime=(param1==RKEY_VolDown)? --g_NextUpdateTime%31 :++g_NextUpdateTime%32; ShowOptionsWindow(); break;
				case NEWFILCHQFREQNT: change_chkFreq(param1,NTS); ShowOptionsWindow(); break;
				case NEWFILCHQFREQTS: change_chkFreq(param1,TSH); ShowOptionsWindow(); break;
				case EMAILVOLUM: gVolume=(param1==RKEY_VolDown)? (gVolume=(--gVolume%3<0)?HIGH:gVolume):++gVolume%3; ShowOptionsWindow(); break;
				case LOADINDIV:
					HideOptions();
					ShowEPGFileList(gepgfileNum);
					break; //individual
				case LOADALL:
					HideWindow();
					delete_old_tgd();
					if (!gautoloadepg) 	gloadallepg=TRUE;
					g_loadstate=load_new_tgd(TRUE);//load with "1st time true" so will load all files 
					g_loadedSatOnce=FULL_LOAD;
					//g_beit_uploaded=TRUE; //set flag so idle event EIT entry checking can start
					break;  //all
				case BUILDTSV: HideWindow();createtimerTSVfile(); break;
				case AUTLOADTGD: gautoloadepg=!gautoloadepg; ShowOptionsWindow(); break;
				case DELETESTART: gautodeleteepg=!gautodeleteepg; ShowOptionsWindow(); break;
				case RELOADHOUR: gReloadHour=(param1==RKEY_VolDown)? (gReloadHour=(--gReloadHour%25<0)?24:gReloadHour) :++gReloadHour%25; ShowOptionsWindow(); break;
				case SHOWOSD: g_ShowLoadingOSD=(param1==RKEY_VolDown)?(g_ShowLoadingOSD=(--g_ShowLoadingOSD%3<0)?LARGE:g_ShowLoadingOSD) :++g_ShowLoadingOSD%3; ShowOptionsWindow(); break;
				case LOGLEVL: g_loglevel=(param1==RKEY_VolDown)? (g_loglevel=(--g_loglevel%5<0)?4:g_loglevel) :++g_loglevel%5; ShowOptionsWindow(); break;
				case EITBLOCK: gEITload=!gEITload; logf(4,"EIT%s return:%d",(gEITload)?"LOAD":"BLOCK",TAP_ControlEit(gEITload));  ShowOptionsWindow(); break;
				case EPGLOADWAIT: g_loadtimewait=(param1==RKEY_VolDown)? (g_loadtimewait=(--g_loadtimewait%11<0)?10:g_loadtimewait) :++g_loadtimewait%11; ShowOptionsWindow(); break;
				case LOADNUMDAYS: gLoadDay=(param1==RKEY_VolDown)? (gLoadDay=(--gLoadDay%29<0)?28:gLoadDay) :++gLoadDay%29; ShowOptionsWindow(); break;
                case MANUALSTRT: g_startupTimeSet=!g_startupTimeSet;ShowOptionsWindow(); break;
				case MANUALPRWF: g_powerFailTime=!g_powerFailTime;ShowOptionsWindow(); break;
				case MANUALGMTFX: g_manualGMTTime=!g_manualGMTTime;ShowOptionsWindow(); break;
				case ADJUSTGMTLD: g_GMTloadAdjust=!g_GMTloadAdjust;ShowOptionsWindow(); break;
				case IGNOREEXIT: g_buseExitKey=!g_buseExitKey;ShowOptionsWindow(); break;
				case LCNTGDLOAD: g_bLCNTGDload=!g_bLCNTGDload;ShowOptionsWindow(); break;
				case ICELOAD:
					temp=g_LoadIceIni;
					g_LoadIceIni=(param1==RKEY_VolDown)?(g_LoadIceIni=(--g_LoadIceIni%3<0)?2:g_LoadIceIni) :++g_LoadIceIni%3; 
					SaveSettings(temp);
					temp=g_LoadIceIni;
					LoadSettings(FALSE);
					g_LoadIceIni=temp;
					ShowOptionsWindow(); break;
				case TEDCREATE:gTedRecordingsfile=!gTedRecordingsfile;	ShowOptionsWindow(); break;
				case RELOADTAP: 
					HideWindow();ChangeDirRoot();
					g_btapdirinifile=FALSE;g_brestarttap=TRUE;
					g_bLoadSettings=TRUE; 
					printf("restarted tap\n");
					if (gGmt_updateblock)	*(dword *)gGmt_updateblock=gGmt_updateblockbackup; //restore the blocked GMT update code
					if (gGMTLocation) *(dword *)gGMTLocation=0x00000000; //set to zero so that the exTAP will load broadcaster timestamp again.
					break; //changedir so loading algorithm doesn't detect inifile the same way a tap load would.
				case SERVICDETL: HideOptions();show_service_details(); break;
				case SHOWINIFIL: HideOptions();ShowIniFile(); break;
				case SHOWLOGF: HideOptions();ShowLogFile(); break;
				case EXITTAP:	
					TSRCommander.ExitTAP = TRUE;
					break;
				case NEXTTAP: HideWindow(); SaveSettings(NOICE); return RKEY_Exit;			// cascades exit-key to other TAP's
					break;
				}
				return 0;
			}
			else if (param1 == RKEY_Exit) // exit-key hides the options window
			{
				//ChangeDir(gtapDirName);  //change back to tap directory so we can save settings to correct spot
				HideWindow();
				SaveSettings(NOICE);
				return 0;
			}
			else
			{
				TAP_Win_Action (&g_winOpts, param1); // send all other key's to menu-Win
			}
			return 0;
}

bool getTDTUtcFromExTAP(word *d,byte *h,byte *m)
{
	int count=0;
	while (exTAPret==0 && *d==0 && count<5) //sometimes, within seconds of bootup, TDT stamp hasn't loaded... so have to put a while loop
	{
		count++;
		logf(3,"exTAP_GetUTCTime returned:%d ",exTAP_GetUTCTime(d, h, m));
		logf(3,"TDT_UTC:mjd:%x %02d:%02d",*d,*h,*m);
		if (*d==0) {
			normalTAP_delay(120);
		}
		else return TRUE;
	} //else
	logf(3,"Broadcaster timestamp hasn't arrived, will rerun in a minute...");
	return FALSE;
}

//---------------------------------------  SendToFP --------------------------------
//
bool SendToFP (byte *FPData)
{
  static dword          SendFrontPanelString = 0;
  dword                 i;

  if (SendFrontPanelString == 0)
  {
    //Find the entry point of the 'SendFrontPanelString' procedure
    //The 5000 and MP 'SendFrontPanelString' routines are different at the beginning, therefore we have to
    //find a common point inside of the routine and step back until we reach the “addiu $sp” opcode.
    SendFrontPanelString = FindInstructionSequence ("0080f025 93d90000 3335000f 24040002 0c00111c 00000000",
                                                    "fc0007ff fc00ffff fc00ffff ffffffff fc000000 ffffffff",
                                                     0x80000000, 0x80010000, 0, TRUE);
    if (SendFrontPanelString == 0) return FALSE;
	//else printf("Location of Frontpanelstring:%x\n",SendFrontPanelString);
  }

  //Call the routine, which expects a pointer to the data array.
  //SendFrontPanelString is taking care about the STX/ETX characters
  CallFirmware (SendFrontPanelString, (dword) &FPData [0], 0, 0, 0);
  return TRUE;
}

void setManualTime(dword localdatetime, word day2, byte hour2, byte min2)
{
	tFPSetTime	FPSetTime;
	int count;
	if (day2>0)
	{
		logf(3,"Setting Manual DATE to 0x%x & TIME for emma2 & front panel time: %02d:%02d",(localdatetime>>16)&0xffff,(localdatetime>>8)&0xff,localdatetime&0xff);
		*(word *)0xa3fffffe=(localdatetime>>16)&0xffff;  //set toppy date to broadcaster timestamp
		*(word *)0xb2010408=((BCD(localdatetime>>8)&0xff)<<8)|BCD((localdatetime)&0xff); //set EMMA RTC time to timestamp

		FPSetTime.OpCode        = 0x15; //0x15="set FP time, 5=number of bytes to follow"
		FPSetTime.LocalDateTime = localdatetime; 
		FPSetTime.seconds 		= 0x00;
		
		SendToFP ((byte*) &FPSetTime); //set FrontPanel clock to broadcaster timestamp+g_tdtUTCoffset
		
		
		//logf(4,"setManualTime clock before sendToFP %x",*(dword *)gGMTLocation);
		//SendToFP ("\x10"); //requests front panel time & instantly updates the firmware clock according to firebird.
		//logf(4,"setManualTime clock after sendToFP %x",*(dword *)gGMTLocation);		
		//*(dword *)(gGMTLocation)=(day2<<16)|((char)hour2<<8)|(char)min2;  //and finally patch the GMT location (firebirds GMTbug fix)!
		//logf(4,"setManualTime after we set it %x",*(dword *)gGMTLocation);
	}
}

void PatchGMT_SetManualClock(word exTAPday,byte exTAPhour,byte exTAPmin)
{
	word day;
	byte hour,min,sec;
	dword localdatetime=0;
	//bool powerfailed=FALSE;
	
	//routine mainly patches print GMT location that is 0 for up to 1min after reboot causing all epg to be GMT hours out,
	//but also gets GMT offset //1248=8033eb4c and sets the date & time in manual mode.
	if (gFlashlocation && gClockSetting->Mode!=0 && gGMTLocation) //we have TDT clock from exTAP_GetUTCTime & manual time is set
	{
		int calc;
		TAP_GetTime( &day, &hour, &min, &sec); //time used for front panel etc.
 		logf(3,"Determining GMT offset and manual time is set to 0x%x %02d:%02d:%02d",day, hour,min,sec);
		calc=subtracttimes((day<<16)|(hour<<8)|min, (exTAPday<<16)|((char)exTAPhour<<8)|(char)exTAPmin);//subtract UTC TDT from local realtime clock in minutes
		//now round to the nearest 15mins
		g_tdtUTCoffset=(calc/15)*15;
		if ((calc%15)>7) g_tdtUTCoffset+=15; else if ((calc%15)<-7) g_tdtUTCoffset-=15;
		if (abs(g_tdtUTCoffset-g_GMToffst)==60) //a DST transition has likely occured
		{
			logf(3,"DST transition has possibly happened: TAP/infile offset:%dmins calcd based on broadcast:%dmins",g_GMToffst,g_tdtUTCoffset);
			g_GMToffst=g_tdtUTCoffset;
			
		}
		else if (g_tdtUTCoffset!=g_GMToffst) //if the calculated offset isn't equal to the toppy one
		{
			logf(3,"GMT offsets aren't the same, TAP/infile one:%dmins calcd based on broadcast:%dmins so using TAP one",g_GMToffst,g_tdtUTCoffset);
			//if ((g_GMToffst-g_tdtUTCoffset)==60)
			//{
				//due to power fail bug where Toppy subtracts an hour if DST is set to yes, but current time is not in dst.
			//	localdatetime=addtime((exTAPday<<16)|((char)exTAPhour<<8)|(char)exTAPmin,g_GMToffst);
			//}
			//else 
			if (day<=0xCF0d)//if power failure.
			{
					logf(3,"Power failure detected, using broadcaster GMT timestamp & tap offset");
					gbpowerfailed=TRUE;
					localdatetime=addtime((exTAPday<<16)|((char)exTAPhour<<8)|(char)exTAPmin,g_GMToffst); //may as well use broadcaster timestamp
					gclockSetDSTflag=gClockSetting->DST; //silly toppie sees DST changes from power fail date to non-power fail date, so subtracts an hour from current time
			}
			else
			{
				localdatetime=(day<<16)|(hour<<8)|min; //use the front panel clock because for some reason something is a little wrong
			}
			g_tdtUTCoffset=g_GMToffst; //so err on the side of caution and use the TAP setup one, and continue to use the local panel time
		}
		if (!localdatetime) //use broadcaster GMT and calc'd offset
		{  
			localdatetime=addtime((exTAPday<<16)|((char)exTAPhour<<8)|(char)exTAPmin,g_tdtUTCoffset);
			logf(3,"BroadcasterGMT + calcd g_tdtUTCoffset=%x %02d:%02d",(localdatetime>>16)&0xffff,(localdatetime>>8)&0xff,localdatetime&0xff);
		}
		logf(3,"Setting Flash UTCOffset to:%dmins",g_tdtUTCoffset);
		gClockSetting->UTCOffset=g_tdtUTCoffset; //works for about 5mins, but seems to regularly get changed to 0x0000.. oh well.
//strstr returns pointer to first occurrence of the current LCN in the inifile, or if we have detected a power fail and the option to set is true
		if ((g_startupTimeSet && 
			(strstr(g_manualLCN,g_LCNinfo)||strstr(g_manualLCN,"ALL"))) 
		   || (g_powerFailTime && gbpowerfailed)) 
		{
			setManualTime(localdatetime,exTAPday,exTAPhour,exTAPmin);
			if (!gclockSetDSTflag) gbpowerfailed=FALSE; //don't need to reset time again if this flag isn't set.
		}
		logf(3,"Setting GMTbugfix Time to 0x%x %02d:%02dGMT",exTAPday,(char)exTAPhour,(char)exTAPmin);
		*(dword *)(gGMTLocation)=(exTAPday<<16)|((char)exTAPhour<<8)|(char)exTAPmin;  //and finally patch the GMT location (firebirds GMTbug fix)!
		//*(dword *)(gGMTLocation)=addtime(((day<<16)+(hour<<8)+min+(byte)s/40),-g_tdtUTCoffset);  //subtract UTC offset from local to get GMT (if seconds>40, may as well add one).
		//else essentially we don't touch the clock, i.e. no flashlocationfound for settings or no date found with exTAP.
	}
}



void LoadBootSettings(void)
{
	TYPE_TapChInfo chInfo;
//			TYPE_File	fp;
	int i;
	word exTAPday=0,day;
	byte minute,sec;
	byte hour,exTAPhour,min,exTAPmin;
	dword epg_stick=0;

	int count=0;
	int channelNum;
	char str[0x100];
	dword localdatetime;
	
	//word hhmmtime;
	tFPSetTime	FPSetTime;
	dword pPrintGMT;

	//determine support for different firmwares

	exTAPret = exTAP_Init();
	if (exTAPret) {
		sprintf(str,"contact %s Author FW-%d:%x", __tap_program_name__, *(word*)pSysID, _appl_version);
		ShowMessageWin("Unsupported Platform for exTAP environ, not all features supported",str,300,TRUE);
		//return 0;
	}	
	//printf("ID:%d,appl %x\n",fw_sysid,_appl_version); //gotten from extap library
	GetFWInfo (&SysID, &ApplID, &BIOSAddress, &EventDispatcherAddress, &TAPgp, &FWgp, &TAP_TableAddress, &TAP_IdentAddress); //gotten from firmware.c

	gFlashlocation = FlashInitialize (SysID); //gotten from flash.c

	
	_emailIconRAM = TAP_Osd_Create( EBOX_X, EBOX_Y, EBOX_W, EBOX_H, 0, OSD_Flag_MemRgn );
	TAP_Osd_PutGd( _emailIconRAM, 0 , 0, &_emailGd, TRUE );
	_cogIconRAM1 = TAP_Osd_Create( CBOX_X, CBOX_Y, CBOX_W, CBOX_H, 0, OSD_Flag_MemRgn );
	TAP_Osd_PutGd( _cogIconRAM1, 0 , 0, &_cog1Gd, TRUE );
	_cogIconRAM2 = TAP_Osd_Create( CBOX_X, CBOX_Y, CBOX_W, CBOX_H, 0, OSD_Flag_MemRgn );
	TAP_Osd_PutGd( _cogIconRAM2, 0 , 0, &_cog2Gd, TRUE );
	_cogIconRAM3 = TAP_Osd_Create( CBOX_X, CBOX_Y, CBOX_W, CBOX_H, 0, OSD_Flag_MemRgn );
	TAP_Osd_PutGd( _cogIconRAM3, 0 , 0, &_cog3Gd, TRUE );
	for (i=0;i<1000;i++)  gLCNfound[i].LCNfound=FALSE;
	for (i=0;i<5000;i++)  geventnumberarray[i]=0;
	
	pPrintGMT = FindInstructionSequence ("3c04801e 0c02b6eb 248418c0 8f9884b4 0018c402 0c04539b 3304ffff 8f9e84b4",
									   "ffff0000 fc000000 fc1f0000 ffe00000 ffe007ff fc000000 fc1fffff fc000000",
										0x80100000, 0x80200000, 3, FALSE);
	
	if (pPrintGMT) 
	{
		(dword ) gGMTLocation = FWgp + (short) ((*(dword *) pPrintGMT) & 0x0000ffff);//if can find the location to hack the GMT at bootup, which is zeroed for newer firmwares, hence the EPG being 10hours out
	              //older firmwares it is correct, but exTAP doesn't report TDT tables, so instead uses the same location for GMT
	}			  
		
//			bloadhour=TRUE;  //setting hour to TRUE for beginning (in case tap loaded on the 6th hour).
	TAP_Channel_GetCurrent( &g_svcType, &channelNum );
	TAP_Channel_GetInfo( g_svcType, channelNum, &chInfo );
	g_chNum=channelNum;
	sprintf(g_LCNinfo,"%03d",chInfo.logicalChNum); //make LCN ascii to lookup later to set manual time
	strcpy(g_satname,chInfo.satName);
	if (TAP_Hdd_Exist(INI_FILE))
	{
		g_btapdirinifile=TRUE;
	}
	if (!g_brestarttap && LoadSettings(TRUE)) 
	{
		LoadSettings(FALSE);
	}	
	else
	{	
		LoadSettings(FALSE);//
	}
	logf(3,"EIT%s return:%d",(gEITload)?"LOAD":"BLOCK",TAP_ControlEit(gEITload));

	//printf("ID:%d,appl %x\n",fw_sysid,_appl_version); //gotten from extap library

	logf(3,"FWrevision from extap-%d:%x, from firebirds firmware.c-%d:%x",*(word*)pSysID, _appl_version,SysID,ApplID);
	
	if (gFlashlocation == 0)  //have to do this here after load ini settings otherwise get logf logfile error.
	{
		logf(3,"System not supported or flash not found!\n");
		
	}
	else
	{
		//Get the address of the clock structure (add 4 bytes as the typedef struct doesn't include the CRC)
		gClockSetting = (TYPE_ClockSetup*) (FlashGetBlockStartAddress(7) + 4);  //for 1248 is dump $81b21150
		logf(3,"Flash GMT Offset Detected: %d minutes", gClockSetting->UTCOffset);
	}
	
	g_tdtUTCoffset=g_GMToffst; //set this just in case the broadcaster stuff can't be determined (and tap in "broadcaster" mode).
	// as need an offset to use for loading EPG in the meantime, may as well be the tap one.
	
	//exTAPret if f/w supported
	if ((!getTDTUtcFromExTAP(&exTAPday,&exTAPhour,&exTAPmin) || (g_fileplaying&2)) && gFlashlocation && gClockSetting->Mode!=0) 
	//if didn't get date/time, or is currently playing a file (which buggers the broadcaster timestamp of course, and in manual time.
	{
		TAP_GetTime( &day, &hour, &min, &sec); //time used for front panel etc.
		logf(3,"Setting GMTbugFix at Location (&exTAP returned date=0) %x to %xGMT",gGMTLocation,addtime((day<<16)+(hour<<8)+min+(byte)sec/40,-g_tdtUTCoffset));
		*(dword *)gGMTLocation=addtime((day<<16)+(hour<<8)+min+(byte)sec/40,-g_tdtUTCoffset); //local-gmtoffset if seconds>40secs, will add 1 minute
		g_setTime1min=TRUE;
	}
	else 
	{	
		PatchGMT_SetManualClock(exTAPday,exTAPhour,exTAPmin);  
	}
	gexTAPday=exTAPday; gexTAPhour=exTAPhour; gexTAPmin=exTAPmin;
	
	
	//AUTO time GMT bootup patch (where GMT time is incorrect after bootup so EPG is GMT hours out).
	if (gFlashlocation && gClockSetting->Mode==0 && gGMTLocation) //AUTO time set & flash found ok, so patch GMTLocation to be correct GMT time (GMT bugfix)
	{
		g_tdtUTCoffset=gClockSetting->UTCOffset;
		if (exTAPday!=0) //if we have got a exTAP UTC response-
		{
			logf(3,"Setting AUTOtime GMTbugFix at Location (AUTO &exTAP returned good) %x to %xGMT",gGMTLocation,(exTAPday<<16)|((char)exTAPhour<<8)|(char)exTAPmin);
			*(dword *)gGMTLocation=(exTAPday<<16)|((char)exTAPhour<<8)|(char)exTAPmin; //patch GMT location that has zeros after bootup (firebirds GMTbug fix)
		}
		else //extap didn't return a day, so can still use the TAP ini setup tdtUTCoffset.
		{
			TAP_GetTime( &day, &hour, &min, &sec); //time used for front panel etc.
			//logf(3,"AUTO GMT boot Patch, GetPaneltime %x %02d:%02d:%02d",day, hour,min,sec);
			logf(3,"Setting AUTOtime GMTbugFix at Location (AUTO &exTAP returned date=0) %x to %xGMT",gGMTLocation,addtime((day<<16)+(hour<<8)+min+(byte)sec/40,-g_tdtUTCoffset));
			*(dword *)gGMTLocation=addtime((day<<16)+(hour<<8)+min+(byte)sec/40,-g_tdtUTCoffset); //local-gmtoffset if seconds>40secs, will add 1 minute
		}
	}

	if (!epg_stick) epg_stick = FindInstructionSequence ("0018C600 0018C7C2 5700000F 8FBF0020",
						   "ffffffff ffffffff ffffffff ffffffff",
							0x80110000, 0x80140000, 1, FALSE);   //lets try restricting a bit..
								
	if (!gGmt_updateblock)	
	{	
		gGmt_updateblock=	FindInstructionSequence ("03380018 0000C812 31EF000F 032FC821 00591025 AF828538",
										 "ffffffff ffffffff ffffffff ffffffff ffffffff ffff0000",
										0x80100000, 0x80130000, 5, FALSE);	
		if (gGmt_updateblock)	gGmt_updateblockbackup=*(dword *)gGmt_updateblock;	
	}

	if (epg_stick)
	{
		logf(3,"making epg guide stick at Location:%x",epg_stick);
		*(dword *)(epg_stick) = 0x24180001;  //setting 1248 0x80120994 EPG hack to allow channel browsing for stock epg without channel surfing!
	}
	
	if (gGmt_updateblock && gFlashlocation && gClockSetting->Mode!=0 && g_manualGMTTime) //if manual time is set and we found location to block, and INIoption is set
	{
			logf(3,"Setting NOP to GMT update code, orig instruction 0x%x",(dword) gGmt_updateblockbackup);
			*(dword *)gGmt_updateblock=0x00000000; //setting a NOP to the location that normally regularly updates the GMT 1248=af8284c4
			gGMTPatched=TRUE;
	}
		
} //LoadBootSettings(void)


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	static dword tick60_1=0,tick60_2=0,tick_freqmin=0,tick_Load_state=0,tickUpdateClock=0; //60second counter & newfilecheck counter
	static int numberoftimers=0;
	static bool bloadingbroadcasterEIT=FALSE;
	static int channelNum,tvsvc,radiosvc;//i,eventdelta,freqHour
	static word mjd;//,year;
	static dword nowtime;
	static byte hour,min,sec;//, month, day, weekDay;
	static dword dwState, dwSubState;
	static bool displayedyellow=FALSE;
	static word _yellowRgn=0;
	word updatevar=0;

	if (gbSystemProc) return param1;
	
	if( event == EVT_KEY )
	{
		TAP_GetState (&dwState, &dwSubState);
		//printf("state:%d,substate:%d\n",dwState,dwSubState);
		//------------------------------------options window------------------------------------------------
		//
		if (g_bOptsWinShown) // handling keys when options window is shown
		{
			return optsWinKeyHandler(param1);
		}
		else if (g_bepgFileWinShown) // handling keys when list of tgd files is shown
		{
			epgKeyHandler(param1);
			return 0;
		}//epgfilelist
		else if (g_bMailOptsWinShown)
		{
			EmailOptsKeyHandler(param1);
			return 0;
		}
		else if (g_bSvcWinShown||g_bIniWinShown||g_bLogWinShown) // handling keys when Service Details is shown
		{
			WinKeyHandler(param1);
			return 0;
		} //timerwin
		else if (g_bemailIconexists && dwState==STATE_Normal &&		// are we in normal-state
			dwSubState==SUBSTATE_Normal)
		{
			return emailIconKeyHandler(param1);
		}
		else if (param1 == RKEY_Yellow && dwState==STATE_Menu && dwSubState==SUBSTATE_TimeSettingMenu)
		{
			word day2=0;
			byte hour2,min2;
			dword localdatetime;
			logf(4,"Setting Manual Time");
			if (gGmt_updateblock) *(dword *)gGmt_updateblock=gGmt_updateblockbackup; //restore nopped location so updates from broadcaster work
			if (gGMTLocation) *(dword *)gGMTLocation=0x00000000; //set to zero so that the exTAP will load broadcaster timestamp again.
			if (gFlashlocation) gClockSetting->Mode=1; //set to manual time
			if (getTDTUtcFromExTAP(&day2,&hour2,&min2))
			{
				localdatetime=addtime((day2<<16)|((char)hour2<<8)|(char)min2,g_tdtUTCoffset);
				setManualTime(localdatetime,day2,hour2,min2);
				TAP_GenerateEvent(EVT_KEY,RKEY_Menu,RKEY_Menu); //refresh view as toppy doesn't actively update
				TAP_GenerateEvent(EVT_KEY,RKEY_Ok,RKEY_Ok);
			}	
		}
		else if ((dwState==STATE_Menu && dwSubState==SUBSTATE_TimeSettingMenu) || (_yellowRgn && dwSubState!=SUBSTATE_TimeSettingMenu)){  
		//annoying I have to put some of this code in as arrow movement (or any keys really) 
		//in the time setting screen wipes the OSD,
		//this code will ensure the idle even draws the yellow button stuff each time a key is pressed, or the region is deleted when leaving the menu.
			TAP_Osd_Delete( _yellowRgn );
			_yellowRgn=0;
		}
		else if (param1 == RKEY_Exit&&g_buseExitKey && g_loadstate == NOMORE && dwState==STATE_Normal &&		// are we in normal-state
			dwSubState==SUBSTATE_Normal && !OsdActive()) // exit-key shows the options window when nothing on screen& normal state
		{
			g_nselected=NEXTTAP;
			//ChangeDir(gtapDirName); //just in case
			LoadSettings(FALSE);   
			ShowOptionsWindow();
			return 0;
		}
	}//evtkey

	if ( event == EVT_IDLE )
	{
		if (g_bLoadSettings) //g_bLoadSettings is set at boot time, so this runs once, designed to take as much sting out of bootup time as possible
		{
			g_bLoadSettings=FALSE;  //no need to load settings again
			LoadBootSettings();
			if (gautoloadepg)
			{			
				logf(3,"AutoLoad is ON, deleting/adding files");
				delete_old_tgd(); //always delete old
				if (gautodeleteepg)	erase_current_epg(); //erase current epg 
				g_loadstate=load_new_tgd(TRUE); // load all up, true=loadall.
				g_loadedSatOnce=FULL_LOAD;
				g_bchannelchange=FALSE;//may as well not try to scan data immediately for running status.
			}
			tick_Load_state=TAP_GetTick();
			tick60_1=tick_Load_state;//set tick time.
			tick60_2=tick_Load_state-4500;//tick for ram display and time reset after playback or power fail etc
			tick_freqmin=tick_Load_state;	//set file reload time
		} //if (g_bLoadSettings) 
		
		
		TSRCommanderWork();
		
		nowtime=TAP_GetTick();
		TAP_GetTime( &mjd, &hour, &min, &sec); //time	
		
		TAP_GetState (&dwState, &dwSubState);
		
		if (!_yellowRgn && dwState==STATE_Menu && dwSubState==SUBSTATE_TimeSettingMenu){
			if (!_yellowRgn) _yellowRgn = TAP_Osd_Create( YBOX_X, YBOX_Y, YBOX_W, YBOX_H, 0, 0 );
			TAP_Osd_PutGd( _yellowRgn, 0 , 0, &_yellowbuttonGd, TRUE );
			TAP_Osd_PutStringAf1419( 0, YBOX_X+35, YBOX_Y-1, -1, "Set Manual Time", COLOR_White, COLOR_None );		//Print to bar
		}
		
		if (nowtime>tick60_2+6000)  //every 60ish secs display ram and check to set the clock if it hasn't been done from bootup/reload already
		{
			dword heapSize, freeHeapSize, availHeapSize;
			word exTAPday=0;
			byte exTAPhour,exTAPmin;
			tick60_2=nowtime;
			TAP_MemInfo(&heapSize, &freeHeapSize, &availHeapSize);
			
			if (gclockSetDSTflag)//silly toppie sees DST changes from power fail date to non-power fail date, so subtracts an hour from current time if in non-DST time
			{
				gclockSetDSTflag=0;
				g_setTime1min=TRUE; //reset the time again.
				tick60_2-=3000; //make it only 30secs to try again on fail
				gexTAPmin-=1;  //take a minute off gexTAPmin so the code below will instantly reset time(again), rather than see the same minute.
				logf(4,"Resetting time due to clock DST option set to on at time of power fail");
			}
			
			logf(4,"heapSize:%d, freeHeapSize:%d, availHeapSize:%d",heapSize, freeHeapSize, availHeapSize);
			if (g_setTime1min) 
			{
				if (!getTDTUtcFromExTAP(&exTAPday,&exTAPhour,&exTAPmin) || g_fileplaying&2) //if didn't get date/time, or is currently playing a file (which buggers the broadcaster timestamp of course
				{
					g_setTime1min=TRUE;
					//tick60_2-=3000; //lets try setting every 30secs
				}
				else if ((gexTAPday!=exTAPday) || (gexTAPhour!=exTAPhour) || (gexTAPmin!=exTAPmin)) //clock has now updated since file playback finished
				{
					g_setTime1min=FALSE;
					PatchGMT_SetManualClock(exTAPday,exTAPhour,exTAPmin);
					if (gbpowerfailed) gbpowerfailed=FALSE;  //have to do this or else time won't check channel for future time updates after the power fail fix.
				}	
				gexTAPday=exTAPday; gexTAPhour=exTAPhour; gexTAPmin=exTAPmin;
			}
		}
		
		if (sec<=9 && nowtime>=tickUpdateClock+5100 && gFlashlocation && gClockSetting->Mode!=0 && gGMTLocation && g_manualGMTTime) //every 60ish secs set date.
		{	
			int offset;
			//do our once every 60secs GMT clock update if in manual time
			tickUpdateClock=nowtime;
			if (g_useGMT==GMTTOPPY) //if offset is set to TOPPY, use the tdt calculated (or flash obtained) offset.
				offset=g_tdtUTCoffset;
			else
				offset=g_GMToffst; //else use the TAP set offset, such as offset.ini or the menu set TAP offset.
			//printf("GMTPatch:%x gGmt_updateblock:%x ",	gGMTPatched,gGmt_updateblock);
			if (!gGMTPatched && gGmt_updateblock) //if we have an address and not already patched and gClockSetting->Mode!=0 i.e. manual time & inifile set
			{
				logf(4,"Must have changed from AUTO to manual time, NOP GMT update location");
				//gGmt_updateblockbackup=*(dword *)gGmt_updateblock;
				*(dword *)gGmt_updateblock=0x00000000;//setting to NOP
				gGMTPatched=TRUE;
				gClockSetting->UTCOffset=offset;
			}
			*(dword *)gGMTLocation=addtime(((mjd<<16)+(hour<<8)+min),-offset);  //subtract UTC offset from local to get GMT
			logf(4,"GMTbugfix clock update %x %02d:%02dGMT",(*(dword *)gGMTLocation>>16)&0xffff,(*(dword *)gGMTLocation>>8)&0xff,(*(dword *)gGMTLocation)&0xff);
			
			//and check the flash still has the offset set	
		} 
		else if (gGMTPatched && gFlashlocation && gClockSetting->Mode==0) //if user went to auto & code is patched, better restore orig code
		{
				logf(4,"Restoring NOPPED location back to normal GMT update code");
				gGMTPatched=FALSE;
				*(dword *)gGmt_updateblock=gGmt_updateblockbackup; //restoring the location that normally regularly updates the GMT  af8284c4
		}
		

		if (!gloading && g_loadstate!=NOMORE && (nowtime>=tick_Load_state+g_loadtimewait*100))  //leave g_loadtimewait secs between load
		{	
			g_loadstate=load_TGD_stateMachine(g_loadstate);
#if DEBUG
			logf(4,"g_loadstate=%d",g_loadstate);
#endif
			tick_Load_state=nowtime;
			//g_beit_uploaded=TRUE; //set flag so idle event EIT entry checking can start
		}
		
		if (nowtime%500<=1)  //reduce load on toppy by only doing this stuff once every 5sec (or so!).
		{
			if (g_bcanclearRAM && nowtime>g_tick)
			{
					const unsigned char data[4] = {0x00,0x00,0x00,0x00};
					g_bcanclearRAM=FALSE;
					logf(4,"callback found, ram location %x cleared",g_mailramfree);
					TAP_MemFree(g_mailramfree);
					TAP_PlayPCM( (void *)data, 4, FREQ_48K, NULL );  //cludge to get toppy to be back in 48K mode.
			}
			if (g_bcancloseLOG)
			{
				TAP_Hdd_Fclose(gLogFPointer);
				//*gLogFPointer=0;
				g_bcancloseLOG=FALSE;
#if DEBUG
				printf("LF_Closed");
#endif
			}
			
			TAP_GetState (&dwState, &dwSubState);
			TAP_Channel_GetTotalNum( &tvsvc,&radiosvc );
			if ((g_total_tvSvc!=tvsvc || g_total_radSvc!=radiosvc)  && dwState==STATE_Normal &&		// are we in normal-state
				dwSubState==SUBSTATE_Normal) //person has rescanned or deleted channels && state=normal
			{
				logf(2,"Channel changes detected, doing a reload of "INI_FILE" settings");
				//ChangeDir(gtapDirName); 
				LoadSettings(FALSE);  //have to rescan
			} 
			if (TAP_Timer_GetTotalNum()!=numberoftimers)  //if number of timers have changed, create csv file.
			{
				logf(3,"Number of timers changed, saving "TIMER_FILE" file");
				numberoftimers=TAP_Timer_GetTotalNum();
				createtimerTSVfile();
			}
			TAP_Channel_GetCurrent( &g_svcType, &channelNum );   //test for channel change.
			if (channelNum!=g_chNum)
			{
				TYPE_TapChInfo chInfo;
				g_chNum=channelNum;
				g_bchannelchange=TRUE;
				TAP_Channel_GetInfo( g_svcType, channelNum, &chInfo );

				if (g_loadstate==NOMORE && g_loadedSatOnce!=NO_MORE && strcmp(g_satname,chInfo.satName)!=0)//if they aren't equal and we have only loaded stuff once
				{
#if DEBUG
logf(4,"loadstate is nomore, loadedsatonce is not nomore, and satellite has changed");
#endif 				
					strcpy(g_satname,chInfo.satName);   //make our new satname
					if (g_loadedSatOnce==FULL_LOAD) 
					{
#if DEBUG
logf(4,"g_loadedSatOnce=FULLLOAD");
#endif 						
						g_loadstate=load_new_tgd(TRUE);//load with "1st time true" so will load all files 
					}
					else if (g_loadedSatOnce==NEW_LOAD)
					{
#if DEBUG
logf(4,"g_loadedSatOnce=NEWLOAD, g_nlastfile=%d g_nlastsatfile=%d",g_nlastfile,g_nlastsatfile);
#endif 						
						g_nlastfile=g_nlastsatfile;
						g_loadstate=load_new_tgd(FALSE);//load will only load new file.						
					}
					g_loadedSatOnce=NO_MORE; //set g_loadedSatOnce to no_more once channel change has happened
				}
			}



			
			if (g_deleteregion && !g_bOptsWinShown && !g_bepgFileWinShown && dwState == STATE_Normal &&		// are we in normal-state
				dwSubState==SUBSTATE_Normal && nowtime>g_deleteregion+450)  //handle OSD better, remove osd after 4.5secs.
			{
				g_deleteregion=0;
				if (_gFullRegion>0)
				{
					TAP_Osd_Delete (_gFullRegion);
					if (_timersUploadedRegion==_gFullRegion) _timersUploadedRegion=0;
				}
				if (_timersUploadedRegion>0)
					TAP_Osd_Delete (_timersUploadedRegion);
				if (_rotateUploadedRegion>0)
					TAP_Osd_Delete (_rotateUploadedRegion);
				_rotateUploadedRegion=0;
				_gFullRegion=0;
				_timersUploadedRegion=0;
				//TAP_EnterNormal();
			}
			
			
			
			if (g_beit_uploaded && g_loadstate==NOMORE && g_NextUpdateTime!=31) //if we have uploaded anything, and currently not in a load state, and user agrees to update N&N
			{
                //printf("uploadedsomthing\n");
				TAP_GetTime( &mjd, &hour, &min, &sec); //time
				if (addtime((mjd<<16)|(hour<<8)|min,-5-g_GMToffst)>gTGDlastEITStartTime) //if current time-5 >last starting EIT entry
				{
                    logf(4,"current time 0x%x-%02d:%02d -5 >last starting entry",mjd,hour,min);
					//printf("gTGDlastEITStartTime:%x nowish:%x\n",gTGDlastEITStartTime,addtime((mjd<<16)|(hour<<8)|min,-5-g_GMToffst*15));
					if (!bloadingbroadcasterEIT) //if we aren't already loading broadcasterEIT
					{
						bloadingbroadcasterEIT=TRUE;
						logf(4,"EIT%s return:%d",(bloadingbroadcasterEIT)?"LOAD":"BLOCK",TAP_ControlEit(bloadingbroadcasterEIT));
                        //g_beit_uploaded=FALSE;
					}
				}
				else if (bloadingbroadcasterEIT)  //else (current time < last starting EIT, so if broadcaster loaded)
				{
                    logf(4,"current time 0x%x-%02d:%02d less than last EIT",mjd,hour,min);
					bloadingbroadcasterEIT=FALSE;
					if (!gEITload)//if end user has setup EIT to load, then don't want to block below:
						logf(4,"EIT%s return:%d",(bloadingbroadcasterEIT)?"LOAD":"BLOCK",TAP_ControlEit(bloadingbroadcasterEIT));
				}

				if (g_bchannelchange && nowtime>=gtick_lastloaded+6000)  //update N&N for current channel when changing channels, but wait 60secs after last loaded
				{
#if DEBUG
					logf(4,"Channel Change, nowtime %x, tick_lastloaded %x",nowtime,gtick_lastloaded);
#endif
					//TAP_GetTime( &mjd, &hour, &min, &sec); //time used for
					tick60_1=nowtime;
					g_bchannelchange=FALSE;
					update_nowandnext(g_chNum,(mjd<<16|hour<<8|min));  //update now and next for current channel
				} //channelchange/etc/evt stuff
				else if (nowtime>=tick60_1+6000)// && nowtime>=gtick_lastloaded+9000)  //every 60ish secs check whether to update all the N&Ns.
				{
					int i;
					tick60_1=nowtime;
					for (i=0;i<g_NumServices;i++)
					{
						update_nowandnext(g_IniSvcPtr[i].topfieldinternalchannel,(mjd<<16|hour<<8|min));
					}
				} //channelchange/etc/evt stuff
			} //if we have uploaded anything
			
			g_fileplaying= createRecPlaybackFile(TAP_GetSystemVar(SYSVAR_Timeshift));  //check whether any playback or recording
			
			//printf("State:%d Substate:%d\n",dwState,dwSubState); //i see the screensaver for mediamanager changes substate to 0
			if ( g_loadstate==NOMORE && dwState == STATE_Normal &&	// are we in normal-state
				(dwSubState == SUBSTATE_Normal ||dwSubState==SUBSTATE_MainMenu) && nowtime>=tick_freqmin+g_freqMin[(g_fileplaying>0)?1:0]*100*60) //&& !OsdActive())   //some other taps do state/substate 0/0 when they kick in.
			{
				//bloadhour=FALSE; //first time it hits routine, won't let it do it again.
				//ChangeDir(gTGDdirectory);
				logf(3,"Regular file delete and check-");
				delete_old_tgd();
				g_loadstate=load_new_tgd(FALSE); //FALSE=only load new files
				if (g_loadedSatOnce!=FULL_LOAD) //ok, channel change has not happened yet, so keep the full_load option
					g_loadedSatOnce=NEW_LOAD;
				//ChangeDir(gtapDirName);
				tick_freqmin=nowtime;
			}
			//}
			//else  //hour is anything else
			//	bloadhour=TRUE;


//			TAP_GetTime( &mjd, &hour, &min, &sec); //time
			if (gReloadHour==hour) //menu setup for reload (will only do reload when playback not happening).
			{
				if (g_bReloaded==FALSE && dwState == STATE_Normal &&		// are we in normal-state
					dwSubState == SUBSTATE_Normal )//&& !OsdActive())  // and nothing on screen
				{
					g_bReloaded=TRUE; //first time it hits routine, won't let it do it again.
					g_brestarttap=TRUE;
					g_bLoadSettings=TRUE; 
					if (gGmt_updateblock) *(dword *)gGmt_updateblock=gGmt_updateblockbackup; //restore the blocked GMT update code
					if (gGMTLocation) *(dword *)gGMTLocation=0x00000000; //set to zero so that the exTAP will load broadcaster timestamp again.
					logf(3,"Daily TAP Reload about to start-");
				}
			}
			else  //hour is anything else, so reset flag for next reload.
				g_bReloaded=FALSE;
		}//reduce load on toppy by only doing this once every xticks (or so!).
	}//idle event
	return param1;	
} // TAP_EventHandler( word event, dword param1, dword param2 )


int TAP_Main(void)
{
	int tvsvc,radsvc;
	//char str[0x100];
	TAP_Channel_GetTotalNum( &tvsvc, &radsvc );
	if (tvsvc==0) return 0;   //bail if no toppy tv channels (factory reset would cause this for example).
	
	printf("EITBLOCK return:%d\n",TAP_ControlEit(FALSE));  //try to stop N&N from broadcasters ASAP.
	
	//*(dword *)0x8010d668 = 0x00000000;  //orig af8284c4  //appears to be regular updater.
	//*(dword *)0x80117d48 = 0xaf9584c4;  //doesn't appear to put 0x into gmt timestamp location..  orig 0xaf9584c4
	//*(dword *)0x80117ebc = 0xaf8284c4;  //when in auto time, and viewing menu, offset adjusts this time in menu. orig 0xaf8284c4
	//*(word *)(0x81B2114c+4) = 0x0258;
	//*(byte *)(0x81B2114c+8) = 0xe0;   //0xe1 is manual time, 0xe0 is auto time.
	//*(dword *)0x08010d6c8= 0xaf8284c8;    //was af8284c8	sw	$2,-31544($28) //also regularly updated, this is the 2nd timestamp it checks against in auto.

	//dword test=0xffffffff;
	//printf("test:%x,test+5:%x,if(6)>test+5,%s",test,test+5,(6>test+5)?"yes":"no");   yep, as suspected, a dword is unsigned, result
	// is test+5=4, therefore 6>test+5 is true.
	
	
	g_IniSvcPtr=0;
	if ( TAP_GetTick() > 8000 && !OsdActive())
	{
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			ShowMessageWin("wird gestartet ",_PROGRAM_NAME_,100,FALSE);	// show user TAP has started if manually started
		else
			ShowMessageWin("starting ",_PROGRAM_NAME_,100,FALSE);	// show user TAP has started if manually started
	}

	TSRCommanderInit( 0, TRUE );        // setup the values for TSRCommander
	
	//g_bLoadSettings=TRUE; //going to try this, as some have complained can't load multiple TAPs.
	//..ahh, read on the forums it is a changedir from TAP_Main that causes this issue..
	//so I will wait till an idle period to access the HDD to load the settings (since this uses changedir also).

	return 1; //then OK to keep going, so return 1
}
