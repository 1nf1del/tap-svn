#define TM_FILENAME			"timemon.log"
#define TM_INFO				"Time Monitor v1.0\r\nPrevious: Time/Date, Lev, Qual, Timers    Current:  Time/Date, Lev, Qual, Timers\r\n"
#define TM_TIME_DIFFERENCE_IN_MINS	10
#define TM_FILE_BLOCK_SIZE		512
#define TM_FILE_MAX_LENGTH		(10 * TM_FILE_BLOCK_SIZE)
#define TM_EVENT_BUFFER_SIZE		128

enum
{
	TM_INITILAISE = 0,
	TM_SERVICE
};

