/************************************************************
Part of the ukEPG project
This module defines different text strings for different languages

To add new texts for ukas 
  1. define new variable for new text string e.g. 
    static char * text_NewString = langDefaultString;

  2. add function call to loadTextsToVariables -function e.g. 
    text_NewString = findTranslationFromBuffer("text_NewString", "Default (english) value of the text_NewString", mode);

  3. Use your new variable where ever you like e.g. 
    PrintCenter( rgn, SCH_SHOW_DIVIDER_X1, 450, SCH_SHOW_DIVIDER_X6, text_NewString, MAIN_TEXT_COLOUR, 0, FNT_Size_1622 );

( 4. Add translation to Settings/UkAuto/UkAutoLanguage.txt -file e.g.
    text_NewString=Default (english) value of the text_NewString=My own text which is used instead of the default )

v0.1 jpuhakka:  28-12-07  Initial version of multi language support

**************************************************************/
extern bool GotoPath(char *);

#define DELIMIT_TAB		0x09
#define DELIMIT_NL		0x0A
#define DELIMIT_CR		0x0D

#define LANGUAGE_FILENAME "UkAutoLanguage.txt"
#define LANGUAGE_TMP_FILENAME "UkAutoLanguage.tmp"
#define ADD_YOUR_TRANSLATION_HERE "add your translation here\0"

#define ONLY_COUNT_REQUIRED_MEMORY_FOR_DEFAULT_TEXTS 1
#define LOAD_TEXTS_TO_VARIABLES 2

static char langDefaultString[] = "Uninitialized!";
static char langErrorString[] = "Memory error!";

static char * text_ChannelSelection = langDefaultString;
static char * text_ModelType = langDefaultString;
static char * text_TF5800_UK = langDefaultString;
static char * text_TF5000_International = langDefaultString;
static char * text_Keyboard = langDefaultString;
static char * text_English = langDefaultString;
static char * text_Finnish = langDefaultString;
static char * text_German = langDefaultString;
static char * text_ActivationKey = langDefaultString;
static char * text_PressNewKeyNow = langDefaultString;
static char * text_InvalidChoice = langDefaultString;
static char * text_PerformSearch = langDefaultString;
static char * text_Every10Minutes = langDefaultString;
static char * text_Every30Minutes = langDefaultString;
static char * text_OnceEveryHour = langDefaultString;
static char * text_OnceEveryDayAt = langDefaultString;
static char * text_EnterTime = langDefaultString;
static char * text_FirmwareCalls = langDefaultString;
static char * text_Disabled = langDefaultString;
static char * text_Enabled = langDefaultString;
static char * text_TRC = langDefaultString;
static char * text_SearchAhead = langDefaultString;
static char * text_Unlimited = langDefaultString;
static char * text_Days = langDefaultString;
static char * text_DateFormat = langDefaultString;
static char * text_DDMMYY_dot = langDefaultString;
static char * text_DDMMYYYY_dot = langDefaultString;
static char * text_YYMMDD_dot = langDefaultString;
static char * text_YYYYMMDD_dot = langDefaultString;
static char * text_DDMMYY = langDefaultString;
static char * text_DDMMYYYY = langDefaultString;
static char * text_YYMMDD = langDefaultString;
static char * text_YYYYMMDD = langDefaultString;
static char * text_DDMMYY_slash = langDefaultString;
static char * text_DDMMYYYY_slash = langDefaultString;
static char * text_YYMMDD_slash = langDefaultString;
static char * text_YYYYMMDD_slash = langDefaultString;
static char * text_DDMM_dot = langDefaultString;
static char * text_MMDD_dot = langDefaultString;
static char * text_DDMM = langDefaultString;
static char * text_MMDD = langDefaultString;
static char * text_DDMM_slash = langDefaultString;
static char * text_MMDD_slash = langDefaultString;
static char * text_TimeFormat = langDefaultString;
static char * text_HHMM_colon = langDefaultString;
static char * text_HHMM_dot = langDefaultString;
static char * text_HHMM = langDefaultString;
static char * text_ConflictHandling = langDefaultString;
static char * text_CombineTimers = langDefaultString;
static char * text_SeparateTimers = langDefaultString;
static char * text_SeparateTimersKeepEndPadding = langDefaultString;
static char * text_Configuration = langDefaultString;
static char * text_Cancel = langDefaultString;
static char * text_Save = langDefaultString;
static char * text_Delete = langDefaultString;
static char * text_Ins = langDefaultString;
static char * text_Ovr = langDefaultString;
static char * text_UpDown = langDefaultString;
static char * text_MoveHighlightUpDown = langDefaultString;
static char * text_LeftRight = langDefaultString;
static char * text_MoveHighlightLeftRight = langDefaultString;
static char * text_MoveTextCursorLeftRight = langDefaultString;
static char * text_OK = langDefaultString;
static char * text_EnterSelectedCharacter = langDefaultString;
static char * text_Stop = langDefaultString;
static char * text_ChangeInsertOverwriteMode = langDefaultString;
static char * text_Pause = langDefaultString;
static char * text_ChangeUpperLowerCaseMode = langDefaultString;
static char * text_White = langDefaultString;
static char * text_DeleteACharacter = langDefaultString;
static char * text_Play = langDefaultString;
static char * text_InsertASpace = langDefaultString;
static char * text_List = langDefaultString;
static char * text_ShiftCharacterSet = langDefaultString;
static char * text_Record = langDefaultString;
static char * text_SaveAndExit = langDefaultString;
static char * text_Exit = langDefaultString;
static char * text_DiscardAndExit = langDefaultString;
static char * text_Recall = langDefaultString;
static char * text_ReloadOriginal = langDefaultString;
static char * text_space_0 = langDefaultString;
static char * text_KeyboardHelp = langDefaultString;
static char * text_PressEXITorINFO = langDefaultString;
static char * text_toCloseThisHelpWindow = langDefaultString;
static char * text_Shift = langDefaultString;
static char * text_Del = langDefaultString;
static char * text_PressINFO = langDefaultString;
static char * text_forHelpScreen = langDefaultString;
static char * text_Credits = langDefaultString;
static char * text_PressEXITtoReturnToTheMenu = langDefaultString;
static char * text_DeleteConfirmation = langDefaultString;
static char * text_DeleteAllTheEpisodeInformation = langDefaultString;
static char * text_Yes = langDefaultString;
static char * text_No = langDefaultString;
static char * text_Menu = langDefaultString;
static char * text_Configure = langDefaultString;
static char * text_ChangeTheWayThisTAPworks = langDefaultString;
static char * text_StopThisTAP = langDefaultString;
static char * text_TerminateAndUnloadThisTAP = langDefaultString;
static char * text_DeleteEpisodeInformation = langDefaultString;
static char * text_DeleteAllEpisodeInformation = langDefaultString;
static char * text_ThanksUsIfYouLike = langDefaultString;
static char * text_NoKeysDefined = langDefaultString;
static char * text_Mute_button = langDefaultString;
static char * text_TV_RadioOrList_button = langDefaultString;
static char * text_UHF_button = langDefaultString;
static char * text_TV_Sat_button = langDefaultString;
static char * text_Sleep_button = langDefaultString;
static char * text_Opt_button = langDefaultString;
static char * text_Recall_button = langDefaultString;
static char * text_Info_button = langDefaultString;
static char * text_RightArrow_button = langDefaultString;
static char * text_LeftArrow_button = langDefaultString;
static char * text_UpArrow_button = langDefaultString;
static char * text_DownArrow_button = langDefaultString;
static char * text_Ok_button = langDefaultString;
static char * text_Menu_button = langDefaultString;
static char * text_Guide_button = langDefaultString;
static char * text_TV_Radio_button = langDefaultString;
static char * text_AudioTrack_button = langDefaultString;
static char * text_Subtitle_button = langDefaultString;
static char * text_Teletext_button = langDefaultString;
static char * text_Exit_button = langDefaultString;
static char * text_Fav_button = langDefaultString;
static char * text_FastForward_button = langDefaultString;
static char * text_Rewind_button = langDefaultString;
static char * text_Play_button = langDefaultString;
static char * text_Pause_button = langDefaultString;
static char * text_Record_button = langDefaultString;
static char * text_Stop_button = langDefaultString;
static char * text_SlowMotion_button = langDefaultString;
static char * text_Previous_button = langDefaultString;
static char * text_PIP_button = langDefaultString;
static char * text_FileList_button = langDefaultString;
static char * text_Next = langDefaultString;
static char * text_Text_button = langDefaultString;
static char * text_Red_button = langDefaultString;
static char * text_Green_button = langDefaultString;
static char * text_Yellow_button = langDefaultString;
static char * text_Blue_button = langDefaultString;
static char * text_PIPSwap_button = langDefaultString;
static char * text_Sat_button = langDefaultString;
static char * text_White_button = langDefaultString;
static char * text_AutoSchedule = langDefaultString;
static char * text_All_brackets = langDefaultString;
static char * text_RecordOnly_brackets = langDefaultString;
static char * text_WatchOnly_brackets = langDefaultString;
static char * text_SearchOnly_brackets = langDefaultString;
static char * text_Record_char = langDefaultString;
static char * text_RecordMinus_char = langDefaultString;
static char * text_Watch_char = langDefaultString;
static char * text_All = langDefaultString;
static char * text_NotAll = langDefaultString;
static char * text_Mon = langDefaultString;
static char * text_Tue = langDefaultString;
static char * text_Wed = langDefaultString;
static char * text_Thu = langDefaultString;
static char * text_Fri = langDefaultString;
static char * text_Sat = langDefaultString;
static char * text_Sun = langDefaultString;
static char * text_Jan = langDefaultString;
static char * text_Feb = langDefaultString;
static char * text_Mar = langDefaultString;
static char * text_Apr = langDefaultString;
static char * text_May = langDefaultString;
static char * text_Jun = langDefaultString;
static char * text_Jul = langDefaultString;
static char * text_Aug = langDefaultString;
static char * text_Sep = langDefaultString;
static char * text_Oct = langDefaultString;
static char * text_Nov = langDefaultString;
static char * text_Dec = langDefaultString;
static char * text_ScheduleDeleteConfirmation = langDefaultString;
static char * text_DoYouWantToDeleteThisSchedule = langDefaultString;
static char * text_Found = langDefaultString;
static char * text_NewSchedule = langDefaultString;
static char * text_NewSchedules = langDefaultString;
static char * text_PressOKToContinue = langDefaultString;
static char * text_SearchThis = langDefaultString;
static char * text_Search = langDefaultString;
static char * text_SearchAll = langDefaultString;
static char * text_Filter = langDefaultString;
static char * text_No_dot = langDefaultString;
static char * text_Status = langDefaultString;
static char * text_SearchTerm = langDefaultString;
static char * text_Channel = langDefaultString;
static char * text_Day = langDefaultString;
static char * text_EditSearch = langDefaultString;
static char * text_TimerFunction = langDefaultString;
static char * text_SearchOnly = langDefaultString;
static char * text_TimersWillNotBeAutomaticallySet = langDefaultString;
static char * text_RecordAll = langDefaultString;
static char * text_AutomaticallySetRecordTimersForAllProgrammes = langDefaultString;
static char * text_RecordNewIgnoreRepeats = langDefaultString;
static char * text_AndIgnoreRepeats = langDefaultString;
static char * text_Watch = langDefaultString;
static char * text_WatchAutomaticallySetWatchTimers = langDefaultString;
static char * text_Match2 = langDefaultString;
static char * text_Advanced = langDefaultString;
static char * text_Exact = langDefaultString;
static char * text_Partial = langDefaultString;
static char * text_Title = langDefaultString;
static char * text_Description = langDefaultString;
static char * text_Extended = langDefaultString;
static char * text_From = langDefaultString;
static char * text_To = langDefaultString;
static char * text_Only = langDefaultString;
static char * text_Any = langDefaultString;
static char * text_TV = langDefaultString;
static char * text_Radio = langDefaultString;
static char * text_Time = langDefaultString;
static char * text_Days2 = langDefaultString;
static char * text_SelectedDays = langDefaultString;
static char * text_Padding = langDefaultString;
static char * text_Start = langDefaultString;
static char * text_End = langDefaultString;
static char * text_Attach = langDefaultString;
static char * text_None = langDefaultString;
static char * text_Prefix = langDefaultString;
static char * text_Append = langDefaultString;
static char * text_S_hash = langDefaultString;
static char * text_Date = langDefaultString;
static char * text_Folder = langDefaultString;
static char * text_NotAvailable = langDefaultString;
static char * text_FirmwareCallsDisabled = langDefaultString;
static char * text_Keep = langDefaultString;
static char * text_Last = langDefaultString;
static char * text_Smallest = langDefaultString;
static char * text_Largest = langDefaultString;
static char * text_For = langDefaultString;
static char * text_Programme = langDefaultString;
static char * text_Programmes = langDefaultString;
static char * text_InvalidSearchStringLength = langDefaultString;
static char * text_InvalidAdvancedSearchString = langDefaultString;
static char * text_InvalidNumberOfDays = langDefaultString;
static char * text_InvalidMatchOptions = langDefaultString;
static char * text_InvalidChannelRange = langDefaultString;
static char * text_ExitWithoutSaving = langDefaultString;
static char * text_ScheduleHasChanged = langDefaultString;
static char * text_DoYouWantToExitWithoutSaving = langDefaultString;
static char * text_Warning = langDefaultString;
static char * text_DestinationFolderDoesNotExist = langDefaultString;
static char * text_WouldYouLikeToCreateTheFolder = langDefaultString;
static char * text_ProgrammeName = langDefaultString;
static char * text_SearchResults = langDefaultString;
static char * text_Time_brackets = langDefaultString;
static char * text_Channel_brackets = langDefaultString;
static char * text_Name_brackets = langDefaultString;
static char * text_hr = langDefaultString;
static char * text_hrs = langDefaultString;
static char * text_min = langDefaultString;
static char * text_mins = langDefaultString;
static char * text_SelectAll = langDefaultString;
static char * text_Sort = langDefaultString;
static char * text_Searching = langDefaultString;
static char * text_PressEXITtoCancelSearch = langDefaultString;
static char * text_Match = langDefaultString;
static char * text_Matches = langDefaultString;

static char * reservedMemoryPtrsForLanguage[512];
static int reservedMemoryPtrsForLanguageIndex=0;

static char *languageBuffer = NULL;
static int	languageFileLength = 0;
static char *languageTmpBuffer = NULL;
static int	languageTmpBufferLength = 0;
static int languageTmpBufferPtr = 0;
static int requiredMemForDefault = 0;
static TYPE_File	*languageFile = NULL;

char * findTranslationFromBuffer(char * text, char * defaultText, int mode);

int loadTextsToVariables(int mode)
{
  if ( mode == ONLY_COUNT_REQUIRED_MEMORY_FOR_DEFAULT_TEXTS)
    requiredMemForDefault = 0;

  text_ChannelSelection = findTranslationFromBuffer("text_ChannelSelection", /*default->*/"Channel Selection", mode);
  text_ModelType = findTranslationFromBuffer("text_ModelType", /*default->*/"Model Type", mode);
  text_TF5800_UK = findTranslationFromBuffer("text_TF5800_UK", /*default->*/"TF5800  (UK)", mode);
  text_TF5000_International = findTranslationFromBuffer("text_TF5000_International", /*default->*/"TF5000  (International)", mode);
  text_Keyboard = findTranslationFromBuffer("text_Keyboard", /*default->*/"Keyboard", mode);
  text_English = findTranslationFromBuffer("text_English", /*default->*/"English", mode);
  text_Finnish = findTranslationFromBuffer("text_Finnish", /*default->*/"Finnish", mode);
  text_German = findTranslationFromBuffer("text_German", /*default->*/"German", mode);
  text_ActivationKey = findTranslationFromBuffer("text_ActivationKey", /*default->*/"Activation Key", mode);
  text_PressNewKeyNow = findTranslationFromBuffer("text_PressNewKeyNow", /*default->*/"Press new key NOW !", mode);
  text_InvalidChoice = findTranslationFromBuffer("text_InvalidChoice", /*default->*/"Invalid choice", mode);
  text_PerformSearch = findTranslationFromBuffer("text_PerformSearch", /*default->*/"Perform Search", mode);
  text_Every10Minutes = findTranslationFromBuffer("text_Every10Minutes", /*default->*/"Every 10 Minutes", mode);
  text_Every30Minutes = findTranslationFromBuffer("text_Every30Minutes", /*default->*/"Every 30 Minutes", mode);
  text_OnceEveryHour = findTranslationFromBuffer("text_OnceEveryHour", /*default->*/"Once Every Hour", mode);
  text_OnceEveryDayAt = findTranslationFromBuffer("text_OnceEveryDayAt", /*default->*/"Once Every Day at ", mode);
  text_EnterTime = findTranslationFromBuffer("text_EnterTime", /*default->*/"Enter Time - ", mode);
  text_FirmwareCalls = findTranslationFromBuffer("text_FirmwareCalls", /*default->*/"Firmware Calls", mode);
  text_Disabled = findTranslationFromBuffer("text_Disabled", /*default->*/"Disabled", mode);
  text_Enabled = findTranslationFromBuffer("text_Enabled", /*default->*/"Enabled", mode);
  text_TRC = findTranslationFromBuffer("text_TRC", /*default->*/"TRC", mode);
  text_SearchAhead = findTranslationFromBuffer("text_SearchAhead", /*default->*/"Search Ahead", mode);
  text_Unlimited = findTranslationFromBuffer("text_Unlimited", /*default->*/"Unlimited", mode);
  text_Days = findTranslationFromBuffer("text_Days", /*default->*/"Days", mode);
  text_DateFormat = findTranslationFromBuffer("text_DateFormat", /*default->*/"Date Format", mode);
  text_DDMMYY_dot = findTranslationFromBuffer("text_DDMMYY_dot", /*default->*/"DD.MM.YY", mode);
  text_DDMMYYYY_dot = findTranslationFromBuffer("text_DDMMYYYY_dot", /*default->*/"DD.MM.YYYY", mode);
  text_YYMMDD_dot = findTranslationFromBuffer("text_YYMMDD_dot", /*default->*/"YY.MM.DD", mode);
  text_YYYYMMDD_dot = findTranslationFromBuffer("text_YYYYMMDD_dot", /*default->*/"YYYY.MM.DD", mode);
  text_DDMMYY = findTranslationFromBuffer("text_DDMMYY", /*default->*/"DDMMYY", mode);
  text_DDMMYYYY = findTranslationFromBuffer("text_DDMMYYYY", /*default->*/"DDMMYYYY", mode);
  text_YYMMDD = findTranslationFromBuffer("text_YYMMDD", /*default->*/"YYMMDD", mode);
  text_YYYYMMDD = findTranslationFromBuffer("text_YYYYMMDD", /*default->*/"YYYYMMDD", mode);
  text_DDMMYY_slash = findTranslationFromBuffer("text_DDMMYY_slash", /*default->*/"DD/MM/YY", mode);
  text_DDMMYYYY_slash = findTranslationFromBuffer("text_DDMMYYYY_slash", /*default->*/"DD/MM/YYYY", mode);
  text_YYMMDD_slash = findTranslationFromBuffer("text_YYMMDD_slash", /*default->*/"YY/MM/DD", mode);
  text_YYYYMMDD_slash = findTranslationFromBuffer("text_YYYYMMDD_slash", /*default->*/"YYYY/MM/DD", mode);
  text_DDMM_dot = findTranslationFromBuffer("text_DDMM_dot", /*default->*/"DD.MM", mode);
  text_MMDD_dot = findTranslationFromBuffer("text_MMDD_dot", /*default->*/"MM.DD", mode);
  text_DDMM = findTranslationFromBuffer("text_DDMM", /*default->*/"DDMM", mode);
  text_MMDD = findTranslationFromBuffer("text_MMDD", /*default->*/"MMDD", mode);
  text_DDMM_slash = findTranslationFromBuffer("text_DDMM_slash", /*default->*/"DD/MM", mode);
  text_MMDD_slash = findTranslationFromBuffer("text_MMDD_slash", /*default->*/"MM/DD", mode);
  text_TimeFormat = findTranslationFromBuffer("text_TimeFormat", /*default->*/"Time Format", mode);
  text_HHMM_colon = findTranslationFromBuffer("text_HHMM_colon", /*default->*/"HH:MM", mode);
  text_HHMM_dot = findTranslationFromBuffer("text_HHMM_dot", /*default->*/"HH.MM", mode);
  text_HHMM = findTranslationFromBuffer("text_HHMM", /*default->*/"HHMM", mode);
  text_ConflictHandling = findTranslationFromBuffer("text_ConflictHandling", /*default->*/"Conflict Handling", mode);
  text_CombineTimers = findTranslationFromBuffer("text_CombineTimers", /*default->*/"Combine Timers", mode);
  text_SeparateTimers = findTranslationFromBuffer("text_SeparateTimers", /*default->*/"Separate Timers", mode);
  text_SeparateTimersKeepEndPadding = findTranslationFromBuffer("text_SeparateTimersKeepEndPadding", /*default->*/"Separate Timers - Keep End Padding", mode);
  text_Configuration = findTranslationFromBuffer("text_Configuration", /*default->*/"Configuration", mode);
  text_Cancel = findTranslationFromBuffer("text_Cancel", /*default->*/"Cancel", mode);
  text_Save = findTranslationFromBuffer("text_Save", /*default->*/"Save", mode);
  text_Delete = findTranslationFromBuffer("text_Delete", /*default->*/"Delete", mode);
  text_Ins = findTranslationFromBuffer("text_Ins", /*default->*/"[Ins]", mode);
  text_Ovr = findTranslationFromBuffer("text_Ovr", /*default->*/"[Ovr]", mode);
  text_UpDown = findTranslationFromBuffer("text_UpDown", /*default->*/"Up/Down :", mode);
  text_MoveHighlightUpDown = findTranslationFromBuffer("text_MoveHighlightUpDown", /*default->*/"Move highlight Up/Down.", mode);
  text_LeftRight = findTranslationFromBuffer("text_LeftRight", /*default->*/"Left/Right :", mode);
  text_MoveHighlightLeftRight = findTranslationFromBuffer("text_MoveHighlightLeftRight", /*default->*/"Move highlight Left/Right.", mode);
  text_MoveTextCursorLeftRight = findTranslationFromBuffer("text_MoveTextCursorLeftRight", /*default->*/"Move text cursor Left/Right.", mode);
  text_OK = findTranslationFromBuffer("text_OK", /*default->*/"OK :", mode);
  text_EnterSelectedCharacter = findTranslationFromBuffer("text_EnterSelectedCharacter", /*default->*/"Enter selected character.", mode);
  text_Stop = findTranslationFromBuffer("text_Stop", /*default->*/"Stop :", mode);
  text_ChangeInsertOverwriteMode = findTranslationFromBuffer("text_ChangeInsertOverwriteMode", /*default->*/"Change Insert/Overwrite mode.", mode);
  text_Pause = findTranslationFromBuffer("text_Pause", /*default->*/"Pause :", mode);
  text_ChangeUpperLowerCaseMode = findTranslationFromBuffer("text_ChangeUpperLowerCaseMode", /*default->*/"Change Upper/Lower Case mode.", mode);
  text_White = findTranslationFromBuffer("text_White", /*default->*/"White :", mode);
  text_DeleteACharacter = findTranslationFromBuffer("text_DeleteACharacter", /*default->*/"Delete a character.", mode);
  text_Play = findTranslationFromBuffer("text_Play", /*default->*/"Play :", mode);
  text_InsertASpace = findTranslationFromBuffer("text_InsertASpace", /*default->*/"Insert a space.", mode);
  text_List = findTranslationFromBuffer("text_List", /*default->*/"List :", mode);
  text_ShiftCharacterSet = findTranslationFromBuffer("text_ShiftCharacterSet", /*default->*/"Shift character set.", mode);
  text_Record = findTranslationFromBuffer("text_Record", /*default->*/"Record :", mode);
  text_SaveAndExit = findTranslationFromBuffer("text_SaveAndExit", /*default->*/"Save and exit.", mode);
  text_Exit = findTranslationFromBuffer("text_Exit", /*default->*/"Exit :", mode);
  text_DiscardAndExit = findTranslationFromBuffer("text_DiscardAndExit", /*default->*/"Discard and exit.", mode);
  text_Recall = findTranslationFromBuffer("text_Recall", /*default->*/"Recall :", mode);
  text_ReloadOriginal = findTranslationFromBuffer("text_ReloadOriginal", /*default->*/"Reload original.", mode);
  text_space_0 = findTranslationFromBuffer("text_space_0", /*default->*/"[space]0", mode);
  text_KeyboardHelp = findTranslationFromBuffer("text_KeyboardHelp", /*default->*/"Keyboard Help", mode);
  text_PressEXITorINFO = findTranslationFromBuffer("text_PressEXITorINFO", /*default->*/"(Press EXIT or INFO", mode);
  text_toCloseThisHelpWindow = findTranslationFromBuffer("text_toCloseThisHelpWindow", /*default->*/"to close this help window)", mode);
  text_Shift = findTranslationFromBuffer("text_Shift", /*default->*/"Shift", mode);
  text_Del = findTranslationFromBuffer("text_Del", /*default->*/"Del", mode);
  text_PressINFO = findTranslationFromBuffer("text_PressINFO", /*default->*/"(Press INFO", mode);
  text_forHelpScreen = findTranslationFromBuffer("text_forHelpScreen", /*default->*/"for Help Screen)", mode);
  text_Credits = findTranslationFromBuffer("text_Credits", /*default->*/"Credits", mode);
  text_PressEXITtoReturnToTheMenu = findTranslationFromBuffer("text_PressEXITtoReturnToTheMenu", /*default->*/"Press EXIT to return to the menu", mode);
  text_DeleteConfirmation = findTranslationFromBuffer("text_DeleteConfirmation", /*default->*/"Delete Confirmation", mode);
  text_DeleteAllTheEpisodeInformation = findTranslationFromBuffer("text_DeleteAllTheEpisodeInformation", /*default->*/"Delete all the episode information?", mode);
  text_Yes = findTranslationFromBuffer("text_Yes", /*default->*/"Yes", mode);
  text_No = findTranslationFromBuffer("text_No", /*default->*/"No", mode);
  text_Menu = findTranslationFromBuffer("text_Menu", /*default->*/"Menu", mode);
  text_Configure = findTranslationFromBuffer("text_Configure", /*default->*/"Configure", mode);
  text_ChangeTheWayThisTAPworks = findTranslationFromBuffer("text_ChangeTheWayThisTAPworks", /*default->*/"Change the way this TAP works", mode);
  text_StopThisTAP = findTranslationFromBuffer("text_StopThisTAP", /*default->*/"Stop this TAP", mode);
  text_TerminateAndUnloadThisTAP = findTranslationFromBuffer("text_TerminateAndUnloadThisTAP", /*default->*/"Terminate and unload this TAP", mode);
  text_DeleteEpisodeInformation = findTranslationFromBuffer("text_DeleteEpisodeInformation", /*default->*/"Delete Episode Information", mode);
  text_DeleteAllEpisodeInformation = findTranslationFromBuffer("text_DeleteAllEpisodeInformation", /*default->*/"Delete all episode information", mode);
  text_ThanksUsIfYouLike = findTranslationFromBuffer("text_ThanksUsIfYouLike", /*default->*/"Thanks us if you like", mode);
  text_NoKeysDefined = findTranslationFromBuffer("text_NoKeysDefined", /*default->*/"No Keys defined", mode);
  text_Mute_button = findTranslationFromBuffer("text_Mute_button", /*default->*/"Mute", mode);
  text_TV_RadioOrList_button = findTranslationFromBuffer("text_TV_RadioOrList_button", /*default->*/"TV/Radio or List", mode);
  text_UHF_button = findTranslationFromBuffer("text_UHF_button", /*default->*/"UHF", mode);
  text_TV_Sat_button = findTranslationFromBuffer("text_TV_Sat_button", /*default->*/"TV/Sat", mode);
  text_Sleep_button = findTranslationFromBuffer("text_Sleep_button", /*default->*/"Sleep", mode);
  text_Opt_button = findTranslationFromBuffer("text_Opt_button", /*default->*/"Opt", mode);
  text_Recall_button = findTranslationFromBuffer("text_Recall_button", /*default->*/"Recall", mode);
  text_Info_button = findTranslationFromBuffer("text_Info_button", /*default->*/"Info", mode);
  text_RightArrow_button = findTranslationFromBuffer("text_RightArrow_button", /*default->*/"Right Arrow (volume up)", mode);
  text_LeftArrow_button = findTranslationFromBuffer("text_LeftArrow_button", /*default->*/"Left Arrow (volume down)", mode);
  text_UpArrow_button = findTranslationFromBuffer("text_UpArrow_button", /*default->*/"Up Arrow (channel up)", mode);
  text_DownArrow_button = findTranslationFromBuffer("text_DownArrow_button", /*default->*/"Down Arrow (channel down)", mode);
  text_Ok_button = findTranslationFromBuffer("text_Ok_button", /*default->*/"Ok", mode);
  text_Menu_button = findTranslationFromBuffer("text_Menu_button", /*default->*/"Menu", mode);
  text_Guide_button = findTranslationFromBuffer("text_Guide_button", /*default->*/"Guide", mode);
  text_TV_Radio_button = findTranslationFromBuffer("text_TV_Radio_button", /*default->*/"TV/Radio", mode);
  text_AudioTrack_button = findTranslationFromBuffer("text_AudioTrack_button", /*default->*/"Audio track", mode);
  text_Subtitle_button = findTranslationFromBuffer("text_Subtitle_button", /*default->*/"Subtitle", mode);
  text_Teletext_button = findTranslationFromBuffer("text_Teletext_button", /*default->*/"Teletext", mode);
  text_Exit_button = findTranslationFromBuffer("text_Exit_button", /*default->*/"Exit", mode);
  text_Fav_button = findTranslationFromBuffer("text_Fav_button", /*default->*/"Fav", mode);
  text_FastForward_button = findTranslationFromBuffer("text_FastForward_button", /*default->*/"Fast Forward", mode);
  text_Rewind_button = findTranslationFromBuffer("text_Rewind_button", /*default->*/"Rewind", mode);
  text_Play_button = findTranslationFromBuffer("text_Play_button", /*default->*/"Play", mode);
  text_Pause_button = findTranslationFromBuffer("text_Pause_button", /*default->*/"Pause", mode);
  text_Record_button = findTranslationFromBuffer("text_Record_button", /*default->*/"Record", mode);
  text_Stop_button = findTranslationFromBuffer("text_Stop_button", /*default->*/"Stop", mode);
  text_SlowMotion_button = findTranslationFromBuffer("text_SlowMotion_button", /*default->*/"Slow Motion", mode);
  text_Previous_button = findTranslationFromBuffer("text_Previous_button", /*default->*/"Previous", mode);
  text_PIP_button = findTranslationFromBuffer("text_PIP_button", /*default->*/"PIP", mode);
  text_FileList_button = findTranslationFromBuffer("text_FileList_button", /*default->*/"File List", mode);
  text_Next = findTranslationFromBuffer("text_Next", /*default->*/"Next", mode);
  text_Text_button = findTranslationFromBuffer("text_Text_button", /*default->*/"Text", mode);
  text_Red_button = findTranslationFromBuffer("text_Red_button", /*default->*/"Red", mode);
  text_Green_button = findTranslationFromBuffer("text_Green_button", /*default->*/"Green", mode);
  text_Yellow_button = findTranslationFromBuffer("text_Yellow_button", /*default->*/"Yellow", mode);
  text_Blue_button = findTranslationFromBuffer("text_Blue_button", /*default->*/"Blue", mode);
  text_PIPSwap_button = findTranslationFromBuffer("text_PIPSwap_button", /*default->*/"PIP Swap", mode);
  text_Sat_button = findTranslationFromBuffer("text_Sat_button", /*default->*/"Sat", mode);
  text_White_button = findTranslationFromBuffer("text_White_button", /*default->*/"White", mode);
  text_AutoSchedule = findTranslationFromBuffer("text_AutoSchedule", /*default->*/"Auto Schedule", mode);
  text_All_brackets = findTranslationFromBuffer("text_All_brackets", /*default->*/" [All]", mode);
  text_RecordOnly_brackets = findTranslationFromBuffer("text_RecordOnly_brackets", /*default->*/" [Record Only]", mode);
  text_WatchOnly_brackets = findTranslationFromBuffer("text_WatchOnly_brackets", /*default->*/" [Watch Only]", mode);
  text_SearchOnly_brackets = findTranslationFromBuffer("text_SearchOnly_brackets", /*default->*/" [Search Only]", mode);
  text_Record_char = findTranslationFromBuffer("text_Record_char", /*default->*/"R", mode);
  text_RecordMinus_char = findTranslationFromBuffer("text_RecordMinus_char", /*default->*/"R-", mode);
  text_Watch_char = findTranslationFromBuffer("text_Watch_char", /*default->*/"W", mode);
  text_All = findTranslationFromBuffer("text_All", /*default->*/"All", mode);
  text_NotAll = findTranslationFromBuffer("text_NotAll", /*default->*/"Set", mode);
  text_Mon = findTranslationFromBuffer("text_Mon", /*default->*/" Mon", mode);
  text_Tue = findTranslationFromBuffer("text_Tue", /*default->*/" Tue", mode);
  text_Wed = findTranslationFromBuffer("text_Wed", /*default->*/" Wed", mode);
  text_Thu = findTranslationFromBuffer("text_Thu", /*default->*/" Thu", mode);
  text_Fri = findTranslationFromBuffer("text_Fri", /*default->*/" Fri", mode);
  text_Sat = findTranslationFromBuffer("text_Sat", /*default->*/" Sat", mode);
  text_Sun = findTranslationFromBuffer("text_Sun", /*default->*/" Sun", mode);
  text_Jan = findTranslationFromBuffer("text_Jan", /*default->*/"Jan", mode);
  text_Feb = findTranslationFromBuffer("text_Feb", /*default->*/"Feb", mode);
  text_Mar = findTranslationFromBuffer("text_Mar", /*default->*/"Mar", mode);
  text_Apr = findTranslationFromBuffer("text_Apr", /*default->*/"Apr", mode);
  text_May = findTranslationFromBuffer("text_May", /*default->*/"May", mode);
  text_Jun = findTranslationFromBuffer("text_Jun", /*default->*/"Jun", mode);
  text_Jul = findTranslationFromBuffer("text_Jul", /*default->*/"Jul", mode);
  text_Aug = findTranslationFromBuffer("text_Aug", /*default->*/"Aug", mode);
  text_Sep = findTranslationFromBuffer("text_Sep", /*default->*/"Sep", mode);
  text_Oct = findTranslationFromBuffer("text_Oct", /*default->*/"Oct", mode);
  text_Nov = findTranslationFromBuffer("text_Nov", /*default->*/"Nov", mode);
  text_Dec = findTranslationFromBuffer("text_Dec", /*default->*/"Dec", mode);
  text_ScheduleDeleteConfirmation = findTranslationFromBuffer("text_ScheduleDeleteConfirmation", /*default->*/"Schedule Delete Confirmation", mode);
  text_DoYouWantToDeleteThisSchedule = findTranslationFromBuffer("text_DoYouWantToDeleteThisSchedule", /*default->*/"Do you want to delete this schedule?", mode);
  text_Found = findTranslationFromBuffer("text_Found", /*default->*/"Found", mode);
  text_NewSchedule = findTranslationFromBuffer("text_NewSchedule", /*default->*/"New Schedule", mode);
  text_NewSchedules = findTranslationFromBuffer("text_NewSchedules", /*default->*/"New Schedules", mode);
  text_PressOKToContinue = findTranslationFromBuffer("text_PressOKToContinue", /*default->*/"Press OK to continue", mode);
  text_SearchThis = findTranslationFromBuffer("text_SearchThis", /*default->*/"Search", mode);
  text_Search = findTranslationFromBuffer("text_Search", /*default->*/"Search", mode);
  text_SearchAll = findTranslationFromBuffer("text_SearchAll", /*default->*/"Search All", mode);
  text_Filter = findTranslationFromBuffer("text_Filter", /*default->*/"Filter", mode);
  text_No_dot = findTranslationFromBuffer("text_No_dot", /*default->*/"No.", mode);
  text_Status = findTranslationFromBuffer("text_Status", /*default->*/"Set", mode);
  text_SearchTerm = findTranslationFromBuffer("text_SearchTerm", /*default->*/"Search Term", mode);
  text_Channel = findTranslationFromBuffer("text_Channel", /*default->*/"Channel", mode);
  text_Day = findTranslationFromBuffer("text_Day", /*default->*/"Day", mode);
  text_EditSearch = findTranslationFromBuffer("text_EditSearch", /*default->*/"Edit Search", mode);
  text_TimerFunction = findTranslationFromBuffer("text_TimerFunction", /*default->*/"Status", mode);
  text_SearchOnly = findTranslationFromBuffer("text_SearchOnly", /*default->*/"Search Only", mode);
  text_TimersWillNotBeAutomaticallySet = findTranslationFromBuffer("text_TimersWillNotBeAutomaticallySet", /*default->*/"Timers will not be automatically set.", mode);
  text_RecordAll = findTranslationFromBuffer("text_RecordAll", /*default->*/"Record All", mode);
  text_AutomaticallySetRecordTimersForAllProgrammes = findTranslationFromBuffer("text_AutomaticallySetRecordTimersForAllProgrammes", /*default->*/"Automatically set 'Record' timers for all programmes", mode);
  text_RecordNewIgnoreRepeats = findTranslationFromBuffer("text_RecordNewIgnoreRepeats", /*default->*/"Record New - Ignore Repeats", mode);
  text_AndIgnoreRepeats = findTranslationFromBuffer("text_AndIgnoreRepeats", /*default->*/"and 'ignore repeats'.", mode);
  text_Watch = findTranslationFromBuffer("text_Watch", /*default->*/"Watch", mode);
  text_WatchAutomaticallySetWatchTimers = findTranslationFromBuffer("text_WatchAutomaticallySetWatchTimers", /*default->*/"Watch: Automatically set 'Watch' timers.", mode);
  text_Match2 = findTranslationFromBuffer("text_Match2", /*default->*/"Match", mode);
  text_Advanced = findTranslationFromBuffer("text_Advanced", /*default->*/"Advanced", mode);
  text_Exact = findTranslationFromBuffer("text_Exact", /*default->*/"Exact", mode);
  text_Partial = findTranslationFromBuffer("text_Partial", /*default->*/"Partial", mode);
  text_Title = findTranslationFromBuffer("text_Title", /*default->*/"Title", mode);
  text_Description = findTranslationFromBuffer("text_Description", /*default->*/"Description", mode);
  text_Extended = findTranslationFromBuffer("text_Extended", /*default->*/"Extended", mode);
  text_From = findTranslationFromBuffer("text_From", /*default->*/"From", mode);
  text_To = findTranslationFromBuffer("text_To", /*default->*/"To", mode);
  text_Only = findTranslationFromBuffer("text_Only", /*default->*/"Only", mode);
  text_Any = findTranslationFromBuffer("text_Any", /*default->*/"Any", mode);
  text_TV = findTranslationFromBuffer("text_TV", /*default->*/"TV", mode);
  text_Radio = findTranslationFromBuffer("text_Radio", /*default->*/"Radio", mode);
  text_Time = findTranslationFromBuffer("text_Time", /*default->*/"Time", mode);
  text_Days2 = findTranslationFromBuffer("text_Days2", /*default->*/"Days", mode);
  text_SelectedDays = findTranslationFromBuffer("text_SelectedDays", /*default->*/"Selected Days:", mode);
  text_Padding = findTranslationFromBuffer("text_Padding", /*default->*/"Padding", mode);
  text_Start = findTranslationFromBuffer("text_Start", /*default->*/"Start", mode);
  text_End = findTranslationFromBuffer("text_End", /*default->*/"End", mode);
  text_Attach = findTranslationFromBuffer("text_Attach", /*default->*/"Attach", mode);
  text_None = findTranslationFromBuffer("text_None", /*default->*/"None", mode);
  text_Prefix = findTranslationFromBuffer("text_Prefix", /*default->*/"Prefix", mode);
  text_Append = findTranslationFromBuffer("text_Append", /*default->*/"Append", mode);
  text_S_hash = findTranslationFromBuffer("text_S_hash", /*default->*/"S#", mode);
  text_Date = findTranslationFromBuffer("text_Date", /*default->*/"Date", mode);
  text_Folder = findTranslationFromBuffer("text_Folder", /*default->*/"Folder", mode);
  text_NotAvailable = findTranslationFromBuffer("text_NotAvailable", /*default->*/"Not Available", mode);
  text_FirmwareCallsDisabled = findTranslationFromBuffer("text_FirmwareCallsDisabled", /*default->*/"Firmware Calls Disabled", mode);
  text_Keep = findTranslationFromBuffer("text_Keep", /*default->*/"Keep", mode);
  text_Last = findTranslationFromBuffer("text_Last", /*default->*/"Last", mode);
  text_Smallest = findTranslationFromBuffer("text_Smallest", /*default->*/"Smallest", mode);
  text_Largest = findTranslationFromBuffer("text_Largest", /*default->*/"Largest", mode);
  text_For = findTranslationFromBuffer("text_For", /*default->*/"For", mode);
  text_Programme = findTranslationFromBuffer("text_Programme", /*default->*/"Programme", mode);
  text_Programmes = findTranslationFromBuffer("text_Programmes", /*default->*/"Programmes", mode);
  text_InvalidSearchStringLength = findTranslationFromBuffer("text_InvalidSearchStringLength", /*default->*/"Invalid Search String Length!", mode);
  text_InvalidAdvancedSearchString = findTranslationFromBuffer("text_InvalidAdvancedSearchString", /*default->*/"Invalid Advanced Search String!", mode);
  text_InvalidNumberOfDays = findTranslationFromBuffer("text_InvalidNumberOfDays", /*default->*/"Invalid Number Of Days!", mode);
  text_InvalidMatchOptions = findTranslationFromBuffer("text_InvalidMatchOptions", /*default->*/"Invalid Match Options!", mode);
  text_InvalidChannelRange = findTranslationFromBuffer("text_InvalidChannelRange", /*default->*/"Invalid Channel Range!", mode);
  text_ExitWithoutSaving = findTranslationFromBuffer("text_ExitWithoutSaving", /*default->*/"Exit without saving", mode);
  text_ScheduleHasChanged = findTranslationFromBuffer("text_ScheduleHasChanged", /*default->*/"Schedule has changed", mode);
  text_DoYouWantToExitWithoutSaving = findTranslationFromBuffer("text_DoYouWantToExitWithoutSaving", /*default->*/"Do you want to exit without saving?", mode);
  text_Warning = findTranslationFromBuffer("text_Warning", /*default->*/"Warning!", mode);
  text_DestinationFolderDoesNotExist = findTranslationFromBuffer("text_DestinationFolderDoesNotExist", /*default->*/"Destination folder does not exist", mode);
  text_WouldYouLikeToCreateTheFolder = findTranslationFromBuffer("text_WouldYouLikeToCreateTheFolder", /*default->*/"Would you like to create the folder?", mode);
  text_ProgrammeName = findTranslationFromBuffer("text_ProgrammeName", /*default->*/"Programme Name", mode);
  text_SearchResults = findTranslationFromBuffer("text_SearchResults", /*default->*/"Search Results", mode);
  text_Time_brackets = findTranslationFromBuffer("text_Time_brackets", /*default->*/" [Time]", mode);
  text_Channel_brackets = findTranslationFromBuffer("text_Channel_brackets", /*default->*/" [Channel]", mode);
  text_Name_brackets = findTranslationFromBuffer("text_Name_brackets", /*default->*/" [Name]", mode);
  text_hr = findTranslationFromBuffer("text_hr", /*default->*/"hr", mode);
  text_hrs = findTranslationFromBuffer("text_hrs", /*default->*/"hrs", mode);
  text_min = findTranslationFromBuffer("text_min", /*default->*/"min", mode);
  text_mins = findTranslationFromBuffer("text_mins", /*default->*/"mins", mode);
  text_SelectAll = findTranslationFromBuffer("text_SelectAll", /*default->*/"Select All", mode);
  text_Sort = findTranslationFromBuffer("text_Sort", /*default->*/"Sort", mode);
  text_Searching = findTranslationFromBuffer("text_Searching", /*default->*/"Searching...", mode);
  text_PressEXITtoCancelSearch = findTranslationFromBuffer("text_PressEXITtoCancelSearch", /*default->*/"(Press EXIT to cancel search)", mode);
  text_Match = findTranslationFromBuffer("text_Match", /*default->*/"Match", mode);
  text_Matches = findTranslationFromBuffer("text_Matches", /*default->*/"Matches", mode);

  return requiredMemForDefault;
}
void freeAllReservedMemoryForLanguage(void)
{
  int counter=0;
  while ( reservedMemoryPtrsForLanguage[counter] != NULL )
  {
    TAP_MemFree( reservedMemoryPtrsForLanguage[counter] );
    reservedMemoryPtrsForLanguage[counter] = NULL;
    counter++;
  }
}

char * findTranslationFromBuffer(char * text, char * defaultText, int mode)
{
  int bufferIndex = 0;
  int	elementIndex1, elementIndex2, elementIndex3, i;
	char	strTemp1[256];
  char	strTemp2[256];
  char	strTemp3[256];
  char	strTemp4[256];
  int writeLen = 0;
  //char* strTemp4 = NULL;
  char * ret = NULL;
  memset( strTemp1, 0, 256 );
  memset( strTemp2, 0, 256 );
  memset( strTemp3, 0, 256 );
  memset( strTemp4, 0, 256 );

  if ( mode != ONLY_COUNT_REQUIRED_MEMORY_FOR_DEFAULT_TEXTS)
  {
    while ( bufferIndex < languageFileLength )
    {
      elementIndex1 = 0;
      elementIndex2 = 0;
      elementIndex3 = 0;

      // find variable name for the text (e.g. text_SomeVariableName)
	    while ( (languageBuffer[bufferIndex] != '=') // separated with '=' -character.
          && (languageBuffer[bufferIndex] != DELIMIT_CR) 
          && (languageBuffer[bufferIndex] != DELIMIT_NL) 
          &&	(languageBuffer[bufferIndex] != 0) )
        strTemp1[elementIndex1++] = languageBuffer[bufferIndex++];

      strTemp1[elementIndex1++] = '\0'; // add end of line

      bufferIndex++; //skip the '='

      // find default text
	    while ( (languageBuffer[bufferIndex] != '=') // separated with '=' -character.
          && (languageBuffer[bufferIndex] != DELIMIT_CR) 
          && (languageBuffer[bufferIndex] != DELIMIT_NL) 
          &&	(languageBuffer[bufferIndex] != 0) )
        strTemp2[elementIndex2++] = languageBuffer[bufferIndex++];
   
      strTemp2[elementIndex2++] = '\0'; // add end of line

      bufferIndex++; //skip the '='

      // find translation
      while ( (languageBuffer[bufferIndex] != DELIMIT_CR) 
          && (languageBuffer[bufferIndex] != DELIMIT_NL) 
          &&	(languageBuffer[bufferIndex] != 0) )
        strTemp3[elementIndex3++] = languageBuffer[bufferIndex++];

      strTemp3[elementIndex3++] = '\0'; // add end of line

      // go to end of line
      while ( (languageBuffer[bufferIndex] == DELIMIT_CR) 
          || (languageBuffer[bufferIndex] == DELIMIT_NL) 
          ||	(languageBuffer[bufferIndex] == 0) )
        bufferIndex++; //skip the DELIMIT_CR, DELIMIT_NL and 0

      // check if variable match
      if (strcmp(text,strTemp1) == 0)
      { // yes
        // is there translation?
        if (strcmp(strTemp3, ADD_YOUR_TRANSLATION_HERE) != 0)
        { // yes
          // is it 'empty'
          if ( strlen(strTemp3) > 0 )
          {// no so we can use it
            ret = (char *)TAP_MemAlloc( ( strlen(strTemp3) + 1 ) );
            if (ret != NULL)
              strcpy(ret, strTemp3);
          }
          else
          {
            ret = (char *)TAP_MemAlloc( 2 );
            if (ret != NULL)
              strcpy(ret, "-");
          }
        }
        break;
      }
    }

    // if ret is still NULL we will use default text
    if (ret == NULL)
    {
      ret = (char *)TAP_MemAlloc( ( strlen(defaultText) + 1 ) );
      if (ret != NULL)
      {
        strcpy(ret, defaultText);
        sprintf(strTemp3, ADD_YOUR_TRANSLATION_HERE);
      }
    }
    
    // to make sure that also changed or new texts will be "up to date" in file
    // lets update the file with this text row
    
    if ( languageTmpBuffer != NULL )
    {
      if (languageTmpBufferLength > languageTmpBufferPtr)
      {
        sprintf(strTemp4, "%s=%s=%s\r\n", text, defaultText, strTemp3);
        writeLen = strlen(strTemp4);
  	    for ( i=0; i<writeLen; i++)
	      {
		      languageTmpBuffer[languageTmpBufferPtr+i] = strTemp4[i];				// copy accross 1 char at a time
	      }
	      languageTmpBufferPtr += i;
      }
      else
      {
        sprintf(strTemp4, "%s=%s=%s\r\n", text, defaultText, strTemp3);
      }
    }

    // if we have reserved memory succesfully
    if (ret != NULL)
    {
      // save ptr for memory free purposes (for freeAllReservedMemoryForLanguage -function)
      reservedMemoryPtrsForLanguage[reservedMemoryPtrsForLanguageIndex++] = ret;
    }
    else
      ret = langErrorString;
  }
  else // ONLY_COUNT_REQUIRED_MEMORY_FOR_DEFAULT_TEXTS
  {
    sprintf(strTemp3, ADD_YOUR_TRANSLATION_HERE);
    sprintf(strTemp4, "%s=%s=%s\r\n", text, defaultText, strTemp3);
    requiredMemForDefault += strlen(strTemp4);
  }

  return ret;
}

int LoadLanguage(void)
{
  int updateFileRequired = 0;
  int comparisionCounter = 0;
  if(GotoPath(SETTINGS_FOLDER) == TRUE)
	{
		if ( TAP_Hdd_Exist( LANGUAGE_FILENAME ) )
			languageFile = TAP_Hdd_Fopen( LANGUAGE_FILENAME );
  }
	
	if ( languageFile != NULL )
  {
    languageFileLength = TAP_Hdd_Flen( languageFile );
    if (languageFileLength > 0)
    {
	    languageBuffer = (char *)TAP_MemAlloc( languageFileLength );
      if ( languageBuffer != NULL )
      {
        memset( languageBuffer, '\0', languageFileLength );
        // Read whole translation file to buffer
	      TAP_Hdd_Fread( languageBuffer, languageFileLength, 1, languageFile );
      }
    }
    TAP_Hdd_Fclose( languageFile );
  }
  languageTmpBufferLength = loadTextsToVariables(ONLY_COUNT_REQUIRED_MEMORY_FOR_DEFAULT_TEXTS); 
  languageTmpBuffer = (char *)TAP_MemAlloc( languageTmpBufferLength );
  if ( languageTmpBuffer != NULL )
    memset( languageTmpBuffer, '\0', languageTmpBufferLength );
  
  loadTextsToVariables(LOAD_TEXTS_TO_VARIABLES);

  if ( TAP_Hdd_Exist( LANGUAGE_TMP_FILENAME ) )
    TAP_Hdd_Delete( LANGUAGE_TMP_FILENAME );

  // update language file if it has been changed from previous one
  if ( languageBuffer == NULL )
    updateFileRequired = 1;
  else 
  {
    while ( comparisionCounter < languageTmpBufferPtr )
    {
      if (languageTmpBuffer[comparisionCounter] != languageBuffer[comparisionCounter])
      {
        updateFileRequired++;
        break;
      }
      comparisionCounter++;
    }
  }

  if (updateFileRequired)
  {
	    TAP_Hdd_Create( LANGUAGE_TMP_FILENAME, ATTR_NORMAL );
      if ( TAP_Hdd_Exist( LANGUAGE_TMP_FILENAME ) )
      {
        languageFile = TAP_Hdd_Fopen( LANGUAGE_TMP_FILENAME );
        TAP_Hdd_Fwrite( languageTmpBuffer, languageTmpBufferPtr, 1, languageFile );
        TAP_Hdd_Fclose( languageFile );
        TAP_Hdd_Delete( LANGUAGE_FILENAME );
        TAP_Hdd_Rename( LANGUAGE_TMP_FILENAME, LANGUAGE_FILENAME );
      }
  }
  if (languageBuffer != NULL) TAP_MemFree( languageBuffer );
  if (languageTmpBuffer != NULL)TAP_MemFree( languageTmpBuffer );

  return 1;
}