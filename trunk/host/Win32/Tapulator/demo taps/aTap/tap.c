#include "tap.h"

#include "graphics.h"
#include "graphics.c"
#include "smallgreenbar.c"

#define GRAPHIC _smallgreenbarGd

word rgn;

dword TAP_EventHandler( word event, dword param1, dword param2 )
{
    if( event == EVT_KEY )
    {
	    TAP_Osd_Delete( rgn );
		TAP_EnterNormal();
        TAP_Exit();
	    return 0;
    }
    return event;
}


int TAP_Main(void)
{
	TAP_ExitNormal();

	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );
	//TAP_Osd_FillBox( rgn, 0,0, 720,576, 0xffff);
    //TAP_Osd_PutGd( rgn, (720-GRAPHIC.width)/2 , (576-GRAPHIC.height)/2, &GRAPHIC, FALSE );
	TAP_Osd_DrawPixmap( rgn, (720-smallgreenbar_Width)/2 , (576-smallgreenbar_Height)/2, smallgreenbar_Width,smallgreenbar_Height, smallgreenbarData, 0, OSD_1555 );
	TAP_Osd_DrawPixmap( rgn, (720-smallgreenbar_Width)/2 , 100, smallgreenbar_Width,smallgreenbar_Height, g_pBitmap, 0, OSD_1555 );
    //TAP_Osd_PutGd( rgn, (720-_redcircleGd.width)/2 , (576-_redcircleGd.height)/2, &_redcircleGd, TRUE );

    return 1;
}
