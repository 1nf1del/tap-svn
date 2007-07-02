#include "localtap.h"
#include "RasterFontBrowser.h"
#include "tapapifix.h"
#include <string.h>
#define ID_FREETYPE_TEST			0x07

TAP_ID( ID_FREETYPE_TEST );
TAP_PROGRAM_NAME("Font test");
TAP_AUTHOR_NAME("Justason & simonc");
TAP_DESCRIPTION("FontTest");
TAP_ETCINFO(__DATE__);


int TAP_Main(void);
word rgn;
int _appl_version = 0x1200;


void Draw1(FONT *font, unsigned char *msg1, int filelen)
{
	unsigned char ptr1[256];
	int err1, width, text_length;
	int i, j, lines;
	word *image=NULL;


	SetColors(font, COLOR_Yellow, COLOR_DarkBlue);
	i=j=0;
	lines=0;
	do 
	{
		if (msg1[i]==0x0A || i==filelen)
		{
			strncpy((char*)ptr1, (const char*)(msg1+j), i-j-1);
			ptr1[i-j-1]=0;

			width=0;
			err1 = CalcSize(font, ptr1, &width, &text_length);
			if (err1==0)
			{
				err1 = RenderString(font, ptr1, text_length, width, &image);
				if (err1==0)
				{
					TAP_Osd_DrawPixmap(rgn, 40, 100+42*lines, width, font->m_fontHeight, image, FALSE, OSD_1555);
					TAP_MemFree(image);
				}
			}
			j=i+1;
			lines++;
		}
		i++;
	} while(i<=filelen);
}

void Draw2(unsigned char *msg1, int filelen)
{
	unsigned char ptr1[256];
	int i, j, lines;
	word *image=NULL;


	i=j=0;
	lines=0;
	do 
	{
		if (msg1[i]==0x0A || i==filelen)
		{
			strncpy((char*)ptr1, (const char*)(msg1+j), i-j-1);
			ptr1[i-j-1]=0;
			TAP_Osd_PutString(rgn, 40, 100+22*lines, 400, ptr1, COLOR_Yellow, COLOR_DarkBlue, 0, FNT_Size_1419, 0);
			j=i+1;
			lines++;
		}
		i++;
	} while(i<=filelen);
}

void Draw3(FONT *font, unsigned char *msg1, int filelen)
{
	unsigned char ptr1[256];
	int err1, width, text_length;
	int i, j, lines;
	word *image=NULL;


	SetColors(font, COLOR_Yellow, COLOR_DarkBlue);

	i=j=0;
	lines=0;
	do 
	{
		if (msg1[i]==0x0A || i==filelen)
		{
			strncpy((char*)ptr1, (const char*)(msg1+j), i-j-1);
			ptr1[i-j-1]=0;

			width=0;
			err1 = CalcSize(font, ptr1, &width, &text_length);
			if (err1==0)
			{
				RenderStringDirect(font, ptr1, text_length, 40, 100+22*lines, width);
			}
			j=i+1;
			lines++;
		}
		i++;
	} while(i<=filelen);
}

dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	static int count = 0, i, err1, filelen;
	char msg[256], msg1[256], heading[]="Running 100 times";
	dword start, stop;
	TYPE_File *f1;
	FONT font;



	if( event == EVT_KEY )
	{
		if ( param1 == RKEY_Exit)
		{
			TAP_Osd_Delete( rgn );
			TAP_EnterNormal();
			TAP_Exit();
			return 0;
		}
		if ( param1 == RKEY_1)
		{
			TAP_ExitNormal();
			TAP_Osd_FillBox( rgn, 20, 20, 680, 536, COLOR_Gray );
			Draw_Font_String(rgn, 40, 20, heading, "calibri_iso8859-5_21.rasterized", COLOR_Yellow, COLOR_DarkBlue);

			f1 = TAP_Hdd_Fopen("msg_eng.txt");
			if (f1==0) return 0;

			filelen = TAP_Hdd_Fread((void*)msg1, 1, 330, f1);
			msg1[filelen]=0;

			err1 = Load_Font(&font, "calibri_iso8859-1_18.rasterized");
			if (err1!=0) return 0;

			start = TAP_GetTick();
			for (i=0; i<100; i++)
			{
				Draw1(&font, msg1, filelen);
			}
			stop = TAP_GetTick();

			TAP_Hdd_Fclose(f1);
			Delete_Font(&font);

			TAP_SPrint(msg, "Duration: %lu ticks", stop-start);
			Draw_Font_String(rgn, 220, 20, msg, "calibri_iso8859-5_21.rasterized", COLOR_Yellow, COLOR_DarkBlue);

			return 0;
		}

		if ( param1 == RKEY_2)
		{
			TAP_ExitNormal();
			TAP_Osd_FillBox( rgn, 20,20, 680, 536, COLOR_DarkGray );
			TAP_Osd_PutString(rgn, 40, 20, 600, heading, COLOR_Yellow, COLOR_DarkBlue, 0, FNT_Size_1419, 0);

			f1 = TAP_Hdd_Fopen("msg_eng.txt");
			if (f1==0) return 0;

			filelen = TAP_Hdd_Fread((void*)msg1, 1, 330, f1);
			msg1[filelen]=0;

			start = TAP_GetTick();
			for (i=0; i<100; i++)
			{
				Draw2(msg1, filelen);
			}
			stop = TAP_GetTick();

			TAP_Hdd_Fclose(f1);

			TAP_SPrint(msg, "Duration: %lu ticks", stop-start);
			TAP_Osd_PutString(rgn, 40, 400, 600, msg, COLOR_Yellow, COLOR_DarkBlue, 0, FNT_Size_1419, 0);

			return 0;
		}


		if ( param1 == RKEY_3)
		{
			TAP_ExitNormal();
			TAP_Osd_FillBox( rgn, 20, 20, 680, 536, COLOR_Gray );
			Draw_Font_String(rgn, 40, 20, heading, "calibri_iso8859-5_21.rasterized", COLOR_Yellow, COLOR_DarkBlue);

			f1 = TAP_Hdd_Fopen("msg_eng.txt");
			if (f1==0) return 0;

			filelen = TAP_Hdd_Fread((void*)msg1, 1, 330, f1);
			msg1[filelen]=0;

			err1 = Load_Font(&font, "calibri_iso8859-1_18.rasterized");
			if (err1!=0) return 0;

			InitDirect();
			start = TAP_GetTick();
			for (i=0; i<100; i++)
			{
				Draw3(&font, msg1, filelen);
			}
			stop = TAP_GetTick();

			TAP_Hdd_Fclose(f1);
			Delete_Font(&font);

			TAP_SPrint(msg, "Duration: %lu ticks", stop-start);
			Draw_Font_String(rgn, 220, 20, msg, "calibri_iso8859-5_21.rasterized", COLOR_Yellow, COLOR_DarkBlue);

			return 0;
		}
	}
	return param1;//event;
}


int TAP_Main(void)
{

	InitTAPAPIFix();
	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );

	return 1;
}
