#include "tap.h"

#define ID_PIPSwitch 0x81000005
#define _PROGRAM_NAME_ "ViewMaster3"

#define COLOR_Orange RGB (31, 16, 0)
#define COLOR_DarkOrange RGB (16, 8, 0)
#define COLOR_OrangeYellow RGB (31, 23, 0)
#define COLOR_MidRed RGB (24, 0, 0)
#define COLOR_MidGreen RGB (0, 24, 0)
#define COLOR_Cream RGB (31, 31, 24)
// #define COLOR_ListBack RGB (16, 16, 20)
#define COLOR_ListBack COLOR_Blue
#define COLOR_ForeClock COLOR_Black
#define COLOR_BackClock COLOR_OrangeYellow
#define FNT_Size_Clock FNT_Size_1622
// #define strClockFormat "03/05/2004 23:59:59 L100 Q100" // for calculating width of clock display box
// #define strClockFormat "03/05/2004 23:59:59, 99999MB (99.9%) free, 99 rec, C9" // for calculating width of clock display box
// #define strClockFormat "03/05/2004 23:59:59, 99999MB (99.9%) free, 99 rec, C9, PLAY 0:59:59 (0:59:59)" // for calculating width of clock display box
#define KeyToggleClock RKEY_Mute
/*
#define COLOR_ForePlayStatus COLOR_DarkGreen
#define COLOR_ForeRecStatus COLOR_DarkRed
#define COLOR_BackPlayStatus COLOR_White
#define COLOR_BackRecStatus COLOR_White
*/
#define COLOR_ForePlayStatus COLOR_White
#define COLOR_ForeRecStatus COLOR_White
#define COLOR_BackPlayStatus COLOR_MidGreen
#define COLOR_BackRecStatus COLOR_MidRed
#define FNT_Size_PlayRec FNT_Size_1622

// #define NumTimerEntryFields 4
// #define NumTimerEntryFields 7
#define NumTimerEntryFields 6  // file name entry treated separately
#define TimerFieldIndexStart 2  // start entry at start time field
// #define MaxTimerEntryFieldLen 4
#define MaxTimerEntryFieldLen 9
#define FNT_Size_TimerHead FNT_Size_1622
#define FNT_Size_TimerList FNT_Size_1622
#define FNT_Size_TimerEntry FNT_Size_1622
#define FNT_Size_TimerError FNT_Size_1622
#define TimerWinX 0
#define TimerWinY 24
#define TimerWinW 720
// #define TimerWinH 528
#define TimerHeadH 24
#define TimerListYincrement 24
#define TimerEntryH 24
#define TimerEntryFieldXGap 10
#define TimerErrorH 24
// #define TimerErrorYoffset TimerWinH - TimerErrorH
// #define TimerEntryFileNameYoffset TimerErrorYoffset - TimerEntryH
// #define TimerEntryYoffset TimerEntryFileNameYoffset - TimerEntryH
// #define TimerListEntriesPerScreen (TimerEntryYoffset - TimerHeadH) / TimerListYincrement
#define TimerWinXMarL 50
#define TimerWinXMarR 680
#define COLOR_ForeTimerHead COLOR_Black
#define COLOR_ForeTimerListNormal COLOR_White
// #define COLOR_ForeTimerListHighlight COLOR_Yellow
#define COLOR_ForeTimerError COLOR_Red
#define COLOR_ForeTimerEntryFixed COLOR_White
#define COLOR_ForeTimerEntryFixedNext COLOR_Green
#define COLOR_ForeTimerEntryFixedError COLOR_Red
#define COLOR_ForeTimerEntryVble COLOR_Yellow
// #define COLOR_BackTimerHead COLOR_Cyan
#define COLOR_BackTimerHead COLOR_OrangeYellow
// #define COLOR_BackTimerList COLOR_User6
// #define COLOR_BackTimerList COLOR_Gray
#define COLOR_BackTimerListNormal COLOR_ListBack
// #define COLOR_BackTimerListHighlight COLOR_DarkCyan
#define COLOR_BackTimerError COLOR_White
#define COLOR_BackTimerEntry COLOR_DarkGray
#define TimerFixedHeader "ViewMaster Timer"
// #define KeyToggleTimerMode RKEY_Sleep
#define KeyToggleTimerMode RKEY_Teletext
#define TimerReservationTypeDefault RESERVE_TYPE_Onetime
#define RAM_start 0x81800000
// #define RAM_STOP  0x83FFFFFF
#define RAM_STOP  0x82FFFFFF
// #define SIGNATURE_OFFSET 0x54        //offset to 1st timer record that generally starts 0x140 or 0x0040.
#define SIGNATURE_OFFSET -0x14
#define EPG_RECORD_LENGTH 0x88
#define printf TAP_Print
#define MAX_TIMERS 70
#define TimerListSortOption0 "Date/Time"
#define TimerListSortOption1 "File Name"
#define TimerListSortOption2 "Timer Index"

// #define KeyToggleFileMode RKEY_TvSat
#define KeyToggleFileMode RKEY_TvRadio
// #define COLOR_OrangeYellow RGB (31, 23, 0)
#define FNT_Size_FileHead FNT_Size_1622
#define FNT_Size_FileList FNT_Size_1622
#define FNT_Size_FileError FNT_Size_1622
#define FileWinX 0
#define FileWinY 24
#define FileWinW 720
// #define FileWinH 528
#define FileHeadH 24
#define FileListYincrement 24
#define FileErrorH 24
// #define FileErrorYoffset FileWinH - FileErrorH
// #define FileNameEntryYoffset FileErrorYoffset - NameEntryH
// #define FileListEntriesPerScreen (FileErrorYoffset - FileHeadH) / FileListYincrement
//#define FileWinXMarL 50
#define FileWinXMarL 40
#define FileWinXMarR 680
#define COLOR_ForeFileHead COLOR_Black
// #define COLOR_ForeFileListNormal COLOR_White
// #define COLOR_ForeFileListNormal COLOR_Yellow
// #define COLOR_ForeFileListSelected COLOR_Yellow
#define COLOR_ForeFileError COLOR_Red
#define COLOR_ForeFileListFolder COLOR_White
#define COLOR_ForeFileListTs COLOR_Cream
// #define COLOR_ForeFileListTs COLOR_Cream
// #define COLOR_ForeFileListPlaying COLOR_DarkGreen
// #define COLOR_ForeFileListRecording COLOR_DarkRed
// #define COLOR_ForeFileListPlayingRecording COLOR_OrangeYellow
// #define COLOR_ForeFileListPlaying RGB (0, 20, 0)
// #define COLOR_ForeFileListRecording RGB (20, 0, 0)
// #define COLOR_ForeFileListPlayingRecording RGB (24, 24, 0)
#define COLOR_ForeFileListPlaying COLOR_Green
#define COLOR_ForeFileListRecording COLOR_Red
#define COLOR_ForeFileListPlayingRecording COLOR_Yellow
#define COLOR_ForeFileListOther COLOR_White
#define COLOR_BackFileListFolder COLOR_DarkGray
// #define COLOR_BackFileListTs COLOR_Gray
#define COLOR_BackFileListTs COLOR_ListBack
// #define COLOR_BackFileListOther COLOR_Gray
#define COLOR_BackFileListOther COLOR_ListBack
#define COLOR_BackFileHead COLOR_OrangeYellow
// #define COLOR_BackFileListNormal COLOR_Gray
#define COLOR_BackFileListNormal COLOR_ListBack
// #define COLOR_BackFileListSelected COLOR_DarkCyan
#define COLOR_BackFileError COLOR_White
// #define FileFixedHeader "FileMaster"
#define FileListMaxPathLevels 10
#define FileListEntriesPerFolder 50
#define FNT_Size_FileDisplay FNT_Size_1622
#define FileDisplayYincrement 24
#define FileDisplayEntriesPerScreen (FileErrorYoffset - FileHeadH) / FileDisplayYincrement
#define FileDisplayBytesPerLine 16
// #define FileDisplayLinesPerScreen 20
// #define FileDisplayBytesPerScreen FileDisplayBytesPerLine * FileDisplayLinesPerScreen
#define FileDisplayXincHex 25
#define FileDisplayXincAsc 14
#define COLOR_ForeFileDisplay COLOR_White

#define NAME_ENTRY_BUFFER_SIZE TS_FILE_NAME_SIZE
#define NameEntryH 24
// #define TimerNameEntryHelpYoffset TimerEntryYoffset - NameEntryH
// #define FileNameEntryHelpYoffset FileNameEntryYoffset - NameEntryH
// #define FileNameEntryPrefix "File Name: "
#define NameEntryPrefixXoffset 50
#define COLOR_ForeNameEntryFixed COLOR_White
#define COLOR_ForeNameEntryFixedNext COLOR_Green
#define COLOR_ForeNameEntryVble COLOR_Yellow
#define COLOR_ForeNameEntryVbleMarkerWord COLOR_Green
#define COLOR_ForeNameEntryVbleMarkerUpper COLOR_Magenta
#define COLOR_ForeNameEntryVbleMarkerLower COLOR_White
#define COLOR_ForeNameEntryHelp COLOR_Red
#define COLOR_BackNameEntryHelp COLOR_White
#define COLOR_BackNameEntry COLOR_DarkGray
#define FNT_Size_NameEntry FNT_Size_1622

#define PrefixFileNameTimer 39 // (apostrophe, used to indicate file came from a timer)
#define PrefixFileRenameEPGnow 58 // (colon, used to indicate file has been renamed to current EPG event name)
#define PrefixFileRenameEPGnext 59 // (semi-colon, used to indicate file has been renamed to next EPG event name)
#define KeyToggleListHeight RKEY_Mute

#define TsHeaderByteDurationSec 7
#define TsHeaderByteStartHour 66
#define TsHeaderByteStartMin 67
#define TsHeaderByteStartSec 6

#define KeyEnterTimedStopRec RKEY_AudioTrk

TAP_ID (ID_PIPSwitch);
// TAP_PROGRAM_NAME ("PIPSwitch");
TAP_PROGRAM_NAME ("ViewMaster3");
TAP_AUTHOR_NAME ("mvtaylor, Tonymy01, Wurstepeter");
TAP_DESCRIPTION ("Switch PIP channel directly, move it to ideal spot, etc.");
TAP_ETCINFO (__DATE__);

// static char g_str[9] = "12345678\0";
// static dword g_nmemfound=0x00000000;
// char *g_pstr2;

static int rgnOpts;   // rgn-handle for options (and file list sort options) window
static TYPE_Window winOpts; // window-handle for options window
static bool bOptsWinShown; // toggles whether options window is shown
static bool bFileSortOptsWinShown; // toggles whether file list sort options window is shown
static bool bTimerSortOptsWinShown; // toggles whether timer list sort options window is shown
static bool bActivated = TRUE; // toggles whether sub-switch is activated/deactivated
// static bool bVolumeKeys = TRUE; // toggles whether switching using volume-keys
// static int positioncount;
static int positioncount=3;
static int ypos=0;
//static int chNum=0;
// static int position[2][4] = {{ 0,540,540,0 },{0,0,432,432}}; 
// static int position[2][4] = {{ 30,510,510,30 },{8,8,424,424}}; 
static int position[2][4];   // initialized in main
// static bool bIsmain=TRUE;
static bool bStartSubChannel = FALSE;
static bool bDontStartSub = FALSE; // used in conjunction with bStartSubChannel
static bool bSat_key_hit = FALSE;
static bool bPIPshown = FALSE;
static int  iPOPshown = 0;
static bool bChanSub = FALSE; // toggles whether quick channel selection keys affect sub or main picture
// static bool bTimerRAMhackActivated = FALSE;
static bool bCaptureOSD = FALSE; // tv/stb key captures main channel or osd
static bool bQuickChanActivated = TRUE; // toggles whether to activate quick channel change
static bool bPlaybackNumericActivated = TRUE; // toggles whether to activate special handling of numeric keys during playback
// static bool bPlayback = FALSE;  // attempts to track whether or not in playback mode (from standard file list, not mine)
static bool bDeleteKeyHit = FALSE;
static bool bLeftOrRightKeyHit = FALSE;

static int xmax = 720;
static int ymax = 576;
// static int pipxoff = 30;
// static int pipyoff = 0;
// static int pipxoff = 20;
// static int pipyoff = 12;
// static int pipw = 180;
// static int piph = 144;
static int pipxoff = 22;
static int pipyoff = 14;
static int pipw = 120;
static int piph = 96;
static int tvRadio,chNum;

// static int CurrentChanSub = 0; // used to remember current channel in sub picture, to avoid -1 on chan start
// static int rgnScreen = 0; // region handle for whole screen
// static dword BorderThickness = 2; // thickness of rectangle border around sub picture
// static dword BorderColor = COLOR_Red; // color of rectangle border around sub picture
// static dword swaptick = 0;  // tick count when key hit to swap main and sub

static bool bClockShown = FALSE;
static word rgnClock;
static dword lasttick;
static word mjd,year;
static byte hour,min,sec,month,day,weekDay,lastsec;
// static char strclock[29];
// static char strclock[60];
// static char strclock[80];
 static char strclock[90];
static int ClockX;  // x-coord of clock display box, will be calculated based on font size
static int ClockY = 530; // y-coord of clock display box (near bottom of screen)
static int ClockW;  // width of clock display box, will be calculated based on font size
static int ClockH = 24; // height of clock display box
static int ClockXoff = 5;
static int ClockYoff = 0;

// 23jan05 API ver 1.11 Play/Record info displayed above clock
static word rgnPlayRec;
static int PlayRecX;  // x-coord of play/record status boxes, will be calculated based on font size
// static int PlayRecY = 530 - 72; // y-coord of top of play/record status boxes (up to 3, immediately above clock)
static int PlayRecY = 530 - 66; // y-coord of top of play/record status boxes (up to 3, immediately above clock)
static int PlayRecW;   // width of clock play/record status boxes, will be calculated based on font size
// static int PlayRecH = 24;  // height of each play/record status box
static int PlayRecH = 22;  // height of each play/record status box
static int PlayRecXoff = 5;
static int PlayRecYoff = 0;
static int PlayRecInfoYoff;
static char strPlayRec[256];
static char strPlayRecFileName[TS_FILE_NAME_SIZE];

static TYPE_PlayInfo PlayInfo;
// static TYPE_RecInfo RecInfo1;
// static TYPE_RecInfo RecInfo2;
static TYPE_RecInfo RecInfo[2];
// static TYPE_TapEvent *eventinfo;
static int OptionsX = 150; // was 220
static int OptionsY = 180; // was 200
static int OptionsW = 380; // was 240
static int OptionsH = 185;  // was 165
// static dword ClockCounter;
// static int lvlmin = 101, qualmin = 101;
static dword State, SubState;
static dword PrevState, PrevSubState;
static bool bStopSpinDown = TRUE;
// static dword StopSpinDownCounter;
static bool bReduceFullScreen = FALSE;
static dword totalsize;

static bool bTimerMode = FALSE;
static bool bTimerModeHeightFull = TRUE;
static word rgnTimer;
// static int yoff = 0;
// static int TimerEntryXoffset[NumTimerEntryFields] = {160, 300, 440, 580};
// static int TimerEntryXoffset[NumTimerEntryFields] = {55, 160, 310, 410, 510, 590};
// static int TimerEntryXoffset[NumTimerEntryFields] = {50, 150, 300, 400, 490, 570, 650};
// static int TimerEntryXoffset[NumTimerEntryFields] = {50, 150, 300, 400, 490, 570};
static int TimerEntryXoffset[NumTimerEntryFields] = {50, 150, 300, 400, 490, 590};
static int TimerEntryDataXoffset[NumTimerEntryFields];
static int TimerEntryDataW[NumTimerEntryFields];
static char TimerEntryFieldName[NumTimerEntryFields][20];
static char TimerEntryFieldData[NumTimerEntryFields][MaxTimerEntryFieldLen+1];
// static int TimerEntryFieldLen[NumTimerEntryFields] = {4, 4, 1, 1};
// static int TimerEntryFieldLen[NumTimerEntryFields] = {0, 0, 4, 4, 1, 1};
// static int TimerEntryFieldLen[NumTimerEntryFields] = {0, 0, 4, 4, 1, 1, 1};
// static int TimerEntryFieldLen[NumTimerEntryFields] = {0, 0, 4, 4, 1, 1};
static int TimerEntryFieldLen[NumTimerEntryFields] = {0, 0, 4, 4, 3, 1};
// static int TimerFieldIndex = 0;
static int TimerFieldIndex = TimerFieldIndexStart;
static int TimerFieldIndexSave;
static int TimerCharIndex = 0;

static TYPE_TimerInfo timerInfo;
static TYPE_ReservationType TimerReservationType;
static int retvalTimer;
static char TimerStart[5], TimerEnd[5], TimerWeekDay[4];
static int TimerStartMinutes, TimerEndMinutes, TimerNowMinutes;
static byte TimerStartHr, TimerStartMin;
static word TimerDate;
static word TimerDateTest;
static word TimerYear;
static byte TimerMonth, TimerDay;
static word TimerSvcNum, TimerDuration;
static byte TimerTuner;
// static int TimerListEntriesPerScreen;
static int TimerListIndex, TimerListIndexTop, TimerListIndexBottom, TimerListIndexMax;
static bool bErrorMsgShown;
// static TYPE_Window winError;
static byte TimerFilenameFixed = 1;
static bool bTimerDeleteInProgress = FALSE;
static bool bTimerModifyInProgress = FALSE;
static int TimerModifyEntryIndex;
static TYPE_TimerInfo TimerListArray[MAX_TIMERS];
static int TimerNumArray[MAX_TIMERS];
static byte TimerListSortKey = 0; // 1 for alphabetic, 2 for unsorted (i.e. as in RAM), else date/time (all ascending)

static bool bFileMode = FALSE;
static bool bFileListShown = FALSE;
static bool bFileModeHeightFull = TRUE;
static int FileWinH;
static int FileErrorYoffset;
static int FileNameEntryYoffset;
static int FileNameEntryHelpYoffset;
static int FileListEntriesPerScreen;
static int FileDisplayLinesPerScreen;
static int FileDisplayBytesPerScreen;

static int TimerWinH;
static int TimerErrorYoffset;
static int TimerEntryFileNameYoffset;
static int TimerEntryYoffset;
static int TimerListEntriesPerScreen;
static int TimerNameEntryHelpYoffset;

static word rgnFile;
// static int FileListEntriesPerScreen;
// static dword FileListIndex, FileListIndexTop, FileListIndexBottom, FileListIndexFound, FileListIndexMax;
static int FileListIndex, FileListIndexTop, FileListIndexBottom, FileListIndexFound, FileListIndexMax;
static dword FileListCountFiles, FileListCountFolders;
static ulong64 FileListCountBytes;
static bool bFileErrorMsgShown;
static TYPE_File FileListEntry;
static TYPE_File FileListEntryPlay;
// static TYPE_File FileListScreenArray[FileListEntriesPerScreen];
// static int FileListDurationArray[FileListEntriesPerScreen];
static TYPE_File FileListScreenArray[20];
static int FileListDurationArray[20];
static byte FileListDurationSecArray[20];
static byte FileListDurationSec;
static int FileListLcnArray[20];
static int FileListLcn;
static byte FileListStartHourArray[20], FileListStartMinArray[20], FileListStartSecArray[20];
static byte FileListStartHour, FileListStartMin, FileListStartSec;
static TYPE_File FileListFolderArray[FileListEntriesPerFolder];
static char FileListNamePlay[TS_FILE_NAME_SIZE];
static char FileListNamePlayNext[TS_FILE_NAME_SIZE];
static char FileListNamePlayPrev[TS_FILE_NAME_SIZE];
static char FileListNameRec[2][TS_FILE_NAME_SIZE];
static char FileListNameRecPrev[2][TS_FILE_NAME_SIZE];
/*
static dword FileListStartTimePlay;
static dword FileListStartTimePlayNext;
*/
static word FileListStartPlayMjd;
static byte FileListStartPlayHour, FileListStartPlayMin, FileListStartPlaySec;
static word FileListStartPlayNextMjd;
static byte FileListStartPlayNextHour, FileListStartPlayNextMin, FileListStartPlayNextSec;
static bool bFileListNameChangedRec[2];
static int FileListIndexRec[2];
// static dword FileListIndexPlay = 0;
static char FileListPath[FileListMaxPathLevels][255];
static int FileListPathLevel = 2;
static byte FileListSortKey = 0; // 1 for alphabetic, 2 for size, else date/time (all ascending)
static bool bGetFilesFromArray;
static bool bFileDeleteInProgress = FALSE;
static bool bFilePlayingBack = FALSE;
static bool bFilePlaybackStopInProgress = FALSE;
static bool bPlaybackPaused = FALSE;
// static bool bPreDeleteStop = FALSE;
// static bool bPreDeleteHide = FALSE;
static bool bStopRecInProgress = FALSE;
static byte RecSlotBeingStopped;
static bool bFileListRefreshDuration = TRUE;
static bool bFileDisplayShown = FALSE;
static bool bResumeFileDisplay = FALSE;
static bool bFileInfoShown = FALSE;
static bool bFileCopyInProgress = FALSE;
// static bBookmark = FALSE;
static char FileNameAction[TS_FILE_NAME_SIZE];
static TYPE_File* FileDisplayPointer;
static TYPE_File* FileCopySourcePointer;
static TYPE_File* FileCopyDestPointer;
static char FileCopyName[TS_FILE_NAME_SIZE];
static byte FileCopyAttr;
// static unsigned char FileDisplayBuffer[FileDisplayBytesPerScreen + 1];
// TS file header info for EPG text can be more than 320 (FileDisplayBytesPerScreen)
// so following are bigger so we can fit it it one read
static unsigned char FileDisplayBuffer[512];
static unsigned char TsFileHeader[512];
static dword FileDisplayLength;
static ulong64 FileDisplayLengthLong;
static dword FileDisplayStart;
static int FileListDurationPlay;
static byte FileListDurationSecPlay;
static int FileListLcnPlay;
static byte FileListStartHourPlay, FileListStartMinPlay, FileListStartSecPlay;

static word rgnNameEntry;

static char KeyCharArray[10][6] = {" &-'", "ABCD", "EFGH", "IJKL", "MNOP", "QRST", "UVWX", "YZ,.", "01234", "56789"};
static char FileNameEntryPrefix[16]; // longest string should be "Create Folder: "
static bool bNameEntryMode = FALSE;
static char NameEntryBuffer[NAME_ENTRY_BUFFER_SIZE];
static char FileRenameOld[NAME_ENTRY_BUFFER_SIZE];
static char RecAutoRenameNew[NAME_ENTRY_BUFFER_SIZE];
static int NameEntryBufferIndex;
static int NameEntryBufferIndexMax;
static int NameEntryKeyPressCount;
static bool bNameEntryHelpShown = FALSE;
static bool bNameEntryKeyCheckMax;
static bool bNameEntryKeyCheckPrev;
static bool bNameEntryInsertMode;
static bool bFolderCreationInProgress = FALSE;
static bool bRenameEpgNext;
static bool bTryRenameLater[2]; // flag to try rename of rec slot to current epg name if can't do next at start
static bool bRefreshFileListInIdle = TRUE;
static bool bRefreshFileListAfterLoop;
static bool bRecEventInfoSaved[2];   // flag event info saved for each rec slot, so can save to file header when rec stops
static bool bFileListRenameToEpg;
static char RecEventName[2][128];
static char RecEventDesc[2][128];
static char RecEventNameFinal[2][128];
static char RecEventDescFinal[2][128];
static char RecFileNameUpdate[2][TS_FILE_NAME_SIZE];
static char RecEventNameFileList[128];
static char RecEventDescFileList[128];
static byte RecTuner[2];
static byte RecordTuner[2];
static dword TickRecStopped[2];
static dword RecEventStartTime[2];
static dword RecEventEndTime[2];
static dword RecEventStartTimeFinal[2];
static dword RecEventEndTimeFinal[2];
static dword RecEventStartTimeFileList;
static dword RecEventEndTimeFileList;
static dword RecordedSec[2];
static dword RecDuration[2];
static word RecStartMjd[2];
static word RecStartMjdFinal[2];
static byte RecStartHour[2], RecStartMin[2], RecStartSec[2];
static byte RecStartHourFinal[2], RecStartMinFinal[2], RecStartSecFinal[2];
/*
static word RecStartMjdSaved[2];
static byte RecStartHourSaved[2], RecStartMinSaved[2], RecStartSecSaved[2];
*/
/*
static bool bNameEntryLowerCase;
static bool bNameEntryWordCase;
*/
static byte NameEntryCase; // 0 = word case (default), 1 = upper case, 2 = lower case
static dword NameEntryMarkerColor[3] = {COLOR_ForeNameEntryVbleMarkerWord, COLOR_ForeNameEntryVbleMarkerUpper, COLOR_ForeNameEntryVbleMarkerLower};  
static dword NameEntryPrevKeyCode;
static dword TickProgBarSaved;
static dword TickPlayScanSaved;
static dword TickRecordKeySaved;
static dword TickClockSaved;
static dword TickSpinDownSaved;
// static dword TickPlayStart;
// static dword TickPlayResume;
static dword TickFilePlay;
static dword TickFilePlayStart;
static dword TickPlaybackNumericSaved;
static dword TickRecAutoRenameSaved;
static byte HourFilePlayStart;
static byte MinFilePlayStart;
// static byte SecFilePlayStart;

static bool bPlayScanMode = FALSE;
static bool bPlayScanFast;
// static bool bRecordKeyGen = FALSE;
// static int RecordKeyCount = 0;
// static bool bChannelChanged = TRUE;
// static bool bRecordMsgShown = FALSE;

// static int PercentKeyReturnCount = 0;
static int PlaybackNumericKeyCount = 0;
/*
static int PlaybackPercent;
static dword PlaybackPercentKeyCode1;
static dword PlaybackPercentKeyCode2;
static dword PlaybackPercentKeyCode3;
*/
static char PlaybackNumericKeyString[4];
static word rgnNumKey;
// static byte PlaybackJumpType = 0;   // 0 for absolute, 1 for relative forward, 2 for relative backward
static bool bPlaybackNumericJumpMode = FALSE;

static bool bShowFileListPending = FALSE;
static bool bShowTimerListPending = FALSE;
// static bool bUnlimitedRecordPending = FALSE;
// static int UnlimitedRecordLoopCount = 0; // not needed with 06dec04 firmware, was needed for workaround with 30mar05 firmware.
static bool bFileIsRecording;
static TYPE_TapEvent *evtInfo;
// static TYPE_TapEvent evtInfo;
static TYPE_TapChInfo chInfo;
static char ErrorMsgSaved[512];

static int BlockRateNormalPlay; // curenntly-used playback rate in blocks per minute * 10
static int BlockRateNormalPlayDyn; // calculated dynamic playback rate in blocks per minute * 10
static int BlockRateNormalPlayInit; // initial (duration-based) playback rate in blocks per minute * 10
static bool bBlockRateDynamic;  // flag whether using dynamic or duration-based block rate
static bool bBlockRatesShown = FALSE;
// static dword TickStartNormalPlay;
// static dword BlockStartNormalPlay;
// static dword TicksInNormalPlayCurrent;
static dword TicksInNormalPlayCumulative;
static dword TicksInNormalPlaySaved;
// static dword BlocksInNormalPlayCurrent;
static dword BlocksInNormalPlayCumulative;
static dword TickBlockRateUpdated;
static dword TickBlockRateSaved;
static dword BlockCountSaved;
static byte TrickModePrev;

static dword BlockPlayStopped;  // for resuming playback
static bool bPlayCounterShown = FALSE;
static word rgnPlayCounter;
static dword TickPlayCounterSaved;

// variables for stop recording at specified time, then sleep.
static bool bTimedStopRecEntryMode = FALSE;
static bool bTimedStopRecSleepGen = FALSE;
static char TimedStopRecEntry[5];
static word TimedStopRecMjd = 0;
static byte TimedStopRecHour = 0;
static byte TimedStopRecMin = 0;
static bool bPlaying, bRecording[2];    // set by event handler to avoid calling getplayinfo and getrecinfo repeatedly

void ShowTimerErrorMsg (char *msg);
void ShowFileErrorMsg (char *msg, bool bAcknowledge);
void HideTimerErrorMsg ();
void HideFileErrorMsg ();
void ScanCurrentFolder ();
void ListCurrentFolder (bool bClearFirst);

// start of stuff for screen capture
#define LE16( p )   ( (((p&0x00ff))<<8) | ((p&0xff00)>>8) )
#define LE32( p )   ( (((p&0x000000ff))<<24) | (((p&0x0000ff00))<<8) | (((p&0x00ff0000))>>8) | ((p&0xff000000)>>24) )

struct BMP_HEAD {
    byte id[2];
    word size_l;
    word size_h;
    word reserved[2];
    word offset_l;
    word offset_h;
};

struct BMP_INFO {
    long info_size;
    long width;
    long height;
    word plane;
    word bits_per_pixel;
    long compress;
    long img_size;
    long x_res;
    long y_res;
    long color;
    long icolor;
};

static struct BMP_HEAD head;
static struct BMP_INFO info;

static int capture_num = 0;
static char out_name[64];
static byte *_bmpBuf = NULL;

int _bmpWidth;
int _bmpHeight;
TYPE_File *_bmpFile;

#define _R(y,u,v) (0x2568*(y)              + 0x3343*(u)) /0x2000
#define _G(y,u,v) (0x2568*(y) - 0x0c92*(v) - 0x1a1e*(u)) /0x2000
#define _B(y,u,v) (0x2568*(y) + 0x40cf*(v))              /0x2000

#define _S(a)       (a)>255 ? 255 : (a)<0 ? 0 : (a)

inline void YUV2RGB( word yy, word uu, word vv, byte *r, byte *g, byte *b )
{
    signed int _r,_g,_b;
    signed int y, u, v;

    y = yy; // - 12;
    u = (int)uu - 128;
    v = (int)vv - 128;

    _r = _R(y,u,v);
    _g = _G(y,u,v);
    _b = _B(y,u,v);

    *r = _S(_r);
    *g = _S(_g);
    *b = _S(_b);
}


// void BMP_WriteHeader( int width, int height )
void BMP_WriteHeader( int width, int height, char *FileNameSuffix )
{
    long size;
    int i, j;
    word *p;

    size = sizeof( head ) + sizeof( info ) + (width*height)*3;

    head.id[0] = 'B';
    head.id[1] = 'M';
    head.size_l  = LE16( (size&0xffff) );
    head.size_h  = LE16( (size>>16) );
    i =  (sizeof( head ) + sizeof( info ));
    head.offset_l = LE16( i&0xffff );
    head.offset_h = LE16( i>>16 );

    info.info_size  = LE32( 40 );
    info.width      = LE32( width );
    info.height        = LE32( height );
    info.plane      = LE16( 1 );
    info.bits_per_pixel = LE16( 24 );
    info.compress       = LE32( 0 );
    info.img_size       = LE32( (width*height)*3 );
    info.x_res          = 0;
    info.y_res          = 0;
    info.color          = 0;
    info.icolor         = 0;

//    TAP_SPrint( out_name, "SCR%04d.BMP", capture_num++ );
    TAP_SPrint( out_name, "CAP%04d%s.BMP", capture_num++, FileNameSuffix );
    TAP_Hdd_Create( out_name, ATTR_NORMAL );
    _bmpFile = TAP_Hdd_Fopen( out_name );
    if( _bmpFile==NULL ) 
        return;

    TAP_Hdd_Fwrite( &head, sizeof( head ), 1, _bmpFile );
    TAP_Hdd_Fwrite( &info, sizeof( info ), 1, _bmpFile );

    _bmpWidth = width;
    _bmpHeight = height;

    if( _bmpBuf )
    {
        TAP_MemFree( _bmpBuf );
        _bmpBuf = NULL;
    }
    _bmpBuf = TAP_MemAlloc( (width*height)*3 );
    
}

inline void BMP_SetPixel( int x, int y, byte r, byte g, byte b )
{
    register pos = (y * _bmpWidth + x)*3;
        _bmpBuf[ pos ] = b;
        _bmpBuf[ pos+1 ] = g;
        _bmpBuf[ pos+2 ] = r;
}
    
void BMP_End( void )
{
    char msg[96];

    if( _bmpFile==NULL ) 
        return;
    TAP_Hdd_Fwrite( _bmpBuf, _bmpWidth*3, _bmpHeight, _bmpFile );
    if( _bmpBuf )
    {
        TAP_MemFree( _bmpBuf );
        _bmpBuf = NULL;
    }
    TAP_Hdd_Fclose( _bmpFile );
    sprintf (msg, "Capture written to %s", out_name);
    ShowFileErrorMsg (msg, FALSE);
}

void CaptureVideo( byte mainSub, int width, int height )
{
    TYPE_VideoBank videoBank;
    byte *picAddr, *chrAddr;
    int x, y, xx, yy;
    dword xinc, yinc;
    byte r, g, b;
    int ypos, ypos2;
    int xpos;
    int yPos, cPos;
    char FileNameSuffix[5];

    memset( &videoBank, 0, sizeof(videoBank) );

    TAP_CaptureScreen( mainSub, &videoBank );

    picAddr = videoBank.yAddress;
    chrAddr = videoBank.cAddress;

    TAP_Print("yAddr = %x, cAddr = %x, videoBank.height = %d, videoBank.width = %d\r\n",
        picAddr, chrAddr, videoBank.height, videoBank.width );

    if (mainSub == CHANNEL_Main)
        strcpy (FileNameSuffix, "MAIN");
    else
        strcpy (FileNameSuffix, "SUB");
    BMP_WriteHeader( width, height, FileNameSuffix );
    for( yy = 0; yy<height; yy++ ) 
    {
        y = yy * videoBank.height / height;
        ypos  = (( y    & 0xffffffe0) * 736) + (y&0x1f)*32;
        ypos2 = (((y/2) & 0xffffffe0) * 736)  + ((y/2)&0x1f)*32;
        for( xx=0; xx<width; xx++ )
        {
            x = xx * videoBank.width / width;
            xpos =  ((x & 0xffffffe0)<<5) + (x&0x1f);
            yPos  = ypos  + xpos;
            cPos = (ypos2 + xpos)&0xfffffffe;
                
            YUV2RGB( picAddr[yPos], chrAddr[cPos+1], chrAddr[cPos], &r, &g, &b );
            BMP_SetPixel( xx, height-yy-1, r, g, b ); // bottom -> top
        }
    }
    BMP_End();

    TAP_MemFree( videoBank.cAddress );
    TAP_MemFree( videoBank.yAddress );
    
}
// end of stuff for screen capture

void OSD2BMP ()
{
    word *_vAddr[576];
    int x, y, i;
    byte r, g, b;

    TYPE_OsdBaseInfo osdBaseInfo;

    TAP_Osd_GetBaseInfo( &osdBaseInfo );

    // set vertical address
    for( i=0; i<576; i+=2 )
    {
        _vAddr[i] = osdBaseInfo.eAddr + 720*i;
        _vAddr[i+1] = osdBaseInfo.oAddr + 720*i;
    }

    BMP_WriteHeader( 720, 576, "OSD" );
    for( y=0; y<576; y++ )
    {
        for( x=0; x<720; x++ )
        {
            r = (*(_vAddr[y]+x) >> 10) & 0x1F;
            g = (*(_vAddr[y]+x) >> 5) & 0x1F;
            b = *(_vAddr[y]+x) & 0x1F;
// BMP is expecting 8-bit color, I think, and OSD only has 5-bit, so multiply by 8 to increase brightness
            BMP_SetPixel( x, 576-y-1, r<<3, g<<3, b<<3 ); // bottom -> top
        }
    }
    BMP_End();
}

// start of stuff for finding current tuner
#define CMD_MASK 0xfc000000 
#define REG_MASK 0x03ff0000 
#define LOAD_TYPE 0x9000000b 
#define LOAD_TUNER 0x90000015 
#define JR_CMD 0x03e00008 
#define SHIFT_CMD 0 
#define NOP_CMD 0 

//------------------------------------------------------ 
// function pointer definitions for the wrapped code 
// The returned tuner number is zero-based (i.e. 0 = Tuner 1, 1 = Tuner 2). 
// The return value contains the channel number (as in TAP_Channel_GetCurrent). 
typedef dword (*GetTuner)(dword* tuner, dword* chNum); 
GetTuner getTuner = NULL; 

//-------------------------------------------------------- 
// initCodeWrapper() creates a code wrapper based on TAP_Channel_GetCurrent. The 
// function allocates a buffer and copies the code of TAP_Channel_GetCurrent into 
// it except for the actual memory access operations. 
// initCodeWrapper() returns a buffer with code returning the current tuner number. 
// The initialization is to be done only once. 
// For simplicity reasons the function signature remains unchanged (that is, two 
// parameters and a return value). 
// The returned tuner number is zero-based (i.e. 0 = Tuner 1, 1 = Tuner 2). 
// The return value contains the channel number (as in TAP_Channel_GetCurrent). 
dword* initCodeWrapper() 
{ 
  int i; 
  dword *pSrc = (dword*)TAP_Channel_GetCurrent; 
  dword *pBuffer; 
  bool found = FALSE; 
  int bufSize = 0; 
  int count = 0; 

  // find the return statement 
  while(pSrc[bufSize] != JR_CMD) 
  { 
    bufSize++; 
  } 

  // the buffer should include the return statement and the command following 
  // the return statement 
  bufSize += 2; 

  // allocate memory 
  pBuffer = malloc(sizeof(dword) * bufSize); 

  if(pBuffer == NULL) 
  { 
    return NULL; 
  } 

  for(i = 0; i < bufSize; i++, pSrc++) 
  { 
    if((*pSrc & ~REG_MASK) == LOAD_TYPE) 
    { 
      // the command loads the service type (offset 11(0x000b) in the structure) 
      // replace it with loading the tuner number (offset 21(0x0015) in the structure) 
      // preserve used registers 
      pBuffer[i] = LOAD_TUNER | (*pSrc & REG_MASK); 
      found = TRUE; 
    } 
    else 
    { 
      // replace the two shift operations following the load operation 
      if(found  && (count < 2) && ((*pSrc & CMD_MASK) == SHIFT_CMD)) 
      { 
        pBuffer[i] = NOP_CMD; 
        count++; 
      } 
      else 
      { 
        pBuffer[i] = *pSrc; 
      } 
    } 
  } 

  if(!found) 
  { 
    free(pBuffer); 
    pBuffer = NULL; 
  } 

  return pBuffer; 
} 
// end of stuff for finding current tuner

//dword TimeAdjust (dword time, int minutes);
void TimeAdjust (word *mjd, byte *hh, byte *mm, byte *ss, long adjsec);

// void bubble_sort(int size);
// bool IsProgressBarShown (dword state, dword substate)

bool IsProgressBarShown ()
{
 if (State == STATE_Normal &&
  SubState == SUBSTATE_PvrPlayingSearch)
/*
  (SubState == SUBSTATE_PvrPlayingSearch ||
  SubState == SUBSTATE_PvrRecSearch ||
  SubState == SUBSTATE_PvrTimeSearch))
*/
   return TRUE;
 else
  return FALSE;
}

void ShowMessageWin (char* lpMessage, char* lpMessage1)
{
 int rgn;       // stores rgn-handle for osd
 dword w;       // stores width of message-text

 TAP_ExitNormal ();
 rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );  // create rgn-handle
 w = TAP_Osd_GetW( lpMessage, 0, FNT_Size_1926 ) + 10;  // calculate width of message

 if (TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10>w)
 {
  w = TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10; // calculate width of message
 }
 if (w > 720) w = 720;      // if message is to long
 TAP_Osd_FillBox(rgn, (720-w)/2-5, 265, w+10, 62, RGB(19,19,19) ); // draw background-box for border
 TAP_Osd_PutS(rgn, (720-w)/2, 270, (720+w)/2, lpMessage,  // show message
  RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_CENTER);
 TAP_Osd_PutS(rgn, (720-w)/2, 270+26, (720+w)/2, lpMessage1,  // show message
  RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926,FALSE, ALIGN_CENTER);
 TAP_Delay(65);       // show it for 0.65 second
 TAP_Osd_Delete(rgn);     // release rgn-handle
 TAP_EnterNormal ();
}
/*
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
//    ShowMessageWin("LICHTETE ALLES GEDÄCHTNIS", "NEIGUNGCEntdeckungcTimer-UNTERZEICHNUNG AB.",270);
//    ShowMessageWin("Lassen Sie bitte Debugger laufen und treten Sie mit Autor in Verbindung", "um neue TAP zu errichten",290);
    ShowMessageWin("LICHTETE ALLES GEDÄCHTNIS", "NEIGUNGCEntdeckungcTimer-UNTERZEICHNUNG AB.");
    ShowMessageWin("Lassen Sie bitte Debugger laufen und treten Sie mit Autor in Verbindung", "um neue TAP zu errichten");
   }
   else
   {
//    ShowMessageWin("SCANNED ALL MEMORY", "CANT FIND TIMER SIGNATURE",270);
//    ShowMessageWin("Please run debugger and contact author", "to build new TAP",300);
    ShowMessageWin("SCANNED ALL MEMORY", "CANT FIND TIMER SIGNATURE");
    ShowMessageWin("Please run debugger and contact author", "to build new TAP");
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
 else if (j==0) //if numberoforigtimers = 0
 {
  //printf("j=0, deleting 2 timers\n");
  TAP_Timer_Delete(1);
  TAP_Timer_Delete(0);
 }
 return any+SIGNATURE_OFFSET;
}//search RAM
*/
void ShowOptionsWindow ()
{
// char *PipPosition="",*PipCurrPosition="";
 char str[60];

 TAP_ExitNormal ();
 rgnOpts = TAP_Osd_Create (0, 0, 720, 576, 0, FALSE);
 TAP_Win_SetDefaultColor (&winOpts);
 TAP_Win_Create (&winOpts, rgnOpts, OptionsX, OptionsY, OptionsW, OptionsH, FALSE, FALSE);
// TAP_Win_SetTitle (&winOpts, "ViewMaster3 Options", 0, FNT_Size_1622);
 strcpy (str, "ViewMaster3 Options (");
 strcat (str, __DATE__);
 strcat (str, " ");
 strcat (str, __TIME__);
 strcat (str, ")");
 TAP_Win_SetTitle (&winOpts, str, 0, FNT_Size_1622);
 TAP_Win_AddItem (&winOpts, bActivated ? "Deactivate ViewMaster" : "Activate ViewMaster");
 TAP_Win_AddItem (&winOpts, bCaptureOSD ? "Capture Main Channel" : "Capture OSD");
 TAP_Win_AddItem (&winOpts, bQuickChanActivated ? "Disable Quick Channel Change" : "Enable Quick Channel Change");
 TAP_Win_AddItem (&winOpts, bPlaybackNumericActivated ? "Treat Playback Numeric Keys As Percent" : "Treat PlayBack Numeric Keys As Minutes");
 TAP_Win_AddItem (&winOpts, bStopSpinDown ? "Allow HDD Spin Down" : "Prevent HDD Spin Down");
 TAP_Win_AddItem (&winOpts, bReduceFullScreen ? "Normal Full Screen" : "Reduced Full Screen");
 TAP_Win_AddItem (&winOpts, bFilePlayingBack ? "Clear Playback Flag" : "Set Playback Flag");
 TAP_Win_AddItem (&winOpts, "Exit ViewMaster");
 TAP_Win_AddItem (&winOpts, "Cancel");
 TAP_Win_SetSelection (&winOpts, 0);
 bOptsWinShown = TRUE;
}

void HideOptionsWindow ()
{
 TAP_Win_Delete (&winOpts);
 TAP_Osd_Delete (rgnOpts);
 bOptsWinShown = FALSE;
 TAP_EnterNormal ();
}

void ShowFileSortOptionsWindow ()
{
/*
 char str[60];

// TAP_ExitNormal ();
 rgnOpts = TAP_Osd_Create (0, 0, 720, 576, 0, FALSE);
 TAP_Win_SetDefaultColor (&winOpts);
 TAP_Win_Create (&winOpts, rgnOpts, 250, 210, 180, 55, FALSE, FALSE);
 TAP_Win_SetTitle (&winOpts, "File List Sort Options", 0, FNT_Size_1622);
 TAP_Win_AddItem (&winOpts, "Date/Time");

 TAP_Win_AddItem (&winOpts, "Alphabetic");
 TAP_Win_AddItem (&winOpts, "Size");
// TAP_Win_SetSelection (&winOpts, 0);

 TAP_Win_SetSelection (&winOpts, FileListSortKey);
*/
 ShowFileErrorMsg ("Sort: by Date/Time (press 0) File Name (press 1) File Size (press 2)", TRUE);
 bFileSortOptsWinShown = TRUE;
}

void HideFileSortOptionsWindow ()
{
/*
 TAP_Win_Delete (&winOpts);
 TAP_Osd_Delete (rgnOpts);
*/
 HideFileErrorMsg ();
 bFileSortOptsWinShown = FALSE;
// TAP_EnterNormal ();
}

void ShowTimerSortOptionsWindow ()
{
/*
// TAP_ExitNormal ();
// rgnOpts = TAP_Osd_Create (0, 0, 720, 576, 0, FALSE);
 rgnOpts = TAP_Osd_Create (240, 330, 250, 75, 0, FALSE);
 TAP_Win_SetDefaultColor (&winOpts);
// TAP_Win_Create (&winOpts, rgnOpts, 270, 380, 210, 55, FALSE, FALSE);
 TAP_Win_Create (&winOpts, rgnOpts, 0, 0, 200, 55, FALSE, FALSE);
 TAP_Win_SetTitle (&winOpts, "Timer List Sort Options", 0, FNT_Size_1622);
 TAP_Win_AddItem (&winOpts, TimerListSortOption0);
 TAP_Win_AddItem (&winOpts, TimerListSortOption1);
 TAP_Win_AddItem (&winOpts, TimerListSortOption2);
 TAP_Win_SetSelection (&winOpts, TimerListSortKey);
*/
 ShowTimerErrorMsg ("Sort by: Date/Time (press 0) File Name (press 1) Timer Index (press 2)");
 bTimerSortOptsWinShown = TRUE;
}

void HideTimerSortOptionsWindow ()
{
/*
 TAP_Win_Delete (&winOpts);
 TAP_Osd_Delete (rgnOpts);
*/
 HideTimerErrorMsg ();
 bTimerSortOptsWinShown = FALSE;
// TAP_EnterNormal ();
}

int hextoascii(unsigned long l, char str[9])
{
 int i;
 unsigned int c;
 unsigned char b;

 for (i=0; i<=7; i++) {
  b = (l >> 28) &0x0000000f;
  c = b;
  if ( (c>=0) && (c<=9) ) b+= 0x30;
  else b+= 0x37;
  str[i] = b;
  l = l<<4;
 }
 str[8]='\0';
 return 0;
}

int hextoasciibyte (byte l, char str[3])
{
 int i;
 unsigned int c;
 unsigned char b;
 for (i=0; i<=1; i++) {
  b = (l >> 4) &0xF;
  c = b;
  if ( (c>=0) && (c<=9) ) b+= 0x30;
  else b+= 0x37;
  str[i] = b;
  l = l<<4;
 }
 str[2]='\0';
 return 0;
}

void ClearPIPBorder(void)
{
// TAP_Osd_DrawRectangle(rgnScreen, position[0][positioncount]+BorderThickness, position[1][positioncount]+BorderThickness, pipw-2*BorderThickness, piph-2*BorderThickness, BorderThickness, COLOR_None);
}

void SetFileListParameters ()
{
 FileErrorYoffset = FileWinH - FileErrorH;
 FileNameEntryYoffset = FileErrorYoffset - NameEntryH;
// FileNameEntryHelpYoffset = FileNameEntryYoffset - NameEntryH;
 FileNameEntryHelpYoffset = FileErrorYoffset;
 FileListEntriesPerScreen = (FileErrorYoffset - FileHeadH) / FileListYincrement;
}

void SetTimerListParameters ()
{
    TimerErrorYoffset = TimerWinH - TimerErrorH;
    TimerEntryFileNameYoffset = TimerErrorYoffset - TimerEntryH;
    TimerEntryYoffset = TimerEntryFileNameYoffset - TimerEntryH;
//    TimerNameEntryHelpYoffset = TimerEntryYoffset;  // use first line of timer entry, rather than last line of list
    TimerNameEntryHelpYoffset = TimerErrorYoffset;  // use error msg line (clock line), rather than last line of list
    TimerListEntriesPerScreen = (TimerEntryYoffset - TimerHeadH) / TimerListYincrement;
}

void ScaleMainFull (void)
{

 if (bReduceFullScreen)
  TAP_Channel_Scale(CHANNEL_Main, 30, 24, 660, 528, TRUE);
 else
  TAP_Channel_Scale(CHANNEL_Main, 0, 0, xmax, ymax, FALSE);
}

void ScaleMainHalfHeight (int y)
{
   int x, w, h;
 
   h = ymax - y;
   w = h * xmax / ymax;
   x = (xmax - w) / 2;
   TAP_Channel_Scale (CHANNEL_Main, x, y, w, h, TRUE);
}

void ScalePIP (void)
{
 TAP_Channel_Scale(CHANNEL_Sub, position[0][positioncount], position[1][positioncount], pipw, piph, TRUE);
}

void ScalePOP (void)
{
 int x, y, w, h;

 if (bFileMode || bTimerMode)
 {
// half-height file or timer mode with pop below
   if (bFileMode)
     y = FileWinY + FileWinH;
   else
     y = TimerWinY + TimerWinH;
   h = ymax - y;
   w = h * xmax / ymax;
   x = xmax - 30 - w;
//   TAP_Channel_Scale (CHANNEL_Main, 296, 324, 384, 216, TRUE);
   TAP_Channel_Scale (CHANNEL_Main, x, y, w, h, TRUE);
//   TAP_Channel_Scale (CHANNEL_Sub,   40, 360, 256, 144, TRUE);
//   w = x - 40;
   w = min (x - 30, w);
   h = w * ymax / xmax;
   x = 30;
   y = y + (ymax - y - h) / 2;
   TAP_Channel_Scale (CHANNEL_Sub, x, y, w, h, TRUE);
 }
 else if (iPOPshown == 1)
 {
  TAP_Channel_Scale(CHANNEL_Main, xmax/2, 0, xmax/2, ymax/2, TRUE);
  TAP_Channel_Scale(CHANNEL_Sub, 0, ymax/2, xmax/2, ymax/2, TRUE);
 }
 else
 {
//  TAP_Channel_Scale(CHANNEL_Main, xmax/4, 0, xmax*3/4, ymax*3/4, TRUE);
//  TAP_Channel_Scale(CHANNEL_Sub, 0, ymax*3/4, xmax/4, ymax/4, TRUE);
  TAP_Channel_Scale(CHANNEL_Main, xmax/3, 0, xmax*2/3, ymax*2/3, TRUE);
  TAP_Channel_Scale(CHANNEL_Sub, 0, ymax*2/3, xmax/3, ymax/3, TRUE);
 }
}

bool IsDataFilesFolderShown ()
{
//    char msg[255];
    if (bFileListShown)
    {
//        sprintf (msg, "FileListPathLevel = %d", FileListPathLevel); 
//        ShowFileErrorMsg (msg, FALSE);
        if (FileListPathLevel < FileListMaxPathLevels)
        {
            if (strcmp (FileListPath[FileListPathLevel], "DataFiles") == 0)
                return TRUE;
        }
    }
    return FALSE;
}

// void ChangeToRootFolder (int levels)
void ChangeToRootFolder ()
{
/*
 int i;

//     TAP_Hdd_ChangeDir ("..");
 for (i = 1; i <= levels; i++)
      TAP_Hdd_ChangeDir (".."); // hopefully get to root (/ does not work)
*/
    TYPE_File   fp;
    int     iNdx;

    iNdx = 0;
    TAP_Hdd_FindFirst( &fp );

//  Loop until Root found allow maximum of 10 levels

    while ( ( strcmp( fp.name, "__ROOT__" ) != 0 ) && ( iNdx++ < 10 ) )
    {
        TAP_Hdd_ChangeDir( ".." );
        TAP_Hdd_FindFirst( &fp );
    }

    FileListPathLevel = 0;
}

void ChangeToDataFilesFolder ()
{
// ChangeToRootFolder (FileListPathLevel);
 ChangeToRootFolder ();
 TAP_Hdd_ChangeDir ("DataFiles");
 FileListPathLevel = 1;
 strcpy (FileListPath[FileListPathLevel], "DataFiles");
}

void ChangeToProgramFilesFolder ()
{
// ChangeToRootFolder (FileListPathLevel);
 ChangeToRootFolder ();
 TAP_Hdd_ChangeDir ("ProgramFiles");
 FileListPathLevel = 1;
 strcpy (FileListPath[FileListPathLevel], "ProgramFiles");
}
bool IsRec( char *name )
{
    static char tmp[128];

    strcpy( tmp, name );
    strlwr( tmp );
    if( strcmp( tmp + strlen(tmp) - 4, ".rec" ) )
        return FALSE;
    else
        return TRUE;
}

int CountRecFiles( void )
{
 int totalEntry;
 int totrec;
 TYPE_File file;

// rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );

// TAP_Win_SetDefaultColor( &win );
// TAP_Win_Create( &win, rgn, 100, 100, 200, 200, FALSE, TRUE );
// TAP_Win_SetTitle( &win, "FILES", 0, FNT_Size_1622 );

// count .rec files in DataFiles folder (but not in file mode since will mess up current folder)



 if (!bFileMode)
  ChangeToDataFilesFolder ();

 totrec = 0;
 totalEntry = TAP_Hdd_FindFirst( &file );
 while( totalEntry-- )
 {
//  if( IsRec( file.name ) )
// count ts attributes now, rather than .rec extensions
  if (file.attr == ATTR_TS)
              totrec = totrec + 1;
  if( totalEntry==0 || TAP_Hdd_FindNext( &file ) == 0 )
   break;
 }
     return totrec;
}

dword NumericToKeyCode (int num)
{
 switch (num)
 {
  case 0: return RKEY_0;
  case 1: return RKEY_1;
  case 2: return RKEY_2;
  case 3: return RKEY_3;

  case 4: return RKEY_4;
  case 5: return RKEY_5;
  case 6: return RKEY_6;
  case 7: return RKEY_7;
  case 8: return RKEY_8;
  case 9: return RKEY_9;
  default: return 0; // should never happen
 }
}

void CreatePlaybackNumericRegion ()
{
//    rgnNumKey = TAP_Osd_Create (0, 78, xmax, 26, 0, FALSE);
    rgnNumKey = TAP_Osd_Create (0, ClockY - (4 * PlayRecH) - 5, xmax, 24, 0, FALSE);
    bPlaybackNumericJumpMode = TRUE;
}

void TerminatePlaybackNumericKey ()
{
 PlaybackNumericKeyCount = 0;
 TAP_Osd_Delete (rgnNumKey);
 bPlaybackNumericJumpMode = FALSE;
// 11jun05 TAP_EnterNormal ();
}

int SvcNumToLcn (word svcnum)
{
/*
 int lcn;
 switch (svcnum)
 {
  case 0: lcn = 1; break;
  case 1: lcn = 2; break;
  case 2: lcn = 3; break;
  case 3: lcn = 7; break;
  case 4: lcn = 9; break;
  default: lcn = 0;
 }
 return lcn;
*/
// 03jun05 try the new logical channel number returned by newer api (v1.21 in my case).
TAP_Channel_GetInfo (SVC_TYPE_Tv,  svcnum, &chInfo);
return chInfo.logicalChNum;
}

// word LcnToSvcNum (int lcn)
int LcnToSvcNum (int lcn)
{
// word svcnum;
 int svcnum;
 int i;
 int tvsvc, radiosvc;

/*
 switch (lcn)
 {
  case 1: svcnum = 0; break;
  case 2: svcnum = 1; break;

  case 3: svcnum = 2; break;
  case 7: svcnum = 3; break;
  case 9: svcnum = 4; break;
  default: svcnum = -1;
 }
*/
// 03jun05 try the new logical channel number returned by newer api (v1.21 in my case).
 svcnum = -1;
 TAP_Channel_GetTotalNum (&tvsvc, &radiosvc);
 for (i = 0; i < tvsvc; i++)
 {
    TAP_Channel_GetInfo (SVC_TYPE_Tv,  i, &chInfo);
    if (lcn == chInfo.logicalChNum)
    {
        svcnum = i;
        break;
    }
 }
 return svcnum;
}

int GetFileDuration (TYPE_File file)
{
 int duration;
 word svcnum;

// read duration from header if it's a TS file
 duration = -1; // default return if we can't find duration
 FileListLcn = -1;
 if (file.attr == ATTR_TS)
 {
// 24apr05. it looks like Fopen sometimes returns a non-zero file pointer even when the file is empty.
// this caused me to pick up some spurious duration and lcn info even though there is nothing to read in the file.
// checking the file size first works better, returning -1 for duration and lcn. similar change made in DisplayNewFile.
  if (file.size > 0)
  {
   FileDisplayPointer = TAP_Hdd_Fopen (file.name);
   if (FileDisplayPointer != 0)
   {
// if we can't open the TS file, just don't display the duration

//    TAP_Hdd_Fread (FileDisplayBuffer, 12, 1, FileDisplayPointer);
    TAP_Hdd_Fread (FileDisplayBuffer, sizeof TsFileHeader, 1, FileDisplayPointer);
    duration = FileDisplayBuffer[8] * 255 + FileDisplayBuffer[9];
    FileListDurationSec = FileDisplayBuffer[TsHeaderByteDurationSec];  // top bit set if we have saved seconds
    svcnum = FileDisplayBuffer[10] * 255 + FileDisplayBuffer[11];
    FileListLcn = SvcNumToLcn (svcnum);
    FileListStartHour = FileDisplayBuffer[TsHeaderByteStartHour];
    FileListStartMin = FileDisplayBuffer[TsHeaderByteStartMin];
    FileListStartSec = FileDisplayBuffer[TsHeaderByteStartSec];
    TAP_Hdd_Fclose (FileDisplayPointer);
   }
  }
 }
 return duration;
}

void DisplayFileSize (ulong64 size, char* strnumber, char* strunit)
{
 dword dwtemp;
/*
 if (size >> 20 == 0)
 {
  dwtemp = size;
  sprintf (strnumber, "%d", dwtemp);
  strcpy (strunit, "B");
 }
 else if (size >> 30 == 0)
*/
 if (IsDataFilesFolderShown () || (size >> 30 != 0))
 {
  dwtemp = (size + 524288) >> 20; // round to nearest megabyte (2**20 bytes)
  sprintf (strnumber, "%d", dwtemp);
  strcpy (strunit, "MB");
 }
 else
 {
  dwtemp = (size + 512) >> 10; // round to nearest kilobyte (2**10 bytes)
  sprintf (strnumber, "%d", dwtemp);
  strcpy (strunit, "kB");
 }
}

void WeekdayToAlpha (byte weekday, char *str)
{
 switch (weekday)
 {

  case 0: strcpy (str, "Mon"); break;
  case 1: strcpy (str, "Tue"); break;
  case 2: strcpy (str, "Wed"); break;
  case 3: strcpy (str, "Thu"); break;
  case 4: strcpy (str, "Fri"); break;
  case 5: strcpy (str, "Sat"); break;
  case 6: strcpy (str, "Sun"); break;
  default: strcpy (str, "   "); break;
 }
}

void ShowFileListIndex (dword fileindex)
{
 int yoffset, xoffset;
 char str[255];
 char str1[30];
 char strblank[2];
 word mjd,year;
 byte hour,min,sec,month,day,weekDay;
 dword ColorFore, ColorBack;
 dword dwtemp;
 int width, len;
 int arrayindex;
 int duration, lcn, xoffendname;
// int lcn, xoffendname;
// char strdur[10];
 int xoffendlcn, xoffenddur;
 int xoffdatetime;
 int xoffenddatetime = 590;
 bool tsfile;
 int i;
// bool bFileIsRecording;
 dword secs, mins;
 int recslot, rectuner, rechh, recmm, recss;
 byte dursec, starthour, startmin, startsec;
 char strdur[10];
 int min1, min2;
 byte fontsize;
 int yoff;

  strcpy (strblank, " ");

  arrayindex = fileindex - FileListIndexTop;
  yoffset = FileHeadH + (arrayindex * FileListYincrement);

// if we are in name entry mode and the file entry we are trying to list conflicts with the lines used for
// the rename filename or help prompt, do not list the file entry so that we don't clobber the name entry info.

  if (bNameEntryMode)
  {
   if (yoffset == FileNameEntryYoffset || yoffset == FileNameEntryHelpYoffset)
    return;
  }
  FileListEntry = FileListScreenArray[arrayindex];

  switch (FileListEntry.attr)
  {
  case ATTR_NORMAL:  strcpy (str1, "N"); ColorFore = COLOR_ForeFileListOther; ColorBack = COLOR_BackFileListOther; break;
  case ATTR_TS:  strcpy (str1, "T"); ColorFore = COLOR_ForeFileListTs;  ColorBack = COLOR_BackFileListTs;  break;
  case ATTR_PROGRAM: strcpy (str1, "P"); ColorFore = COLOR_ForeFileListOther; ColorBack = COLOR_BackFileListOther; break;
  case ATTR_FOLDER:  strcpy (str1, "F"); ColorFore = COLOR_ForeFileListFolder; ColorBack = COLOR_BackFileListFolder; break;
  default:   strcpy (str1, "?"); ColorFore = COLOR_ForeFileListOther; ColorBack = COLOR_BackFileListOther; break;
  }
//  ColorBack = COLOR_BackFileListNormal;

// highlight currently playing and recording files
  if (strcmp (FileListEntry.name, FileListNamePlay) == 0)
   ColorFore = COLOR_ForeFileListPlaying;

  bFileIsRecording = FALSE;
  for (i = 0;  i <= 1; i++)
  {
// check name against my copy of what's recording, since I may have renamed it unknown to getrecinfo
//   FileListIndexRec[i] = -1; // default to indicate no recording in slot i
//   if (strcmp (FileListEntry.name, FileListNameRec[i]) == 0)
// 26apr05. can think a file is recording when it isn't, due to name matching what recinfo returns,
// so check file size too. if it is non-zero, it can't be recording (i hope).
// probably better to err on the side of not showing it when it is recording, rather than vice-versa, since
// in the latter case, we don't show its size.
// 13may05. actually a recording file can sometimes show a non-zero size. i had two overlapping timers
// where the second started 1 minute before the first finished. the size of the second showed
// 52828 kB during most of its recording (because I thought it was not recording the size was displayed
// instead of the elapsed time). it looks like the size got updated when the first recording stopped
// since it corresponds to about 1 minute of recording time.

//   if ((FileListEntry.size == 0) && (strcmp (FileListEntry.name, FileListNameRec[i]) == 0))
   if (strcmp (FileListEntry.name, FileListNameRec[i]) == 0)
   {
    bFileIsRecording = TRUE;
    FileListIndexRec[i] = fileindex;
    if (strcmp (FileListEntry.name, FileListNamePlay) == 0)
        ColorFore = COLOR_ForeFileListPlayingRecording;
    else
        ColorFore = COLOR_ForeFileListRecording;
// save start time calculated by idle event
    starthour = RecStartHour[i];
    startmin = RecStartMin[i];
    startsec = RecStartSec[i];
// get slot number, tuner number and service number of recording file for display
    if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]))
    {
//     if (RecInfo[i].recType == RECTYPE_Normal)
     if ((RecInfo[i].recType == RECTYPE_Normal) || (RecInfo[i].recType == RECTYPE_Copy))
     {
      lcn = SvcNumToLcn (RecInfo[i].svcNum);
      recslot = i + 1;
      rectuner = RecInfo[i].tuner + 1;
      secs = RecInfo[i].recordedSec;
      mins = secs / 60;
      rechh = mins / 60;
      recmm = mins % 60;
      recss = secs % 60;
     }
    }
   }
  }

  if (fileindex == FileListIndex)
  {
// for highlighting current entry, swap foreground and background colors
   dwtemp = ColorFore;
   ColorFore = ColorBack;
   ColorBack = dwtemp;
  }
// put the background color across the whole screen width first, because we may not fill in all fields
// this should not be necessary if a ts file, since all fields should be filled in, and stops flicker as
// recording time updates.

//  if (FileListEntry.attr != ATTR_TS || fileindex == FileListIndex)
  if (FileListEntry.attr != ATTR_TS)
  {
//   strcpy (str, " ");
//   TAP_Osd_PutS(rgnFile, 0, yoffset, xmax, str, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
   TAP_Osd_FillBox (rgnFile, 0, yoffset, xmax, FileListYincrement, ColorBack);
  }
  else
  {
// clear out the small space below the font (which is 22 pixels high) and the next line (which is 24 pixels away).
   TAP_Osd_FillBox (rgnFile, 0, yoffset + 22, xmax, 2, ColorBack);
  }

  sprintf (str, "%d", fileindex);
//  xoffset = 60 - TAP_Osd_GetW (str, 0, FNT_Size_FileList);
  xoffset = 55 - TAP_Osd_GetW (str, 0, FNT_Size_FileList);
//  TAP_Osd_PutS(rgnFile, xoffset, yoffset, 70, str, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
  TAP_Osd_PutS(rgnFile, xoffset, yoffset, 65, str, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
//  TAP_Osd_PutS(rgnFile, 0, yoffset, xoffset, strblank, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
  TAP_Osd_FillBox (rgnFile, 0, yoffset, xoffset, FileListYincrement, ColorBack);

// 04may05. don't show attribute flag now, color coding should be enough (leaves more room for other things)
//  TAP_Osd_PutS(rgnFile, 70, yoffset, 90, str1, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);

//  xoffendname = 430; // x offset for end of filename if not TS or can't get duration
  if (FileListEntry.attr == ATTR_TS)
//   xoffendname = 430;
//   xoffendname = 390;   // allow for 3-char lcn
//   xoffendname = 370;   // allow for 3-char lcn
    xoffendname = 395;
  else
    xoffendname = 405;
  xoffendlcn = xoffendname;
  tsfile = FALSE;

  if (FileListEntry.attr != ATTR_FOLDER)
  {
   if (FileListEntry.attr == ATTR_TS)
   {
    tsfile = TRUE;
    if (strcmp (FileListEntry.name, FileListNamePlay) == 0)
    {
     duration = FileListDurationPlay;
     dursec = FileListDurationSecPlay;
     lcn = FileListLcnPlay;
     starthour = FileListStartHourPlay;
     startmin = FileListStartMinPlay;
     startsec = FileListStartSecPlay;
    }
    else
    {
     if (!bFileIsRecording)
     {
      duration = FileListDurationArray [arrayindex];
      dursec = FileListDurationSecArray [arrayindex];
      lcn = FileListLcnArray [arrayindex];
      starthour = FileListStartHourArray[arrayindex];
      startmin = FileListStartMinArray[arrayindex];
      startsec = FileListStartSecArray[arrayindex];
     }
// already got lcn if file is recording
    }
// if file is recording, put slot and tuner in place of duration field
    if (bFileIsRecording)
       sprintf (strdur, "%d:%02d:%02d", rechh, recmm, recss);
    else
    {
      if (duration >= 0)
      {
        sprintf (strdur, "%d:%02d", duration/60, duration%60);
        if ((dursec & 0x80) == 0x80)
        {
            sprintf (str, ":%02d", dursec & 0x7F);
            strcat (strdur, str);
        }
      }
      else
        strcpy (strdur, strblank);
    }

    width = TAP_Osd_GetW ("9:59:59", 0, FNT_Size_FileList);
//    xoffendname = xoffendname + 25 - width; // +25 because we will drop year in date
//    xoffenddur = xoffendname + width;
    xoffenddur = xoffendname;
    xoffendname = xoffenddur - width;
    TAP_Osd_PutS(rgnFile, xoffendname, yoffset, xoffenddur, strdur, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
//    TAP_Osd_PutS(rgnFile, xoffenddur, yoffset, xoffenddur+10, strblank, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
    TAP_Osd_FillBox (rgnFile, xoffenddur, yoffset, 5, FileListYincrement, ColorBack);
// like duration, lcn could be -1 if we failed to get it from ts header, so leave blank in this case
    if (lcn >= 0)
        sprintf (str, "C%d", lcn);
    else
        strcpy (str, strblank);
    width = TAP_Osd_GetW (str, 0, FNT_Size_FileList);
    xoffendlcn = xoffenddur + 5 + width;
    TAP_Osd_PutS(rgnFile, xoffenddur + 5, yoffset, xoffendlcn, str, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);

   } // end of ts file

   if (TAP_ExtractMjd (FileListEntry.mjd, &year, &month, &day, &weekDay))
   {
    WeekdayToAlpha (weekDay, str);
    if (tsfile)
     sprintf (str1, " %02d/%02d", day, month);
    else
     sprintf (str1, " %02d/%02d/%02d", day, month, year%100);
    strcat (str, str1);
   }
   else
    strcpy (str, " ");
   width = TAP_Osd_GetW (str, 0, FNT_Size_FileList);
// allow for seconds in non-ts files
// 28jun05 now need to allow for seconds in ts files too
    xoffdatetime = 520 - width;
//    TAP_Osd_PutS(rgnFile, xoffdatetime, yoffset, 590, str, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
    TAP_Osd_PutS(rgnFile, xoffdatetime, yoffset, xoffdatetime + width, str, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
//    TAP_Osd_PutS(rgnFile, xoffendlcn, yoffset, xoffdatetime, strblank, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
    if (xoffendlcn < xoffdatetime)
        TAP_Osd_FillBox (rgnFile, xoffendlcn, yoffset, xoffdatetime - xoffendlcn, FileListYincrement, ColorBack);

// ts files always have seconds = zero for some reason so don't bother to display seconds, others do not.
// 28jun05 we may now save start time seconds in ts header. if so, and the hour and minute saved matches
// those in the file entry, add it to the start time display, else just show hours and minutes from file entry
   if (tsfile)
   { 
// determine whether or not to use saved seconds in start time.
// if file is recording, use them
// if not, use them if the flag in the saved duration in the tsheader is set, provided the
// saved time matches that in the file entry to within 1 minute.
    sprintf (str, "%02d:%02d", FileListEntry.hour, FileListEntry.min);
    min1 = (starthour * 60) + startmin;
    min2 = (FileListEntry.hour * 60) + FileListEntry.min;
    if (bFileIsRecording || (((dursec & 0x80) == 0x80) && ((min1 - min2) >= -1) && ((min1 - min2) <= 1)))
        sprintf (str, "%02d:%02d:%02d", starthour, startmin, startsec);
   }
   else
    sprintf (str, "%02d:%02d:%02d", FileListEntry.hour, FileListEntry.min, FileListEntry.sec);
//   TAP_Osd_PutS(rgnFile, xoffdatetime + width + 10, yoffset, 600, str, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
   TAP_Osd_PutS(rgnFile, xoffdatetime + width + 5, yoffset, xoffenddatetime, str, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
// clear out the gap between the end of the date and the start of the time
   TAP_Osd_FillBox (rgnFile, xoffdatetime + width, yoffset, 5, FileListYincrement, ColorBack);
  } // end of not folder


// if a .rec file in DataFiles folder, suppress the .rec from display
  strcpy (str, FileListEntry.name);
  if (strcmp (FileListPath[FileListPathLevel], "DataFiles") == 0)
  if (IsRec (str))
  {   
// remove the ".rec" from the end of the filename
// i can't find a substring function, so stick a string delimeter in the 4th last char to get rid of ".rec"
   len = strlen(str);
   if (len > 4)
    str[len-4] = '\0';
  }
  xoffendname -= 5;
  if (TAP_Osd_GetW (str, 0, FNT_Size_FileList) <= xoffendname - 65)
  {
    fontsize = FNT_Size_FileList;
    yoff = yoffset;
  }
  else
  {
    fontsize = FNT_Size_1419;
// move the smaller font down so it sits on same baseline
    yoff = yoffset + 3;
// clear out the extra space above the smaller font
//    TAP_Osd_FillBox (rgnFile, 65, yoffset + 19, xoffendname, 3, ColorBack);
    TAP_Osd_FillBox (rgnFile, 65, yoffset, xoffendname, 3, ColorBack);
  }
//  TAP_Osd_PutS(rgnFile, 65, yoffset, xoffendname, str, ColorFore, ColorBack, 0, fontsize, 1, ALIGN_LEFT);
  TAP_Osd_PutS(rgnFile, 65, yoff, xoffendname, str, ColorFore, ColorBack, 0, fontsize, 1, ALIGN_LEFT);
// clear out the gap from the end of the name to the start of the duration
  TAP_Osd_FillBox (rgnFile, xoffendname, yoffset, 5, FileListYincrement, ColorBack);

  if (bFileIsRecording)
  {
   sprintf (str, "S%d T%d", recslot, rectuner);
   xoffset = FileWinXMarR - TAP_Osd_GetW (str, 0, FNT_Size_FileList);
   TAP_Osd_PutS(rgnFile, xoffset, yoffset, xmax, str, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
  }
  else
  {
   DisplayFileSize (FileListEntry.size, str, str1);
   xoffset = FileWinXMarR - 30 - TAP_Osd_GetW (str, 0, FNT_Size_FileList);
   TAP_Osd_PutS(rgnFile, xoffset, yoffset, FileWinXMarR, str, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
   TAP_Osd_PutS(rgnFile, FileWinXMarR - 25, yoffset, xmax, str1, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
  }
//  TAP_Osd_PutS(rgnFile, 600, yoffset, xoffset, strblank, ColorFore, ColorBack, 0, FNT_Size_FileList, 0, ALIGN_LEFT);
//  TAP_Osd_FillBox (rgnFile, 600, yoffset, xoffset - 600, FileListYincrement, ColorBack);
// clear out the gap between the end of the date/time and the start of the size
   if (xoffenddatetime < xoffset)
       TAP_Osd_FillBox (rgnFile, xoffenddatetime, yoffset, xoffset - xoffenddatetime, FileListYincrement, ColorBack);

}

void ShowPlayCounter ()
{
    dword secs, mins;
    int hh, mm, ss, x, w;
    char str[30];
// if play counter is not already up, initiate it, unless clock is shown (since play counter info
// is in the clock extras, anyway).
// if it is already up, just update its value (called from idle every second)
//  if (!bClockShown)
// 27jun don't clobber file error msg
  if (!bClockShown && !bFileErrorMsgShown)
  {
    if (!bPlayCounterShown)
    {
// use same area as clock
        rgnPlayCounter = TAP_Osd_Create (0, ClockY, xmax, ClockH, 0, FALSE);
        bPlayCounterShown = TRUE;
    }
    if (TAP_Hdd_GetPlayInfo (&PlayInfo) && (PlayInfo.playMode == PLAYMODE_Playing))
    {
//      secs = (PlayInfo.currentBlock * PlayInfo.duration * 60) / PlayInfo.totalBlock;
//      secs = ((PlayInfo.currentBlock * 60) + (BlockRateNormalPlay / 2)) / BlockRateNormalPlay;
// rate is now per 10 minutes
      secs = ((PlayInfo.currentBlock * 600) + (BlockRateNormalPlay / 2)) / BlockRateNormalPlay;
      mins = secs / 60;
      hh = mins / 60;
      mm = mins % 60;
      ss = secs % 60;
//      sprintf (str, "%d:%02d:%02d (%d blocks/min)", hh, mm, ss, BlockRateNormalPlay);
//      sprintf (str, "%d:%02d:%02d (%d.%d blocks/min)", hh, mm, ss, BlockRateNormalPlay / 10, BlockRateNormalPlay % 10);
      sprintf (str, "%d:%02d:%02d", hh, mm, ss);
      w = TAP_Osd_GetW (str, 0, FNT_Size_PlayRec) + (2 * PlayRecXoff);
//      x = (xmax - w) / 2;
//      x = 60;
      x = 40;
      TAP_Osd_PutS (rgnPlayCounter, x, 0, x + w - 1, str, COLOR_ForePlayStatus, COLOR_BackPlayStatus, 0, FNT_Size_PlayRec, 0, ALIGN_CENTER);
// clear out areas either side
      TAP_Osd_FillBox (rgnPlayCounter, 0, 0, x, ClockH, 0xFF);
      TAP_Osd_FillBox (rgnPlayCounter, x + w, 0, xmax - (x + w - 1), ClockH, 0xFF);
    }
  }
}

void HidePlayCounter ()
{
    TAP_Osd_Delete (rgnPlayCounter);
    bPlayCounterShown = FALSE;
}

bool PositionPlayback (dword block)
{
    if (TAP_Hdd_GetPlayInfo (&PlayInfo) && (PlayInfo.playMode == PLAYMODE_Playing))
    {
        if (block < PlayInfo.totalBlock)
        {
            ShowPlayCounter ();
            TickPlayCounterSaved = TAP_GetTick ();
            TickClockSaved = TickPlayCounterSaved;  // for idle event every-second test
/*
            if (PlayInfo.trickMode == TRICKMODE_Normal)
            {
                TicksInNormalPlayCurrent = 0;
                BlocksInNormalPlayCurrent = 0;
            }
*/
            if (TAP_Hdd_ChangePlaybackPos (block))
            {
// 16jun05 don't show play counter immediately after jump, because there is then a gap of 3 to 4 sec
// to the first one in idle
//                ShowPlayCounter ();
//                TickPlayCounterSaved = TAP_GetTick ();
//                TickClockSaved = TickPlayCounterSaved;  // for idle event every-second test
// make idle event think we have just entered normal play after positioning
// by setting trickmodeprev other than normal
// so that it resets the counters. system always seems to resume normal play after positioning
// even if was in other trickmodes before.
                TrickModePrev = TRICKMODE_Pause;
                return TRUE;
            }
            else
                ShowFileErrorMsg ("Playback positioning failed", FALSE);
        }
    }
    else
        ShowFileErrorMsg ("Incorrect PlayInfo state for positioning playback", FALSE);
    return FALSE;
}

// bool SkipPlayback (int secs)
bool SkipPlayback (long secs)
{
    dword block, offset, tick;
    bool back;
    
    if (TAP_Hdd_GetPlayInfo (&PlayInfo) && (PlayInfo.playMode == PLAYMODE_Playing))
    {
// the first second in idle after positioning jumps 3 or 4 sec based on block count,
// so allow for this here
        if (secs < 0)
        {
//            secs = -secs;
            secs = -secs + 3;
            back = TRUE;
        }
        else
        {
            if (secs >= 3)
                secs = secs - 3;
            else
                secs = 0;
            back = FALSE;
        }
//        offset = (PlayInfo.totalBlock * secs) / (PlayInfo.duration * 60);
// BlockRateNormalPlay is per 10 sec. round the calculated block offset.
//        offset = ((BlockRateNormalPlay * secs) + 5) / 10;
// try block rate per minute for more precision
/*
        if (PlayInfo.trickMode == TRICKMODE_Normal)
        {
            tick = TAP_GetTick ();
            TicksInNormalPlayCumulative += (tick - TickBlockRateSaved);
            BlocksInNormalPlayCumulative += (PlayInfo.currentBlock - BlockCountSaved);
// only recalculate running average block rate if we have been in normal play for at least 1 minute
            if (TicksInNormalPlayCumulative >= 6000)
                BlockRateNormalPlay = BlocksInNormalPlayCumulative * 6000 / TicksInNormalPlayCumulative;
            TickBlockRateSaved = tick;
            BlockCountSaved = PlayInfo.currentBlock;
        }
*/
//        offset = ((BlockRateNormalPlay * secs) + 30) / 60;
        offset = ((BlockRateNormalPlay * secs) + 300) / 600;
        if (back)
        {
// asssuming here that blocks are numbered from 0 to totalBlock - 1;
            if (PlayInfo.currentBlock >= offset)
                block = PlayInfo.currentBlock - offset;
            else
            {
/*
                ShowFileErrorMsg ("Too close to start of file to skip back", FALSE);
                return FALSE;
*/              block = 0;
            }
        }
// for forward skips, PositionPlayback will check that calculated block is within total blocks.
        else
        {
            block = PlayInfo.currentBlock + offset;
            if (block >= PlayInfo.totalBlock)
            {
                ShowFileErrorMsg ("Too close to end of file to skip forward", FALSE);
                return FALSE;
            }
        }
//        if (TAP_Hdd_ChangePlaybackPos (block))
//            return TRUE;
        if (PositionPlayback (block))
            return TRUE;
//        else
//            ShowFileErrorMsg ("Playback positioning failed", FALSE);
    }
    else
        ShowFileErrorMsg ("Incorrect PlayInfo state for skipping playback", FALSE);
    return FALSE;
}

void ShowPlayRecInfo (bool bPlay)
{
    int yoff1, width;
    byte fontsize;
    dword ColorFore, ColorBack;

    if (bPlay)
    {
        ColorFore = COLOR_ForePlayStatus;
        ColorBack = COLOR_BackPlayStatus;
    }
    else
    {
        ColorFore = COLOR_ForeRecStatus;
        ColorBack = COLOR_BackRecStatus;
    }
      PlayRecW = TAP_Osd_GetW (strPlayRec, 0, FNT_Size_PlayRec);
      width = TAP_Osd_GetW (strPlayRecFileName, 0, FNT_Size_PlayRec);
      if (PlayRecW + width + (2 * PlayRecXoff) > xmax - 60)
      {
          fontsize = FNT_Size_1419;
          width = TAP_Osd_GetW (strPlayRecFileName, 0, fontsize);
// start the smaller font lower so it sits on same baseline
          yoff1 = PlayRecInfoYoff + 3;
      }
      else
      {
          fontsize = FNT_Size_PlayRec;
          yoff1 = PlayRecInfoYoff;
      }

      if (PlayRecW + width + (2 * PlayRecXoff) > xmax - 60)
        width = xmax - 60 - PlayRecW - (2 * PlayRecXoff);
      PlayRecX = (xmax - (PlayRecW + width + (2 * PlayRecXoff))) / 2;
// display the first part
      TAP_Osd_PutS (rgnPlayRec, PlayRecX, PlayRecInfoYoff, PlayRecX + PlayRecW + PlayRecXoff, strPlayRec, ColorFore, ColorBack, 0, FNT_Size_PlayRec, 1, ALIGN_RIGHT);
// now the file name
      TAP_Osd_PutS (rgnPlayRec, PlayRecX + PlayRecW + PlayRecXoff, yoff1, PlayRecX + PlayRecW + width + (2 * PlayRecXoff), strPlayRecFileName, ColorFore, ColorBack, 0, fontsize, 1, ALIGN_LEFT);
// clear out areas either side (and between the two parts)
      TAP_Osd_FillBox (rgnPlayRec, 0, PlayRecInfoYoff, PlayRecX, PlayRecH, 0xFF);
      TAP_Osd_FillBox (rgnPlayRec, PlayRecX + PlayRecW + width + (2 * PlayRecXoff), PlayRecInfoYoff, xmax - (PlayRecX + PlayRecW + width), PlayRecH, 0xFF);
        
      if (fontsize == FNT_Size_1419)
          TAP_Osd_FillBox (rgnPlayRec, PlayRecX + PlayRecW + PlayRecXoff, PlayRecInfoYoff, width + PlayRecXoff, 3, ColorBack);

      PlayRecInfoYoff = PlayRecInfoYoff - PlayRecH;
}

void UpdateClock ()
{
// update clock display if second has changed since last time check (called each second by idle event)

 dword freesize;
 int prc, recfiles;
 int tvrad, chan, lcn;
 dword tick, secs, mins;
 int hh, mm, ss;
 int hhstart, mmstart, hhdur, mmdur;
// int yoff, len, i;
 int len, i;
 char str[30];
//  char strtemp[TS_FILE_NAME_SIZE];
 word duration;
 char strdur[12];
 int ssdur;
 char strblockrate[30];
 dword TimeRecorded;
// int hhrec, mmrec;
 byte hhrec, mmrec, ssrec;
 word mjdrec;
 word year;
 byte month, day, weekDay;
 char daystr[4];
/*
 int width;
 byte fontsize;
 int yoff1;
*/
 char strtuner[4];

 dword tuner = -1, chnum;

// if error message shown, don't show clock line (it uses same area of screen) but still show play/rec info
// also name entry help now uses clock line
 if (!bFileErrorMsgShown && !bErrorMsgShown && !bNameEntryHelpShown)
 { 
   TAP_GetTime( &mjd, &hour, &min, &sec);
   freesize = TAP_Hdd_FreeSize();        //megabytes remaining
   prc = freesize*1000/totalsize;  // calculate percentage (*10) for free space
   recfiles = CountRecFiles ();
   TAP_Channel_GetCurrent (&tvrad, &chan);
   lcn = SvcNumToLcn (chan);

  // call modified code to find tuner, if initialization of it worked 
   if (getTuner == NULL)
    strcpy (strtuner, "");
   else
   {
    getTuner(&tuner, &chnum); 
    sprintf (strtuner, " T%d", tuner + 1); 
   }

   if (TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay ))
//    sprintf (strclock, "%02d/%02d/%02d %02d:%02d:%02d, %dMB (%d.%01d%%) free, %d rec, C%d", day, month, year, hour, min, sec, freesize, prc/10, prc%10, recfiles, lcn);
//    sprintf (strclock, "%02d/%02d/%02d %02d:%02d:%02d, %dMB (%d.%01d%%) free, %d rec, C%d T%d", day, month, year, hour, min, sec, freesize, prc/10, prc%10, recfiles, lcn, tuner + 1);
    sprintf (strclock, "%02d/%02d/%02d %02d:%02d:%02d, %dMB (%d.%01d%%) free, %d rec, C%d%s", day, month, year, hour, min, sec, freesize, prc/10, prc%10, recfiles, lcn, strtuner);
   else
//    sprintf (strclock, "%02d:%02d:%02d %dMB (%d.%01d%%) free, %d rec, C%d", hour, min, sec, freesize, prc/10, prc%10, recfiles, lcn);
//    sprintf (strclock, "%02d:%02d:%02d %dMB (%d.%01d%%) free, %d rec, C%d T%d", hour, min, sec, freesize, prc/10, prc%10, recfiles, lcn, tuner + 1);
    sprintf (strclock, "%02d:%02d:%02d %dMB (%d.%01d%%) free, %d rec, C%d%s", hour, min, sec, freesize, prc/10, prc%10, recfiles, lcn, strtuner);
   if (bFilePlayingBack)
   {
    tick = TAP_GetTick ();
    secs = (TickFilePlay + tick - TickFilePlayStart) / 100;
    mins = secs / 60;
    hh = mins / 60;
    mm = mins % 60;
    ss = secs % 60;
    sprintf (str, ", Play %d:%02d:%02d since %02d:%02d", hh, mm, ss, HourFilePlayStart, MinFilePlayStart);
    strcat (strclock, str);
   }
   if (bFileMode)
    TAP_Osd_PutS(rgnFile, 0, FileErrorYoffset, FileWinW-1, strclock, COLOR_ForeClock, COLOR_BackClock, 0, FNT_Size_FileError, 1, ALIGN_CENTER);
   else if (bTimerMode)
    TAP_Osd_PutS(rgnTimer, 0, TimerErrorYoffset, TimerWinW-1, strclock, COLOR_ForeClock, COLOR_BackClock, 0, FNT_Size_FileError, 1, ALIGN_CENTER);
   else
   {
// blank out clock line first, in case new display width is less than previous one
//    TAP_Osd_PutS (rgnClock, 0, ClockYoff, xmax, " ", 0xFF, 0xFF, 0, FNT_Size_Clock, 0, ALIGN_CENTER);
    ClockW = TAP_Osd_GetW (strclock, 0, FNT_Size_Clock) + (2 * ClockXoff);
    ClockX = (xmax - ClockW) / 2;
//    TAP_Osd_PutS (rgnClock, ClockX, ClockYoff, ClockX + ClockW - 1, strclock, COLOR_ForeClock, COLOR_BackClock, 0, FNT_Size_Clock, 0, ALIGN_CENTER);
    TAP_Osd_PutS (rgnClock, ClockX, ClockYoff, ClockX + ClockW, strclock, COLOR_ForeClock, COLOR_BackClock, 0, FNT_Size_Clock, 0, ALIGN_CENTER);
// clear out areas either side of clock
    TAP_Osd_FillBox (rgnClock, 0, ClockYoff, ClockX, ClockH, 0xFF);
//    TAP_Osd_FillBox (rgnClock, ClockX + ClockW, ClockYoff, xmax - (ClockX + ClockW - 1), ClockH, 0xFF);
    TAP_Osd_FillBox (rgnClock, ClockX + ClockW, ClockYoff, xmax - (ClockX + ClockW), ClockH, 0xFF);
   }
 } // end if no error message shown


// if playing or recording (and not in file or timer mode), show extra status boxes above clock
   if (!bFileMode && !bTimerMode)
   {
    PlayRecInfoYoff = PlayRecYoff + (2 * PlayRecH);

    if (TAP_Hdd_GetPlayInfo (&PlayInfo) && (PlayInfo.playMode == PLAYMODE_Playing))
    {
// this calculation seemed to recycle to zero after about 5 hours and 34 minutes, so try without rounding, which may
// be causing numerator to overflow
//      secs = (PlayInfo.currentBlock * PlayInfo.duration * 60 + (PlayInfo.totalBlock / 2)) / PlayInfo.totalBlock;
//      secs = (PlayInfo.currentBlock * PlayInfo.duration * 60) / PlayInfo.totalBlock;
      secs = ((PlayInfo.currentBlock * 600) + (BlockRateNormalPlay / 2)) / BlockRateNormalPlay;
      prc = PlayInfo.currentBlock * 1000 / PlayInfo.totalBlock;  // calculate percentage (*10) from block counts
      mins = secs / 60;
      hh = mins / 60;
      mm = mins % 60;
      ss = secs % 60;
/*
      TimeRecorded = TimeAdjust (FileListStartTimePlay, (secs + 30) / 60);
      TAP_ExtractMjd (TimeRecorded>>16, &year, &month, &day, &weekDay);
      WeekdayToAlpha (weekDay, daystr);
      hhrec = (TimeRecorded & 0xFF00) >> 8;
      mmrec = TimeRecorded & 0xFF;
*/
      mjdrec = FileListStartPlayMjd;
      hhrec = FileListStartPlayHour;
      mmrec = FileListStartPlayMin;
      ssrec = FileListStartPlaySec;
      TimeAdjust (&mjdrec, &hhrec, &mmrec, &ssrec, secs);
      TAP_ExtractMjd (mjdrec, &year, &month, &day, &weekDay);
      WeekdayToAlpha (weekDay, daystr);
/* 25jun05 replace duration with date/time recorded
//      hhdur = PlayInfo.duration / 60;
//      mmdur = PlayInfo.duration % 60;
      duration = PlayInfo.duration;
      if (duration == 0xFFFF)
       strcpy (strdur, "unlimited");
      else
      {
// instead of using alleged duration in minutes (which could be wrong if file corrupt)
// extrapolate actual duration from dynamic block rate.
        secs = ((PlayInfo.totalBlock * 600) + (BlockRateNormalPlay / 2)) / BlockRateNormalPlay;
        mins = secs / 60;
        hhdur = mins / 60;
        mmdur = mins % 60;
        ssdur = secs % 60;
        sprintf (strdur, "%d:%02d:%02d", hhdur, mmdur, ssdur);
      }
*/

      lcn = SvcNumToLcn (PlayInfo.svcNum);
// remove the ".rec" from the end of the filename
// i can't find a substring function, so stick a string delimiter in the 4th last char to get rid of ".rec"
// there is a bug in playinfo, where if a file rename is done while playing back, it will return the name of the
// newly-renamed file, instead of the playing file, so use my playback file name if playing back from my file list.
// this bug may be fixed in API v1.20 (needs March 2005 firmware).
      if (bFilePlayingBack)
       strcpy (strPlayRecFileName, FileListNamePlay);
      else
       strcpy (strPlayRecFileName, PlayInfo.file->name);
      if (IsRec (strPlayRecFileName))
      {
       len = strlen(strPlayRecFileName);
       if (len > 4)
        strPlayRecFileName[len-4] = '\0';
      }
//      sprintf (strPlayRec, "Play: %d:%02d:%02d / %d:%02d, C%d, %s", hh, mm, ss, hhdur, mmdur, lcn, strtemp);
//      sprintf (strPlayRec, "Play: %d:%02d:%02d / %d:%02d (%d.%01d%%) C%d, %s", hh, mm, ss, hhdur, mmdur, prc/10, prc%10, lcn, strtemp);
//      sprintf (strPlayRec, "Play: %d:%02d:%02d / %s (%d.%01d%%) C%d, %s", hh, mm, ss, strdur, prc/10, prc%10, lcn, strtemp);
//      sprintf (strPlayRec, "Play: %d:%02d:%02d (%d.%d blocks/min) / %s (%d.%01d%%) C%d, %s", hh, mm, ss, BlockRateNormalPlay / 10, BlockRateNormalPlay % 10, strdur, prc/10, prc%10, lcn, strtemp);
      strcpy (strdur, "");
      if (bBlockRatesShown)
      {
          if (bBlockRateDynamic)
            sprintf (strblockrate, ", %d.%d, %d.%d*", BlockRateNormalPlayInit/10, BlockRateNormalPlayInit%10, BlockRateNormalPlayDyn/10, BlockRateNormalPlayDyn%10);
          else
            sprintf (strblockrate, ", %d.%d*, %d.%d", BlockRateNormalPlayInit/10, BlockRateNormalPlayInit%10, BlockRateNormalPlayDyn/10, BlockRateNormalPlayDyn%10);
          strcat (strblockrate, " bpm");
      }
      else
      {
          strcpy (strblockrate, "");
// block rates not shown, show duration after play time
          if (FileListDurationPlay >= 0)
          {
            sprintf (strdur, " / %d:%02d", FileListDurationPlay/60, FileListDurationPlay%60);
            if ((FileListDurationSecPlay & 0x80) == 0x80)
            {
                sprintf (str, ":%02d", FileListDurationSecPlay & 0x7F);
                strcat (strdur, str);
            }
          }
      }
//      sprintf (strPlayRec, "Play: %d:%02d:%02d (%s %02d/%02d %02d:%02d:%02d, %d.%01d%%, %s) C%d, ", hh, mm, ss, daystr, day, month, hhrec, mmrec, ssrec, prc/10, prc%10, strblockrate, lcn);
//      sprintf (strPlayRec, "Play: %d:%02d:%02d (%d.%01d%%, %s %02d/%02d %02d:%02d:%02d, %s) C%d, ", hh, mm, ss, prc/10, prc%10, daystr, day, month, hhrec, mmrec, ssrec, strblockrate, lcn);
//      sprintf (strPlayRec, "Play: %d:%02d:%02d (%d.%01d%%, %s %02d/%02d %02d:%02d:%02d%s) C%d, ", hh, mm, ss, prc/10, prc%10, daystr, day, month, hhrec, mmrec, ssrec, strblockrate, lcn);
      sprintf (strPlayRec, "Play: %d:%02d:%02d%s (%d.%01d%%, %s %02d/%02d %02d:%02d:%02d%s) C%d, ", hh, mm, ss, strdur, prc/10, prc%10, daystr, day, month, hhrec, mmrec, ssrec, strblockrate, lcn);
      ShowPlayRecInfo (TRUE);
    }
//    else
//     TAP_Osd_PutS (rgnPlayRec, 0, yoff, xmax, " ", 0xFF, 0xFF, 0, FNT_Size_PlayRec, 1, ALIGN_CENTER);

    for (i = 0;  i <= 1; i++)
    {
// 23mar05 already done recinfo above, but seems to stuff up recinfo if playing back and recinfo not refreshed
//     if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]) && (RecInfo[i].recType == RECTYPE_Normal))
     if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]) && ((RecInfo[i].recType == RECTYPE_Normal) || (RecInfo[i].recType == RECTYPE_Copy)))
     {
/*
       hhstart = (RecInfo[i].startTime >> 8) & 0xFF;
       mmstart = RecInfo[i].startTime & 0xFF;
*/
       secs = RecInfo[i].recordedSec;
       mins = secs / 60;
       hh = mins / 60;
       mm = mins % 60;
       ss = secs % 60;
       duration = RecInfo[i].duration;
       if (duration == 0xFFFF)
        strcpy (strdur, "unlimited");
       else
       {
        hhdur = duration / 60;
        mmdur = duration % 60;
        sprintf (strdur, "%d:%02d", hhdur, mmdur);
       }
       lcn = SvcNumToLcn (RecInfo[i].svcNum);
// remove the ".rec" from the end of the filename
// i can't find a substring function, so stick a string delimiter in the 4th last char to get rid of ".rec"
       strcpy (strPlayRecFileName, FileListNameRec[i]);
       if (IsRec (strPlayRecFileName))
       {
        len = strlen(strPlayRecFileName);
        if (len > 4)
         strPlayRecFileName[len-4] = '\0';
       }
//       sprintf (strPlayRec, "Rec %d: tuner %d since %02d:%02d:%02d, %d:%02d:%02d / %s, C%d, %s", i + 1, RecInfo[i].tuner + 1, RecStartHour[i], RecStartMin[i], RecStartSec[i], hh, mm, ss, strdur, lcn, strtemp);
       sprintf (strPlayRec, "Rec %d: tuner %d since %02d:%02d:%02d, %d:%02d:%02d / %s, C%d, ", i + 1, RecInfo[i].tuner + 1, RecStartHour[i], RecStartMin[i], RecStartSec[i], hh, mm, ss, strdur, lcn);
       ShowPlayRecInfo (FALSE);
     }
//     else
//      TAP_Osd_PutS (rgnPlayRec, 0, yoff, xmax, " ", 0xFF, 0xFF, 0, FNT_Size_PlayRec, 1, ALIGN_CENTER);
    } // end of loop on each recording slot

// if we didn't use all three lines of play/rec area, blank out the unused lines
    for (i = PlayRecInfoYoff; i >= PlayRecYoff; i = i - PlayRecH)
    {
//     TAP_Osd_PutS (rgnPlayRec, 0, i, xmax, " ", 0xFF, 0xFF, 0, FNT_Size_PlayRec, 1, ALIGN_CENTER);
     TAP_Osd_FillBox (rgnPlayRec, 0, i, xmax, PlayRecH, 0xFF);
    }
   } // end if not timer or file mode
}


void HideClock ()
{
 TAP_Osd_Delete (rgnClock);
 TAP_Osd_Delete (rgnPlayRec);
// TAP_EnterNormal ();
 bClockShown = FALSE;
}

void ReturnToNormal(void)
{
// TAP_Channel_Scale(CHANNEL_Main, 0, 0, xmax, ymax, FALSE);
 ScaleMainFull ();
 TAP_Channel_Stop(CHANNEL_Sub);
// HideSubPicture();
// ClearPIPBorder();

 bPIPshown = FALSE;
 iPOPshown = 0;
}

void ReservationTypeToString (TYPE_ReservationType restype, char* str)
{
// char str[3];
 switch (restype)

 {
  case RESERVE_TYPE_Onetime:  strcpy (str, "OT"); break;
  case RESERVE_TYPE_Everyday: strcpy (str, "ED"); break;
  case RESERVE_TYPE_EveryWeekend: strcpy (str, "WE"); break;
  case RESERVE_TYPE_Weekly:  strcpy (str, "WL"); break;
  case RESERVE_TYPE_WeekDay:  strcpy (str, "WD"); break;
  default:     strcpy (str, "  "); break;
 }
// return str;
 return;
}

void SortTimerListArray ()
{
 int nswaps,i;
 int total_timers;
 TYPE_TimerInfo tempbuff;
 int tempnum;

 total_timers = TAP_Timer_GetTotalNum();
 if (total_timers <= 1)
  return;
 do {
  for (nswaps = i = 0 ; i < total_timers - 1 ; i++)
  {
   switch (TimerListSortKey)
   {
    case 1:
    {
// sort by file name
     if (strcmp (TimerListArray[i].fileName, TimerListArray[i+1].fileName) <= 0)
      continue;
     else
      break;
    }
    case 2:
    {
// sort by original RAM timer index
     if (TimerNumArray[i] <= TimerNumArray[i+1])
      continue;
     else
      break;
    }
    default:
    {
// sort by start time, then duration, then service number
     if (TimerListArray[i].startTime < TimerListArray[i+1].startTime)
      continue;
     else if (TimerListArray[i].startTime == TimerListArray[i+1].startTime)
     {
      if (TimerListArray[i].duration < TimerListArray[i+1].duration)
       continue;
      else if (TimerListArray[i].duration == TimerListArray[i+1].duration)
      {
       if (TimerListArray[i].svcNum < TimerListArray[i+1].svcNum)
        continue;
       else
        break;
      }
      else
       break;
     }
     else
      break;
    }
   }

   tempbuff = TimerListArray[i];
   TimerListArray[i] = TimerListArray[i+1];
   TimerListArray[i+1] = tempbuff;

   tempnum = TimerNumArray[i];
   TimerNumArray[i] = TimerNumArray[i+1];
   TimerNumArray[i+1] = tempnum;

   ++nswaps;
  }
 } while (nswaps > 0);
}

void RefreshTimerListArray ()
{
 int i, totalTimerNum;
 char msg[128];

 totalTimerNum = TAP_Timer_GetTotalNum();
 if (totalTimerNum > MAX_TIMERS)
 {
  sprintf (msg, "There are %d timers. Increase array size from MAX_TIMERS.", totalTimerNum);
  ShowTimerErrorMsg (msg);
  totalTimerNum = MAX_TIMERS;
 }
 for (i = 0; i < totalTimerNum; i++)
 {
  TAP_Timer_GetInfo (i, &timerInfo);
  TimerListArray[i] = timerInfo;
  TimerNumArray[i] = i + 1;
 }
// need to sort after refresh, to preserve current sort order, unless it is unsorted
 if (TimerListSortKey != 2)
  SortTimerListArray ();
}

void ListTimers (bool bClearFirst)
{
 int result;
 int i;
 int totalTimerNum;
 int yoffset;
 word year;
 byte month, day, weekDay;
 char str[100];
 char str1[30];
 char endtime[5], daystr[4];
 int endmin;
 int len;
 dword ColorFore, ColorBack, dwtemp;
 int xoffendfilename;
 
 totalTimerNum = TAP_Timer_GetTotalNum();
 TimerListIndexMax = totalTimerNum - 1;
 TimerListIndexBottom = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TimerListIndexMax);

 strcpy (str, TimerFixedHeader);
 if (totalTimerNum == 0) strcat (str, " (No Entries)");
 else if (totalTimerNum == 1) strcat (str, " (1 Entry)");
 else
 {
  sprintf (str1, " (%d Entries)", totalTimerNum);
  strcat (str, str1);
 }
 strcat (str, " Sorted By ");
 switch (TimerListSortKey)
 {
  case 0: strcat (str, TimerListSortOption0); break;
  case 1: strcat (str, TimerListSortOption1); break;
  case 2: strcat (str, TimerListSortOption2); break;
 }
 TAP_Osd_FillBox (rgnTimer, 0, 0, TimerWinW, TimerHeadH, COLOR_BackTimerHead);
 TAP_Osd_PutS(rgnTimer, 0, 0, TimerWinW-1, str, COLOR_ForeTimerHead, COLOR_None, 0, FNT_Size_TimerHead, 0, ALIGN_CENTER);

 if (bClearFirst || totalTimerNum == 0)
 {
// clear out the timer list box before refresh
  TAP_Osd_FillBox (rgnTimer, 0, TimerHeadH, TimerWinW, TimerListEntriesPerScreen * TimerListYincrement, COLOR_BackTimerListNormal);
 }
 if (totalTimerNum == 0)
  return;
/*
// 29apr05 get info for all timers into array, then sort according to TimerListSortKey
 
 for (i = 0; i < totalTimerNum; i++)
 {
  TAP_Timer_GetInfo (i, &timerInfo);
  TimerListArray[i] = timerInfo;
  TimerNumArray[i] = i + 1;
 }
 SortTimerListArray ();
*/
 for(i = TimerListIndexTop; i <= TimerListIndexBottom; i++)
 {
  yoffset = TimerHeadH + ((i - TimerListIndexTop) * TimerListYincrement);
  {
   ColorFore = COLOR_ForeTimerListNormal;
   ColorBack = COLOR_BackTimerListNormal;
   if (i == TimerListIndex)
   {
// highlighted selected entry by swapping foreground and background colors
    dwtemp = ColorFore;
    ColorFore = ColorBack;
    ColorBack = dwtemp;
   }
   strcpy (str, " ");
   TAP_Osd_PutS(rgnTimer, 0, yoffset, 45, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);

//   TAP_Timer_GetInfo( i, &timerInfo );
   timerInfo = TimerListArray[i];

   sprintf (str, "%02d", i+1); // timer list index
//   sprintf (str, "%02d", TimerNumArray[i]); // timer number to tie up with any conflict error message on add
//   TAP_Osd_PutS(rgnTimer, 45, yoffset, 65, str, ColorFore, COLOR_None, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
   TAP_Osd_PutS(rgnTimer, 45, yoffset, 70, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);

   TAP_ExtractMjd(timerInfo.startTime>>16, &year, &month, &day, &weekDay);
   WeekdayToAlpha (weekDay, daystr);
   sprintf (str, "%s %02d/%02d", daystr, day, month);
//   TAP_Osd_PutS(rgnTimer, 70, yoffset, 160, str, ColorFore, COLOR_None, 0, FNT_Size_TimerList, 0, ALIGN_RIGHT);
   TAP_Osd_PutS(rgnTimer, 70, yoffset, 160, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_RIGHT);

   strcpy (str, " ");
   TAP_Osd_PutS(rgnTimer, 160, yoffset, 170, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);

   endmin = ((timerInfo.startTime & 0xff00) >> 8) * 60 + (timerInfo.startTime & 0xff) + timerInfo.duration;
   if (endmin >= 1440) endmin = endmin - 1440;
   sprintf (str, "%02d:%02d-%02d:%02d", (timerInfo.startTime&0xff00)>>8, (timerInfo.startTime&0xff), endmin/60, endmin%60);
//   TAP_Osd_PutS(rgnTimer, 170, yoffset, 260, str, ColorFore, COLOR_None, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
   TAP_Osd_PutS(rgnTimer, 170, yoffset, 265, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);

   sprintf (str, "(%d:%02d)", timerInfo.duration/60, timerInfo.duration%60);
//   TAP_Osd_PutS(rgnTimer, 265, yoffset, 310, str, ColorFore, COLOR_None, 0, FNT_Size_TimerList, 0, ALIGN_RIGHT);
   TAP_Osd_PutS(rgnTimer, 265, yoffset, 310, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_RIGHT);

   strcpy (str, " ");
//   TAP_Osd_PutS(rgnTimer, 310, yoffset, 320, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
   TAP_Osd_PutS(rgnTimer, 310, yoffset, 320, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);

   sprintf (str, "C%d", SvcNumToLcn (timerInfo.svcNum));
//   TAP_Osd_PutS(rgnTimer, 320, yoffset, 345, str, ColorFore, COLOR_None, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
   TAP_Osd_PutS(rgnTimer, 320, yoffset, 360, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);

   sprintf (str, "T%d", timerInfo.tuner + 1);
//   TAP_Osd_PutS(rgnTimer, 350, yoffset, 375, str, ColorFore, COLOR_None, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
//   TAP_Osd_PutS(rgnTimer, 350, yoffset, 380, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
   TAP_Osd_PutS(rgnTimer, 360, yoffset, 390, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);

   ReservationTypeToString (timerInfo.reservationType, str);
//   TAP_Osd_PutS(rgnTimer, 380, yoffset, 410, str, ColorFore, COLOR_None, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
//   TAP_Osd_PutS(rgnTimer, 380, yoffset, 420, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
   TAP_Osd_PutS(rgnTimer, 390, yoffset, 430, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);

   if (timerInfo.nameFix == 0)
    strcpy (str, " ");
   else
    strcpy (str, "F");
//   TAP_Osd_PutS(rgnTimer, 420, yoffset, 435, str, ColorFore, COLOR_None, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
//   TAP_Osd_PutS(rgnTimer, 420, yoffset, 440, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
   TAP_Osd_PutS(rgnTimer, 430, yoffset, 450, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);

// remove the ".rec" from the end of the filename
// i can't find a substring function, so stick a string delimeter in the 4th last char to get rid of ".rec"
   len = strlen(timerInfo.fileName);
   if (len > 4)
    timerInfo.fileName[len-4] = '\0';
   sprintf (str, "%s", timerInfo.fileName);
// file name can be null. if so, make it blank, else background color doesn't display
   if (strcmp (str, "") == 0)
    strcpy (str, " ");

//   TAP_Osd_PutS(rgnTimer, 440, yoffset, 690, str, ColorFore, COLOR_None, 0, FNT_Size_TimerList, 1, ALIGN_LEFT);
   if (TimerListSortKey == 2)
    xoffendfilename = 685;
   else
    xoffendfilename = 660;
//   TAP_Osd_PutS(rgnTimer, 440, yoffset, xoffendfilename, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 1, ALIGN_LEFT);
   TAP_Osd_PutS(rgnTimer, 450, yoffset, xoffendfilename, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 1, ALIGN_LEFT);

   if (TimerListSortKey != 2)
   {
    sprintf (str, "%02d", TimerNumArray[i]); // timer number to tie up with any conflict error message on add
    TAP_Osd_PutS(rgnTimer, 660, yoffset, 685, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 1, ALIGN_LEFT);
   }

   strcpy (str, " ");
   TAP_Osd_PutS(rgnTimer, 685, yoffset, 719, str, ColorFore, ColorBack, 0, FNT_Size_TimerList, 0, ALIGN_LEFT);
  }
 }
}

void ShowTimerErrorMsg (char *msg)
{
/*
 int width;

 rgnError = TAP_Osd_Create(0, 0, 720, 576, 0, FALSE); // Define a region that covers the whole screen
 width = TAP_Osd_GetW(msg, 0, FNT_Size_1926);
 TAP_Osd_FillBox(rgnError,((720-width)/2)-50,(576/2)-25,width+100,50,COLOR_DarkGray);
 TAP_Osd_PutS(rgnError, (720-width)/2, (576/2)-14, -1, msg, COLOR_White, COLOR_DarkGray, 0, FNT_Size_1926, FALSE, ALIGN_LEFT);
// TAP_Osd_FillBox(rgnError,0,0,1,1,COLOR_None); // This seems to be necessary to prevent the box crashing. Why? I don't know...
*/
 TAP_Osd_FillBox(rgnTimer, 0, TimerErrorYoffset, TimerWinW, TimerErrorH, COLOR_BackTimerError);
// width = TAP_Osd_GetW (msg, 0, FNT_Size_TimerError);
 TAP_Osd_PutS(rgnTimer, 0, TimerErrorYoffset, TimerWinW-1, msg, COLOR_ForeTimerError, COLOR_None, 0, FNT_Size_TimerError, 0, ALIGN_CENTER);
 bErrorMsgShown = TRUE;
// save msg so we can re-instate when switching height
 strcpy (ErrorMsgSaved, msg);
}

void HideTimerErrorMsg ()
{
/*
 TAP_Osd_Delete (rgnError);
*/
// ListTimers (TRUE); // refresh timer list to overwrite error message
// TAP_Osd_FillBox(rgnTimer, 0, TimerErrorYoffset, TimerWinW, TimerErrorH, COLOR_BackTimerError);
// clock is now present when no error message, so set appropriate background color
 TAP_Osd_FillBox(rgnTimer, 0, TimerErrorYoffset, TimerWinW, TimerErrorH, COLOR_BackClock);
 bErrorMsgShown = FALSE;
}

/* Simple bubble sort */
/*
void SortTimers ()
{
// int nswaps,i,j;
 int nswaps,i;
 int total_timers;
 TYPE_TimerRAM  tempbuff;

 //tempbuff=TAP_MemAlloc(EPG_RECORD_LENGTH);

 total_timers = TAP_Timer_GetTotalNum();
 do {
  for (nswaps = i = 0 ; i < total_timers - 1 ; i++)
   if (g_timerPointer[i].timer_start > (g_timerPointer+i+1)->timer_start) {
    tempbuff=*(g_timerPointer+i);  //fill buffer with timer
    *(g_timerPointer+i)=*(g_timerPointer+i+1);
    *(g_timerPointer+i+1)=tempbuff;
    ++nswaps;
   }
 } while (nswaps > 0);

 //TAP_MemFree( tempbuff );
}
*/

void ShowTimerReservationType ()
{
 char str[3];
 int restypexoff, restypew;

 ReservationTypeToString (TimerReservationType, str);
 restypexoff = TimerEntryDataXoffset[0];
 restypew = TimerEntryDataW[0];
 TAP_Osd_FillBox (rgnTimer, restypexoff, TimerEntryYoffset, restypew, TimerEntryH, COLOR_BackTimerEntry);
 TAP_Osd_PutS(rgnTimer, restypexoff, TimerEntryYoffset, restypexoff + restypew, str, COLOR_ForeTimerEntryVble, COLOR_None, 0, FNT_Size_TimerEntry, 0, ALIGN_LEFT);
}

void ShowTimerDate (void)
{
 char str[10];
// word year;
// byte month, day, weekDay;
 byte weekDay;
 int datexoff, datew;

 if (TAP_ExtractMjd (TimerDate, &TimerYear, &TimerMonth, &TimerDay, &weekDay))
 {
  WeekdayToAlpha (weekDay, TimerWeekDay);
  sprintf (str, "%s %02d/%02d", TimerWeekDay, TimerDay, TimerMonth);
 }
 else
  strcpy (str, "          ");

 datexoff = TimerEntryDataXoffset[1];
 datew = TimerEntryDataW[1];

// TAP_Osd_FillBox (rgnTimer, TimerDateXoffset, TimerEntryYoffset, TimerEntryXoffset[0] - 10 - TimerDateXoffset, TimerEntryH, COLOR_BackTimerEntry);
// TAP_Osd_PutS(rgnTimer, TimerDateXoffset, TimerEntryYoffset, TimerEntryXoffset[0] - 10, str, COLOR_ForeTimerEntryVble, COLOR_None, 0, FNT_Size_TimerEntry, 0, ALIGN_RIGHT);
 TAP_Osd_FillBox (rgnTimer, datexoff, TimerEntryYoffset, datew, TimerEntryH, COLOR_BackTimerEntry);
 TAP_Osd_PutS(rgnTimer, datexoff, TimerEntryYoffset, datexoff + datew - 1, str, COLOR_ForeTimerEntryVble, COLOR_None, 0, FNT_Size_TimerEntry, 0, ALIGN_LEFT);
}

void SetTimerDateToCurrent (void)
{

 byte hour, min, sec;
 TAP_GetTime (&TimerDate, &hour, &min, &sec);
}

void PutFieldEntry (void)
{
// int y;
// int x;
// y = EntryWinFieldYoffset + (index * EntryWinFieldYincrement); 
// TAP_Osd_PutStringAf1622(rgnTimer, EntryWinFieldXoffset, y, EntryWinW-20, Field[index], COLOR_EntryFieldVble, COLOR_None);
// x =  TimerEntryXoffset[TimerFieldIndex] + TAP_Osd_GetW (TimerEntryFieldName[TimerFieldIndex], 0, FNT_Size_TimerEntry) + 20;
 TAP_Osd_FillBox (rgnTimer, TimerEntryDataXoffset[TimerFieldIndex], TimerEntryYoffset, TimerEntryDataW[TimerFieldIndex], TimerEntryH, COLOR_BackTimerEntry);
 TAP_Osd_PutS(rgnTimer, TimerEntryDataXoffset[TimerFieldIndex], TimerEntryYoffset, TimerWinW-1, TimerEntryFieldData[TimerFieldIndex], COLOR_ForeTimerEntryVble, COLOR_None, 0, FNT_Size_TimerEntry, 0, ALIGN_LEFT);
// TAP_Osd_PutS(rgnTimer, x, TimerEntryYoffset, TimerWinW, TimerEntryFieldData[TimerFieldIndex], COLOR_ForeTimerEntryVble, COLOR_None, 0, FNT_Size_TimerEntry, 0, ALIGN_LEFT);
}


void DrawNameEntryMarker (dword color)
{
 char str[NAME_ENTRY_BUFFER_SIZE];

 int width, i, x, yoff;
 
 for (i = 0; i < NameEntryBufferIndex; i++)
  str[i] = NameEntryBuffer[i];
 str[NameEntryBufferIndex] = '\0';
 width = TAP_Osd_GetW (str, 0, FNT_Size_NameEntry);
 x = NameEntryPrefixXoffset + TAP_Osd_GetW (FileNameEntryPrefix, 0, FNT_Size_NameEntry) + width;
// if (rgnNameEntry == rgnTimer)
 if (bTimerMode)
  yoff = TimerEntryFileNameYoffset;
// else if (rgnNameEntry == rgnFile)
 else if (bFileMode)
  yoff = FileNameEntryYoffset;
 else
  return;
 if (bNameEntryInsertMode)
  width = 1;
 else
 {
  if (NameEntryBufferIndexMax == -1 || NameEntryBufferIndex > NameEntryBufferIndexMax)
   width = 16;
  else
  {
   str[0] = NameEntryBuffer[NameEntryBufferIndex];
   str[1] = '\0';

   width = TAP_Osd_GetW (str, 0, FNT_Size_NameEntry);
  }
 }
 TAP_Osd_DrawRectangle (rgnNameEntry, x, yoff, width, 22, 1, color);

 if (bNameEntryInsertMode)
 {
// draw short horizontal cross pieces on top and bottom of vertical insertion marker
// DrawRectangle doesn't seem to like 1 pixel high, although 1 pixel wide is OK
  TAP_Osd_DrawRectangle (rgnNameEntry, x-2, yoff,    5, 2, 1, color);
  TAP_Osd_DrawRectangle (rgnNameEntry, x-2, yoff+22, 5, 2, 1, color);
 }

}

void ShowNameEntryPos ()
{
 dword color;
 char msg[50];

// if (bNameEntryLowerCase)
 color = NameEntryMarkerColor[NameEntryCase];
 DrawNameEntryMarker (color);
/*
// debug code for indexes
 sprintf (msg, "Index = %d, IndexMax = %d", NameEntryBufferIndex, NameEntryBufferIndexMax);
 ShowTimerErrorMsg (msg);
*/
}

void HideNameEntryPos ()
{
 DrawNameEntryMarker (COLOR_BackNameEntry);
}

void ShowNameEntryPrefix ()
{
 int yoff, width, height;

 dword color;

// if (rgnNameEntry == rgnTimer)
 if (bTimerMode)
 {
  yoff = TimerEntryFileNameYoffset;
  width = TimerWinW;
  height = TimerEntryH;

  if (TimerFieldIndex == NumTimerEntryFields)
   color = COLOR_ForeNameEntryFixedNext;
  else
   color = COLOR_ForeNameEntryFixed;
 }
// else if (rgnNameEntry == rgnFile)
 else if (bFileMode)
 {
  yoff = FileNameEntryYoffset;
  width = FileWinW;

  height = FileErrorH;
  color = COLOR_ForeNameEntryFixed;
 }
 else

  return;
 TAP_Osd_FillBox(rgnNameEntry, 0, yoff, width, height, COLOR_BackNameEntry);
 TAP_Osd_PutS(rgnNameEntry, NameEntryPrefixXoffset, yoff, width, FileNameEntryPrefix, color, COLOR_None, 0, FNT_Size_NameEntry, 0, ALIGN_LEFT);
}

void ShowNameEntry (char *name)
{
// int xoff, yoff, width, height;
 int xoff, yoff, xMax;

// if (rgnNameEntry == rgnTimer)
 if (bTimerMode)
 {
  yoff = TimerEntryFileNameYoffset;
  xMax = TimerWinXMarR;
//  width = TimerWinW;
//  height = TimerEntryH;
 }
// else if (rgnNameEntry == rgnFile)
 else if (bFileMode)
 {
//  yoff = FileErrorYoffset;
  yoff = FileNameEntryYoffset;
  xMax = FileWinXMarR;
//  width = FileWinW;
//  height = FileErrorH;
 }
 else
  return;

// TAP_Osd_FillBox(rgnNameEntry, 0, yoff, width, height, COLOR_BackNameEntry);
// TAP_Osd_PutS(rgnNameEntry, NameEntryPrefixXoffset, yoff, TimerWinW, msg, COLOR_ForeNameEntryVble, COLOR_None, 0, FNT_Size_NameEntry, 0, ALIGN_LEFT);
 ShowNameEntryPrefix ();
 xoff = NameEntryPrefixXoffset + TAP_Osd_GetW (FileNameEntryPrefix, 0, FNT_Size_NameEntry);
 TAP_Osd_PutS(rgnNameEntry, xoff, yoff, xMax, name, COLOR_ForeNameEntryVble, COLOR_None, 0, FNT_Size_NameEntry, 0, ALIGN_LEFT);
}

void HideNameEntry ()
{
 int yoff, width, height;
 dword color;

// if (rgnNameEntry == rgnTimer)
 if (bTimerMode)
 {
  yoff = TimerEntryFileNameYoffset;
  width = TimerWinW;
  height = TimerEntryH;
  color = COLOR_BackNameEntry;
 }
// else if (rgnNameEntry == rgnFile)
 else if (bFileMode)
 {
  yoff = FileNameEntryYoffset;
  width = FileWinW;
  height = FileErrorH;
  color = COLOR_BackFileListNormal;
 }
 else
  return;
 TAP_Osd_FillBox(rgnNameEntry, 0, yoff, width, height, color);
}

void ShowNameEntryHelp ()
{
// display help for name entry keys in error message line
 char msg[255], keystr[3];
 int i, yoff;

 msg[0] = '\0';
 for (i = 0; i <=9; i++)
 {
  sprintf (keystr, " %d:", i);
  strcat (msg, keystr);
  strcat (msg, KeyCharArray[i]);
 }
// if (rgnNameEntry == rgnTimer)
 if (bTimerMode)
 {
  TAP_Osd_FillBox(rgnTimer, 0, TimerNameEntryHelpYoffset, TimerWinW, NameEntryH, COLOR_BackNameEntryHelp);
  TAP_Osd_PutS(rgnTimer, 0, TimerNameEntryHelpYoffset, TimerWinW-1, msg, COLOR_ForeNameEntryHelp, COLOR_None, 0, FNT_Size_TimerError, 0, ALIGN_CENTER);
 }
// else if (rgnNameEntry == rgnFile)
 else if (bFileMode)
 {
  TAP_Osd_FillBox(rgnFile, 0, FileNameEntryHelpYoffset, FileWinW, NameEntryH, COLOR_BackNameEntryHelp);
  TAP_Osd_PutS(rgnFile, 0, FileNameEntryHelpYoffset, FileWinW-1, msg, COLOR_ForeNameEntryHelp, COLOR_None, 0, FNT_Size_TimerError, 0, ALIGN_CENTER);
 }
 bNameEntryHelpShown = TRUE;
}

void HideNameEntryHelp ()
{
    char msg[64];
// this can actually be called if name entry help is not shown (e.g. by ShowEntryInsertionField when timer
// list is shown, so check flag
 if (bNameEntryHelpShown)
 {
//     if (rgnNameEntry == rgnTimer)
     if (bTimerMode)
     {
//  TAP_Osd_FillBox(rgnTimer, 0, TimerErrorYoffset, TimerWinW, TimerErrorH, COLOR_BackTimerError);
//    TAP_Osd_FillBox(rgnTimer, 0, TimerNameEntryHelpYoffset, TimerWinW, NameEntryH, COLOR_BackTimerListNormal);
/* will now get replaced by clock in next idle, so don't need to do anything here
        TAP_Osd_FillBox(rgnTimer, 0, TimerNameEntryHelpYoffset, TimerWinW, NameEntryH, COLOR_BackTimerEntry);
/*
// if there was a timer entry on the last line of the listing, it will be clobbered by the above,
// so re-list the timers in this case.
    if (TimerListIndexBottom % (TimerListEntriesPerScreen - 1) == 0)
        ListTimers (FALSE);
*/
/*
// timer name entry help overwrites first line of timer entry, so re-instate this
        TimerFieldIndexSave = TimerFieldIndex;
        for (TimerFieldIndex = 0; TimerFieldIndex < NumTimerEntryFields; TimerFieldIndex++)
        {
            TAP_Osd_PutS(rgnTimer, TimerEntryXoffset[TimerFieldIndex], TimerEntryYoffset, TimerWinW-1, TimerEntryFieldName[TimerFieldIndex], COLOR_ForeTimerEntryFixed, COLOR_None, 0, FNT_Size_TimerEntry, 0, ALIGN_LEFT);
            if (TimerFieldIndex == 0)
                ShowTimerReservationType ();
            else if (TimerFieldIndex == 1)
                ShowTimerDate ();
            else
                PutFieldEntry ();
        }
*/
/*
        TimerFieldIndex = TimerFieldIndexSave;
        ShowEntryInsertionField ();
*/
// if there was a timer modify or delete in progress when we moved into the name entry field, re-instate the message,
// since it got clobbered by the name entry help.
/* no longer necessary, since saved msg should be reshown when show timer list
        if (bTimerModifyInProgress)
        {
           sprintf (msg, "Modify in progress for timer entry %d", TimerModifyEntryIndex + 1);
           ShowTimerErrorMsg (msg);
        }
        else if (bTimerDeleteInProgress)
        {
          sprintf (msg, "Press OK key to confirm deletion of entry %02d", TimerNumArray[TimerListIndex]);
          ShowTimerErrorMsg (msg);
        }
        else
*/
// clock is now shown when name entry help goes in file or timer mode, so put up the clock background
          TAP_Osd_FillBox(rgnTimer, 0, TimerNameEntryHelpYoffset, TimerWinW, ClockH, COLOR_BackClock);
     }
//     else if (rgnNameEntry == rgnFile)
     else if (bFileMode)
     {
//  TAP_Osd_FillBox(rgnFile, 0, FileErrorYoffset, FileWinW, FileErrorH, COLOR_BackFileError);
//        TAP_Osd_FillBox(rgnFile, 0, FileNameEntryHelpYoffset, FileWinW, NameEntryH, COLOR_BackFileListNormal);
// clock is now shown when name entry help goes in file or timer mode, so put up the clock background
      TAP_Osd_FillBox(rgnFile, 0, FileNameEntryHelpYoffset, FileWinW, ClockH, COLOR_BackClock);
     }
     bNameEntryHelpShown = FALSE;
 }  // end name entry help shown
}

void ShowEntryInsertionField ()
{
// want to show new insertion point.
 int xoff, yoff;
 char str[20], msg[64];
 dword color;
 byte fntsize;

 if (TimerFieldIndex < NumTimerEntryFields)
 {
  xoff = TimerEntryXoffset[TimerFieldIndex];
  yoff = TimerEntryYoffset;
  strcpy (str, TimerEntryFieldName[TimerFieldIndex]);
  color = COLOR_ForeTimerEntryFixedNext;
  fntsize = FNT_Size_TimerEntry;
 }
 else
 {
  xoff = NameEntryPrefixXoffset;
  yoff = TimerEntryFileNameYoffset;
  strcpy (str, FileNameEntryPrefix); 
  color = COLOR_ForeNameEntryFixedNext;
  fntsize = FNT_Size_NameEntry;
 }
// TAP_Osd_PutS(rgnTimer, TimerEntryXoffset[TimerFieldIndex], TimerEntryYoffset, TimerWinW, TimerEntryFieldName[TimerFieldIndex], COLOR_ForeTimerEntryFixedNext, COLOR_None, 0, FNT_Size_TimerEntry, 0, ALIGN_LEFT);
//  sprintf (msg, "ShowEntryInsertionField y offset = %d", yoff);
//  ShowTimerErrorMsg (msg);
  TAP_Osd_PutS(rgnTimer, xoff, yoff, TimerWinW-1, str, color, COLOR_None, 0, fntsize, 0, ALIGN_LEFT);

// if we are moving to file name field, set up for entry of the name

 if (TimerFieldIndex == NumTimerEntryFields)
 {
  rgnNameEntry = rgnTimer;
  bNameEntryMode = TRUE;
  bNameEntryInsertMode = TRUE;
  NameEntryKeyPressCount = 0;
  bNameEntryKeyCheckMax = TRUE;
//  bNameEntryKeyCheckPrev = TRUE;
//  bNameEntryLowerCase = FALSE;
//  bNameEntryWordCase = TRUE;
  NameEntryBufferIndex = 0;
//     NameEntryBufferIndexMax = 0;
  NameEntryPrevKeyCode = 0;
  strcpy (FileNameEntryPrefix, "File Name: ");
// if there is nothing in the file name field, put an apostrophe in the first position as a default convention
// so that when we see this in a recorded file name, we know it probably came from a timer entry.
//  if (strcmp (NameEntryBuffer, "") == 0)
  if (NameEntryBufferIndexMax == -1)
  {
//   strcpy (NameEntryBuffer, "'");
   NameEntryBuffer[0] = PrefixFileNameTimer;
   NameEntryBuffer[1] = '\0';
   NameEntryBufferIndex = 1;
   NameEntryBufferIndexMax = 0;
  }
  ShowNameEntry (NameEntryBuffer);
/*
  if (!bTimerModifyInProgress)
  {
//   strcpy (NameEntryBuffer, " ");
//   ShowNameEntry (FileNameEntryPrefix);
//   ShowNameEntryPrefix ();
//      NameEntryBufferIndexMax = 0;
//   NameEntryBufferIndexMax = -1;
  }
*/
  ShowNameEntryPos ();
  ShowNameEntryHelp ();
 }
 else
 {
  HideNameEntryPos ();
  HideNameEntryHelp ();
  bNameEntryMode = FALSE;
 }
}

void HideEntryInsertionField ()
{
// want to clear any indication of current insertion point
 int xoff, yoff;
 char str[20];
 dword color;
 byte fntsize;

 if (TimerFieldIndex < NumTimerEntryFields)
 {
  xoff = TimerEntryXoffset[TimerFieldIndex];
  yoff = TimerEntryYoffset;
  strcpy (str, TimerEntryFieldName[TimerFieldIndex]);
  color = COLOR_ForeTimerEntryFixed;
  fntsize = FNT_Size_TimerEntry;
 }
 else
 {
  xoff = NameEntryPrefixXoffset;
  yoff = TimerEntryFileNameYoffset;
  strcpy (str, FileNameEntryPrefix); 
  color = COLOR_ForeNameEntryFixed;
  fntsize = FNT_Size_NameEntry;
 }
// TAP_Osd_PutS(rgnTimer, TimerEntryXoffset[TimerFieldIndex], TimerEntryYoffset, TimerWinW, TimerEntryFieldName[TimerFieldIndex], COLOR_ForeTimerEntryFixed, COLOR_None, 0, FNT_Size_TimerEntry, 0, ALIGN_LEFT);
 TAP_Osd_PutS(rgnTimer, xoff, yoff, TimerWinW-1, str, color, COLOR_None, 0, fntsize, 0, ALIGN_LEFT);

// if we are moving out of file name field, set flag to go back to normal timer entry mode
 if (TimerFieldIndex == NumTimerEntryFields)
  bNameEntryMode = FALSE;
}

void CheckTimerDateIncrement (void)
{
 word mjd;
 byte hour, min, sec;

 if (TimerFieldIndex == 2)
 {
  if (isTimerEntryFieldValid ())
  {
// if we are moving from start time field and time is less than now, bump display to next day
// 08apr05 this should only be done if timer date is today (i.e. bump date to tomorrow)

   TAP_GetTime (&mjd, &hour, &min, &sec);
   if (TimerDate == mjd)
   {
    if (TimerStartMinutes <= (hour * 60 + min))
    {
     TimerDate = mjd + 1;
     ShowTimerDate ();
    }
   }

  }
 }
}

void MoveToNextEntryField (void)
{
// (TimerFieldIndex += 1) % NumTimerEntryFields;
 CheckTimerDateIncrement ();
 TimerFieldIndex += 1;
// if (TimerFieldIndex >= NumTimerEntryFields)
//  TimerFieldIndex = 0;
 if (TimerFieldIndex >= NumTimerEntryFields + 1)  // special case of file name on second line
  TimerFieldIndex = 0;
 TimerCharIndex = 0;
 ShowEntryInsertionField ();
}

void MoveToPrevEntryField (void)
{
// (TimerFieldIndex -= 1) %NumTimerEntryFields;
 CheckTimerDateIncrement ();
 TimerFieldIndex -= 1;
 if (TimerFieldIndex < 0)
//  TimerFieldIndex = NumTimerEntryFields - 1;
  TimerFieldIndex = NumTimerEntryFields;  // special case of file name on second line
 TimerCharIndex = 0;
 ShowEntryInsertionField ();
}

void ClearTimerEntryField (void)
{
 int i, j;
 j = TimerEntryFieldLen[TimerFieldIndex];
 for (i = 0; i < j; i++)
 {
  TimerEntryFieldData[TimerFieldIndex][i] = 0x20;
 }
 TimerEntryFieldData[TimerFieldIndex][j] = '\0';
// PutFieldEntry ();
 TAP_Osd_FillBox (rgnTimer, TimerEntryDataXoffset[TimerFieldIndex], TimerEntryYoffset, TimerEntryDataW[TimerFieldIndex], TimerEntryH, COLOR_BackTimerEntry);

}

void ClearNameEntryField ()
{
 strcpy (NameEntryBuffer, "");
 NameEntryBufferIndexMax = -1;
 NameEntryBufferIndex = 0;
// ShowNameEntry (FileNameEntryPrefix);
 strcpy (FileNameEntryPrefix, "File Name: ");
 ShowNameEntryPrefix ();
// ShowNameEntryPos ();

 if (bTimerMode)
 {
// default timer file name to special prefix
/*
     NameEntryBuffer[0] = PrefixFileNameTimer;
     NameEntryBuffer[1] = '\0';
*/
     strcpy (NameEntryBuffer, "'");
     NameEntryBufferIndex = 1;
     NameEntryBufferIndexMax = 0;
     ShowNameEntry (NameEntryBuffer);
 }
}

bool isTimerEntryFieldValid (void)
{
 int i, n;
 byte hr, min;

 if (TimerFieldIndex == 2 || TimerFieldIndex == 3)
 {
  // start or end time (hhmm)
  // convert 4 characters to integer equivalent
  hr = 0; min = 0;

  for (i = 0; i < 4; i++)
  {
// 05jun05 field must contain all numeric digits. any blanks (left after clearing) invalidate field.
   if (TimerEntryFieldData[TimerFieldIndex][i] == 0x20)
    return FALSE;
   if (i < 2) hr  = (hr  * 10) + (TimerEntryFieldData[TimerFieldIndex][i] - 0x30);
   else  min = (min * 10) + (TimerEntryFieldData[TimerFieldIndex][i] - 0x30);
  }
  if (hr <= 23 && min <= 59)
  {
   if (TimerFieldIndex == 2)
   {
    strcpy (TimerStart, TimerEntryFieldData[TimerFieldIndex]);
    TimerStartHr = hr;
    TimerStartMin = min;
    TimerStartMinutes = hr * 60 + min;
   }
   else
    strcpy (TimerEnd, TimerEntryFieldData[TimerFieldIndex]);
    TimerEndMinutes = hr * 60 + min;
   return TRUE;
  }
  else
   return FALSE;
 }
 else if (TimerFieldIndex == 4)
 {
  // channel (LCN)
/*
  n = TimerEntryFieldData[TimerFieldIndex][0] - 0x30; // convert ascii character to binary integer
  switch (n)
  {
   case 1: break;
   case 2: break;
   case 3: break;
   case 7: break;
   case 9: break;
   default: return FALSE;
  }
*/
// 03jun05 allow more than 1 digit for lcn
  n = 0;
  for (i = 0; i < TimerEntryFieldLen[TimerFieldIndex]; i++)
  {
    if (TimerEntryFieldData[TimerFieldIndex][i] == '\0')
        break;
    else if (TimerEntryFieldData[TimerFieldIndex][i] != 0x20)   // ignore blanks
        n = (n * 10) + (TimerEntryFieldData[TimerFieldIndex][i] - 0x30); // convert ascii character to binary integer
  }
  i = LcnToSvcNum (n);
  if (i >= 0)
  {
      TimerSvcNum = i;
      return TRUE;
  }
  else
      return FALSE;
 }
 else if (TimerFieldIndex == 5)
 {

  // tuner 

//  if (TimerEntryFieldData[TimerFieldIndex][0] == '\0') // if tuner omitted, assume 1 (i.e. internal 0)
  if (TimerEntryFieldData[TimerFieldIndex][0] == 0x20) // if tuner omitted, assume 4 (i.e. internal 3)
  {
   TimerTuner = 3;
//   HideEntryInsertionField ();
//   MoveToNextEntryField ();
  }
  else

  {
   n = TimerEntryFieldData[TimerFieldIndex][0] - 0x30; // convert ascii character to binary integer
   switch (n)
   {
    case 1: break;
    case 2: break;
    case 4: break; // new value for firmware which removes concept of particular tuner
    default: return FALSE;
   }
   TimerTuner = n - 1;
  }
  return TRUE;
 }
/*
 else if (TimerFieldIndex == 6)
 {
// last field is for file name. 0 = not fixed, 1 = fixed using my convention, 2 = prompt for name (and fix). default 0
//  if (TimerEntryFieldData[TimerFieldIndex][0] == '\0') // if omitted, assume 0 (name not fixed)
  if (TimerEntryFieldData[TimerFieldIndex][0] == 0x20) // if omitted, assume 0 (name not fixed)
  {

   TimerFilenameFixed = 0;
  }
  else
  {
   n = TimerEntryFieldData[TimerFieldIndex][0] - 0x30; // convert ascii character to binary integer
   switch (n)
   {
    case 0: break;
    case 1: break;

    case 2: break;

    default: return FALSE;
   }
   TimerFilenameFixed = n;
  }
  return TRUE;
 }
*/
 else
 {
  return FALSE; // if any new fields addded, need to add validation here
 }
}

void ShowTimerList (bool bSwitchHeight)
{
// char FieldStr[30];
 char strtemp[9];
 dword dwtemp;
 int i;
 char str[10];
 word mjd,year;
 byte hour,min,sec,month,day,weekDay;
 int BufferIndexSave;
// int x, y, w, h;

 if (!bSwitchHeight)
 {
     bTimerDeleteInProgress = FALSE;
     bTimerModifyInProgress = FALSE;
     TimerDateTest = TimerDate;
// if (bClockShown)
     if (rgnClock)
     {
      TAP_Osd_Delete (rgnClock); // we will now show clock in timer region
      TAP_Osd_Delete (rgnPlayRec);
     }
// 28apr05 exit normal regardless of clock since clock does not exit normal any more
// else
     TAP_ExitNormal();      // leaving normal receiver operation, receiver hides its OSDs

     bTimerMode = TRUE;
 }
// set timerlist height
 if (bTimerModeHeightFull)
 {
  TimerWinH = 528; // gives 18 entries per screen
 }
 else
 {
//  TimerWinH = 312; // gives 9 entries per screen
  TimerWinH = 288; // 05jun05 gives 8 entries per screen (so that it matches file list half height)
// if already in PIP or POP, show two windows below file list, else one
  if (bPIPshown || (iPOPshown != 0))
   ScalePOP ();
  else
  {
//   TAP_Channel_Scale (CHANNEL_Main, 168, 324, 384, 216, TRUE);
/*
   y = TimerWinY + TimerWinH;
   h = ymax - y;
//   w = h * 16 / 9;
   w = h * xmax / ymax;
   x = (xmax - w) / 2;
   TAP_Channel_Scale (CHANNEL_Main, x, y, w, h, TRUE);
*/
   ScaleMainHalfHeight (TimerWinY + TimerWinH);
  }
 }
 SetTimerListParameters ();
// if we are changing to full height make better use of the extra space so we can see as many entries
// as possible at once.
 if (bSwitchHeight && bTimerModeHeightFull)
 {
   if (TimerListIndexTop > 0 && TimerListIndexMax < TimerListEntriesPerScreen)
        TimerListIndexTop = 0;
 }

 rgnTimer = TAP_Osd_Create (TimerWinX, TimerWinY, TimerWinW, TimerWinH, 0, FALSE);

// strcpy (str, "          ");
/*
 TAP_Osd_FillBox (rgnTimer, 0, TimerEntryYoffset, TimerWinW, TimerEntryH, COLOR_BackTimerEntry);
 TAP_Osd_FillBox (rgnTimer, 0, TimerEntryFileNameYoffset, TimerWinW, TimerEntryH, COLOR_BackTimerEntry);
*/
 TAP_Osd_FillBox (rgnTimer, 0, TimerEntryYoffset, TimerWinW, 2 * TimerEntryH, COLOR_BackTimerEntry);
 if (!bSwitchHeight)
 {
  TimerReservationType = TimerReservationTypeDefault;
  SetTimerDateToCurrent ();
 }
 ShowTimerReservationType ();
 ShowTimerDate ();

 TimerFieldIndexSave = TimerFieldIndex;
 BufferIndexSave = NameEntryBufferIndex;
 for (i=0; i < NumTimerEntryFields; ++i)
 {
  TAP_Osd_PutS(rgnTimer, TimerEntryXoffset[i], TimerEntryYoffset, TimerWinW-1, TimerEntryFieldName[i], COLOR_ForeTimerEntryFixed, COLOR_None, 0, FNT_Size_TimerEntry, 0, ALIGN_LEFT);
  if (i >= TimerFieldIndexStart)
  {
   TimerFieldIndex = i;
   if (bSwitchHeight)
    PutFieldEntry ();
   else
    ClearTimerEntryField ();
  }
 }
 
 if (!bSwitchHeight)
 {
     rgnNameEntry = rgnTimer;
     ClearNameEntryField ();
/*
// default file name to special prefix
     NameEntryBuffer[0] = PrefixFileNameTimer;
     NameEntryBuffer[1] = '\0';
     NameEntryBufferIndex = 1;
     NameEntryBufferIndexMax = 0;
     ShowNameEntry (NameEntryBuffer);
*/
     TimerFieldIndex = TimerFieldIndexStart;
//     ShowEntryInsertionField ();
// list (first) timer entries
     RefreshTimerListArray ();
     TimerListIndex = 0;
     TimerListIndexTop = 0;
     ShowEntryInsertionField ();
 }
 else
 {
//    TimerFieldIndex = NumTimerEntryFields;  // force ShowEntryInsertionField to display file name field
// }
// ShowEntryInsertionField ();
// if (bSwitchHeight)
// {
// now force ShowEntryInsertionField to show saved field
//    HideEntryInsertionField ();

// ensure current timer index is shown after switch height
// and put it at bottom rather than top
    if (TimerListIndex + 1 > TimerListEntriesPerScreen)
//        FileListIndexTop = FileListIndex;
        TimerListIndexTop = max (TimerListIndex - TimerListEntriesPerScreen + 1, 0);

    ShowNameEntry (NameEntryBuffer);
    TimerFieldIndex = TimerFieldIndexSave;
    ShowEntryInsertionField ();
// also the saved position in the name entry buffer, but only if the name entry field is current,
// since the position should not be shown otherwise
//    if (TimerFieldIndex == NumTimerEntryFields)
    if (bNameEntryMode)
    {
        HideNameEntryPos ();
        NameEntryBufferIndex = BufferIndexSave;
        ShowNameEntryPos ();   // re-display saved name entry position after ShowEntryInsertionField zeroes it.
        ShowNameEntryHelp ();
    }
// reinstate any message which was outstandng when we switched height
    else if (bErrorMsgShown)
        ShowTimerErrorMsg (ErrorMsgSaved);
 }
// system may have deleted a timer entry because it has started recording, so TimerListIndexMax may be out of date
// hence, refresh it
// TimerListIndexBottom = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TimerListIndexMax);
// TimerListIndexBottom = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TAP_Timer_GetTotalNum() - 1);
 ListTimers (TRUE);
// bTimerMode = TRUE;
 TickClockSaved = 0; // force clock update next time in idle
 bClockShown = TRUE;
}

void HideTimerList (bool bSwitchHeight)
{
 TAP_Osd_Delete (rgnTimer);
 if (!bSwitchHeight)
 {
     TAP_EnterNormal();      // re-entering normal receiver operation
     bTimerMode = FALSE;
     bClockShown = FALSE;
     bErrorMsgShown = FALSE;
     bNameEntryMode = FALSE;
     HideNameEntryHelp ();
    if (bPIPshown)
        ScalePIP ();
    else if (iPOPshown != 0)
        ScalePOP ();
 }
}

bool isTimerFunctionOK (int type)
{
// type is 0 for timer add, 1 for modify 

 char str[10], msg[100], strconf[60], strlistindex[30];
 char str2[10];
// int totaltimers, timerindex, listindex, i;
 int totaltimers, timerindex1, timerindex2, listindex1, listindex2, i;
// unsigned long l;

 if (retvalTimer)
 {
  if (type == 0) strcpy (str, "adding");
  else   strcpy (str, "modifying");
// 09jul05 new return codes as per tony's timer extend 8.8 (sort of)
//     if (retvalTimer == 1)
  if (retvalTimer == 0xFFFFFFFF)
   sprintf (msg, "Error %s timer (Invalid Entry)", str);
//  else if (retvalTimer == 2)
  else if (retvalTimer == 0xFFFFFFFE)
   sprintf (msg, "Error %s timer (Invalid Tuner)", str);
  else if ((retvalTimer & 0xFFFF0000) == 0xFFFF0000)
  {
// conflict with other timer(s)
   totaltimers = TAP_Timer_GetTotalNum ();
//   timerindex = retvalTimer & 0x0000FFFF;
   timerindex1 = retvalTimer & 0x000000FF;
//   if (timerindex >= 0 && timerindex < totaltimers)
/* multiple timer conflict always seems to return FFFE0000 (no indices in lower bytes, as assumed by timer extend 8.8)
at least with may 10 2005 firmware and 1.22 api
   if ((retvalTimer & 0xFFFF0000) == 0xFFFE0000)
    timerindex2 = (retvalTimer & 0x0000FF00) >> 8;
   else
*/
    timerindex2 = -1;
//   {
//    sprintf (strconf, "entry %02d", timerindex + 1);
    sprintf (strconf, "entry %02d", timerindex1 + 1);
    if (timerindex2 >= 0)
    {
        sprintf (str2, " and %02d", timerindex2 + 1);
        strcat (strconf, str2);
    }
    if (TimerListSortKey != 2)
    {
// find list index corresponding to timer number in conflict, so we can put both in message 
// (easier to find by list index if not sorted by timer index).
// no need for this if sorted by timer index, since they will be one and the same.
     listindex1 = 0; // in case we can't find it
     listindex2 = 0;
     for (i = 0; i < totaltimers; i++)
     {
      if (timerindex1 + 1 == TimerNumArray[i])
      {
       listindex1 = i + 1;
       break;
      }
      else if ((timerindex2 >= 0) && (timerindex2 + 1 == TimerNumArray[i]))
      {
       listindex2 = i + 1;
       break;
      }
     }
     if (listindex1 > 0)
     {
      sprintf (strlistindex, ", list index %02d", listindex1);
      strcat (strconf, strlistindex);
      if (listindex2 > 0)
      {
        sprintf (str2, " and %02d", listindex2);
        strcat (strconf, str2);
      }
     }
    }   //end of need to show list index as well as timer index
    strcat (strconf, ")");
    sprintf (msg, "Error %s timer (Conflicts with %s", str, strconf);
//    l = retvalTimer;
//    hextoascii (l, str2);
//    sprintf (msg, "Error %s timer (Conflict: %d", str, retvalTimer);
  } // end of single timer conflict
  else
    sprintf (msg, "Error %s timer (Conflicts with multiple entries?)", str);
/*
  else if ((retvalTimer & 0xFFFF0000) == 0xFFFE0000)
    sprintf (msg, "Error %s timer (Conflicts with multiple entries", str);
  else
  {
//    sprintf (msg, "Error %s timer (Unknown Error)", str);
    sprintf (msg, "retvalTimer = %d, %x", retvalTimer, retvalTimer);
  }
*/
  ShowTimerErrorMsg (msg);
  return FALSE;
 }
 else
  return TRUE;
}

void CheckTimerChannelChange (void)
{

// this is an attempt to get around bug in TAP_Timer_Add where it does not always change to correct channel
// when timer recording starts.


 int tvRadio, chNum, i, totalTimerNum;
 word mjd;
 byte hour, min, sec;
// unsigned long timerstartmin, nowmin;
 int timerstartmin, nowmin;
 char str[10];
// TYPE_TapChInfo chInfo;
 TYPE_TimerInfo timerInf;

 TAP_Channel_GetCurrent (&tvRadio, &chNum);
// chInfo.tunerNum is not the real tuner (0 or 1 as used in timer), so don't bother
// TAP_Channel_GetInfo (tvRadio,  chNum, &chInfo);

// sprintf (str, "%d/%d", chNum, chInfo.tunerNum);
// ShowMessageWin ("Current channel/tuner ", str);

 totalTimerNum = TAP_Timer_GetTotalNum();
 TAP_GetTime (&mjd, &hour, &min, &sec);
 nowmin = (mjd * 1440) + (hour * 60) + min;

// nowmin = (hour * 60) + min;


// if there is a timer due to start in the next minute on the current main tuner and
// the current main channel is different from that in the timer, change the main to that in the timer,
// because, due to a bug in TAP_Timer_Add/Modify, it won't change the channel automatically when
// the timer starts in these circumstances.

 for (i = 0; i < totalTimerNum; i++)
 {
  TAP_Timer_GetInfo (i, &timerInf);
  
//  sprintf (str, "%d/%d", timerInf.tuner, timerInf.svcNum);
//  ShowMessageWin ("Timer channel/tuner ", str);

// chInfo.tunerNum is useless since it always returns 3, regardless of which tuner the main is on

// so change main channel to timer channel if main is not already on the timer channel, regardless of tuner
//  if (timerInf.tuner == chInfo.tunerNum)
  {
   timerstartmin = (((timerInf.startTime >> 16) & 0xffff) * 1440) + (((timerInf.startTime >> 8) & 0xff) * 60) + (timerInf.startTime & 0xff);
//   timerstartmin = (((timerInf.startTime >> 8) & 0xff) * 60) + (timerInf.startTime & 0xff);
   if (timerstartmin  == (nowmin + 1))
   {
    if (timerInf.svcNum != chNum)
    {
     TAP_Channel_Start (CHANNEL_Main, 0, timerInf.svcNum);
     return;
    }
   }
  
  }
 }
}

void SortFileListFolderArray ()
{
// global FileListSortKey is 0 for date/time, 1 for alphabetic, 2 for size, all ascending
/* Simple bubble sort */
 int nswaps, i;
 dword mjd1, sec1, mjd2, sec2;
 int entrytype1, entrytype2;
 char name1[TS_FILE_NAME_SIZE], name2[TS_FILE_NAME_SIZE];
 ulong64 size1, size2;
 TYPE_File tempfile;

 if (FileListIndexMax  <= 1) return;
 do {
  for (nswaps = i = 0 ; i < FileListIndexMax - 1 ; i++)
  {
// set primary sort key so that folders sort before anything else
   if (FileListFolderArray[i].attr == ATTR_FOLDER)
   {
    entrytype1 = 0;
    strcpy (name1, FileListFolderArray[i].name);
   }
   else
   {
    entrytype1 = 1;
    switch (FileListSortKey)
    {
     case 1:
     {
      strcpy (name1, FileListFolderArray[i].name); break;
     }
     case 2:
     {
      size1 = FileListFolderArray[i].size; break;

     }
     default:
     {
      mjd1 = FileListFolderArray[i].mjd;
      sec1 = (FileListFolderArray[i].hour * 86400) + (FileListFolderArray[i].min * 60) + FileListFolderArray[i].sec;
     }
    }
   }
   if (FileListFolderArray[i+1].attr == ATTR_FOLDER)
   {   
    entrytype2 = 0;
    strcpy (name2, FileListFolderArray[i+1].name);
   }
   else
   {
    entrytype2 = 1;
    switch (FileListSortKey)
    {
     case 1:
     {
      strcpy (name2, FileListFolderArray[i+1].name); break;
     }
     case 2:
     {
      size2 = FileListFolderArray[i+1].size; break;
     }
     default:
     {
      mjd2 = FileListFolderArray[i+1].mjd;
      sec2 = (FileListFolderArray[i+1].hour * 86400) + (FileListFolderArray[i+1].min * 60) + FileListFolderArray[i+1].sec;
     }
    }
   }

   if (entrytype1 < entrytype2) continue;
   if (entrytype1 == entrytype2)
   {
    if (entrytype1 == 0)
    {
     if (strcmp (name1, name2) <= 0) continue;
    }
    else
    switch (FileListSortKey)
    {
     case 1:
     {
      if (strcmp (name1, name2) <= 0) continue;
      else break;
     }
     case 2:
     {
      if (size1 <= size2) continue;
      else break;
     }
     default:

     {
      if ((mjd1 < mjd2) || ((mjd1 == mjd2) && (sec1 <= sec2))) continue;
     }
    }
   }

   tempfile = FileListFolderArray[i];
   FileListFolderArray[i] = FileListFolderArray[i+1];
   FileListFolderArray[i+1] = tempfile;
   ++nswaps;
  }
 } while (nswaps > 0);
}

void ScanCurrentFolder ()
{
// scan current folder, counting files, folders, bytes for display in header, and storing entries in folder array.
// sets flag to indicate whether files entries will subsequently come from folder array or direct from FindFirst/FindNext
// (the latter is necessary if there are too many to fit in the folder array).
/*
 char str[255];
 char str1[100];
 char strunit[3];
 dword dwtemp;
 int width, i;
*/
 dword fileindex, j;
 TYPE_File file;

  FileListCountFiles = 0;
  FileListCountFolders = 0;
  FileListCountBytes = 0;
  FileListIndexMax = TAP_Hdd_FindFirst( &file );

  bGetFilesFromArray = TRUE;
  if (FileListIndexMax > 0)
  {
   fileindex = 0;
//   while (TRUE)
   for (j = 1; j <= FileListIndexMax; j++)
   {
    if (file.attr == ATTR_FOLDER)
     FileListCountFolders += 1;
    else if (file.attr == ATTR_NORMAL || file.attr == ATTR_TS || file.attr == ATTR_PROGRAM)
    {
// ignore the funny file in the DataFiles folder
     if (IsDataFilesFolderShown () && (strcmp (file.name, "__temprec__.ss") == 0))
        goto GetNext;
     FileListCountFiles += 1;
     FileListCountBytes += file.size;
    }
    else
     goto GetNext;
    if (fileindex < FileListEntriesPerFolder)
    {
     FileListFolderArray[fileindex] = file;
     fileindex += 1;
    }
    else
     bGetFilesFromArray = FALSE;
   GetNext:
    if (TAP_Hdd_FindNext (&file) == 0)
     break;
   }

   if (bGetFilesFromArray)
   {
    FileListIndexMax = FileListCountFolders + FileListCountFiles;
    SortFileListFolderArray ();
   }
// 26jul05 if we just stopped a recording which is too short to be saved,
// FileListIndex can be pointing to a file which doesn't exist any more, so set it to the last one
   FileListIndex = min (FileListIndex, FileListIndexMax);
  }
}

bool GetFiles (dword indexfirst)
{
// get file entries into screen display array, starting at the one specified by indexfirst
// return false if we did not find the specified index

 dword totentry;
 int screenindex;
 dword folderindex;
 
 if (bGetFilesFromArray)
 {
  for (screenindex = 0; screenindex < FileListEntriesPerScreen; screenindex++)
  {
   folderindex = indexfirst + screenindex - 1;
   if (folderindex < FileListEntriesPerFolder)
   {
    FileListScreenArray[screenindex] = FileListFolderArray[folderindex];
    if (bFileListRefreshDuration)
    {
     FileListDurationArray[screenindex] = GetFileDuration (FileListScreenArray[screenindex]);
     FileListDurationSecArray[screenindex] = FileListDurationSec;
     FileListLcnArray[screenindex] = FileListLcn; // set by GetFileDuration
     FileListStartHourArray[screenindex] = FileListStartHour; // set by GetFileDuration
     FileListStartMinArray[screenindex] = FileListStartMin; // set by GetFileDuration
     FileListStartSecArray[screenindex] = FileListStartSec; // set by GetFileDuration
//     bFileListRefreshDuration = FALSE;
    }
   }
  }
 }
 else
 {
  totentry = TAP_Hdd_FindFirst (&FileListEntry);
  if (totentry == 0) return FALSE;
// position to first entry
  for (folderindex = 2; folderindex <= indexfirst; folderindex++)
  {
   if (TAP_Hdd_FindNext (&FileListEntry) == 0)

    return FALSE;
  }
// store entries from first one onwards
  for (screenindex = 0; screenindex < FileListEntriesPerScreen; screenindex++)
  {
// don't store entries with unknown attribute in screen array
   if (FileListEntry.attr == ATTR_FOLDER || FileListEntry.attr == ATTR_NORMAL || FileListEntry.attr == ATTR_TS || FileListEntry.attr == ATTR_PROGRAM)
   {
    FileListScreenArray[screenindex] = FileListEntry;
    if (bFileListRefreshDuration)
    {
     FileListDurationArray[screenindex] = GetFileDuration (FileListEntry);
     FileListDurationSecArray[screenindex] = FileListDurationSec;
     FileListLcnArray[screenindex] = FileListLcn; // set by GetFileDuration
     FileListStartHourArray[screenindex] = FileListStartHour; // set by GetFileDuration
     FileListStartMinArray[screenindex] = FileListStartMin; // set by GetFileDuration
     FileListStartSecArray[screenindex] = FileListStartSec; // set by GetFileDuration
//     bFileListRefreshDuration = FALSE;
    }

   }
   else
   {
//    FileListIndexMax -=1 ;
    screenindex -= 1;
   }
   if (TAP_Hdd_FindNext (&FileListEntry) == 0)
    break;
  }
 }
 if (bFileListRefreshDuration)
  bFileListRefreshDuration = FALSE;
 return TRUE;
}

void ShowFiles (bool bClearFirst)
{
// display file entries from screen array
 dword fileindex;

 if (bClearFirst)
 {

// clear out the file list box before refreshing
  TAP_Osd_FillBox (rgnFile, 0, FileHeadH, FileWinW, FileWinH-FileHeadH-FileErrorH, COLOR_BackFileListNormal);
 }

 FileListIndexBottom = min (FileListIndexTop + FileListEntriesPerScreen - 1, FileListIndexMax);
 for (fileindex = FileListIndexTop; fileindex <= FileListIndexBottom; fileindex++)
  ShowFileListIndex (fileindex);
}

void ListCurrentFolder (bool bClearFirst)
{
 char str[255];
 char str1[100];
 char strunit[3];
 int i, width;

 if (bClearFirst)
 {
// clear out the file list box before displaying header so that we don't see new folder name with old folder's contents
  TAP_Osd_FillBox (rgnFile, 0, FileHeadH, FileWinW, FileWinH-FileHeadH-FileErrorH, COLOR_BackFileListNormal);
 }

 TAP_Osd_FillBox (rgnFile, 0, 0, FileWinW, FileHeadH, COLOR_BackFileHead);
 DisplayFileSize (FileListCountBytes, str1, strunit);
 sprintf (str, "%d Folder(s), %d File(s), ", FileListCountFolders, FileListCountFiles);
 strcat (str, str1);
 strcat (str, " ");
 strcat (str, strunit);
 width = TAP_Osd_GetW (str, 0, FNT_Size_FileHead);
 TAP_Osd_PutS(rgnFile, FileWinXMarR-width, 0, FileWinXMarR, str, COLOR_ForeFileHead, COLOR_None, 0, FNT_Size_FileHead, 0, ALIGN_RIGHT);

 if (FileListPathLevel < FileListMaxPathLevels)  // level counter can now exceed capacity of path array
 {
  strcpy (str, FileListPath[0]);
  for (i = 1; i <= FileListPathLevel; i++)
  {
   strcat (str, "/");
   strcat (str, FileListPath[i]);
  }
// if full path name won't fit, just show lowest level
  if (TAP_Osd_GetW (str, 0, FNT_Size_FileHead) > (FileWinXMarR - width - FileWinXMarL - 10))
   strcpy (str, FileListPath[FileListPathLevel]);
 }
 else
  strcpy (str, "[Folder Path Level Exceeds Capacity]");
 TAP_Osd_PutS(rgnFile, FileWinXMarL, 0, FileWinXMarR-width-10, str, COLOR_ForeFileHead, COLOR_None, 0, FNT_Size_FileHead, 1, ALIGN_LEFT);
/*
 if (GetFiles (FileListIndexTop));
  ShowFiles (bClearFirst);
*/
 if (GetFiles (FileListIndexTop))
  ShowFiles (FALSE); // already cleared file list above, if necessary
}

void ListNewFolder ()
{

 int index;
 bool foundplay;

 ScanCurrentFolder ();

// if playing back and new folder is DataFiles, set index to point to file playing
// scancurrentfolder sets bgetfilesfromarray, but the files we are looking for (playing) may not be in the first
// screenful, so we can only do this if the files are in the folder array, then tell getfiles where to start
// the listing in the screen array.

// set default values for indices
 FileListIndexTop = 1;
 FileListIndex = 1;

 if (bGetFilesFromArray)
 {
  if (bFilePlayingBack && (strcmp (FileListPath[FileListPathLevel], "DataFiles") == 0))
  {
   for (index = 1; index <= FileListIndexMax; index++)
   {
    if (strcmp (FileListFolderArray[index-1].name, FileListNamePlay) == 0)
    {
     FileListIndex = index;
     if (FileListIndex > FileListEntriesPerScreen)
      FileListIndexTop = FileListIndex;
     else
      FileListIndexTop = 1;
     break;
    }
   }
  }
 }
 ListCurrentFolder (TRUE);
}

void DisplayFileBuffer ()
{
 int i, yoff, xpos, xoffhex, xoffasc, width;
 byte c;
 char str[3];
 char str1[35];
 char strnum[12], strunit[3];

 TAP_Osd_FillBox (rgnFile, 0, 0, FileWinW, FileHeadH, COLOR_BackFileHead);
// if file length is more than 31 bits, dword returned by TAP_Hdd_Flen will appear negative
// if (FileDisplayLength << 1 & 1 == 1)
/*
 if ((FileDisplayLength & 0x80000000) != 0)
  {
   sprintf (str1, "Byte %d of ", FileDisplayStart);
   strcat (str1, "> 2**31");
  }
 else
  sprintf (str1, "Byte %d of %d", FileDisplayStart, FileDisplayLength);
*/
 if (FileDisplayLengthLong >> 31 == 0)
  sprintf (str1, "Byte %d of %d", FileDisplayStart, FileDisplayLength);
 else
 {
  sprintf (str1, "Byte %d of ", FileDisplayStart);
  DisplayFileSize (FileDisplayLengthLong, strnum, strunit);
  strcat (str1, strnum);
  strcat (str1, strunit);
 }
 width = TAP_Osd_GetW (str1, 0, FNT_Size_FileHead);
 TAP_Osd_PutS(rgnFile, FileWinXMarR-width, 0, FileWinXMarR, str1, COLOR_ForeFileHead, COLOR_None, 0, FNT_Size_FileHead, 0, ALIGN_LEFT);
 TAP_Osd_PutS(rgnFile, FileWinXMarL, 0, FileWinXMarR-width-10, FileListEntry.name, COLOR_ForeFileHead, COLOR_None, 0, FNT_Size_FileHead, 1, ALIGN_LEFT);
 TAP_Osd_FillBox (rgnFile, 0, FileHeadH, FileWinW, FileWinH-FileHeadH-FileErrorH, COLOR_BackFileListNormal);


 for (i = 0; i < FileDisplayBytesPerScreen; i++)
 {
  if (FileDisplayStart + i >= FileDisplayLength)
   break;
  yoff = FileHeadH + ((i / FileDisplayBytesPerLine) * FileDisplayYincrement);
  xpos = i % FileDisplayBytesPerLine;
  xoffhex = FileWinXMarL + (xpos * FileDisplayXincHex);

  xoffasc = FileWinXMarR - ((FileDisplayBytesPerLine - xpos) * FileDisplayXincAsc);
  c = FileDisplayBuffer[i];
  hextoasciibyte (c, str);

//  TAP_Osd_PutS(rgnFile, xoffhex, yoff, FileWinW, str, COLOR_ForeFileDisplay, COLOR_None, 0, FNT_Size_FileDisplay, 0, ALIGN_LEFT);
  TAP_Osd_PutS(rgnFile, xoffhex, yoff, FileWinW-1, str, COLOR_ForeFileDisplay, COLOR_None, 0, FNT_Size_FileDisplay, 0, ALIGN_LEFT);
//  TAP_Osd_PutS(rgnFile, xoffhex, yoff, xoffhex+17, str, COLOR_ForeFileDisplay, COLOR_None, 0, FNT_Size_FileDisplay, 0, ALIGN_CENTER);

// substitute rectangle for ascii control code, to distinguish from a blank
  if (c < 32)
   TAP_Osd_DrawRectangle (rgnFile, xoffasc, yoff, 12, 19, 2, COLOR_ForeFileDisplay);

  else
   {
    str[0] = c;
    str[1] = '\0';
    TAP_Osd_PutS(rgnFile, xoffasc, yoff, FileWinW-1, str, COLOR_ForeFileDisplay, COLOR_None, 0, FNT_Size_1419, 0, ALIGN_LEFT);
//    TAP_Osd_PutS(rgnFile, xoffasc, yoff, xoffasc+15, str, COLOR_ForeFileDisplay, COLOR_None, 0, FNT_Size_1419, 0, ALIGN_LEFT);
   }
 }
}

void ShowFileList (bool bSwitchHeight)
{
//    int x, y, w, h;
// 04jun05 bClockShown will still be set if an error message or name entry help is displayed on the clock line
// with the file list already up (so that the clock would have been using rgnFile prior to the message),
// so test for the existence of the clock region instead (rgnClock and rgnPlayRec should exist together).
// if (bClockShown)
 if (!bSwitchHeight)
 {
     if (rgnClock)
     {
      TAP_Osd_Delete (rgnClock); // we will now show clock in file region
      TAP_Osd_Delete (rgnPlayRec);
     }
// 28apr05 exit normal regardless of clock since clock does not exit normal any more
// else
     TAP_ExitNormal();      // leaving normal receiver operation, receiver hides its OSDs
     bFileMode = TRUE; // UpdateClock and ScalePOP need this
 }
// set file list height
 if (bFileModeHeightFull)
 {
  FileWinH = 528; // gives 20 entries per screen
  FileDisplayLinesPerScreen = 20;
 }
 else
 {
  FileWinH = 288; // gives 10 entries per screen
  FileDisplayLinesPerScreen = 10;
// if already in PIP or POP, show two windows below file list, else one
  if (bPIPshown || (iPOPshown != 0))
   ScalePOP ();
  else
   ScaleMainHalfHeight (FileWinY + FileWinH);
 }
 SetFileListParameters ();
 FileDisplayBytesPerScreen = FileDisplayBytesPerLine * FileDisplayLinesPerScreen;
// if we are changing to full height make better use of the extra space so we can see as many entries
// as possible at once.
 if (bSwitchHeight && bFileModeHeightFull)
 {
   if (FileListIndexTop > 1 && FileListIndexMax <= FileListEntriesPerScreen)
        FileListIndexTop = 1;
 }

 rgnFile = TAP_Osd_Create (FileWinX, FileWinY, FileWinW, FileWinH, 0, FALSE);
// put up the background boxes before any hdd call, in case disk has to spin up
 TAP_Osd_FillBox (rgnFile, 0, 0, FileWinW, FileHeadH, COLOR_BackFileHead);
 TAP_Osd_FillBox (rgnFile, 0, FileHeadH, FileWinW, FileWinH-FileHeadH-FileErrorH, COLOR_BackFileListNormal);
// TAP_Osd_FillBox (rgnFile, 0, FileErrorYoffset, FileWinW, FileErrorH, COLOR_BackClock);

// bFileMode = TRUE; // UpdateClock needs this
 if (bResumeFileDisplay)
 {
  bResumeFileDisplay = FALSE;
  bFileDisplayShown = TRUE;
  DisplayFileBuffer ();
 }
 else
 {
// show clock before listing folder, as this can be quite slow (particularly if datafiles folder) and looks funny
  UpdateClock ();
  if (bSwitchHeight)
  {
// ensure current file index is shown after switch height
// 06jul05 but put it at bottom rather than top
    if (FileListIndex > FileListEntriesPerScreen)
//        FileListIndexTop = FileListIndex;
        FileListIndexTop = max (FileListIndex - FileListEntriesPerScreen + 1, 1);
    ListCurrentFolder (TRUE);
    if (bNameEntryMode)
    {
        ShowNameEntry (NameEntryBuffer);
        ShowNameEntryHelp ();
        ShowNameEntryPos ();
    }
    else if (bFileErrorMsgShown)
        ShowFileErrorMsg (ErrorMsgSaved, TRUE);
  }
  else
  {
// 28jun05 isdatafilesfoldershown will be called as a result of this call to listnewfolder,
// so bfilelistshown must be set first.
//      ListNewFolder ();
      bFileListShown = TRUE;
      ListNewFolder ();
  }
 }
// bFileMode = TRUE;
 bClockShown = TRUE;
// ClockCounter = 0;
 TickClockSaved = 0; // force clock update next time in idle
 rgnNameEntry = rgnFile;
}

void HideFileList (bool bSwitchHeight)

{
 TAP_Osd_Delete (rgnFile);
// set flag to refresh duration when next shown, otherwise things may
// get out of step
// bFileListRefreshDuration = TRUE; 
 if (!bSwitchHeight)
 {
     TAP_EnterNormal();      // re-entering normal receiver operation

// if not on the first screenful, set flag to refresh duration when next shown, otherwise things
// get out of step
    if (FileListIndexBottom > FileListEntriesPerScreen)
        bFileListRefreshDuration = TRUE; 

     bFileListShown = FALSE;
     bFileMode = FALSE;
     bClockShown = FALSE;
     bFileErrorMsgShown = FALSE;
     bNameEntryMode = FALSE;
     HideNameEntryHelp ();
    if (bPIPshown)
        ScalePIP ();
    else if (iPOPshown != 0)
        ScalePOP ();
 }
}

void HideFileErrorMsg ()
{
// if (!bFileListShown)
 bFileErrorMsgShown = FALSE;
 if (!bFileMode)
 {
  TAP_Osd_Delete (rgnFile);
//  TAP_EnterNormal();      // re-entering normal receiver operation
 }
 else if (!bFileCopyInProgress)
 {
//  TAP_Osd_FillBox(rgnFile, 0, FileErrorYoffset, FileWinW, FileErrorH, COLOR_BackFileError);
// clock is now present when no error message, so set appropriate background color
  TAP_Osd_FillBox(rgnFile, 0, FileErrorYoffset, FileWinW, FileErrorH, COLOR_BackClock);
//  bFileErrorMsgShown = FALSE;
 }
// if no other msg to be shown, we still want to know if we have a file copy pending
 else
  ShowFileErrorMsg ("File copy pending. Press yellow key to copy, exit to abort.", TRUE);
}

void ShowFileErrorMsg (char *msg, bool bAcknowledge)
{
// if (!bFileListShown)
 if (!bFileMode)
 {
// drop play counter if up, since shares area with file error msg
  if (bPlayCounterShown)
    HidePlayCounter ();
//  TAP_ExitNormal ();
//  rgnFile = TAP_Osd_Create (FileWinX, FileWinY, FileWinW, FileWinH, 0, FALSE);
  rgnFile = TAP_Osd_Create (FileWinX, ymax - 2*FileErrorH, FileWinW, FileErrorH, 0, FALSE);
//  TAP_Osd_FillBox(rgnFile, 0, FileErrorYoffset, FileWinW, FileErrorH, COLOR_BackFileError);
//  TAP_Osd_PutS(rgnFile, FileWinXMarL, FileErrorYoffset, FileWinXMarR, msg, COLOR_ForeFileError, COLOR_None, 0, FNT_Size_FileError, 1, ALIGN_CENTER);
  TAP_Osd_FillBox(rgnFile, 0, 0, FileWinW, FileErrorH, COLOR_BackFileError);
  TAP_Osd_PutS(rgnFile, FileWinXMarL, 0, FileWinXMarR, msg, COLOR_ForeFileError, COLOR_None, 0, FNT_Size_FileError, 1, ALIGN_CENTER);

// if not in file mode, message should always be near bottom of screen, since FileErrorYoffset may have been
// set for half height last time we were in file mode.
 }
 else
 {
  TAP_Osd_FillBox(rgnFile, 0, FileErrorYoffset, FileWinW, FileErrorH, COLOR_BackFileError);
  TAP_Osd_PutS(rgnFile, FileWinXMarL, FileErrorYoffset, FileWinXMarR, msg, COLOR_ForeFileError, COLOR_None, 0, FNT_Size_FileError, 1, ALIGN_CENTER);
 }
 if (bAcknowledge)
 {
  bFileErrorMsgShown = TRUE;
// save error msg so we can re-instate when switching height
  strcpy (ErrorMsgSaved, msg);
 }
 else
 {
//  TAP_Delay (150);
  TAP_Delay (100);
  HideFileErrorMsg ();
 }
}

bool GetFileEntry (dword index)
{
// find specified file entry number in current folder and save it in global variable
// return true if found, else false
 dword totentry, i;
 
 if (bGetFilesFromArray)
 {
/* 11jun05 this makes no sense since deletefilenameaction requires finding entries which are not
   necessarily in the current screen, to find the next file to play.
   it became more likely to fail in half-height mode because entries per screen would be smaller.
  if ((index - FileListIndexTop) >= FileListEntriesPerScreen)
   return FALSE;
  FileListEntry = FileListScreenArray[index - FileListIndexTop];
*/
  if (index < 1 || index > FileListIndexMax)
    return FALSE;
  FileListEntry = FileListFolderArray[index - 1];
 }
 else
 {
  totentry = TAP_Hdd_FindFirst (&FileListEntry);
// if (totentry == 0)
//  return FALSE;
  for (i = 1; i < index; i++)
  {
   if (TAP_Hdd_FindNext (&FileListEntry) == 0)
    return FALSE;
  }
 }
 return TRUE;
}

void SaveTsFileHeader (TYPE_File file)
{
 int i;

 if (file.attr == ATTR_TS)
 {

  FileDisplayPointer = TAP_Hdd_Fopen (file.name);
  if ((file.size > 0) && (FileDisplayPointer != 0))
  {
   TAP_Hdd_Fread (TsFileHeader, sizeof TsFileHeader, 1, FileDisplayPointer);
//   duration = FileDisplayBuffer[8] * 255 + FileDisplayBuffer[9];
   TAP_Hdd_Fclose (FileDisplayPointer);
  }
  else
  {
   for (i = 0; i < sizeof TsFileHeader; i++)
    TsFileHeader[i] = 0x00;
  }
 }
}
/*
void ShowFilePlaying (char *prefix)
{

// display message with "starting" or "resuming" (in prefix parameter) followed by file name playing
 char msg[256];
 char str[30];

 strcpy (msg, prefix);
 strcat (msg, FileListNamePlay);
// sprintf (str, " (%d blocks per minute)", BlockRateNormalPlay);
 sprintf (str, " (%d.%d bpm)", BlockRateNormalPlay / 10, BlockRateNormalPlay % 10);
 strcat (msg, str);
 ShowFileErrorMsg (msg, FALSE);
}
*/
void PlayFileListNamePlay (bool bResume)
{
 char msg[255];
 word svcnum;

/*
 if (!bResume)
 {
// save TS file header (including duration_ before we play it, because we can't open it while it is playing

  SaveTsFileHeader (FileListEntry);
 }
*/
 if (!TAP_Hdd_Exist (FileListNamePlay))
 {
// try to find last played file in DataFiles Folder, since we may have changed current folder
  ChangeToDataFilesFolder ();
 }
 if (TAP_Hdd_PlayTs (FileListNamePlay) == 0)
 {
  TickFilePlayStart = TAP_GetTick ();
  bFilePlayingBack = TRUE;
  if (bResume)
  {

// resuming playback
// go to previous bookmark and delete it
// generate events seem not to work if clock shown (when clock exits normal)
// so kill the clock if it is shown
/* 11jun05 now use playback positioning function to go direct to saved block
   if (bClockShown)
    HideClock ();
   TAP_GenerateEvent (EVT_KEY, RKEY_Goto, RKEY_Goto);
   TAP_GenerateEvent (EVT_KEY, RKEY_Bookmark, RKEY_Bookmark);
*/
//   if (BlockPlayStopped > 0) 
//    sprintf (msg, "Resume play at block %d", BlockPlayStopped);
//    ShowFileErrorMsg (msg, FALSE);
// for some reason positioning to the exact block saved loses about 3 sec., so adjust the block count
//   if (BlockPlayStopped > 0) 
//    PositionPlayback (BlockPlayStopped);
   if (BlockPlayStopped > 50) 
    PositionPlayback (BlockPlayStopped - 50);
  }
  else
  {
// starting playback of a new file
   TickFilePlay = 0;
   TAP_GetTime (&mjd, &HourFilePlayStart, &MinFilePlayStart, &sec);
// round play start time to nearest minute
   if (sec >= 30)
   {
    MinFilePlayStart++;
    if (MinFilePlayStart > 59)
    {
     MinFilePlayStart = 0;
     HourFilePlayStart++;
     if (HourFilePlayStart > 23)
      HourFilePlayStart = 0;
    }
   }
//   strcpy (FileListNamePlay, FileListEntry.name);
//   FileListEntryPlay = FileListEntry;
//   FileListStartTimePlay = FileListEntry.mjd << 16 | FileListEntry.hour << 8 | FileListEntry.min;
   FileListDurationPlay = TsFileHeader[8] * 255 + TsFileHeader[9];
   svcnum = TsFileHeader[10] * 255 + TsFileHeader[11];
   FileListLcnPlay = SvcNumToLcn (svcnum);
   FileListDurationSecPlay = TsFileHeader[TsHeaderByteDurationSec];
   FileListStartHourPlay = TsFileHeader[TsHeaderByteStartHour];
   FileListStartMinPlay = TsFileHeader[TsHeaderByteStartMin];
   FileListStartSecPlay = TsFileHeader[TsHeaderByteStartSec];
// initialize for dynamic block rate calculation
   if (TAP_Hdd_GetPlayInfo (&PlayInfo) && (PlayInfo.playMode == PLAYMODE_Playing))
   { 
//    BlockRateNormalPlay = (PlayInfo.totalBlock + (PlayInfo.duration * 3))/ (PlayInfo.duration * 6); // default blocks per 10 sec
//    BlockRateNormalPlay = (PlayInfo.totalBlock + (PlayInfo.duration / 2))/ (PlayInfo.duration); // default blocks per minute
//    BlockRateNormalPlay = ((PlayInfo.totalBlock + (PlayInfo.duration / 2)) * 10) / PlayInfo.duration; // default blocks per minute (*10)
// if we have saved seconds for duration, use it here
    if ((FileListDurationSecPlay & 0x80) == 0x80)
    {
        BlockRateNormalPlay = (PlayInfo.totalBlock * 600) / (PlayInfo.duration * 60 + (FileListDurationSecPlay & 0x7F)); // default blocks per minute (*10)
// we have the start seconds, so if the hour and minute in the ts header match those
// previously saved from the file entry (in the eventhandler or deletefilenameaction) override the zero seconds value set there
        if ((FileListStartHourPlay == FileListStartPlayHour) && (FileListStartMinPlay == FileListStartPlayMin))
          FileListStartPlaySec = FileListStartSecPlay;
    }
    else
        BlockRateNormalPlay = ((PlayInfo.totalBlock * 10) + (PlayInfo.duration / 2)) / PlayInfo.duration; // default blocks per minute (*10)
   }
   else
    BlockRateNormalPlay = 0;    // should not happen
// 16jun05 save initial block rate for later comparison with calculated one
   BlockRateNormalPlayInit = BlockRateNormalPlay;
   BlockRateNormalPlayDyn = BlockRateNormalPlay;
   bBlockRateDynamic = FALSE;
/*
   TrickModePrev = TRICKMODE_Normal;
   TicksInNormalPlayCurrent = 0;
   BlocksInNormalPlayCurrent = 0;
*/
// force idle to do its "just entered normal play" code next time.
   TrickModePrev = TRICKMODE_Pause;
   TicksInNormalPlayCumulative = 0;
   BlocksInNormalPlayCumulative = 0;
   TicksInNormalPlaySaved = 0;
   TickBlockRateUpdated = TickFilePlayStart;
//   TickBlockRateSaved = TickFilePlayStart;
//   BlockCountSaved = 0;

//   ShowFilePlaying ("Starting ");
  } //end of starting play new file
  BlockPlayStopped = 0;
  TAP_EnterNormal ();
 }
 else
 {
  strcpy (msg, "Playback of ");
  strcat (msg, FileListNamePlay);
  strcat (msg, " failed");
  ShowFileErrorMsg (msg, TRUE);
 }
}

void DeleteFileNameAction ()
{
 char msg[256];
 dword i;

 TAP_Hdd_Delete (FileNameAction);
 if (TAP_Hdd_Exist (FileNameAction))
 {

  strcpy (msg, "Deletion of ");
  strcat (msg, FileNameAction);
  strcat (msg, " failed");
  ShowFileErrorMsg (msg, TRUE);
  return;
 }
 if (strcmp (FileNameAction, FileListNamePlay) == 0)
 {
//  strcpy (FileListNamePlay, " ");
// we just deleted the file which was playing or last-played. if the file list is not shown,
// find its index so that the next play key
// (not from the file list) can start playback of the next file in the array
//  FileListIndexPlay = 0;
  strcpy (FileListNamePlayNext, "");
  if (!bFileListShown)
  {
   for (i = 1; i <= FileListIndexMax; i++)
   {
    if (GetFileEntry (i))
    {
     if (strcmp (FileListNamePlay, FileListEntry.name) == 0)
     {
//      if (i < FileListIndexMax)
// look for next TS file to play
      while (i < FileListIndexMax)
      {
//       FileListIndexPlay = i; // after refresh, next file should have same index as one just deleted
       if (GetFileEntry (i+1))
       {
        if (FileListEntry.attr == ATTR_TS)
        {
         strcpy (FileListNamePlayNext, FileListEntry.name);
//         FileListStartTimePlayNext = FileListEntry.mjd << 16 | FileListEntry.hour << 8 | FileListEntry.min;
         FileListStartPlayNextMjd = FileListEntry.mjd;
         FileListStartPlayNextHour = FileListEntry.hour;
         FileListStartPlayNextMin = FileListEntry.min;
         FileListStartPlayNextSec = 0;
         SaveTsFileHeader (FileListEntry);
         break;
        }
       }
       i++;
      }
//      break;
     }

    }
   }
  }
  strcpy (FileListNamePlay, "");
 }
/* 01sep04 after re-instating play next code above
 if (strcmp (FileNameAction, FileListNamePlay) == 0)
  strcpy (FileListNamePlay, "");
*/
// 29apr05 force refresh of duration, etc. from file headers either if file list is shown now, or when it is shown next.
 bFileListRefreshDuration = TRUE;
 if (bFileListShown)
 {
// after successful delete, refresh list
// if we just deleted the entry at the top of the screen and there are more in front,
// get previous screenful
  if ((FileListIndex == FileListIndexTop) && (FileListIndexTop > 1))
  {
   FileListIndexTop = FileListIndexTop - FileListEntriesPerScreen;
   if (FileListIndexTop < 1)
    FileListIndexTop = 1;
   FileListIndex = FileListIndexTop + FileListEntriesPerScreen - 1;
  }
  if (FileListIndex > FileListIndexMax - 1)
   FileListIndex = FileListIndexMax - 1;

  ScanCurrentFolder ();
//  bFileListRefreshDuration = TRUE;
  ListCurrentFolder (TRUE);
 }
 else
 {
//  ShowMessageWin (FileNameAction, "deleted");
  strcpy (msg, FileNameAction);
  strcat (msg, " deleted");
  ShowFileErrorMsg (msg, FALSE);
/*
// need to rescan even if file list not shown, so that index for next play doesn't point to the
// file we just deleted. also need to refresh entries in screen array, so that GetFileEntry and 
// PlayFileListEntry get updated info (e.g. if next file was still recording when we deleted previous one).
  ScanCurrentFolder ();
  GetFiles (FileListIndexTop);

*/
 }
}

void DisplayNextBlock ()
{
 dword currpos, readbytes;
 bool lastblock;
 
 currpos = TAP_Hdd_Ftell (FileDisplayPointer);
 if (currpos < FileDisplayLength)
 {
  if (FileDisplayLength - currpos < FileDisplayBytesPerScreen)
  {
   readbytes = FileDisplayLength - currpos;
   lastblock = TRUE;
  }
  else
  {
   readbytes = FileDisplayBytesPerScreen;
   lastblock = FALSE;
  }
  TAP_Hdd_Fread (FileDisplayBuffer, readbytes, 1, FileDisplayPointer);
//  TAP_Hdd_Fread (FileDisplayBuffer, FileDisplayBytesPerScreen, 1, FileDisplayPointer);
/*
  currpos = TAP_Hdd_Ftell (FileDisplayPointer);
  if (currpos < FileDisplayLength)



   FileDisplayStart = currpos - FileDisplayBytesPerScreen;
  else
   FileDisplayStart = FileDisplayStart + FileDisplayBytesPerScreen;
*/
  if (lastblock)
   FileDisplayStart = FileDisplayLength - readbytes;
  else
   FileDisplayStart = TAP_Hdd_Ftell (FileDisplayPointer) - readbytes;
  DisplayFileBuffer ();
/*
  if (lastblock)

   FileDisplayStart = FileDisplayLength;
  else
   FileDisplayStart += readbytes;
*/
 }
}

void DisplayNewFile ()
{
 char msg[255];

// 23apr05. check file size before trying to open, since it seems Fopen sometimes returns a non-zero pointer to an
// empty file, causing me to try to read it.

 FileDisplayLengthLong = FileListEntry.size;
 if (FileDisplayLengthLong == 0)
 {
  strcpy (msg, FileListEntry.name);
  strcat (msg, " is empty");
  ShowFileErrorMsg (msg, TRUE);
  return;
 }

 FileDisplayPointer = TAP_Hdd_Fopen (FileListEntry.name);
 if (FileDisplayPointer == 0)
 {
  strcpy (msg, "Unable to open ");
  strcat (msg, FileListEntry.name);
  if (strcmp (FileListEntry.name, FileListNamePlay) == 0)
   strcat (msg, " (it's playing back)");
  ShowFileErrorMsg (msg, TRUE);
 }
 else
 {
  FileDisplayLength = TAP_Hdd_Flen (FileDisplayPointer);
/*
  if (FileDisplayLength == 0)
  {
   strcpy (msg, FileListEntry.name);
   strcat (msg, " is empty");
   ShowFileErrorMsg (msg, TRUE);
  }
  else
*/
  {
//   FileDisplayLengthLong = FileListEntry.size;
   FileDisplayStart = 0;
//    TAP_Hdd_Fread (FileDisplayBuffer, FileDisplayBytesPerScreen, 1, FileDisplayPointer);
   DisplayNextBlock ();
   bFileDisplayShown = TRUE;
   bFileListShown = FALSE;
//    TAP_Hdd_Fclose (FileDisplayPointer);
  }
 }
}

void ExitFileDisplay ()
{
// exit file display mode and return to file list
 if (FileDisplayPointer != 0)
  TAP_Hdd_Fclose (FileDisplayPointer);
 HideFileErrorMsg ();
 bFileDisplayShown = FALSE;
 bFileInfoShown = FALSE;
 bFileListShown = TRUE;
 ScanCurrentFolder ();
 ListCurrentFolder (TRUE);
}

void ClearAllTimerEntryFields ()
{
// clear all timer entry fields to start afresh for a new entry
  for (TimerFieldIndex = TimerFieldIndexStart; TimerFieldIndex < NumTimerEntryFields; TimerFieldIndex++)
   ClearTimerEntryField ();
  TimerFieldIndex = TimerFieldIndexStart; // ensure we set file name prefix back to normal color
  ClearNameEntryField ();

  SetTimerDateToCurrent ();
  ShowTimerDate ();
  TimerReservationType = TimerReservationTypeDefault;
  ShowTimerReservationType ();
}

void CompleteTimerFunction ()
{
 int timertype, timerindex;
 int i, totalTimerNum, CurrentTimerNum;

 if (NameEntryBufferIndexMax >= 0)
 {
  strcpy (timerInfo.fileName, NameEntryBuffer);
  strcat (timerInfo.fileName, ".rec");
  timerInfo.nameFix = 1;
 }
 else
 {
  strcpy (timerInfo.fileName, "");
  timerInfo.nameFix = 0;
 }

 TimerFieldIndex = TimerFieldIndexSave;
 HideEntryInsertionField ();

 if (bTimerModifyInProgress)
 {
  timertype = 1;
  retvalTimer = TAP_Timer_Modify(TimerModifyEntryIndex, &timerInfo);
  HideTimerErrorMsg (); // clear "modify in progress" message
 }
 else
 {
  timertype = 0;
  retvalTimer = TAP_Timer_Add(&timerInfo);
 }

  TimerFieldIndex = TimerFieldIndexStart;
  TimerCharIndex = 0;
  ShowEntryInsertionField ();

 if (isTimerFunctionOK (timertype))
 {
// if add or modify worked, clear all input fields ready for another entry, else leave for modification
/*
  for (TimerFieldIndex = TimerFieldIndexStart; TimerFieldIndex < NumTimerEntryFields; TimerFieldIndex++)
   ClearTimerEntryField ();
  TimerFieldIndex = TimerFieldIndexStart; // ensure we set file name prefix back to normal color
  ClearNameEntryField ();

  SetTimerDateToCurrent ();
  ShowTimerDate ();
  TimerReservationType = TimerReservationTypeDefault;
  ShowTimerReservationType ();
*/
  ClearAllTimerEntryFields ();
  RefreshTimerListArray ();
  totalTimerNum = TAP_Timer_GetTotalNum();
  TimerListIndexMax = totalTimerNum - 1;

  if (!bTimerModifyInProgress)
// we just added a timer
    CurrentTimerNum = totalTimerNum;
  else
// we just modified a timer
    CurrentTimerNum = TimerModifyEntryIndex + 1;

// if we just added a timer amd the list is not sorted by timer index, we should position the current list index
// to the entry corresponding to the new timer, which is not necessarily the bottom one.
// also for modify need to point to timer number just modified, not list index.
  for (i = 0; i < totalTimerNum; i++)
  {
   if (TimerNumArray[i] == CurrentTimerNum)
    {
        TimerListIndex = i;
        break;
    }
  }
//  TimerListIndexBottom = TimerListIndexMax + 1; //TimerListIndexMax not updated till later in ListTimers
// if more than 1 screenful set top to calculated current one
// 07jun05 this should only apply to an added timer. if we just modified one, it will already be
// on the current screen, so leave the index at the top alone, rather than bringing the current one to the top.
// 06jul05. not true. the modified entry could be off the current screen because, for example, its date
// changed and we are sorted by date.
/*
  if (!bTimerModifyInProgress)
  {
      if (totalTimerNum > TimerListEntriesPerScreen)
          TimerListIndexTop = TimerListIndex;
      else
          TimerListIndexTop = max (0, TimerListIndexMax - TimerListEntriesPerScreen + 1);
  }
  else
    bTimerModifyInProgress = FALSE;
*/
  if (totalTimerNum > TimerListEntriesPerScreen)
       TimerListIndexTop = TimerListIndex;
  else
  {
    if (bTimerModifyInProgress)
        bTimerModifyInProgress = FALSE;
    else
        TimerListIndexTop = max (0, TimerListIndexMax - TimerListEntriesPerScreen + 1);
  }

  ListTimers (TRUE);
 }  // end of timer function ok
/*
 TimerFieldIndex = TimerFieldIndexStart;
 TimerCharIndex = 0;
 ShowEntryInsertionField ();
*/
}

void ShowNameEntryPosReset ()
{

 ShowNameEntryPos ();
 NameEntryKeyPressCount = 0;
 bNameEntryKeyCheckMax = TRUE;
 bNameEntryKeyCheckPrev = FALSE;
}

bool ProcessNameEntryKey (dword keycode)
{
 char keychar;
 int keyindex, i;

// char msg[NAME_ENTRY_BUFFER_SIZE + 11];

 bool bLowerCase;

 if (keycode >= RKEY_0 && keycode <= RKEY_9)
 {
// count number of presses of numeric key to translate into alpha, etc.
//  if (NameEntryKeyPressCount == 0)

  if (bNameEntryKeyCheckPrev)
  {
   if (keycode != NameEntryPrevKeyCode)
   {
    NameEntryPrevKeyCode = keycode;

    NameEntryKeyPressCount = 0;
    if (NameEntryBufferIndex == NameEntryBufferIndexMax)
    {
     if (NameEntryBufferIndex < NAME_ENTRY_BUFFER_SIZE - 2)
      NameEntryBufferIndex++;
    }
    bNameEntryKeyCheckMax = TRUE;
   }
  }
  else
  {
   bNameEntryKeyCheckPrev = TRUE;
   NameEntryPrevKeyCode = keycode;
  }
  if (bNameEntryKeyCheckMax)
  {
   bNameEntryKeyCheckMax = FALSE;
//   if (NameEntryBufferIndex != NameEntryBufferIndexMax)
   if (bNameEntryInsertMode)

   {
    if (NameEntryBufferIndex <= NameEntryBufferIndexMax)
    {

// inserting char not at end of current buffer, move following chars right
     for (i = NameEntryBufferIndexMax; i >= NameEntryBufferIndex; i--)
     {
      if (i < NAME_ENTRY_BUFFER_SIZE - 2)
       NameEntryBuffer[i+1] = NameEntryBuffer[i];
     }
    }
    if (NameEntryBufferIndexMax < NAME_ENTRY_BUFFER_SIZE - 2)
     NameEntryBufferIndexMax++;
   }
   else if (NameEntryBufferIndex > NameEntryBufferIndexMax)
   {
    if (NameEntryBufferIndexMax < NAME_ENTRY_BUFFER_SIZE - 2)
     NameEntryBufferIndexMax++;
   }
  }
//  if (bNameEntryLowerCase)
  switch (NameEntryCase)
  {
  case 0:
   {
// word case, i.e. upper if at start of buffer or previous char is space, else lower

    bLowerCase = FALSE;
    if (NameEntryBufferIndex > 0)
    {
// exception for apostrophe at start of buffer (generated automatically for timer file name). next char should
// be upper case.
     if ((NameEntryBufferIndex == 1) && (NameEntryBuffer[0] == PrefixFileNameTimer))
      break;
     if (NameEntryBuffer[NameEntryBufferIndex - 1] != 32)
      bLowerCase = TRUE;
    }
    break;
   }
  case 1: bLowerCase = FALSE; break;
  case 2: bLowerCase = TRUE; break;

  }
  keyindex = keycode - RKEY_0;
  keychar = KeyCharArray[keyindex][NameEntryKeyPressCount];
  if (bLowerCase)
  {
   if (keychar >= 65 && keychar <= 90)
    keychar += 32;

  }

  NameEntryBuffer[NameEntryBufferIndex] = keychar;
  NameEntryBuffer[NameEntryBufferIndexMax+1] = '\0';
//  strcpy (msg, FileNameEntryPrefix);

//  strcat (msg, NameEntryBuffer);
//  ShowNameEntry (msg);

  ShowNameEntry (NameEntryBuffer);
  ShowNameEntryPos ();
  NameEntryKeyPressCount++;
  if (NameEntryKeyPressCount >= strlen (KeyCharArray[keyindex]))
   NameEntryKeyPressCount = 0;
 }

 else


 switch (keycode)
 {
//  case RKEY_Forward:
  case RKEY_VolUp:
  {
// move to next char in name buffer
//   if (NameEntryBufferIndexMax < NAME_ENTRY_BUFFER_SIZE - 2)
   if (NameEntryBufferIndex <= NameEntryBufferIndexMax)
   {
//    NameEntryBufferIndexMax++;
    HideNameEntryPos ();
    NameEntryBufferIndex++;
    ShowNameEntryPosReset ();
   }
   break;
  }
//  case RKEY_Rewind:
  case RKEY_VolDown:
  {
// move to previous char in name buffer
   if (NameEntryBufferIndex > 0)
   {
    HideNameEntryPos ();
    NameEntryBufferIndex--;
/*
    ShowNameEntryPos ();
    NameEntryKeyPressCount = 0;
    bNameEntryKeyCheckMax = TRUE;
    bNameEntryKeyCheckPrev = FALSE;
*/
    ShowNameEntryPosReset ();
   }
   break;
  }
/*
  case RKEY_ChUp:
  {
// move to past last char in bufffer
   HideNameEntryPos ();
   NameEntryBufferIndex = NameEntryBufferIndexMax + 1;
   ShowNameEntryPosReset ();
   break;
  }
  case RKEY_ChDown:


  {
// move to first char in buffer

   HideNameEntryPos ();
   NameEntryBufferIndex = 0;
   ShowNameEntryPosReset ();
   break;

  }
*/
  case RKEY_Menu:
  {
// toggle upper/lower case for following chars
//   bNameEntryLowerCase = !bNameEntryLowerCase;
   NameEntryCase = (NameEntryCase + 1) % 3;
   ShowNameEntryPos ();
   break;
  }
  case RKEY_NewF1:
  {
// red key toggles between insert and overtype mode
   HideNameEntryPos ();
   bNameEntryInsertMode = !bNameEntryInsertMode;
   ShowNameEntryPos ();
   break;
  }
//  case RKEY_Exit:
  case RKEY_Ab:
  {

// white key deletes current char (to right of marker)

   if (NameEntryBufferIndex <= NameEntryBufferIndexMax)

   {
    if (NameEntryBufferIndex < NameEntryBufferIndexMax)
    {
// not deleting last char, move following chars one position to left
     for (i = NameEntryBufferIndex; i < NameEntryBufferIndexMax; i++)
      NameEntryBuffer[i] = NameEntryBuffer[i+1];
    }
//    NameEntryBufferIndex--;
    if (NameEntryBufferIndexMax > -1)

     NameEntryBufferIndexMax--;

    NameEntryBuffer[NameEntryBufferIndexMax+1] = '\0';
    ShowNameEntry (NameEntryBuffer);
/*
    ShowNameEntryPos ();
    NameEntryKeyPressCount = 0;
    bNameEntryKeyCheckMax = TRUE;
    bNameEntryKeyCheckPrev = FALSE;
*/
    ShowNameEntryPosReset ();
   }
   break;
  }

  case RKEY_Recall:
  {
// backspace to prev char in name buffer (erasing current one)
   if (NameEntryBufferIndex > 0)
   {
//    NameEntryBuffer[NameEntryBufferIndex] = 0x20;
//    NameEntryBuffer[NameEntryBufferIndexMax+1] = '\0';
//    HideNameEntryPos ();
//    NameEntryBufferIndex--;
//    if (NameEntryBufferIndex != NameEntryBufferIndexMax)
    if (NameEntryBufferIndex <= NameEntryBufferIndexMax)


    {
// not backspacing over last char, move following chars one position to left

     if (NameEntryBufferIndex > 0)
     {
      for (i = NameEntryBufferIndex; i <= NameEntryBufferIndexMax; i++)
       NameEntryBuffer[i-1] = NameEntryBuffer[i];
     }
    }
//    if (NameEntryBufferIndexMax >= 0)
/*
    if (NameEntryBufferIndexMax == 0)

     NameEntryBufferIndexMax = -1;
    else
*/


//    if (NameEntryBufferIndex > 0)
    NameEntryBufferIndex--;
    if (NameEntryBufferIndexMax > -1)
     NameEntryBufferIndexMax--;

    NameEntryBuffer[NameEntryBufferIndexMax+1] = '\0';
//    strcpy (msg, FileNameEntryPrefix);
//    strcat (msg, NameEntryBuffer);
//    ShowNameEntry (msg);
    ShowNameEntry (NameEntryBuffer);

/*
    ShowNameEntryPos ();

    NameEntryKeyPressCount = 0;
    bNameEntryKeyCheckMax = TRUE;

    bNameEntryKeyCheckPrev = FALSE;
*/
    ShowNameEntryPosReset ();
   }
   break;
  }
  default:


  {
// return false if it's a key we don't process here

   return FALSE;
  }
 }
 return TRUE; 
}

void ShowFileEventText (char *str1)
{
// display ts file event text in file display area
// adapted from Tony's infodisplay7

// static char str1[160];
 char str[130];
 int n,p=0,m=0;
 int finished=0;

// sprintf(str1,"description: %.128s",eventinfo[currentEvtNumber].description);
 //sprintf(str1,"description: This is a big description, I am-going-to-put-some hyphens-into this for-testing-This-is-a-big-description, I am-going-to-put-some hyphens-into this for-testing2");
 strcpy(str,"");  //have to reset string
// for (n=0; n<3; n++)   
 for (n=0; n<7; n++)   
 {
//  while (TAP_Osd_GetW(str,0,1)<(580-120))    //while the width of the text is less than a lines length
  while (TAP_Osd_GetW (str , 0, FNT_Size_FileDisplay) < (FileWinXMarR - FileWinXMarL))    //while the width of the text is less than a lines length
  {
   if (m>strlen(str1))   //if the counter is larger than the stringlength of the orig string, we have really finished here 
   {
    finished=1;     
    break;
   }
   strncat(str,str1+m,1); //copy 1 char from str1 at a time.
   m=m+1;                 //get next char
  }
   if (!finished){
   m=m-1; //reduce m by 1 as it is one too much from last while statement.
   while (!isspace(str1[m])&&!(str1[m]=='-')&&(m>0))  {   //keep reducing m until a space or hyphen is found (or m underflows).

    m=m-1;

   }
   m=m+1; //add 1 to keep hyphen on right line
  }
  str[m-p]='\0';   //terminate str early (at m-p).

//  TAP_Osd_PutS(windownr,120, 353+18*n, 580, str,COLOR_WINDOWTEXT, COLOR_None, 0, 1, 0,0);  //put multiple lines down if required.

  TAP_Osd_PutS(rgnFile, FileWinXMarL, FileHeadH + (FileDisplayYincrement * (n + 2)), FileWinXMarR, str, COLOR_ForeFileDisplay, COLOR_None, 0, FNT_Size_FileDisplay, 0, ALIGN_LEFT);  //put multiple lines down if required.
// 22jun05 shift down one line to make way for event start amd end times
//  TAP_Osd_PutS(rgnFile, FileWinXMarL, FileHeadH + (FileDisplayYincrement * (n + 3)), FileWinXMarR, str, COLOR_ForeFileDisplay, COLOR_None, 0, FNT_Size_FileDisplay, 0, ALIGN_LEFT);  //put multiple lines down if required.
  p=m;   //store current position
  strcpy(str," ");  //have to reset string, indent looks nice while we are here.
 }    
}

// bool GetEpgEventName (int svctype, int svcnum, bool next, char *name)
bool GetEpgEventName (int recslot, bool next, char *name)
{

    int evtNum, currentEvtNumber, j, desiredEvtNumber;
    bool retcode, bFoundCurrentEvent;

    retcode = FALSE;

//            evtInfo = 0;
    evtInfo = TAP_GetEvent(RecInfo[recslot].svcType, RecInfo[recslot].svcNum, &evtNum);
//            evtInfo = TAP_GetCurrentEvent(RecInfo[i].svcType, RecInfo[i].svcNum);
//              memcpy (&evtInfo, TAP_GetCurrentEvent(RecInfo[i].svcType, RecInfo[i].svcNum), sizeof evtInfo);
//            ShowFileErrorMsg (_evtInfo->eventName, FALSE);
    if (evtInfo)
    {
        bFoundCurrentEvent = FALSE;
        for( j = 0; j < evtNum; j++ )  //finding current event
        {
            if( evtInfo[j].runningStatus == 4 )   //runningstatus=4 looks like this equates to the current program showing.
            {
                bFoundCurrentEvent = TRUE;
                currentEvtNumber = j;
                break;
            }
        }
        if (bFoundCurrentEvent)
        {

// look for now or next event (current event number + 1, I hope)

//            if (evtInfo)
//            {
            if (next)
                desiredEvtNumber = currentEvtNumber + 1;
            else
                desiredEvtNumber = currentEvtNumber;
            if (evtNum >= desiredEvtNumber + 1)
            {
// event name can be longer than ts file name size, so make sure we don't overflow the latter
// truncate if necessary, to allow for two prefix chars and .rec on end
                for (j = 0; j <= TS_FILE_NAME_SIZE - 7; j++)
                {
//                     if (evtInfo->eventName[j-2] == '\0')
                 if (evtInfo[desiredEvtNumber].eventName[j] == '\0')
                  break;
//                     RecAutoRenameNew[j] = evtInfo->eventName[j-2];
                 name[j] = evtInfo[desiredEvtNumber].eventName[j];
                }
                name[j] = '\0';
                if (strlen (name) > 0)
                {
                    retcode = TRUE;
// save event name and description for updating file header when recording ends
// 23may05 these are now temporary until we decide whether the file is actually renamed
                    strcpy (RecEventName[recslot], evtInfo[desiredEvtNumber].eventName);
                    strcpy (RecEventDesc[recslot], evtInfo[desiredEvtNumber].description);
                    RecEventStartTime[recslot] = evtInfo[desiredEvtNumber].startTime;
                    RecEventEndTime[recslot] = evtInfo[desiredEvtNumber].endTime;
// 23may05 defer this until we actually rename the file, else we can update header with wrong event
//                        bRecEventInfoSaved[recslot] = TRUE;
                }
            }   // end of sufficient events returned to include desired one
        }   // end found current event
//            }
//            if (evtInfo)
//            {
        TAP_MemFree(evtInfo);
        evtInfo = 0;
//            }
    }   // end of getcurrentevent actually returned a pointer

    if (!retcode)
    {
        strcpy (name, "[Event name not found]");
// if we did not get the event info, clear out the temporary saved strings
        strcpy (RecEventName[recslot], "");
        strcpy (RecEventDesc[recslot], "");
        RecEventStartTime[recslot] = 0;
        RecEventEndTime[recslot] = 0;
    }
    return retcode;
}

// dword TimeAdjust (dword time, int minutes)
// void TimeAdjust (word mjd, byte hh, byte mm, byte ss, long adjsec)
void TimeAdjust (word *mjd, byte *hh, byte *mm, byte *ss, long adjsec)
{
//    word mjd;
    int mins;
    long secs;
/*
// adjust date/time (in format mjd, hhmm) by specified number of minutes (no more than 24 hours either way)
    mjd = (time >> 16) & 0xFFFF;
    mins = ((time & 0xFF00) >> 8) * 60 + (time & 0xFF);
    mins += minutes;
    if (mins >= 1440)
    {
        mjd ++;
        mins -= 1440;
    }
    else if (mins < 0)
    {
        mjd --;
        mins += 1440;
    }
    return (mjd << 16) | (mins / 60) << 8 | (mins % 60);
*/
// adjust date/time by specified number of seconds (no more than 24 hours either way)
    secs = (*hh * 3600) + (*mm * 60) + *ss + adjsec;
    if (secs >= 86400)
    {
//        *mjd ++;
        *mjd = *mjd + 1;
        secs -= 86400;
    }
    else if (secs < 0)
    {
//        *mjd --;
        *mjd = *mjd - 1;
        secs += 86400;
    }
    mins = secs / 60;
    *hh = mins / 60;
    *mm = mins % 60;
    *ss = secs % 60;
}

void ShowTimedStopRecEntry ()
{
    char msg[40];
    
    strcpy (msg, "Stop recording at ");
    strcat (msg, TimedStopRecEntry);
    strcat (msg, " then sleep");
    ShowFileErrorMsg (msg, TRUE);
}

void HideTimedStopRecEntry ()
{
    HideFileErrorMsg ();
    bTimedStopRecEntryMode = FALSE;
}

void StartSubChannel (int chnum)
{
// called from event handler to start specified subchannel if safe to do so
    if (bPlaying && bRecording[0] && bRecording[1])
        ShowFileErrorMsg ("Start subchannel disallowed (dual recording while playing back)", FALSE);
    else
        TAP_Channel_Start(CHANNEL_Sub, 0, chnum);
}

bool PlaybackNumericToMinutes (int *minutes)
{
    int mm;
// convert playback numeric key string from hours and minutes (h:mm) to minutes
    mm = (PlaybackNumericKeyString[1] - 0x30) * 10 + (PlaybackNumericKeyString[2] - 0x30);
    if (mm > 59)
    {
        ShowFileErrorMsg ("Invalid time value for playback jump", FALSE);
        return FALSE;
    }
    else
    {
        *minutes = (PlaybackNumericKeyString[0] - 0x30) * 60 + mm;
        return TRUE;
    }
}

#include "ViewMaster3EventHandler.c"

int TAP_Main (void)
{
 int i, x;

 ShowMessageWin("Starting",_PROGRAM_NAME_); // show user TAP has started

  // initialize the global function pointer (only once) 
  getTuner = (GetTuner)initCodeWrapper(); 

  if(getTuner == NULL) 
  { 
    ShowMessageWin ("Code wrapper initialization failed", ""); 
//    return 0; 
  }
/*
 g_nmemfound=searchRAM();

 if (g_nmemfound==0) return 0;
*/

/*
 if (!(g_timerPointer=(TYPE_TimerRAM *)searchRAM())) 
  return 0;  // 0 means return/quit TAP, 1 means stay TSR
*/
 position[0][0] = pipxoff;
 position[0][1] = xmax - pipw - pipxoff;
 position[0][2] = xmax - pipw - pipxoff;
 position[0][3] = pipxoff;
 position[1][0] = pipyoff;
 position[1][1] = pipyoff;
 position[1][2] = ymax - piph - pipyoff;
 position[1][3] = ymax - piph - pipyoff;

//  HideSubPicture();
// ClockW = TAP_Osd_GetW (strClockFormat, 0, FNT_Size_Clock) + (2 * ClockXoff);

// ClockX = (xmax - ClockW) / 2;

 totalsize = TAP_Hdd_TotalSize();

// ChangeToRootFolder (2);
// FileListPathLevel = 0;
// FileListPathLevel = 2;
 strcpy (FileListPath[0], "");
 ChangeToDataFilesFolder ();
 strcpy (FileListNamePlay, "");
 strcpy (FileListNamePlayPrev, "");
 strcpy (NameEntryBuffer, "");
 strcpy (TimedStopRecEntry, "");
 for (i = 0;  i <= 1; i++)
 {
  strcpy (FileListNameRec[i], "");
  strcpy (FileListNameRecPrev[i], "");
  strcpy (RecFileNameUpdate[i], "");
  bFileListNameChangedRec[i] = FALSE;
  bTryRenameLater[i] = FALSE;
  RecTuner[i] = 0;
  TickRecStopped[i] = 0;
  RecordedSec[i] = 0;
  RecDuration[i] = 0;
  RecStartMjd[i] = 0;
  RecStartHour[i] = 0;
  RecStartMin[i] = 0;
  RecStartSec[i] = 0;
/*
  RecStartMjdSaved[i] = 0;
  RecStartHourSaved[i] = 0;
  RecStartMinSaved[i] = 0;
  RecStartSecSaved[i] = 0;
*/
 }
 for (i = 0;  i <= 19; i++)
 {
  FileListDurationArray[i] = -1;
  FileListDurationSecArray[i] = -1;
  FileListLcnArray[i] = -1;
  FileListStartHourArray[i] = -1;
  FileListStartMinArray[i] = -1;
  FileListStartSecArray[i] = -1;
 }
 TAP_GetState (&PrevState, &PrevSubState);

 strcpy (TimerEntryFieldName[0], "Mode:");
 strcpy (TimerEntryFieldName[1], "Date:");
 strcpy (TimerEntryFieldName[2], "Start:");
 strcpy (TimerEntryFieldName[3], "End:");
 strcpy (TimerEntryFieldName[4], "Chan:");
 strcpy (TimerEntryFieldName[5], "Tuner:");
// strcpy (TimerEntryFieldName[6], "F:");
 for (i = NumTimerEntryFields - 1; i >= 0; i--)
 {
// calculate starting x coords and widths of entry data fields
  TimerEntryDataXoffset[i] = TimerEntryXoffset[i] + TAP_Osd_GetW (TimerEntryFieldName[i], 0, FNT_Size_TimerEntry) + TimerEntryFieldXGap;
  if (i == NumTimerEntryFields - 1)
   x = TimerWinW; // x coord of end of data field
  else
//   x = TimerEntryXoffset[i+1] - 20;
   x = TimerEntryXoffset[i+1] - TimerEntryFieldXGap;

  TimerEntryDataW[i] = x - TimerEntryDataXoffset[i] + 1;
 }
// TimerListEntriesPerScreen = (TimerErrorYoffset - TimerHeadH) / TimerListYincrement;
// TimerListEntriesPerScreen = 5;

 TimerListIndexMax = TAP_Timer_GetTotalNum() - 1;
// ModifyTimerRAM should now make the following unnecessary
// re-instated 06jul04 since it appears that it may not change channel on boot up for timer rec, even with
// extra rf info set in timer entry (does appear to change channel ok if already running).
// CheckTimerChannelChange ();
 TickProgBarSaved = TAP_GetTick ();
 TickRecAutoRenameSaved = TAP_GetTick ();

 return 1; // we're starting in TSR-mode

}

