#include "localtap.h"
#include "RasterFontBrowser.h"
#include "tapapifix.h"
#include <string.h>
#define ID_FREETYPE_TEST			0x07

TAP_ID( ID_FREETYPE_TEST );
TAP_PROGRAM_NAME("Font test slow");
TAP_AUTHOR_NAME("Justason");
TAP_DESCRIPTION("FontTest");
TAP_ETCINFO(__DATE__);


int TAP_Main(void);
word rgn;
int _appl_version = 0x1200;
int transparency_orig;
FONT font0, font1, font2;

#define DWORD_MAX ( (dword) -1)


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	static int count = 0, err1, filelen;
	static byte flip = 0;
	int i, y;
	const int LINE_H = 21;
	const int STR_COUNT = 21;
	dword t1, t2;
	char strings[21][40] = {
		{"Test ajJsjjOjsj"},
		{"qUIck tESt"},
		{"ma"},
		{"keÿä"},
		{"<< Nelonen >>"} ,
		{"03:20 "},
		{" leading SPACE"},
		{"Test ajJsjjOjsj"},
		{"qUIck tESt"},
		{"ma"},
		{"ke"},
		{"<< Nelonen >>"} ,
		{"03:20 "},
		{" leading SPACE"},
		{"Test ajJsjjOjsj"},
		{"qUIck tESt"},
		{"ma"},
		{"ke"},
		{"<< Nelonen >>"} ,
		{"03:20 "},
		{" leading SPACE"}
	};
	char msg[64];


	if( event == EVT_KEY )
	{
		if ( param1 == RKEY_Exit)
		{
			TAP_Osd_SetTransparency(rgn, 0xFF - transparency_orig);

			Delete_Font(&font0);
			Delete_Font(&font1);
			Delete_Font(&font2);

			TAP_Osd_Delete( rgn );

#ifdef DLALLOC
			dlmalloc_exit();
#endif

			TAP_EnterNormal();
			TAP_Exit();
			return 0;
		}
		if ( param1 == RKEY_1)
		{
			TAP_ExitNormal();
			TAP_Osd_FillBox(rgn, 30, 10, 620, 500, COLOR_DarkBlue);

			t1 = TAP_GetTick();
			y = 20;
			for (i = 0; i < STR_COUNT; i++, y+=LINE_H)
			{
				TAP_Osd_PutS_Font(rgn, 50, y,  199, strings[i], COLOR_Red, COLOR_White, &font0, ALIGN_LEFT);
				TAP_Osd_PutS_Font(rgn, 200, y, 349, strings[i], COLOR_Red, COLOR_White, &font1, ALIGN_LEFT);
				TAP_Osd_PutS_Font(rgn, 350, y, 499, strings[i], COLOR_Red, COLOR_White, &font2, ALIGN_LEFT);
			}
			t2 = TAP_GetTick() - t1;

			TAP_SPrint(msg, "Ticks: %d", t2);
			TAP_Osd_PutS(rgn, 500, 500, 650, msg, COLOR_Green, COLOR_Black, 0, FNT_Size_1419, 0, ALIGN_LEFT);

			return 0;
		}
		if ( param1 == RKEY_2)
		{
			TAP_ExitNormal();
			TAP_Osd_FillBox(rgn, 30, 10, 620, 500, COLOR_DarkBlue);

			t1 = TAP_GetTick();
			y = 20;
			for (i = 0; i < STR_COUNT; i++, y+=LINE_H)
			{
				TAP_Osd_PutS_FontL(rgn, 50, y,  199, strings[i], &font0, ALIGN_LEFT);
				TAP_Osd_PutS_FontL(rgn, 200, y, 349, strings[i], &font1, ALIGN_LEFT);
				TAP_Osd_PutS_FontL(rgn, 350, y, 499, strings[i], &font2, ALIGN_LEFT);
			}
			t2 = TAP_GetTick() - t1;

			TAP_SPrint(msg, "Ticks: %d", t2);
			TAP_Osd_PutS(rgn, 500, 500, 650, msg, COLOR_Green, COLOR_Black, 0, FNT_Size_1419, 0, ALIGN_LEFT);

			return 0;
		}
		if ( param1 == RKEY_3)
		{
			TAP_ExitNormal();
			TAP_Osd_FillBox(rgn, 30, 10, 620, 500, COLOR_DarkBlue);

			t1 = TAP_GetTick();
			y = 20;
			for (i = 0; i < STR_COUNT; i++, y+=LINE_H)
			{
				TAP_Osd_PutS_FontEx(rgn, 50, y,  199, 19, 2, strings[i], COLOR_Red, COLOR_White, &font0, ALIGN_LEFT);
				TAP_Osd_PutS_FontEx(rgn, 200, y, 349, 19, 2, strings[i], COLOR_Red, COLOR_White, &font1, ALIGN_LEFT);
				TAP_Osd_PutS_FontEx(rgn, 350, y, 499, 19, 2, strings[i], COLOR_Red, COLOR_White, &font2, ALIGN_LEFT);
			}
			t2 = TAP_GetTick() - t1;

			TAP_SPrint(msg, "Ticks: %d", t2);
			TAP_Osd_PutS(rgn, 500, 500, 650, msg, COLOR_Green, COLOR_Black, 0, FNT_Size_1419, 0, ALIGN_LEFT);

			return 0;
		}
		if ( param1 == RKEY_4)
		{
			TAP_ExitNormal();
			TAP_Osd_FillBox(rgn, 30, 10, 620, 500, COLOR_DarkBlue);

			t1 = TAP_GetTick();
			y = 20;
			for (i = 0; i < STR_COUNT; i++, y+=LINE_H)
			{
				TAP_Osd_PutS(rgn, 50, y,  199, strings[i], COLOR_Red, COLOR_White, 0, FNT_Size_1419, 0, ALIGN_LEFT);
				TAP_Osd_PutS(rgn, 200, y, 349, strings[i], COLOR_Red, COLOR_White, 0, FNT_Size_1419, 0, ALIGN_LEFT);
				TAP_Osd_PutS(rgn, 350, y, 499, strings[i], COLOR_Red, COLOR_White, 0, FNT_Size_1419, 0, ALIGN_LEFT);
			}
			t2 = TAP_GetTick() - t1;

			TAP_SPrint(msg, "Ticks: %d", t2);
			TAP_Osd_PutS(rgn, 500, 500, 650, msg, COLOR_Green, COLOR_Black, 0, FNT_Size_1419, 0, ALIGN_LEFT);

			return 0;
		}
	}
	return param1;//event;
}


int TAP_Main(void)
{
	int err1;


	InitTAPAPIFix();
	transparency_orig = TAP_GetSystemVar( SYSVAR_OsdAlpha );
	rgn = TAP_Osd_Create(0, 0, 720, 576, 0, 0);

	TAP_Osd_SetTransparency( rgn, 150 );

	err1 = Load_Font(&font0, "calibri_small.rasterized");
	if (err1!=0) return 0;
	err1 = Load_Font(&font1, "arialn_small.rasterized");
	if (err1!=0) return 0;
	err1 = Load_Font(&font2, "tahoma_small.rasterized");
	if (err1!=0) return 0;

	SetColors(&font0, COLOR_Red, COLOR_White);
	SetColors(&font1, COLOR_Red, COLOR_White);
	SetColors(&font2, COLOR_Red, COLOR_White);

	return 1;
}
