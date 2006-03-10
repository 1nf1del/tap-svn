struct schDataTag
{
	byte searchStatus;
	byte searchOptions;
	char searchTerm[132];
	char searchFolder[132];
	dword searchStartTime;
	dword searchEndTime;
	byte searchTvRadio;
	word searchStartSvcNum;
	word searchEndSvcNum;
	dword searchStartPadding;
	dword searchEndPadding;
	word searchAttach;
	byte searchDay;
};

struct schMoveTag
{
	bool moveEnabled;
	char moveFileName[132];
	char moveFolder[132];
	dword moveStartTime;
	dword moveEndTime;
	byte moveFailedCount;
};

#define	SCH_USER_DATA_STATUS_DISABLED		0x00
#define	SCH_USER_DATA_STATUS_RECORD		0x01
#define	SCH_USER_DATA_STATUS_WATCH		0x02

#define	SCH_USER_DATA_OPTIONS_EVENTNAME		0x01
#define	SCH_USER_DATA_OPTIONS_DESCRIPTION	0x02
#define	SCH_USER_DATA_OPTIONS_EXT_INFO		0x04
#define	SCH_USER_DATA_OPTIONS_EXACT_MATCH	0x08
#define	SCH_USER_DATA_OPTIONS_ANY_CHANNEL	0x10

#define SCH_MAX_SEARCHES			150
#define SCH_MAX_MOVES				80

#define SCH_TV					0
#define SCH_RADIO				1

#define SEARCH_FILENAME		"UkAutoSearch.txt"
#define SEARCH_INFO		"UK Auto Scheduler Search List v0.2\r\n"

#define MOVE_FILENAME		"UkAutoMove.txt"
#define MOVE_INFO		"UK Auto Scheduler Move List v0.1\r\n"

#define REMOTE_FILENAME		"UkAutoRemote.txt"

enum
{
	SCH_SERVICE_INITIALISE = 0,
	SCH_SERVICE_RESET_SEARCH,
	SCH_SERVICE_WAIT_TO_SEARCH,
	SCH_SERVICE_CHECK_FOR_REMOTE_SEARCHES,
	SCH_SERVICE_UPDATE_SEARCH_LIST,
	SCH_SERVICE_BEGIN_SEARCH,
	SCH_SERVICE_INITIALISE_EPG_DATA,
	SCH_SERVICE_PERFORM_SEARCH,
	SCH_SERVICE_NEXT_USER_SEARCH,
	SCH_SERVICE_NEXT_CHANNEL,
	SCH_SERVICE_COMPLETE
};

enum
{
	SCH_ATTACH_POS_NONE = 0,
	SCH_ATTACH_POS_PREFIX,
	SCH_ATTACH_POS_APPEND
};

enum
{
	SCH_ATTACH_TYPE_DATE = 0,
	SCH_ATTACH_TYPE_NUMBER
};

enum
{
	SCH_CONFIG_SEARCH_PERIOD_TEN_MINS = 0,
	SCH_CONFIG_SEARCH_PERIOD_ONE_HOUR,
	SCH_CONFIG_SEARCH_PERIOD_SPECIFIED
};

enum
{
	SCH_MOVE_SERVICE_INITIALISE = 0,
	SCH_MOVE_SERVICE_WAIT_TO_CHECK,
	SCH_MOVE_SERVICE_CHECK_MOVE,
	SCH_MOVE_SERVICE_CHECK_ARCHIVE,
	SCH_MOVE_SERVICE_MOVE_NOT_SUCCESSFUL,
	SCH_MOVE_SERVICE_NEXT_MOVE,
	SCH_MOVE_SERVICE_DELETE_EXPIRED_MOVES,
	SCH_MOVE_SERVICE_COMPLETE,
	SCH_MOVE_LOG_ARCHIVE
};

