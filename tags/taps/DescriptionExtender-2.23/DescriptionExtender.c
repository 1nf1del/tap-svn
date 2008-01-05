/*
	Copyright (C) 2005-2008 Simon Capewell

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
#include <libFireBird.h>
#include <firmware.h>
#include <model.h>
#include <messagewin.h>
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
	byte		eventNameOffset;
	byte		extendedLengthOffset;
	byte		extendedEventNameOffset;
	byte		genreOffset;
	byte		subgenreOffset;
	byte		crid32LengthOffset;
	byte		crid32Offset;
	byte		crid31LengthOffset;
	byte		crid31Offset;
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


// Map of genre to description
typedef struct
{
    byte genre;
    byte description[0x40];
} GenreTable;

GenreTable genres[] = {
    { 0x00,"Unclassified" },

    { 0x10,"Movie/Drama" },
    { 0x11,"detective/thriller" },
    { 0x12,"adventure/western/war" },
    { 0x13,"science fiction/fantasy/horror" },
    { 0x14,"comedy" },
    { 0x15,"soap/melodrama/folkloric" },
    { 0x16,"romance" },
    { 0x17,"serious/classical/religious/historical movie/drama" },
    { 0x18,"adult movie/drama" },

    { 0x20,"News/Current Affairs" },
    { 0x21,"news/weather report" },
    { 0x22,"news magazine" },
    { 0x23,"documentary" },
    { 0x24,"discussion/interview/debate" },

    { 0x30,"Show/Game Show" },
    { 0x31,"game show/quiz/contest" },
    { 0x32,"variety show" },
    { 0x33,"talk show" },

    { 0x40,"Sports" },
    { 0x41,"special sports events" },
    { 0x42,"sports magazines" },
    { 0x43,"football/soccer" },
    { 0x44,"tennis/squash" },
    { 0x45,"team sports" },
    { 0x46,"athletics" },
    { 0x47,"motor sport" },
    { 0x48,"water sport" },
    { 0x49,"winter sports" },
    { 0x4a,"equestrian" },
    { 0x4b,"martial sports" },

    { 0x50,"Children's/Youth Programmes" },
    { 0x51,"pre-school children's programmes" },
    { 0x52,"entertainment programmes for 6 to 14" },
    { 0x53,"entertainment programmes for 10 to 16" },
    { 0x54,"informational/educational/school programmes" },
    { 0x55,"cartoons/puppets" },

    { 0x60,"Music/Dance" },
    { 0x61,"rock/pop" },
    { 0x62,"serious music/classical music" },
    { 0x63,"folk/traditional music" },
    { 0x64,"jazz" },
    { 0x65,"musical/opera" },
    { 0x66,"ballet" },

    { 0x70,"Arts/Culture (without music)" },
    { 0x71,"performing arts" },
    { 0x72,"fine arts" },
    { 0x73,"religion" },
    { 0x74,"popular culture/traditional arts" },
    { 0x75,"literature" },
    { 0x76,"film/cinema" },
    { 0x77,"experimental film/video" },
    { 0x78,"broadcasting/press" },
    { 0x79,"new media" },
    { 0x7a,"arts/culture magazines" },
    { 0x7b,"fashion" },

    { 0x80,"Social/Political Issues/Economics" },
    { 0x81,"magazines/reports/documentary" },
    { 0x82,"economics/social advisory" },
    { 0x83,"remarkable people" },

    { 0x90,"Education/Science/Factual Topics" },
    { 0x91,"nature/animals/environment" },
    { 0x92,"technology/natural sciences" },
    { 0x93,"medicine/physiology/psychology" },
    { 0x94,"foreign countries/expeditions" },
    { 0x95,"social/spiritual sciences" },
    { 0x96,"further education" },
    { 0x97,"languages" },

    { 0xa0,"Leisure Hobbies" },
    { 0xa1,"tourism/travel" },
    { 0xa2,"handicraft" },
    { 0xa3,"motoring" },
    { 0xa4,"fitness & health" },
    { 0xa5,"cooking" },
    { 0xa6,"advertisement/shopping" },
    { 0xa7,"gardening" },

    { 0xb0,"Original Language" },
    { 0xb1,"black and white" },
    { 0xb2,"unpublished" },
    { 0xb3,"live broadcast" },

	{ 0xff, "" }
};


char* GetGenreName( byte genre )
{
	int i;

    for ( i=0; genres[i].genre != 0xff; ++i )
	{
        if ( genres[i].genre == genre )
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
						byte crid31Length = 0;
						byte crid32Length = 0;

						// Calculate how much memory will be needed for the text
						// Description is in the 250 byte event name block immediately after event name (no zero terminator)
						byte* description = (byte*)*(dword*)(((byte*)e)+parameters->eventNameOffset);
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
							if ( parameters->subgenreOffset )
								genre = GetGenreName( (*(((byte*)e)+parameters->genreOffset) << 4) |
													  (*(((byte*)e)+parameters->subgenreOffset) & 15) );
							else
								genre = GetGenreName( *(((byte*)e)+parameters->genreOffset) );
							if ( genre )
							{
								genreLength = strlen(genre);
								if ( addSpace )
									++outputLength;
								outputLength += genreLength + 3;
							}
						}

						if ( parameters->crid31Offset && parameters->crid31LengthOffset )
						{
							crid31Length = *((byte*)(((byte*)e)+parameters->crid31LengthOffset));
							outputLength += crid31Length + 9;
						}

						if ( parameters->crid32Offset && parameters->crid32LengthOffset )
						{
							crid32Length = *((byte*)(((byte*)e)+parameters->crid32LengthOffset));
							outputLength += crid32Length + 9;
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
									byte* extText = (byte*)*(dword*)(((byte*)e)+parameters->extendedEventNameOffset);
									if ( extText )
									{
										// if additional space is enabled and there is some extended info to append
										if ( settings->insertSpace && addSpace )
											*p++ = ' ';
										// Skip any encoding characters
										if ( *extText < 0x20 )
										{
											if ( *extText == 0x10 )
											{
												extText += 2;
												extendedLength -= 2;
											}
											++extText;
											--extendedLength;
										}
										memcpy( p, (void*)extText, extendedLength );
										p += extendedLength;
										addSpace = FALSE;
									}
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

								if ( crid31Length > 0 )
								{
									strcpy( p, "crid=31\0" ); p+=8;
									memcpy( p, (byte*)*(dword*)(((byte*)e)+parameters->crid31Offset), crid31Length );
									p += crid31Length;
									*p++ = '\0';
								}
								
								if ( crid32Length > 0 )
								{
									strcpy( p, "crid=32\0" ); p+=8;
									memcpy( p, (byte*)*(dword*)(((byte*)e)+parameters->crid32Offset), crid32Length );
									p += crid32Length;
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

	firmwareDetail.genreOffset = 0;
	firmwareDetail.subgenreOffset = 0;
	firmwareDetail.crid31Offset = 0;
	firmwareDetail.crid32Offset = 0;

	// Firmware 5.13.72 and onwards reorganised the event table
	// Genre and subgenre bytes were added for TF5800 firmwares
	if ( _appl_version >= 0x1402 && (firmwareDetail.eventLength == 0x64 || firmwareDetail.eventLength == 0x68) )
	{
		// Freeview Playback firmwares for the 5800 have lots of extra information
		firmwareDetail.eventNameOffset = 0x28;
		firmwareDetail.extendedLengthOffset = 0x32;
		firmwareDetail.extendedEventNameOffset = 0x2c;
		firmwareDetail.genreOffset = 0x44;
		firmwareDetail.subgenreOffset = 0x45;
		firmwareDetail.crid32LengthOffset = 0x46;
		firmwareDetail.crid32Offset = 0x48;
		firmwareDetail.crid31LengthOffset = 0x4c;
		firmwareDetail.crid31Offset = 0x50;
	}
	else if ( _appl_version >= 0x1373 && firmwareDetail.eventLength == 0x44 )
	{
		firmwareDetail.eventNameOffset = 0x2c;
		firmwareDetail.extendedLengthOffset = 0x36;
		firmwareDetail.extendedEventNameOffset = 0x30;
	}
	else if ( _appl_version >= 0x1347 && firmwareDetail.eventLength == 0x48 )
	{
		firmwareDetail.eventNameOffset = 0x2c;
		firmwareDetail.extendedLengthOffset = 0x36;
		firmwareDetail.extendedEventNameOffset = 0x30;
		firmwareDetail.genreOffset = 0x44;
		// subgenre is part of the genre byte
	}
	// Firmware 5.12.88 and onwards reorganised the event table
	// Genre and subgenre bytes were added for TF5800 firmwares
	else if ( _appl_version >= 0x1288 )
	{
		firmwareDetail.eventNameOffset = 0x28;
		firmwareDetail.extendedLengthOffset = 0x32;
		firmwareDetail.extendedEventNameOffset = 0x2c;
		firmwareDetail.genreOffset = 0x40;
		firmwareDetail.subgenreOffset = 0x41;
	}
	else
	{
		firmwareDetail.eventNameOffset = 0x28;
		firmwareDetail.extendedLengthOffset = 0x2e;
		firmwareDetail.extendedEventNameOffset = 0x30;
	}

	// Do a sanity check on the searched firmware parameters
	if ( firmwareDetail.eventTable < 0x80270000 || firmwareDetail.eventTable >= 0x80400000 ||
		 firmwareDetail.eventTableLength < 4000 || firmwareDetail.eventTableLength > 16000 ||
		 firmwareDetail.eventLength < 0x40 || firmwareDetail.eventLength > 0x68 )
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
		sprintf( p, "%s (%s)\n", __tap_program_name__, __tap_etc_str__ );
		p += strlen(p);
		sprintf( p, "%d, 0x%04x, 0x%x, 0x%x\n\n", *sysID, _appl_version, TAP_GetCurrentEvent, TAP_EPG_GetExtInfo );
		p += strlen(p);
		sprintf( p, "eventTable %X\neventTableLength %d (%X)\neventLength %X\n\n",
			firmwareDetail.eventTable, firmwareDetail.eventTableLength, firmwareDetail.eventTableLength, firmwareDetail.eventLength );
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
	if ( PatchIsInstalled((dword*)0x80000000, "De") )
	{
		ShowMessage( "Description Extender is not required\n"
					 "when the DescExt patch is installed", 1000 );
		return FALSE;
	}

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
		HackFirmware( (dword)TAP_EPG_GetExtInfo, JMP_CMD | REL_ADDR(GetEventDescription) );
		HackFirmware( (dword)TAP_EPG_GetExtInfo+4, NOP_CMD ); // MUST insert a nop after the jump

		// Set up TAP API wrappers that don't require the use of $gp to be called
		((dword*)FW_MemAlloc)[0] = JMP_CMD | REL_ADDR(TAP_MemAlloc);
		((dword*)FW_MemAlloc)[1] = NOP_CMD;
		// Useful for debugging
		((dword*)FW_sprintf)[0] = JMP_CMD | REL_ADDR(TAP_SPrint);
		((dword*)FW_sprintf)[1] = NOP_CMD;
		((dword*)FW_Print)[0] = JMP_CMD | REL_ADDR(TAP_Print);
		((dword*)FW_Print)[1] = NOP_CMD;
	}
}
