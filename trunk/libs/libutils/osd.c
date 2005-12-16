/*
	Copyright (C) 2005 Simon Capewell

	This file is part of the TAPs for Topfield PVRs project.
		http://tap.berlios.de/

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <tap.h>
#include "osd.h"


bool OsdActive()
{
    TYPE_OsdBaseInfo osdBaseInfo;
    dword* wScrn;
    dword iRow, iCol;

    TAP_Osd_GetBaseInfo( &osdBaseInfo );

	// scan every 4th line
    for ( iRow = 200; iRow < 350; iRow += 4 )
    {
		// gcc treats sizeof(void) as 1, other compilers do not so explicitly cast to char*
        wScrn = (dword*)((char*)osdBaseInfo.eAddr + 720 * iRow); 

		// scan every 6 pixels to save time and resource
		// stop at 400 as new f/w puts channel details to the right.
        for ( iCol = 300; iCol < 400; iCol += 6 )
        {
            if ( (*(wScrn + iCol)) != 0 )
                return TRUE; // Do not allow Key Action
        }
    }
    return FALSE;
}
