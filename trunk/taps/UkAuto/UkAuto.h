struct schDataTag
{
	byte searchStatus;
	byte searchOptions;
	char searchTerm[132];
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

#define	SCH_USER_DATA_STATUS_DISABLED		0x00
#define	SCH_USER_DATA_STATUS_RECORD		0x01
#define	SCH_USER_DATA_STATUS_WATCH		0x02

#define	SCH_USER_DATA_OPTIONS_EVENTNAME		0x01
#define	SCH_USER_DATA_OPTIONS_DESCRIPTION	0x02
#define	SCH_USER_DATA_OPTIONS_EXT_INFO		0x04
#define	SCH_USER_DATA_OPTIONS_EXACT_MATCH	0x08
#define	SCH_USER_DATA_OPTIONS_ANY_CHANNEL	0x10

#define SCH_MAX_SEARCHES			150

#define SCH_TV					0
#define SCH_RADIO				1


#if DEBUG
#define SCH_MAIN_WAIT_TO_SEARCH_ALARM		10		// In seconds
#else
#define SCH_MAIN_WAIT_TO_SEARCH_ALARM		600		// In seconds
#endif


enum
{
	SCH_SERVICE_INITIALISE = 0,
	SCH_SERVICE_RESET_SEARCH,
	SCH_SERVICE_WAIT_TO_SEARCH,
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

