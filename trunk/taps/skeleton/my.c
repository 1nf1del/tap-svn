/* $Id$
*
* Copyright (c) 2004-2005 Peter Urbanec. All Rights Reserved.
*
*  This file is part of the TAPs for Topfield PVRs project.
*      http://tap.berlios.de/
*
*  This is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  The software is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this software; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* A do nothing TAP to demonstrate how to set up a project. */

#include <tap.h>

TAP_ID(0x8f00c0de);
TAP_AUTHOR_NAME("peteru");
TAP_PROGRAM_NAME("Do Nothing sample.");
TAP_DESCRIPTION("$Id$");
TAP_ETCINFO(__DATE__);

dword TAP_EventHandler(word event, dword param1, dword param2)
{
    return param1;
}

int TAP_Main(void)
{
    return 0;
}

