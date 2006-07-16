/*
	Copyright (C) 2005-2006 Simon Capewell

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

#include <tap.h>
#include <string.h>
#include <firmware.h>
#include <OPCodes.h>
#include <messagewin.h>
#include "epg.h"


#ifdef CT
#define POSTFIX " CT"
#else
#define POSTFIX ""
#endif

TAP_ID( 0x81243235 );
TAP_PROGRAM_NAME("DescriptionExtender 1.4" POSTFIX);
TAP_AUTHOR_NAME("Simon Capewell");
TAP_DESCRIPTION("Provides long TAP EPG descriptions");
TAP_ETCINFO(__DATE__);


#include <TSRCommander.h>


//----------------------------------------------------------------------------
// This function cleans up and closes the TAP
bool TSRCommanderExitTAP()
{
#ifdef DEBUG
	TAP_Print("Exiting DescriptionExtender\n");
#endif
	UndoFirmwareHacks();
	return TRUE;
}


//-----------------------------------------------------------------------------
// This function displays a config menu
void TSRCommanderConfigDialog()
{
}


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	if ( event == EVT_KEY )
	{
#ifdef DEBUG
		if ( param1 == RKEY_Power )
		{
			TSRCommanderExitTAP();
			TAP_Exit();
			return 0;
		}
#endif
	}
	return param1;
}


//-----------------------------------------------------------------------------
typedef struct {
	Model	model;
    int		firmwareVersion;
	dword	eventTable;
	dword	eventTableLength;
	dword	eventTableType;
} FirmwareDetail;


FirmwareDetail firmware[] = 
{
	// Model		FW version,	Event Table,	Size	Type
	TF5800t,		0x1288,		0x80333284,		14000,	2,		// 14 July 2006
	TF5800t,		0x1225,		0x8032a7c8,		5000,	1,		// 08 Dec 2005
	TF5800t,		0x1209,		0x80326c4c,		5000,	1,		// 15 Sept 2005 
	TF5800t,		0x1205,		0x8032e818,		5000,	1,		// 07 Sept 2005
	TF5800t,		0x1204,		0x8032e698,		5000,	1,		// 01 Sept 2005
	
	TF5000_5500,	0x1205,		0x802d6bb4,		5000,	1,		// 13 Sept 2005
	
	TF5010_5510,	0x1212,		0x802d84e0,		4000,	1,		// 05 Oct 2005
	
	TF5100c,		0x1264,		0x802b5be4,		5000,	1,		// 19 Apr 2006
	TF5100c,		0x1260,		0x802b5498,		5000,	1,		// 15 Mar 2006
	TF5100c,		0x1248,		0x802b5120,		5000,	1,		// 20 Feb 2006
	TF5100c,		0x1205,		0x802ad730,		5000,	1,		// 12 Sept 2005
	TF5100c,		0x1170,		0x802c27b0,		5000,	1,		// 04 May 2005
	
	TF5100,			0x1264,		0x802ae9a0,		5000,	1,		// 19 Apr 2006
	TF5100,			0x1260,		0x802ae474,		5000,	1,		// 15 Mar 2006
	TF5100,			0x1205,		0x802a5cac,		5000,	1,		// 12 Sept 2005

	TF5100c_MP,		0x1266,		0x802d823c,		5000,	1,		// 24 Apr 2006
	TF5100c_MP,		0x1260,		0x802d7b60,		5000,	1,		// 15 Mar 2006
	TF5100c_MP,		0x1212,		0x802cf550,		5000,	1,		// 05 Oct 2005

	TF5100t_MP,		0x1266,		0x802d1040,		5000,	1,		// 24 Apr 2006
	TF5100t_MP,		0x1260,		0x802d0b9c,		5000,	1,		// 15 Mar 2006
	TF5100t_MP,		0x1212,		0x802c7ab4,		5000,	1,		// 04 Oct 2005

	TF5200c,		0x1205,		0x802ac8e8,		5000,	1,		// 13 Sept 2005

	// Dedicated Procaster firmware is no longer developed. Update with TF5100 firmwares instead
	PC5101c_5102c,	0x1260,		0x802b5498,		5000,	1,		// TF5100c 15 Mar 2006 (crossflashed)
	PC5101c_5102c,	0x1212,		0x802a8fe4,		5000,	1,		// 05 Oct 2005

	PC5101t_5102t,	0x1260,		0x802ae474,		5000,	1,		// TF5100 15 Mar 2006 (crossflashed)
	PC5101t_5102t,	0x1212,		0x802a1578,		5000,	1,		// 05 Oct 2005

	// Other crossflashed firmwares - will be removed if they cause problems
	TF5000t,		0x1260,		0x802ae474,		5000,	1		// TF5100 15 Mar 2006
};


void* FW_MemAlloc(dword size)
{
}
void FW_sprintf(void *s, const void *fmt, ...)
{
}
void FW_Print(const void *fmt, ...)
{
}


FirmwareDetail* GetFirmwareDetail()
{
	__asm__ __volatile__ (
		"lui	$02,0x0\n"
		"or		$02,$02,0x0\n"
		"nop\n"
		);
}


char* GetGenreName( byte genre )
{
	return 
		genre == 0 ?  "Unclassified" :
		genre == 1 ?  "Movie/Drama" :
		genre == 2 ?  "News" :
		genre == 3 ?  "Show/Game" :
		genre == 4 ?  "Sport" :
		genre == 5 ?  "Children" :
		genre == 6 ?  "Music/Dance" :
		genre == 7 ?  "Arts/Culture" :
		genre == 8 ?  "Social" :
		genre == 9 ?  "Education" :
		genre == 10 ? "Leisure" : "Unclassified";

/*	switch (genre)
	{
	case 0:		return "Unclassified";
	case 1:		return "Film/Drama";
	case 2:		return "News";
	case 3:		return "Show/Game";
	case 4:		return "Sport";
	case 5:		return "Children";
	case 6:		return "Music/Dance";
	case 7:		return "Arts/Culture";
	case 8:		return "Social";
	case 9:		return "Education";
	case 10:	return "Leisure";
	}
	return "Unclassified";
*/
}


// Read description from the firmware's EPG event table
byte* GetEventDescriptionv1( TYPE_TapEvent* event )
{
	byte* result = NULL;

	if ( event && event->svcId )
	{
		FirmwareDetail* parameters = GetFirmwareDetail();
		if ( parameters )
		{
			type_eventtable_v1* et = (type_eventtable_v1*)parameters->eventTable;
			int eventTableLength = parameters->eventTableLength;

			// Try each entry in the event table. Not very efficient, but the firmware does exactly the same
			int i;
			for ( i=0; i<eventTableLength; ++i )
			{
				type_eventtable_v1* e = &et[i];

				// On valid events, match evtId
				if ( (e->char00 & 0xc0) && event->evtId == e->event_id )
				{
					// Match satIdx, orgNetId, tsId, svcId, evtId
					if ( event->svcId == e->service_id && event->tsId == e->transport_stream_id &&
						 event->orgNetId == e->original_network_id && event->satIdx == e->sat_index )
					{
						int outputLength = 0;
						int descriptionLength = 0;
						// Description is in the 250 byte event name block immediately after event name (no zero terminator)
						byte *description = e->event_name;
						if ( description )
						{
							description += e->event_name_length;
							descriptionLength = strlen(description);
							if ( descriptionLength > 127 )
								outputLength += descriptionLength + 1;
						}
						// Add on the extended info length, plus space for a zero terminator
						if ( e->extended_length > 0 )
							outputLength += e->extended_length + 1;

						// If there's going to be something worth returning
						if ( outputLength > 0 )
						{
							// allocate memory, plus space for a zero terminator
							result = FW_MemAlloc( outputLength+1 );
							if ( result )
							{
								byte* p = result;
								// Append any long description text
								if ( descriptionLength > 127 )
								{
									memcpy( p, description, descriptionLength );
#ifndef CT
									// only append an additional space if there is extended info to append
									if ( e->extended_length > 0 )
									{
										p[descriptionLength] = ' ';
										++p;
									}
#endif
									p += descriptionLength;
								}
								// Append the existing extended infomation
								if ( e->extended_length > 0 )
								{
									memcpy( p, e->extended_event_name, e->extended_length );
									p[e->extended_length] = '\0';
									p += e->extended_length + 1;
								}
								// And terminate with an additional terminator
								p[0] = '\0';
							}
						}
						break;
					}
				}
			}
		}
	}

	return result;
}


// Read description from the firmware's EPG event table
byte* GetEventDescriptionv2( TYPE_TapEvent* event )
{
	byte* result = NULL;

	if ( event && event->svcId )
	{
		FirmwareDetail* parameters = GetFirmwareDetail();
		if ( parameters )
		{
			type_eventtable_v2* et = (type_eventtable_v2*)parameters->eventTable;
			int eventTableLength = parameters->eventTableLength;

			// Try each entry in the event table. Not very efficient, but the firmware does exactly the same
			int i;
			for ( i=0; i<eventTableLength; ++i )
			{
				type_eventtable_v2* e = &et[i];

				// On valid events, match evtId
				if ( (e->char00 & 0xc0) && event->evtId == e->event_id )
				{
					// Match satIdx, orgNetId, tsId, svcId, evtId
					if ( event->svcId == e->service_id && event->tsId == e->transport_stream_id &&
						 event->orgNetId == e->original_network_id && event->satIdx == e->sat_index )
					{
						int outputLength = 0;
						int descriptionLength = 0;
						char* genre;
						int genreLength = 0;

						// Description is in the 250 byte event name block immediately after event name (no zero terminator)
						byte *description = e->event_name;
						if ( description )
						{
							description += e->event_name_length;
							descriptionLength = strlen(description);
							if ( descriptionLength > 127 )
								outputLength += descriptionLength + 1;
						}
						// Add on the extended info length, plus space for a zero terminator
						if ( e->extended_length > 0 )
							outputLength += e->extended_length + 1;

						// Add on the genre length, plus space for brackets and a zero terminator
						genre = GetGenreName( e->char74 );
						if ( genre )
						{
							genreLength = strlen( genre );
							outputLength += genreLength + 3;
						}

						// If there's going to be something worth returning
						if ( outputLength > 0 )
						{
							// allocate memory, plus space for a zero terminator
							result = FW_MemAlloc( outputLength+1 );
							if ( result )
							{
								byte* p = result;
								// Append any long description text
								if ( descriptionLength > 127 )
								{
									memcpy( p, description, descriptionLength );
#ifndef CT
									// only append an additional space if there is extended info to append
									if ( e->extended_length > 0 )
									{
										p[descriptionLength] = ' ';
										++p;
									}
#endif
									p += descriptionLength;
								}
								// Append the existing extended infomation
								if ( e->extended_length > 0 )
								{
									memcpy( p, e->extended_event_name, e->extended_length );
									p[e->extended_length] = '\0';
									p += e->extended_length + 1;
								}
								// Append the genre
								if ( genre )
								{
									*p++ = '[';
									memcpy( p, genre, genreLength );
									p += genreLength;
									*p++ = ']';
									*p++ = '\0';
								}
								// And terminate with an additional terminator
								p[0] = '\0';
							}
						}
						break;
					}
				}
			}
		}
	}

	return result;
}

int TAP_Main(void)
{
	dword eventTable;
	bool supported = FALSE;
	int index;

	// Look up the current firmware
	for (index=0; index<sizeof(firmware)/sizeof(FirmwareDetail); ++index)
	{
		if ( GetModel() == firmware[index].model && _appl_version == firmware[index].firmwareVersion )
		{
			supported = TRUE;
			break;
		}
	}

	if ( !supported )
	{
		// Try and get some helpful information of users with unsupported firmware
		char buffer[300];
		sprintf( buffer, "Description Extender is not compatible with your firmware\n"
			"For an update, please contact the author quoting\n"
			"%d, %04X, %X and %X for an update", *sysID, _appl_version, TAP_GetCurrentEvent-0x80000000, TAP_EPG_GetExtInfo-0x80000000 );
		ShowMessage( buffer, 750 );
		return 0;
	}

	// We need to know the address of the firmware event table without needing to use $gp
	((word*)GetFirmwareDetail)[1] = ((dword)(firmware+index) >> 16) & 0xffff;
	((word*)GetFirmwareDetail)[3] = (dword)(firmware+index) & 0xffff;

	// Replace TAP_EPG_GetExtInfo with a jump to our GetEventDescription function
	HackFirmware( (dword*)TAP_EPG_GetExtInfo, firmware[index].eventTableType == 1 ? J(GetEventDescriptionv1) : J(GetEventDescriptionv2) );
	HackFirmware( ((dword*)TAP_EPG_GetExtInfo)+1, NOP_CMD ); // MUST insert a nop after the jump

	// Set up TAP API wrappers that don't require the use of $gp to be called
	((dword*)FW_MemAlloc)[0] = J(TAP_MemAlloc);
	((dword*)FW_MemAlloc)[1] = NOP_CMD;
	// Useful for debugging
	((dword*)FW_sprintf)[0] = J(TAP_SPrint);
	((dword*)FW_sprintf)[1] = NOP_CMD;
	((dword*)FW_Print)[0] = J(TAP_Print);
	((dword*)FW_Print)[1] = NOP_CMD;

	return 1;
}
