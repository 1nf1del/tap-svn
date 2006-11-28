/************************************************************
				Part of the ukEPG project
	This module handles the definition of all common items

Name	: Common.c
Author	: Darkmatter
Version	: 0.9
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
v0.6 sl8		11-04-06	Show window added and tidy up.
v0.7 sl8		08-05-06	API move added.
v0.8 sl8		05-08-06	Keep added.
v0.9 sl8		28-08-06	Keyboard types.

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
	#include "graphics/rowA.GD"
	#include "graphics/rowB.GD"
	#include "graphics/rowAdarkblue.GD"
	#include "graphics/highlight.GD"
	#include "graphics/highlightCellBig.GD"
	#include "graphics/highlightCellMed.GD"
	#include "graphics/highlightCellSma.GD"
	#include "graphics/RedCircle.GD"
	#include "graphics/GreenCircle.GD"
	#include "graphics/top.GD"
	#include "graphics/side.GD"
	#include "graphics/timeBar.GD"
	#include "graphics/smallGreenBar.GD"
	#include "graphics/editCellMed.GD"
	#include "graphics/MenuRow.GD"
	#include "graphics/MenuRow_Highlight.GD"
	#include "graphics/Menu_Title.GD"
	#include "graphics/PopUp466x406.GD"
	#include "graphics/popup476x416.GD"
	#include "graphics/popup360x130.GD"
	#include "graphics/popup360x180.GD"
	#include "graphics/RedOval.GD"
	#include "graphics/GreenOval.GD"
	#include "graphics/BigKeyGreen.GD"
	#include "graphics/BigKeyBlue.GD"
	#include "graphics/InfoOval38x19.GD"
	#include "graphics/RedOval38x19.GD"
	#include "graphics/GreenOval38x19.GD"
	#include "graphics/YellowOval38x19.GD"
	#include "graphics/BlueOval38x19.GD"
	#include "graphics/InfoOval44x22.GD"
	#include "graphics/DateHighlight.GD"
	#include "graphics/DateOverStamp.GD"
	#include "graphics/ConflictCircle.GD"
	#include "graphics/ExitOval38x19.GD"
	#include "graphics/RecordOval38x19.GD"
	#include "graphics/WhiteOval38x19.GD"
	#include "graphics/BigBlueButton.GD"
	#include "graphics/BigGreenButton.GD"
#else
	#include "graphics/win32/graphics.inc"
	TYPE_TapEvent* TAP_GetEvent_SDK( byte, word, int* );
	void TAP_Channel_GetTotalNum_SDK( int*, int* );
#endif


#define PROJECT_DIRECTORY "UK TAP Project"


//#define FILL_COLOUR RGB(0,0,102)
#define FILL_COLOUR			COLOR_Black
#define MAIN_TEXT_COLOUR		RGB(29,29,29)
#define HEADING_TEXT_COLOUR		RGB8888(0,152,192)
#define TITLE_COLOUR			RGB(18,18,18)
#define TIME_COLOUR			RGB(4,4,4)
#define CALENDAR_BACKGROUND_COLOUR	COLOR_User9
#define CALENDAR_DAY_TEXT		MAIN_TEXT_COLOUR
#define INFO_FILL_COLOUR		COLOR_User3
#define POPUP_FILL_COLOUR		RGB8888(37,36,154)
#define INFO_COLOUR			COLOR_White

//#define COLOR_White RGB(31,31,31)
//#define COLOR_OffWhite RGB(29,29,29)
//#define COLOR_LightGray RGB(24,24,24)
//#define COLOR_Gray RGB(16,16,16)

#define LOG_BUFFER_SIZE		512

#define	MJD_OFFSET	51544		// 01/01/2000

#define SYS_Y1_STEP	42
#define SYS_Y1_OFFSET	36


//*****************
// Prototypes
//*****************
//
//void GenerateExitRequest( void );			// UkTimers.c
void sysDrawGraphicBorders(void);

//void MakeNewTimer( byte chosenWeekDay );		// TimerEdit.c
//int ActivateTimerSaveRestore( void );			// TimerSaveRestore.c
//void ReadTimerFile( void );
//void GenerateTimerFile( void );

void ActivateMenu(void);						// MainMenu.c
void RedrawMainMenu( void );
void TerminateMenu( void );

void DisplayConfigWindow( void );				// ConfigMenu.c

void FormatNameString( char *source, int *index, char *dest, int width);		// logo.c

void SaveConfigurationToFile( void );			// IniFile.c

byte schFileRetreiveSearchData(void);
byte schFileRetreiveRemoteData(void);
void schWriteSearchList(void);
void schWriteMoveList(void);

bool schEditWindowActivate(int, int);
void schShowWindowActivate(int, int, int);

void logInitialise(void);
void logStoreEvent(char*);
void logArchive(void);

bool GotoRoot(void);

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

static bool schShowWindowShowing = 0;


static bool returnFromEdit = 0;

static byte schTimeHour = 0, schTimeMin = 0, schTimeSec = 0;
static word schTimeMjd = MJD_OFFSET;

static byte schServiceSV = SCH_SERVICE_INITIALISE;
static byte schMoveServiceSV = SCH_MOVE_SERVICE_INITIALISE;
static byte schKeepServiceSV = SCH_KEEP_SERVICE_INITIALISE;

static byte schStartUpCounter = 0;

static bool FirmwareCallsEnabled = FALSE;
static bool schMainDebugMoveAvailable = FALSE;
static bool schMainApiMoveAvailable = FALSE;

static word schMainChangeDirSuccess = 0;
static word schMainChangeDirFail = 0;

static bool schMainChangeDirAvailable = FALSE;

static byte keyboardLanguage = KEYBOARD_ENGLISH;


enum
{
	SCH_EXISTING_SEARCH = 0,
	SCH_NEW_SEARCH
};


//------------
//
void sysDrawGraphicBorders(void)
{
	TAP_Osd_FillBox( rgn, 0, 0, 720, 576, FILL_COLOUR );		// clear the screen
	TAP_Osd_PutGd( rgn, 0, 0, &_topGd, TRUE );			// draw top graphics
	TAP_Osd_PutGd( rgn, 0, 0, &_sideGd, TRUE );			// draw left side graphics
	TAP_Osd_PutGd( rgn, 672, 0, &_sideGd, TRUE );			// draw right side graphics
}

