/*
   TAPCOM-Definitionen
*/

#ifndef FILESELECTORDEMOSERVER_H
#define FILESELECTORDEMOSERVER_H


#include "hdd.h"


// TAP-ID und -Name
#define FileSelectorDemoServer_ID       0x81bb1006
#define FileSelectorDemoServer_FileName "FileSelectorDemoServer.tap"

// Dienste
#define FileSelectorDemoServer_Service_GetFileName 1

// Daten-Struktur
typedef struct
{
  char filename[TS_FILE_NAME_SIZE];
}
FileSelectorDemoServer_ParamBlock;

// Version
#define FileSelectorDemoServer_ParamBlock_Version 1

// Rückgabewerte
#define FileSelectorDemoServer_Result_OK     0
#define FileSelectorDemoServer_Result_Cancel -1


#endif
