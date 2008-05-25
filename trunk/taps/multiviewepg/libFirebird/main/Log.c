#include  <stdio.h>
#include  <string.h>
#include  "../libFireBird.h"

#define DUMMYNAME       "FBDUMMYFILE"

void Log (char *FileName, char *ProgramName, bool Console, eTimeStampFormat TimeStampFormat, char *Text)
{
  TYPE_File             *File;
  dword                 LogSize;
  char                  *TS;
  char                  CRLF [] = {'\r', '\n'};
  byte                  Sec;

  TS = TimeFormat(Now (&Sec), Sec, TimeStampFormat);
  if (TS [0]) strcat (TS, " ");

  if (FileName && FileName [0])
  {
    InitTAPAPIFix();

    TAP_Hdd_Create (FileName, ATTR_NORMAL);
    if ((File = TAP_Hdd_Fopen (FileName)) != NULL)
    {
      LogSize = TAP_Hdd_Flen (File);
      TAP_Hdd_Fseek (File, 0, SEEK_END);
      TAP_Hdd_Fwrite (TS, strlen (TS), 1, File);
      if (Text && Text [0]) TAP_Hdd_Fwrite (Text, strlen (Text), 1, File);
      TAP_Hdd_Fwrite (CRLF, 2, 1, File);
      HDD_TruncateFile (File, LogSize + strlen (TS) + strlen (Text) + 2);
      HDD_TouchFile (File);
      TAP_Hdd_Fclose (File);

      //The following Create/Delete ensures that the FAT is flushed onto the HDD
      TAP_Hdd_Create (DUMMYNAME, ATTR_NORMAL);
      TAP_Hdd_Delete (DUMMYNAME);
    }
  }

  if (Console)
  {
    if (TimeStampFormat != TIMESTAMP_NONE) TAP_Print (TS);
    if (ProgramName && ProgramName [0]) TAP_Print ("%s: ", ProgramName);
    if (Text && Text [0]) TAP_Print ("%s", Text);
    TAP_Print ("\n");
  }
}
