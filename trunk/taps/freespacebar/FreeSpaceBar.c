/*
Freespacebar 1.1n
Now uses TSR commander to exit, rather than a menu system (saves daisy chaining to multiple taps)
21/01/05

Freespacebar 1.1m
Fixes overlapping text with newer larger keyboard size with new f/w.  Fixed German text display a little.
4/12/04

Freespacebar 1.1k
Now if timeshift is off, will not poll the HDD free space during idle time, thus will let the HDD spin down.  Also accommodated german language mods.   Can only be used with newer than April 2004 firmware.
25/09/04

Freespacebar 1.1j
If rate is 0, now will show 7Mb/s* with the "*" to indicate this is the value chosen to calculate the time remaining.
12/07/04

Freespacebar 1.1i
Fixed the "bitrate" calculation.
LuckyB missed it ;-)
27/06/04

Freespacebar 1.1h
Date: 10/06/2004
Problems with newer firmware fixed
Cheers
LuckyB

Freespacebar 1.1g
Date: 29/04/2004
I made the program average two adjacent readings as I found that the Toppy seems to "pump" high and low rates to the HDD, and the average is almost exactly what the recorded bitrate should be.   This results in far less fluctuations of the reading.   Since I am averaging over 2 periods now, I made the measurement period slightly smaller at 5.5secs (however the smaller the period, the more severe the fluctuations are, averaging luckily seems OK).
I also removed a couple of variables I setup for the prev version but never used.
Enjoy!

Freespacebar 1.1f
Date: 29/4/2004
I modded Koops version, and made this calculate the bitrate of the channel you are watching by calculating how much disk space is lost over a 6 second interval.   The results are a little out (slightly jumpy value) but a pretty good indication of the transmitted bitrate (certainly good enough to determine how much space is left, and allowing the bitrate over 6 secs to be approx calculated).
If the bitrate is <0.5Mb/s or >30Mb/s (indicating a glitch in the maths that happens sometimes when the HDD first spins up, also indicating the HDD isn't recording anything at all) then the value shown= 0Mb/s, but the calculation for hours:minutes is done for 7Megabit/s.

So for newbies to the "TAP" environment, all you need to do is copy this to your Topfield via USB with the ALTAIR proggy avail on the Topfield Australia site, and put the tap file into the "ProgramFiles" folder where you can manually launch it when you wish, or put it into "\ProgramFiles\Auto Start"  so that you don't have to manually choose it to start it (The Auto Start folder launches TAPs when the player boots up).
To manually launch once you have uploaded it: hit the middle filelist button on the remote, then either >| or |< once or twice to see the TAP applications that you have uploaded.  Select the "FreeSpaceBar" file, hit OK, and away you go!  If you hit the exit key when in normal mode (no lists/menus), it pops up a menu asking if you want to close it, selecting "Yes" will completely remove it from memory.
Cheers
Tonymy01

Freespacebar 1.1e
Date: 26/4/2004
I've just edited the marcs original freespace bar and replaced all the text with english and recomplied ( a 2 second job).
It was mainly just a test of my TAP compile environment.
It *should* work. ;)
1.1d - Changed - German to english text
1.1e - Changed - 5Mbit to 7Mbit remaining time calculation. SHould provide a more realistic estimate of recording time remaining. 
Averaged according to the rates on http://users.bigpond.net.au/tharper/DTVBitrates.html
Koops

Original freespacebar 1.1b
*/

#include "tap.h"

#define ID_FREESPACEPIE 0x807A10B4

#define	COLOR_CustomYellow	RGB(28,28,7)
#define _PROGRAM_NAME_ "FreeSpaceBar1.1n"


TAP_ID (ID_FREESPACEPIE);
TAP_PROGRAM_NAME (_PROGRAM_NAME_);
TAP_AUTHOR_NAME ("Judy(Marc,Tonymy01,Koops,Lucky B)");
TAP_DESCRIPTION ("Displays free space on the drive as a bar, and calculates remaining time");
TAP_ETCINFO (__DATE__);

#include "TSRCommander.inc"

//added rate calculation that calculates the bitrate (in Megabit/s) over a 6 second interval now: Tonymy01
//1.1g averages last 2 rates, as it looks like the drive fluctuates a bit over a small period.
static dword rate=0,lastrate=0, lastmn = 0;
static	dword lastsize=1000000000,lastsec=0;


#define COLOR_FRed   RGB( 23,  0, 0 )					// colors for bar (some currently unused)
#define COLOR_BRed   RGB( 15,  0, 0 )
#define COLOR_FGreen RGB(  0, 23, 0 )
#define COLOR_BGreen RGB(  0, 15, 0 )
#define COLOR_LGrey  RGB(8,8,8)
#define COLOR_MGrey  RGB(15,15,15)
#define COLOR_DGrey  RGB( 25, 25, 25)


static int lastwf = -1;							// stores last angle for color-change

#define ABS(x)	(((x)>0)?(x):(-(x)))



dword ShowExitMenu();
void CloseExitMenu();
dword HandleExitWindowKey( dword dwKey );

static TYPE_Window g_wndExit;
static bool g_bExitWindow	= FALSE;
static word g_rgnMem;
static word g_rgn;

bool Keyboard_Active (int x,int y, int col)   // compares color of title of menu to known value (I wrote another locating
// TAP to find out the value 0xF709).
{
	TYPE_OsdBaseInfo osdBI;
	word* pAddr;
	//dword i, x, y;

	TAP_Osd_GetBaseInfo (&osdBI);	// retrieve current OSD

	pAddr = osdBI.eAddr + 720 * y;   //even field
	if (*(pAddr + x)== col)
		return TRUE;
	else
		return FALSE;
}



void DrawLine( word rgn, int Ax, int Ay, int Bx, int By, int Color )
{
	int dX = ABS(Bx-Ax);						// store the change in X and Y of the line endpoints
	int dY = ABS(By-Ay);

	int CurrentX = Ax;						// store the starting point (just point A)
	int CurrentY = Ay;

	int Xincr, Yincr;
	if (Ax > Bx) { Xincr=-1; } else { Xincr=1; }			// which direction in X?
	if (Ay > By) { Yincr=-1; } else { Yincr=1; }			// which direction in Y?

	if (dX >= dY)	// if X is the independent variable
	{
		int dPr 	= dY<<1;				// amount to increment decision if right is chosen (always)
		int dPru	= dPr - (dX<<1);			// amount to increment decision if up is chosen
		int P		= dPr - dX; 				// decision variable start value

		for (; dX>=0; dX--) 					// process each point in the line one at a time (just use dX)
		{
			TAP_Osd_PutPixel( rgn, CurrentX, CurrentY, Color);	// plot the pixel
			if (P > 0)								// is the pixel going right AND up?
			{
				CurrentX+=Xincr;			// increment independent variable
				CurrentY+=Yincr;			// increment dependent variable
				P+=dPru;				// increment decision (for up)
			}
			else						// is the pixel just going right?
			{
				CurrentX+=Xincr;			// increment independent variable
				P+=dPr; 				// increment decision (for right)
			}
		}
	}
	else			// if Y is the independent variable
	{
		int dPr 	= dX<<1;				// amount to increment decision if right is chosen (always)
		int dPru	= dPr - (dY<<1);			// amount to increment decision if up is chosen
		int P		= dPr - dY; 				// decision variable start value

		for (; dY>=0; dY--) 					// process each point in the line one at a time (just use dY)
		{

			TAP_Osd_PutPixel( rgn, CurrentX, CurrentY, Color);	// plot the pixel
			if (P > 0)						// is the pixel going up AND right?
			{
				CurrentX+=Xincr;			// increment dependent variable
				CurrentY+=Yincr;			// increment independent variable
				P+=dPru;				// increment decision (for up)
			}
			else						// is the pixel just going up?
			{
				CurrentY+=Yincr;			// increment independent variable
				P+=dPr; 				// increment decision (for right)
			}
		}
	}
}

void DrawPie (void)
{
	int w;								// counter for rows
	int wf;	                            // stores relative red/green transition position
	int prc;							// stores free space percentage
	int rst;							// stores remaining recording time in seconds
	int mn;								// stores minute-part of remaining recording time
	int hr;								// stores hour-part of remaining recording time
	int x;								// currently unused
	int y;								// currently unused
	int lpos=109;							// stores left starting-position of bar
	int sze=295;							// stores width of bar
	int tpos=460;
	int hgt=20;
	int toffset;
	int colmod=2;
	int cvalue=160;
	char str[40];							// temporary storage for text-outputs

	
	if (Keyboard_Active(390,300,0xC675)||Keyboard_Active(168,114,0xF709))
		return; // don't draw pie if keyboard is up for 2 firmware versions.
	
	toffset=(hgt-20)/2;
	prc = TAP_Hdd_FreeSize()*100/TAP_Hdd_TotalSize();		// calculate percentage for free space
	if (rate<5 || rate>300)	{					//if the calculated bitrate is less than 0.5megabit/s or greater than 30Mb/s(glitch or HDD not recording)
		rst = TAP_Hdd_FreeSize()/7*8;           // calculate remaining rec-time @ 7MBit in seconds
		rate=0;}
	else
		rst = TAP_Hdd_FreeSize()/rate*10*8;          // else must be a legit rate, so use the rate (out by a factor of 10).

	hr = rst/60/60; 						    // calculate remaining rec-time in hours
	mn = (rst-hr*60*60)/60;						// calculate remaining rec-time-minutes
	wf = sze-TAP_Hdd_FreeSize()*sze / TAP_Hdd_TotalSize();		// calculate relative position for red/green transition
	if (wf != lastwf || rate != lastrate || mn != lastmn || !g_rgnMem)			// check if redraw is necessary
	{
		if ( g_rgnMem )
			TAP_Osd_Delete( g_rgnMem );
		g_rgnMem = TAP_Osd_Create( 0, 0, 720, 576, 0, OSD_Flag_MemRgn );		// create rgn-handle


		lastrate=rate;
		lastmn=mn;
		lastwf = wf;						// store last transition-value for redraw-check
		for ( w=0 ; w<hgt ; w++ )				// count w up for the 20 Lines of the bar 
		{	
			if (w<=hgt/2)					// check if we're drawing the upper or the lower half of the bar
			{
				cvalue=cvalue+colmod;
				DrawLine(g_rgnMem , lpos,tpos+w,lpos+wf,tpos+w,RGB(cvalue+7,0,0));		//Draw Red-Part of the bar, use line-value to change color
				DrawLine(g_rgnMem , lpos+wf,tpos+w,lpos+sze-1,tpos+w,COLOR_User4);      //Draw Green-Part of the bar, use line-value to change color
			}
			else
			{
				cvalue=cvalue-colmod;
				DrawLine(g_rgnMem , lpos,tpos+w,lpos+wf,tpos+w,RGB(cvalue+7,0,0));	//Draw Red-Part of the bar, use line-value to change color (inverted for lower half)
				DrawLine(g_rgnMem , lpos+wf,tpos+w,lpos+sze-1,tpos+w,COLOR_User4);	//Draw Green-Part of the bar, use line-value to change color (inverted for lower half)			
			}
		}
		for ( w=0 ; w<sze ; w+=4 )
		{
			DrawLine(g_rgnMem , lpos+w, tpos,lpos+w,tpos+hgt-1,COLOR_Black);
		}
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
		{
			TAP_SPrint(str,"Rest: :         h@        Mb/s ");
			TAP_Osd_PutStringAf1419( g_rgnMem, lpos+132, tpos-21,-1,str,COLOR_White, COLOR_User4 );
			if (rate==0)
			{
				TAP_SPrint( str, "%02d:%02dh @ *7",hr,mn);		//Format Text for free-percentage
				TAP_Osd_PutStringAf1622( g_rgnMem, lpos+168, tpos-23,-1,str,COLOR_White, COLOR_User4 );
			}
			else
			{
				TAP_SPrint( str, "%02d:%02d",hr,mn);		//Format Text for free-percentage
				TAP_Osd_PutStringAf1622( g_rgnMem, lpos+168, tpos-23,-1,str,COLOR_White, COLOR_User4 );
				TAP_SPrint( str, "%d,%01d",rate/10,rate%10);		//Format Text for free-percentage
				TAP_Osd_PutStringAf1622( g_rgnMem, lpos+234, tpos-23,-1,str,COLOR_White, COLOR_User4 );
			}

		}
		else
		{	
			TAP_SPrint(str,"free:           h@        Mb/s");
			TAP_Osd_PutStringAf1419( g_rgnMem, lpos+134, tpos-21,-1,str,COLOR_White, COLOR_User4 );

			if (rate==0)
			{
				TAP_SPrint( str, "%02d:%02dh @ *7",hr,mn);		//Format Text for free-percentage
				TAP_Osd_PutStringAf1622( g_rgnMem, lpos+168, tpos-23,-1,str,COLOR_White, COLOR_User4 );
			}
			else
			{
				TAP_SPrint( str, "%02d:%02d",hr,mn);		//Format Text for free-percentage
				TAP_Osd_PutStringAf1622( g_rgnMem, lpos+168, tpos-23,-1,str,COLOR_White, COLOR_User4 );
				TAP_SPrint( str, "%d.%01d",rate/10,rate%10);		//Format Text for free-percentage
				TAP_Osd_PutStringAf1622( g_rgnMem, lpos+233, tpos-23,-1,str,COLOR_White, COLOR_User4 );

			}




		}

		DrawLine(g_rgnMem,lpos+wf,tpos,lpos+wf,tpos+hgt-1,RGB(255,255,255));
		TAP_Osd_Draw3dBox(g_rgnMem, lpos,tpos,sze,hgt,COLOR_LGrey,COLOR_DGrey); 			//Draw 3D-Box around Bar
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			TAP_SPrint( str, "Frei: %02d%% (%02dGB/%02dGB)", prc,TAP_Hdd_FreeSize()/1024,TAP_Hdd_TotalSize()/1024);							//Format Text for free-percentage
		else
			TAP_SPrint( str, "Free: %02d%% (%02dGB/%02dGB)", prc,TAP_Hdd_FreeSize()/1024,TAP_Hdd_TotalSize()/1024);							//Format Text for free-percentage
		TAP_Osd_PutStringAf1419( g_rgnMem, lpos-1, tpos+toffset, -1, str, COLOR_Black, COLOR_None );		//Print black "framing" of next Text with...
		TAP_Osd_PutStringAf1419( g_rgnMem, lpos+1, tpos+toffset, -1, str, COLOR_Black, COLOR_None );		//... +1/-1 Offset to position
		TAP_Osd_PutStringAf1419( g_rgnMem, lpos, tpos+toffset, -1, str, COLOR_White, COLOR_None );		//Print free and total space-statement to bar
		//savedBox1 = TAP_Osd_SaveBox( rgn, lpos+140,tpos-23,150,19);
		//savedBox = TAP_Osd_SaveBox( rgn, lpos, tpos, sze, hgt+2);					//Store complete bar-area in buffer for fast redraw
	}
	//else
	//	TAP_Osd_RestoreBox( rgn, lpos, tpos, sze, hgt+2, savedBox); // paint pie-bitmap from buffer	//Restore bar-area from buffer (as there were no changes)
	//TAP_Osd_RestoreBox( rgn, lpos+140,tpos-23,150,19,savedBox1);

	if ( !g_rgn )
		g_rgn = TAP_Osd_Create(  0, 0, 720, 576, 0, 0 );		// create rgn-handle
	TAP_Osd_Copy( g_rgnMem, g_rgn, lpos, tpos, sze, hgt, lpos, tpos, FALSE );
	TAP_Osd_Copy( g_rgnMem, g_rgn, lpos+135,tpos-21,160,18,lpos+135,tpos-21, FALSE );
	//TAP_Osd_Copy( g_rgnMem, g_rgn, lpos+135,tpos-25,160,30,lpos+135,tpos-25, FALSE );

}


void ShowMessageWin (char* lpMessage, char* lpMessage1, int time)
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
	TAP_Delay(time);							// show it for 1.1 seconds
	TAP_Osd_Delete(rgn);					// release rgn-handle
}




int IsAnythingShown (void)
{
	TYPE_OsdBaseInfo osdBaseInfo;					// stores OSD-BaseInfo
	word *_vAddr[576];						// stores vertical addresses
	dword i;							// stores loop-variable for building addresses
	dword x;							// stores x-position for screen-scann
	dword y;							// stores y-position for screen-scann

	TAP_Osd_GetBaseInfo( &osdBaseInfo );				// get baseadresses of screen-buffer
	for( i=0; i<576; i+=2 )						// set vertical address
	{
		_vAddr[i] = osdBaseInfo.eAddr + 720*i;
		_vAddr[i+1] = osdBaseInfo.oAddr + 720*i;
	}
	for( y=100; y<576; y += 4)	// scan most of screen-buffer, every 2nd line to save resource
	{
		for ( x=0; x<720;  x+=6)  //may as well only scan every 6 pixels to save time and resource
		{
			if ((*(_vAddr[y]+x)) != 0)			// if any value then graphics are shown
				return TRUE;				// return that somewhat is on the screen
		}
	}
	return FALSE;							// return that nothing is on screen
}

bool TSRCommanderExitTAP (void)
{
	if ( g_rgnMem )
		TAP_Osd_Delete(g_rgnMem);
	if ( g_rgn )
		TAP_Osd_Delete(g_rgn);

	if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
		ShowMessageWin("wird beendet",_PROGRAM_NAME_,100);
	else
		ShowMessageWin("Quitting",_PROGRAM_NAME_,100);

	return TRUE;
}


void TSRCommanderConfigDialog()
{
	return;
}



dword TAP_EventHandler (word wEvent, dword dwParam1, dword dwParam2)
{
	dword dwState, dwSubState,size,sec,rate1;					// stores the states of firmware

	if (wEvent == EVT_IDLE)						// no key pressed, only idle-loop
	{
		TSRCommanderWork();
		TAP_GetState( &dwState, &dwSubState );	// get state of firmware
        
		if ((dwState == STATE_FileList) || (TAP_GetSystemVar(SYSVAR_Timeshift)==1))	//now when timeshift is turned off, won't check the hdd free size to avoid spinning up the HDD
		{  //timeshift on or in filelist menu
			sec=TAP_GetTick();
			if (sec >= (lastsec+550)) {						//every approx 5.5secs as ticks are every 10ms
				size = TAP_Hdd_FreeSize();				    //megabytes remaining... 
				rate1 = (lastsize-size)*8000/(sec-lastsec);	//calculate bitrate by dividing size used by time (is factor of 10 out as
				//I don't want to play with floating point)
				if (lastrate!=0) rate=(lastrate+rate1)/2;	//average measurement over 2 periods as it appears to fluctuate a bit.
				else rate=rate1;
				lastsize=size;
				lastsec=sec;
				lastrate=rate1;
			}
		}
		else //timeshift off and not in filelist menu
			rate=0;

		if (dwState == STATE_FileList )	// is firmware in filelist-mode
		{
			DrawPie();				// draw the pie
		}
	}


	ef (wEvent == EVT_KEY)						// has user pressed any key
	{
		lastwf = -1;
		TAP_GetState( &dwState, &dwSubState );			// get state of firmware

		if (dwState != STATE_Normal)				// skip if in any menu
			return dwParam1;				// passing unchanged keycode to firmware
		if (dwSubState != SUBSTATE_Normal)			// skip if in any sub-menu
			return dwParam1;				// passing unchanged keycode to firmware
	}
	return dwParam1; 						// passing unchanged state to firmware
}



int TAP_Main (void)
{
	g_rgnMem = 0;
	g_rgn	 = 0;
	g_bExitWindow = FALSE;
	;


	if ( TAP_GetTick() > 4000 ){
		if (TAP_GetSystemVar(SYSVAR_OsdLan)==LAN_German)
			ShowMessageWin("beginnen ",_PROGRAM_NAME_,120);	// show user TAP has started if manually started
		else
			ShowMessageWin("starting ",_PROGRAM_NAME_,120);	// show user TAP has started if manually started
	}
	TSRCommanderInit( 0, FALSE );
	return 1;							// we're starting in TSR-mode
}

