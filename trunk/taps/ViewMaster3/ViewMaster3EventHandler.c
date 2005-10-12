// Viewmaster3 Event Handler

dword TAP_EventHandler (word wEvent, dword dwParam1, dword dwParam2)
{
 dword freesize;
 char msg[512];
 unsigned char KeyChar;
 bool bFieldError, bClearTimerList;
 int index;
 int hhmm, endmin;
 int timertype, timerindex;
 bool bClearFileList;
 char str[30], str1[20], strunit[3], strunit1[3], daystr[4];
 char strtemp[TS_FILE_NAME_SIZE];
 int duration;
 dword seekpos;
 int i, j, k, minutes;
 int len, len2;
 dword TickCurrent;
 dword keyreturn;
 dword color;
 dword BytesCopied;
 int svcnum;
 int evtNum;
 int currentEvtNumber, recslot;
 byte rectuner;
 dword block;
 dword starttime, endtime;
 word year;
 byte month, day, weekDay;
 int xoff;
 word mjd1, mjd2;
 byte hh1, hh2, mm1, mm2, ss1, ss2;
 bool bDefaultRec;

// static char RecAutoRenameNew[TS_FILE_NAME_SIZE];

 TAP_GetState (&State, &SubState);

 if (TAP_Hdd_GetPlayInfo (&PlayInfo) && (PlayInfo.playMode == PLAYMODE_Playing))
    bPlaying = TRUE;
 else
    bPlaying = FALSE;
 for (i = 0; i <= 1; i++)
 {
    if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]) && ((RecInfo[i].recType == RECTYPE_Normal) || (RecInfo[i].recType == RECTYPE_Copy)))
        bRecording[i] = TRUE;
    else
        bRecording[i] = FALSE;
 }

 if (wEvent == EVT_IDLE)
 {

// if (wEvent == EVT_IDLE)

//  if (!bActivated) return dwParam1;
//  if (!bActivated) return 0;
 if (bActivated)
 {

/* 25apr05. with delete now following stopts in keyhandlercoommon, this should no longer be necessary
  if (bPreDeleteStop)
  {
   bPreDeleteStop = FALSE;
   DeleteFileNameAction ();
   return 0;

  }
*/
  if (bStartSubChannel)
  {
   if (bPIPshown)
   {
// change PIP to channel specified after started by system on previous channel
    if (!bDontStartSub)
//     TAP_Channel_Start(CHANNEL_Sub, 0, chNum);
        StartSubChannel (chNum);
    else
        bDontStartSub = FALSE;
    if ((bFileMode && !bFileModeHeightFull) || (bTimerMode && !bTimerModeHeightFull))
     ScalePOP ();
    else
    {
     ScaleMainFull ();
     ScalePIP ();
    }
   }
   else if (iPOPshown == 1)
   {
    ScalePOP ();
   }
   bStartSubChannel = FALSE;
  }

  if (bSat_key_hit)
  {
   // move sub to preferred position
   if (bPIPshown)
    ScalePIP ();
   else
    ScalePOP ();
   bSat_key_hit = FALSE;
  }

  if (bShowFileListPending)
  {
   bShowFileListPending = FALSE;
   ShowFileList (FALSE);
  }

  if (bShowTimerListPending)
  {
   bShowTimerListPending = FALSE;
   ShowTimerList (FALSE);
  }

/* with 06dec04 fw, now done in keyhandlercommon
//         for (i = 1; i <= 116; i++)
// generating 116 voldown keys in loop causes it to stop after processing 14 (duration stops at 1h 46m), try delays.
//   for (j = 1; j <= 12; j++)

  if (bUnlimitedRecordPending)
  {
//   while (UnlimitedRecordLoopCount > 0)
//   {
//    for (i = 1; i <= 10; i++)
// 23apr05. in 30mar05 firmware, following loop does not work without artificial delays after each 10 or so voldown keys
// (it stops at 1:46 in the record menu, appearing to lose remaining voldown keys).
// works fine in 06dec04 firmware.
    for (i = 1; i <= 116; i++)
     TAP_GenerateEvent (EVT_KEY, RKEY_VolDown, RKEY_VolDown);
//    TAP_Delay (20);
//    UnlimitedRecordLoopCount--;
//    UnlimitedRecordLoopCount = 0;
//    return 0;
//   }
//   TAP_Delay (20);
//   TAP_GenerateEvent (EVT_KEY, RKEY_Exit, RKEY_Exit);
// 23apr05. can't get exit key to work, so use record key (which I don't process anywhere) to cancel record menu.
   TAP_GenerateEvent (EVT_KEY, RKEY_Record, RKEY_Record);
   bUnlimitedRecordPending = FALSE; 
//   return 0;
  }
*/

// detect state change during playback, and re-instate pop window sizes if pop is on
// since when we start a playback or after the progress bar disappears , the main window goes back to full screen
  if (SubState == SUBSTATE_Normal && iPOPshown != 0)
  {
   if (PrevSubState == SUBSTATE_PvrList || PrevSubState == SUBSTATE_PvrPlayingSearch)
    ScalePOP ();
  }
/* 13jun05 now used by dynamic block rate calculation so uppdate at end of idle.
  PrevState = State;
  PrevSubState = SubState;
*/
  TickCurrent = TAP_GetTick ();

  if (bStopSpinDown)
  {
   if (TickCurrent - TickSpinDownSaved > 30000 || TickCurrent < TickSpinDownSaved) // every 5 minutes or count reset
   {
    TickSpinDownSaved = TickCurrent;
// 29jan05 doesn't seem to work well, so try some directory operations
//    freesize = TAP_Hdd_FreeSize (); // I think this will stop spin down
    CountRecFiles ();
   }
  }

// if we just reverted to normal substate from say skipping via progress bar, reset counters
// for dynamic block rate calculation
//  if (SubState == SUBSTATE_Normal && PrevSubState != SUBSTATE_Normal)
  if (SubState == SUBSTATE_Normal && PrevSubState == SUBSTATE_PvrPlayingSearch)
      TrickModePrev = TRICKMODE_Pause;    // make the code in the 1-second area lower down reset counters

// every second, check for possible auto rename of recording file.
// also check for what is recording in each slot, so we can keep track of changes to recordings and
// re-display file list when recordings start or stop (if file list is shown)
// also if clock is shown, update it.
  if ((TickCurrent - TickClockSaved > 100) || (TickCurrent < TickClockSaved)) // every second
  {
    TickClockSaved = TickCurrent;
    if (bClockShown)
     UpdateClock ();

// to avoid multiple refresh of file list when status of both recording slots changes in same second, set flag
// then refresh when come out of rec slot loop.
    bRefreshFileListAfterLoop = FALSE;
    for (i = 0;  i <= 1; i++)
    {
// check for deferred update of ts file header
     if (strcmp (RecFileNameUpdate[i], "") != 0)
     {
// check if we have delayed long enough after recording stopped in this slot (otherwise system may be updating header)
        if (TickCurrent - TickRecStopped[i] >= 500 || TickCurrent < TickRecStopped[i])
        {
/*
           sprintf (msg, "Attempting update of slot %d with ", i + 1);
           strcat (msg, RecEventNameFinal[i]);
           ShowFileErrorMsg (msg, FALSE);
*/
           ChangeToDataFilesFolder ();         
//           FileDisplayPointer = TAP_Hdd_Fopen (FileListNameRec[i]);
           FileDisplayPointer = TAP_Hdd_Fopen (RecFileNameUpdate[i]);
           if (FileDisplayPointer != 0)
           {
               if (TAP_Hdd_Flen (FileDisplayPointer) >= sizeof FileDisplayBuffer)
               {
                   TAP_Hdd_Fread (FileDisplayBuffer, sizeof FileDisplayBuffer, 1, FileDisplayPointer);
// lower 6 bits of byte 17 should have 010000 for internal tuner 0 and 100000 for internal tuner 1
// (actually in our standard system, it always has 110000).
                   FileDisplayBuffer[17] = (RecTuner[i] + 1) << 4 | (FileDisplayBuffer[17] & 0xCF);

// save seconds to append to system duration in minutes in unused byte before duration
// also update minutes
                   FileDisplayBuffer[9] = (RecDuration[i] / 60) & 0xFF;
                   FileDisplayBuffer[8] = ((RecDuration[i] / 60) & 0xFF00) >> 8;
// set top bit of seconds byte to flag seconds saved (in case zero)
                   FileDisplayBuffer[7] = (RecDuration[i] % 60) | 0x80;

// save accurate start time in ts header
// byte 66: hours (nominally reserved at end of TP_INFO)
// byte 67: minutes (nominally reserved at end of TP_INFO)
// (bytes 64 and 65, nominally reserved, are actually used in 10may05 at least)
// bytes 14/15: mjd (nominally reserved)
// byte 16 is supposed to be reserved but is actually non-zero (and variable) in 10may05 firmware at least
// so use byte 6 instead (nominally reserved)
//                   FileDisplayBuffer[14] = (RecStartMjdSaved[i] & 0xFF00) >> 8; 
//                   FileDisplayBuffer[15] = RecStartMjdSaved[i] & 0xFF; 
                   FileDisplayBuffer[TsHeaderByteStartHour] = RecStartHourFinal[i]; 
                   FileDisplayBuffer[TsHeaderByteStartMin] = RecStartMinFinal[i]; 
                   FileDisplayBuffer[TsHeaderByteStartSec] = RecStartSecFinal[i]; 

// 27may05 for 10may05 firmware, just try updating tuner only, since it may save event info further into file
// anyway.
// 30may05 system seems to do it correctly some times but not always, so try my way again.
                   if (bRecEventInfoSaved[i])
                   {
// put the saved event name (and its length) and event description into the buffer.
// if the saved description matches what is already there in the first n bytes (where n is the length of
// the saved description), don't update it because this is limited by the api to 128 bytes and
// the actual event description in header can be longer than this so don't want to truncate unless it is different
                       len = strlen (RecEventNameFinal[i]);
                       FileDisplayBuffer[85] = len;
                       strncpy (FileDisplayBuffer + 87, RecEventNameFinal[i], len);
// this strncmp doesn't seem to avoid overwriting (and truncating) an existing event description which
// matches, so try another way.
/*
                       if (strncmp (RecEventDescFinal[i], FileDisplayBuffer + 87, strlen (RecEventDescFinal[i])) != 0)
                          strcpy (FileDisplayBuffer + 87 + len, RecEventDescFinal[i]);
*/
                       len2 = strlen (RecEventDescFinal[i]);
                       for (j = 0; j < len2; j++)
                       {
                            if (RecEventDescFinal[i][j] == '\0')
                                break;
                            else if (RecEventDescFinal[i][j] != FileDisplayBuffer[87 + len + j])
                                strcpy (FileDisplayBuffer + 87 + len, RecEventDescFinal[i]);
                       }
                   }

                   TAP_Hdd_Fseek (FileDisplayPointer, 0, 0);    // re-position to start of file
                   TAP_Hdd_Fwrite (FileDisplayBuffer, sizeof FileDisplayBuffer, 1, FileDisplayPointer);
                   TAP_Hdd_Fclose (FileDisplayPointer);
// refresh data files list if up, so we get the extra ts header info just written
                   bFileListRefreshDuration = TRUE;
                     if (IsDataFilesFolderShown ()) 
                     {
//                        ScanCurrentFolder ();
                        ListCurrentFolder (TRUE);
                     }
               }    // end of file is big enough to contain header
               else
               {
                   TAP_Hdd_Fclose (FileDisplayPointer);
                   strcpy (msg, RecFileNameUpdate[i]);
                   strcat (" is too short to update header");
                   ShowFileErrorMsg (msg, TRUE);
               }
//               TAP_Hdd_Fclose (FileDisplayPointer);
           }    // end of successfully opened ts file
           else
           {
//                strcpy (msg, "Unable to update header for ");
                strcpy (msg, "Unable to open ");
                strcat (msg, RecFileNameUpdate[i]);
                strcat (msg, " for header update");
                ShowFileErrorMsg (msg, TRUE);
           }
           strcpy (RecFileNameUpdate[i], "");
        }   // end of time to attempt update of header
     }  // end of something in file name to update

//    if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]) && (RecInfo[i].recType == RECTYPE_Normal))
//     if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]) && ((RecInfo[i].recType == RECTYPE_Normal) || (RecInfo[i].recType == RECTYPE_Copy)))
     if (bRecording[i])
     {
// following is for rectype normal or copy
// save recorded seconds so we can get more accurate duration for saving in ts header when rec stops
       RecordedSec[i] = RecInfo[i].recordedSec;
// 15jul05 also save tuner in separate variable so we can use the latest version when the rec stops, to
// ensure it does not get overwritten by a new recording in the same slot.
       RecordTuner[i] = RecInfo[i].tuner;
// if the recording file in this slot is shown in the file list, call ShowFileListIndex to update its elapsed time
// in the field normally used for file size (since this is always zero if file is recording).
// ensure that we are in the DataFiles folder first, otherwise it can cause flicker when it tries to show
// the recording file list index but we are displaying another folder.
// recinfo doesn't know if I have changed name of file while still recording, so only update my copy of the name
// if the flag saying I have changed the name is not set.
// 18may05 since we don't rename until at least 5 seconds into recording, keep recinfo name until then
// even if flag indicates we have renamed, because if a recording starts in the same slot immediately after
// a previous one (which we renamed) finishes, we would think the new one has the same name as the previous one
// and get confused when showing them in the file list.
       if (RecInfo[i].recordedSec < 5)
         bFileListNameChangedRec[i] = FALSE; 
       if (!bFileListNameChangedRec[i])
        strcpy (FileListNameRec[i], RecInfo[i].fileName);
       if (!bFileListNameChangedRec[i] && strcmp (FileListNameRec[i], FileListNameRecPrev[i]) != 0)
       { 
// a new recording has been detected in slot i.
           bTryRenameLater[i] = FALSE; 
           bRefreshFileListAfterLoop = TRUE;
           bRecEventInfoSaved[i] = FALSE;
// 15jul05           RecTuner[i] = RecInfo[i].tuner;  // save tuner number for writing to file header when rec stops
// calculate accurate recording start time for showing in file list while still recording
           TAP_GetTime (&RecStartMjd[i], &RecStartHour[i], &RecStartMin[i], &RecStartSec[i]);
           TimeAdjust (&RecStartMjd[i], &RecStartHour[i], &RecStartMin[i], &RecStartSec[i], -RecordedSec[i]);
       }
       else if (IsDataFilesFolderShown ()) 
       {
           if ((FileListIndexRec[i] >= FileListIndexTop) && (FileListIndexRec[i] <= FileListIndexBottom))
            ShowFileListIndex (FileListIndexRec[i]);
       }

// check for auto rename of file in slot i
// automatic rename of timer recording file names to current epg event name at halfway point.
// every minute, check recordings in progress. if there is one whose file name is a single apostrophe,
// or starts with an apostrophe followed by a dash (system automatic rename for duplicate file names
// where it started as a single apostrophe), and the recinfo indicates that it is halfway through its
// scheduled duration, rename it.
         if (RecInfo[i].recType == RECTYPE_Normal)
         {
//            strcpy (FileRenameOld, RecInfo[i].fileName);
//            strcpy (FileRenameOld, FileListNameRec[i]);
            if (IsRec (FileListNameRec[i]))
            {
// allow for file name of "'.rec" or "'-xxx.rec" (the latter being an intial name of "'.rec" which has been renamed
// by the system because it already exists
              if ((strlen (FileListNameRec[i]) == 5 && FileListNameRec[i][0] == PrefixFileNameTimer) || (FileListNameRec[i][0] == PrefixFileNameTimer && FileListNameRec[i][1] == 45))
              {
//               if ((RecInfo[i].recordedSec >= (RecInfo[i].duration * 60) / 2) && (RecInfo[i].recordedSec < ((RecInfo[i].duration * 60) / 2) + 90))
//               if ((RecInfo[i].recordedSec >= 30) && (RecInfo[i].recordedSec < 120))
               if (RecInfo[i].recordedSec >= 5)
               {
//                   RecAutoRenameNew[0] = PrefixFileNameTimer;
//                   RecAutoRenameNew[1] = PrefixFileRenameEPGnext;
//                if (!bTryRenameLater[i] || ((RecInfo[i].recordedSec >= (RecInfo[i].duration * 60) / 2) && (RecInfo[i].recordedSec < ((RecInfo[i].duration * 60) / 2) + 90)))
// the 90-second window is too long now that we are checking every second, instead of every minute
                if (!bTryRenameLater[i] || ((RecInfo[i].recordedSec >= (RecInfo[i].duration * 60) / 2) && (RecInfo[i].recordedSec < ((RecInfo[i].duration * 60) / 2) + 5)))                      
                {
// if we don't find the event name, leave name unchanged, so it will be eligible to try again next time
// unless we have tried long enough, in which case rename it to the "not found" name returned by GetEpgEventName
                 RecAutoRenameNew[0] = PrefixFileNameTimer;
                 if (!bTryRenameLater[i])
                 {
                     bRenameEpgNext = TRUE;
                     RecAutoRenameNew[1] = PrefixFileRenameEPGnext;
                 }
                 else
                 {
                     bRenameEpgNext = FALSE;
                     RecAutoRenameNew[1] = PrefixFileRenameEPGnow;
                 }
//                 if (GetEpgEventName (RecInfo[i].svcType, RecInfo[i].svcNum, bRenameEpgNext, RecAutoRenameNew + 2) || RecInfo[i].recordedSec >= 60)
// if we are already in the try later, attempt rename even if we did not find event (rename to the error name returned).
//                 if (GetEpgEventName (RecInfo[i].svcType, RecInfo[i].svcNum, bRenameEpgNext, RecAutoRenameNew + 2) || bTryRenameLater[i])
                 if (GetEpgEventName (i, bRenameEpgNext, RecAutoRenameNew + 2) || bTryRenameLater[i])
                 {
                   strcpy (strtemp, RecAutoRenameNew);  // save original (truncated) event name without .rec, but with prefix chars
                   strcat (RecAutoRenameNew, ".rec");
// if the new name exists, add a suffix (before the .rec) until we find one that doesn't (or we have tried enough)
                     for (j = 1; j <= 9; j++)
                     {
                         if (j > 1)
                         {
// truncate original name if necessary, to allow 2 extra characters for the suffix, plus .rec
                             if (strlen (strtemp) > TS_FILE_NAME_SIZE - 6)
                                 strtemp[TS_FILE_NAME_SIZE - 6] = '\0';
                             sprintf (str, "-%d.rec", j);
                             strcpy (RecAutoRenameNew, strtemp);    // reinstate original event name without .rec
                             strcat (RecAutoRenameNew, str);
                         }
                         if (!TAP_Hdd_Exist (RecAutoRenameNew))
                         {
                             ChangeToDataFilesFolder ();
                             TAP_Hdd_Rename (FileListNameRec[i], RecAutoRenameNew);
                             if (TAP_Hdd_Exist (FileListNameRec[i]))
                             {
//                                  ShowFileErrorMsg ("Auto rename failed", FALSE);
                                  if (!bTryRenameLater[i])
                                   bTryRenameLater[i] = TRUE;
                             }
                             else
                             {
// rename worked, since old file name no longer exists
//                                    ShowFileErrorMsg (RecAutoRenameNew, FALSE);
                                  strcpy (FileListNameRec[i], RecAutoRenameNew);
                                  bFileListNameChangedRec[i] = TRUE;
// save final event name and description which will be used to update file header
// and set flag to say we have saved them, but only if we actually did get the event info, not just renaming
// to error name on later retry.
/*
                                  sprintf (msg, "Slot %d renamed receventname = ", i + 1);
                                  strcat (msg, RecEventName[i]);
                                  ShowFileErrorMsg (msg, FALSE);
*/
                                  if (strcmp (RecEventName[i], "") != 0)
                                  {
                                      strcpy (RecEventNameFinal[i], RecEventName[i]);
                                      strcpy (RecEventDescFinal[i], RecEventDesc[i]);
                                      RecEventStartTimeFinal[i] = RecEventStartTime[i];
                                      RecEventEndTimeFinal[i] = RecEventEndTime[i];
                                      bRecEventInfoSaved[i] = TRUE;
                                  }

                                  bRefreshFileListAfterLoop = TRUE;
                                  bTryRenameLater[i] = FALSE;
                             }  // end of rename worked
                             break;
                         }  // end of desired new name doesn't exist
// remove suffix from new name, so it starts afresh for next suffix append
                     }   // end of loop for adding name suffix
                 }   // end of got event name
                 else if (!bTryRenameLater[i])
                     bTryRenameLater[i] = TRUE;
                }   // end of appropriate time to try rename (now or next)
               }   // end at least 5 sec into recording
              }   // end file name is appropriate for auto rename
              else if (FileListNameRec[i][0] == PrefixFileNameTimer)
              {
// even if not a recording eligible for auto rename, if it is any timer recording, save current event name and description
// at midpoint of duration, for updating file header when recording stops.
// if GetEpgEventName fails to find the event info, don't attempt update
// only do this if we have not already renamed the timer rec and saved the event info
                if (!bRecEventInfoSaved[i])
                {
//                    if ((RecInfo[i].recordedSec >= (RecInfo[i].duration * 60) / 2) && (RecInfo[i].recordedSec < ((RecInfo[i].duration * 60) / 2) + 5))
// 05jun05 increase window to 10 minutes, to allow more time in case tap has not been running
                    if ((RecInfo[i].recordedSec >= (RecInfo[i].duration * 60) / 2) && (RecInfo[i].recordedSec < ((RecInfo[i].duration * 60) / 2) + 600))
                    {
                     if (GetEpgEventName (i, FALSE, strtemp))
                     {
                         strcpy (RecEventNameFinal[i], RecEventName[i]);
                         strcpy (RecEventDescFinal[i], RecEventDesc[i]);
                         RecEventStartTimeFinal[i] = RecEventStartTime[i];
                         RecEventEndTimeFinal[i] = RecEventEndTime[i];
                         bRecEventInfoSaved[i] = TRUE;
                     }
                    }
                }
              } // end timer recording but not eligible for auto rename
              else
              {
// for other (non-timer) recordings, save event info 10 minutes after start, if we have not already saved it
// e.g. due to a file list rename to the current or next epg event
                if (!bRecEventInfoSaved[i])
                {
// 05jun05 increase window from 5 seconds to 10 minutes, to allow more time in case tap has not been running
// also change first attempt from 10 minutes in to 5 minutes in.
                    if ((RecInfo[i].recordedSec >= 300) && (RecInfo[i].recordedSec < 900))
                    {
                     if (GetEpgEventName (i, FALSE, strtemp))
                     {
                         strcpy (RecEventNameFinal[i], RecEventName[i]);
                         strcpy (RecEventDescFinal[i], RecEventDesc[i]);
                         RecEventStartTimeFinal[i] = RecEventStartTime[i];
                         RecEventEndTimeFinal[i] = RecEventEndTime[i];
                         bRecEventInfoSaved[i] = TRUE;
                     }
                    }
                }
              } // end non-timer recording
            }   // end IsRec
         }  // end of rectype normal for auto rename
     }  // end of something recording in slot i
     else
     {
// nothing recording in slot i. if there was something there a second ago and file list is showing datafiles folder
// refresh it to show details of recording just finished.
       if (strcmp (FileListNameRecPrev[i], "") != 0)
       {
// recording in slot i has just stopped
// avoid double refresh of file list if we have already refreshed because we stopped it.
           if (!bRefreshFileListInIdle)
               bRefreshFileListInIdle = TRUE;
           else
               bRefreshFileListAfterLoop = TRUE;
// rewrite file header of rec just stopped, updating it with real tuner number and, if we saved event info,
// with the updated event name and description.
// now defer this till later, since file sometimes fails to open here
//           strcpy (RecFileNameUpdate[i], FileListNameRec[i]);
           strcpy (RecFileNameUpdate[i], FileListNameRecPrev[i]);
           TickRecStopped[i] = TickCurrent;
// save duration in seconds for writing to ts header
// (recordedsec[i] will get overwritten if another rec starts in slot i before we update header,
// so need to save in separate variable).
// like wise for accurate rec start time
           RecDuration[i] = RecordedSec[i] + 1;
// 15jul05 also save final tuner for writing to ts header
           RecTuner[i] = RecordTuner[i];
/*
           RecStartMjdSaved[i] = RecStartMjd[i];
           RecStartHourSaved[i] = RecStartHour[i];
           RecStartMinSaved[i] = RecStartMin[i];
           RecStartSecSaved[i] = RecStartSec[i];
*/
// re-calc accurate start time to the second from rec end time and duration for saving in ts header
// (we should have done this when the recording started, but we may not have been active then)
// also we need to keep them in separate variables in case another recording starts in the same slot
// before we have saved the start time for the previous one.
           TAP_GetTime (&RecStartMjdFinal[i], &RecStartHourFinal[i], &RecStartMinFinal[i], &RecStartSecFinal[i]);
           TimeAdjust (&RecStartMjdFinal[i], &RecStartHourFinal[i], &RecStartMinFinal[i], &RecStartSecFinal[i], -RecDuration[i]);
/*
           sprintf (msg, "Slot %d stopped, file to update = ", i + 1);
           strcat (msg, RecFileNameUpdate[i]);
           ShowFileErrorMsg (msg, FALSE);
*/
       } 
       bFileListNameChangedRec[i] = FALSE;
       strcpy (FileListNameRec[i], "");

     }  // end of nothing recording in slot i
     strcpy (FileListNameRecPrev[i], FileListNameRec[i]);

    }   // end of loop for each recording slot

    if (bRefreshFileListAfterLoop)
    {
        bFileListRefreshDuration = TRUE;
        if (IsDataFilesFolderShown ()) 
        {
           ScanCurrentFolder ();
           ListCurrentFolder (TRUE);
        }
    }

// check whether a playback of mine has stopped since last second, but not by me. if so, set flags to indicate this
// so we can
// resume playback via bookmark (e.g. if timer recording started, dropping us out of playback).
    if (bFilePlayingBack)
    {
//        if (!(TAP_Hdd_GetPlayInfo (&PlayInfo) && (PlayInfo.playMode == PLAYMODE_Playing)))
        if (!bPlaying)
        {
            if (strcmp (FileListNamePlayPrev, "") != 0)
            {
                bFilePlayingBack = FALSE;
                TickFilePlay += TickCurrent - TickFilePlayStart;
            }
        }
        else
        {
            strcpy (FileListNamePlayPrev, FileListNamePlay);

// dynamic block rate calculation during playback (for accurate layback positioning, even if duration does not
// reflect actual file size due to file corruption)
// detect whether we are entering or leaving normal playback.
// while in normal playback, keep track of time spent and blocks read.
// every minute of normal playback, update running average block rate since file playback started.
            if (PlayInfo.trickMode == TRICKMODE_Normal)
            {
                if (TrickModePrev != TRICKMODE_Normal)
                {
// we just entered normal play. the substate check should also allow for coming out of positioning
// via progress bar, because this does not change trickmode.
//                    TickStartNormalPlay = TickCurrent;
//                    TicksInNormalPlayCurrent = 0;
//                    BlocksInNormalPlayCurrent = 0;
//                    TickBlockRateSaved = TickCurrent;
                }
                else
                {
// we are continuing in normal play
// it looks like playinfo returns playmode normal when skipping via progress bar, so
// bypass accumulation code if in this state.
                    if (SubState != SUBSTATE_PvrPlayingSearch)
                    {
/*
                    TicksInNormalPlayCurrent += (TickCurrent - TickBlockRateSaved);
                    BlocksInNormalPlayCurrent += (PlayInfo.currentBlock - BlockCountSaved);
*/
                        TicksInNormalPlayCumulative += (TickCurrent - TickBlockRateSaved);
                        BlocksInNormalPlayCumulative += (PlayInfo.currentBlock - BlockCountSaved);

// only recalculate running average block rate if we have been in normal play for at least 1 minute
// now do this every minute, rather than every second after the first minute.
//                    if (TicksInNormalPlayCumulative >= 6000)
//                        if ((TicksInNormalPlayCumulative >= 6000) && (TickCurrent - TickBlockRateUpdated >= 6000))
                        if ((TicksInNormalPlayCumulative - TicksInNormalPlaySaved) >= 6000)
                        {
//                        BlockRateNormalPlay = BlocksInNormalPlayCumulative * 6000 / TicksInNormalPlayCumulative;
//                            BlockRateNormalPlay = BlocksInNormalPlayCumulative * 60000 / TicksInNormalPlayCumulative;
//                            BlockRateNormalPlay = ((BlocksInNormalPlayCumulative * 60000) + (TicksInNormalPlayCumulative / 2)) / TicksInNormalPlayCumulative;
//                            BlockRateNormalPlayDyn = ((BlocksInNormalPlayCumulative * 60000) + (TicksInNormalPlayCumulative / 2)) / TicksInNormalPlayCumulative;
// 22jun05 adding in half the denominator for rounding may cause overflow when things get too big
// so try without
                            BlockRateNormalPlayDyn = BlocksInNormalPlayCumulative * 60000 / TicksInNormalPlayCumulative;
                            if (bBlockRateDynamic)
                                BlockRateNormalPlay = BlockRateNormalPlayDyn;
                            TickBlockRateUpdated = TickCurrent;
                            TicksInNormalPlaySaved = TicksInNormalPlayCumulative;
                        }
                    }
                }
                TickBlockRateSaved = TickCurrent;
                BlockCountSaved = PlayInfo.currentBlock;
            }
            else if (TrickModePrev == TRICKMODE_Normal)
            {
// we just left normal play
// these should be rest when we next come back into normal play anyway
//                TicksInNormalPlayCurrent = 0;
//                BlocksInNormalPlayCurrent = 0;
            }
            TrickModePrev = PlayInfo.trickMode;
        }   // end of normal playinfo
    }   // end of file playing back

    if (bPlayCounterShown)
    {
// drop play counter after 5 seconds
        if (TickCurrent - TickPlayCounterSaved > 500)
            HidePlayCounter ();
        else
            ShowPlayCounter ();
    }

// check for timed stop of recording
    if (TimedStopRecMjd != 0)
    {
        TAP_GetTime (&mjd, &hour, &min, &sec);
        if ((mjd > TimedStopRecMjd) || ((mjd == TimedStopRecMjd) && (hour > TimedStopRecHour)) ||
           ((mjd == TimedStopRecMjd) && (hour == TimedStopRecHour) && (min >= TimedStopRecMin)))
        {
// time to stop any recordings
            TimedStopRecMjd = 0;
            TimedStopRecHour = 0;
            TimedStopRecMin = 0;
            for (i = 0; i <= 1; i++)
            {
//             if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]) && ((RecInfo[i].recType == RECTYPE_Normal) || (RecInfo[i].recType == RECTYPE_Copy)))
             if (bRecording[i])
             {
                if (!TAP_Hdd_StopRecord (i))
                {
                    sprintf (msg, "Timed stop of recording in slot %d", i);
                    strcat (msg, " failed");
                    ShowFileErrorMsg (msg, FALSE);
                }
             }
            }
        }
// if we are within 9 minutes of the recording stop time, generate 10 minute (the minimum) sleep time
// so that we will shut down asap after recording stops
        else if (!bTimedStopRecSleepGen)
        {
            TimerStartMinutes = hour * 60 + min;
            TimerEndMinutes = TimedStopRecHour * 60 + TimedStopRecMin;
            if (TimedStopRecMjd > mjd)
                TimerEndMinutes += 1440;
            if (TimerEndMinutes - TimerStartMinutes <= 9)
            {
// nb. assumes sleep timer currently disabled
                TAP_GenerateEvent (EVT_KEY, RKEY_Sleep, RKEY_Sleep);
                TAP_GenerateEvent (EVT_KEY, RKEY_Sleep, RKEY_Sleep);
                bTimedStopRecSleepGen = TRUE;
            }
        }
    }   // end of timed stop rec values set.

  }    // end of check every second

  if (bPlayScanMode)
  {

   if (TickCurrent - TickPlayScanSaved > 500) // because it takes about 1 sec to process 30-sec skip

   {
    TickPlayScanSaved = TickCurrent;
/*
    TAP_GenerateEvent (EVT_KEY, RKEY_Goto, RKEY_Goto);
// if in fast scan, skip extra 30 sec between plays
    if (bPlayScanFast)
     TAP_GenerateEvent (EVT_KEY, RKEY_Goto, RKEY_Goto);
*/
    if (bPlayScanFast)
        SkipPlayback (60);
    else
        SkipPlayback (30);
   }
  }
  else
  {

// if progress bar up, drop it more quickly than system does
//   if (IsProgressBarShown (State, SubState))
   if (IsProgressBarShown () && !bPlaybackPaused)
   {
// 24jan05. this may be causing problem of dropping out of progress bar when skipping via volume up/down keys,
// so try without it.
// 21apr05 try again with this re-instated because I really don't need vol up/down keys to skip (except those generated
// other keys).
/* 27apr05 it does still screw up, so take it out again

    if (TickCurrent - TickProgBarSaved > 400) // 4 seconds
    {
// 31aug04     TickProgBarSaved = TickCurrent;
// 11sep04 generated play key causes resumption of play from pause if pause pressed while progress bar up
// actually it seems that either play or exit (maybe anything) generated causes drop out of pause, so
// only do it if we believe we are not paused
      TAP_GenerateEvent (EVT_KEY, RKEY_Play, RKEY_Play);
    }
*/
   }
   else
   {
    TickProgBarSaved = TickCurrent;
/* 09aug05 now use specific key to initiate jumps instead of timeout
//    if (PlaybackNumericKeyCount > 0 && (TickCurrent - TickPlaybackNumericSaved > 100))
    if (PlaybackNumericKeyCount > 0 && (TickCurrent - TickPlaybackNumericSaved > 150))
    {
// if playback numeric input is in progress and it's more than 1.5 sec since we last entered a digit
// assume we have finished and initiate the percentage skip calculation by filling in the leading zeros
// and generating a numeric key event to pretend that we have entered all 3 digits
     if (PlaybackNumericKeyCount == 2)
     {
      PlaybackNumericKeyString[2] = PlaybackNumericKeyString[1];
      PlaybackNumericKeyString[1] = PlaybackNumericKeyString[0];
      PlaybackNumericKeyString[0] = 0x30;
     }
     else
     {
      PlaybackNumericKeyString[2] = PlaybackNumericKeyString[0];
      PlaybackNumericKeyString[1] = 0x30;
      PlaybackNumericKeyString[0] = 0x30;
     }
     PlaybackNumericKeyCount = 3;
     PercentKeyReturnCount = 0;
     TAP_GenerateEvent (EVT_KEY, RKEY_0, RKEY_0); // dummy event for key handler
     TickPlaybackNumericSaved = TickCurrent;
    }
*/
   } // end of progress bar not shown
  } // end of not play scan mode

 }  // end of viewmaster activated

 PrevState = State;
 PrevSubState = SubState;
 return 0;

 } // end of idle event

 else if (wEvent == EVT_KEY)
 {
// seem to be getting spurious key events with Dec 6, 2004 firmware, so ignore values outside
// the range of good keys. this does, in fact, fix the problems I was having.
  if (dwParam1 < RKEY_0 || dwParam1 > RKEY_Next)
   return;

  if (dwParam1 == RKEY_TvSat)
  {
// screen capture of either main channel or OSD (depending on option) as BMP file to
// ProgramFiles folder (so that ImageViewer can display it).
    ChangeToProgramFilesFolder ();
    if (bCaptureOSD)
        OSD2BMP ();
    else
        CaptureVideo (CHANNEL_Main, 720, 576);
    return 0;
  }
/* conflicts with fav key use for 30s forward skp
  else if (dwParam1 == RKEY_Fav)
  {
// screen capture of subchannel as BMP file to root directory
    ChangeToProgramFilesFolder ();
    CaptureVideo (CHANNEL_Sub, 720, 576);
    return 0;
  }
*/
  if (bOptsWinShown) // handling keys when options window is shown

//   #include "KeyHandlerOptions.c"

// ViewMaster3 key handler for options window
{
   if (dwParam1 == RKEY_Ok) // ok-key confirms the selected list-entry
   {
    int iSelection = TAP_Win_GetSelection (&winOpts);
    HideOptionsWindow ();
    switch (iSelection)
    {
    case 0: {
       bActivated = !bActivated;
       if (bActivated)
       {
//         ClockCounter = 0;
//         StopSpinDownCounter = 0;
        TickClockSaved = 0;
        TickSpinDownSaved = 0;
//        ChangeToRootFolder (FileListPathLevel);
        ChangeToRootFolder ();
        FileListPathLevel = 0;
        strcpy (FileListPath[0], "");
       }
       return 0;  // first toggles activation/deactivation 
      }
    case 1:  {
        bCaptureOSD = !bCaptureOSD;
        return 0;
      }
    case 2:  {
       bQuickChanActivated = !bQuickChanActivated;
       return 0;
      }
    case 3:  {
       bPlaybackNumericActivated = !bPlaybackNumericActivated;
       return 0;
      }
    case 4: {
       bStopSpinDown = !bStopSpinDown;
       if (bStopSpinDown)
//         StopSpinDownCounter = 0;
        TickSpinDownSaved = 0;
       return 0;
      }
    case 5: {
       bReduceFullScreen = !bReduceFullScreen;
       ScaleMainFull ();
       return 0;
      }
    case 6: {
       bFilePlayingBack = !bFilePlayingBack;
       return 0;
      }
    case 7: {
       ShowMessageWin("Terminating",_PROGRAM_NAME_);
       TAP_Channel_Stop(CHANNEL_Sub);
       TAP_Exit ();
       return 0;
      }
    case 8: return RKEY_Exit;      // fourth cascades exit-key to other TAP's
    }
   }
   else if (dwParam1 == RKEY_Exit) // exit-key hides the options window
   {
    HideOptionsWindow ();
    return 0; // exit without Cascading
   }
   else
   {
    TAP_Win_Action (&winOpts, dwParam1); // send all other key's to menu-Win
    return 0;
   }
} // end of KeyHandlerOptions

  else if (bFileSortOptsWinShown)
//   #include "KeyHandlerFileSortOptions.c"

// ViewMaster3 key handler for file list sort options window
{
// if (dwParam1 == RKEY_Ok) // ok-key confirms the selected list-entry
 if (dwParam1 >= RKEY_0 && dwParam1 <= RKEY_2)
 {
//    int iSelection = TAP_Win_GetSelection (&winOpts);
//  FileListSortKey = TAP_Win_GetSelection (&winOpts);
  FileListSortKey = dwParam1 - RKEY_0;
  HideFileSortOptionsWindow ();
//  ScanCurrentFolder ();
//  ListCurrentFolder (TRUE);
  bFileListRefreshDuration = TRUE;
  ListNewFolder (); // so we get correct file highlighted if playing back
 }
 else if (dwParam1 == RKEY_Exit) // exit-key hides the options window
 {
  HideFileSortOptionsWindow ();
//  bFileListRefreshDuration = TRUE;
//  ListCurrentFolder (TRUE);
 }
 else
 {
//  TAP_Win_Action (&winOpts, dwParam1); // send all other keys to menu-Win
    ;
 }
 return 0;
} // end of KeyHandlerFileSortOptions

  else if (bTimerSortOptsWinShown)
//   #include "KeyHandlerTimerSortOptions.c"

// ViewMaster3 key handler for timer list sort options window
{
// if (dwParam1 == RKEY_Ok) // ok-key confirms the selected list-entry
 if (dwParam1 >= RKEY_0 && dwParam1 <= RKEY_2)
 {
//  TimerListSortKey = TAP_Win_GetSelection (&winOpts);
  TimerListSortKey = dwParam1 - RKEY_0;
  HideTimerSortOptionsWindow ();
  SortTimerListArray ();
  TimerListIndexTop = 0;
//  TimerListIndexBottom = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TAP_Timer_GetTotalNum() - 1);
  TimerListIndex = 0;
  ListTimers (TRUE);
 }
 else if (dwParam1 == RKEY_Exit)
 {
  HideTimerSortOptionsWindow ();
//  ListTimers (TRUE);
 }
 else
//  TAP_Win_Action (&winOpts, dwParam1); // send all other keys to menu-Win
    ;
 return 0;
} // end of KeyHandlerTimerSortOptions

  else
  {

// if we are in the middle of entering special numeric playback skip keys, any non-numeric aborts the entry
// and if it is not the exit key, will be processed normally. exit key just aborts the numeric entry.
//   if (PlaybackNumericKeyCount > 0)
   if (bPlaybackNumericJumpMode)
   {
// allow jump mode activation keys to be processed further down
    if (dwParam1 == RKEY_Ok || dwParam1 == RKEY_VolUp || dwParam1 == RKEY_VolDown)
        ;
    else if (dwParam1 < RKEY_0 || dwParam1 > RKEY_9)
    {
     TerminatePlaybackNumericKey ();
     if (dwParam1 == RKEY_Exit)
      return 0;
    }
   }
// 04may05 showfileerrormsg no longer exits normal if not in file mode, so if there is a delete or stop recording
// confirmation pending and we abort it via the exit key, we would show the options window instead of just aborting
// the pending message, so add test for file error msg not shown as well as state normal
//   else if ((dwParam1 == RKEY_Exit) && (State == STATE_Normal) && (SubState == SUBSTATE_Normal))
// 15aug05 include block rates shown in play info in exceptions here, so the exit key can be processed
// later to cancel this.
//   else if ((dwParam1 == RKEY_Exit) && (State == STATE_Normal) && (SubState == SUBSTATE_Normal) && !bFileErrorMsgShown)
   else if (!(bPlaying & bClockShown & bBlockRatesShown))
   {
       if ((dwParam1 == RKEY_Exit) && (State == STATE_Normal) && (SubState == SUBSTATE_Normal) && !bFileErrorMsgShown)
       {
        ShowOptionsWindow ();
        return 0;
       }
   }

   if (bActivated)
   {
//    #include "KeyHandlerCommon.c"

// ViewMaster3 key handler common to more than one mode
{

    if (bPlayScanMode)
    {

     if (dwParam1 == RKEY_Play ||
      dwParam1 == RKEY_Stop)
     {
      bPlayScanMode = FALSE;
// following seems to avoid play key generated in idle (when dropping progress bar quicker) being re-processed by me
// as resume playback when stop scan mode with stop key
// 09aug05      TickProgBarSaved = TAP_GetTick ();
// 09aug05 don't return play key to system else it brings up progress bar
      if (dwParam1 == RKEY_Play)
        return 0;
     }
// 30mar05 allow clock toggle key to be processed below.
     else if (dwParam1 == KeyToggleClock) ;
//     else if (dwParam1 != RKEY_Check) // blue key now toggles between slow and fast scan
     else if (dwParam1 != RKEY_Sat) // sat key now toggles between slow and fast scan
      return dwParam1; // other keys in play scan mode tend to stuff up screen
    }

// if progress bar is up certain keys should restart timer for dropping the bar quicker

//    if (IsProgressBarShown (State, SubState))
    if (IsProgressBarShown ())
    {
     if (dwParam1 == RKEY_Play ||
      dwParam1 == RKEY_Rewind || 
      dwParam1 == RKEY_Forward || 
      dwParam1 == RKEY_Goto || 
      dwParam1 == RKEY_Bookmark)
//    dwParam1 == RKEY_Check)
       TickProgBarSaved = TAP_GetTick ();
    }
/* no need for this now I have widescreen tv
    if (dwParam1 == RKEY_AudioTrk)
    {
// 2-way toggle between 16:9 full and 4:3 centre cut
     if (TAP_GetSystemVar (SYSVAR_TvRatio) == SCREEN_RATIO_16_9)
     {
      TAP_SetSystemVar (SYSVAR_TvRatio, SCREEN_RATIO_4_3);
      TAP_SetSystemVar (SYSVAR_16_9_Display, DISPLAY_MODE_CenterExtract);
     }
     else
      TAP_SetSystemVar (SYSVAR_TvRatio, SCREEN_RATIO_16_9);
     return 0;
    }
*/

// key handler for timed stop recording entry mode
  if (bTimedStopRecEntryMode)
  {
     switch (dwParam1)
     {
      case RKEY_0: KeyChar = 0x30; break;
      case RKEY_1: KeyChar = 0x31; break;
      case RKEY_2: KeyChar = 0x32; break;
      case RKEY_3: KeyChar = 0x33; break;
      case RKEY_4: KeyChar = 0x34; break;
      case RKEY_5: KeyChar = 0x35; break;
      case RKEY_6: KeyChar = 0x36; break;
      case RKEY_7: KeyChar = 0x37; break;
      case RKEY_8: KeyChar = 0x38; break;
      case RKEY_9: KeyChar = 0x39; break;
      default:  goto TimedStopRecKeyNotNumeric;
     }
     if (TimerCharIndex < 4)
     {
      TimedStopRecEntry[TimerCharIndex] = KeyChar;
//      TimedStopRecEntry[TimerCharIndex+1] = '\0';
      ShowTimedStopRecEntry ();
      TimerCharIndex ++;
     }
     return 0;

TimedStopRecKeyNotNumeric:

    if (dwParam1 == RKEY_Recall)
    {
      if (TimerCharIndex > 0)
      {
// back up one character in time entry field
       TimerCharIndex --;
       TimedStopRecEntry[TimerCharIndex] = 0x5F;    // fill with underscore
       ShowTimedStopRecEntry ();
      }
      return 0;
    }

    else if (dwParam1 == RKEY_Ok)
    {
// validate time of day entered (hhmm)
  // convert 4 characters to integer equivalent
      hh1 = 0; mm1 = 0;

      for (i = 0; i < 4; i++)
      {
// 05jun05 field must contain all numeric digits. any underscores (left after clearing) invalidate field.
       if (TimedStopRecEntry[i] == 0x5F)
        goto TimedStopRecInvalid;
       if (i < 2) hh1  = (hh1  * 10) + (TimedStopRecEntry[i] - 0x30);
       else  mm1 = (mm1 * 10) + (TimedStopRecEntry[i] - 0x30);
      }
      if (hh1 <= 23 && mm1 <= 59)
      {
// save recording stop time
        TAP_GetTime (&mjd, &hour, &min, &sec);
        if ((hh1 < hour) || ((hh1 == hour) && (mm1 < min)))
            TimedStopRecMjd = mjd + 1;
        else
            TimedStopRecMjd = mjd;
        TimedStopRecHour = hh1;
        TimedStopRecMin = mm1;
        HideTimedStopRecEntry ();
//        ShowFileErrorMsg ("Recording set to stop at specified time, then sleep", FALSE);
        sprintf (msg, "Recording set to stop at %02d:%02d", TimedStopRecHour, TimedStopRecMin);
        strcat (msg, ", then sleep");
        ShowFileErrorMsg (msg, FALSE);
// if there is a current recording of exactly 2 hours duration (the default), issue warning in case it should have
// been extended to near the scheduled stop time.
        bDefaultRec = FALSE;
        for (i = 0; i <= 1; i++)
        {
            if (bRecording[i])
            {
                TAP_Hdd_GetRecInfo (i, &RecInfo[i]);
                if (RecInfo[i].duration == 120)
                    bDefaultRec = TRUE;
            }
        }
        if (bDefaultRec)
            ShowFileErrorMsg ("Warning! Default-duration recording current. It may need extension.", TRUE);
      }
      else
      {
// time is invalid
TimedStopRecInvalid:
        ShowFileErrorMsg ("Invalid time entered", FALSE);
        ShowTimedStopRecEntry ();
      }
      return 0;          
    }   // end of ok key

    else if (dwParam1 == RKEY_Exit)
    {
       TimedStopRecMjd = 0;
       TimedStopRecHour = 0;
       TimedStopRecMin = 0;
       ShowFileErrorMsg ("Recording stop time cleared", FALSE);
       HideTimedStopRecEntry ();
       return 0;
    }

    else
       return 0;
  } // end of timed stop rec entry mode

// 15aug05 if block rates ares shown in play info, exit key sets flag so they won't be
//  else if (bPlaying & bClockShown & bBlockRatesShown)
  else if (bPlaying & bClockShown & bBlockRatesShown & !bFileMode & !bTimerMode)
  {
      if (dwParam1 == RKEY_Exit)
      {
          bBlockRatesShown = FALSE;
          return 0;
      }
  }
// 08jun05 qcc can now work in timer (half height) mode, but only for subchannel (preceded by uhf key)
// since numerics are otherwise needed for timer entry
// numeric keys are used in teletext, so should not be intercepted here in this state (or in menu state)
     if (bQuickChanActivated && (State != STATE_Ttx) && (State != STATE_Menu) && !bNameEntryMode)
     {
      if (dwParam1 == RKEY_Uhf) // conflict with InfoDisplay but can't think of a better key yet
      {
       bChanSub = TRUE;
       return 0;
      }
      switch (dwParam1)
      {
       case RKEY_1: chNum = 0; break;
       case RKEY_2: chNum = 1; break;
       case RKEY_3: chNum = 2; break;
       case RKEY_7: chNum = 3; break;
       case RKEY_9: chNum = 4; break;
//       default: goto nochanstart;
       default: chNum = -1;
      }
      if (bChanSub)
      {
// qcc preceded by UHF key affects subchannel
       bChanSub = FALSE;

       if (chNum == -1)
       {
// invalid subchannel entered, if PIP/POP on, turn it off, and if in half-height file mode, move main to centre
// if not in PIP/POP, just ignore.
        if (bPIPshown || (iPOPshown != 0))
        {
         TAP_Channel_Stop (CHANNEL_Sub);
         bPIPshown = FALSE;
         iPOPshown = 0;
         if (bFileMode && !bFileModeHeightFull)
//          TAP_Channel_Scale (CHANNEL_Main, 168, 324, 384, 216, TRUE);
            ScaleMainHalfHeight (FileWinY + FileWinH);
         else if (bTimerMode && !bTimerModeHeightFull)
            ScaleMainHalfHeight (TimerWinY + TimerWinH);
        }
        return 0;
       }
       else if (bPIPshown || iPOPshown != 0)
//       if (bPIPshown || iPOPshown != 0)
       {
// valid subchannel number. if already in PIP/POP, start (change to) specified subchannel
//        TAP_Channel_Start(CHANNEL_Sub, 0, chNum);
        StartSubChannel (chNum);
        return 0;
       }
       else
       {
// valid subchannel, but not in PIP/POP already, so use system to start PIP
// this doesn't work if we are playing back with file list half height, neither does starting the subchannel explicitly
//        bPIPshown = TRUE;
/*
        if (bFilePlayingBack && bFileMode && !bFileModeHeightFull)
        {
         TAP_Channel_Start(CHANNEL_Sub, 0, chNum);
         ScalePOP ();
         return 0;
        }
        else
*/
        {

//         bStartSubChannel = TRUE;
//         return RKEY_NewF1; // use system to start subchannel
//         TAP_Channel_Start(CHANNEL_Sub, 0, chNum);
         StartSubChannel (chNum);
// if in file or timer half height, make it pop rather than pip, otherwise it looks funny
         if ((bFileMode && !bFileModeHeightFull) || (bTimerMode && !bTimerModeHeightFull))
         {
            iPOPshown = 1;
            ScalePOP ();
         }
         else
         {
            bPIPshown = TRUE;
            ScalePIP ();
         }
         return 0;
        }
       }
      }
// 08jun05     else
// qcc not for subchannel
      else if (!bTimerMode)
      {

// if clock shown, 1 and 2 keys are used to stop recording in corresponding slot, if any.
// but only if not in file (or timer, but that is checked above) mode,
// since these keys are needed for name entry (and play/rec info
// will not be displayed anyway).
         if (!bFileMode && bClockShown && ((dwParam1 == RKEY_1) || (dwParam1 == RKEY_2)))
         {
          for (i = 0; i <= 1; i++)
          {
//           if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]) && ((RecInfo[i].recType == RECTYPE_Normal) || (RecInfo[i].recType == RECTYPE_Copy)))
           if (bRecording[i])
           {
            if ((dwParam1 == RKEY_1 && i == 0) || (dwParam1 == RKEY_2 && i == 1))
            {
// if trying to stop recording which is also playing back, disallow, since it seems to hang the system
             if (bPlaying & (strcmp (FileListNameRec[i], FileListNamePlay) == 0))
                ShowFileErrorMsg ("Recording stop ignored since file is also playing back", FALSE);
             else
             {
                 strcpy (msg, "Press OK to stop recording of ");
// remove any ".rec" from end of file name
                 strcpy (strtemp, FileListNameRec[i]);
                 if (IsRec (strtemp))
                 {
                  len = strlen(strtemp);
                  if (len > 4)
                   strtemp[len-4] = '\0';
                 }
                 strcat (msg, strtemp);
                 ShowFileErrorMsg (msg, TRUE);
                 bStopRecInProgress = TRUE;
                 RecSlotBeingStopped = i;
             }
             return 0;
            }
           }
          }
         }

// qcc not preceded by UHF key, affects main channel
       if (chNum != -1 && !bFilePlayingBack)
       {
// if there are already 2 recordings in progress, return qcc key to system to produce new warning msg with 30mar05 firmware
// not sure this is a good idea, since could be changing to a channel which is already recording and it's
// slower if returned to system. may be just better if the Channel_Start gets ignored (particuarly as the warning message
// doesn't give the option of stopping the recording anyway).
/*
        len = 0;
        for (i = 0; i <= 1; i++)
        {
         if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]) && (RecInfo[i].recType == RECTYPE_Normal))
          len++;
        }
        if (len == 2)
         return dwParam1;
*/
//        TAP_Channel_Start(CHANNEL_Main, 0, chNum);
        if (TAP_Channel_Start(CHANNEL_Main, 0, chNum) != 0)
         ShowFileErrorMsg ("Unable to switch to specified channel", FALSE);
//        bChannelChanged = TRUE; // set flag for special handling of record key
        return 0;
       }
      }

     nochanstart:
// if we hit uhf then an invalid channel no. just reset the flag and don't process the key further
      if (bChanSub)
      {
       bChanSub = FALSE;
       return 0;
      }

     } // end qcc activated

    if (!bTimerMode)
    {
// key handling when neither options window nor timer mode, but could be in file mode

//     if (dwParam1 == RKEY_Mute)
     if (dwParam1 == KeyToggleClock)
     {
// toggle clock display if not in file or timer mode (these modes will show clock anyway)
//      if (!bFileMode)
// don't toggle clock if an error msg is shown, since they share the same area and it will clobber the message
      if (!bFileMode && !bFileErrorMsgShown)
      {
//       bClockShown = !bClockShown;
//       if (bClockShown)
       if (!bClockShown)
       {
// when clock exits normal, several generated key events, such as used for my play and stop keys, will not work if clock shown
//        TAP_ExitNormal ();
//        rgnClock = TAP_Osd_Create (ClockX, ClockY, ClockW, ClockH, 0, FALSE);
// if play counter is shown, drop it, since it uses same line as clock
        if (bPlayCounterShown)
            HidePlayCounter ();
        rgnClock = TAP_Osd_Create (0, ClockY, xmax, ClockH, 0, FALSE);
// supplementary region for up to 3 play/record status boxes
        rgnPlayRec = TAP_Osd_Create (0, PlayRecY, xmax, 3 * PlayRecH, 0, FALSE);
//        ClockCounter = 0;
        TickClockSaved = 0; // force clock update next idle
        bClockShown = TRUE;
       }
       else
/*
       {
        TAP_Osd_Delete (rgnClock);
        TAP_EnterNormal ();
       }
*/
        HideClock ();
       return 0;
      }
     }
/*
// record key shows reminder message about changing default recording time
// can't get record key to do what i want. seems system is very fussy about when it does or does not process
// a returned record key.

     if (!bFileMode && (dwParam1 == RKEY_Record))
     {
      if (!bRecordMsgShown)
      {
       ShowFileErrorMsg ("WARNING: to change recording time press RECORD again, else EXIT", FALSE);
       bRecordMsgShown = TRUE;
      }
      else
      {
//       HideFileErrorMsg ();
       bRecordMsgShown = FALSE;
      }
      return dwParam1;
     } // end record key and not file mode
*/


// use tv/stb key to start recording to see if it works during playback
// useless since it functions exactly as record key, i.e. starts a copy of recording playing back
/*
     if (dwParam1 == RKEY_TvSat)
     {
      if (!TAP_Hdd_StartRecord ())
      {
       ShowFileErrorMsg ("Start Record failed", FALSE);
      }
      return 0;
     }
*/

// following keys should only be acted on (by me) if not in menu state.
     if (State == STATE_Menu)
      return dwParam1;

     if (bFileDeleteInProgress)
     {
// if delete in progress (confirmation outstanding) any key other than ok will abort it
// (actually, due to spurious key events in dec 2004 firmware, only exit key will now abort it).
// if delete in progress, there will also be an error msg shown, so clear it
      if (dwParam1 == RKEY_Exit)
      {
       HideFileErrorMsg ();
       bFileDeleteInProgress = FALSE;
      }
      else if (dwParam1 == RKEY_Ok)
      {
       HideFileErrorMsg ();
       bFileDeleteInProgress = FALSE;
// bFilePlayingBack test no good, because could have stopped by other means (and I still think I am playing back),
// so just check that we are deleting
// the file currently playing or last played.
       if (strcmp (FileNameAction, FileListNamePlay) == 0)
       {
// try to open file we may be playing. if we can it's not playing back, so just delete without pre-delete stop.
        bFilePlayingBack = FALSE;
        FileDisplayPointer = TAP_Hdd_Fopen (FileNameAction);
        if (FileDisplayPointer != 0)
        {
         TAP_Hdd_Fclose (FileDisplayPointer);
         DeleteFileNameAction ();
         return 0;
        }
        else
        {
// couldn't open file, so it's probably playing back. issue pre-delete stop.
//         bPreDeleteStop = TRUE;
//         if (bFileListShown)
         {
// 24apr05. if deleting from file list, returning stop key to system for pre-delete stop doesn't seem to work, so try
// stopts, which works fine when just stopping from file list.
          if (TAP_Hdd_StopTs () == 0)
          {
// increment cumulative count of ticks spent playing back.
           TickFilePlay += TAP_GetTick () - TickFilePlayStart;
           bFilePlayingBack = FALSE;
//           strcpy (FileListNamePlay, "");
// refresh file list so color of file changes from playing to normal
// DeleteFileNameAction should do this after deleting file anyway.
//           ListCurrentFolder (FALSE);
           DeleteFileNameAction ();
          }
/* if the stopts doesn't work, then the subsequent delete should fail anyway, giving us another msg
   so we shouldn't really need this one
          else
          {
           strcpy (msg, "Stopping playback of ");
           strcat (msg, FileListNamePlay);
           strcat (msg, " failed");
           ShowFileErrorMsg (msg, TRUE);
          }
*/
          return 0;

         }
/*
         else
         {
//          bPreDeleteStop = TRUE;
//           bFilePlayingBack = FALSE;
          return RKEY_Stop;
//           TAP_GenerateEvent (EVT_KEY, RKEY_Stop, RKEY_Stop); // doesn't work (delete fails)
         }
*/
        }
       } // end of file is playing or last one played
//       else if (!bFilePlayingBack)
       else
       {
// file is not playing or last played, so just try delete
        DeleteFileNameAction ();
        return 0;

       }

      } // end ok key
      return 0;
     } // end file delete in progress

// if stop recording in progress, ok key key will confirm, exit key will cancel
     if (bStopRecInProgress)
     {
      if (dwParam1 == RKEY_Exit)
      {
       HideFileErrorMsg ();
       bStopRecInProgress = FALSE;
      }
      else if (dwParam1 == RKEY_Ok)
      {
       bStopRecInProgress = FALSE;
       HideFileErrorMsg ();
// 01may05 hidefileerrormsg deletes rgnfile, which causes a hole where the clock would be (they share the same area)
// so force clock update immediately, instead of waiting for stoprecord to complete, since it looks funny
// when the clock disappears for a while. if stoprecord fails, clock will get replaced by new message anyway.
       UpdateClock ();
       if (!TAP_Hdd_StopRecord (RecSlotBeingStopped))
       {
        strcpy (msg, "Stop record of ");
// remove any ".rec" from end of file name (I really should write a function for this)
        strcpy (strtemp, FileListNameRec[RecSlotBeingStopped]);
        if (IsRec (strtemp))
        {
         len = strlen(strtemp);
         if (len > 4)
          strtemp[len-4] = '\0';
        }
        strcat (msg, strtemp);
        strcat (msg, " failed");
        ShowFileErrorMsg (msg, TRUE);
       }
// if we stopped recording, force refresh of duration, etc. when file list next shown
       else
       {
//        UpdateClock ();
        bFileListRefreshDuration = TRUE;
       }
      }
      return 0;
     }
/*
     if (bFileErrorMsgShown)
     {
// any other key will clear error msg
// 08jun05 file or timer height switch key should not do it
        if (dwParam1 != KeyToggleListHeight)
            HideFileErrorMsg ();
// continue processing the key used to clear the message
// unless it's the info key when file list is shown, since this should just clear the cluster info in the error line
// (and not put it back again).
        if ((dwParam1 == RKEY_Info) && bFileListShown)
           return 0;
     }
*/
     if (bFileErrorMsgShown)
     {
        if (dwParam1 == RKEY_Exit)
        {
// exit key in file mode needs special processing, done later
            if (!bFileMode)    
            {
                HideFileErrorMsg ();
                return 0;
            }
        }
     }

     switch (dwParam1)
     {

     case RKEY_Play:
      {
// play ts files
       bPlaybackPaused = FALSE;
       if (bFileListShown)
       {
// play selected file from start
        FileListEntry = FileListScreenArray[FileListIndex - FileListIndexTop];
        if (FileListEntry.attr == ATTR_TS)
        {
         strcpy (FileListNamePlay, FileListEntry.name);
//         FileListStartTimePlay = FileListEntry.mjd << 16 | FileListEntry.hour << 8 | FileListEntry.min;
         FileListStartPlayMjd = FileListEntry.mjd;
         FileListStartPlayHour = FileListEntry.hour;
         FileListStartPlayMin = FileListEntry.min;
         FileListStartPlaySec = 0;  //this may get overridden by ts header seconds in PlayFileListNamePlay
// save TS file header (including duration_ before we play it, because we can't open it while it is playing
         SaveTsFileHeader (FileListEntry);
         HideFileList (FALSE);
         PlayFileListNamePlay (FALSE);
        }
       }
       else
       {
// file list not shown, resume playback of last-played file
// if already playing back, pass play key to system to bring up progress bar
// 17jun05 unless the clock (with playinfo) is shown, in which case play key toggles between using
// initial (duration-based) block rate and dynamic one.
//        if (bFilePlayingBack)
//        if (TAP_Hdd_GetPlayInfo (&PlayInfo) && (PlayInfo.playMode == PLAYMODE_Playing))
        if (bPlaying)
        {
            if (bClockShown)
            {
// 15aug05 extra function for play key if clock shown, so that showing block counts in play info can be optional
// if block counts are not currently shown, set flag so that they will be.
// otherwise, toggle between initial and dynamic block rates.
                if (bBlockRatesShown)
                {
                    bBlockRateDynamic = !bBlockRateDynamic;
                    if (bBlockRateDynamic)
                        BlockRateNormalPlay = BlockRateNormalPlayDyn;
                    else
                        BlockRateNormalPlay = BlockRateNormalPlayInit;
                }
                else
                    bBlockRatesShown = TRUE;
                return 0;
            }
            else   
                return dwParam1;
        }
        if (strcmp (FileListNamePlay, "") != 0) // check if something played
         PlayFileListNamePlay (TRUE); // resume playback of last-played file
        else if (strcmp (FileListNamePlayNext, "") != 0) // name of next file to play set in DeleteFileNameAction
        {
         strcpy (FileListNamePlay, FileListNamePlayNext);
//         FileListStartTimePlay = FileListStartTimePlayNext;
         FileListStartPlayMjd = FileListStartPlayNextMjd;
         FileListStartPlayHour = FileListStartPlayNextHour;
         FileListStartPlayMin = FileListStartPlayNextMin;
         FileListStartPlaySec = FileListStartPlayNextSec;
         PlayFileListNamePlay (FALSE); // start playback of next file
        }
        else
        {
         strcpy (msg, "Nothing to play back");
         ShowFileErrorMsg (msg, TRUE);
        }
       } // end of file list not shown
       return 0;
      } // end of play key

     case RKEY_Stop:
      {
// if we are processing the stop key we generated to stop playback, return it to system.
/* 11jun05
       if (bFilePlaybackStopInProgress)
       {
        bFilePlaybackStopInProgress = FALSE;
        return dwParam1;
       }
*/
// if in file list state, system wants stop key to stop selected recording (and it won't stop playback)
// if playing back but not from my file list, let system process stop key
// 22mar05     if (State == STATE_FileList || !bFilePlayingBack)
       if (State == STATE_FileList)
        return dwParam1;

       if (!bFileListShown)
       {
// drop bookmark for subsequent resume play
        if (bFilePlayingBack)
        {
//         bBookmark = FALSE;
// 11jun05         bFilePlayingBack = FALSE;
// 11jun05         bFilePlaybackStopInProgress = TRUE;
// generate events seem not to work if clock shown (when clock exits normal)
// so kill the clock if it is shown
//         if (bClockShown)
//          HideClock ();
// show progress bar if not already shown, just so we can see where we are dropping bookmark

//         if (SubState != SUBSTATE_PvrPlayingSearch)
//          TAP_GenerateEvent (EVT_KEY, RKEY_Play, RKEY_Play);
/* 11jun05 instead of dropping bookmark, save current block, so can return to it with new block
  positioning function
         TAP_GenerateEvent (EVT_KEY, RKEY_Bookmark, RKEY_Bookmark);
//         TAP_Delay (50);
         TAP_GenerateEvent (EVT_KEY, RKEY_Stop, RKEY_Stop);
*/
         if (TAP_Hdd_GetPlayInfo (&PlayInfo) && (PlayInfo.playMode == PLAYMODE_Playing))
         {
// stopts somehow zeroes the current block count in playinfo structure even though getplayinfo
// has not been called again, so need to
// save the current block before calling stopts.
          block = PlayInfo.currentBlock;
          if (TAP_Hdd_StopTs () == 0)
          {
           BlockPlayStopped = block;
//           sprintf (msg, "Playback stopped at block %d", BlockPlayStopped);
//           ShowFileErrorMsg (msg, FALSE);
// increment cumulative count of ticks spent playing back.
           TickFilePlay += TAP_GetTick () - TickFilePlayStart;
           bFilePlayingBack = FALSE;
          }
          else
          {
           strcpy (msg, "Stopping playback of ");
           strcat (msg, FileListNamePlay);
           strcat (msg, " failed");
           ShowFileErrorMsg (msg, TRUE);
          }
         }
         else
           ShowFileErrorMsg ("Unable to get PlayInfo block count", TRUE);
// increment cumulative count of ticks spent playing back.
// 11jun05         TickFilePlay += TAP_GetTick () - TickFilePlayStart;
         return 0;
        } // end if file playing back while my file list not shown
        else
         return dwParam1; // let system process stop key (e.g. to stop recording)
//        return 0;
       }
       else
       {
        FileListEntry = FileListScreenArray[FileListIndex - FileListIndexTop];
// my file list is shown. 
// if current file in list is recording, stop the recording.
// if current file is playing, stop playback without bookmark and clear out last-played file,
// so we don't attempt resume play on it, since there will be no bookmark and we would skip 30s and set bookmark.
// recording will take precedence over playback if both are true.
// if current file is neither recording nor playing, ignore stop key.
        for (i = 0;  i <= 1; i++)
        {
// check name against my copy of what's recording, since I may have renamed it unknown to getrecinfo
// 20jul05 stop record while playing back the same file hangs the system, so drop through
// and treat it as stop playback instead
         if (strcmp (FileListEntry.name, FileListNameRec[i]) == 0)
         {
            if (strcmp (FileListEntry.name, FileListNamePlay) != 0)
            {
              if (TAP_Hdd_StopRecord (i))
              {
               strcpy (FileListNameRec[i], "");
               bFileListNameChangedRec[i] = FALSE;
// refresh file list so color of file changes from recording to normal
// first need to scan current folder to update file size counts
               ScanCurrentFolder ();
               bFileListRefreshDuration = TRUE;
               ListCurrentFolder (TRUE);
               bRefreshFileListInIdle = FALSE;  //avoid double refresh when idle detects recording has stopped
              }
              else
              {
               strcpy (msg, "Stop record of ");
// remove any ".rec" from end of file name (I really should write a function for this)
               strcpy (strtemp, FileListNameRec[i]);
               if (IsRec (strtemp))
               {
                len = strlen(strtemp);
                if (len > 4)
                 strtemp[len-4] = '\0';
               }
               strcat (msg, strtemp);
               strcat (msg, " failed");
               ShowFileErrorMsg (msg, TRUE);
              }
              return 0;
            }   // end of file is not also playing
         }
        }
//        return 0;
        
// 13may05. playback can stop outside my control, so bFilePlayingBack may not really indicate whether
// playback is really happening. Stopping from file list if playback has already stopped, crashes
// system (not sure why, perhaps it doesn't like StopTs in this state) so use GetPlayInfo instead
//        if (bFilePlayingBack)
        if (TAP_Hdd_GetPlayInfo (&PlayInfo) && (PlayInfo.playMode == PLAYMODE_Playing))
        {
         if (strcmp (FileListEntry.name, FileListNamePlay) == 0)
         {
          if (TAP_Hdd_StopTs () == 0)
          {
// increment cumulative count of ticks spent playing back.
           TickFilePlay += TAP_GetTick () - TickFilePlayStart;
           bFilePlayingBack = FALSE;
           strcpy (FileListNamePlay, "");
// refresh file list so color of file changes from playing to normal
// and set flag to refresh duration, etc. from header, since we were previously getting it from data
// saved prior to starting play.
           bFileListRefreshDuration = TRUE;
           ListCurrentFolder (FALSE);
          }
          else
          {
           strcpy (msg, "Stopping playback of ");
           strcat (msg, FileListNamePlay);
           strcat (msg, " failed");
           ShowFileErrorMsg (msg, TRUE);
          }
          return 0;
         }
        }
       }
       return dwParam1;
      }

     case RKEY_Pause:
      {
       if (bFilePlayingBack)
        bPlaybackPaused = !bPlaybackPaused;
       return dwParam1;
      }

// if file list is up, slow key will force refresh of duration, etc. from ts file header
// otherwise, if not playing back, try to use it a a record key which forces an unlimited recording duration.

     case RKEY_Slow:
      {
       if (bFileListShown)
       {
        bFileListRefreshDuration = TRUE;
        ScanCurrentFolder ();
        ListCurrentFolder (TRUE);
        return 0;
       }
/* doesn't work with 30mar05 or 10may05 without a lot of fiddling, so do without it.
       else if (!bFileMode && !bFilePlayingBack)
       {
        if (!TAP_Hdd_StartRecord ())
         ShowFileErrorMsg ("Start Record failed", FALSE);
        else
        {
         TAP_GenerateEvent (EVT_KEY, RKEY_Record, RKEY_Record);
         for (i = 1; i <= 116; i++)
          TAP_GenerateEvent (EVT_KEY, RKEY_VolDown, RKEY_VolDown);
         TAP_GenerateEvent (EVT_KEY, RKEY_Record, RKEY_Record);

//         bUnlimitedRecordPending = TRUE;
//         UnlimitedRecordLoopCount = 12; // not needed with 06dec04 fw (only 30mar05)
// handle rest of this in idle event
        }
        return 0;
       }
*/
       else
        return dwParam1;
      }

     case RKEY_Ab:
      {
// white key used for delete char in name entry mode, so leave for possible later processing
       if (bNameEntryMode)
        break;
       else
       {
        if (State != STATE_FileList)
        {
// white key will prompt for deletion of currently playing (or last played) file if file list not shown,
// else will prompt for deletion of selected entry.
         if (bFileListShown)
         {
          FileListEntry = FileListScreenArray[FileListIndex - FileListIndexTop];
          strcpy (FileNameAction, FileListEntry.name);
         }      
         else
         {
// check that there is a file playing (or last played and not yet deleted)
          if (strcmp (FileListNamePlay, "") == 0)
           return dwParam1;
          strcpy (FileNameAction, FileListNamePlay);
         }
         if (!TAP_Hdd_Exist (FileNameAction) && !bFileListShown)
         {

// try to find last played file in DataFiles Folder, since we may have changed current folder
          ChangeToDataFilesFolder ();
         }
         if (TAP_Hdd_Exist (FileNameAction))
         {
          strcpy (msg, "Press OK to confirm deletion of ");
          bFileDeleteInProgress = TRUE;
         }
         else
          strcpy (msg, "Can't find ");
         strcat (msg, FileNameAction);
         ShowFileErrorMsg (msg, TRUE);
         return 0;
        }
        else
         return dwParam1; // let system process white key in filelist state
       } // end of not name entry mode
      }

     case KeyToggleTimerMode:
      {
//       if (!bFileMode)
       if (!bFileMode && (State != STATE_FileList))
       {
// only enter timer mode if not in file mode
// if timer list brought up while progress bar shown, system stuffs up my display, even though ExitNormal
// so disallow this
// 21apr05 try dropping progress bar first
//        if (!IsProgressBarShown ())
        if (IsProgressBarShown ())
        {
         bShowTimerListPending = TRUE;
         return RKEY_Play;
        }
        else
        {
         if (bPlayCounterShown)
            HidePlayCounter ();
         ShowTimerList (FALSE);
         return 0;
        }
       }
      }

     } // end of switch on dwParam1

     if (!bFileMode)
     {
      switch (dwParam1)
      {
      case KeyToggleFileMode:
       {
// 28apr05. don't bring up my file list if system one is up.
        if (State != STATE_FileList)
        {
//          bFileMode = TRUE;
// if file list brought up while progress bar shown, system stuffs up my display, even though ExitNormal
// so disallow this
// 21apr05 try with play key to drop progress bar first
//          if (!IsProgressBarShown ())
         if (IsProgressBarShown ())
         {
          bShowFileListPending = TRUE;
          return RKEY_Play;
         }
         else
         {
          if (bPlayCounterShown)
             HidePlayCounter ();
          ShowFileList (FALSE);
          return 0;
         }
        }
       }

//      case RKEY_NewF1:
      case RKEY_Recall:
       {
// red key skips back 30 sec or so if playing back and not in filelist or teletext state
// 13jan05 changed to recall key, so that red key can revert to normal PIP operation.
// recall key skips back 30 sec or so if playing back and not in filelist state

        if (bFilePlayingBack && (State != STATE_FileList))
        {
// generate events seem not to work if clock shown (when clock exits normal)
// so kill the clock if it is shown
//         if (bClockShown)
//          HideClock ();
/*
         if (!IsProgressBarShown ())
          TAP_GenerateEvent (EVT_KEY, RKEY_Play, RKEY_Play);
// with 10may05 firmware, 7 keys causes skip back to be about 40 sec, so try 6.
// this does very close to 30 sec now.
//         for (i = 1; i <= 7; i++)
         for (i = 1; i <= 6; i++)
//          TAP_GenerateEvent (EVT_KEY, RKEY_Rewind, RKEY_Rewind);
          TAP_GenerateEvent (EVT_KEY, RKEY_VolDown, RKEY_VolDown);
*/
         SkipPlayback (-30);
         return 0;
        }
        else
         break;
       }

//      case RKEY_Check:
      case RKEY_Prev:
       {
// |< key skips back 15 sec or so if playing back and not in filelist state
        if (bFilePlayingBack && (State != STATE_FileList))
        {
// generate events seem not to work if clock shown (when clock exits normal)
// so kill the clock if it is shown
//         if (bClockShown)
//          HideClock ();
/*
         if (!IsProgressBarShown ())
         {
          TAP_GenerateEvent (EVT_KEY, RKEY_Play, RKEY_Play);
//          bPlayGen = TRUE;
         }
//         else


//          bPlayGen = FALSE;
// with 10may05 firmware, 5 keys causes skip back to be about 24 sec, so try 4.
// this does about 14 sec now.
//         for (i = 1; i <= 5; i++)
         for (i = 1; i <= 4; i++)
//          TAP_GenerateEvent (EVT_KEY, RKEY_Rewind, RKEY_Rewind);
          TAP_GenerateEvent (EVT_KEY, RKEY_VolDown, RKEY_VolDown);
*/
         SkipPlayback (-15);
         return 0;
        }
        else
//         return dwParam1;
            break;
       }

      case RKEY_Next:
       {
// >| key skips forward 10 sec or so if playing back and not in filelist state
        if (bFilePlayingBack && (State != STATE_FileList))
        {
// generate events seem not to work if clock shown (when clock exits normal)
// so kill the clock if it is shown
//         if (bClockShown)
//          HideClock ();
/*
         if (!IsProgressBarShown ())
         {
          TAP_GenerateEvent (EVT_KEY, RKEY_Play, RKEY_Play);
//          bPlayGen = TRUE;
         }
//         else
//          bPlayGen = FALSE;

// with 10may05 firmware, 4 keys causes skip forward to be about 18 sec, so try 3.
// this does about 12 sec now.
//         for (i = 1; i <= 4; i++)
         for (i = 1; i <= 3; i++)
//          TAP_GenerateEvent (EVT_KEY, RKEY_Forward, RKEY_Forward);
          TAP_GenerateEvent (EVT_KEY, RKEY_VolUp, RKEY_VolUp);
*/
         SkipPlayback (15);
         return 0;
        }
        else
         break;
       }

      case RKEY_Fav:
       {
        if (bFilePlayingBack && (State != STATE_FileList))
        {
            SkipPlayback (30);
            return 0;
        }
        else
            break;
       }

//      case RKEY_Sat:
//      case RKEY_Check:
      case RKEY_Guide:
       {

// guide key skips forward 2 minutes if playing back
// this key does nothing in playback in normal system, so seems a good choice for this
//        if (bFilePlayingBack)
        if (bFilePlayingBack && (State != STATE_FileList))
        {
// generate events seem not to work if clock shown (when clock exits normal)
// so kill the clock if it is shown
//         if (bClockShown)
//          HideClock ();
/*
         for (i = 1; i <= 4; i++)
          TAP_GenerateEvent (EVT_KEY, RKEY_Goto, RKEY_Goto);
*/
// 09jun05 try the new api v1.22 function for block count positioning
            SkipPlayback (120);
            return 0;
        }
        else
            break;
       }

// for playback numeric jump, ok key jumps to block corresponding to entered hours and minutesinto file
      case RKEY_Ok:
        if (bPlaybackNumericJumpMode)
        {
            if (PlaybackNumericToMinutes (&minutes))
            {
// allow for apparent quick block count increment after positioning by positioning 3 secs ahead.
                block = ((BlockRateNormalPlay * (minutes * 60 - 3)) + 300) / 600;
                PositionPlayback (block);
            }
            TerminatePlaybackNumericKey ();
            return 0;
        }
        else
            break;        

// for playback numeric jump, volume up key jumps forward by entered hours and minutes
      case RKEY_VolUp:
/*
        if (bFilePlayingBack && (State != STATE_FileList))
        {
            PlaybackJumpType = 1;
// forward jump, display + before the numeric chars entered
            CreatePlaybackNumericRegion ();
            TAP_Osd_PutS (rgnNumKey, 50, 0, 70, "+", COLOR_Black, COLOR_OrangeYellow, 0, FNT_Size_1926, 0, ALIGN_CENTER);
            PlaybackNumericKeyCount = 0;
            return 0;
        }
*/
        if (bPlaybackNumericJumpMode)
        {
            if (PlaybackNumericToMinutes (&minutes))
                SkipPlayback (minutes * 60);
            TerminatePlaybackNumericKey ();
            return 0;
        }
        else
            break;        

// for playback numeric jump, volume down key jumps backward by entered hours and minutes
      case RKEY_VolDown:
/*
        if (bFilePlayingBack && (State != STATE_FileList))
        {
            PlaybackJumpType = 2;
// backward jump, display + before the numeric chars entered
            CreatePlaybackNumericRegion ();
            TAP_Osd_PutS (rgnNumKey, 50, 0, 70, "-", COLOR_Black, COLOR_OrangeYellow, 0, FNT_Size_1926, 0, ALIGN_CENTER);
            PlaybackNumericKeyCount = 0;
            return 0;
        }
*/
        if (bPlaybackNumericJumpMode)
        {
            if (PlaybackNumericToMinutes (&minutes))
                SkipPlayback (- minutes * 60);
            TerminatePlaybackNumericKey ();
            return 0;
        }
        else
            break;        

//      case RKEY_Check:
      case RKEY_Sat:
       {
// sat key initates special playback scan mode, in which we alternately skip 30 sec, then play for 4 sec.
// until play or stop key is pressed.
// toggle between slow (default) and fast play scan speeds.
        if (bPlayScanMode)
        {
         bPlayScanFast = !bPlayScanFast;
         return 0;
        }
        else if (bFilePlayingBack && (State != STATE_FileList) && (State != STATE_Ttx))
        {
// generate events seem not to work if clock shown (when clock exits normal)
// so kill the clock if it is shown
//         if (bClockShown)
//          HideClock ();
         bPlayScanMode = TRUE;
         bPlayScanFast = FALSE;
//         TAP_GenerateEvent (EVT_KEY, RKEY_Goto, RKEY_Goto);
         SkipPlayback (30);
         TickPlayScanSaved = TAP_GetTick ();
         return 0;
        }
        else
            break;
       }

      case KeyEnterTimedStopRec:
       {
// prompt for entry of time at which to stop all recordings and issue sleep key
// this gives idle event time to rewrite ts headers, which the pending off state does not.
        if (!bTimedStopRecEntryMode)
        {
            bTimedStopRecEntryMode = TRUE;
// if we have already set a stop rec time, display it
            if (TimedStopRecMjd != 0)
                sprintf (TimedStopRecEntry, "%02d%02d", TimedStopRecHour, TimedStopRecMin);
            else
                strcpy (TimedStopRecEntry, "____");
/*
            TimedStopRecMjd = 0;
            TimedStopRecHour = 0;
            TimedStopRecMin = 0;
*/
            TimerCharIndex = 0;
            ShowTimedStopRecEntry ();
            return 0;
        }
        else
            break;
       }

      } // end of switch on key
     } // end of not file mode
    } // end of not timer mode
} // end of KeyHandlerCommon

    if (bTimerMode)
//     #include "KeyHandlerTimer.c"

// ViewMaster3 keyhandler for timer mode
{
     if (bTimerDeleteInProgress || bTimerModifyInProgress)
     {
// if delete or modify in progress (confirmation outstanding) any key other than ok will abort it
// if delete or modify in progress, there will also be an error msg shown, so clear it
//      HideTimerErrorMsg ();
//      if (dwParam1 != RKEY_Ok)
      if (dwParam1 == RKEY_Exit)
      {
       HideTimerErrorMsg ();
       if (bTimerDeleteInProgress)
           bTimerDeleteInProgress = FALSE;
       else
       {
// 05jun05 when timer modify cancelled, clear timer entry fields so we can start afresh with a new entry.
           ClearAllTimerEntryFields ();
           bTimerModifyInProgress = FALSE;
       }
       return 0;
      }
     }

//   if (bErrorMsgShown && !bTimerModifyInProgress)
//     if (bErrorMsgShown && !bTimerModifyInProgress && !bFieldError)
     if (bErrorMsgShown && !bTimerModifyInProgress && !bTimerDeleteInProgress && !bFieldError)
     {
// any key will clear error msg
      HideTimerErrorMsg ();
      return 0;
     }

//     if (dwParam1 == KeyToggleTimerMode)
     if (dwParam1 == KeyToggleTimerMode || dwParam1 == RKEY_Exit)
     {
//    bTimerMode = FALSE;
      HideTimerList (FALSE);
//   bTimerDeleteInProgress = FALSE;
//   bTimerModifyInProgress = FALSE;
      return 0;
     }

     else if (bNameEntryMode)
     {
// process numeric keys, etc. for entry of file name
      if (ProcessNameEntryKey (dwParam1))
       return 0;
     }

// Timer entry keys
     switch (dwParam1)
     {
      case RKEY_0: KeyChar = 0x30; break;
      case RKEY_1: KeyChar = 0x31; break;
      case RKEY_2: KeyChar = 0x32; break;
      case RKEY_3: KeyChar = 0x33; break;
      case RKEY_4: KeyChar = 0x34; break;
      case RKEY_5: KeyChar = 0x35; break;
      case RKEY_6: KeyChar = 0x36; break;
      case RKEY_7: KeyChar = 0x37; break;
      case RKEY_8: KeyChar = 0x38; break;
      case RKEY_9: KeyChar = 0x39; break;
      default:  goto KeyNotNumeric;
     }
     if (TimerCharIndex <= TimerEntryFieldLen[TimerFieldIndex] - 1)
     {
      if (TimerCharIndex == 0)
       ClearTimerEntryField ();
      TimerEntryFieldData[TimerFieldIndex][TimerCharIndex] = KeyChar;
      TimerEntryFieldData[TimerFieldIndex][TimerCharIndex+1] = '\0';
      PutFieldEntry ();

      HideEntryInsertionField ();
      TimerCharIndex += 1;
      if (TimerCharIndex >= TimerEntryFieldLen[TimerFieldIndex])
// when we get to end of channel field, skip next field (tuner) and go straight to filename, since tuner
// nearly always defaults to 4 with latest firmware
      { 
       MoveToNextEntryField ();
       if (TimerFieldIndex == 5)
       {
        HideEntryInsertionField ();
        MoveToNextEntryField ();
       }
      }
      else
       ShowEntryInsertionField ();
     }
     return 0;
    KeyNotNumeric:
     if (dwParam1 == RKEY_Recall)
     {
// recall key is a backspace to previous character position
      if (TimerCharIndex > 0)
      {
// back up one character in current field
//     TimerEntryFieldData[TimerFieldIndex][TimerCharIndex] = '\0';
       TimerCharIndex -= 1;
       TimerEntryFieldData[TimerFieldIndex][TimerCharIndex] = 0x20;
//     PutFieldEntry ();
      }
      else
      {
// move to last character of previous field
       HideEntryInsertionField ();
       MoveToPrevEntryField ();
       if (TimerFieldIndex >= NumTimerEntryFields)
        return 0; // if we moved to file name field
       TimerCharIndex = TimerEntryFieldLen[TimerFieldIndex] - 1;
//      TimerEntryFieldData[TimerFieldIndex][TimerCharIndex+1] = '\0';
       TimerEntryFieldData[TimerFieldIndex][TimerCharIndex] = 0x20;
      }
      PutFieldEntry ();
      return 0;
     }

//   if (dwParam1 == RKEY_VolUp)
     else if (dwParam1 == RKEY_Next)
     {
      HideEntryInsertionField ();
      MoveToNextEntryField ();
      return 0;
     }

//   else if (dwParam1 == RKEY_VolDown)
     else if (dwParam1 == RKEY_Prev)
     {
      HideEntryInsertionField ();
      MoveToPrevEntryField ();
      return 0;
     }

//   else if (dwParam1 == RKEY_VolUp)
     else if (dwParam1 == RKEY_Forward)
     {
// scroll values right in timer entry mode or date field
      if (TimerFieldIndex == 0)
      {
       TimerReservationType = (TimerReservationType + 1) % N_ReserveType;

       ShowTimerReservationType ();
      }
      else if (TimerFieldIndex == 1)
      {
       TimerDate += 1;
       ShowTimerDate ();
      }
      return 0;
     }

//   else if (dwParam1 == RKEY_VolDown)
     else if (dwParam1 == RKEY_Rewind)
     {
// scroll values left in timer entry mode or date field
      if (TimerFieldIndex == 0)
      {
       if (TimerReservationType == 0)
        TimerReservationType = N_ReserveType - 1;
       else
        TimerReservationType -= 1;
       ShowTimerReservationType ();
      }

      else if (TimerFieldIndex == 1)
      {
       TAP_GetTime( &mjd, &hour, &min, &sec);
       if (TimerDate > mjd)
       {
        TimerDate -= 1;
        ShowTimerDate ();
       }
      }
      return 0;
     }

     else if (dwParam1 == RKEY_Ok)
     {
      if (bTimerDeleteInProgress)
      {
//       if (TAP_Timer_Delete (TimerListIndex))
       if (TAP_Timer_Delete (TimerNumArray[TimerListIndex]-1))
       {
        HideTimerErrorMsg ();
        RefreshTimerListArray ();
// if we just deleted the entry at the top of the screen and there are more in front,
// get previous screenful
        if ((TimerListIndex == TimerListIndexTop) && (TimerListIndexTop > 0))
        {
         TimerListIndexTop = max (TimerListIndexTop - TimerListEntriesPerScreen, 0);
//         TimerListIndexBottom = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TimerListIndexMax - 1);
//         TimerListIndex = TimerListIndexBottom;
         TimerListIndex = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TimerListIndexMax - 1);
        }
        else
        {
// leave timerlistindextop alone
//         TimerListIndexBottom = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TimerListIndexMax - 1); // we haven't decremented max yet
         TimerListIndex = min (TimerListIndex, TimerListIndexMax - 1);     
        }
        ListTimers (TRUE);
       }
       else
        ShowTimerErrorMsg ("Timer entry deletion failed");    
       bTimerDeleteInProgress = FALSE;
       return 0;
      }
      else
      {
// validate all input fields before adding or modifying entry

       TimerFieldIndexSave = TimerFieldIndex;
       bFieldError = FALSE;
       for (TimerFieldIndex = TimerFieldIndexStart; TimerFieldIndex < NumTimerEntryFields; TimerFieldIndex++)
        if (!isTimerEntryFieldValid ())
        {
         TAP_Osd_PutS(rgnTimer, TimerEntryXoffset[TimerFieldIndex], TimerEntryYoffset, TimerWinW, TimerEntryFieldName[TimerFieldIndex], COLOR_ForeTimerEntryFixedError, COLOR_None, 0, FNT_Size_TimerEntry, 0, ALIGN_LEFT);
         if (!bFieldError)
         {
          bFieldError = TRUE;
          index = TimerFieldIndex;
         }
//     return 0;
        }
       if (bFieldError)
       {
        sprintf (msg, "Error in timer entry field(s) shown in red");
        ShowTimerErrorMsg (msg);
        TimerFieldIndex = index; // point to first field in error
        TimerCharIndex = 0; // point to start of first field in error
        return 0;
       }
       else
        HideTimerErrorMsg ();
// set up the timerinfo structure
// if start time less than now, bump date to tomorrow (already done when moved out of field)
//   if (TimerStartMinutes < TimerNowMinutes)
//    TimerDate += 1;
       if (TimerEndMinutes <= TimerStartMinutes)
        TimerDuration = TimerEndMinutes + 1440 - TimerStartMinutes;
       else
        TimerDuration = TimerEndMinutes - TimerStartMinutes;

       if (!bTimerModifyInProgress)
       {
// fields which we can't modify here must be left alone on modify
        timerInfo.isRec = 1;
        timerInfo.svcType = 0;
//     timerInfo.reservationType = TimerReservationTypeDefault;
       }
       timerInfo.tuner = TimerTuner;
       timerInfo.svcNum = TimerSvcNum;
       timerInfo.reservationType = TimerReservationType;
       timerInfo.duration = TimerDuration;
       timerInfo.startTime = (TimerDate<<16 & 0xffff0000) | (TimerStartHr<<8 & 0xff00) | (TimerStartMin & 0xff); 
       CompleteTimerFunction ();
       return 0;
      } // end of not delete in progress
     } // end of ok key
     else if (dwParam1 == RKEY_ChUp)
     {
// move up the timer list
      if (TimerListIndex > TimerListIndexTop)
      {
       TimerListIndex -= 1;
       bClearTimerList = FALSE;
      }
      else
      {
// we are at the top of the list
       if (TimerListIndexTop > 0)
       {
// top of list but more entries in front so get previous page
        bClearTimerList = TRUE;
        TimerListIndexTop = max (TimerListIndexTop - TimerListEntriesPerScreen, 0);
//        TimerListIndexBottom = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TimerListIndexMax);
        TimerListIndex -= 1;
       }
       else
       {
// top of list but no more entries in front so get last screenful
// if there is only 1 screenful, don't need to clear before listing
        if (TimerListIndexBottom < TimerListIndexMax)
         bClearTimerList = TRUE;
        else
         bClearTimerList = FALSE;
//        TimerListIndexBottom = TimerListIndexMax;
//        TimerListIndexTop = max (TimerListIndexBottom - TimerListEntriesPerScreen + 1, 0);
        TimerListIndexTop = max (TimerListIndexMax - TimerListEntriesPerScreen + 1, 0);
//        TimerListIndex = TimerListIndexBottom;
        TimerListIndex = TimerListIndexMax;
       }
      }
      ListTimers (bClearTimerList);
      return 0;
     }
     else if (dwParam1 == RKEY_ChDown)
     {
// move down the timer list
      if (TimerListIndex < TimerListIndexBottom)
      {
       TimerListIndex += 1;
       bClearTimerList = FALSE;
      }
      else
      {
       if (TimerListIndexBottom < TimerListIndexMax)
       {
// we are at bottom of screen and there are more entries, so get the next screenful
        bClearTimerList = TRUE;
//        TimerListIndexTop = TimerListIndexTop + TimerListEntriesPerScreen;
// 05jun05 just increment top by 1 so that next entry is at bottom, not top of next screen
        TimerListIndexTop ++;
        TimerListIndex ++;
       }
       else
       {
// we are at the bottom of the screen and there are no more entries, so get the first screenful
// if only 1 screenful, don't need to clear before list
        if (TimerListIndexTop > 0)
         bClearTimerList = TRUE;
        else
         bClearTimerList = FALSE;
        TimerListIndexTop = 0;
        TimerListIndex = TimerListIndexTop;
       }
//       TimerListIndexBottom = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TimerListIndexMax);
//       TimerListIndex = TimerListIndexTop;
      }
      ListTimers (bClearTimerList);
      return 0;
     }
     else if (dwParam1 == RKEY_VolUp)
     {
// scroll to next page in timer list
//      if (TimerListIndexBottom < TimerListIndexMax)
      if (TimerListIndex == TimerListIndexMax)
      {
// at bottom of last screen, get first screenful
        TimerListIndex = 0;
        TimerListIndexTop = 0;
      }
      else if (TimerListIndex == TimerListIndexBottom)
      {
// at bottom of current screen, get next screen
        TimerListIndex = min (TimerListIndex + TimerListEntriesPerScreen, TimerListIndexMax);
        TimerListIndexTop = max (TimerListIndex - TimerListEntriesPerScreen + 1, 0);
      }
      else
      {
        TimerListIndex = TimerListIndexBottom;
/* 05jul05
       TimerListIndexTop = min (TimerListIndexTop + TimerListEntriesPerScreen, TimerListIndexMax);
//       TimerListIndexBottom = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TimerListIndexMax);
       TimerListIndex = TimerListIndexTop;
*/
//       TimerListIndex = min (TimerListIndexTop + TimerListEntriesPerScreen, TimerListIndexMax);
/*
       TimerListIndex = min (TimerListIndex + TimerListEntriesPerScreen - 1, TimerListIndexMax);
       TimerListIndexTop = max (TimerListIndex - TimerListEntriesPerScreen + 1, 0);
*/
      }
      ListTimers (TRUE);
      return 0;
     }
     else if (dwParam1 == RKEY_VolDown)
     {
// scroll to previous page in timer list
//      if (TimerListIndexTop > 0)
      if (TimerListIndex == 0)
      {
// at top of first screen, get last screenful
        TimerListIndex = TimerListIndexMax;
        TimerListIndexTop = max (TimerListIndex - TimerListEntriesPerScreen + 1, 0);
      }
      else if (TimerListIndex == TimerListIndexTop)
      {
// at top of current screen, get previous screen
        TimerListIndex = max (TimerListIndex - TimerListEntriesPerScreen, 0);
        TimerListIndexTop = TimerListIndex;
      }
      else
      {
        TimerListIndex = TimerListIndexTop;
/*
       TimerListIndexTop = max (TimerListIndexTop - TimerListEntriesPerScreen, 0);
//       TimerListIndexBottom = min (TimerListIndexTop + TimerListEntriesPerScreen - 1, TimerListIndexMax);
       TimerListIndex = TimerListIndexTop;
*/
/*
       TimerListIndex = max (TimerListIndex - TimerListEntriesPerScreen + 1, 0);
       TimerListIndexTop = TimerListIndex;
*/
      }
      ListTimers (TRUE);
      return 0;
     }
     else if (dwParam1 == RKEY_F2)
     {
// green key, delete current timer entry
//      sprintf (msg, "Press OK key to confirm deletion of entry %02d", TimerListIndex + 1);
      sprintf (msg, "Press OK key to confirm deletion of entry %02d", TimerNumArray[TimerListIndex]);
      ShowTimerErrorMsg (msg);
      bTimerDeleteInProgress = TRUE;
      bTimerModifyInProgress = FALSE; // delete cancels modify
      return 0;
     }
     else if (dwParam1 == RKEY_F3)
     {
/* 01jun05 to be consistent with delete, use exit key to cancel modify, so that yellow only initiates modify,
   rather than toggle.
      bTimerModifyInProgress = !bTimerModifyInProgress;
      if (bTimerModifyInProgress)
      {
*/
       TimerFieldIndexSave = TimerFieldIndex;
// yellow key, modify current timer entry
// extract the data we can modify from current timer entry and place in entry fields
//       TAP_Timer_GetInfo (TimerListIndex, &timerInfo);
       TAP_Timer_GetInfo (TimerNumArray[TimerListIndex]-1, &timerInfo);
  
       TimerReservationType = timerInfo.reservationType;
       ShowTimerReservationType ();
 
       TimerDate = timerInfo.startTime >> 16;
       ShowTimerDate ();
 
       hhmm = ((timerInfo.startTime >> 8) & 0xff) * 100 + (timerInfo.startTime & 0xff);
       TimerFieldIndex = 2;
       sprintf (TimerEntryFieldData[TimerFieldIndex], "%04d", hhmm);
       PutFieldEntry ();

       endmin = ((timerInfo.startTime >> 8) & 0xff) * 60 + (timerInfo.startTime & 0xff) + timerInfo.duration;
       if (endmin >= 1440)
        endmin -= 1440;
       hhmm = (endmin / 60 * 100) + (endmin % 60);
       TimerFieldIndex = 3;
       sprintf (TimerEntryFieldData[TimerFieldIndex], "%04d", hhmm);
       PutFieldEntry ();

       TimerFieldIndex = 4;
       sprintf (TimerEntryFieldData[TimerFieldIndex], "%d", SvcNumToLcn (timerInfo.svcNum));
       PutFieldEntry ();
 
       TimerFieldIndex = 5;
       sprintf (TimerEntryFieldData[TimerFieldIndex], "%d", timerInfo.tuner + 1);
       PutFieldEntry ();

// put timer file name into buffer for editing and display it
       rgnNameEntry = rgnTimer;
       strcpy (NameEntryBuffer, timerInfo.fileName);
       if (IsRec (NameEntryBuffer))
       {
// remove the ".rec" from the end of the filename
// i can't find a substring function, so stick a string delimeter in the 4th last char to get rid of ".rec"
        len = strlen(NameEntryBuffer);
        if (len > 4)
         NameEntryBuffer[len-4] = '\0';
       }
       NameEntryBufferIndexMax = strlen (NameEntryBuffer) - 1;
       NameEntryBufferIndex = 0;
       ShowNameEntry (NameEntryBuffer);

//       HideEntryInsertionField ();  // clear any previous indication of current entry field
// why should modify change current insertion field? leave it up to other functions which move between fields
//       TimerFieldIndex = TimerFieldIndexStart;
       TimerFieldIndex = TimerFieldIndexSave;
//       TimerCharIndex = 0;
//       ShowEntryInsertionField ();

// for some reason, the above code clears the last timer entry, immediately above the timer entry line,
// so refresh the list to get it back.
//       ListTimers (FALSE);

// set flag so that a subsequent ok key will do the modify (rather than add)
// pressing yellow key again will toggle between modify and add mode

//       TimerModifyEntryIndex = TimerListIndex;
       TimerModifyEntryIndex = TimerNumArray[TimerListIndex] - 1;
       bTimerModifyInProgress = TRUE;
// delete and modify are mutually exclusive, so cancel any delete
       bTimerDeleteInProgress = FALSE;
       bNameEntryMode = FALSE;
       sprintf (msg, "Modify in progress for timer entry %d", TimerModifyEntryIndex + 1);
       ShowTimerErrorMsg (msg);
/*
      }
      else
       HideTimerErrorMsg ();
*/
      return 0;
     }
     else if (dwParam1 == RKEY_F4)
     {
// blue key shows options window for timer sort
      ShowTimerSortOptionsWindow ();
      return 0;
     }
//     else if (dwParam1 == RKEY_Mute)
     else if (dwParam1 == KeyToggleListHeight)
     {
// toggle between timer list full and half height (latter with picture below)
      HideTimerList (TRUE);
      bTimerModeHeightFull = !bTimerModeHeightFull;
      ShowTimerList (TRUE);
      return 0;
     }
     else if (dwParam1 == RKEY_Uhf)
     {
// uhf key adds a dummy entry as a quick way of testing scrolling, etc.
      TimerReservationType = TimerReservationTypeDefault; 
      TimerDateTest += 1;
      TimerDate = TimerDateTest;
      strcpy (TimerEntryFieldData[2], "0000");
      strcpy (TimerEntryFieldData[3], "0001");
      strcpy (TimerEntryFieldData[4], "1");
      strcpy (TimerEntryFieldData[5], "1");
      strcpy (TimerEntryFieldData[6], "0");
      TAP_GenerateEvent (EVT_KEY, RKEY_Ok, 0); // generate ok key so we do the add
      bTimerModifyInProgress = FALSE; // ensure we add not modify
     }

} // end of KeyHandlerTimer

    else if (bFileMode)
//     #include "KeyHandlerFile.c"

// ViewMaster3 key handler for file mode
{
// keys used in file mode whether or not file list or content display shown
 switch (dwParam1)
 {
 case RKEY_Exit:
 {
  if (bNameEntryMode && !bFileErrorMsgShown)
  {
//   HideNameEntry ();
   HideNameEntryHelp ();
   bNameEntryMode = FALSE;
// name entry lines may have clobbered some file list entries, so refresh the list
// 01jun name entry help now on error line
   ListCurrentFolder (TRUE);
//   return 0;
  }
  else if (bFileCopyInProgress)
  {
// exit key aborts file copy. close source file if open
   if (FileCopySourcePointer != 0)
    TAP_Hdd_Fclose (FileCopySourcePointer);
   ShowFileErrorMsg ("File copy aborted", TRUE);
   bFileCopyInProgress = FALSE;
//   return 0;
  }
  else if (bFileDisplayShown || bFileInfoShown)
  {
// exit file display mode and return to file list mode
   ExitFileDisplay ();
//   return 0;
  }
  else if (bFileErrorMsgShown)
  {
      HideFileErrorMsg ();
      if (bNameEntryMode)
        ShowNameEntryHelp ();
  }
  else if (bFileListShown)
  {
   HideFileList (FALSE);
//   return 0;
  }
//  break;
  return 0;
 }
 case KeyToggleFileMode:
 {
// coming out of file mode. only act if not in file content display (at least until resume file display works)
//       if (!bFileListShown)
  if (bFileDisplayShown)
  {
   {
    bResumeFileDisplay = TRUE;
    bFileDisplayShown = FALSE;
//         ExitFileDisplay ();
//         HideFileList ();
   }
  }
  else if (bFileInfoShown)
   TAP_Hdd_Fclose (FileDisplayPointer);
//       else
  HideFileList (FALSE);
  return 0;
 }
 case RKEY_Info:
 {
  if (!bFileListShown)
  {
   if (bFileInfoShown)
    ExitFileDisplay ();
   return 0;
  }
/* should be done by keyhandlercommon when any key is pressed
  else if (bFileErrorMsgShown) // cluster info could be shown on error line, so clear it
  {
   HideFileErrorMsg ();
   return 0;
  }
*/
  else
   break; // further processing will be done below
 }
// case RKEY_Mute:
 case KeyToggleListHeight:
 {
// toggle between file list/display full and half height (latter with picture below)
  if (bFileDisplayShown)
  {
   bResumeFileDisplay = TRUE;
   bFileDisplayShown = FALSE;
  }

  HideFileList (TRUE);
  bFileModeHeightFull = !bFileModeHeightFull;
  ShowFileList (TRUE);
/*
  if (bFileDisplayShown)
  {
   bFileListShown = FALSE;
   DisplayFileBuffer ();
  }
*/
  return 0;
 }
/*
 default:
 {
// any other key clears error message if shown
//  if (bFileErrorMsgShown)
  if (bFileErrorMsgShown && !bFileCopyInProgress)
  {
   HideFileErrorMsg ();
   return 0;
  }
 }
*/
 } // end of switch on keys used in file mode, whether or not filelist or display shown

// following processing used only when file list shown

 if (bFileListShown)
 {
  if (bNameEntryMode)
  {
// process numeric keys, etc. for entry of file name
   if (ProcessNameEntryKey (dwParam1))
    return 0;
  }

  switch (dwParam1)
  {
  case RKEY_ChUp:
  {
// move up the file list
   if (FileListIndex > FileListIndexTop)
   {
    FileListIndex -= 1;
//          ShowFiles (FALSE);
    ShowFileListIndex (FileListIndex + 1); // unhighlight prev index
    ShowFileListIndex (FileListIndex); // highlight new index
    return 0;
   }
   else
   {
// we are at the top of the list
    if (FileListIndexTop > 1)
    {
// top of list but more entries in front so get previous page
     bClearFileList = TRUE;
     if (FileListIndexTop < FileListEntriesPerScreen)
      FileListIndexTop = 1;
     else
      FileListIndexTop = FileListIndexTop - FileListEntriesPerScreen;
//       FileListIndexTop = maximum (FileListIndexTop - FileListEntriesPerScreen, 1);
     FileListIndex -= 1;
    }
    else
    {
// top of list but no more entries in front so get last screenful
// if there is only 1 screenful, don't need to clear before listing
     if (FileListIndexBottom == FileListIndexMax)
     {
      FileListIndex = FileListIndexBottom;
//            ShowFiles (FALSE);
      ShowFileListIndex (FileListIndexTop); // unhighlight prev index
      ShowFileListIndex (FileListIndex); // highlight new index
      return 0;
     }
     else
     {
      bClearFileList = TRUE;
      if (FileListIndexMax <= FileListEntriesPerScreen)
       FileListIndexTop = 1;
      else
       FileListIndexTop = FileListIndexMax - FileListEntriesPerScreen + 1;
//            FileListIndexTop = maximum (FileListIndexMax - FileListEntriesPerScreen + 1, 1);
      FileListIndex = FileListIndexMax;
     }
    }
    bFileListRefreshDuration = TRUE;
   }
   ListCurrentFolder (bClearFileList);
   return 0;
  }
  case RKEY_ChDown:
  {
// move down the file list
   if (FileListIndex < FileListIndexBottom)
   {
    FileListIndex += 1;
//          ShowFiles (FALSE);
    ShowFileListIndex (FileListIndex - 1); // unhighlight previous index
    ShowFileListIndex (FileListIndex); // highlight new index
    return 0;
   }
   else
   {
    if (FileListIndexBottom < FileListIndexMax)
    {
// we are at bottom of screen and there are more entries, so get the next screenful
     bClearFileList = TRUE;
//     FileListIndexTop = FileListIndexTop + FileListEntriesPerScreen;
// 05jun05 now just get next entry and scroll the others up one.
     FileListIndexTop ++;
     FileListIndex ++;
    }
    else
    {
// we are at the bottom of the screen and there are no more entries, so get the first screenful
// if only 1 screenful, don't need to clear before list
     if (FileListIndexTop == 1)
     {
      FileListIndex = FileListIndexTop;
//            ShowFiles (FALSE);
      ShowFileListIndex (FileListIndexBottom); // unhighlight previous index
      ShowFileListIndex (FileListIndex); // highlight new index
      return 0;
     }
     else
     {
      bClearFileList = TRUE;
      FileListIndexTop = 1;
      FileListIndex = FileListIndexTop;
     }
    }
//    FileListIndex = FileListIndexTop;
    bFileListRefreshDuration = TRUE;
   }
   ListCurrentFolder (bClearFileList);
   return 0;
  }
  case RKEY_VolUp:
  {
// scroll to next page in file list
//         if (FileListIndexTop < FileListIndexMax)
//   if (FileListIndexBottom < FileListIndexMax)
      i = FileListIndexTop;
      if (FileListIndex == FileListIndexMax)
      {
// at bottom of last screen, get first screenful
        FileListIndex = 1;
        FileListIndexTop = 1;
      }
      else if (FileListIndex == FileListIndexBottom)
      {
// at bottom of current screen, get next screen
        FileListIndex = min (FileListIndex + FileListEntriesPerScreen, FileListIndexMax);
        FileListIndexTop = max (FileListIndex - FileListEntriesPerScreen + 1, 1);
      }
      else
      {
        FileListIndex = FileListIndexBottom;
      }
/*
    FileListIndexTop = min (FileListIndexTop + FileListEntriesPerScreen, FileListIndexMax);
//    FileListIndexBottom = min (FileListIndexTop + FileListEntriesPerScreen - 1, FileListIndexMax);
    FileListIndex = FileListIndexTop;
*/
//    FileListIndex = min (FileListIndexTop + FileListEntriesPerScreen, FileListIndexMax);
/*
    FileListIndex = min (FileListIndex + FileListEntriesPerScreen - 1, FileListIndexMax);
*/
//    FileListIndexTop = max (FileListIndex - FileListEntriesPerScreen + 1, 1);
    if (FileListIndexTop != i)
        bFileListRefreshDuration = TRUE;
    ListCurrentFolder (TRUE);
   return 0;
  }
  case RKEY_VolDown:
  {
// scroll to previous page in file list
//   if (FileListIndexTop > 1)
      i = FileListIndexTop;
      if (FileListIndex == 1)
      {
// at top of first screen, get last screenful
        FileListIndex = FileListIndexMax;
        FileListIndexTop = max (FileListIndex - FileListEntriesPerScreen + 1, 1);
      }
      else if (FileListIndex == FileListIndexTop)
      {
// at top of current screen, get previous screen
        FileListIndex = max (FileListIndex - FileListEntriesPerScreen, 1);
        FileListIndexTop = FileListIndex;
      }
      else
      {
        FileListIndex = FileListIndexTop;
      }

/*
    FileListIndexTop = max (FileListIndexTop - FileListEntriesPerScreen, 1);
//    FileListIndexBottom = min (FileListIndexTop + FileListEntriesPerScreen - 1, FileListIndexMax);
    FileListIndex = FileListIndexTop;
*/
/*
    FileListIndex = max (FileListIndex - FileListEntriesPerScreen + 1, 1);
    FileListIndexTop = FileListIndex;
*/
    if (FileListIndexTop != i)
        bFileListRefreshDuration = TRUE;
    ListCurrentFolder (TRUE);
   return 0;
  }
  case RKEY_Recall:
  {
   if (FileListPathLevel > 0)
   {
    TAP_Hdd_ChangeDir (".."); // change to parent directory
    FileListPathLevel -= 1;
    bFileListRefreshDuration = TRUE;
    ListNewFolder ();
   }
   return 0;
  }
  case RKEY_Ok:
  {
   if (bNameEntryMode)
   {
    if (bFolderCreationInProgress)
    {
// create folder if it does not already exist
     if (TAP_Hdd_Exist (NameEntryBuffer))
      ShowFileErrorMsg ("New folder name already exists", TRUE);
     else
     {
      TAP_Hdd_Create (NameEntryBuffer, ATTR_FOLDER);
//      HideNameEntry ();
      HideNameEntryHelp ();
      bNameEntryMode = FALSE;
      ScanCurrentFolder ();
      bFileListRefreshDuration = TRUE;
      ListCurrentFolder (TRUE);
//      bNameEntryMode = FALSE;
      bFolderCreationInProgress = FALSE;
     }
    } // end create folder
    else
    {
// rename file
     if (TAP_Hdd_Exist (NameEntryBuffer))
      ShowFileErrorMsg ("New file name already exists", TRUE);
//     else if (TAP_Hdd_Rename (FileRenameOld, NameEntryBuffer))
// rename returns false when it works
     else
     {
      TAP_Hdd_Rename (FileRenameOld, NameEntryBuffer);
// check that rename actually worked
      if (TAP_Hdd_Exist (FileRenameOld))
        ShowFileErrorMsg ("File rename failed", TRUE);
      else
      {
// if we just renamed a file which is recording, update our copy of the recording file name, and set flag to tell
// UpdateClock not to change it back to what GetRecInfo says
          for (i = 0;  i <= 1; i++)
          {
           if (strcmp (FileRenameOld, FileListNameRec[i]) == 0)
           {
            strcpy (FileListNameRec[i], NameEntryBuffer);
            bFileListNameChangedRec[i] = TRUE;
// also for rename of recording file, set flags to save event info when recording stops
// this flag does not actually guarantee that we have renamed to the epg name. we could have called up
// the epg name then decided to rename to something else. however we will have to update the event info to
// the last selected epg event in any case.
            if (bFileListRenameToEpg)
            {
//                strcpy (RecEventNameFinal[i], RecEventName[i]);
//                strcpy (RecEventDescFinal[i], RecEventDesc[i]);
// use values saved in rename to epg code
                strcpy (RecEventNameFinal[i], RecEventNameFileList);
                strcpy (RecEventDescFinal[i], RecEventDescFileList);
                RecEventStartTimeFinal[i] = RecEventStartTimeFileList;
                RecEventEndTimeFinal[i] = RecEventEndTimeFileList;
                bRecEventInfoSaved[i] = TRUE;
/*
                sprintf (msg, "Slot %d final event name = ", i + 1);
                strcat (msg, RecEventNameFinal[i]);
                ShowFileErrorMsg (msg, FALSE);
*/
            }
           }
          }
// likewise, if we just renamed the file playing back (or last played), update my version of it.
          if (strcmp (FileRenameOld, FileListNamePlay) == 0)
            strcpy (FileListNamePlay, NameEntryBuffer);
//           HideNameEntry ();
          HideNameEntryHelp ();
          bNameEntryMode = FALSE;
          ScanCurrentFolder ();
          ListCurrentFolder (TRUE);
      } // end of rename worked
//      bNameEntryMode = FALSE;
     }  // end of new name does not exist
//     else
//      ShowFileErrorMsg ("File rename failed", TRUE);
    } // end file rename
   } // end name entry mode
   else
   {
    FileListEntry = FileListScreenArray[FileListIndex - FileListIndexTop];
    if (FileListEntry.attr == ATTR_FOLDER)
    {
     TAP_Hdd_ChangeDir (FileListEntry.name);
     FileListPathLevel += 1;
     if (FileListPathLevel < FileListMaxPathLevels)
     {
      strcpy (FileListPath[FileListPathLevel], FileListEntry.name);
     }
     bFileListRefreshDuration = TRUE;
     ListNewFolder ();
    }
    else
    {
// display content of file in hex and ascii
     DisplayNewFile ();
    }
   }
   return 0;
  }
  case RKEY_Info:
  {
// toggle additional info for selected file
//   if (!bFileInfoShown)
   {
// display cluster info for selected file in message line
    FileListEntry = FileListScreenArray[FileListIndex - FileListIndexTop];
         
    sprintf (msg, "Total Clusters: %d, Unused Bytes: %d", FileListEntry.totalCluster, FileListEntry.unusedByte);
    ShowFileErrorMsg (msg, TRUE);

// if ts file, display event info text in file display area
    if (FileListEntry.attr == ATTR_TS)
    {

// display file name in header
     TAP_Osd_FillBox (rgnFile, 0, 0, FileWinW, FileHeadH, COLOR_BackFileHead);
     sprintf (msg, "%s", FileListEntry.name);
     TAP_Osd_PutS(rgnFile, FileWinXMarL, 0, FileWinXMarR, msg, COLOR_ForeFileHead, COLOR_None, 0, FNT_Size_FileHead, 1, ALIGN_CENTER);
// clear file list area
     TAP_Osd_FillBox (rgnFile, 0, FileHeadH, FileWinW, FileWinH-FileHeadH-FileErrorH, COLOR_BackFileListNormal);

     bFileInfoShown = TRUE;
     bFileListShown = FALSE;

// if file is recording, get event information and display it in file display area.
     bFileIsRecording = FALSE;
     for (i = 0; i <= 1; i++)
     {
//      if ((FileListEntry.size == 0) && (strcmp (FileListEntry.name, FileListNameRec[i]) == 0))
      if (strcmp (FileListEntry.name, FileListNameRec[i]) == 0)
      {
//       if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]))
//       {
//        if ((RecInfo[i].recType == RECTYPE_Normal) || (RecInfo[i].recType == RECTYPE_Copy))
//
        if (bRecording[i])
        {
         svcnum = RecInfo[i].svcNum;
         evtInfo = TAP_GetCurrentEvent(RecInfo[i].svcType, svcnum);
         TAP_Channel_GetInfo (RecInfo[i].svcType,  svcnum, &chInfo);

/*
         strcpy (msg, evtInfo->eventName);
         strcat (msg, evtInfo->description);
         ShowFileEventText (msg);
         return 0;
*/
         bFileIsRecording = TRUE;
         recslot = i;
         rectuner = RecInfo[i].tuner + 1;
         break;
        }
//       }
      }

     }

// if it is the file currently playing, get info from header saved before play started
// 05may05 could probably get this from playinfo now, since this has event info in it.
     if (!bFileIsRecording)
     {
      if (strcmp (FileListEntry.name, FileListNamePlay) == 0)
      {
//      strcpy (FileDisplayBuffer, TsFileHeader);
       for (i = 0; i < sizeof FileDisplayBuffer; i++)
        FileDisplayBuffer[i] = TsFileHeader[i];
       FileDisplayPointer = 0; // so that ExitFileDisplay avoids closing file
      }
      else
      {
       FileDisplayPointer = TAP_Hdd_Fopen (FileListEntry.name);
       if ((FileListEntry.size == 0) || (FileDisplayPointer == 0))
       {
        strcpy (msg, "Unable to display TS file header information");
        TAP_Osd_PutS(rgnFile, 0, FileHeadH, FileWinW, msg, COLOR_Red, COLOR_White, 0, FNT_Size_FileDisplay, 0, ALIGN_CENTER);
        return 0;
       }
       else
//       TAP_Hdd_Fread (FileDisplayBuffer, FileDisplayBytesPerScreen, 1, FileDisplayPointer);
        TAP_Hdd_Fread (FileDisplayBuffer, sizeof FileDisplayBuffer, 1, FileDisplayPointer);
      }
     }

//      if (FileDisplayPointer != 0)
     {
// if we can't open the TS file, just don't display the duration
//      TAP_Hdd_Fread (FileDisplayBuffer, FileDisplayBytesPerScreen, 1, FileDisplayPointer);
// clear file display area for event text
      TAP_Osd_FillBox (rgnFile, 0, FileHeadH, FileWinW, FileWinH-FileHeadH-FileErrorH, COLOR_BackFileListNormal);
// background for first line (service name, etc.)
      TAP_Osd_FillBox (rgnFile, 0, FileHeadH, FileWinW, FileDisplayYincrement, COLOR_DarkGray);
      if (!bFileIsRecording)
      {
// service name
       for (i = 0; i <= 23; i++)
        msg[i] = FileDisplayBuffer[i+28];
       msg[24] = '\0';
      }
      else
       strcpy (msg, chInfo.chName);
      TAP_Osd_PutS(rgnFile, FileWinXMarL, FileHeadH, FileWinXMarR, msg, COLOR_White, COLOR_None, 0, FNT_Size_FileDisplay, 1, ALIGN_LEFT);
// service number
      if (!bFileIsRecording)
       i = FileDisplayBuffer[10] * 255 + FileDisplayBuffer[11]; 
      else
       i = svcnum;
      sprintf (msg, "Service no. %d", i);
      i = SvcNumToLcn (i);
      if (i > 0)
      {
       sprintf (str, " (LCN %d)", i);
       strcat (msg, str);
      }
//      TAP_Osd_PutS(rgnFile, FileWinXMarL, FileHeadH, FileWinXMarR, msg, COLOR_White, COLOR_None, 0, FNT_Size_FileDisplay, 0, ALIGN_RIGHT);
// looks like it always has 3 like tuner in chinfo structure
// exept if i manage to update it when recording stops
// tuner number (value in header is internal number + 1, i.e. the external number we want here.
      if (!bFileIsRecording)
          i = (FileDisplayBuffer[17] >> 4) & 0x3;
      else
          i = rectuner;
      if (i != 1 && i != 2)
          strcpy (str, " Tuner Unknown");
      else
          sprintf (str, " Tuner %d", i);
      strcat (msg, str);
      TAP_Osd_PutS(rgnFile, FileWinXMarL, FileHeadH, FileWinXMarR, msg, COLOR_White, COLOR_None, 0, FNT_Size_FileDisplay, 0, ALIGN_RIGHT);

// 18jun05 background for third line (event start and end times)
// don't forget to change showfileeventtext so it starts one line lower
      TAP_Osd_FillBox (rgnFile, 0, FileHeadH+FileDisplayYincrement, FileWinW, FileDisplayYincrement, COLOR_White);
// event name length and text
      if (!bFileIsRecording)
      {
// event start and end times from ts header
        starttime = 0;
        for (i = 0; i <= 3; i++)
            starttime = starttime | (FileDisplayBuffer[76 + i] << (8 * (3 - i)));
        endtime = 0;
        for (i = 0; i <= 3; i++)
            endtime = endtime | (FileDisplayBuffer[80 + i] << (8 * (3 - i)));
      }
      else
      {
// file is recording. if i have saved event info for it (e.g. on a rename), use this, else use current event info.
       if (bRecEventInfoSaved[recslot])
       {
        starttime = RecEventStartTimeFinal[recslot];
        endtime = RecEventEndTimeFinal[recslot];
       }
       else
       {
        starttime = evtInfo->startTime;
        endtime = evtInfo->endTime;
       }
      }
// convert event times to ACST (UTC + 09:30)
/*
      starttime = TimeAdjust (starttime, 570);
      endtime = TimeAdjust (endtime, 570);
*/
      mjd1 = starttime >> 16;
      hh1 = (starttime & 0xFF00) >> 8;
      mm1 = starttime & 0xFF;
      ss1 = 0;
      mjd2 = endtime >> 16;
      hh2 = (endtime & 0xFF00) >> 8;
      mm2 = endtime & 0xFF;
      ss2 = 0;
// 15aug05 event times returned for recording file are now local not utc, so don't adjust
      if (!bFileIsRecording)
      {
          TimeAdjust (&mjd1, &hh1, &mm1, &ss1, 570 * 60);
          TimeAdjust (&mjd2, &hh2, &mm2, &ss2, 570 * 60);
      }
//      if (TAP_ExtractMjd(starttime>>16, &year, &month, &day, &weekDay) != 0)
      if (TAP_ExtractMjd(mjd1, &year, &month, &day, &weekDay) != 0)
      {
          WeekdayToAlpha (weekDay, daystr);
//          sprintf (str, "%s %02d/%02d %02d:%02d - %02d:%02d", daystr, day, month, (starttime&0xff00)>>8, starttime&0xff, (endtime&0xff00)>>8, endtime&0xff);
          sprintf (str, "%s %02d/%02d %02d:%02d - %02d:%02d", daystr, day, month, hh1, mm1, hh2, mm2);
      }
      else
          strcpy (str, "Invalid Start Time");
      xoff = FileWinXMarR - TAP_Osd_GetW (str, 0, FNT_Size_FileDisplay);
      TAP_Osd_PutS(rgnFile, xoff, FileHeadH+FileDisplayYincrement, FileWinXMarR, str, COLOR_DarkGreen, COLOR_None, 0, FNT_Size_FileDisplay, 1, ALIGN_LEFT);

// background for second line (event name and start/end times)
//      TAP_Osd_FillBox (rgnFile, 0, FileHeadH+FileDisplayYincrement, FileWinW, FileDisplayYincrement, COLOR_White);
// event name length and text
      if (!bFileIsRecording)
      {
       len = FileDisplayBuffer[85];
       if (len == 0)
       {
        strcpy (msg, "No event name");
        color = COLOR_Red;
       }
       else
       {
        for (i = 0; i < len; i++)
         msg[i] = FileDisplayBuffer[i+87];
        msg[len] = '\0';
        color = COLOR_DarkGreen;
       }
      }
      else
      {
// file is recording. if i have saved event info for it (e.g. on a rename), use this, else use current event info.
       if (bRecEventInfoSaved[recslot])
        strcpy (msg, RecEventNameFinal[recslot]);
       else
        strcpy (msg, evtInfo->eventName);
       color = COLOR_DarkGreen;
      }
//      TAP_Osd_PutS(rgnFile, 0, FileHeadH+FileDisplayYincrement, FileWinW, msg, color, COLOR_None, 0, FNT_Size_FileDisplay, 1, ALIGN_CENTER);
      TAP_Osd_PutS(rgnFile, FileWinXMarL, FileHeadH+FileDisplayYincrement, xoff - 30, msg, color, COLOR_None, 0, FNT_Size_FileDisplay, 1, ALIGN_LEFT);

// doco says event text length is 129 bytes but it can actually be more than double this
      if (!bFileIsRecording)
      {
       for (i = len; i <= 511; i++)
        msg[i-len] = FileDisplayBuffer[i+87];
       msg[511-len] = '\0';
      }
      else
      {
// file is recording. if i have saved event info for it (e.g. on a rename), use this, else use current event info.
       if (bRecEventInfoSaved[recslot])
        strcpy (msg, RecEventDescFinal[recslot]);
       else
        strcpy (msg, evtInfo->description);
      }
      ShowFileEventText (msg);

// Fclose should be done when ExitFileDisplay called via exit key or info key toggle
//            TAP_Hdd_Fclose (FileDisplayPointer);
     }
/*
     else
     {
      strcpy (msg, "Unable to open file for displaying information");
      TAP_Osd_PutS(rgnFile, 0, FileHeadH, FileWinW, msg, COLOR_Red, COLOR_White, 0, FNT_Size_FileDisplay, 0, ALIGN_CENTER);
     }
//           bFileDisplayShown = TRUE;
     bFileInfoShown = TRUE;
     bFileListShown = FALSE;
*/
    } // end of ts file
   } // end of file info not shown
/* not possible, file list and file info are mutually exclusive
   else
    ExitFileDisplay ();
*/
   return 0;
  } // end of info key
  case RKEY_NewF1:
  {
// red key renames selected file, if not already in name entry mode
   if (!bNameEntryMode)
   {
       FileListEntry = FileListScreenArray[FileListIndex - FileListIndexTop];
// put file name into buffer for editing and display it
       rgnNameEntry = rgnFile;
       strcpy (FileRenameOld, FileListEntry.name);
       strcpy (NameEntryBuffer, FileListEntry.name);
       NameEntryBufferIndexMax = strlen (NameEntryBuffer) - 1;
       NameEntryBufferIndex = 0;
       bNameEntryMode = TRUE;
       bFolderCreationInProgress = FALSE;
       bNameEntryInsertMode = TRUE;
       NameEntryKeyPressCount = 0;
       bNameEntryKeyCheckMax = TRUE;
       NameEntryPrevKeyCode = 0;
       if (FileListEntry.attr == ATTR_FOLDER)
        strcpy (FileNameEntryPrefix, "Rename Folder: ");
       else
        strcpy (FileNameEntryPrefix, "Rename File: ");
       ShowNameEntry (NameEntryBuffer);
       ShowNameEntryPos ();
       ShowNameEntryHelp ();
       bRenameEpgNext = FALSE;  // toggle for now/next event name when renaming 
       bFileListRenameToEpg = FALSE;
   }
   return 0;
  } // end of red key
  case RKEY_F2:
  {
// green key creates new folder. prompt for name, if not already in name entry mode
   if (!bNameEntryMode)
   {
       rgnNameEntry = rgnFile;
       NameEntryBufferIndex = 0;
       bNameEntryMode = TRUE;
       bFolderCreationInProgress = TRUE;
       bNameEntryInsertMode = TRUE;
       NameEntryKeyPressCount = 0;
       bNameEntryKeyCheckMax = TRUE;
       NameEntryPrevKeyCode = 0;
       strcpy (FileNameEntryPrefix, "Create Folder: ");
       strcpy (NameEntryBuffer, "");
       ShowNameEntry (NameEntryBuffer);
       ShowNameEntryPos ();
       ShowNameEntryHelp ();
   }
   return 0;
  } // end of green key
  case RKEY_F3:
  {
   if (bNameEntryMode)
    return 0;
// yellow key copies file to specified folder
// open selected file as source, issue msg to prompt for dest folder, set flag for copy in progress
// (user must then position to desired destination folder and press yellow key again (or exit to abort copy))
   if (!bFileCopyInProgress)
   {
    FileListEntry = FileListScreenArray[FileListIndex - FileListIndexTop];
// ignore if a folder, since can't open it and not practical to copy all files in folder
// yellow key in this case may then be eligible for processing later
    if (FileListEntry.attr == ATTR_FOLDER)
     break;
    FileCopySourcePointer = TAP_Hdd_Fopen (FileListEntry.name);
    if (FileCopySourcePointer == 0)
     ShowFileErrorMsg ("Unable to open source file for copy", TRUE);
    else
    {
     strcpy (FileCopyName, FileListEntry.name); // save in case we clobber FileListEntry before completing copy
     FileCopyAttr = FileListEntry.attr; // save in case we clobber FileListEntry before completing copy
     ShowFileErrorMsg ("Position to desired destination folder, then press yellow key again", TRUE);
     bFileCopyInProgress = TRUE;
    }
   }
   else
   {
// copy source file to dest folder. if file name already exists, issue msg (user must manually delete first, for now)
    if (TAP_Hdd_Exist (FileCopyName))
    {
     strcpy (msg, FileCopyName);
     strcat (msg, " already exists in destination folder");
     ShowFileErrorMsg (msg, TRUE);
    }
    else
    {
     TAP_Hdd_Create (FileCopyName, FileCopyAttr);
     FileCopyDestPointer = TAP_Hdd_Fopen (FileCopyName);
     if (FileCopyDestPointer == 0)
      ShowFileErrorMsg ("Unable to open destination file for copy", TRUE);
     else
     {
// copy source to destination
// this won't work on files whose length needs more than a double word
/* doesn't seem to do anything
      FileCopyDestPointer->mjd = FileCopySourcePointer->mjd;
      FileCopyDestPointer->hour = FileCopySourcePointer->hour;
      FileCopyDestPointer->min = FileCopySourcePointer->min;
      FileCopyDestPointer->sec = FileCopySourcePointer->sec;
*/
      FileDisplayLength = TAP_Hdd_Flen (FileCopySourcePointer);
      DisplayFileSize (FileDisplayLength, str, strunit);
      BytesCopied = 0;
      while (TAP_Hdd_Ftell (FileCopySourcePointer) < FileDisplayLength)
      {
       TAP_Hdd_Fread (FileDisplayBuffer, sizeof FileDisplayBuffer, 1, FileCopySourcePointer);
       TAP_Hdd_Fwrite (FileDisplayBuffer, sizeof FileDisplayBuffer, 1, FileCopyDestPointer);
       BytesCopied += sizeof FileDisplayBuffer;
       DisplayFileSize (BytesCopied, str1, strunit1);
       strcpy (msg, "Copied ");
       strcat (msg, str1);
       strcat (msg, strunit1);
       strcat (msg, " of ");
       strcat (msg, str);
       strcat (msg, strunit);
       ShowFileErrorMsg (msg, TRUE);
      }
      TAP_Hdd_Fclose (FileCopyDestPointer);
      TAP_Hdd_Fclose (FileCopySourcePointer);
// refresh destination folder list
      ScanCurrentFolder ();
      bFileListRefreshDuration = TRUE;
      ListCurrentFolder (TRUE);
      bFileCopyInProgress = FALSE;
//      HideFileErrorMsg (); // clear file copy in progress msg
     }
    }
   } // end of file copy in progress
   return 0;
  } // end of yellow key
/* 18apr05. move this to subset of blue key below
  case RKEY_Check:
  {
// blue key displays window prompting for sort sequence
   ShowFileSortOptionsWindow ();
   return 0;
  } // end of blue key
*/
  case RKEY_Forward:
  case RKEY_Rewind:
  {
// quick and dirty toggle between level 1 folders DataFiles and ProgramFiles
   if (FileListPathLevel == 1)
   {
    if (strcmp (FileListPath[1], "DataFiles") != 0)
     strcpy (str, "DataFiles");
    else
     strcpy (str, "ProgramFiles");
    TAP_Hdd_ChangeDir ("..");
    TAP_Hdd_ChangeDir (str);
    strcpy (FileListPath[1], str);
    bFileListRefreshDuration = TRUE;
    ListNewFolder ();
//    return 0;
   }
// 05jun05 if not at path level 1, do nothing and ensure we don't drop through to the check key code,
// which would bring up the file sort options window
   return 0;
  }
// if renaming a file which is recording, blue key toggles between current and next EPG info of recording channel for new name

//  case RKEY_Sat:
  case RKEY_F4:
  {
   if (bNameEntryMode && !bFolderCreationInProgress)
   {
    FileListEntry = FileListScreenArray[FileListIndex - FileListIndexTop];
    for (i = 0;  i <= 1; i++)
    {
//     if (TAP_Hdd_GetRecInfo (i, &RecInfo[i]))
//     {
//      if (RecInfo[i].recType == RECTYPE_Normal)
      if (bRecording[i] && (RecInfo[i].recType != RECTYPE_Copy))
      {
// use my version of recording file name, because recinfo will be wrong if I renamed it.
       if (strcmp (FileListEntry.name, FileListNameRec[i]) == 0)
       {
/*
//         ClearNameEntryField ();
        evtInfo = TAP_GetCurrentEvent (RecInfo[i].svcType, RecInfo[i].svcNum);
//        memcpy (&evtInfo, TAP_GetCurrentEvent(RecInfo[i].svcType, RecInfo[i].svcNum), sizeof evtInfo);
// event name can be longer than name entry buffer, so make sure we don't overflow the latter
// put special character in front of event name to indicate file is renamed from epg event name
        NameEntryBuffer[0] = PrefixFileRenameEPG;
        for (j = 1; j <= NAME_ENTRY_BUFFER_SIZE - 5; j++)
        {
         if (evtInfo->eventName[j-1] == '\0')
          break;
         NameEntryBuffer[j] = evtInfo->eventName[j-1];
        }
//         strcpy (NameEntryBuffer[i], ".rec");
//         NameEntryBuffer[i+4] = '\0';
        NameEntryBuffer[j] = '\0';
*/
        if (bRenameEpgNext)
            NameEntryBuffer[0] = PrefixFileRenameEPGnext;
        else
            NameEntryBuffer[0] = PrefixFileRenameEPGnow;
//        GetEpgEventName (RecInfo[i].svcType, RecInfo[i].svcNum, bRenameEpgNext, NameEntryBuffer + 1);
        GetEpgEventName (i, bRenameEpgNext, NameEntryBuffer + 1);
        strcat (NameEntryBuffer, ".rec");
        bRenameEpgNext = !bRenameEpgNext;
        bFileListRenameToEpg = TRUE;
// save event name and description set by GetEpgEventName so we can use when we do the actual rename
// as they may get changed by idle event code in the meantime
// also event start and end times
        strcpy (RecEventNameFileList, RecEventName[i]);
        strcpy (RecEventDescFileList, RecEventDesc[i]);
        RecEventStartTimeFileList = RecEventStartTime[i];
        RecEventEndTimeFileList = RecEventEndTime[i];
        NameEntryBufferIndex = 0;
        ShowNameEntry (NameEntryBuffer);
        ShowNameEntryPos ();
        break;
       }
      }
//     }
    }   // end of loop for each rec slot
   }
   else
   {
// not renaming a file, blue key displays window prompting for sort sequence
    ShowFileSortOptionsWindow ();
//    return 0;
   }
   return 0;
  } // end of blue key

  } // end of switch on key value when file list shown
 } // end of file list shown

 else if (bFileDisplayShown)
// processing keys when file display shown
 {
  switch (dwParam1)
  {
  case RKEY_ChUp:
  {
// move display back one line
   if (FileDisplayStart >= FileDisplayBytesPerLine)
   {
// SEEK_CUR, etc. as described in documentation, don't seem to be defined anywhere, so guessing.

// 1 doesn't work for negative offset relative to current position
    TAP_Hdd_Fseek (FileDisplayPointer, FileDisplayStart - FileDisplayBytesPerLine, 0);       
    DisplayNextBlock ();
   }
   return 0;
  }
  case RKEY_ChDown:
  {
// move display forward one line
   if (FileDisplayStart + FileDisplayBytesPerLine < FileDisplayLength)
   {
    TAP_Hdd_Fseek (FileDisplayPointer, FileDisplayStart + FileDisplayBytesPerLine, 0);       
    DisplayNextBlock ();
   }
   return 0;
  }
  case RKEY_VolUp:
  {
// move forward to next screenful
//        if (FileDisplayStart + FileDisplayBytesPerScreen < FileDisplayLength)
   DisplayNextBlock ();
   return 0;
  }
  case RKEY_VolDown:
  {
// move back to previous screenful
//        currpos = TAP_Hdd_Ftell (FileDisplayPointer);
   if (FileDisplayStart > 0)
   {
    if (FileDisplayStart >= FileDisplayBytesPerScreen)
     seekpos = FileDisplayStart - FileDisplayBytesPerScreen;
    else
     seekpos = 0;
// SEEK_CUR, etc. as described in documentation, don't seem to be defined anywhere, so seeking relative to start
// (really need bigger than dword, though)
// 1 doesn't work for negative offset relative to current position
    TAP_Hdd_Fseek (FileDisplayPointer, seekpos, 0);       
    DisplayNextBlock ();
   }
   return 0;
  }
  } // end of switch on key
 } // end of file display shown

} // end of KeyHandlerFile

    else
//     #include "KeyHandlerOther.c"

// ViewMaster3 key handler for other modes
{
// numeric keys are used in teletext, so should not be intercepted here in this state
//     if (bPlaybackNumericActivated && bFilePlayingBack && !IsProgressBarShown () && (State != STATE_Ttx))
// 10jun05 should now work regardless of progress bar
     if (bPlaybackNumericActivated && bFilePlayingBack && (State != STATE_Ttx))
     {
// special handling of numeric key input while playing back
// interpret as number of minutes, not percentage, into file for positioning
// leading zero key must be returned to system for aspect ratio control
      if (dwParam1 == RKEY_0 && PlaybackNumericKeyCount == 0)
       return dwParam1;
/* 09aug05
      if (PercentKeyReturnCount > 0)
      {
       PercentKeyReturnCount++;
       if (PercentKeyReturnCount <= 3)
        return dwParam1;
       else
        PercentKeyReturnCount = 0;
      }
// return count 0 and keycount 3 means this is a dummy event generated by idle loop after entry timeout, so
// key string is already set up.
*/
//      if (PlaybackNumericKeyCount < 3)
//      {
       switch (dwParam1)
       {
        case RKEY_0: KeyChar = 0x30; break;
        case RKEY_1: KeyChar = 0x31; break;
        case RKEY_2: KeyChar = 0x32; break;
        case RKEY_3: KeyChar = 0x33; break;
        case RKEY_4: KeyChar = 0x34; break;
        case RKEY_5: KeyChar = 0x35; break;
        case RKEY_6: KeyChar = 0x36; break;
        case RKEY_7: KeyChar = 0x37; break;
        case RKEY_8: KeyChar = 0x38; break;
        case RKEY_9: KeyChar = 0x39; break;
        default:  goto PlaybackKeyNotNumeric;
       }
       if (PlaybackNumericKeyCount > 2)
        PlaybackNumericKeyCount = 0;
       if (PlaybackNumericKeyCount == 0)
       {
// 11jun05        TAP_ExitNormal ();
        if (!bPlaybackNumericJumpMode)
            CreatePlaybackNumericRegion ();
        strcpy (PlaybackNumericKeyString, "000");
       }
       else
       {
        for (i = 0; i <= 1; i++)
            PlaybackNumericKeyString[i] = PlaybackNumericKeyString[i + 1];
       }
// display numeric chars entered so far
//       PlaybackNumericKeyString[PlaybackNumericKeyCount] = KeyChar;
//       PlaybackNumericKeyString[PlaybackNumericKeyCount+1] = '\0';
//       i = 2 - PlaybackNumericKeyCount;
       PlaybackNumericKeyString[2] = KeyChar;
       str[0] = PlaybackNumericKeyString[0];
       str[1] = '\0';
       str1[0] = PlaybackNumericKeyString[1];
       str1[1] = PlaybackNumericKeyString[2];
       str1[2] = '\0';
       strcat (str, ":");
       strcat (str, str1);
//       i = TAP_Osd_GetW1926 (PlaybackNumericKeyString);
//       TAP_Osd_PutS (rgnNumKey, 70, 0, i + 70, PlaybackNumericKeyString, COLOR_Black, COLOR_OrangeYellow, 0, FNT_Size_1926, 0, ALIGN_CENTER);
       i = TAP_Osd_GetW1622 (str);
       TAP_Osd_PutS (rgnNumKey, 50, 0, i + 50, str, COLOR_Black, COLOR_OrangeYellow, 0, FNT_Size_1622, 0, ALIGN_CENTER);
       TickPlaybackNumericSaved = TAP_GetTick ();
       PlaybackNumericKeyCount++;
//      }
/*
      if (PlaybackNumericKeyCount == 3)
      {
// delay a bit so we can seen third digit entered
//       TAP_Delay (50);
       TAP_Delay (25);
       TerminatePlaybackNumericKey ();
       minutes = ((PlaybackNumericKeyString[0] - 0x30) * 100) + ((PlaybackNumericKeyString[1] - 0x30) * 10) + (PlaybackNumericKeyString[2] - 0x30);
       if (PlaybackJumpType == 0)
       {
// absolute jump, jump to block corresponding to entered minutes
// block rate is per 10 sec.
//       PositionPlayback (BlockRateNormalPlay * 6 * minutes);
//       block = BlockRateNormalPlay * minutes;
//       block = ((BlockRateNormalPlay * minutes) + 5) / 10;
// allow for apparent quick block count increment after positioning by positioning 3 secs ahead.
           block = ((BlockRateNormalPlay * (minutes * 60 - 3)) + 300) / 600;
           PositionPlayback (block);
       }
       else
       {
// relative jump
           if (PlaybackJumpType == 1)
            SkipPlayback (minutes * 60);
           else
            SkipPlayback (-minutes * 60);
           PlaybackJumpType = 0;
       }
      }
*/
//      else
      return 0;
// if we get a non-numeric key before the third digit is entered, it should terminate entry then be processed normally,
// except for exit key which is intercepted earlier to just terminate numeric key entry.
    PlaybackKeyNotNumeric:
     }
// following red key is used in states other then menu (e.g. channel list)
// so return it to system in some other states too

     if (dwParam1 == RKEY_NewF1)  // toggle PIP on/off
     {
      if (State == STATE_List || State == STATE_Ttx) return dwParam1;

      if (!bPIPshown) // turn on PIP window in previous position
      {
        if (iPOPshown != 0)
        {
// if in pop mode, don't want system to process red key, else it will stop subchannel
// just want to reposition pip in idle loop.
            iPOPshown = 0; // PIP and POP are mutually exclusive
            bPIPshown = TRUE;
            bStartSubChannel = TRUE;
// subchannel is already active, so just position window in idle
            bDontStartSub = TRUE;
            return 0;       
        }
        else
        {         
// 24jul05 disallow if dual recording and playback, since it may corrupt one of the recordings
// (e.g. by swapping it to the other recording channel).
            if (bPlaying && bRecording[0] && bRecording[1])
                ShowFileErrorMsg ("PIP disallowed (dual recording while playing back)", FALSE);
            else
            {
//      TAP_Osd_DrawRectangle(rgnScreen, position[0][positioncount]+BorderThickness, position[1][positioncount]+BorderThickness, pipw-2*BorderThickness, piph-2*BorderThickness, BorderThickness, BorderColor);

               bPIPshown = TRUE;
               bStartSubChannel = TRUE;
//               if (iPOPshown == 0)
//               {
               bDontStartSub = TRUE;  // just move PIP in idle, don't actually start subchannel, because system has done it.
               return RKEY_NewF1; // use system to start subchannel
//               }
/*
           else
           {
// if in pop mode, don't want system to process red key, else it will stop subchannel
// just want to reposition pip in idle loop.
            iPOPshown = 0; // PIP and POP are mutually exclusive
            return 0;       
           }
*/
            }
        
        }
      }
      else   // turn off PIP window
       ReturnToNormal();
      return 0;
     } // end if red key

// volume down and up keys now used for moving in progress bar, so ignore their POP/PIP functions
// if progress bar is shown
     if (!IsProgressBarShown ())
     {

//     if (dwParam1 == RKEY_Prev) // toggle POP (1/2, 1/2), (1/4, 3/4), off
// it looks like the march 30, 2005 firmware is returning state normal when the new record menu is up
// and the voldown key is needed to reduce the default recording duration, so i need another key for pop, 
// since i don't want to disallow pop just because recording is in progress.
// i reckon the green key is ok (it is also next to the red key used for pip so seems logical for pop)
// progress bar is not up, so just ensure we are not processing the bookmark key we generated for stop playback
// the green key is used in timer mode, etc. but that would have been processed before we get to this code.
//     if (dwParam1 == RKEY_VolDown) // toggle POP (1/2, 1/2), (1/4, 3/4), off
     if ((dwParam1 == RKEY_Bookmark) && !bFilePlaybackStopInProgress) // toggle POP (1/2, 1/2), (1/4, 3/4), off
// sub will be in lower left, main in upper right
     {
      if ((State == STATE_Normal) && !bFileMode && !bTimerMode)
      {
       if (iPOPshown == 0) // turn on POP at (1/2 sub, 1/2 main) size
       {
            if (bPIPshown)
            {
// don't return red key to system, else it stops subchannel
             bPIPshown = FALSE; // PIP and POP are mutually exclusive
             iPOPshown = 1;
             bStartSubChannel = TRUE;
// subchannel is already active, so just position window in idle
             bDontStartSub = TRUE;
             return 0;
            }
            else
            {
// 24jul05 disallow if dual recording and playback, since it may corrupt one of the recordings
// (e.g. by swapping it to the other recording channel).
                if (bPlaying && bRecording[0] && bRecording[1])
                    ShowFileErrorMsg ("POP disallowed (dual recording while playing back)", FALSE);
                else
                {
//        ClearPIPBorder();
                    iPOPshown = 1;
                    bStartSubChannel = TRUE;
//        bDontStartSub = FALSE;
//                    if (!bPIPshown)
                    return RKEY_NewF1; // use system to start subchannel
/*
            else
            {
// don't return red key to system, else it stops subchannel
             bPIPshown = FALSE; // PIP and POP are mutually exclusive
             return 0;
            }
*/
                }
            }
       }
       else
       {
        if (iPOPshown == 1) // change to (1/4 sub, 3/4 main) size
        {
         iPOPshown = 2;
         ScalePOP ();
        }
        else
         ReturnToNormal();
       }
       return 0;
      } // end state normal, etc.
     } // end if vol down key

     if (bPIPshown) // check for assigned keys if sub-switch is activated
     {

//      if (dwParam1 == RKEY_Next)
//      if (dwParam1 == RKEY_VolUp)
// 07aug05 use blue key to move pip to avoid conflict with increasing duration in record menu
// in later firmwares (which apparently returns state_normal)
      if (dwParam1 == RKEY_F4)
      {
//       if (State == STATE_Normal && !bRecordKeyGen && !bFileMode && !bTimerMode)
//       if (State == STATE_Normal && (RecordKeyCount == 0) && !bFileMode && !bTimerMode)
       if (State == STATE_Normal && !bFileMode && !bTimerMode)
       {
        positioncount=(positioncount+1)%4;
        ScalePIP ();
        return 0;
       }
      }
     
     } // end pip shown, etc

     } // end if progressbar not shown

     if (bPIPshown || (iPOPshown != 0)) // check for assigned keys if sub-switch is activated
     {
/* deactivate this as incompatible with new use of these keys for pip move and pop selection  
      if (dwParam1 == RKEY_VolDown) // switch channel down
      {
 
       TAP_Channel_Move (CHANNEL_Sub, -1);
       return 0;
      }
      if (dwParam1 == RKEY_VolUp)  // switch channel up
      {
       TAP_Channel_Move (CHANNEL_Sub, 1);
       return 0;
      }
*/
      if (dwParam1 == RKEY_Sat)
      {

       bSat_key_hit=TRUE;
       return RKEY_Sat;
      }
     } // end pip or pop shown, etc.
} // end of KeyHandlerOther

   } // end of bActivated

  } // end of not one of the options windows

 } // end of key event

 return dwParam1;

} // end of event handler
