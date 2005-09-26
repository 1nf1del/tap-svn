#include <stdarg.h>
#include <stdio.h>

#include <tap.h>
#include <osdlib.h>

#define ID_TAP	0x80100212
#define TAP_NAME	"testosd v0.1"

TAP_ID(ID_TAP);
TAP_PROGRAM_NAME(TAP_NAME);
TAP_AUTHOR_NAME("Steve Bennett");
TAP_DESCRIPTION("Test OSD " OS " " __TIME__);
TAP_ETCINFO(__DATE__);

#include <string.h>

static int msg_rgn;
static char msg_buf[1024];

#define msg(format...) sprintf(msg_buf, format); log_msg(msg_rgn, msg_buf)

void log_msg(int rgn, const char *str)
{
	static int log_line = 0;

	TAP_Osd_PutS(msg_rgn, 100, 100 + log_line * 20, 500, str, COLOR_White, COLOR_None, 0,FNT_Size_1419, FALSE, ALIGN_LEFT);

	if (++log_line == 20) {
		log_line = 0;
	}
}

dword	TAP_EventHandler ( word wEvent, dword dwKey, dword dwParam2 )
{
	if (wEvent == EVT_KEY && dwKey != 0) {
		/*TAP_Win_Delete(&win);*/
		TAP_Osd_Delete(msg_rgn);
		TAP_EnterNormal();
		TAP_Exit();
		return 0;
	}

	return dwKey; 
}

int	TAP_Main (void)
{
	TYPE_Window 	win;
	int rgn;
 
	TAP_ExitNormal();
	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );

	msg_rgn = rgn;

	osd_grid(rgn, 20, COLOR_Gray);

	TAP_Win_SetDefaultColor( &win );

	msg("msg_rgn=%d", rgn);

	TAP_Win_Create( &win, rgn, 100, 200, 300, 200, TRUE, FALSE );
	TAP_Win_SetTitle( &win, "Win Title", 0, FNT_Size_1419 );

#if 1
	msg("win: x=%d", win.win.x);
	msg("win: y=%d", win.win.y);
	msg("win: w=%d", win.win.w);
	msg("win: h=%d", win.win.h);
	msg("win: titleX=%d", win.win.titleX);
	msg("win: titleY=%d", win.win.titleY);
	msg("win: titleW=%d", win.win.titleW);
	msg("win: titleH=%d", win.win.titleH);
	msg("win: bodyX=%d", win.win.bodyX);
	msg("win: bodyY=%d", win.win.bodyY);
	msg("win: bodyW=%d", win.win.bodyW);
	msg("win: bodyH=%d", win.win.bodyH);
#endif
#if 0
#define SHOW(X) msg("%s=#%x", #X, win.win.color.X);

	msg("test: ABC=%x", 0xABC);
	SHOW(titleBack);
	SHOW(titleText);
	SHOW(bodyBack);
	SHOW(bodyText);	
	SHOW(border);
	SHOW(shadow);
	SHOW(dark);
	SHOW(light);
	SHOW(check);
#endif

	TAP_Osd_PutS(rgn, 50, 100, 200, "W", COLOR_Yellow, COLOR_Blue, 0,FNT_Size_1622, FALSE, ALIGN_LEFT);
	TAP_Osd_DrawRectangle(rgn, 50, 100, 20, 20, 1, COLOR_Green);

	/*TAP_Win_SetTitle( &win, "Win: x=100,y=200,w=300,h=200", 0, FNT_Size_1622 );*/

	TAP_Osd_PutS( rgn, 300, 100, 700, "line 1 w/b left", COLOR_White, COLOR_None, 0,FNT_Size_1622, FALSE, ALIGN_LEFT );
	TAP_Osd_PutS( rgn, 300, 130, 700, "line 2 w/b center", COLOR_White, COLOR_None, 0,FNT_Size_1622, FALSE, ALIGN_CENTER );
	TAP_Osd_PutS( rgn, 300, 160, 700, "line 3 w/b right", COLOR_White, COLOR_None, 0,FNT_Size_1622, FALSE, ALIGN_RIGHT );

#if 0
	TAP_Delay( 1500 );	
	TAP_Win_Delete(&win);
	TAP_Osd_Delete(rgn);
	TAP_EnterNormal();

	return 0;
#else
	return 1;
#endif

}
