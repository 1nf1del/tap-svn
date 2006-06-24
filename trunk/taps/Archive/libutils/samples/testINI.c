#include "Utils.h"
#include <string.h>
//#define NULL    ((void *)0)
#define MAX_LEN 1024

#include "tap.h"
//#include "file.h"
#include "ini.h"

#define printf TAP_Print

#define ID_FILE 0x80500101
TAP_ID( ID_FILE );
TAP_PROGRAM_NAME("INI Sample v1.2");
TAP_AUTHOR_NAME("deangelj");
TAP_DESCRIPTION("A tap to test my ini functions.");
TAP_ETCINFO(__DATE__);

void PrintMem()
{
	dword heapSize, freeHeapSize, availHeapSize;
	TAP_MemInfo(&heapSize, &freeHeapSize, &availHeapSize);
	TAP_Print("TotalHeap: %d, FreeHeap: %d, AvailHeap: %d\n", heapSize, freeHeapSize, availHeapSize); 
}



dword TAP_EventHandler( word event, dword param1, dword param2 )
{
	return param1;
}

int TAP_Main(void)
{
	char buffer[MAX_LEN];
	char iniBuff[MAX_LEN];
	int length, i;
	char last = '\0';
	char* nextKey;
	char* appname = "test";
	char* filename = "testINI.ini";


	printf("Starting TestINI test...\r\n\r\n");
	PrintMem();

	printf("initing utils package...\r\n");
	initUtils(_INFO);
	PrintMem();

	//this *should* delete the entire section
	//ret = WritePrivateProfileString(appname, (char*) NULL, buffer, filename);
	
	/*
	if (!TAP_Hdd_Exist(filename))
	{
		TAP_Hdd_Create(filename, ATTR_NORMAL);
	}

	if (WritePrivateProfileString(appname, "key1", "value1", filename) > 0)
	{
		//WritePrivateProfileString(appname, "key2", "value2", filename);
	}
	else
	{
		printf("Error from WritePrivateProfileString: %s\r\n", GetLastErrorString());
		return 0;
	}
	printf("after writeprivate...\r\n");
	PrintMem();
	*/

	buffer[0]='\0';

	printf("calling GetPrivateProileString, appname = %s\r\n", appname);
	if ((length = GetPrivateProfileString(appname, (char*) NULL, (char*) NULL, 
				buffer, sizeof(buffer), filename)) <= 0) {
		printf("Error from GetPrivateProfile: %s\r\n", GetLastErrorString());
		return 0;
	}

	printf("Getting keys...\r\n");
	nextKey = buffer;  // final dir is marked with double null
	while(strlen(nextKey)>0)
	{
		printf("Found keys = %s. Getting value\r\n", nextKey);

		if (GetPrivateProfileString(appname, nextKey, "", iniBuff, sizeof(iniBuff), filename) > 0)
		{
			printf("Found %s = %s\r\n", nextKey, iniBuff);
			//check for commas in next version
			//attach to key structure
		}
		nextKey += strlen(nextKey) + 1;
	}
	printf("Ending test\r\n");
	PrintMem();
 	return 0;
}	
