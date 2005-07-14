/**********************************************************************/
/* Name:     timer_extend.c                                           */
/* For:      Topfield TF5000 PVR                                      */
/* Author:   Tonymy01  tony_h@bigpond.net.au                          */
/*                                                                    */
/* Descr.:   Extends the start and end times of your Timers!!
             EXIT key brings up the menu, Vol+/Vol- keys change start
             and end times (you may have to hit OK to select start/end before
             using the vol keys).   The menu should be self explanatory.
             It also repairs timers set in the past or future (normally caused by corrupted GMT time from a broadcaster).

             V2.1 Introduces first "Favourites" and "Timer" text stings
             so that gives user confidence it has located the correct
             memory location.
             V2.2 Fixes a problem with one dword of the signature being
             different between some peoples players.  I also sped up the
             interface a little
             V2.3 Moved the timer modification signature, as this got erased on
             daily events
             V2.4 Fixed "exit key only with no OSD" issue.
             V2.5 Fixed "P" or "R" events done from EPG.
             V3.0 Stopped any overlap of entries.
             V3.1 had an "=" instead of "==" which caused start timer to keep getting modified
             V4.0 Sort Timer Entries by date/time from exit-popup menu or when leaving recordings list
             V5.0 Added Timer information in the timer screen infobar
             V5.1 Fixed almost all synch problems between timer filename and timer scrolling (99% foolproof now).
             V6.0 Added feature that will check for "future timers", i.e, timers that have been shuffled into the future
                  due to a bad broadcaster timestamp.  It checks once a minute, and requires turning on/off from
                  the EXIT menu ("Future timers being checked" or "Futures NOT checked" are the two menu choices).
             V6.1 Adds a counter in timer_extend.ini to count the number of adjusted timers that have been done
                  (will count up to 65535 timers changes).
             V6.2 Adds a menu option to allow rec in EPG functionality.
             V6.3 Fixes a logic problem with weekday timers on Friday (kept trying to fix timer once it moved to Monday).
                  Stops future timer fixer trying to edit future timers while in the timer menu.
             V6.4 Fixes "frozen text" when editing or adding new timer entries with Satellite version
             V6.5 Uses new API for more features, which means no chance of timer conflicts for extending now,
                  stopped any issues with "backup" bytes which unfortunately also means can't "restore to normal"
                  anymore (was buggy anyway if the Toppy shifted the timers when the time was re-adjusted, then the backup
                  bytes were corrupted).
                  First timer confirmation is now done against the API so there doesn't need to be the initial "text" that
                  pops up when starting.
             V6.6 Added manual sort option
             V6.7 Added timer end info that gets corrupted by the API to each modded timer.
             V7.0 Fixed starting issue where there are no timers, added "past" timer support too, added date to timerlist view
                  made timerlist filename not overlap box anymore.
             V7.1 Changed manual startup to bring up menu, fixed "losing chunk" on the menu.
             V7.2 Oops, mucked the startup up a little (it *always* brought up the menu, now only after 40secs after startup)
             V7.3 23/09/04  Fixed a logic problem with determining conflicting timer (had 0xFFFF, should have been 0xFFF).
             V7.4 4/11/04 German/English version, got rid of searching for a signature that seems to change under certain firmwares under
                  certain circumstances.
             V7.4a 16/11/04 Fixes to german language.
             V7.5 18/11/04 Fixes exit from filename entry, also up/down/left/right keys in filename entry (did used to confuse
                  algorithm and end up with wrong entry listed with newer firmware).
             V7.6 19/11/04 Fixed code error in weekday timer handling (had "=" instead of "==" that may have caused weekday timer
                  correction issues).   Stopped 60sec timer checking if have any OSD (as a correction would erase the screen when
                  it displays the warning text).
             V7.7 29/11/04 Added infinity toggle so you can postpone timers by toggling with the blue key.
             V7.8 09/12/04 Changed English menu around a bit based on Peter G's suggestions.   Added tuner toggle option
             V7.9 11/12/04 Added break out of while loop (I think the Topfield timer conflict code mucks up), added default tuner
                  option that is called when doing a timer extend.
             V7.10 11/12/04 7hours later ;-)  I made the tuner number code alter the default tuner when sorting now, with both auto
                   and manual sort.  I made the code a little more efficient (had a bit of code repeated 5 or 6 times throughout the firmware
                   and consolidated a bit).   It also changes the tuner# when extending (in case people like extending but don't like sorting).
             V7.11 27/12/04 I backed up another word of data that the Topfield timers appear to use with new firmware, that correlates with
                   the service ID.   Without backing up this information, the timer editing API would leave incorrect information in those
                   bytes, and indeterminate timer behaviour (some worked, some didn't).
                   I made the subtending information -# or -+ as the standard Topfield keyboard didn't support the underscore character. bit still
                   support the _ char.  This will work better with Andy's keyboard enhancments to his PBK tap.
             V7.12 06/01/05 Fixed any possible conflict issue with the timer creation at startup to locate timers.  Compiled with old April API 1.0 for
                   people who haven't upgraded to Dec04 or greater firmwares.
             V7.13 22/01/05 Fixed bug where bringing up the tap menu when there was the progressbar on the screen would crash.
                   Fixed bug where multiple timer conflict when extending would cause time to get moved to a silly time, simply ignored that stretch if this was the
                   case.   Added TSR_Commander support.   Stopped tap menu popping up over OSD, same with tap messages (will stop corrupting other screens).
            V7.14  26/01/05 timestamp to printf.
            V8.0   05/02/05 Fixed string buffer sizes so we should no longer get any filenames etc to cause any crashes.   Changed to be structure based
                   Finally!!  Have one_time timer restoration!   Added a don't change option for the tuner.  Removed EPG time mods (topfield displayed a yellow
                   timer highlight with old f/ws, but does nothing with new).
                   Made tuner number update once a minute if you choose to do the update.
            V8.1   06/02/05 Hmm, structure dword declares look like they have to be on dword boundaries, hence, the result
                   was the structure was 2 bytes too big, and it thus wrote over the start of every timer after the 1st (turning them to one time tuner1 timers!)
                   Other problems happenned, specifically the sort too, anyway, now hopefull it is ok now with splitting ONID to H/L words.
            V8.2   16/02/05 Saves one time timers to HDD each time it discovers a new one, so can restore after a power failure/etc.
                   Fixes a bug that the tap has had a long time where it had hour>24 rather than hour>23 for addtime routine..
                   Deletes any single timer from backup that is even opened in the editor (but redetects it as soon as the timer window is quit).
                   Re-scans all timers if any REC key is pressed (and no extra timer is detected) in EPG, or the OK is pressed at any time
                   in the EPG view.  This should avoid having the tap restore any timers that are manually altered in time/channel/whatever.
            V8.3   25/03/05  Fixed an oops where I didn't re-set a flag and one time restore thus only worked for one item (if that!).
                   Made timer repair only work if currentdate > 20/03/05.   Thus if a power failure or whatever has occurred and the time hasn't had a chance
                   to get reset, the TAP won't stupidly move all the timers back to the power fail time.
                   Made TAP not startup if you have no channels setup (such as after a factory reset), to avoid any popups from confusing the channel scan screen etc.
                   Fixed single timer code where if not enough ram for backups, would still attempt to do single timer backups when extending/norming (now won't).
                   Fixed regular timer check code where todaydate may have only been obtained if a timer existed (todaydate used in onetime restore).
            V8.3a  Modded for 0x84 byte timer records in certain Topfields.
            V8.4   08/04/05 Fixed references to strnlen that should be strlen (fixed .rec text).   Delete one time backup file after number of channels varied (new scanned
                   or deleted).  Made delay between one-time re-building 3secs as anything less seems they get lost on reboot.
            V8.5   28/05/05  Added Finnish language, made some changes for Finish & German displays, fixed menu draw memory leak.  Removed signature.ini stuff.
                   added EPG_uploader support for deleting backup one time timers.  Changed timer clash code in show_timer_error to comply with topfields new claimed
                   options (although from what I have seen, still isn't quite right, it won't wreck anything thankfully).
            V8.6   02/06/05 Fixed file checking so won't keep spinning up hdd for people with timeshift off.
                   20/06/05 Fixed Finnish & German language to remove 12H timerlist timestamp (12H time not used in Finland)
                   Fixed bug with 0 timers showing junk in the timerview window.   Revamped German menu thanks to input from "Michael Hof"
            V8.7   21/06/05 Fixed Finnish a little more...  Fixed where I had the test number in to test the finnish language..
			V8.8   25/06/05 Again, made a few subtle changes with Finnish and German.   Added purge backup timers menu option.   Fixed issue
			       with hitting exit key while progressbar was on screen (now look at a bigger area again before popping up menu).
		    V8.9   29/06/05 Fixed OSDactive once more... now exit key should let tap come up under all circumstances, while avoiding
			       the tap popping up the menu when hitting exit to close the progressbar or infobar.   Oops, didn't check the state for exit key either...
*/
/**********************************************************************/

#include "tap.h"
#include "morekeys.h"
//#include <stdio.h>
#include "bluebutton.gd"
#include "yellowbutton.gd"

// needs to be requested from Topfield!  I picked a *very* high ID, shouldn't clash with anything.
#define ID_SIGNAL 0x80009021
#define INI_FILE "timer_extend.ini"
#define TIMER_DELETE_FILE "epg_dtimer.tsv"
//#define TF5100C

#ifndef TF5100C
    #define _PROGRAM_NAME_ "Timer Extend V8.9"
#else
    #define _PROGRAM_NAME_ "Timer Extend V8.9_5100C"
#endif

#define BACKUP_FILE "timer_backup.dat"

TAP_ID( ID_SIGNAL );
TAP_PROGRAM_NAME(_PROGRAM_NAME_);
TAP_AUTHOR_NAME("tonymy01 & thanks to others who have helped");
TAP_DESCRIPTION("extend timers, fix future timers, automatically saves/restores single timers");
TAP_ETCINFO(__DATE__);

#include "TSRCommander.inc"

#define COLOR_WINDOW COLOR_DarkCyan
#define COLOR_WINDOWTEXT COLOR_White
#define COLOR_BTNFACE COLOR_DarkBlue
#define COLOR_BTNTEXT COLOR_White
#define COLOR_BTNSHADOW COLOR_Black
#define COLOR_BTNHIGHLIGHT COLOR_Blue

#define COLOR_FRed   RGB( 23,  0, 0 )                   // colors for bar (some currently unused)
#define COLOR_BRed   RGB( 15,  0, 0 )
#define COLOR_FGreen RGB(  0, 23, 0 )
#define COLOR_BGreen RGB(  0, 15, 0 )
#define COLOR_LGrey  RGB(8,8,8)
#define COLOR_MGrey  RGB(15,15,15)
#define COLOR_DGrey  RGB( 25, 25, 25)
#define COLOR_TonyBlue  RGB(12,18,30)
#define COLOR_L2Blue  RGB(9,16,23)
#define COLOR_L3Blue  RGB(3,15,26)
#define COLOR_LBlue  RGB(8,13,22)


#define printf TAP_Print



#define RAM_start 0x81800000
#define RAM_STOP 0x82ffffff
//#define   RAM_STOP  0x83FFFFFF
//#define SIGNATURE_OFFSET 0x54        //offset to 1st timer record that generally starts 0x140 or 0x0040.
#define SIGNATURE_OFFSET -0x14         //offset from 1st timer filename to beginning of timer.
//#define SIGNATURE_OFFSET_EPG 0x2588
#define SIGNATURE_OFFSET_EPG 0x2520    //offset to epg reserved "highlight" option.
#define SIGNATURE_FILENAME "signature.ini"

//#define TIMER_start 0x818F272C is start of timer table with some fws, each record is EPG_RECORD_LENGTH bytes, 70 records max
#define TABLE_LENGTH 0x2530
#define TEMP_ONETIME_LENGTH 0x2530
#define EPG_TABLE_LENGTH 0x230

#define DATEOFFSET 0x8
#define FILENAMEOFFSET 0x14

#ifndef TF5100C
    #define EPG_RECORD_LENGTH 0x88
#else
    #define EPG_RECORD_LENGTH 0x84
#endif

//#define FAV_start 0x818F0208
#define FAV_LENGTH 0x24CC             //each fav is 314 bytes =0x13a  30 Favourites all up

//#define BOTH_start 0x818F0204         //save both favourites and timers
#define BOTH_LENGTH 0x4A58

//position the blue button
#define BBOX_X      69
#define BBOX_Y      528
#define BBOX_W      (_bluebuttonGd.width)
#define BBOX_H      (_bluebuttonGd.height)

//position the yellow button
#define YBOX_X      222
#define YBOX_Y      528
#define YBOX_W      (_yellowbuttonGd.width)
#define YBOX_H      (_yellowbuttonGd.height)

#define _num_items_ 14 //number of menu entries


/*****************************************************************************/
/* Global variables */
/*****************************************************************************/
typedef struct
{
    word date; // MJDDate format
    byte hour;
    byte min;
}
DateTime;


typedef struct
{
    byte tuner;    /*0x00 Tuner 1, 0x01 Tuner2, 0x03 Auto Tuner*/
    byte xxx0 :4; /*Unknown always equals 0x4*/
    byte program_type :4;  /* 0x2 = programmed event, 0x0 = EPG "R" event */
    byte xxx1; /* Unknown, always equals 0x00*/
    byte broadcast_type :4; /* 0x0 = tvtuner, 0x8 = radio */
    byte recurance :4; /* 0x0 = Once, 0x1 = Daily, 0x2 = Weekend, 0x3 = Weekly, 0x4 = Weekday */
    word duration; /* Duration in minutes */
    word channel; /* Topfield internal channel number */
    dword timer_start;
    dword timer_stop;
    byte  recording; /*0x00 record off, 0x01 record*/
    byte  nameFix; /* Unknown sometimes= 0x01, is probably the name recording from EIT flag (where 00=get name)*/
    word serviceId; /* Timer serviceId details TF5000*/
    char filename[88]; /* Filename to record to   (should be 96 or 97, but truncating by 8 for my backup)*/
    dword timer_startBackup; //added for this tap
    word durationBackup;  //added for this tap
    word BackupSignature; //==0xDEAF if extended
    word epgRt; /*epg rt marker, equals 0x0002 if marked*/
    word xxx2; /*always equals 0x0000*/
    #ifndef TF5100C
        word rf_channel; /* RF Channel Number appears to be BCD for these 2 bytes*/
        word xxx3; /* Unknown = 0x0700 for all recordings*/
    #endif
    dword frequency; /*TF5000 div by 1000 to get Freq in Mhz */
    word transportstreamId; /* TF5000transport stream Id= often 1st svcid for that particular RF channel*/
    word originalnetworkIdH; // TF5000 DWORD must be on a DWORD boundary! all the previous items are all ok.  the end result is this would write over the next memory location!
    word originalnetworkIdL; // TF5100C probably not original network id, looks more like 0x0009 0000 could be channel number or something
    word xxx4; //always=0x0300 for TF5100C, 0x0000 for TF5000
}
TYPE_TimerRAM ;


struct TYPE_timerTailBackup
{
    word serviceId; /* Timer serviceId details */
    dword timer_startBackup; //added for this tap
    word durationBackup;  //added for this tap
    word BackupSignature; //==0xDEAF if extended
    word epgRt; /*epg rt marker, equals 0x0002 if marked*/
    word xxx2; /*always equals 0x0000*/
    word rf_channel; /* RF Channel Number appears to be BCD for these 2 bytes*/
    word xxx3; /* Unknown = 0x0700 for all recordings*/
    dword frequency; /* div by 1000 to get Freq in Mhz */
    word transportstreamId; /*transport stream Id= often 1st svcid for that particular RF channel*/
    word originalnetworkIdH; //
    word originalnetworkIdL;
    word xxx4; //always=0x0000
}
__attribute__ ((packed));


static TYPE_TimerRAM *g_timerPointer;

static TYPE_TimerRAM *g_BackupPointer;

static struct TYPE_timerTailBackup timerTailBackup;

static short g_nstart,g_nend;   // start and end duration
static int g_nrgnOpts;          // rgn-handle for options window
static TYPE_Window g_winOpts;   // window-handle for options window

static TYPE_TimerInfo g_timerinfo;
static int rgn;                             // stores rgn-handle for exit-menu

static dword g_last_event=0;

static bool g_bOptsWinShown;    // toggles whether options window is shown
static bool g_bloadoptions=FALSE;
static bool g_bchecktimer=FALSE;
static bool g_brecfunction=FALSE;
static bool g_btimerfunction=FALSE;
static bool g_bsortfunction=FALSE;
static short g_defaulttuner=0;

static bool g_btapdirinifile=FALSE; //to flag if inifile found with tap (new versions will put in programfiles).

static int debug_counter=0; //used to break out of timer modify while loops if something goes wrong
static dword g_refreshtime=0;
static word g_faulty;

static int g_nselected;
static int g_timerposition=0;
//static dword g_timerPointer=0x00000000;
static bool bMakerRecKey = FALSE;
static bool bMaker0Key = FALSE;

static bool g_bSingleTimerRam=TRUE;
static int g_SingleTimerRAMcount=0;

static int g_total_tvSvc,g_total_radSvc; //store services, so if number of channels changed, will delete onetime backup.

//region for blue button to be displayed
static bool _blueShow = FALSE;   //boolean
static word _blueRgn;
static word _blueTempRgn;
static word _yellowRgn;
static word _yellowTempRgn;
/*****************************************************************************/
/* Functions */
/*****************************************************************************/
void bubble_sort(int size);
void ShowMessageWin (char* lpMessage, char* lpMessage1, int time);
void changetimer(word *day, byte *hour, byte *minute, short add);
void SaveOptions(void);
void ShowOptionsWindow ();
void show_timer_error(int retval, int i);
int sort_timers(void);
void tailrestore(int i);
void tailbackup(int i);
void load_backup_timers();
void save_backup_timers();
void ChangeDirRoot();
void ChangeDir(char *DirName);
void FindTapDir(char *tapDirName);
//bool onetime_nolonger_exists(byte *onetimeaddress);
/*****************************************************************************/

void show_timer_error(int retval, int i)
{
    char msg[300];
    char tmp[64];
    char tmp1[64];
    char tmp2[64];
    TYPE_TimerInfo timerinfo;
    TYPE_TimerInfo timerinfo1,timerinfo2;

    printf("timer_extend: Timer error retval:0x%x",retval);

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
            else if (retval >= 0xFFFF0000)
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
            //else if (retval==4)
            //  TAP_SPrint(msg,"Störungen mit Timer! (Konflikt mit mehrfachen Timern)");
            //else
            //  TAP_SPrint(msg,"Fehler beim Verändern von Timer %s! (unbekannter Fehler)",strncpy(tmp,timerinfo.fileName,60));
            //logf(3,"Timer Fehler, %s",msg);
            ShowMessageWin("Timer Fehler",msg,240);
            break;
        case 15:  //finish language
            if (retval == 0xffffffff)
                TAP_SPrint(msg,"Virhe muokattaessa ajastusta %s. (Virheellinen ajastus)",tmp);
            else if (retval == 0xfffffffe )
                TAP_SPrint(msg,"Virhe muokattaessa ajastusta %s. (Virheellinen viritin)",tmp);
            else if (retval >= 0xFFFF0000)
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
            //else if (retval==4)
            //  TAP_SPrint(msg,"Virhe muokattaessa ajastusta/ajastuksen luonnissa. (Päällekkäisyys monen ajastuksen kanssa)");
            //else
            //  TAP_SPrint(msg,"Virhe muokattaessa ajastusta %s. (Tuntematon virhe)",i+1);
            //logf(3,"Ajastuksen luontivirhe %s",msg);
            ShowMessageWin("Ajastuksen luontivirhe",msg,240);
            break;
        default:
            if (retval == 0xffffffff)
                TAP_SPrint(msg,"Error Modding Entry %s! (Invalid Entry)",tmp);
            else if (retval == 0xfffffffe )
                TAP_SPrint(msg,"Error Modding Entry %s! (Invalid Tuner)",tmp);
            else //if ((retval&0xFFFF0000) == 0xFFFE0000)
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
            //else if (retval==4)
            //  TAP_SPrint(msg,"Error Modding/Creating New Entry! (Conflicts with multiple timers)");
            //else
            //  TAP_SPrint(msg,"Error Modding Entry %s! (Unknown Error)",i+1);
            //logf(3,"Timer create FAILURE, %s",msg);
            ShowMessageWin("Timer create FAILURE",msg,240);
        }
} //show_timer_error


void ChangeDirRoot()
{
    TYPE_File   fp;
    int     iNdx;

    iNdx = 0;
    TAP_Hdd_FindFirst( &fp );
    //  Loop until Root found allow maximum of 20 levels
    while ( !strstr( fp.name, "__ROOT__" ) &&  iNdx < 20 )
    {
        TAP_Hdd_ChangeDir( ".." );
        TAP_Hdd_FindFirst( &fp );
    }
}



void ChangeDir(char *DirName)
{
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

void tailbackup(int i)
{
    timerTailBackup.serviceId=g_timerPointer[i].serviceId;
    timerTailBackup.timer_startBackup=g_timerPointer[i].timer_startBackup;
    timerTailBackup.durationBackup=g_timerPointer[i].durationBackup;
    timerTailBackup.BackupSignature=g_timerPointer[i].BackupSignature;
    timerTailBackup.epgRt=g_timerPointer[i].epgRt;
    timerTailBackup.xxx2=g_timerPointer[i].xxx2;
    #ifndef TF5100C
        timerTailBackup.rf_channel=g_timerPointer[i].rf_channel;
        timerTailBackup.xxx3=g_timerPointer[i].xxx3;
    #endif
    timerTailBackup.frequency=g_timerPointer[i].frequency;
    timerTailBackup.transportstreamId=g_timerPointer[i].transportstreamId;
    timerTailBackup.originalnetworkIdH=g_timerPointer[i].originalnetworkIdH;
    timerTailBackup.originalnetworkIdL=g_timerPointer[i].originalnetworkIdL;
    timerTailBackup.xxx4=g_timerPointer[i].xxx4;
}

void tailrestore(int i)
{
    g_timerPointer[i].serviceId=timerTailBackup.serviceId;
    g_timerPointer[i].timer_startBackup=timerTailBackup.timer_startBackup;
    g_timerPointer[i].durationBackup=timerTailBackup.durationBackup;
    g_timerPointer[i].BackupSignature=timerTailBackup.BackupSignature;
    g_timerPointer[i].epgRt=timerTailBackup.epgRt;
    g_timerPointer[i].xxx2=timerTailBackup.xxx2;
    #ifndef TF5100C
        g_timerPointer[i].rf_channel=timerTailBackup.rf_channel;
        g_timerPointer[i].xxx3=timerTailBackup.xxx3;
    #endif
    g_timerPointer[i].frequency=timerTailBackup.frequency;
    g_timerPointer[i].transportstreamId=timerTailBackup.transportstreamId;
    g_timerPointer[i].originalnetworkIdH=timerTailBackup.originalnetworkIdH;
    g_timerPointer[i].originalnetworkIdL=timerTailBackup.originalnetworkIdL;
    g_timerPointer[i].xxx4=timerTailBackup.xxx4;

}

int sort_timers(void)
{
    int i,retval;
    bool modflag=FALSE;
    TYPE_TimerInfo timerinfo;
    const int total_timers=TAP_Timer_GetTotalNum();

    //dword mempointers[total_timers];

    //setup pointers to timers in RAM
    //for (i=0;i<total_timers; ++i){
    //  mempointers[i]=g_timerPointer+SIGNATURE_OFFSET+i*EPG_RECORD_LENGTH;
    //}
    bubble_sort(total_timers);  //sort

    for (i=0;i<total_timers;++i)   //change tuner info
    {
        if (g_defaulttuner!=4)
        {
            TAP_Timer_GetInfo(i,&timerinfo);
            if (g_defaulttuner!=3) //if older firmware, i.e. not tuner 4 chosen in the settings menu
            {
                if(timerinfo.tuner==3) //if 3 equals newer firmware and won't work with older
                {
                    g_timerPointer[i].tuner=g_defaulttuner; //set to default tuner for new firmware
                }
            }
            else if (timerinfo.tuner!=3) //else if newer firmware, and current tuner !=3
            {
                g_timerPointer[i].tuner=3;
            }
        }

    }
} //sort timers



/* Simple bubble sort */
void bubble_sort(int size)
{
    int nswaps,i,j;
    TYPE_TimerRAM  tempbuff;

    //tempbuff=TAP_MemAlloc(EPG_RECORD_LENGTH);

    do
    {
        for (nswaps = i = 0 ; i < size - 1 ; i++)
            if (g_timerPointer[i].timer_start > (g_timerPointer+i+1)->timer_start)
        {
            tempbuff=*(g_timerPointer+i);  //fill buffer with timer
            *(g_timerPointer+i)=*(g_timerPointer+i+1);
            *(g_timerPointer+i+1)=tempbuff;
            ++nswaps;
        }
    }
    while (nswaps > 0);

    //TAP_MemFree( tempbuff );

}



void ShowMessageWin (char* lpMessage, char* lpMessage1, int time)
{
    //int rgn;                          // stores rgn-handle for osd
    dword w;                            // stores width of message-text

    word year,todaydate;
    byte month,day,hour,min,dummy;

    TAP_GetTime(&todaydate, &hour, &min, &dummy);
    TAP_ExtractMjd(todaydate, &year ,&month ,&day , &dummy);
    printf("%04d/%02d/%02d %02d:%02d %s, %s\n",year,month,day,hour,min,lpMessage,lpMessage1);  //prints to the serial port, 115200 8N1 rate.

    if (!g_nrgnOpts)
    {
        g_nrgnOpts = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );        // create rgn-handle
    }
    w = TAP_Osd_GetW( lpMessage, 0, FNT_Size_1926 ) + 10;       // calculate width of message
    if (TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10>w)
    {
        w = TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10;  // calculate width of message
    }
    if (w > 700) w = 700;                       // if message is to long
    TAP_Osd_FillBox(g_nrgnOpts, (720-w)/2-5, 265, w+10, 62, RGB(19,19,19) );    // draw background-box for border
        TAP_Osd_PutS(g_nrgnOpts, (720-w)/2, 270, (720+w)/2, lpMessage,      // show message
                     RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_CENTER);
    TAP_Osd_PutS(g_nrgnOpts, (720-w)/2, 270+26, (720+w)/2, lpMessage1,      // show message
                 RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926,FALSE, ALIGN_CENTER);
    TAP_Delay(50);
    g_refreshtime= TAP_GetTick()+time;
    //printf("showmessage now %d, modded %d\n",TAP_GetTick(),g_refreshtime);
}


void HideOptionsWindow ()
{
    TAP_Win_Delete (&g_winOpts);
    TAP_Osd_Delete (g_nrgnOpts);
    TAP_EnterNormal();      // re-entering normal receiver operation
    g_bOptsWinShown = FALSE;
    g_nrgnOpts=0;
    SaveOptions();
}

void LoadOptions(void)
{
    TYPE_File* fp;

    if (!g_btapdirinifile) //if inifile not found with tap (will be case if tap is in autostart and inifile is in progfiles, the situation ..
    {
        ChangeDir("ProgramFiles"); //..that this new tap version will create. Supports "moving" the inifile from autostart
    }

    if ((fp = TAP_Hdd_Fopen (INI_FILE)))
    {
        char ReadBuffer[11];

        if (TAP_Hdd_Fread (ReadBuffer, 1, 11, fp) == 11)
        {
            g_nstart = ReadBuffer[0];
            g_nend   = ReadBuffer[1];
            g_bchecktimer = (ReadBuffer[2]!=0);
            g_faulty=(ReadBuffer[4]<<8)|ReadBuffer[5];
            g_brecfunction = (ReadBuffer[7]!=0);
            g_btimerfunction= (ReadBuffer[8]!=0);
            g_bsortfunction= (ReadBuffer[9]!=0);
            g_defaulttuner= ReadBuffer[10];
            if (g_defaulttuner>4) g_defaulttuner=0;
        }
        TAP_Hdd_Fclose (fp);
        printf("timer_extend: inifile loaded\n");
        if (g_btapdirinifile)  //if inifile was originally detected with tap (such as if tap is in autostart, inifile in autostart)
        {
            printf("timer_extend: inifile deleted from tap directory and being saved to new directory\n");
            TAP_Hdd_Delete(INI_FILE);  //delete it
            g_btapdirinifile=FALSE;
            SaveOptions();            //save to progfiles folder
        }
    }
    else
    {
        if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
           ShowMessageWin(INI_FILE" kann nicht geöffnet werden.", "Standardeinstellungen werden verwendet.",250);
        else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
            ShowMessageWin("Asetustiedoston ("INI_FILE") avaus epäonnistui", "Käytetään oletusasetuksia",250);
        else
            ShowMessageWin("can't open "INI_FILE, "Setting default options",250);
        g_nstart=0;
        g_nend=0;
        g_bchecktimer=FALSE;
        g_brecfunction=FALSE;
        g_btimerfunction=FALSE;
        g_bsortfunction=FALSE;
        g_faulty=0;
        g_defaulttuner=0;
        SaveOptions();
    }

} //loadoptions


void SaveOptions(void)
{
    TYPE_File* fp;

    ChangeDir("ProgramFiles");

    if (!TAP_Hdd_Exist (INI_FILE))
        TAP_Hdd_Create (INI_FILE, ATTR_NORMAL); // create file if not existing

    if ((fp = TAP_Hdd_Fopen (INI_FILE)))
    {
        char WriteBuffer[512];

        memset (WriteBuffer, 0, sizeof WriteBuffer);
        WriteBuffer[0] = g_nstart;
        WriteBuffer[1] = g_nend;
        WriteBuffer[2] = g_bchecktimer?1:0;
        WriteBuffer[3] = 255;
        WriteBuffer[4] = (g_faulty>>8)&0xff;
        WriteBuffer[5] = g_faulty&0xff;
        WriteBuffer[6] = 255;
        WriteBuffer[7] = g_brecfunction?1:0;
        WriteBuffer[8] = g_btimerfunction?1:0;
        WriteBuffer[9] = g_bsortfunction?1:0;
        WriteBuffer[10] =  g_defaulttuner;

        sprintf(&WriteBuffer[20],"\r\nNumber of \"future\" timers fixed: %05d\r\n",g_faulty);
        TAP_Hdd_Fwrite (WriteBuffer, 1, 512, fp);
        printf("timer_extend: inifile saved\n");
        TAP_Hdd_Fclose (fp);
    }
    else
    {
        if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
            ShowMessageWin(INI_FILE" kann nicht geöffnet werden", "",250);
        else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
            ShowMessageWin("Asetustiedoston ("INI_FILE") avaus epäonnistui","",250);
        else
            ShowMessageWin("can't open "INI_FILE, "",250);
    }
} //saveoptions


void change_defaulttuner(int code)
{
    if (code==RKEY_VolDown)
    {
        if (g_defaulttuner==0) g_defaulttuner=4;
        else if (g_defaulttuner==3) g_defaulttuner=1;
        else g_defaulttuner=0;
    }
    else //volup or ok key pressed
    {
        g_defaulttuner=(g_defaulttuner+1)%5;   //change default tuner
        if (g_defaulttuner==2) g_defaulttuner++;
    }
}


dword searchRAM (void)
{
    TYPE_File* fp;
    dword any;
    int i=0,j=0,n=0;
    TYPE_TimerInfo timerinfo;
    word todaydate;
    byte hour,min,sec;
    unsigned int retval;
    int addedtimerflag=2;
    char timerfilename[100],timerfilename2[100];
    int jump;

    j=TAP_Timer_GetTotalNum();
    //printf("total number of timers=%d\n",j);
    addedtimerflag=2-j;
    TAP_GetTime(&todaydate, &hour, &min, &sec);
    while (addedtimerflag>0)  //if we don't have 2 timers, we have to add some to check the signature and RAM match
    {
        timerinfo.startTime=(((todaydate+1000+addedtimerflag)<<16) + n);  //make it 1000days into the future and at 0:00am
        sprintf(timerinfo.fileName,"Tony Timer Extend Test%d.rec\0",addedtimerflag);
        timerinfo.duration=5;
        timerinfo.isRec=1;
        timerinfo.svcType=0;
        timerinfo.svcNum=0;
        timerinfo.reservationType=0;
        timerinfo.nameFix=0;
        timerinfo.tuner=0;

        retval=TAP_Timer_Add(&timerinfo);
        if (retval)
        {
            printf("Some kind of timer conflict:%x, trying new time\n",retval);
            n+=0x0100; //keep adding an hour to n if conflicts
            if (n==0x1800)
            {
                n=0x0000;
                todaydate=todaydate+1;
            }
        }
        else
            addedtimerflag--;
    }
    TAP_Timer_GetInfo(0,&timerinfo);
    strcpy(timerfilename,timerinfo.fileName);
    TAP_Timer_GetInfo(1,&timerinfo);
    strcpy(timerfilename2,timerinfo.fileName);

    if (j==1 && !strcmp(timerfilename,"Tony Timer Extend Test2.rec"))
    {
        //printf("extend test2 is 1st undel timer");
        j=0;  //topfield seems to "undelete" timer after reboot, so this will hopefully fix this.
    }
    else if (j==2 && !strcmp(timerfilename2,"Tony Timer Extend Test1.rec"))
    {
        //printf("extend test1 is 2nd undel timer");
        j=1;  //topfield seems to "undelete" timer after reboot, so this will hopefully fix this.
    }

    jump=4;
    label:
    for ( any = RAM_start ; any <= RAM_STOP ; any+=jump )
    {
        if (!strncmp(timerfilename,(char *)any,strlen(timerfilename)))  //if 1st time in API equal to 1st in memory
        {
            if (!strncmp(timerfilename2,(char *)(any+EPG_RECORD_LENGTH),strlen(timerfilename2))) //and if 2nd timer in API equal to 2nd timer in memory (double check)
            {
                //printf("debug, got here %d\n",jump);
                break;
            }
        }
    }
    if (any>=(RAM_STOP))
    {
        if (jump==4)
        {
            jump=1;
            goto label;
        }
        else
        {
            if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
            {
				ShowMessageWin("GESAMTER SPEICHER WURDE GESCANT", "TIMER SIGNATUR NICHT GEFUNDEN.",270);
				ShowMessageWin("Lassen Sie bitte Debugger laufen und treten Sie mit", "dem Autor in Verbindung, um neues TAP zu erstellen.",290);
	           }
            else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
            {
                ShowMessageWin("MUISTI TUTKITTU", "AJASTUSTUNNISTETTA EI LÖYTYNYT",270);
                ShowMessageWin("Ota yhteyttä tekijään", "uuden TAP-version saamiseksi",300);
            }

            else
            {
                ShowMessageWin("SCANNED ALL MEMORY", "CANT FIND TIMER SIGNATURE",270);
                ShowMessageWin("Please run debugger and contact author", "to build new TAP",300);
            }
            any=0-SIGNATURE_OFFSET;
        }
    }
    //delete the extra created timers that were used to locate the RAM.
    if (j==1)
    {
        //printf("j=1, deleting 1 timer\n");
        TAP_Timer_Delete(1);
    }
    else if (j==0)  //if numberoforigtimers = 0
    {
        //printf("j=0, deleting 2 timers\n");
        TAP_Timer_Delete(1);
        TAP_Timer_Delete(0);
    }
    return any+SIGNATURE_OFFSET;
}//search RAM

void save_backup_timers()
{
    //int i=0,filelength;
    TYPE_File* fp;
    //byte tony=0;
    //char * rampointer;
    //int backupcount=(g_SingleTimerRAMcount>=140)?140:g_SingleTimerRAMcount;
    //int size=EPG_RECORD_LENGTH*backupcount + EPG_RECORD_LENGTH*backupcount%512;

    if ( TAP_Hdd_Exist( BACKUP_FILE ) )
        TAP_Hdd_Delete( BACKUP_FILE );

    TAP_Hdd_Create( BACKUP_FILE, ATTR_NORMAL );
    fp = TAP_Hdd_Fopen( BACKUP_FILE );
    if (fp)
    {
        TAP_Hdd_Fwrite((byte *)g_BackupPointer,1,EPG_RECORD_LENGTH*g_SingleTimerRAMcount+512-(g_SingleTimerRAMcount*EPG_RECORD_LENGTH)%512, fp );
        //TAP_Hdd_Fseek(fp,(g_SingleTimerRAMcount*EPG_RECORD_LENGTH),0);
        //TAP_Hdd_Fwrite(&tony,1,512-(g_SingleTimerRAMcount*EPG_RECORD_LENGTH)%512,fp);
        TAP_Hdd_Fclose(fp);
        printf("%d backup timers written to "BACKUP_FILE" \n", g_SingleTimerRAMcount);
    }
    else
    {
        if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
            ShowMessageWin( BACKUP_FILE," kann nicht geöffnet werden.",150);
        else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
            ShowMessageWin("Tiedoston avaus epäonnistui", BACKUP_FILE,150);
        else
            ShowMessageWin("can't open file", BACKUP_FILE,150);
    }
} //save_backup_timers()

void load_backup_timers()
{
    int filelength,i;
    TYPE_File* fp;
    TYPE_TimerRAM tempstore;

    //int backupcount=(g_SingleTimerRAMcount>=140)?140:g_SingleTimerRAMcount;
    g_SingleTimerRAMcount=0;
    memset((byte *)g_BackupPointer,0x0,512-(140*EPG_RECORD_LENGTH)%512 + 140*EPG_RECORD_LENGTH);  //clear all backup ram (make it a multiple of 512 so write above is clean too).

    if (!( fp = TAP_Hdd_Fopen( BACKUP_FILE ) ))   //if file doesn't exist, lets create (will create in the save routine)
    {
        for (i=0;i<TAP_Timer_GetTotalNum(); i++) //put single timers into buffer
        {
            if (g_timerPointer[i].recurance==0x00 && g_timerPointer[i].timer_start!=0)
            {
                *(g_BackupPointer+g_SingleTimerRAMcount++)=*(g_timerPointer+i);
            }
        }
        printf("load backup, No logfile exists-Number of single timers:%d\n",g_SingleTimerRAMcount);
        save_backup_timers();
    }
    else
    {
        filelength=TAP_Hdd_Flen( fp );
        if (filelength!=0 && (filelength%512)==0) filelength+=512;  //FRIGGIN TOPFIELD BUG! flen is 512bytes out if the filesize is a multiple of 512.
        if (filelength>140*EPG_RECORD_LENGTH) filelength=140*EPG_RECORD_LENGTH;
        //tempstore=TAP_MemAlloc(EPG_RECORD_LENGTH);
        for (i=0;i<filelength/EPG_RECORD_LENGTH;i++)
        {
            TAP_Hdd_Fread(&tempstore, EPG_RECORD_LENGTH, 1, fp );
            /*
                                printf("time tun:%d type:%d brod:%d recur:%d dur:%d chan:%d star:%x rec:%d Fix:%d Id:%d nam:%s ",
                                tempstore.tuner,tempstore.program_type,tempstore.broadcast_type,tempstore.recurance,
                                tempstore.duration,tempstore.channel,tempstore.timer_start,tempstore.recording,
                                tempstore.nameFix,tempstore.serviceId,tempstore.filename);
                                printf("strtb:%x durb:%d epg:%d rf:%x freq:%d ts:%d ont:%x xx4:%x\n",
                                tempstore.timer_startBackup,
                                tempstore.durationBackup,tempstore.epgRt,tempstore.rf_channel,tempstore.frequency,
                                tempstore.transportstreamId,tempstore.originalnetworkIdH<<16|tempstore.originalnetworkIdL,tempstore.xxx4);
                        */
            if (tempstore.timer_start !=0)  *(g_BackupPointer+g_SingleTimerRAMcount++)=tempstore;
        }
        //TAP_MemFree(tempstore);
        printf("load_backup, Number of backups found:%d\n",g_SingleTimerRAMcount);
        TAP_Hdd_Fclose(fp);

    }
} //load_backup_timers()

int check_timer(void)
{
    static bmemwarningshown=FALSE;
    int i,j,k;
    //int numbercounter=0;
    word todaydate,timerdate,nowHourMin;
    byte timerweekday,todaytimeweekday;
    TYPE_TimerInfo timerinfo;
    byte dummy;
    word dummy2;

    byte month ,day, hour,min;
    word year;
    unsigned int retval;

    bool moddedflag=FALSE;
    bool bsaveflag=FALSE;
    bool bsave_backup=FALSE;

    TAP_GetTime(&todaydate, &hour, &min, &dummy);
    if (todaydate<0xD0FB)
    {
        printf("Bad date\n");
        return 0;//if less than 20 May 2005 or so
    }
    changetimer(&todaydate, &hour, &min, 2);  //add two minutes to the current time
    for (i=0;i<TAP_Timer_GetTotalNum();++i)
    {
        if (g_defaulttuner!=4)
        {
            if (g_defaulttuner!=3) //if older firmware
            {
                if (g_timerPointer[i].tuner==3) //if 3 equals newer firmware and won't work with older
                    g_timerPointer[i].tuner=g_defaulttuner; //set to default tuner for new firmware
            }
            else if (g_timerPointer[i].tuner!=3)
                g_timerPointer[i].tuner=3;
        }
        TAP_Timer_GetInfo(i,&timerinfo);
        if (strncmp((char *)strstr(timerinfo.fileName,".rec\0")-2,"_+",2)!=0 && strncmp((char *)strstr(timerinfo.fileName,".rec\0")-2,"-+",2)!=0) //if _+ not found on timer, i.e. not a permanently postponed timer
        {
            timerdate=(timerinfo.startTime>>16)&0xffff;
            nowHourMin =((hour<<8) | (min));  //sometimes timer would start just before the minute changed over, so now going to give two mins of slack
            TAP_ExtractMjd(timerdate, &dummy2, &dummy, &dummy, &timerweekday);
            TAP_ExtractMjd(todaydate, &dummy2 ,&dummy ,&dummy , &todaytimeweekday);

            switch(timerinfo.reservationType)
            {
                case RESERVE_TYPE_Weekly:

                    while (timerdate>(todaydate+7) || ((timerdate==(todaydate+7)) && nowHourMin<(timerinfo.startTime&0xffff))) //if timestamp shifted, either >7days or =7days if not already happened today
                    {
                        timerdate-=7;
                        moddedflag=TRUE;
                    }
                    while (timerdate<todaydate)  //weekly past timer repair
                    {
                        timerdate+=7;
                        moddedflag=TRUE;
                    }
                    if (moddedflag) ShowMessageWin("Timer weekly being fixed","",200); //test

                    break;


                case RESERVE_TYPE_WeekDay:

                    if (todaytimeweekday<=4)   //if today weekday
                    {
                        if (timerdate>todaydate && nowHourMin<(timerinfo.startTime&0xffff)) //if timestamp >today and nowtime is less than timertime
                        {
                            timerdate=todaydate;
                            moddedflag=TRUE;
                            ShowMessageWin("timer weekday is being fixed for today","",200); //test
                        }
                        else if (todaytimeweekday==4 && timerdate>(todaydate+3) && nowHourMin>(timerinfo.startTime&0xffff)) //else if today friday, and timerdate>monday and nowtime is greater than timertime
                        {
                            timerdate=todaydate+3;
                            moddedflag=TRUE;
                            ShowMessageWin("timer weekday being fixed for Monday","",200); //test
                        }
                        else if (todaytimeweekday!=4 && timerdate>(todaydate+1) && nowHourMin>(timerinfo.startTime&0xffff)) //else if timestamp >tomorrow and nowtime is greater than timertime
                        {
                            timerdate=todaydate+1;
                            TAP_ExtractMjd(timerdate, &dummy2, &dummy, &dummy, &timerweekday);
                            if (timerweekday==5) timerdate+=2; //if now changed to Saturday, then move to monday.
                            moddedflag=TRUE;
                            ShowMessageWin("timer weekday being fixed for tomorrow","",200); //test
                        }

                    }
                    else  //accomodate that today is weekend
                    {
                        if (timerdate>(todaydate+7-todaytimeweekday))   //if timer is > next monday
                        {
                            timerdate=todaydate+7-todaytimeweekday;
                            ShowMessageWin("timer weekday being fixed for Monday2","",200); //test
                            moddedflag=TRUE;
                        }
                    }
                    if (timerdate<todaydate) ShowMessageWin("past timer weekday being fixed","",200); //weekday past timer repair
                    while (timerdate<todaydate) //weekday past timer repair
                    {
                        timerdate++;
                        moddedflag=TRUE;
                    }
                    break;

                case RESERVE_TYPE_EveryWeekend:
                    if (todaytimeweekday<=4)   //if today weekday
                    {
                        if (timerdate>(todaydate+5-todaytimeweekday))  //if timer greater than next saturday
                        {
                            timerdate=todaydate+5-todaytimeweekday;  //timerdate=todaysdate +5days-daycount (where mon=0,tue=1 etc)
                            ShowMessageWin("timer weekend being fixed for next Sat","",200); //test
                            moddedflag=TRUE;
                        }
                    }
                    else if ((todaytimeweekday==5 || todaytimeweekday==6) && timerdate>todaydate && nowHourMin<(timerinfo.startTime&0xffff)) //if weekend timer, and today saturday||sunday, and timer not already due to start
                    {
                        timerdate=todaydate;
                        moddedflag=TRUE;
                        ShowMessageWin("Timer weekend being fixed for today","",200); //test
                    }
                    else if (todaytimeweekday==5 && timerdate>todaydate+1 && nowHourMin>(timerinfo.startTime&0xffff)) //if today is sat and timer not tomorrow (due to passing today already)
                    {
                        timerdate=todaydate+1; //make timer tomorrow
                        moddedflag=TRUE;
                        ShowMessageWin("Timer weekend Sunday being fixed","",200); //test
                    }
                    else if (todaytimeweekday==6 && timerdate>(todaydate+6)) //if today Sunday, and timer > next Sat
                    {
                        timerdate=todaydate+6;
                        moddedflag=TRUE;
                        ShowMessageWin("Timer weekend next Sat being fixed2","",200); //test
                    }

                    if (timerdate<todaydate) ShowMessageWin("past timer weekend being fixed","",200); //weekend past timer repair
                    while (timerdate<todaydate)
                    {
                        timerdate++;
                        moddedflag=TRUE;
                    }
                    break;
                case RESERVE_TYPE_Everyday:

                    while (timerdate>(todaydate+1) || ((timerdate==(todaydate+1)) && nowHourMin<(timerinfo.startTime&0xffff))) //if timestamp shifted, either >1day or =1day if not already happened today
                    {
                        timerdate-=1;
                        moddedflag=TRUE;
                    }
                    while (timerdate<todaydate) //everyday past timer repair
                    {
                        timerdate++;
                        moddedflag=TRUE;
                    }
                    if (moddedflag) ShowMessageWin("timer daily being fixed","",200); //test
                    break;
            }//end switch


            if (moddedflag)
            {
                printf("timerfix:%s newtimerdate:%x timerinfo.start:%02d%02d todaydate:%x nowHourMin:%02d%02d\n",
                       timerinfo.fileName,timerdate,(timerinfo.startTime&0xff),(timerinfo.startTime&0xff00)>>8,
                       todaydate,(nowHourMin&0xff00)>>8,nowHourMin&0xff);
                //ShowMessageWin("INFO",msg,1000); //test
                moddedflag=FALSE;
                bsaveflag=TRUE;
                g_faulty++;

                timerinfo.startTime=(((timerdate<<16)&0xffff0000)|(timerinfo.startTime&0xffff));
                tailbackup(i);
                retval=TAP_Timer_Modify(i,&timerinfo);
                if (retval)
                {
                    show_timer_error(retval,i);
                }
                else tailrestore(i);

            }//moddedflag

        }//if not postponed timer
    } // for i=0<totaltimers


    if (g_bSingleTimerRam)  // do onetime if we have enough ram.
    {
        char str[40];
        //int backupcount=(g_SingleTimerRAMcount>=140)?140:g_SingleTimerRAMcount;
        //int timercount=TAP_Timer_GetTotalNum();
        //restoration of backup timers not found in timerlist
        for (i=0;i<g_SingleTimerRAMcount;i++)
		{
            //oops, 8.2 didn't have this flag set, so as soon as foundsingle was set to true,
            //e.g. the 1st single timer matched but others were lost, this wouldn't have restored any more timers
            bool foundsingle=FALSE;
            if ( ((( g_BackupPointer[i].timer_start >>16)&0xffff) > todaydate) ||
				((((g_BackupPointer[i].timer_start >>16)&0xffff)==todaydate) &&
				((g_BackupPointer[i].timer_start&0xffff) >nowHourMin) )) //if backup timer is greater than now(+2mins added earlier just in case a timer is late to erase itself from the menu when it starts)
			{
				//printf("g_SingleTimerRAMcount:%d found a backuptimer>today %x timercount:%d\n",i,g_BackupPointer[i].timer_start,TAP_Timer_GetTotalNum());
				for (j=0;j<TAP_Timer_GetTotalNum();j++)
				{
					if (g_timerPointer[j].timer_start==g_BackupPointer[i].timer_start &&   //checks times match
						g_timerPointer[j].channel==g_BackupPointer[i].channel ) //
					{
						//printf("g_SingleTimerRAMcounter:%d,TAP_Timer_GetTotalNumer:%d found a timer that matches the backup skipping restore\n",i,j);
						foundsingle=TRUE;
						break;
					}
				}
				if (!foundsingle && TAP_Timer_GetTotalNum()<70)  //if backup is greater than now and if none of our timers match the backup timers, then restore it
				{
					if (TAP_Hdd_Exist(TIMER_DELETE_FILE))        //unless TIMER_DELETE_FILE exists, this indicates a timer was deleted, and to delete the backups since we don't know which timer
					{
						int m;
						TAP_Hdd_Delete(TIMER_DELETE_FILE);
						for(m=0;m<g_SingleTimerRAMcount;m++)
						{
							if ( (((g_BackupPointer[m].timer_start >>16)&0xffff) > todaydate) ||
								( (((g_BackupPointer[m].timer_start >>16)&0xffff)==todaydate) && ((g_BackupPointer[m].timer_start&0xffff) > nowHourMin ))
								)
								g_BackupPointer[m].timer_start=0x000000;  //effectively deleting the backuppointer.
						}
						printf("timer_extend: Deleted all future backups >now as we don't know if any were changed/deleted in epg_upload or other taps that create "TIMER_DELETE_FILE".\n");
						break; //break main for loop
					}
					else
					{
						strncpy(str,g_BackupPointer[i].filename,39);
						printf("g_SingleTimerRAMcounter:%d,TAP_Timer_GetTotalNumer:%d didn't found a timer that matches the backup RESTORING %s\n",i,j,str);
						*(g_timerPointer+TAP_Timer_GetTotalNum())=*(g_BackupPointer+i) ;
						ShowMessageWin("timer single being fixed",str,50); //test
						TAP_Delay(300);
						bsaveflag=TRUE;
						g_faulty++;
					}
				}
			}
		}

        //printf("TAP_Timer_GetTotalNum():%d\n",TAP_Timer_GetTotalNum());

        //backup of single timers starts again
        for (i=0;i<TAP_Timer_GetTotalNum(); i++)
        {
            if (g_timerPointer[i].recurance==0x00 && g_timerPointer[i].timer_start!=0)  //for each timer that is onetime and legit date
            {
                bool singlefoundinstore=FALSE;
                //int count=(g_SingleTimerRAMcount>=140)?140:g_SingleTimerRAMcount;
                //printf("count: %d singlefoundinstore: %d\n",count,singlefoundinstore);
                for (j=0;j<g_SingleTimerRAMcount;j++)  //check all backup timers in RAM
                {
                    if (( g_timerPointer[i].timer_start==g_BackupPointer[j].timer_start) &&   //if any in RAM=the one time timer
                        (g_timerPointer[i].channel==g_BackupPointer[j].channel ))
                    {
                        //printf("backup: TAP_Timer_GetTotalNumer:%d,g_SingleTimerRAMcounter:%d found a timer that matches the backup\n",i,j);
                        singlefoundinstore=TRUE; //then we have found, so...
                        break;
                    }
                } //check backup exists
                if (!singlefoundinstore) //but if our timer isn't in backup, then add it.
                {
                    char str[40];
                    strncpy(str,g_timerPointer[i].filename,39);
                    printf("TAP_Timer_GetTotal:%d,SingleTimerRAMcnt:%d  didn't find timer that matches the backup, adding %s to backup\n",TAP_Timer_GetTotalNum(),g_SingleTimerRAMcount,str);
                    if (g_SingleTimerRAMcount==140)
                    {
                        for (k=0;k<140;k++) //find timer already passed
                        {
                            if ((( g_BackupPointer[k].timer_start >>16)&0xffff)==0) //if wiped with a "0"
                                break;
                        }
                        if (k==140)  //if none were found that were 0, try looking for older ones instead
                        {
                            for (k=0;k<140;k++) //find timer already passed
                            {
                                if (
                                    ((( g_BackupPointer[k].timer_start >>16)&0xffff) < todaydate) ||
                                ((((g_BackupPointer[k].timer_start >>16)&0xffff)==todaydate) && ((g_BackupPointer[k].timer_start&0xffff) < (nowHourMin)) )
                                )
                                break;
                            }
                            if (k==140) k=0;  //if we don't have any already passed (not likely since we only can have 70!)
                        }
                        *(g_BackupPointer+k)=*(g_timerPointer+i);
                    }
                    else
                    {
                        *(g_BackupPointer+g_SingleTimerRAMcount++)=*(g_timerPointer+i);
                    }
                    bsave_backup=TRUE;
                    if (!bmemwarningshown && g_SingleTimerRAMcount>=140)
                    {
                        bmemwarningshown=TRUE;
                        printf("Timer Backup RAM filled, now overwriting early backups\n");
                    }
                }//but if our timer isn't in backup, then add it.
            }
            //printf("g_SingleTimerRAMcount=%d\n",g_SingleTimerRAMcount);
        }

        if (bsave_backup) save_backup_timers();
        if (bsaveflag)  SaveOptions();   //will save the "modded" number of timers to HDD.
    } //check_timer
}


void changetimer(word *day, byte *hour, byte *minute, short add)
{
    short lminute;
    short lhour;
    lhour=*hour;
    lminute=*minute+add%60;
    if (lminute<0)
    {
        lhour=lhour-1;
        lminute=lminute+60;
    }
    else if (lminute>59)
    {
        lhour=lhour+1;
        lminute=lminute-60;
    }

    lhour+=add/60;

    if (lhour<0)
    {
        *day=*day-1;
        lhour=lhour+24;
    }
    else if (lhour>23)
    {
        *day=*day+1;
        lhour=lhour-24;
    }

    *minute=lminute;
    *hour=lhour;
}


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


void apply_settings(void)
{
    word timer_start_day,timer_end_day;
    byte timer_start_min,timer_end_min, timer_start_hour,timer_end_hour;
    int timer_duration,timerduration2;
    int location_contents;
    long i,j;
    int numbercounter=0;
    dword newendtodaydate,newstarttodaydate,startepgdate;
    int n_end,n_start;
    unsigned int retval;
    char str[300];
    TYPE_TimerInfo timerinfo,timerinfo2;
    char * rectext;

    for (i=0;i<TAP_Timer_GetTotalNum();++i)  //stretch end to begin with
    {
        TAP_Timer_GetInfo(i,&timerinfo);
        if ((rectext= (char *)strstr(timerinfo.fileName,".rec")))
        {

            if (!(!strncmp(rectext-2,"_#",2) || !strncmp(rectext-2,"-#",2) || !strncmp(rectext-2,"_+",2) || !strncmp(rectext-2,"-+",2)))   //timer tap signature "_#" not found, hence hasn't modded timer event previously
            {
                numbercounter++;   //count number of hits for user

                //read current parameters
                timer_duration=  timerinfo.duration;

                timer_start_day = (timerinfo.startTime>>16)&0xffff;
                timer_start_hour= (timerinfo.startTime>>8)&0xff;
                timer_start_min = (timerinfo.startTime)&0xff;

                //save current parameters to end of filename

                if (rectext-timerinfo.fileName >=57)  //if filename up to .REC is > than 81chars long
                    strncpy (timerinfo.fileName+58,"##.rec\0",7);
                else
                    strncpy (rectext,"##.rec\0",7);

                //strncpy(rectext+6,timer_duration,4);     //store original parameters

                //strncpy(rectext+2,timer_start_day,4);
                //strncpy(rectext+4,timer_start_hour,2);
                //strncpy(rectext+5,timer_start_min,2);

                g_timerPointer[i].timer_startBackup=timerinfo.startTime;
                g_timerPointer[i].durationBackup=timer_duration;
                g_timerPointer[i].BackupSignature=0xdeaf;   //store original params and flags, won't use flag in this version

                //printf("time %x,duration %d, signature %x\n",g_timerPointer[i].timer_startBackup,g_timerPointer[i].durationBackup,g_timerPointer[i].BackupSignature);

                //now adjust end parameters

                timer_duration=timer_duration+g_nend;
                timerinfo.duration =timer_duration;

                tailbackup(i);

                //printf("End Mod %s, New duration %d\n",timerinfo.fileName,timer_duration);

                if (g_defaulttuner!=4)
                {
                    if (g_defaulttuner!=3) //if older firmware
                    {
                        if(timerinfo.tuner==3) //if 3 equals newer firmware and won't work with older
                            timerinfo.tuner=g_defaulttuner; //set to default tuner for new firmware
                    }
                    else
                        timerinfo.tuner=3;
                }

                debug_counter=0;  //need this to breakout of loop, looks like conflict stuff fails with 3rd timer overlapping 2 other timers.
                while ((retval=TAP_Timer_Modify(i,&timerinfo)) >=0xFFFF0000)
                {
                    debug_counter++;
                    if (debug_counter>5)
                    {
                        ShowMessageWin("Something is wrong??!!","bailing modification, please check timers.",260);
                        break;
                    }
                    printf("end-WhileLoop %d! (Conflicts with %d)",i,retval & 0x00000FFF);

                    if ((retval&0x00000FFF)>=0 && (retval&0x00000FFF)<TAP_Timer_GetTotalNum())
                    {
                        TAP_Timer_GetInfo((retval&0x00000FFF),&timerinfo2);  //find conflicting timer and make current timer 7.3 bug fix here
                        sprintf(str,"End conflict timer %s with:",timerinfo.fileName);
                        ShowMessageWin(str,timerinfo2.fileName,120);
                        printf("Conflicting timer starttime %x\n",timerinfo2.startTime);

                        timerduration2=subtracttimes(timerinfo2.startTime , timerinfo.startTime)-1;
                        if (timerduration2<=timer_duration-g_nend)  //if conflict-1 is less than orig end, stick with orig end
                            timer_duration =timer_duration-g_nend;
                        else
                            timer_duration =timerduration2;

                        timerinfo.duration=timer_duration;
                    }
                    else
                    {
                        ShowMessageWin("Timer end conflict with multiple timers",timerinfo.fileName,120);
                        timerinfo.duration =timer_duration-g_nend;
                    }
                }
                if (retval)
                {
                    show_timer_error(retval,i);
                }
                else
                {
                    tailrestore(i);
                }
            }
        }
    }


    for (i=0;i<TAP_Timer_GetTotalNum();++i)   //stretch start time now
    {
        TAP_Timer_GetInfo(i,&timerinfo);

        if ((rectext=(char *)strstr(timerinfo.fileName,"##.rec\0")))
        {  //look for ##.rec modded signature
            //read current parameters
            timer_duration=  timerinfo.duration;

            startepgdate=timerinfo.startTime;

            timer_start_day = (startepgdate>>16)&0xffff;
            timer_start_hour= (startepgdate>>8)&0xff;
            timer_start_min = (startepgdate)&0xff;

            //now adjust start parameters
            timer_duration=timer_duration-g_nstart;

            changetimer(&timer_start_day, &timer_start_hour, &timer_start_min, g_nstart);   //g_nstart is normally -ve

            newstarttodaydate=((timer_start_day&0xffff)<<16)+((timer_start_hour&0xff)<<8)+(timer_start_min&0xff); //calc date
            //newendtodaydate=(timer_end_day&0xffff)<<16+(timer_end_hour&0xff)<<8+(timer_end_minute&0xff);


            // and save start parameters
            timerinfo.duration=timer_duration;
            timerinfo.startTime=newstarttodaydate;

            printf("StartMod %s, NewStart %x, NewDur %d\n",timerinfo.fileName,newstarttodaydate,timer_duration);

            strncpy(rectext,"-#",2); //write completed signature

            tailbackup(i);

            debug_counter=0;
            while ((retval=TAP_Timer_Modify(i,&timerinfo)) >=0xFFFF0000)
            {
                debug_counter++;
                if (debug_counter>5)
                {
                    ShowMessageWin("Something is wrong??!!","please check timers",260);
                    break;
                }
                printf("start-WhileLoop %d! (Conflicts with %d)",i,retval & 0x00000FFF);
                if ((retval&0x00000FFF)>=0 && (retval&0x00000FFF)<TAP_Timer_GetTotalNum())
                {
                    TAP_Timer_GetInfo(retval&0x00000FFF,&timerinfo2);  //find conflicting timer and make current timer 7.3 bug fix here

                    sprintf(str,"Start conflict timer %s with:",timerinfo.fileName);
                    ShowMessageWin(str,timerinfo2.fileName,120);
                    printf("Conflicting timer starttime %x\n",timerinfo2.startTime);

                    timerduration2=subtracttimes(newstarttodaydate,timerinfo2.startTime);

                    changetimer(&timer_start_day, &timer_start_hour, &timer_start_min, (timerinfo2.duration-timerduration2));
                    newstarttodaydate=((timer_start_day&0xffff)<<16)+((timer_start_hour&0xff)<<8)+(timer_start_min&0xff);
                    timer_duration=timer_duration-(timerinfo2.duration-timerduration2);

                    //TAP_SPrint(msg,"timer_duration %d",timer_duration);
                    //ShowMessageWin(timerinfo.fileName,msg,270);

                    timerinfo.duration=timer_duration;
                    timerinfo.startTime=newstarttodaydate;

                }
                else
                {
                    ShowMessageWin("Timer start conflict with multiple timers",timerinfo.fileName,120);
                    changetimer(&timer_start_day, &timer_start_hour, &timer_start_min, -g_nstart);   //g_nstart is normally -ve
                    newstarttodaydate=((timer_start_day&0xffff)<<16)+((timer_start_hour&0xff)<<8)+(timer_start_min&0xff); //calc date
                    timerinfo.startTime=newstarttodaydate;
                    timerinfo.duration=timer_duration+g_nstart;
                }
            }

            if (retval)
            {
                show_timer_error(retval,i);
            }
            else
            {
                tailrestore(i);
            }

            //find backup timer to change settings
            if (g_timerPointer[i].recurance == 0 && g_bSingleTimerRam) //onetime timer and have backups for onetime.
            {
                //int   backupcount =(g_SingleTimerRAMcount>=140)?140:g_SingleTimerRAMcount;
                //printf("backupcnt:%d,  g_SingleTimerRAMcount:%d\n",backupcount,g_SingleTimerRAMcount);

                for(j=0;j<g_SingleTimerRAMcount;j++)
                {
                    //printf("got here apply1\n");
                    if (g_BackupPointer[j].timer_start == g_timerPointer[i].timer_startBackup &&
                        g_BackupPointer[j].channel== g_timerPointer[i].channel )
                    {
                        //printf("%s modded in backup memory\n",g_BackupPointer[j].filename);
                        *(g_BackupPointer+j)=*(g_timerPointer+i);
                        break;
                    }
                }
            }


            //check for EPG entry, and change if have one
            /*
                        for (j=(dword)g_timerPointer+SIGNATURE_OFFSET_EPG; j<((dword)g_timerPointer+SIGNATURE_OFFSET_EPG+EPG_TABLE_LENGTH); j=j+0x08)
                        {
                        if (*(dword *)(j+0x4)==0x00000000) break;   //I believe that if we reach 0000s, then no more new EPG timer entries
                        if (startepgdate == *(dword *)(j+0x4) && timerinfo.svcNum==*(byte *)(j+0x1)) //if time and channel same
                        {
                        * (dword *)(j+0x4) =newstarttodaydate;
                        break;
                        }
                        }
                        */
        }
    }
    if (numbercounter)
    {
        if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
        {
            sprintf( str, "%2d Einträge", numbercounter);
            ShowMessageWin("Neue Timereinträge erweitert:",str ,210);
        }
        else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
        {
            sprintf( str, "%2d ajastusta", numbercounter);
            ShowMessageWin("Ajastuksia pidennetty:",str ,210);
        }
        else
        {
            sprintf( str, "%2d entries", numbercounter);
            ShowMessageWin("New Timer Entries Extended:",str ,210);
        }
    }
} //apply_settings


void erase_settings(void)
{
    int location_contents;
    word timer_start_day,timer_end_day;
    byte timer_start_min,timer_end_min, timer_start_hour,timer_end_hour;
    int timer_duration;
    unsigned int retval;
    long i,j;
    TYPE_TimerInfo timerinfo;
    int numbercounter=0;
    char str[300];
    char * rectext;
    //int backupcount=(g_SingleTimerRAMcount>=140)?140:g_SingleTimerRAMcount;

    for (i=0;i<TAP_Timer_GetTotalNum();i++)
    {
        TAP_Timer_GetInfo(i,&timerinfo);
        if (strstr(timerinfo.fileName,"_#.rec")||strstr(timerinfo.fileName,"-#.rec") )  //look for _#.rec modded signature
        {
            rectext=(char *)(strstr(timerinfo.fileName,".rec") -2);
            numbercounter++;   //count number of hits for user
            strncpy (rectext,".rec\0",5); //erase signature.

            /*

                        //restore EPG entry
                        for (j=(dword)g_timerPointer+SIGNATURE_OFFSET_EPG; j<((dword)g_timerPointer+SIGNATURE_OFFSET_EPG+EPG_TABLE_LENGTH); j=j+0x08)
                        {
                        if (*(dword *)(j+0x4)==0x00000000) break;   //I believe that if we reach 0000s, then no more new EPG timer entries
                        if (timerinfo.startTime == *(dword *)(j+0x4) && timerinfo.svcNum== *(byte *)(j+0x1)) //if datetime and channel same
                        {
                        * (dword *)(j+0x4) =g_timerPointer[i].timer_startBackup;
                        break;
                        }
                        }
                        */

            //restore original params and flags
            timerinfo.startTime= g_timerPointer[i].timer_startBackup;
            timerinfo.duration= g_timerPointer[i].durationBackup;
            //printf("restore time %x,duration %d, signature %x\n",g_timerPointer[i].timer_startBackup,g_timerPointer[i].durationBackup,g_timerPointer[i].BackupSignature);
            g_timerPointer[i].BackupSignature=0x0000;         //flag set to 0
            g_timerPointer[i].timer_startBackup=0x00000000;   //time reset to 0
            g_timerPointer[i].durationBackup=0x0000;          //duration set to 0
            //printf("restore wipe time %x,duration %d, signature %x\n",g_timerPointer[i].timer_startBackup,g_timerPointer[i].durationBackup,g_timerPointer[i].BackupSignature);


            tailbackup(i);
            retval=TAP_Timer_Modify(i,&timerinfo);
            if (retval)
            {
                show_timer_error(retval,i);
            }
            else tailrestore(i);

            //find backup timer to change settings
            if (g_timerPointer[i].recurance==0x0 && g_bSingleTimerRam) //onetime timer
            {
                for(j=0;j<g_SingleTimerRAMcount;j++)
                {
                    if (g_timerPointer[i].timer_start==g_BackupPointer[j].timer_startBackup &&
                        g_timerPointer[i].channel==g_BackupPointer[j].channel )
                    {
                        //printf("%s modded in backup memory\n",g_BackupPointer[j].filename);
                        *(g_BackupPointer+j)=*(g_timerPointer+i);
                        break;
                    }
                }
            }
        }
    } //for (i=0;i<TAP_Timer_GetTotalNum();i++)


    if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
    {
        sprintf( str, "%2d Einträge", numbercounter);
        ShowMessageWin("Timereinträge wieder hergestellt:",str ,330 );
    }
    else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
    {
        sprintf( str, "%2d ajastusta", numbercounter);
        ShowMessageWin("Ajastuksia palautettu:",str ,330 );
    }
    else
    {
        sprintf( str, "%2d entries", numbercounter);
        ShowMessageWin("Timer Entries Restored:",str ,330 );
    }
} //erase_settings


bool OsdActive (void)
{
    TYPE_OsdBaseInfo osdBaseInfo;
    dword* wScrn;
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

bool TV_SvcsActive (int x,int y)   // compares color of title of TV services to known value (I wrote another locating
// TAP to find out the value 0xF709).
{
    TYPE_OsdBaseInfo osdBI;
    word* pAddr;
    //dword i, x, y;

    TAP_Osd_GetBaseInfo (&osdBI);   // retrieve current OSD

    pAddr = osdBI.eAddr + 720 * y;   //even field
    if (*(pAddr + x)==0xF709)
        return TRUE;
    else
        return FALSE;
}


void DisplayTimerDetails (void)
{
    int lpos=315;                       // stores left starting-position of bar
    int sze=340;                        // stores width of bar
    int tpos=480;
    int hgt=26;
    int toffset;
    int n=0; //counter
    word year;
    byte month,day,dummy;
    char str[128];                      // temporary storage for text-outputs
    //char * rectext;
	int yellow_offset=0;  //offset used for finnish language
    rgn = 0;                            // tricky, hope it runs with newer firmware
    toffset=(hgt-20)/2;
    
	if( !_blueShow )
    {
        _blueShow = TRUE;
        _blueRgn = TAP_Osd_Create( BBOX_X, BBOX_Y, BBOX_W, BBOX_H, 0, 0 );
        TAP_Osd_Copy( _blueTempRgn, _blueRgn, 0, 0, BBOX_W, BBOX_H, 0, 0, FALSE );
        if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
		{
			yellow_offset=46;
            sprintf( str, "Verzögert Eintrag." );   //Format Text for postpone toggle
		}
        else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
		{
			yellow_offset=14;
            sprintf( str, "Lykkää ajastusta." );
		}
        else
            sprintf( str, "Postpone entry." );
        TAP_Osd_PutStringAf1419( rgn, BBOX_X+35, BBOX_Y-1, -1, str, COLOR_White, COLOR_None );      //Print to bar

        _yellowRgn = TAP_Osd_Create( YBOX_X+yellow_offset, YBOX_Y, YBOX_W, YBOX_H, 0, 0 );
        TAP_Osd_Copy( _yellowTempRgn, _yellowRgn, 0, 0, YBOX_W, YBOX_H, 0, 0, FALSE );

        if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
            sprintf( str, "Umschalt-Tuner." );	//Format Text for tuner toggle
        else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
            sprintf( str, "Vaihda viritintä." );
        else
            sprintf( str, "Toggle Tuner." );
        TAP_Osd_PutStringAf1419( rgn, YBOX_X+35+yellow_offset, YBOX_Y-1, -1, str, COLOR_White, COLOR_None );      //Print to bar
    }

    //for the filename
    TAP_SPrint( str, "%s",g_timerinfo.fileName);        //Format Text for timer details
    strncpy (str+(strlen(str)-4),"\0",1);   // basically remove .rec from filename for print by copying a null char at the "."
    if (TAP_GetSystemVar(SYSVAR_OsdLan)!=LAN_English) //finnish or german overlaps
    {
        TAP_Osd_FillBox(rgn,lpos+52,tpos+1,sze+2-54,hgt-1,COLOR_LBlue);
        TAP_Osd_Draw3dBox(rgn, lpos+52,tpos,sze+3-54,hgt,COLOR_LGrey,COLOR_DGrey);              //Draw 3D-Box around Bar
        while (TAP_Osd_GetW( str, 0, FNT_Size_1622)>286)  //truncate filename if greater then width of box
        {
            strncpy (str+(strlen(str)-(++n)),"\0",1);  //shift "\0" back by one char each time
        }
        TAP_Osd_PutStringAf1622( rgn, lpos+53, tpos+toffset, -1, str, COLOR_White, COLOR_None );        //Print to bar
    }
    else
    {
        TAP_Osd_FillBox(rgn,lpos-2,tpos+1,sze+2,hgt-1,COLOR_LBlue);
        TAP_Osd_Draw3dBox(rgn, lpos-3,tpos,sze+3,hgt,COLOR_LGrey,COLOR_DGrey);              //Draw 3D-Box around Bar
        while (TAP_Osd_GetW( str, 0, FNT_Size_1622)>340)  //truncate filename if greater then width of box
        {
			strncpy (str+(strlen(str)-(++n)),"\0",1);  //shift "\0" back by one char each time
		}
		TAP_Osd_PutStringAf1622( rgn, lpos, tpos+toffset, -1, str, COLOR_White, COLOR_None );       //Print to bar
	}


	//for the date & tuner
	TAP_ExtractMjd((g_timerinfo.startTime>>16)&0xffff, &year, &month, &day, &dummy);

	TAP_Osd_FillBox(rgn,lpos+150,tpos+28,sze-150,hgt-1,COLOR_LBlue);

	if (strstr(g_timerinfo.fileName,"_+.rec")||strstr(g_timerinfo.fileName,"-+.rec"))  //if we have "postponed stamp"
	{
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			TAP_SPrint( str, "Tuner%1d  gestoppt",g_timerinfo.tuner+1);     //Format Text for timer details
		else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
			TAP_SPrint( str, "Viritin%1d  LYKÄTTY",g_timerinfo.tuner+1);    //Format Text for timer details
		else
			TAP_SPrint( str, "Tuner%1d POSTPONED",g_timerinfo.tuner+1);     //Format Text for timer details
	}
	else //not postponed
	{
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			TAP_SPrint( str, "Tuner%1d  %02d/%02d/%d",g_timerinfo.tuner+1,day,month,year);     //Format Text for timer details
		else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
			TAP_SPrint( str, "Viritin%1d  %02d/%02d/%d",g_timerinfo.tuner+1,day,month,year);    //Format Text for timer details
		else
			TAP_SPrint(str, "T%1d %02d/%02d/%d %02d:%02d%s",g_timerinfo.tuner+1,day,month,year,
			((g_timerinfo.startTime>>8)&0xff)%12==0?12:((g_timerinfo.startTime>>8)&0xff)%12,g_timerinfo.startTime&0xff,((g_timerinfo.startTime>>8)&0xff)>=12?"pm":"am");  
	}      

	TAP_Osd_Draw3dBox(rgn, lpos+149,tpos+27,sze-149,hgt,COLOR_LGrey,COLOR_DGrey);               //Draw 3D-Box around Bar
	TAP_Osd_PutStringAf1622( rgn, lpos+152, tpos+27+toffset, -1, str, COLOR_White, COLOR_None );        //Print to bar


} //DisplayTimerDetails


void TSRCommanderConfigDialog()
{
    g_nselected=_num_items_;
    ShowOptionsWindow();
}

bool TSRCommanderExitTAP (void)
{
    HideOptionsWindow();
    TAP_Osd_Delete( _blueTempRgn );  //delete button memory
    TAP_Osd_Delete( _yellowTempRgn );
    if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
       ShowMessageWin(_PROGRAM_NAME_,"wird beendet",150);
    else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
        ShowMessageWin("Suljetaan",_PROGRAM_NAME_,150);
    else
        ShowMessageWin("Quitting",_PROGRAM_NAME_,150);
    TAP_Delay(50);
    TAP_Osd_Delete (g_nrgnOpts);

    if(g_bSingleTimerRam)
    {
        TAP_MemFree(g_BackupPointer);
    }
    return TRUE;
}



void ShowOptionsWindow (void)
{
    char str1[128]; //,str2[50],str3[60],str4[50];

    if (!g_nrgnOpts)
    {
        TAP_ExitNormal();      // leaving normal receiver operation, receiver hides its OSDs
        g_nrgnOpts= TAP_Osd_Create (0, 0, 720, 576, 0, FALSE);
    }
    if (g_bOptsWinShown)
    {
        TAP_Win_Delete (&g_winOpts);
    }
    g_bOptsWinShown = TRUE;

    TAP_Win_Create (&g_winOpts, g_nrgnOpts, 185, ((241-_num_items_*11)<20)?20:(241-_num_items_*11), 350, ((_num_items_*22)>440)?440:_num_items_*22, FALSE, FALSE);
    TAP_Win_SetDefaultColor (&g_winOpts);
    TAP_Win_SetTitle (&g_winOpts, _PROGRAM_NAME_, 0, FNT_Size_1622);

    switch (TAP_GetSystemVar(SYSVAR_OsdLan))
    {
	case LAN_German:
		sprintf( str1, "Timer-Vorlauf: %3d Min.\0", g_nstart);
		TAP_Win_AddItem (&g_winOpts, str1);
		sprintf( str1, "Timer-Nachlauf: %3d Min.\0", g_nend);	
		TAP_Win_AddItem (&g_winOpts, str1);
		TAP_Win_AddItem (&g_winOpts, "Timer JETZT erweitern...");
		TAP_Win_AddItem (&g_winOpts, "Timer JETZT wiederherstellen...");
		TAP_Win_AddItem (&g_winOpts, "Timer JETZT sortieren...");
		TAP_Win_AddItem (&g_winOpts, "Timer "BACKUP_FILE" Gelöschte...");
		TAP_Win_AddItem (&g_winOpts, g_bchecktimer?"Zukünftige Timer prüfen: EIN":"Zukünftige Timer prüfen: AUS");
		TAP_Win_AddItem (&g_winOpts, g_brecfunction?"EPG-Timer erweitern: EIN":"EPG-Timer erweitern: AUS");
		TAP_Win_AddItem (&g_winOpts, g_btimerfunction?"Timer-Erweiterung: AUTO" : "Timer-Erweiterung: MAN");
		TAP_Win_AddItem (&g_winOpts, g_bsortfunction?"Timer-Sortierung: AUTO" : "Timer-Sortierung: MAN");
		sprintf( str1, "Bevorzugter Tuner: %d\0", g_defaulttuner+1);
		TAP_Win_AddItem (&g_winOpts, (g_defaulttuner!=4)?str1:"Bevorzugter Tuner: Nicht ändern!");
		sprintf( str1, "%d zukünftige Timer repariert. RESET...\0", g_faulty);
		TAP_Win_AddItem (&g_winOpts, str1);
		TAP_Win_AddItem (&g_winOpts, _PROGRAM_NAME_ " beenden");
		TAP_Win_AddItem (&g_winOpts, "Nächstes TAP");
		break;


	case 15: //finnish
		sprintf( str1, "Aloitusajan muutos: %3d min", g_nstart);
		TAP_Win_AddItem (&g_winOpts, str1);
		sprintf( str1, "Lopetusajan muutos: %3d min", g_nend);
		TAP_Win_AddItem (&g_winOpts, str1);
		TAP_Win_AddItem (&g_winOpts, "Pidennä ajastuksia");
		TAP_Win_AddItem (&g_winOpts, "Poista ajastusten pidennykset");
		TAP_Win_AddItem (&g_winOpts, "Järjestä ajastukset");
		TAP_Win_AddItem (&g_winOpts, "Ajastusten "BACKUP_FILE" tyhjentää.");
		TAP_Win_AddItem (&g_winOpts, g_bchecktimer?"Tarkista siirtyneet ajastukset: Käytössä":"Tarkista siirtyneet ajastukset: Ei käytössä");
		TAP_Win_AddItem (&g_winOpts, g_brecfunction?"Ajastusten pidennys EPG:ssä: Käytössä":"Ajastusten pidennys EPG:ssä: Ei käytössä");
		TAP_Win_AddItem (&g_winOpts, g_btimerfunction?"Ajastusten pidennys: Automaattisesti":"Ajastusten pidennys: Manuaalisesti");
		TAP_Win_AddItem (&g_winOpts, g_bsortfunction?"Ajastusten järjestäminen: Automaattisesti":"Ajastusten järjestäminen: Manuaalisesti");
		sprintf( str1, "Oletusviritin: %d\0", g_defaulttuner+1);
		TAP_Win_AddItem (&g_winOpts, (g_defaulttuner!=4)?str1:"Oletusviritin: Älä vaihda");
		sprintf( str1, "Siirtyneitä ajastuksia korjattu %d. Nollaa?", g_faulty);
		TAP_Win_AddItem (&g_winOpts, str1);
		TAP_Win_AddItem (&g_winOpts, "Sulje "_PROGRAM_NAME_);
		TAP_Win_AddItem (&g_winOpts, "Seuraava TAP");
		break;

	default: //all other languages
		sprintf( str1, "Start delay: %3d mins\0", g_nstart);
		TAP_Win_AddItem (&g_winOpts, str1);
		sprintf( str1, "End delay: %3d mins\0", g_nend);
		TAP_Win_AddItem (&g_winOpts, str1);
		TAP_Win_AddItem (&g_winOpts, "Apply timer extend");
		TAP_Win_AddItem (&g_winOpts, "Restore timers to normal");
		TAP_Win_AddItem (&g_winOpts, "Sort timers");
		TAP_Win_AddItem (&g_winOpts, "Purge "BACKUP_FILE" timers.");
		TAP_Win_AddItem (&g_winOpts, g_bchecktimer?"Check future timers: ON":"Check future timers: OFF");
		TAP_Win_AddItem (&g_winOpts, g_brecfunction?"EPG Timer Extend: ON":"EPG Timer Extend: OFF");
		TAP_Win_AddItem (&g_winOpts, g_btimerfunction?"Timer Extend: AUTO":"Timer Extend: MANUAL");
		TAP_Win_AddItem (&g_winOpts, g_bsortfunction?"Timer Sort: AUTO":"Timer Sort: MANUAL");
		sprintf( str1, "Default Tuner: %d\0", g_defaulttuner+1);
		TAP_Win_AddItem (&g_winOpts, (g_defaulttuner!=4)?str1:"Default Tuner: Don't change");
		sprintf( str1, "%d future timer repairs, reset?\0", g_faulty);
		TAP_Win_AddItem (&g_winOpts, str1);
		TAP_Win_AddItem (&g_winOpts, "Exit "_PROGRAM_NAME_);
		TAP_Win_AddItem (&g_winOpts, "Next TAP");
		break;
    }
    TAP_Win_SetSelection (&g_winOpts, g_nselected);
} //ShowOptionsWindow


/*****************************************************************************/
/* Callbacks */
/*****************************************************************************/
dword TAP_EventHandler( int event, dword param1, dword param2 )
{
    static bool bisTimerDetailsShown = FALSE;    // stores flag for drawn timer details
    static bool btimerliststate = FALSE;     // stores timer state
    static bool btvservices = FALSE;      // stores TV services flag (when editing timers).
    static bool bgreendelete = FALSE;    // tracks deleted timer
    static bool bYesNo = FALSE;         //  tracks yes or no answer
    static bool bmodTimertoInfinity = FALSE; //tracks whether timer is modded to infinity
    static bool b_was_in_EPG=FALSE;  //tracks whether in EPG view
    static bool b_recordhitinEPG=FALSE;
    static int guide_totaltimers = 0;
    char * rectext; //a pointer to char to make changing filename easier
    unsigned int retval; //returned value for timer modification
    int tuner_counter; //used to track tuner toggle
    int number; //used to track number of menu entries, save me calculating option window height.
	dword eventdelta;
    /***********************************************KEY HIT*********************************/
    if( event == EVT_KEY )  //key pressed
    {
        dword dwState, dwSubState;
        int totaltimers=0;

        TAP_GetState (&dwState, &dwSubState);
        if (dwState == STATE_Normal && dwSubState == SUBSTATE_Normal)
        {
            if(param1 == RKEY_Guide)
            {
                b_was_in_EPG=TRUE;
                guide_totaltimers=totaltimers;
                return param1;
            }
        }

        //if(param1 == RKEY_Uhf){
        //  apply_settings();
        //  return 0;
        //}

        if(dwState == STATE_Epg && dwSubState == SUBSTATE_Epg )
        {
            if (param1 == RKEY_Record || param1 == RKEY_Ok) //timer possibly deleted/edited, lets delete all of the ones in the future
            {
                b_recordhitinEPG=TRUE;
            }

            if (param1 == RKEY_Record && g_brecfunction)
            {
                if(!bMakerRecKey)
                {
                    bMakerRecKey=TRUE;
                    TAP_GenerateEvent(EVT_KEY,RKEY_Record,RKEY_Record);
                }
                else
                {
                    bMakerRecKey=FALSE;
                    return param1;
                }

                bMaker0Key=TRUE;
                TAP_GenerateEvent(EVT_KEY,RKEY_0,RKEY_0);
                return 0;
            }

            if (param1 == RKEY_0 && bMaker0Key==TRUE)
            {
                bMaker0Key=FALSE;
                apply_settings();
                TAP_GenerateEvent(EVT_KEY,RKEY_Guide,RKEY_Guide);
                TAP_GenerateEvent(EVT_KEY,RKEY_Guide,RKEY_Guide);
                return 0;
            }

        }//state epg

        if(dwState == STATE_Menu && dwSubState == SUBSTATE_TimerMenu)
        {
            //bisTimerDetailsShown = TRUE;      // set flag to get pie drawn now we are in timer menu
            btimerliststate = TRUE;   // set due to bug with some f/w versions that report state/substate of 0 in timer edit
            //DisplayTimerDetails();                // draw the pie
        }

        if (btimerliststate && !btvservices && !bgreendelete )
        {
            int i;
            totaltimers=TAP_Timer_GetTotalNum();
            switch (param1)
            {
                case RKEY_Exit:                  //exit or menu to leave the timerlist
                case RKEY_Menu:                 
                //g_bokpressed=FALSE;
                    btimerliststate = FALSE;
                    if (g_bsortfunction) sort_timers();
                    g_timerposition=0;
                    bisTimerDetailsShown = FALSE;
                    bgreendelete = FALSE;
                    if (g_btimerfunction) apply_settings();
                    if (_blueShow)
                    {    //delete region with blue button
                        _blueShow=FALSE;
                        TAP_Osd_Delete( _blueRgn );
                        TAP_Osd_Delete( _yellowRgn );
                    }
                    g_last_event=1; //to trigger instant future timer check
                    return param1;
                    case RKEY_Ok:  //ok opens timer
                    if (g_bSingleTimerRam && g_bchecktimer && totaltimers!=0)
                    {
                        for(i=0;i<g_SingleTimerRAMcount;i++)
                        {
                            if (g_timerPointer[g_timerposition].recurance==0x0 &&  //if current OK'd timer is one time
                                g_timerPointer[g_timerposition].timer_start==g_BackupPointer[i].timer_start && //and start&channel matches
                                g_timerPointer[g_timerposition].channel==g_BackupPointer[i].channel )
                            {
                                //mark the timer start_start to be 0x0000001 which effectively "deletes" it, 0x1 will avoid matching timerbackup from extended timers
                                printf("%s deleted from backup memory\n",g_BackupPointer[i].filename);
                                g_BackupPointer[i].timer_start=0x000000;  //effectively deleting the backuppointer.
                                save_backup_timers();
                                break;
                            }
                        }
                    }
                    bisTimerDetailsShown = FALSE;  //to get redraw happening
                    return param1;

                    case RKEY_Red: //red inserts timer
                    g_timerposition=totaltimers;  //selects last entry when adding
                    g_last_event=1;
                    bisTimerDetailsShown = FALSE;  //to get redraw happening
                    return param1;

					case RKEY_Green:
						bgreendelete=TRUE;  //setup so that left/right doesn't change timer selected.
						return param1;
                        
					case RKEY_ChDown:
                        if (totaltimers!=0)
                        {
 						    g_timerposition=(g_timerposition+1)%totaltimers;
						    bisTimerDetailsShown = FALSE;
                        }
						return param1;

					case RKEY_ChUp:
                        if (totaltimers!=0)
                        {
						if (g_timerposition==0)
							g_timerposition=totaltimers-1;
						else
							g_timerposition--; //=(g_timerposition-1)%TAP_Timer_GetTotalNum();

						bisTimerDetailsShown = FALSE;
                        }
						return param1;

					case RKEY_VolDown:
                        if (totaltimers!=0)
                        {
						g_timerposition=g_timerposition-9;
						if (g_timerposition<0) g_timerposition=0;
						bisTimerDetailsShown = FALSE;
                        }
						return param1;

					case RKEY_VolUp:
                        if (totaltimers!=0)
                        {
						    g_timerposition=g_timerposition+9;
						    if (g_timerposition>(totaltimers-1)) g_timerposition=totaltimers-1;
						    bisTimerDetailsShown = FALSE;
                        }
						return param1;

					case RKEY_Yellow:
                        if (totaltimers!=0)
                        {
							tuner_counter=(g_timerinfo.tuner+1)%4;   //read existing tuner
							if (tuner_counter==2) tuner_counter++;
							g_timerPointer[g_timerposition].tuner=tuner_counter;
							bisTimerDetailsShown = FALSE;
							return 0;   //don't want to send the blue key at this stage really.
						}

					case RKEY_Blue:  //toggle postpone timer mode
                        if (totaltimers!=0)
                        {
							TAP_Timer_GetInfo(g_timerposition,&g_timerinfo);  //get timer info again just to make sure
							if (strstr(g_timerinfo.fileName,"_+.rec")||strstr(g_timerinfo.fileName,"-+.rec\0"))  //look for _+.rec modded signature
							{
								rectext=(char *)(strstr(g_timerinfo.fileName,".rec")-2);
								strncpy (rectext,".rec\0",5); //erase signature.
								g_timerinfo.startTime-=(7273<<16);  //get 20 years back to some decade near now ;-)

							}
							else if ((rectext= (char *)strstr(g_timerinfo.fileName,".rec\0")))
							{  //else mustn't have the sig
								g_timerinfo.startTime+=(7273<<16); //so add some time (20 years!) to timer
								//save infinity flag to end of filename
								if ((strstr(rectext-2,"-#") || strstr(rectext-2,"_#")) && rectext-g_timerinfo.fileName >=55)
									strncpy (g_timerinfo.fileName+56,"-#-+.rec\0",7);
								else if (rectext-g_timerinfo.fileName >=57)  //if filename up to .REC is >= than 57chars long
									strncpy (g_timerinfo.fileName+58,"-+.rec\0",7);
								else
									strncpy (rectext,"-+.rec\0",7);
							}
							else //shouldn't be an else, something must have gone really wrong if there has been!
							{
								return param1;  //neither _+ or .rec found, something not right so just returning key
							}

							tailbackup(g_timerposition);
							retval=TAP_Timer_Modify(g_timerposition,&g_timerinfo);
							if (retval)
							{
								show_timer_error(retval,g_timerposition);
							}
							else tailrestore(g_timerposition);

							bisTimerDetailsShown = FALSE;
							return 0;   //don't want to send the blue key at this stage really.
							//end blue key
						}
			}//end case
		}

        else if (bgreendelete && (totaltimers=TAP_Timer_GetTotalNum())!=0)
        {
            if (param1==RKEY_Exit||param1==RKEY_Ok)  //exit or OK hit from a delete
            {
                if (bYesNo)  //if timer is deleted
                {
                    if (g_bSingleTimerRam && g_bchecktimer)
                    {
                        int i;
                        for(i=0;i<g_SingleTimerRAMcount;i++)
                        {
                            if (g_timerPointer[g_timerposition].recurance==0x0 &&  //if current deleted timer is one time
                                g_timerPointer[g_timerposition].timer_start==g_BackupPointer[i].timer_start &&
                                g_timerPointer[g_timerposition].channel==g_BackupPointer[i].channel )
                            {
                                //mark the timer start_start to be 0x0000001 which effectively "deletes" it, 0x1 will avoid matching timerbackup from extended timers
                                printf("%s deleted from backup memory\n",g_BackupPointer[i].filename);
                                g_BackupPointer[i].timer_start=0x000000;  //effectively deleting the backuppointer.
                                save_backup_timers();
                                break;
                            }
                        }
                    }
                    if (g_timerposition==(totaltimers-1)) g_timerposition--;  //if last timer deleted
                    bYesNo=FALSE;
                }
                bgreendelete=FALSE;  //to leave the "YES/NO" delete state
                bisTimerDetailsShown = FALSE;
            }

            if (param1==RKEY_VolDown||param1==RKEY_VolUp)
            {
                bYesNo=!bYesNo;
            }
            return param1;
        }


        if (g_bOptsWinShown) // handling keys when options window is shown
        {
            if (param1 == RKEY_Ok ||param1==RKEY_VolUp ||param1==RKEY_VolDown) // ok-key confirms the g_nselected list-entry
            {
                int iSelection = TAP_Win_GetSelection (&g_winOpts);
                g_nselected=iSelection;

                switch (iSelection)
                {
                    case 0: g_nstart=(param1==RKEY_VolDown)? (g_nstart-1)%61:(g_nstart+1)%61; ShowOptionsWindow(); break;
                    case 1: g_nend=(param1==RKEY_VolDown)? (g_nend-1)%61:(g_nend+1)%61; ShowOptionsWindow(); break;
                    case 2: HideOptionsWindow(); apply_settings();  break;
                    case 3: HideOptionsWindow(); erase_settings();  break;
                    case 4: HideOptionsWindow();
                        sort_timers();
                        if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
                            ShowMessageWin("Timer sind jetzt sortiert", "nach Datum & Uhrzeit", 180);
                        else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
                            ShowMessageWin("Ajastukset järjestetty", "päiväyksen ja ajan mukaan.", 180);
                        else
                            ShowMessageWin("Timers are now sorted", "by date & time.", 180);
                        break;
					case 5: if (g_bSingleTimerRam)  
							{
								g_SingleTimerRAMcount=0;
								memset((byte *)g_BackupPointer,0x0,512-(140*EPG_RECORD_LENGTH)%512 + 140*EPG_RECORD_LENGTH);  //clear all backup ram (make it a multiple of 512 so write above is clean too).
								save_backup_timers();
								g_last_event=0;
							}
							HideOptionsWindow();
							return 0;
                    case 6: g_bchecktimer=!g_bchecktimer;
                        ShowOptionsWindow(); break;
                    case 7: g_brecfunction=!g_brecfunction;
                        ShowOptionsWindow();  break;
                    case 8: g_btimerfunction=!g_btimerfunction;
                        ShowOptionsWindow(); break;
                    case 9: g_bsortfunction=!g_bsortfunction;
                        ShowOptionsWindow(); break;
                    case 10: change_defaulttuner(param1); ShowOptionsWindow();   break;
                    case 11: g_faulty=0;
                        ShowOptionsWindow(); break;
	                case 12:
                        TSRCommander.ExitTAP = TRUE;
                        return 0;
                    case 13: HideOptionsWindow(); return RKEY_Exit;          // cascades exit-key to other TAP's
                    }
                return 0;
            }
            else if (param1 == RKEY_Exit) // exit-key hides the options window
            {
                HideOptionsWindow ();
                return 0; // exit without Cascading
            }
            else
            {
                TAP_Win_Action (&g_winOpts, param1); // send all other key's to menu-Win
                return 0;
            }
		}
        else // handling keys when options window is hidden
        {
            if (param1 == RKEY_Exit && dwState == STATE_Normal && dwSubState == SUBSTATE_Normal && !OsdActive()) // exit-key shows the options window
            {
                g_nselected=_num_items_;
                ShowOptionsWindow();
                return 0;
            }
        }

        return param1;
    } //KEY HIT
    /**************************************************IDLE*********************************/
    if( event == EVT_IDLE )
    {
        dword nowtime, eventdelta;
        int i;
        dword dwState, dwSubState;
        //word mjd;
        //byte hour,min,sec;
        /*
                if (dwState == STATE_Normal)        // has user exited any menu
                {
                btimerliststate = FALSE;        // erase timerlist flag (seems that state isn't erase unless put this here)
                }
                */
        if (g_bloadoptions)
        {
            dword dummy;
            dword availHeapSize;
            if (TAP_Hdd_Exist(INI_FILE))
            {
                g_btapdirinifile=TRUE;
            }
            LoadOptions();         //
            //setup regions for the buttons
            _blueTempRgn = TAP_Osd_Create( BBOX_X, BBOX_Y, BBOX_W, BBOX_H, 0, OSD_Flag_MemRgn );
            TAP_Osd_PutGd( _blueTempRgn, 0 , 0, &_bluebuttonGd, TRUE );
            _yellowTempRgn = TAP_Osd_Create( YBOX_X, YBOX_Y, YBOX_W, YBOX_H, 0, OSD_Flag_MemRgn );
            TAP_Osd_PutGd( _yellowTempRgn, 0 , 0, &_yellowbuttonGd, TRUE );

            TAP_MemInfo(&dummy, &dummy, &availHeapSize);
            if (availHeapSize<(140*EPG_RECORD_LENGTH*2)) //memx2 for file save too
            {
                g_bSingleTimerRam=FALSE;
                if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
                    ShowMessageWin(_PROGRAM_NAME_" Nicht genügend RAM","für einzelne Timer-Unterstützung",210 );
                else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
                    ShowMessageWin(_PROGRAM_NAME_" Muisti ei riitä kerta-ajastusten","varmuuskopiointiin",210 );
                else
                    ShowMessageWin(_PROGRAM_NAME_" Not enough RAM","for single timer support",210 );
            }
            else
            {
                g_bSingleTimerRam=TRUE;
                //printf("got here before the malloc\n");
                g_BackupPointer=(TYPE_TimerRAM *)TAP_MemAlloc(512-(140*EPG_RECORD_LENGTH)%512 + 140*EPG_RECORD_LENGTH);  //reserve RAM for one time timer backup (make multiple of 512).
                //printf("got here after the malloc %x \n",(dword) g_BackupPointer);
                load_backup_timers();
                save_backup_timers();//rebuild the file when loaded straight away.
            } //memory allocation

            g_bloadoptions=FALSE;  //no need to load again
            //if (TAP_GetTick() > 4000  && !OsdActive()) //if after 40secs after startup, and nothing else on the screen
            //  ShowOptionsWindow();
        }//loadoptions

        TSRCommanderWork();

        nowtime=TAP_GetTick();
        TAP_GetState(&dwState, &dwSubState);
        eventdelta=nowtime-g_last_event;
        if (eventdelta<0) eventdelta=-eventdelta;  // if evendelta integer overflows.

        if ((nowtime%40)==39)  //every 0.4 secs may as well check timer details etc
        {
            if (g_refreshtime && nowtime>=g_refreshtime && dwState == STATE_Normal && (dwSubState==0 || dwSubState==SUBSTATE_Normal))  //routine used to clear showmessagewin windows now.
            {
                //printf("idle %d, now %d, now-gref %d\n",g_refreshtime,nowtime,nowtime-g_refreshtime);
                //delete screen only if state=normal
                TAP_Osd_Delete (g_nrgnOpts);
                g_nrgnOpts=0;
                g_refreshtime=0;
            }
            if (btimerliststate)   //if in timerlist state, check to see whether services active (keyboard entry or timer entry)
            {
                if (TV_SvcsActive(168,138)||TV_SvcsActive(162,140)||TV_SvcsActive(164,138)||TV_SvcsActive(168,114)) //if the TV Services or keyboard panel is up
                    btvservices=TRUE;
                else
                    btvservices=FALSE;
            }

            if(dwState == STATE_Menu && dwSubState == SUBSTATE_TimerMenu && !bisTimerDetailsShown && !btvservices)
            {
                bisTimerDetailsShown = TRUE;            // set flag so doesn't keep displaying
				if (TAP_Timer_GetTotalNum()>0)
				{
					TAP_Timer_GetInfo(g_timerposition,&g_timerinfo);  //get timer info
					DisplayTimerDetails();   //display timer info
				}
                else g_timerposition=0;
            }

            if (dwState == STATE_Normal &&  dwSubState == SUBSTATE_Normal)  // are we in normal-state
            {
                int tvsvc, radiosvc;
                TAP_Channel_GetTotalNum( &tvsvc,&radiosvc );
                if (g_bSingleTimerRam && (g_total_tvSvc!=tvsvc || g_total_radSvc!=radiosvc)) //person has rescanned or deleted channels
                {
                    printf("Channel scan redone, deleting onetime backup\n");
                    TAP_Hdd_Delete( BACKUP_FILE );

                    load_backup_timers();
                    g_total_tvSvc=tvsvc;
                    g_total_radSvc=radiosvc;
                }

                if (g_bSingleTimerRam && g_bchecktimer)
                {
                    if ((b_recordhitinEPG && TAP_Timer_GetTotalNum()<=guide_totaltimers))  //if ok/rec was hit while in epg and timers less or same
                    {
                        int i;
                        byte hour,min,dummy;
                        word todaydate;

                        TAP_GetTime(&todaydate, &hour, &min, &dummy);
                        for(i=0;i<g_SingleTimerRAMcount;i++)
                        {
                            if (
                                ((( g_BackupPointer[i].timer_start >>16)&0xffff) > todaydate) ||
                            ((((g_BackupPointer[i].timer_start >>16)&0xffff)==todaydate) && ((g_BackupPointer[i].timer_start&0xffff) > ((hour<<8) | (min))) )
                            )
                            g_BackupPointer[i].timer_start=0x000000;  //effectively deleting the backuppointer.
                        }
                        eventdelta=6001;
                        printf("timer_extend: Deleted all future backups >now as we don't know if any were deleted/edited in guide\n");
                    }
                }

                b_recordhitinEPG=FALSE;
                if (b_was_in_EPG)
                {
                    b_was_in_EPG=FALSE;
                    if (TAP_Timer_GetTotalNum()>guide_totaltimers) eventdelta=6001; //so check for new timers instantly
                }
                if (g_bchecktimer && eventdelta>6000)  //if 60seconds has passed
                {
                    check_timer();
                    g_last_event=nowtime;
                }
            }
        }

        return param1;
    }  //IDLE
    return param1;  //full routine
}
/*****************************************************************************/
/* TAP_main */
/*****************************************************************************/

int TAP_Main(void)
{
    int i;
    TAP_Channel_GetTotalNum( &g_total_tvSvc, &g_total_radSvc );
    if (g_total_tvSvc==0) return 0;   //bail if no toppy TV channels (factory reset would cause this for example).

    if ( TAP_GetTick() > 6000 && !OsdActive())
    {
        if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			ShowMessageWin(_PROGRAM_NAME_,"wird gestartet",100);	// show user TAP has started if manually started
		else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
            ShowMessageWin("Käynnistetään ",_PROGRAM_NAME_,100);    // show user TAP has started if manually started
        else
            ShowMessageWin("starting ",_PROGRAM_NAME_,100); // show user TAP has started if manually started
    }

    //printf("memfoundaddr=%x, what it points to=%x\n",(dword)g_timerPointer,*g_timerPointer);

    if (!(g_timerPointer=(TYPE_TimerRAM *)searchRAM()))
    {
        //if (!LoadSignatureAddr())
        //{
        if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			ShowMessageWin("Signaturadresse kann nicht geladen werden","Programm wird beendet...",250);
        else if (TAP_GetSystemVar(SYSVAR_OsdLan)==15)
            ShowMessageWin("Tunnisteen osoitteen avaaminen epäonnistui","Suljetaan...",250);
        else
            ShowMessageWin("can't load signature address","quitting...",250);
        //}
        return 0;  /* 0 means return/quit TAP, 1 means stay TSR */
    }
    TSRCommanderInit( 0, TRUE );        // setup the values for TSRCommander
    g_bloadoptions=TRUE; //going to try this, as some have complained can't load multiple TAPs
    //so I will wait till an idle period to access the HDD to load the settings

	return 1; //if signature, or memory found, then OK to keep going, so return 1
}


