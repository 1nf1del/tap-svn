/*
	Copyright (C) 2005-2007 Simon Capewell

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
#include <tapconst.h>
#include <string.h>
#include <firmware.h>
#include <OPCodes.h>
#include <messagewin.h>
#include <model.h>
#include "DescriptionExtender.h"
#include "EPG.h"
#include "Settings.h"


#define LOGFILE "DescriptionExtenderLog.txt"


// $gp independent calls to TAP API functions
static void* FW_MemAlloc(dword size)
{
}
static void FW_sprintf(void *s, const void *fmt, ...)
{
}
static void FW_Print(const void *fmt, ...)
{
}


typedef struct
{
    Settings*	settings;
	dword		eventTable;
	word		eventTableLength;
	word		eventLength;
	byte		extendedLengthOffset;
	byte		extendedEventNameOffset;
	byte		genreOffset;
} FirmwareDetail;

FirmwareDetail firmwareDetail;

// $gp independent running parameters accessor
static FirmwareDetail* GetFirmwareDetail()
{
	__asm__ __volatile__ (
		"lui	$02,0x0\n"
		"or		$02,$02,0x0\n"
		"nop\n"
		);
}


// Map of genre/subgenre to description
typedef struct
{
    byte genre;
    byte subgenre;
    byte description[0x40];
} GenreTable;

GenreTable genres[] = {
    { 0x0,0x00,"Unclassified" },

    { 0x1,0x00,"Movie/Drama" },
    { 0x1,0x01,"detective/thriller" },
    { 0x1,0x02,"adventure/western/war" },
    { 0x1,0x03,"science fiction/fantasy/horror" },
    { 0x1,0x04,"comedy" },
    { 0x1,0x05,"soap/melodrama/folkloric" },
    { 0x1,0x06,"romance" },
    { 0x1,0x07,"serious/classical/religious/historical movie/drama" },
    { 0x1,0x08,"adult movie/drama" },

    { 0x2,0x00,"News/Current Affairs" },
    { 0x2,0x01,"news/weather report" },
    { 0x2,0x02,"news magazine" },
    { 0x2,0x03,"documentary" },
    { 0x2,0x04,"discussion/interview/debate" },

    { 0x3,0x00,"Show/Game Show" },
    { 0x3,0x01,"game show/quiz/contest" },
    { 0x3,0x02,"variety show" },
    { 0x3,0x03,"talk show" },

    { 0x4,0x00,"Sports" },
    { 0x4,0x01,"special sports events" },
    { 0x4,0x02,"sports magazines" },
    { 0x4,0x03,"football/soccer" },
    { 0x4,0x04,"tennis/squash" },
    { 0x4,0x05,"team sports" },
    { 0x4,0x06,"athletics" },
    { 0x4,0x07,"motor sport" },
    { 0x4,0x08,"water sport" },
    { 0x4,0x09,"winter sports" },
    { 0x4,0x0a,"equestrian" },
    { 0x4,0x0b,"martial sports" },

    { 0x5,0x00,"Children's/Youth Programmes" },
    { 0x5,0x01,"pre-school children's programmes" },
    { 0x5,0x02,"entertainment programmes for 6 to 14" },
    { 0x5,0x03,"entertainment programmes for 10 to 16" },
    { 0x5,0x04,"informational/educational/school programmes" },
    { 0x5,0x05,"cartoons/puppets" },

    { 0x6,0x00,"Music/Ballet/Dance" },
    { 0x6,0x01,"rock/pop" },
    { 0x6,0x02,"serious music/classical music" },
    { 0x6,0x03,"folk/traditional music" },
    { 0x6,0x04,"jazz" },
    { 0x6,0x05,"musical/opera" },
    { 0x6,0x06,"ballet" },

    { 0x7,0x00,"Arts/Culture (without music)" },
    { 0x7,0x01,"performing arts" },
    { 0x7,0x02,"fine arts" },
    { 0x7,0x03,"religion" },
    { 0x7,0x04,"popular culture/traditional arts" },
    { 0x7,0x05,"literature" },
    { 0x7,0x06,"film/cinema" },
    { 0x7,0x07,"experimental film/video" },
    { 0x7,0x08,"broadcasting/press" },
    { 0x7,0x09,"new media" },
    { 0x7,0x0a,"arts/culture magazines" },
    { 0x7,0x0b,"fashion" },

    { 0x8,0x00,"Social/Political Issues/Economics" },
    { 0x8,0x01,"magazines/reports/documentary" },
    { 0x8,0x02,"economics/social advisory" },
    { 0x8,0x03,"remarkable people" },

    { 0x9,0x00,"Education/Science/Factual Topics" },
    { 0x9,0x01,"nature/animals/environment" },
    { 0x9,0x02,"technology/natural sciences" },
    { 0x9,0x03,"medicine/physiology/psychology" },
    { 0x9,0x04,"foreign countries/expeditions" },
    { 0x9,0x05,"social/spiritual sciences" },
    { 0x9,0x06,"further education" },
    { 0x9,0x07,"languages" },

    { 0xa,0x00,"Leisure Hobbies" },
    { 0xa,0x01,"tourism/travel" },
    { 0xa,0x02,"handicraft" },
    { 0xa,0x03,"motoring" },
    { 0xa,0x04,"fitness & health" },
    { 0xa,0x05,"cooking" },
    { 0xa,0x06,"advertisement/shopping" },
    { 0xa,0x07,"gardening" },

    { 0xb,0x00,"Original Language" },
    { 0xb,0x01,"black and white" },
    { 0xb,0x02,"unpublished" },
    { 0xb,0x03,"live broadcast" },

	{ 0xff, 0xff, "" }
};


char* GetGenreName( byte genre, byte subgenre )
{
	int i;
	if (subgenre>0)
		FW_Print("%d %d\n", genre,subgenre);
    for ( i=0; genres[i].genre != 0xff; ++i )
	{
        if ( genres[i].genre == genre && genres[i].subgenre == subgenre )
            return genres[i].description;
    }
    return genres[0].description;
}


// Read description from the firmware's EPG event table
byte* GetEventDescription( TYPE_TapEvent* event )
{
	byte* result = NULL;

	if ( event && event->svcId )
	{
		FirmwareDetail* parameters = GetFirmwareDetail();
		if ( parameters )
		{
			Settings* settings = parameters->settings;
			type_eventtable* e = (type_eventtable*)parameters->eventTable;
			int eventTableLength = parameters->eventTableLength;
			int eventLength = parameters->eventLength;

			// Try each entry in the event table. Not very efficient, but the firmware does exactly the same
			int i;
			for ( i=0; i<eventTableLength; ++i, (byte*)e+=eventLength )
			{
				// On valid events, match evtId
				if ( (e->char00 & 0xc0) && (e->duration_hour | e->duration_min) && event->evtId == e->event_id )
				{
					// Match satIdx, orgNetId, tsId, svcId, evtId
					if ( event->svcId == e->service_id && event->tsId == e->transport_stream_id &&
						 event->orgNetId == e->original_network_id && event->satIdx == e->sat_index )
					{
						int outputLength = 0;
						int descriptionLength = 0;
						char* genre = 0;
						int genreLength = 0;
						bool addSpace = FALSE;
						word extendedLength = *((word*)(((byte*)e)+parameters->extendedLengthOffset));

						// Calculate how much memory will be needed for the text
						// Description is in the 250 byte event name block immediately after event name (no zero terminator)
						byte* description = e->event_name;
						if ( description )
						{
							description += e->event_name_length;
							descriptionLength = strlen(description);
							if ( descriptionLength > 127 )
							{
								outputLength += descriptionLength;
								addSpace = TRUE;
							}
						}

						// Add on the extended info length, plus space for a zero terminator
						if ( extendedLength > 0 )
						{
							// if additional space is enabled and there is some extended info to append
							if ( settings->insertSpace && addSpace )
								++outputLength;
							outputLength += extendedLength + 1;
							addSpace = FALSE;
						}

						// if the genre is enabled and the firmware supports it
						if ( settings->addGenre && parameters->genreOffset )
						{
							// Add on the genre length, plus space for brackets and a zero terminator
							genre = GetGenreName( *(((byte*)e)+parameters->genreOffset),
												  *(((byte*)e)+parameters->genreOffset+1));
							if ( genre )
							{
								genreLength = strlen(genre);
								if ( addSpace )
									++outputLength;
								outputLength += genreLength + 3;
							}
						}

						// If there's going to be something worth returning
						if ( outputLength > 0 )
						{
							bool addSpace = FALSE;

							// allocate memory, plus space for a zero terminator
							result = FW_MemAlloc( outputLength+1 );
							if ( result )
							{
								byte* p = result;
								// Append any long description text
								if ( descriptionLength > 127 )
								{
									memcpy( p, description, descriptionLength );
									p += descriptionLength;
									addSpace = TRUE;
								}

								// Append the existing extended information
								if ( extendedLength > 0 )
								{
									byte* extText = (((byte*)e)+parameters->extendedEventNameOffset);
									// if additional space is enabled and there is some extended info to append
									if ( settings->insertSpace && addSpace )
										*p++ = ' ';
									memcpy( p, (void*)*((dword*)extText), extendedLength );
									p += extendedLength;
									addSpace = FALSE;
								}

								// Append the genre
								if ( genre )
								{
									if ( addSpace )
										*p++ = ' ';

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


static dword TAP_GetCurrentEventSignature[] = {
	0x27bdffb8,                             //addiu   $sp, -0x48
	0xafb1002c,                             //sw      $s12c($sp)
	0xafb20030,                             //sw      $s230($sp)
	0xafb30034,                             //sw      $s334($sp)
	0xafb40038,                             //sw      $s438($sp)
	0xafb5003c,                             //sw      $s53c($sp)
	0xafb60040,                             //sw      $s640($sp)
	0xafbe0044,                             //sw      $fp44($sp)
	0x3c11FFFF,                             //lui     $s1, XXXX
	0xafbf0028,                             //sw      $ra48+var_20($sp)
	0x2631FFFF,                             //la      $s1, TapIdent
	0x8eFF0000,                             //lw      $XX, 0($s1)
	0x3cFFFFFF,                             //la      $XX, TapTable
	0xffFFFFFF,                             //
	0x00FFFF40,                             //sll     $XX, 5
	0xffFFFF21,                             //addu    $XX, $XX

	0xFFFF0000,                             //lw      $XX, 0($XX)
	0x0080ff25,                             //move    $s4, $a0
	0x00a0ff25,                             //move    $s5, $a1
	0x00000000,                             //nop
	0x00000000,                             //nop
	0x03801025,                             //move    $v0, $gp
	0x00000000,                             //nop
	0x00000000,                             //nop
	0x02c0e025,                             //move    $gp, $s6
	0x3c16FFFF,                             //li      $s6, XXXXXXXX
	0x26d6FFFF,                             //
	0x02c02025,                             //move    $a0, $s6
	0x00002825,                             //move    $a1, $0
	0x0cFFFFFF,                             //jal     XXXXXX                    ; setmem
	0x2406FFFF,                             //li      $a2, XXXX                 ; sizeof(type_tapevent)118
	0x3c1eFFFF,                             //la      $fp, XXXXXXXX             ; event_table

	0x27deFFFF,                             //
	0x2413FFFF,                             //li      $s3, XXXX                 ; 1388 = 5000 = MAX_EVENTS
	0x8fc50000,                             //lw      $a100($fp)
	0x240f0004,                             //li      $t7, 4

	0x0005c0FF,                             //sll     $t8, $a1, 3               ;2 from 1288
	0x0018FFFF,                             //srl     $t8, 27                   ;29 from 1288
	0x570fFFFF,                             //bnel    $t8, $t7, XXXX
	0x27de00FF,                             //addiu   $fp40                 ;44 from 1288
};


// Look event table details up from the TAP_GetCurrentEvent code
bool AnalyseFirmware()
{
	// Verify that we can reliably pick parameters from TAP_GetCurrentEvent
	if ( FindFirmwareFunction( TAP_GetCurrentEventSignature, sizeof(TAP_GetCurrentEventSignature),
		(dword)TAP_GetCurrentEvent, (dword)TAP_GetCurrentEvent + 0x100 ) != (dword)TAP_GetCurrentEvent )
		return FALSE;

	firmwareDetail.eventTable = GetFirmwareVariable( (dword)TAP_GetCurrentEvent, 0x7e, 0x82 );
	firmwareDetail.eventTableLength = GetFirmwareVariable( (dword)TAP_GetCurrentEvent, 0x86, 0x86 );
	firmwareDetail.eventLength = GetFirmwareVariable( (dword)TAP_GetCurrentEvent, 0x9e, 0x9e );

	// Firmware 5.12.88 and onwards reorganised the event table
	// Genre and subgenre bytes were added for TF5800 firmwares
	if ( _appl_version >= 0x1288 )
	{
		firmwareDetail.extendedLengthOffset = 0x32;
		firmwareDetail.extendedEventNameOffset = 0x2c;
		firmwareDetail.genreOffset = 0x40;
	}
	else
	{
		firmwareDetail.extendedLengthOffset = 0x2e;
		firmwareDetail.extendedEventNameOffset = 0x30;
		firmwareDetail.genreOffset = 0;
	}

	// Do a sanity check on the searched firmware parameters
	if ( firmwareDetail.eventTable < 0x80280000 || firmwareDetail.eventTable >= 0x80400000 ||
		 firmwareDetail.eventTableLength < 4000 || firmwareDetail.eventTableLength >= 15000 ||
		 firmwareDetail.eventLength < 0x40 || firmwareDetail.eventLength >= 0x50 )
		return FALSE;

	return TRUE;
}


// Try and get some helpful information from users with unsupported firmware
void WriteErrorLog()
{
	TYPE_File* fp;
	char buffer[4096];
	int i;
	char* p;

	TAP_Hdd_ChangeDir("..");
	TAP_Hdd_ChangeDir("..");
	TAP_Hdd_ChangeDir("ProgramFiles");
	TAP_Hdd_Delete( LOGFILE );
	if ( !TAP_Hdd_Exist( LOGFILE ) )
		TAP_Hdd_Create( LOGFILE, ATTR_NORMAL );
	if ( fp = TAP_Hdd_Fopen( LOGFILE ) )
	{
		memset( buffer, ' ', sizeof(buffer) );
		p = buffer;
		sprintf( p, "%d, 0x%04x, 0x%x, 0x%x\n\n", *sysID, _appl_version, TAP_GetCurrentEvent, TAP_EPG_GetExtInfo );
		p += strlen(p);
		sprintf( p, "eventTable %X\neventTableLength %X\neventLength %X\n\n",
			firmwareDetail.eventTable, firmwareDetail.eventTableLength, firmwareDetail.eventLength );
		p += strlen(p);
		sprintf( p, "TAP_GetCurrentEvent\n" );
		p += strlen(p);
		for ( i = 0; i < 0x40; ++i )
		{
			sprintf( p, "%08X %08X \r\n", &((dword*)TAP_GetCurrentEvent)[i], ((dword*)TAP_GetCurrentEvent)[i] );
			p += strlen(p);
		}
		*p = ' ';
		TAP_Hdd_Fwrite( buffer, 1, (p-buffer+511) & ~0x1ff, fp );
		TAP_Hdd_Fclose( fp );
	}
}

bool DescriptionExtender_Init()
{
	if ( !AnalyseFirmware() )
	{
		char buffer[512];

		WriteErrorLog();
		sprintf( buffer,
			"%s is not compatible with your firmware\n"
			"For an update, please contact the author for an update\n"
			"quoting your model type, firmware date and the following:\n"
			"%d, %04X, %X and %X\n"
			"A log file has been created in ProgramFiles",
			__tap_program_name__, *sysID, _appl_version, TAP_GetCurrentEvent-0x80000000, TAP_EPG_GetExtInfo-0x80000000 );
		ShowMessage( buffer, 1000 );
		return FALSE;
	}
	else
	{
		// We need to know various settings without needing to use $gp
		// firmwareVersion is no longer needed, use it to pass the settings structure
		firmwareDetail.settings = &settings;
		((word*)GetFirmwareDetail)[1] = ((dword)&firmwareDetail >> 16) & 0xffff;
		((word*)GetFirmwareDetail)[3] = (dword)&firmwareDetail & 0xffff;

		// Replace TAP_EPG_GetExtInfo with a jump to our GetEventDescription function
		HackFirmware( (dword*)TAP_EPG_GetExtInfo, J(GetEventDescription) );
		HackFirmware( ((dword*)TAP_EPG_GetExtInfo)+1, NOP_CMD ); // MUST insert a nop after the jump

		// Set up TAP API wrappers that don't require the use of $gp to be called
		((dword*)FW_MemAlloc)[0] = J(TAP_MemAlloc);
		((dword*)FW_MemAlloc)[1] = NOP_CMD;
		// Useful for debugging
		((dword*)FW_sprintf)[0] = J(TAP_SPrint);
		((dword*)FW_sprintf)[1] = NOP_CMD;
		((dword*)FW_Print)[0] = J(TAP_Print);
		((dword*)FW_Print)[1] = NOP_CMD;
	}
}
