/************************************************************
				Part of the ukEPG project
	This module handles the definition of all common items

Name	: Common.c
Author	: Darkmatter
Version	: 0.5
For	: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	:
v0.0 Darkmatter:	04-07-05	Inception date
v0.1 sl8:		20-11-05	Modified for Auto Scheduler
v0.2 sl8:		20-01-06	Modified for TAP_SDK. All variables initialised
v0.3 sl8:		06-02-06	Added UK Project directory define
v0.4 sl8:		15-02-06	Modified to allow for 'Perform Search' config option
v0.5 sl8:		09-03-06	Folder, Remote and log archive modifications

************************************************************/

//-----------------
// Screen coordinates
//
//  Max X screen width  720
//  Max Y screen hegith 576
#define MAX_SCREEN_X  720
#define MAX_SCREEN_Y  576


//-----------------
// Dependant files
//
#include "Menu.inc"
#include "RemoteKeys.inc"


//-----------------
// Common Graphics
//
#include "gif.h"

#ifndef WIN32
	#include "graphics/rowA.gd"
	#include "graphics/rowB.gd"
	#include "graphics/rowAdarkblue.gd"
	#include "graphics/highlight.gd"
	#include "graphics/highlightCellBig.gd"
	#include "graphics/highlightCellMed.gd"
	#include "graphics/highlightCellSma.gd"
	#include "graphics/redcircle.gd"
	#include "graphics/greencircle.gd"
	#include "graphics/top.gd"
	#include "graphics/side.gd"
	#include "graphics/timeBar.gd"
	#include "graphics/smallGreenBar.gd"
	#include "graphics/editCellMed.gd"
	#include "graphics/MenuRow.gd"
	#include "graphics/MenuRow_Highlight.gd"
	#include "graphics/Menu_Title.gd"
	#include "graphics/PopUp466x406.gd"
	#include "graphics/PopUp476x416.gd"
	#include "graphics/PopUp360x180.gd"
	#include "graphics/redoval.gd"
	#include "graphics/greenoval.gd"
	#include "graphics/BigKeyGreen.gd"
	#include "graphics/BigKeyBlue.gd"
	#include "graphics/InfoOval38x19.gd"
	#include "graphics/redoval38x19.gd"
	#include "graphics/InfoOval44x22.gd"
	#include "graphics/DateHighlight.gd"
	#include "graphics/DateOverStamp.gd"
#else
	#include "graphics/win32/graphics.inc"
#endif


#define PROJECT_DIRECTORY "UK TAP Project"


//#define FILL_COLOUR RGB(0,0,102)
#define FILL_COLOUR COLOR_Black
#define MAIN_TEXT_COLOUR RGB(29,29,29)
#define HEADING_TEXT_COLOUR RGB8888(0,152,192)
#define TITLE_COLOUR RGB(18,18,18)
#define INFO_COLOUR RGB(18,18,18)
#define TIME_COLOUR RGB(4,4,4)
#define CALENDAR_BACKGROUND_COLOUR COLOR_User9
#define CALENDAR_DAY_TEXT MAIN_TEXT_COLOUR
//#define COLOR_White RGB(31,31,31)
//#define COLOR_OffWhite RGB(29,29,29)
//#define COLOR_LightGray RGB(24,24,24)
//#define COLOR_Gray RGB(16,16,16)

#define LOG_BUFFER_SIZE		512

#define	MJD_OFFSET	51544		// 01/01/2000


//*****************
// Prototypes
//*****************
//
void GenerateExitRequest( void );				// UkTimers.c
void DrawGraphicBoarders(void);

void MakeNewTimer( byte chosenWeekDay );		// TimerEdit.c
int ActivateTimerSaveRestore( void );			// TimerSaveRestore.c
void ReadTimerFile( void );
void GenerateTimerFile( void );

void ActivateMenu(void);						// MainMenu.c
void RedrawMainMenu( void );
void TerminateMenu( void );

void DisplayConfigWindow( void );				// ConfigMenu.c

void FormatNameString( char *source, int *index, char *dest, int width);		// logo.c

void SaveConfigurationToFile( void );			// IniFile.c

byte schInitRetreiveData(void);
byte schInitRetreiveRemoteData(void);
void schWriteSearchList(void);
void schWriteMoveList(void);

bool SearchEdit (int, int);

void logInitialise(void);
void logStoreEvent(char*);
void logArchive(void);

struct _reent *_impure_ptr; 					// need this declared for reentrant functions in ANSI C library [comment by Sunstealer]



//*****************
//global variables
//*****************
//
static dword mainActivationKey = 0;
static TYPE_ModelType unitModelType;

static bool exitFlag = 0;
static bool terminateFlag = 0;
static word rgn = 0;														// one region used for all our graphics

static bool schEditWindowShowing = 0;
static bool channelListWindowShowing = 0;
static bool calendarWindowShowing = 0;
static bool keyboardHelpWindowShowing = 0;
static bool timeEditExitWindowShowing = 0;
static bool creditsShowing = 0;
static bool menuShowing = 0;
static bool configWindowShowing = 0;
static bool keyboardWindowShowing = 0;

static bool returnFromEdit = 0;

static byte schTimeHour = 0, schTimeMin = 0, schTimeSec = 0;
static word schTimeMjd = MJD_OFFSET;

static byte schServiceSV = SCH_SERVICE_INITIALISE;
static byte schMoveServiceSV = SCH_MOVE_SERVICE_INITIALISE;

static byte schStartUpCounter = 0;

static bool FirmwareCallsEnabled = FALSE;
static bool TAP_Hdd_Move_Available = FALSE;

enum
{
	SCH_EXISTING_SEARCH = 0,
	SCH_NEW_SEARCH
};
