//#include <stdlib.h>
#include <string.h>
#include "tap.h"
#include "file.h"

#define ID_FILE 0x80500104
TAP_ID( ID_FILE );
TAP_PROGRAM_NAME("File Sample");
TAP_AUTHOR_NAME("deangelj");
TAP_DESCRIPTION("A tap to test my file functions.");
TAP_ETCINFO(__DATE__);

dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	return param1;
}

//#define printf(a) ShowMessageWin(a, "", 300);
#define printf TAP_Print

void ShowError()
{
	char err[2048];
	sprintf(err, "Code: %d, Msg: %s", GetLastErrorCode(), GetLastErrorString());
	printf(err);
}

void PrintMem()
{
	dword heapSize, freeHeapSize, availHeapSize;
	TAP_MemInfo(&heapSize, &freeHeapSize, &availHeapSize);
	TAP_Print("TotalHeap: %d, FreeHeap: %d, AvailHeap: %d\n", heapSize, freeHeapSize, availHeapSize); 
}



int TAP_Main(void)
{
	char* path = "fileTest.txt";
	char line[512];
	UFILE *fp;
	TYPE_File *tf;

	printf("Starting file tester...\r\n");
	PrintMem();

	initFile(_INFO);
	PrintMem();

	if (fp = fopen(path, "w"))
	{
		if (fputs("This is a test message1\n", fp) == EOF)
			ShowError();
	//TAP_Print("1. File is %d bytes\r\n", TAP_Hdd_Flen(fp->fd));
		if (fputs("This is a test message2\n", fp) == EOF)
			ShowError();
	//TAP_Print("2. File is %d bytes\r\n", TAP_Hdd_Flen(fp->fd));
		fclose(fp);
	}
	PrintMem();

	tf = TAP_Hdd_Fopen("fileTest.txt");
	TAP_Print("re-opened file is %d bytes\r\n", TAP_Hdd_Flen(tf));
	TAP_Hdd_Fclose(tf);

	printf("Ending file tester...\r\n");
	return 0;

}
