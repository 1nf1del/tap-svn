#include <stdlib.h>

#include <tap.h>

#define ID_TAP	0x80100217
#define TAP_NAME	"menulibc v0.1"

TAP_ID(ID_TAP);
TAP_PROGRAM_NAME(TAP_NAME);
TAP_AUTHOR_NAME("Steve Bennett");
TAP_DESCRIPTION("Menu libc");
TAP_ETCINFO(__DATE__);

#include <string.h>

static int iMenuRgn;
static TYPE_Window iMenuWin;

void MenuWinCreate( int iRows, int iWidth )
{
	int	iCol, iRow, iHeight;

	iHeight = iRows * 20;
	if ( iRows > 10 ) {
		iHeight = iHeight + 20;
	}

	iCol = ( ( 720 - iWidth ) / 2 ) - 25;
	iRow = ( ( 576 - iHeight ) / 2 ) - 60;

	if (!iMenuRgn) 
	{
		TAP_ExitNormal();
		iMenuRgn = TAP_Osd_Create ( 0, 0, 720, 576, 0, FALSE );
	}
	TAP_Win_SetDefaultColor (&iMenuWin);
	TAP_Win_Create ( &iMenuWin, iMenuRgn, iCol, iRow, iWidth, iHeight, TRUE, FALSE );
}

dword	TAP_EventHandler ( word wEvent, dword dwKey, dword dwParam2 )
{
	if (wEvent == EVT_KEY) {
		if (dwKey == RKEY_Exit) {
			TAP_Win_Delete( &iMenuWin);
			TAP_Osd_Delete(iMenuRgn);
			TAP_EnterNormal();
			TAP_Exit();
		}
		else if (dwKey == RKEY_ChUp || dwKey == RKEY_ChDown) {
			TAP_Win_Action(&iMenuWin, dwKey);
			return 0;
		}
	}

	return dwKey; 
}

int	TAP_Main (void)
{
	TAP_ExitNormal();
	MenuWinCreate( 11, 250 );
	TAP_Win_SetTitle( &iMenuWin, TAP_NAME, 0, FNT_Size_1622 );

	TAP_Win_AddItem( &iMenuWin, strchr("Menu Item 1", 'I'));
	TAP_Win_AddItem( &iMenuWin, strchr("Menu Item 2", 'M'));
	TAP_Win_AddItem( &iMenuWin, strchr("Menu Item 3", 'e'));
	TAP_Win_AddItem( &iMenuWin, strchr("Menu Item 4", 't'));
	TAP_Win_AddItem( &iMenuWin, strchr("Menu Item 5", ' '));

	TAP_Win_SetSelection( &iMenuWin, 3);

	return 1;
}
