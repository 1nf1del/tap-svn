/*
	Copyright (C) 2005 Sunstealer, tonymy01, Simon Capewell

	This TAP Dumps addressable memory to a file

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
#include "messagewin.h"

TAP_ID( 0x8000fe99 );
TAP_PROGRAM_NAME("Memory Dump");
TAP_AUTHOR_NAME("Sunstealer");
TAP_DESCRIPTION("Dumps Addressable Memory");
TAP_ETCINFO(__DATE__);


/*****************************************************************************/
/* Global variables */
/*****************************************************************************/

// Readable memory is 64MB. The MSB is used for bank selection. The maximum theoretical
// address space is 4GB. Bank selection appears to work for values from 6 to C ie.
// 0x6??????? (1.5GB) to 0xC??????? (3GB) although there may be other accessible regions. I think
// from reverse engineering the libtap.a file that the firmware is in the 0xF???????
// bank but I have made no effort to dump memory from that region yet. There appear to
// be unreadable areas of memory in some of the banks.
//
// Banks 0x6-8 and 0xA-C appear to address the SDRAM. Bank 0x9 is addressing something else and
// much of that bank is unreadable (between 0x90001000 to 0x9e000000) and oddly, cause a freeze
// rather than a crash and reset which is normally what happens.
// Banks 0x8 and 0xA are the addresses referenced by TAPS. If you save the address of a pointer
// to any of the TAP functions, they are in the 0x8??????? range and if you save the OSD base
// address pointer, that is in the 0xA??????? range. For this reason I think 0x8 and 0xA are the
// banks to use. I am dumping from 0xA for my experiments so far.
//
// The first 64MB can be dumped from both without any unreadable areas although very
// occasionally, you get a crash. Reading immediately after the first 64MB causes a crash but
// later addresses in the same bank can often be read ie. 0x80000000 to 0x83FFFFFF can be read
// but 0x84000000 cannot and then there is a gap before some more memory is readable.
// I tried 0x8e000000 to 0x8FFFFFFF (if I remember correctly) which worked ok and I think it
// contained a partial repeat of the SDRAM 64MB.
//
// banks 0x6, 0x7, 0xb and 0xc appear to contain the same SDRAM addresses (64MB) but there are
// differences in what is readable. For example, 0x60000000 to 0x61FFFa00 is readable then there
// is an unreadable gap then 0x62000000 to 0x63FFFc00 is readable but 0x63FFFc01 to 0x64000000 is not
//
// I haven't tested 0x7, 0xb and 0xc other than to see that they seem to be copies of the same
// 64MB address space in the first 16MB at least.
//
// There are some bugs in the TAP_Hdd_Fwrite function. It cannot write less than 512 bytes
// I think that means that it will always address 512bytes into memory which is awkward if you
// want to look at smaller segments that might be surrounded by non-readable regions.

#define BANK_START 0xA0000000
#define MEM_BLOCK  0x01000000 // largest blocksize for TAP_Hdd_Fwrite seems to be 32MB-1 byte, so we'll do 16MB chunks


void DumpMemory()
{
	TYPE_File* fp;
	int i;
	char filename[TS_FILE_NAME_SIZE];

	ShowMessage("Starting memory dump\nThis may take a moment and may cause a crash...", 200);
	TAP_Print("Memory dump starting...\n");

	sprintf( filename, "dump.dat" );
	if ( TAP_Hdd_Exist (filename) )
		TAP_Hdd_Delete (filename);

	TAP_Hdd_Create (filename, ATTR_NORMAL); // create new file

	if (fp = TAP_Hdd_Fopen (filename))
	{
		for ( i=0; i<4; ++i )
			TAP_Hdd_Fwrite ((void *) BANK_START + MEM_BLOCK*i, MEM_BLOCK, 1, fp);
		TAP_Hdd_Fclose (fp);
	}
	else
	{
		ShowMessage("Unable to open file", 200);
	}

	ShowMessage("Memory dump complete\nExiting now...", 200);
	TAP_Print("Memory dump completed to file %s.\n", filename);
}


dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	return param1;
}


int TAP_Main(void)
{
	DumpMemory();

	return 0;
}
