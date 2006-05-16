/*
	Copyright (C) 2005-2006 Darkmatter

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

// This module displays the current time in a bar


void UpdateListClock(void)
{
	char	str[80], str2[20], str3[20];
	byte 	hour, min, sec, month, day, weekDay;
	word 	year, mjd;
	dword	width;

	TAP_GetTime( &mjd, &hour, &min, &sec);
	TAP_ExtractMjd( mjd, &year, &month, &day, &weekDay) ;

//	weekDay=2; month=8; day=28; hour=12; min=58;		// use for testing width
	
	switch ( weekDay )
	{
		case 0:	TAP_SPrint(str2,"Mon"); break;
		case 1:	TAP_SPrint(str2,"Tue"); break;
		case 2:	TAP_SPrint(str2,"Wed"); break;
		case 3:	TAP_SPrint(str2,"Thu"); break;
		case 4:	TAP_SPrint(str2,"Fri"); break;
		case 5:	TAP_SPrint(str2,"Sat"); break;
		case 6:	TAP_SPrint(str2,"Sun"); break;
		default : TAP_SPrint(str2,"BAD"); break;
	}

	switch ( month )
	{
		case 1:	TAP_SPrint(str3,"Jan"); break;
		case 2:	TAP_SPrint(str3,"Feb"); break;
		case 3:	TAP_SPrint(str3,"Mar"); break;
		case 4:	TAP_SPrint(str3,"Apr"); break;
		case 5:	TAP_SPrint(str3,"May"); break;
		case 6:	TAP_SPrint(str3,"Jun"); break;
		case 7:	TAP_SPrint(str3,"Jul"); break;
		case 8:	TAP_SPrint(str3,"Aug"); break;
		case 9:	TAP_SPrint(str3,"Sep"); break;
		case 10:TAP_SPrint(str3,"Oct"); break;
		case 11:TAP_SPrint(str3,"Nov"); break;
		case 12:TAP_SPrint(str3,"Dec"); break;
		default : TAP_SPrint(str3,"BAD"); break;
	}

//	TAP_SPrint( str, "%s %d %s %02d:%02d:%02d", str2, day, str3, hour, min, sec);		// includes seconds - not normally shown
	TAP_SPrint( str, "%s %d %s %02d:%02d", str2, day, str3, hour, min);
	width = TAP_Osd_GetW( str, 0, FNT_Size_1622 );

	TAP_Osd_PutGd( rgn, 494, 34, &_timebarGd, TRUE );
	TAP_Osd_PutStringAf1622( rgn, 581-(width/2), 39, 660, str, TIME_COLOUR, 0 );
}


