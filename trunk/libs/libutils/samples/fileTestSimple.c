#include <stdlib.h>
#include <string.h>
#include "tap.h"

#define ID_LIST 0x80500102

TAP_ID( ID_LIST );
TAP_PROGRAM_NAME("Simple File i/o test");
TAP_AUTHOR_NAME("deangelj");
TAP_DESCRIPTION("Simple file i/o test.");
TAP_ETCINFO(__DATE__);

/* required event handler in every TAP */

dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	return param1;
}

/*	Test out some of the TAP file functions */

/*	TAP_Main is required */

int TAP_Main(void)
{
	dword ret, size;
	bool bExists;
	TYPE_File* f;
	char buff[520];

	TAP_Print("starting\r\n");

	TAP_Print("checking for file...\r\n");
	bExists = TAP_Hdd_Exist("test.txt");
	TAP_Print("TAP_Hdd_Exist returned value %s...\r\n", (bExists ? "True" : "False"));

	if (bExists)
	{
		word wRet;
		TAP_Print("deleting file...\r\n");
		wRet = TAP_Hdd_Delete("test.txt");
		TAP_Print("TAP_Hdd_Delete return value %d...\r\n", wRet);
	} else {
		TAP_Print("file doesn't exist...\r\n");
	}

	TAP_Print("creating file...\r\n");
	//ret = TAP_Hdd_Create("test.txt", ATTR_PROGRAM);
	ret = TAP_Hdd_Create("test.txt", ATTR_NORMAL);
	TAP_Print("TAP_Hdd_Create returned value %d...\r\n", ret);

	TAP_Print("opening file...\r\n");
	f = TAP_Hdd_Fopen("test.txt");
	TAP_Print("TAP_Hdd_Fopen returned value %s...\r\n", (f ? "OK" : "Error"));

	if (f)
	{
		//memset(buff, ' ', sizeof(buff));
		//strcpy(buff, "this is a test message1");
		//size = strlen(buff);

		/* Fwrite writes a multiple of 512 bytes */
		//ret = TAP_Hdd_Fwrite( buff, size, 1, f );

		//TAP_Print("TAP_Hdd_Fwrite returned value %d...\r\n", ret);

		//TAP_Print("File is %d bytes in size...\r\n", TAP_Hdd_Flen(f));

		//ret = TAP_Hdd_Ftell(f);
		//TAP_Print("Next file write pos is %d...\r\n", ret);

		//strcpy(buff, "this is a test message2");
		//size = strlen(buff);
		//ret = TAP_Hdd_Fwrite( buff, size, 1, f );
		//TAP_Print("TAP_Hdd_Fwrite returned value %d...\r\n", ret);

		//TAP_Print("File is %d bytes in size...\r\n", TAP_Hdd_Flen(f));

		//ret = TAP_Hdd_Ftell(f);
		TAP_Print("1..\r\n");
		ret = TAP_Hdd_Fwrite( "This is line one\n", 1, 17, f );
		TAP_Print("2..\r\n");
		ret = TAP_Hdd_Fwrite( "This is line two\n", 1, 17, f );
		TAP_Print("file: size: %d, currentPos: %d, unusedByte: %d..\r\n", f->size, f->currentPos, f->unusedByte);
		f->size = 34;
		TAP_Print("4..\r\n");
		TAP_Hdd_Fclose(f);
		TAP_Print("5..\r\n");
	}

	f = TAP_Hdd_Fopen("test.txt");
		ret = TAP_Hdd_Fwrite( "This is line two\n", 1, 17, f );
		TAP_Print("file: size: %d, currentPos: %d, unusedByte: %d..\r\n", f->size, f->currentPos, f->unusedByte);
		f->size = 34;
		TAP_Hdd_Fclose(f);
	TAP_Print("exiting\r\n");

	return 0;	/* 0 return value for app mode */
}
