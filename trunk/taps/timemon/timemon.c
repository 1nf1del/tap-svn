/**********************************************************************************/
/* Name:     timemon.c v1.0                                                       */
/* For:      Topfield TF5800PVRt                                                  */
/* Author:   sl8 (Thanks to everyone at http://www.toppy.org.uk/)                 */
/* Descr.:   Logs differences in received network time.                           */
/**********************************************************************************/

// This tap compares the current time/date with the time/date from the previous iteration.
// If the difference is 10 minutes or more the following data is stored in the 'timemon.log' file:
//
// 'Previous time/date, Siganl Level, Siganl Quality, total number of timers'
// 'Current time/date, Siganl Level, Siganl Quality, total number of timers'
//
// The size of the 'timemon.log' file is 5k and it can store up to 62 events.
//
// Pressing 'Exit' will show how many events have been stored and pressing the 'Blue' function key will exit the tap.
//
// Warning! - This tap reads/writes to the HDD so it is advisable that all recordings are backed up before
//  using this tap.

#include <tap.h>
#include "timemon.h"

#define COLOR_WINDOW COLOR_DarkCyan
#define COLOR_WINDOWTEXT COLOR_White
#define COLOR_BTNFACE COLOR_DarkBlue
#define COLOR_BTNTEXT COLOR_White
#define COLOR_BTNSHADOW COLOR_Black
#define COLOR_BTNHIGHLIGHT COLOR_Blue

// needs to be requested from Topfield!
#define ID_SIGNAL 0x8000fef9

TAP_ID( ID_SIGNAL );
TAP_PROGRAM_NAME("Time Monitor v1.0");
TAP_AUTHOR_NAME("sl8");
TAP_DESCRIPTION("Logs differences in received network time.");
TAP_ETCINFO(__DATE__);


/*****************************************************************************/
/* Global variables */
/*****************************************************************************/
static dword tmInfoLength = 0;

static word mjdLast, yearLast;
static byte hourLast, minLast, secLast, monthLast, dayLast;

static word mjd, year, tmBufferOffset, tmNumberOfEvents;
static byte hour, min, sec, month, day, weekday, tmExitFL;

static int signalLevel, signalQuality, signalLevelLast, signalQualityLast, timers, timersLast;


/*****************************************************************************/
/* Function Prototypes */
/*****************************************************************************/
void tmMain(void);
void tmInitialise(void);
void tmService(void);
void tmLogEvent(void);


/*****************************************************************************/
/* Functions */
/*****************************************************************************/

void ShowMessageWin (char* lpMessage, char* lpMessage1)
{
	int rgn;							// stores rgn-handle for osd
	dword w;							// stores width of message-text

	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );		// create rgn-handle
	w = TAP_Osd_GetW( lpMessage, 0, FNT_Size_1926 ) + 10;		// calculate width of message
	if (TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10>w)
	{
		w = TAP_Osd_GetW( lpMessage1, 0, FNT_Size_1926 ) + 10;	// calculate width of message
	}
	if (w > 720) w = 720;						// if message is to long
	TAP_Osd_FillBox(rgn, (720-w)/2-5, 265, w+10, 62, RGB(19,19,19) );	// draw background-box for border
	TAP_Osd_PutS(rgn, (720-w)/2, 270, (720+w)/2, lpMessage,		// show message
		RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926, FALSE, ALIGN_CENTER);
	TAP_Osd_PutS(rgn, (720-w)/2, 270+26, (720+w)/2, lpMessage1,		// show message
		RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926,FALSE, ALIGN_CENTER);
	TAP_Delay(200);							// show it for 2 seconds
	TAP_Osd_Delete(rgn);					// release rgn-handle
}




/*****************************************************************************/
/* tmMain */
/*****************************************************************************/
void tmMain(void)
{
	static byte tmMainSV = TM_INITILAISE;

	signalLevel = TAP_GetSignalLevel();
	signalQuality = TAP_GetSignalQuality();
	TAP_GetTime( &mjd, &hour, &min, &sec);
	TAP_ExtractMjd( mjd, &year, &month, &day, &weekday);
	timers = TAP_Timer_GetTotalNum();

	switch(tmMainSV)
	{
	/* ------------------------------------------------------------------------------------ */
	case TM_INITILAISE:

		tmInitialise();

		tmMainSV = TM_SERVICE;

		break;
	/* ------------------------------------------------------------------------------------ */
	case TM_SERVICE:

		tmService();

		break;
	/* ------------------------------------------------------------------------------------ */
	}

	mjdLast = mjd;
	hourLast = hour;
	minLast = min;
	secLast = sec;
	yearLast = year;
	monthLast = month;
	dayLast = day;

	signalLevelLast = signalLevel;
	signalQualityLast = signalQuality;

	timersLast = timers;
}


/*****************************************************************************/
/* tmInitialise */
/*****************************************************************************/
void tmInitialise(void)
{
	char tmBuffer[TM_FILE_BLOCK_SIZE];
	char hddBuffer[1];

	byte i;

	TYPE_File* fp;

	memset (tmBuffer, 0, TM_FILE_BLOCK_SIZE);

	if (!TAP_Hdd_Exist (TM_FILENAME))
	{
		TAP_Hdd_Create (TM_FILENAME, ATTR_NORMAL);

		if (fp = TAP_Hdd_Fopen (TM_FILENAME))
		{
			for (i = 0; i < TM_FILE_MAX_LENGTH / TM_FILE_BLOCK_SIZE; i++)
			{
				TAP_Hdd_Fwrite (tmBuffer, 1, TM_FILE_BLOCK_SIZE, fp);
			}

			TAP_Hdd_Fseek (fp, 0, 0);

			TAP_Hdd_Fwrite (TM_INFO, 1, sizeof(TM_INFO), fp);

			tmBufferOffset = sizeof(TM_INFO) - 1;

			tmNumberOfEvents = 0;

			TAP_Hdd_Fclose (fp);
		}
	}
	else
	{
		if (fp = TAP_Hdd_Fopen (TM_FILENAME))
		{
			tmBufferOffset = 0;
			tmExitFL = FALSE;

			while
			(
				(tmExitFL == FALSE)
				&&
				(tmBufferOffset < TM_FILE_MAX_LENGTH)
			)
			{
				TAP_Hdd_Fseek (fp, tmBufferOffset, 0);

				TAP_Hdd_Fread (hddBuffer, 1, 1, fp);

				if(hddBuffer[0] != 0)
				{
					tmBufferOffset++;

					if(hddBuffer[0] == 0x0A)
					{
						tmNumberOfEvents++;
					}
				}
				else
				{
					tmExitFL = TRUE;
				}
			}

			tmNumberOfEvents -= 2;

			TAP_Hdd_Fclose (fp);

			if (tmBufferOffset >= (TM_FILE_MAX_LENGTH - TM_EVENT_BUFFER_SIZE))
			{
				ShowMessageWin("Log File Full","Exiting!");

				TAP_Exit();

				return;
			}
		}
	}

	ShowMessageWin("Time Monitor Tap v1.0","Started");
}


/*****************************************************************************/
/* tmService */
/*****************************************************************************/
void tmService(void)
{
	dword timeDateInMins, timeDateInMinsLast;

	timeDateInMins = (mjd * 24 * 60) + (hour * 60) + min; 
	timeDateInMinsLast = (mjdLast * 24 * 60) + (hourLast * 60) + minLast; 

	if
	(
		(
			(timeDateInMins > timeDateInMinsLast)
			&&
			((timeDateInMins - timeDateInMinsLast) >= TM_TIME_DIFFERENCE_IN_MINS)
		)
		||
		(
			(timeDateInMinsLast > timeDateInMins)
			&&
			((timeDateInMinsLast - timeDateInMins) >= TM_TIME_DIFFERENCE_IN_MINS)
		)
	)
	{
		tmLogEvent();
	}
}

/*****************************************************************************/
/* tmLogEvent */
/*****************************************************************************/
void tmLogEvent(void)
{
	char	tmBuffer[TM_EVENT_BUFFER_SIZE];

	TYPE_File* fp;

	memset (tmBuffer, 0, TM_EVENT_BUFFER_SIZE);

	TAP_SPrint( tmBuffer, "%02u:%02u:%02u %02u/%02u/%04u, %3u,  %3u, %3u       %02u:%02u:%02u %02u/%02u/%04u, %3u,  %3u, %3u\r\n", hourLast, minLast, secLast, dayLast, monthLast, yearLast, signalLevelLast, signalQualityLast, timersLast, hour, min, sec, day, month, year, signalLevel, signalQuality, timers);

	if (fp = TAP_Hdd_Fopen (TM_FILENAME))
	{
		TAP_Hdd_Fseek (fp, tmBufferOffset, 0);

		TAP_Hdd_Fwrite (tmBuffer, 1, strlen(tmBuffer), fp);

		TAP_Hdd_Fclose (fp);

		tmBufferOffset += strlen(tmBuffer);

		tmNumberOfEvents++;

		if (tmBufferOffset >= (TM_FILE_MAX_LENGTH - TM_EVENT_BUFFER_SIZE))
		{
			ShowMessageWin("Log File Full","Exiting!");

			TAP_Exit();

			return;
		}
	}
	else
	{
		ShowMessageWin("Error! - Unable to open file","Exiting!");

		TAP_Exit();

		return;
	}
}


/*****************************************************************************/
/* TAP_EventHandler */
/*****************************************************************************/
dword TAP_EventHandler(word wEvent, dword dwParam1, dword dwParam2)
{
	char message[10];
	dword returnKey, state, subState;

	returnKey = dwParam1;

	switch(wEvent)
	/* ------------------------------------------------------------------------------------ */
	{
	case EVT_IDLE:

		tmMain();

		returnKey = 0;

		break;
	/* ------------------------------------------------------------------------------------ */
	case EVT_KEY:

	 	TAP_GetState( &state, &subState );

		if
		(
			(dwParam1 == RKEY_Exit)
			&&
			(state == STATE_Normal)
			&&
			(subState == SUBSTATE_Normal)
		)
		{
			TAP_SPrint( message, "%5u", tmNumberOfEvents);

			ShowMessageWin("Number Of Events Stored:",message);
		}

		if(dwParam1 == RKEY_F4)
		{
			ShowMessageWin("Exiting","TimeMon Tap");

			TAP_Exit();
		}

		break;
	/* ------------------------------------------------------------------------------------ */
	default :
		 	break;
	/* ------------------------------------------------------------------------------------ */
	}

	return (returnKey);
}


int TAP_Main(void) {
  return 1; // we're starting in TSR-mode
}
