/*
	Copyright (C) Bjorn

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	The software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this software; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <string.h>
#include <tap.h>
#include "ScreenCapture.h"


#define _Clip(a)		(a)>255 ? 255 : (a)<0 ? 0 : (a)

#define _R(y,u,v) ((0x2568*(y)			   + 0x3343*(u)) >>13)//0x2000
#define _G(y,u,v) ((0x2568*(y) - 0x0c92*(v) - 0x1a1e*(u))>>13)//0x2000
#define _B(y,u,v) ((0x2568*(y) + 0x40cf*(v)) 			 >>13)//0x2000

#define LE16( p )	( (((p&0x00ff))<<8) | ((p&0xff00)>>8) )
#define LE32( p )	( (((p&0x000000ff))<<24) | (((p&0x0000ff00))<<8) | (((p&0x00ff0000))>>8) | ((p&0xff000000)>>24) )

typedef struct _BMP_HEAD {
    byte id[2];
	word size_l;
	word size_h;
    word reserved[2];
    word offset_l;
	word offset_h;
} BMP_HEAD;

typedef struct _BMP_INFO {
    long info_size;
    long width;
    long height;
    word plane;
    word bits_per_pixel;
    long compress;
    long img_size;
    long x_res;
    long y_res;
    long color;
    long icolor;
} BMP_INFO;


// ------------------------------ YUV2RGB ------------------------------------
void YUV2RGB( word yy, word uu, word vv, byte *r, byte *g, byte *b )
{
	signed int _r,_g,_b;
	signed int y, u, v;

	y = yy; // - 12;
	u = (int)uu - 128;
	v = (int)vv - 128;

	_r = _R(y,u,v);
	_g = _G(y,u,v);
	_b = _B(y,u,v);

	*r = _Clip(_r);
	*g = _Clip(_g);
	*b = _Clip(_b);
}


// ------------------------------ SaveBitmap ------------------------------------
bool SaveBitmap(char *strName, int width, int height, byte* buffer )
{
	TYPE_File *file;
	if( !buffer || !strName ) 
		return FALSE;

	TAP_Hdd_Create( strName, ATTR_NORMAL );
	file = TAP_Hdd_Fopen( strName );
	if ( !file )
		return FALSE;

	// Write Header
	BMP_WriteHeader( file, width, height );
	// write bitmap data
	TAP_Hdd_Fwrite( buffer, width*3, height, file );

	TAP_Hdd_Fclose( file );

	return TRUE;
}


// ------------------------------ CaptureScreen ------------------------------------
//
bool CaptureScreen( byte *buffer, int width, int height, byte mainSub, ScreenCaptureFlags flags )
{
    TYPE_OsdBaseInfo osdBaseInfo;
	int nR, nG, nB, alpha, n1, n2, i;
	word color;	
	byte *p;
	TYPE_VideoBank videoBank;
	byte *picAddr, *chrAddr;
	int	x, y, xx, yy;
	dword xinc, yinc;
	byte r, g, b;
	int ypos, ypos2, xpos, yPos, cPos;
	word *_vAddr[576];
	bool result = TRUE;

	// get base info and set vertical addresses
	if ( flags & SCREENCAP_OSD )
	{
		TAP_Osd_GetBaseInfo( &osdBaseInfo );
		for( i=0; i<576; i+=2 )
		{
			_vAddr[i]	= osdBaseInfo.eAddr + 720*i;
			_vAddr[i+1] = osdBaseInfo.oAddr + 720*i;
		}
		// get alpha for osd
		alpha = (flags & SCREENCAP_ALPHA) ? TAP_GetSystemVar( SYSVAR_OsdAlpha ) : 0;
		n1 = alpha;
		n2 = 100 - alpha;
	}


	// clear videoBank
	memset( &videoBank, 0, sizeof(videoBank) );

	// capture screen and set adress
	if ( flags & SCREENCAP_TV )
	{
		if ( TAP_CaptureScreen( mainSub, &videoBank ) != 0 )
			result = FALSE;
		if ( result )
		{
			picAddr = videoBank.yAddress;
			chrAddr = videoBank.cAddress;

			// if size ok ?
			if ( videoBank.width == width && videoBank.height == height )
			{
				// convert from uyvy to rgb mode and don't scale image
				for( y = 0; y<height; y++ ) 
				{
 					ypos  = (( y    & 0xffffffe0) * 736)  + ((y&0x1f)<<5);		//(y&0x1f)*32;
					ypos2 = (((y/2) & 0xffffffe0) * 736)  + (((y/2)&0x1f)<<5);	//((y/2)&0x1f)*32;
					for( x=0; x<width; x++ )
					{
						xpos =  ((x & 0xffffffe0)<<5) + (x&0x1f);
						yPos  = ypos  + xpos;
						cPos = (ypos2 + xpos)&0xfffffffe;

						// get rgb screen value
						YUV2RGB( picAddr[yPos], chrAddr[cPos+1], chrAddr[cPos], &r, &g, &b );
						
						// get osd value
						if ( flags & SCREENCAP_OSD )
						{
							color = *(_vAddr[y] + x);
							if ( color != 0 )
							{
								nR = ( (color >> 10) & 0x1f ) << 3;
								nG = ( (color >> 5 ) & 0x1f ) << 3;
								nB = ( (color      ) & 0x1f ) << 3;
								nR = (n1*r + n2*nR)/ 100;
								nG = (n1*g + n2*nG)/ 100;
								nB = (n1*b + n2*nB)/ 100;
								r = _Clip(nR);
								g = _Clip(nG);
								b = _Clip(nB);
							} // end if color
						} // end if osd
							
						// set pixel value in buffer bottom -> top
						p = buffer + 3*( ((height - y -1)* width) + x);
						p[0] = b;
						p[1] = g;
						p[2] = r;
					} // end for x
				} // end for y
			}
			else
			{
				// convert from uyvy to rgb mode and scale image
				for( yy = 0; yy<height; yy++ ) 
				{
					y = yy * videoBank.height / height;
 					ypos  = (( y    & 0xffffffe0) * 736)  + ((y&0x1f)<<5);		//(y&0x1f)*32;
					ypos2 = (((y/2) & 0xffffffe0) * 736)  + (((y/2)&0x1f)<<5);	//((y/2)&0x1f)*32;

					for( xx=0; xx<width; xx++ )
					{
						x = xx * videoBank.width / width;
						xpos =  ((x & 0xffffffe0)<<5) + (x&0x1f);
						yPos  = ypos  + xpos;
						cPos = (ypos2 + xpos)&0xfffffffe;

						// get rgb screen value
						YUV2RGB( picAddr[yPos], chrAddr[cPos+1], chrAddr[cPos], &r, &g, &b );
						
						// get osd value
						if ( flags & SCREENCAP_OSD )
						{
							color = *(_vAddr[yy] + xx);
							if ( color != 0 )
							{
								nR = ( (color >> 10) & 0x1f ) << 3;
								nG = ( (color >> 5 ) & 0x1f ) << 3;
								nB = ( (color      ) & 0x1f ) << 3;
								nR = (n1*r + n2*nR)/ 100;
								nG = (n1*g + n2*nG)/ 100;
								nB = (n1*b + n2*nB)/ 100;
								r = _Clip(nR);
								g = _Clip(nG);
								b = _Clip(nB);
							} // end if color
						} // end if osd
							
						// set pixel value in buffer bottom -> top
						p = buffer + 3*( ((height - yy -1)* width) + xx);
						p[0] = b;
						p[1] = g;
						p[2] = r;
					} // end for x
				} // end for y
			} // size
		} // if result

		// free resources
		TAP_MemFree( videoBank.cAddress );
		TAP_MemFree( videoBank.yAddress );
	}
	else if ( flags & SCREENCAP_OSD )
	{
		// convert from uyvy to rgb mode and don't scale image
		for( y = 0; y<height; y++ ) 
		{
			for( x=0; x<width; x++ )
			{
				color = *(_vAddr[y] + x);
				nR = ( (color >> 10) & 0x1f ) << 3;
				nG = ( (color >> 5 ) & 0x1f ) << 3;
				nB = ( (color      ) & 0x1f ) << 3;
				r = (n2*nR)/ 100;
				g = (n2*nG)/ 100;
				b = (n2*nB)/ 100;
				// set pixel value in buffer bottom -> top
				p = buffer + 3*( ((height - y -1)* width) + x);
				p[0] = b;
				p[1] = g;
				p[2] = r;
			} 
		}
	}

	return result;	
}


// ------------------------------ BMP_WriteHeader ------------------------------------
//
bool BMP_WriteHeader(TYPE_File *file, int width, int height )
{
	static BMP_HEAD head;
	static BMP_INFO info;
	long size;
	int i;

	size = sizeof( head ) + sizeof( info ) + (width*height)*3;

	head.id[0]			= 'B';
	head.id[1]			= 'M';
	head.size_l			= LE16( (size&0xffff) );
	head.size_h			= LE16( (size>>16) );
	i					=  (sizeof( head ) + sizeof( info ));
	head.offset_l		= LE16( i&0xffff );
	head.offset_h		= LE16( i>>16 );

	info.info_size 		= LE32( 40 );
	info.width			= LE32( width );
	info.height			= LE32( height );
	info.plane			= LE16( 1 );
	info.bits_per_pixel	= LE16( 24 );
	info.compress       = LE32( 0 );
	info.img_size       = LE32( (width*height)*3 );
	info.x_res          = 0;
	info.y_res          = 0;
	info.color          = 0;
	info.icolor         = 0;

	TAP_Hdd_Fwrite( &head, sizeof( head ), 1, file );
	TAP_Hdd_Fwrite( &info, sizeof( info ), 1, file );	
}


TYPE_TapEvent *GetCurrentEvent(int *nCurEvent)
{
	TYPE_TapEvent *evtInfo;
	unsigned int eventNum; 
	int svcType, svcNum;
	int i;

	TAP_Channel_GetCurrent( &svcType, &svcNum );				// get current channel
	if ( svcType != SVC_TYPE_Tv )
		return NULL;											// cancel if not in TV mode
	evtInfo = TAP_GetEvent( svcType, svcNum, &eventNum );		// get events

	if(evtInfo) 
    { 
        for( i=0; i<eventNum; i++ )								// find current event
        { 
            if( evtInfo[i].runningStatus == 4 )					// current event found
                break; 
        } 
    } 

    if( evtInfo && evtInfo[i].runningStatus == 4) 
    { 
		if ( nCurEvent )
			*nCurEvent = i;
		return evtInfo;
	}

	if ( evtInfo )
		TAP_MemFree( evtInfo );
	return NULL;
}


// ----------------------------- Getfilename ---------------------------------
word isLegalChar(unsigned char c)
{
	word retValue = 1;	
	if (c < 32)
	{
		// Anything below 32 is a control character!
		retValue = 0;
	}
	else
	{
		switch (c)
		{
			// These characters are not allowed in Windows.
			case ':':
			case '/':
			case '\\':
			case '*':
			case '?':
			case '"':
			case '<':
			case '>':
			case '¦':
			case '|':
				retValue = 0;
				break;
			
			default:
				break;
		}
	}
	return(retValue);
}

void makeValidFileName(unsigned char *strName)
{
	unsigned char *p, c;
	int i;
	int fileNameLength;
	
	p = strName;
	i = 0;
	fileNameLength = strlen(strName);
	while (i < fileNameLength)
	{
		c = strName[i];
		if (isLegalChar(c))
		{
			*p = c;
			p++;
		}
		i++;
	}
	*p = '\0';
}

void GetFileName(unsigned char *name )
{
	int tvRadio, chNum;
	TYPE_TapChInfo chInfo;
	TYPE_TapEvent* pEvent = NULL;
	int evtNum;
	char tempName[128];
	int number;

	// get event or channel name
	pEvent = GetCurrentEvent( &evtNum );
	if ( pEvent )
		TAP_SPrint( tempName, "%.80s", pEvent[evtNum].eventName );
	else
	{
		TAP_Channel_GetCurrent( &tvRadio, &chNum );
		TAP_Channel_GetInfo( tvRadio,  chNum, &chInfo );
		TAP_SPrint( tempName, "%.80s", chInfo.chName );
	}
	// free resources
	if ( pEvent )
		TAP_MemFree( pEvent );

	// remove any illegal chars!
	makeValidFileName(tempName);

	// now search for an unused number ( if file already exists )
	number = 1;
	TAP_SPrint( name, "%.80s.bmp", tempName );

	while ( TAP_Hdd_Exist( name ) )
	{
		TAP_SPrint( name, "%.80s-%02d.bmp", tempName, ++number );
	}
}


//-------------------------------- DoScreenCapture ---------------------------
#define SCREEN_WIDTH 720
#define SCREEN_HEIGHT 576

void CaptureScreenToFile( ScreenCaptureFlags flags )
{
	char fileName[128];
	byte* bitmap = TAP_MemAlloc(SCREEN_WIDTH*SCREEN_HEIGHT*4);
	if ( bitmap == 0)
		return;

	// get a expedient filename
	TAP_Hdd_ChangeDir("/");
	TAP_Hdd_Create("ScreenShots", ATTR_FOLDER);
	TAP_Hdd_ChangeDir("ScreenShots");

	GetFileName( fileName );
	// capture screen + OSD
	if ( CaptureScreen( bitmap, SCREEN_WIDTH, SCREEN_HEIGHT, CHANNEL_Main, flags ) )
	{
		TAP_Print( "Captured to /ScreenShots/%s\n", fileName );
		// save bitmap
		SaveBitmap( fileName, SCREEN_WIDTH, SCREEN_HEIGHT, bitmap );
	}

	TAP_MemFree(bitmap);
}
