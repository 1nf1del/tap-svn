/*
   A File Selector Module by Topfield, Sigittarius-E and ibbi

   2007-01-22  first release
   2007-03-31  minor bug fixes, larger window, some flags
   2007-05-18  fix for old firmware
   2007-07-11  case-insensitive filter and sort; sorting bug fixed
   2007-07-26  window in dialog grafic; larger filter; integrated into FireBirdLib
   2007-09-04  handle files and directories having the same name
*/


#include <string.h>
#include "FBLib_fs.h"
#include "graphic/FileSelectorDir.gd"


#define MY_NAME "FileSelector"


char FileSelectorFilter[80];
dword FileSelectorFlags;
bool FileSelector_initialized = FALSE;
word FileSelectorRegion;
TYPE_Window FileSelectorWindow;
tDialogWindow DialogWindow;
tDialogProfile DialogProfile;
bool Window = FALSE;


char *FileSelectorFilterGet (char *filter, int *n)
{
  char *p;

  if (!filter || !*filter)
  {
    *n = 0;
    return NULL;
  }

  if (*filter == ';') filter++;

  if ((p = strstr(filter, ";")))
  {
    *n = p - filter;
    return filter;
  }
  else
  {
    *n = strlen(filter);
    return filter;
  }
}


void FileSelectorFill (void)
{
  dword MENU_W, MENU_H;
  bool newWindow;
  TYPE_File file;
  dword files;
  bool add = FALSE, sort;
  char *p = NULL;
  int i = 0, n, j;
  size_t l;
  dword time[MAX_SORT], t;
  char file_n[TS_FILE_NAME_SIZE + 1], file_j[TS_FILE_NAME_SIZE + 1];

  MENU_W = (FileSelectorFlags & FS_WIDE ? 380 : 280);
  MENU_H = 200;

  newWindow = !Window;

  if (!Window)
  {
    if (FileSelectorFlags & FS_DIALOGWINDOW)
    {
      DialogWindowInit(&DialogWindow, &DialogProfile, 0, 0, MENU_W, win_l(MENU_H), NULL, (TAP_GetSystemVar(SYSVAR_OsdLan) == LAN_German ? "Wählen Sie eine Datei  " : "Select File  "), NULL, MENU_W, 0, DialogWindowType_NoLinesSmallIcons);
      DialogProfileLoadMy(MY_NAME);

      if (DialogProfile.FontSize)
      {
        MENU_W = MENU_W * 59 / 50;
        DialogWindowReInit(0, 0, MENU_W, win_l(MENU_H), MENU_W, 0);
      }

      DialogWindowReInit((720 - DialogWindow.OSDWidth) >> 1, (576 - DialogWindow.OSDHeight) * 7 / 12, MENU_W, win_l(MENU_H), MENU_W, 0);
    }
    else FileSelectorRegion = TAP_Osd_Create((720 - win_w(MENU_W)) >> 1, (576 - win_h(MENU_H)) * 7 / 12, win_w(MENU_W), win_h(MENU_H), 0, 0);

    Window = TRUE;
  }

  if (!(FileSelectorFlags & FS_DIALOGWINDOW))
  {
    TAP_Win_SetDefaultColor(&FileSelectorWindow);
    TAP_Win_Create(&FileSelectorWindow, FileSelectorRegion, 0, 0, MENU_W, MENU_H, (byte) FALSE, (byte) TRUE);
    TAP_Win_SetTitle(&FileSelectorWindow, (TAP_GetSystemVar(SYSVAR_OsdLan) == LAN_German ? "Wählen Sie eine Datei    " : "Select File    "), 0, FNT_Size_1622);
  }

  files = TAP_Hdd_FindFirst(&file);

  if (files && (strcmp(file.name, "__ROOT__") != 0))
  {
    if (i < MAX_SORT) time[i++] = 0;

    if (FileSelectorFlags & FS_DIALOGWINDOW) DialogWindowItemAdd("\x01..", 0, NULL, 0, TRUE, TRUE, DialogItemIcon_User, &_FileSelectorDir_Gd);
    else TAP_Win_AddItem(&FileSelectorWindow, "\x01..");
  }

  while (files--)
  {
    if (file.attr == ATTR_FOLDER)
    {
      if (i < MAX_SORT) time[i++] = 0;

      *file_n = '\x01';
      strncpy(file_n + 1, file.name, TS_FILE_NAME_SIZE - 1);
      file_n[TS_FILE_NAME_SIZE] = '\0';

      if (FileSelectorFlags & FS_DIALOGWINDOW) DialogWindowItemAdd(file_n, 0, NULL, 0, TRUE, TRUE, DialogItemIcon_User, &_FileSelectorDir_Gd);
      else TAP_Win_AddItem(&FileSelectorWindow, file_n);
    }

    (void) TAP_Hdd_FindNext(&file);
  }

  files = TAP_Hdd_FindFirst(&file);

  while (files--)
  {
    if (file.attr != ATTR_FOLDER && file.attr != ATTR_THISFOLDER && file.attr != ATTR_PARENTFOLDER)
    {
      if ((FileSelectorFlags & FS_SHOW_ALL) ||
          ((file.attr == ATTR_NORMAL || file.attr == ATTR_TS || file.attr == ATTR_PROGRAM) && strcmp(file.name, "_EIT_DATA.DAT") != 0 && strcmp(file.name, "__temprec__.ss") != 0))
      {
        if (*FileSelectorFilter)
        {
          add = FALSE;
          p = FileSelectorFilter;

          while ((p = FileSelectorFilterGet(p, &n)))
          {
            if (n)
            {
              l = strlen(file.name);

              if (l >= (size_t) n && (FileSelectorFlags & FS_NOCASE_FILTER ? strncasecmp(file.name + l - n, p, n) == 0 : strncmp(file.name + l - n, p, n) == 0))
              {
                add = TRUE;
                break;
              }

              p += n;
            }
          }
        }
        else add = TRUE;

        if (add)
        {
          if (i < MAX_SORT)
          {
            if (!(time[i] = DATE(file.mjd, file.hour, file.min))) time[i] = 1;
            i++;
          }

          if (FileSelectorFlags & FS_STRIP_EXTENSION)
            if ((p = strrchr(file.name, '.'))) *p = '\0';

          if (FileSelectorFlags & FS_DIALOGWINDOW) DialogWindowItemAdd(file.name, 0, NULL, 0, TRUE, TRUE, DialogItemIcon_None, NULL);
          else TAP_Win_AddItem(&FileSelectorWindow, file.name);
        }
      }
    }

    (void) TAP_Hdd_FindNext(&file);
  }

  // sort
  if ((FileSelectorFlags & (FS_SORT_BY_NAME | FS_SORT_BY_DATE)) != 0)
  {
    for (n = 0; n < i - 1; n++)
      for (j = n + 1; j < i; j++)
      {
        // two directory entries
        if (time[n] == 0 && time[j] == 0)
        {
          strcpy(file_n, (FileSelectorFlags & FS_DIALOGWINDOW ? DialogWindow.DialogItems[n].Parameter : FileSelectorWindow.item[n]));
          MakeValidFileName(file_n, ControlChars | LFChars);

          strcpy(file_j, (FileSelectorFlags & FS_DIALOGWINDOW ? DialogWindow.DialogItems[j].Parameter : FileSelectorWindow.item[j]));
          MakeValidFileName(file_j, ControlChars | LFChars);

          sort = (FileSelectorFlags & FS_NOCASE_SORT ? strcasecmp(file_j, file_n) < 0 : strcmp(file_j, file_n) < 0);
        }
        // one directory, one file
        else if (time[n] == 0 && time[j] != 0) sort = FALSE;
        // two files
        else
        {
          if (FileSelectorFlags & FS_SORT_BY_DATE) sort = (time[j] < time[n]);
          else
          {
            strcpy(file_n, (FileSelectorFlags & FS_DIALOGWINDOW ? DialogWindow.DialogItems[n].Parameter : FileSelectorWindow.item[n]));
            MakeValidFileName(file_n, ControlChars | LFChars);

            strcpy(file_j, (FileSelectorFlags & FS_DIALOGWINDOW ? DialogWindow.DialogItems[j].Parameter : FileSelectorWindow.item[j]));
            MakeValidFileName(file_j, ControlChars | LFChars);

            sort = (FileSelectorFlags & FS_NOCASE_SORT ? strcasecmp(file_j, file_n) < 0 : strcmp(file_j, file_n) < 0);
          }
        }

        if (sort)
        {
          if (FileSelectorFlags & FS_DIALOGWINDOW) strcpy(DialogWindow.DialogItems[n].Value, DialogWindow.DialogItems[n].Parameter);
          else p = FileSelectorWindow.item[n];
          t = time[n];

          if (FileSelectorFlags & FS_DIALOGWINDOW) strcpy(DialogWindow.DialogItems[n].Parameter, DialogWindow.DialogItems[j].Parameter);
          else FileSelectorWindow.item[n] = FileSelectorWindow.item[j];
          time[n] = time[j];

          if (FileSelectorFlags & FS_DIALOGWINDOW)
          {
            strcpy(DialogWindow.DialogItems[j].Parameter, DialogWindow.DialogItems[n].Value);
            *DialogWindow.DialogItems[n].Value = '\0';
          }
          else FileSelectorWindow.item[j] = p;
          time[j] = t;
        }
      }

    if (!(FileSelectorFlags & FS_DIALOGWINDOW))
      for (n = min(((int) win_h(MENU_H) - 83) / 22, i) - 1; n >= 0; n--) TAP_Win_SetSelection(&FileSelectorWindow, n);
  }

  if (FileSelectorFlags & FS_DIALOGWINDOW)
  {
    if (newWindow) DialogWindowShow();
    else DialogWindowRefresh();
  }
}


bool FileSelectorInitialize (char *filter, dword flags)
{
  InitTAPAPIFix();
  TAP_ExitNormal();

  FileSelectorFlags = flags;
  memset(FileSelectorFilter, 0, sizeof(FileSelectorFilter));

  if (filter)
  {
    strncpy(FileSelectorFilter, filter, sizeof(FileSelectorFilter));
    FileSelectorFilter[sizeof(FileSelectorFilter) - 1] = '\0';
  }

  FileSelectorFill();
  FileSelector_initialized = TRUE;

  DialogProfileCheck(&DialogProfile, MY_NAME);

  return FileSelector_initialized;
}


bool FileSelectorHandle (dword *key, char *filename)
{
  word event = EVT_KEY;
  dword param2 = 0;
  char *f;

  if (!key || !filename) return TRUE;

  switch (*key)
  {
    case RKEY_Prev:
    case RKEY_Next:
      if (!(FileSelectorFlags & FS_DIALOGWINDOW)) return TRUE;
      // no break here
    case RKEY_VolUp:
    case RKEY_VolDown:
    case RKEY_ChUp:
    case RKEY_ChDown:
      if (FileSelectorFlags & FS_DIALOGWINDOW) DialogEvent(&event, key, &param2);
      else TAP_Win_Action(&FileSelectorWindow, *key);
      *key = 0;
      return TRUE;
      break;

    case RKEY_Ok:
      *key = 0;
      f = (FileSelectorFlags & FS_DIALOGWINDOW ? DialogWindow.DialogItems[DialogWindow.SelectedItem].Parameter : FileSelectorWindow.item[TAP_Win_GetSelection(&FileSelectorWindow)]);
      if (*f == '\x01')
      {
        if (TAP_Hdd_ChangeDir(f + 1))
        {
          if (FileSelectorFlags & FS_DIALOGWINDOW) DialogWindowItemDeleteAll();
          else TAP_Win_Delete(&FileSelectorWindow);
          FileSelectorFill();
        }
        return TRUE;
      }
      else
      {
        strcpy(filename, f);
        if (FileSelectorFlags & FS_DIALOGWINDOW) DialogWindowExit();
        else
        {
          TAP_Win_Delete(&FileSelectorWindow);
          TAP_Osd_Delete(FileSelectorRegion);
        }
        Window = FALSE;
        FileSelector_initialized = FALSE;
        TAP_EnterNormal();
        return FALSE;
      }
      break;

    case RKEY_Exit:
      *key = 0;
      *filename = '\0';
      if (FileSelectorFlags & FS_DIALOGWINDOW) DialogWindowExit();
      {
        TAP_Win_Delete(&FileSelectorWindow);
        TAP_Osd_Delete(FileSelectorRegion);
      }
      Window = FALSE;
      FileSelector_initialized = FALSE;
      TAP_EnterNormal();
      return FALSE;
      break;

    default:
      return TRUE;
  }
}


bool FileSelector (dword *key, char *filename, char *filter, dword flags)
{
  if (key == NULL && filename == NULL)
    return FileSelectorInitialize(filter, flags);

  else if (!FileSelector_initialized)
  {
    if (key) *key = 0;
    if (filename) *filename = '\0';
    return FALSE;
  }

  else return FileSelectorHandle(key, filename);
}
