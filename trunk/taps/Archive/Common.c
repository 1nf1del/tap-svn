/************************************************************
				Part of the ukEPG project
	This module handles the definition of all common items
 
Name	: Common.c
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 04-07-05	Inception date

	Last change:  USE   2 Aug 105   11:30 pm
************************************************************/
  
//-----------------
// Screen coordinates
//
//  Max X screen width  720
//  Max Y screen hegith 576
#define MAX_SCREEN_X  720
#define MAX_SCREEN_Y  576

#define COLUMN_GAP_W       3   // Width of gap between columns.

// Dimensions of Information Window
#define BOTTOM_OF_ROWS ( (NUMBER_OF_LINES*Y1_STEP) + Y1_OFFSET - 8 + 39 )  // 487 or 445
#define INFO_AREA_X 53
#define INFO_AREA_Y (BOTTOM_OF_ROWS + COLUMN_GAP_W) // 490 or 448
#define INFO_AREA_W 614  // Total width of list window.
//#define INFO_AREA_H (MAX_SCREEN_Y-INFO_AREA_Y)
#define INFO_AREA_H 83 //86

// Dimensions for instructions on Info Window
#define INSTR_AREA_W 150
#define INSTR_AREA_X (INFO_AREA_X+INFO_AREA_W-INSTR_AREA_W)
#define INSTR_AREA_Y INFO_AREA_Y
#define INSTR_AREA_H INFO_AREA_H

// Coordinates for the text in the Information Window
#define INFO_TEXT_X (INFO_AREA_X + 5)
#define INFO_TEXT_Y (INFO_AREA_Y + 3)
#define INFO_TEXT_W (INFO_AREA_W - INSTR_AREA_W)  //666
#define INFO_TEXT_H 25  // Height of a single text row.

// x-Coordinates of the column seperators
#define COLUMN1_X 85
#define COLUMN2_X 372
#define COLUMN3_X 496
#define COLUMN4_X 604

// Coordinates for line and time bar
#define CLOCK_X 420
#define CLOCK_Y 31
#define CLOCK_W 250
#define CLOCK_H 35

// Define the height of the top graphic component.
#define TOP_H 71

// Define the tags to go at the end of the filenames for each of the views - standard file view, and recycle bin view.
#define REC_STRING ".rec"
#define RECYCLED_STRING ".rec.del"

/*
// x-Coordinates of the column field contents
#define FILE_ICON_X 50
#define FILE_NUMBER_X 63
#define FILE_NAME_X 93
#define FILE_NAME_END_X 370
#define TIME_START_X 375
#define TIME_END_X 495
#define DATE_START_X 499
#define DATE_END_X 602
#define LOGO_X 607
*/

#define COLUMN1_START      50
static  int column1Width=35;
#define COLUMN1_END        (COLUMN1_START + column1Width)

// FILENAME 
#define COLUMN2_START      (COLUMN1_END + 1)  //85
#define COLUMN2_TEXT_START (COLUMN2_START+COLUMN_GAP_W+5)
#define COLUMN2_DEFAULT_W  286
#define COLUMN2_OPTION1_W  (COLUMN2_DEFAULT_W)
#define COLUMN2_OPTION2_W  (COLUMN2_DEFAULT_W)          // Used with Column4 Option 2
#define COLUMN2_OPTION3_W  (COLUMN2_DEFAULT_W + 50)     // Used with Column4 Option 3
static  int column2Width=COLUMN2_DEFAULT_W;
#define COLUMN2_END        (COLUMN2_START + column2Width)

// DATE
#define COLUMN3_START      (COLUMN2_END + 1)
#define COLUMN3_TEXT_START (COLUMN3_START+COLUMN_GAP_W)
#define COLUMN3_DEFAULT_W  107
#define COLUMN3_OPTION1_W  (COLUMN3_DEFAULT_W)        // DDD # MMM
#define COLUMN3_OPTION2_W  (COLUMN3_DEFAULT_W)        // DDD x wks ago
#define COLUMN3_OPTION3_W  (COLUMN3_DEFAULT_W)        
static  int column3Width=COLUMN3_DEFAULT_W;
#define COLUMN3_END        (COLUMN3_START + column3Width)   // 108

// TIME & DURATION
#define COLUMN4_START      (COLUMN3_END + 1)
#define COLUMN4_TEXT_START (COLUMN4_START+COLUMN_GAP_W)
#define COLUMN4_DEFAULT_W  123
#define COLUMN4_OPTION1_W  (COLUMN4_DEFAULT_W)        // HH:M (dur)
#define COLUMN4_OPTION2_W  (COLUMN4_DEFAULT_W)        // HH:MM ~ HH:MM
#define COLUMN4_OPTION3_W  (COLUMN4_DEFAULT_W - 50)   // dur
static  int column4Width=COLUMN4_DEFAULT_W;
#define COLUMN4_END        (COLUMN4_START + column4Width)    // 124

// LOGO & SIZE
#define COLUMN5_START      (COLUMN4_END + 1)
#define COLUMN5_TEXT_START (COLUMN5_START+COLUMN_GAP_W)
#define COLUMN5_DEFAULT_W  62
static  int column5Width=COLUMN5_DEFAULT_W;
#define COLUMN5_END        (COLUMN5_START + column5Width)

// Coordindates for the disk information bar and text
#define DISK_INFO_X INFO_TEXT_X
#define DISK_INFO_Y (INFO_AREA_Y+55)
#define DISK_PROGRESS_BAR_WIDTH 200  // Width of the Progress Bar for the disk space.

#define MAX_FULL_DIR_NAME_LENGTH 200 // Define the maximum length of the full directory name.


// Define the numbers of the options on the  Archive Info window.
#define INFO_OK_OPTION 0
#define INFO_DELETE_OPTION 1
#define INFO_RENAME_OPTION 2

// Define the numbers for the different sort options.
#define SORT_DATE_OPTION   0
#define SORT_NAME_OPTION   1
#define SORT_SVCNUM_OPTION 2
#define SORT_SIZE_OPTION   3

// Define the numbers for the different progress bar colour options.
#define PB_MULTI    0
#define PB_SINGLE   1
#define PB_SOLID    2
#define PB_REDGREEN 3
#define PB_WHITE    4

//-----------------
// Dependant files
//
#include "Menu.inc"
#include "RemoteKeys.inc"


//-----------------
// Common Graphics
//
#include "gif.h"
#include "graphics/rowA.GD"
#include "graphics/rowB.GD"
#include "graphics/rowE.GD"
//#include "graphics/rowC.GD"
//#include "graphics/row.GD"
#include "graphics/highlight.GD"

#include "graphics/top.GD"
#include "graphics/side.GD"
#include "graphics/bottom.GD"
#include "graphics/timeBar.GD"
#include "graphics/LineAndTimeBar250x35.GD"
#include "graphics/top_black.GD"
#include "graphics/side_black.GD"
#include "graphics/bottom_black.GD"
#include "graphics/timeBar_black.GD"

#include "graphics/smallGreenBar.GD"

#include "graphics/MenuRow.GD"
#include "graphics/MenuRow_Highlight.GD"
#include "graphics/Menu_Title.GD"

#include "graphics/popup476x416.GD"

#include "graphics/BigGreenButton.GD"
#include "graphics/BigBlueButton.GD"

#include "graphics/GreenGlassCircle25x25.GD"
#include "graphics/BlueGlassCircle25x25.GD"
#include "graphics/BlueGlassCircleTick25x25.GD"
#include "graphics/RedGlassCircle25x25.GD"
#include "graphics/newstar.GD"


#include "graphics/InfoOval38x19.GD"
#include "graphics/RedOval38x19.GD"
#include "graphics/WhiteOval38x19.GD"
#include "graphics/BlueOval38x19.GD"

#include "graphics/archive_help_uk.GD"
#include "graphics/archive_help_oz.GD"
#include "graphics/archive_recycle_help_uk.GD"
#include "graphics/archive_recycle_help_oz.GD"

 
#include "graphics/folder_yellow.GD"
#include "graphics/folder_yellow_parent.GD"
#include "graphics/trash_bin.GD"


//#define FILL_COLOUR RGB(0,0,102)
#define FILL_COLOUR COLOR_Black
//#define INFO_FILL_COLOUR COLOR_User3
#define MAIN_TEXT_COLOUR RGB(29,29,29)
//#define HEADING_TEXT_COLOUR RGB8888(90,104,213)
#define HEADING_TEXT_COLOUR RGB8888(0,152,192)
//#define INFO_FILL_COLOUR RGB(34,34,151)
#define INFO_FILL_COLOUR COLOR_User3
#define POPUP_FILL_COLOUR RGB8888(34,34,151)
//#define INFO_FILL_COLOUR RGB8888(48,72,111)
#define TITLE_COLOUR RGB(18,18,18)
//#define INFO_COLOUR RGB(18,18,18)
//#define INFO_COLOUR RGB(29,29,29)
#define INFO_COLOUR COLOR_White

#define TIME_COLOUR RGB(4,4,4)
//#define TIME_COLOUR RGB(29,29,29)
#define CALENDAR_BACKGROUND_COLOUR COLOR_User9
#define CALENDAR_DAY_TEXT TIME_COLOUR
//#define COLOR_White RGB(31,31,31)
//#define COLOR_OffWhite RGB(29,29,29)
//#define COLOR_LightGray RGB(24,24,24)
//#define COLOR_Gray RGB(16,16,16)



//*****************
// Prototypes
//*****************
//

void CloseArchiveInfoWindow(void);            // ArchiveRecycle.c
void DeleteAction(int type);
void DisplayArchiveMoveFolderList(void);
void CloseArchiveMoveWindow(void);


void ReadGmtOffset();

void ActivateMoveWindow(void);                // ArchiveMove.c
void populateMoveFileList(void);
void MoveAFile(char* sourceDir, char* destDir, char* sourceFile, dword attr);

void GenerateExitRequest( void );				// UkTimers.c
void DrawGraphicBoarders(void);

int ActivateTimerSaveRestore( void );			// TimerSaveRestore.c
void ReadTimerFile( void );
void GenerateTimerFile( void );

void ActivateMenu(void);						// MainMenu.c
void RedrawMainMenu( void );
void TerminateMenu( void );

void DisplayConfigWindow( void );				// ConfigMenu.c

void FormatNameString( char *source, int *index, char *dest, int width);		// logo.c

void SaveConfigurationToFile( void );			// IniFile.c

struct _reent *_impure_ptr; 					// need this declared for reentrant functions in ANSI C library [comment by Sunstealer]

void ArchiveAction (int line);
void SortList(int sortOrder);
void ActivateDeleteWindow(char* filename, dword attr);
void ActivateStopWindow(char* filename);
void ActivateInfoWindow(void);
void RefreshArchiveList( bool reposition );
void DisplayInfoLine(void);
void DisplayArchiveInfoWindow(void);
void CreateNewFolder(void);
void ChangeToParentDir(void);
int StartPlayback(char filename[TS_FILE_NAME_SIZE], int jump);
void WeekdayToAlpha (byte weekday, char *str);
void RestartPlayback(int line, int jump);
void DeleteProgressInfo(int dirNumbr, int index, bool message);
void ShowMessageBox( word msgRgn, char* msg1, char* msg2, char* msg3);
void DetermineStartingLine(int *line);


//*****************
//global variables
//*****************
//

static dword mainActivationKey;
static Model unitModelType;

static bool exitFlag;
static bool terminateFlag;
static word rgn;														// one region used for all our graphics
static word memRgn;														// one memory region used for all our graphics
static word listRgn;												    // a memory region used for all our graphics
static word clockRgn;												    // a memory region used for the line count and clock.

static int chosenLine, printLine;

static bool tapStartup;                       // Flag to indicate if we are just starting up the TAP.
static bool initialLoad;                      // Flag to indicate if we are doing a full, initial load of the Archive data.

// Variables showing which windows are currently displayed.
static bool editWindowShowing;
static bool deleteWindowShowing;
static bool infoWindowShowing;
static bool renameWindowShowing;
static bool keyboardWindowShowing;
static bool keyboardHelpWindowShowing;
static bool archiveHelpWindowShowing;
static bool timeEditExitWindowShowing;
static bool stopWindowShowing;
static bool creditsShowing;
static bool menuShowing;
static bool configWindowShowing;
static bool msgWindowShowing;
static bool recycleBinWindowShowing;
static bool moveWindowShowing;

static bool recycleWindowMode;      // Flag to indicate if we are in the Recycle Bin mode.

static int  numberOfFiles;
static int  numberOfFolders;
static int  numberOfPlayedFiles;
static bool playinfoChanged;
static dword playbackInfoTick;           // Keep track of when we last read/wrote the playback information.
static int maxShown;
static int  sortOrder;
static int  folderSortOrder = 0;
static char sortTitle[20];
static char CurrentDir[ MAX_FULL_DIR_NAME_LENGTH ];
static int CurrentDirNumber;       
static char  infoCommandOption;
static int recordingRateOption;
static int GMToffsetOption;
static int sortOrderOption;
static int progressBarOption;
static int column1Option;
static int column2Option;
static int column3Option;
static int column4Option;
static int column5Option;
static int infoLineOption;
static int numberLinesOption;
static bool recursiveLoadOption=FALSE;
static int borderOption;
static int recCheckOption;
static int yOffsetOption=36;
static int okPlayOption;
static int folderDeleteOption;
static int extInfoFontOption;
static int recycleBinOption;
static int recycleBinCleanoutOption;
static int recycleBinThresholdOption;
static int fileListKeyOption;
static int splashScreenOption;
static int PBKgmtOffsetOption;
static int NewIndicatorOption;
static int RenameOnMoveOption;

static bool  listMoved;
static bool  returnFromInfo;
static bool  returnFromRecycleBinEmpty;         // Flag to indicate when we have returned from emptying the Recycle Bin.                
static bool  returnFromRecycleBinWindowEmpty;   // Flag to indicate when we have returned from emptying the Recycle Bin from the Recycle Bin Window.                

static bool  initialFileLoad;                   // Flag to indicate first initial file load.

       
static char gstr[500];
       
static bool CalledByTSRCommander=FALSE;
static bool inPlaybackMode = FALSE;
static bool generatedPlayList;

static int playbackOnScreenEntry=0;
static int playbackOnScreenLine=0;
static int recordingOnScreenEntry1=0;
static int recordingOnScreenLine1=0;
static int recordingOnScreenEntry2=0;
static int recordingOnScreenLine2=0;

#define ApiOverRunArray 128    // Allocate some space to allow for buffer overrun
#define ApiOverRunCheck 1      // Value to check to determine if buffer overrun

static TYPE_File    *CurrentPlaybackFile;                      // Place to store info about current playback file if in playback mode.
static TYPE_TapEvent CurrentPlaybackEvent;                     // Place to store info about current playback event if in playback mode.
static TYPE_PlayInfo CurrentPlaybackInfo;                      // Place to store info about current playback entry if in playback mode.
static char          sOverRun[ApiOverRunArray];                // Keep some space to protect against buffer overrun.
static int           iOverRunCheck;                            // If buffer overrun is larger than sOverRun iOverRunCheck will be hit.

static char tagStr[10];          // Holds the current tag at the end of the filename ".rec" when in normal file mode, ".rec.del" when in Recycle Bin mode
static int  tagLength;           // Length of the current tag at the end of the filename.                             


