#include "localtap.h"
#include "RasterFontBrowser_fast.h"
#include "tapapifix.h"
#include <string.h>
#define ID_FREETYPE_TEST			0x07

TAP_ID( ID_FREETYPE_TEST );
TAP_PROGRAM_NAME("Font test");
TAP_AUTHOR_NAME("Justason");
TAP_DESCRIPTION("FontTest");
TAP_ETCINFO(__DATE__);


int TAP_Main(void);
word rgn;
int _appl_version = 0x1200;
int transparency_orig;


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	static int count = 0, err1, filelen;
	char heading[]="Astalavista baby";
	char message[]="Humpty Dumpty sat on a wall";
	FONT font;


	if( event == EVT_KEY )
	{
		if ( param1 == RKEY_Exit)
		{
			TAP_Osd_SetTransparency(rgn, 0xFF - transparency_orig);

			TAP_Osd_Delete( rgn );
			TAP_EnterNormal();
			TAP_Exit();
			return 0;
		}
		if ( param1 == RKEY_1)
		{
			TAP_ExitNormal();
			TAP_Osd_FillBox(rgn, 50, 20, 620, 300, COLOR_DarkBlue);
			Draw_Font_String(rgn, 55, 20, heading, "segoeui_iso8859-1_20.rasterized", COLOR_Yellow, COLOR_DarkBlue);

			err1 = Load_Font(&font, "segoeui_iso8859-1_16.rasterized");
			if (err1!=0) return 0;
			TAP_Osd_PutS_Font(rgn, 55, 40, 400, message, COLOR_Yellow, COLOR_Blue, &font, ALIGN_LEFT);
			TAP_Osd_PutS_Font(rgn, 55, 70, 400, message, COLOR_Yellow, COLOR_Blue, &font, ALIGN_CENTER);
			TAP_Osd_PutS_Font(rgn, 55, 100, 400, message, COLOR_Yellow, COLOR_Blue, &font, ALIGN_RIGHT);
			Delete_Font(&font);

			err1 = Load_Font(&font, "segoeui_iso8859-1_20.rasterized");
			if (err1!=0) return 0;
			TAP_Osd_PutS_Font(rgn, 55, 140, 100, message, COLOR_Yellow, COLOR_Blue, &font, ALIGN_LEFT);
			TAP_Osd_PutS_Font(rgn, 55, 170, 100, message, COLOR_Yellow, COLOR_Blue, &font, ALIGN_CENTER);
			TAP_Osd_PutS_Font(rgn, 55, 200, 100, message, COLOR_Yellow, COLOR_Blue, &font, ALIGN_RIGHT);
			Delete_Font(&font);
			return 0;
		}
	}
	return param1;//event;
}


int TAP_Main(void)
{
	InitTAPAPIFix();
	transparency_orig = TAP_GetSystemVar( SYSVAR_OsdAlpha );
	rgn = TAP_Osd_Create(0, 0, 720, 576, 0, 0);

	TAP_Osd_SetTransparency( rgn, 150 );

	return 1;
}
