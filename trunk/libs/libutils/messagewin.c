#include <string.h>
#include <tap.h>
#include "strtok.h"
#include "messagewin.h"


//---------------------------------------  ShowMessage --------------------------------
void ShowMessage( const char* message, dword delay )
{
	dword rgn;
	const int lineHeight = 26;
	int width, height, x,y;
	char* c;
	char buffer[1024];

	TAP_Print( message );
	strncpy( buffer, message, sizeof(buffer)-1 );
	buffer[1023] = 0;

	// Calculate maximum width
	width = 100;
	height = 0;
	c = strtok( buffer, "\n" );
	while ( c )
	{
		width = max( width, TAP_Osd_GetW( c, 0, FNT_Size_1926 )+10 );		// calculate width of message
		c = strtok( NULL, "\n" );
		height += lineHeight;
	}
	width = min( width, 720 );
	height = max( height, lineHeight );

	rgn = TAP_Osd_Create( 0, 0, 720, 576, 0, FALSE );						// create rgn-handle
	x = (720-width)/2;
	y = (578-height)/2;
	TAP_Osd_FillBox( (word)rgn, (word)(x-5),(word)(y-5), (word)(width+10),(word)(height+10), RGB(19,19,19) );		// draw background-box for border

	strncpy( buffer, message, sizeof(buffer)-1 );
	buffer[1023] = 0;
	c = strtok( buffer, "\n" );
	while ( c )
	{
		TAP_Osd_PutS( (word)rgn, x,y, x+width, c,		
			RGB(31,31,31), RGB(3,5,10), 0, FNT_Size_1926,
			FALSE, ALIGN_CENTER );
		y += lineHeight;
		c = strtok( NULL, "\n" );
	}

	TAP_Delay( delay );														// show it for dwDelay /100 seconds
	TAP_Osd_Delete( (word) rgn );													// release rgn-handle
}
