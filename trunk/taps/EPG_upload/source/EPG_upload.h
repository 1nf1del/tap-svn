// EPG Record Structure
typedef struct _EPG_INFO
{
	char    toppysatName[ MAX_SatName ];
	word service_id;
	word transport_stream_id;
	word original_network_id;
	char service_name[256];
	int topfieldinternalchannel;
	int	year;
	int	month;
	int day;
	int hour;
	int min;
	int offsetminutes;
	int	duration;
	char title[257];
	char shortevt[257];
	char description[4000];
	int parental;
	char recordingStatus[20];
	word eventid;
} PRIVATE_EPG_INFO;

/*
typedef structdid
{
	word service_id;
	word transport_stream_id;
	word original_network_id;
} TYPE_ServiceLookup;
*/

/*
char	*gservice_name[30] ;//__attribute__ ((section(".xdata"))); 
char	*gtoppyservice_name[30];
word	gservice_id[30] ;
word	gtransport_stream_id[30] ;
word	goriginal_network_id[30] ;
*/

typedef struct {
char    toppysatName[ MAX_SatName ];
int		topfieldinternalchannel;
char	tgdservice_name[128];
//char	toppyservice_name[30];
word	service_id;
word	transport_stream_id;
word	original_network_id;
int		LCN_channel_id;
} TYPE_IniSvcDetails ;



typedef struct {
  byte tuner;    /*0x00 Tuner 1, 0x01 Tuner2, 0x03 Auto Tuner*/
  byte xxx0 :4; /*Unknown always equals 0x4*/ 
  byte program_type :4;  /* 0x2 = programmed event, 0x0 = EPG "R" event */
  byte xxx1; /* Unknown, always equals 0x00*/
  byte broadcast_type :4; /* 0x0 = tvtuner, 0x8 = radio */
  byte recurance :4; /* 0x0 = Once, 0x1 = Daily, 0x2 = Weekend, 0x3 = Weekly, 0x4 = Weekday */
  word duration; /* Duration in minutes */
  word channel; /* Topfield internal channel number */
  dword timer_start;
  dword timer_stop;
  byte  recording; /*0x00 record off, 0x01 record*/
  byte  nameFix; /* Unknown sometimes= 0x01, is probably the name recording from EIT flag (where 00=get name)*/
  word serviceId; /* Timer serviceId details */
  char filename[88]; /* Filename to record to   (should be 97, but truncating by 8 for my backup)*/
  dword timer_startBackup; //added for this tap
  word durationBackup;  //added for this tap
  word BackupSignature; //==0xDEAF if extended
  word epgRt; /*epg rt marker, equals 0x0002 if marked*/
  word xxx2; /*always equals 0x0000*/
  word rf_channel; /* RF Channel Number appears to be BCD for these 2 bytes*/
  word xxx3; /* Unknown = 0x0700 for all recordings*/
  dword frequency; /* div by 1000 to get Freq in Mhz */
  word transportstreamId; /*transport stream Id= often 1st svcid for that particular RF channel*/
  word originalnetworkIdH; //DWORD must be on a DWORD boundary!??! all the previous items are all ok.  the end result is this would write over the next memory location!
  word originalnetworkIdL; 
  word xxx4; //always=0x0000
} TYPE_TimerRAM ;


//satellite version
typedef struct {
	byte tuner; /*0x00 Tuner 1, 0x01 Tuner2, 0x03 Auto Tuner*/
	byte xxx0 :4; /*Unknown always equals 0x4*/ 
	byte program_type :4; /* 0x2 = programmed event, 0x0 = EPG "R" event */
	byte xxx1; /* Unknown, always equals 0x00 ===> Sometimes 0x05 on PVR*/ 
	byte broadcast_type :4; /* 0x0 = tvtuner, 0x8 = radio */
	byte recurance :4; /* 0x0 = Once, 0x1 = Daily, 0x2 = Weekend, 0x3 = Weekly, 0x4 = Weekday */
	word duration; /* Duration in minutes */
	word channel; /* Topfield internal channel number */
	dword timer_start;
	dword timer_stop;
	byte recording; /*0x00 record off, 0x01 record*/
	byte nameFix; /* Unknown sometimes= 0x01, is probably the name recording from EIT flag (where 00=get name)*/
	word serviceId; /* Timer serviceId details */
	char filename[88]; /* Filename to record to (should be 97, but truncating by 8 for my backup)*/
	dword timer_startBackup; //added for this tap
	word durationBackup; //added for this tap
	word BackupSignature; //==0xDEAF if extended
	byte epgRt; /*epg rt marker, equals 0x02 if marked*/
	byte xxx5;
	word xxx2; /*always equals 0x0000*/
	word rf_channel; /* RF Channel Number appears to be BCD for these 2 bytes*/
	word xxx3; /* Unknown = 0x0700 for all recordings*/
	dword frequency; /* div by 1000 to get Freq in Mhz */
	word bitrate; /* in kbps */
	word transportstreamId; /*transport stream Id= often 1st svcid for that particular RF channel*/
	word originalnetworkIdH; //DWORD must be on a DWORD boundary!??! all the previous items are all ok. 
	word originalnetworkIdL; //the end result is this would write over the next memory location!
} TYPE_TimerRAM_SAT ;





typedef struct 
{
	//{
	dword	tableId						:8;
	dword	section_syntax_indicator	:1;
	dword	reserved					:3;
	dword	sectionLength				:12;
	dword	service_id_h				:8;

	dword	service_id_l				:8;
	dword	reserved2					:2;
	dword	version_number				:5;
	dword	current_next_indicator		:1;
	dword	section_number				:8;
	dword	lastSectionNumber			:8;

	dword	transport_stream_id			:16;
	dword	original_network_id			:16;

	dword	segment_last_section_number:8;
	dword	last_table_id				:8;
	//for(i=0;){
	dword	event_id					:16;

	dword	start_time_mjd				:16;
	dword	start_time_hour				:8;
	dword	start_time_min				:8;


	dword	start_time_sec				:8;
	dword	duration					:24;

	word	runningStatus				:3;
	word	free_ca_mode				:1;
	word	descriptor_loop_length		:12;
	//for(i=0;{ descriptor()}
    //}
	//}
	//crc_32
} TYPE_Eit;


typedef struct 
{
	byte	tableId						:8;

	word	section_syntax_indicator	:1;
	word	reserved_future_use		    :1;
	word   reserved						:2;
	word	sectionLength				:12;

	dword	transport_stream_id			:16;
	dword	original_network_id			:16;
	
	dword	service_id					:16;
	dword	event_id					:16;
	
	byte	reserved_future_use2		:5;
	byte	runningStatus				:3;
} TYPE_Rst;


const	sdword	_mpegCrc32Tbl[256] =
{
	0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
		0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
		0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
		0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
		0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
		0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
		0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
		0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
		0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
		0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
		0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
		0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
		0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
		0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
		0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
		0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
		0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
		0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
		0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
		0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
		0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
		0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
		0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
		0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
		0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
		0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
		0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
		0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
		0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
		0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
		0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
		0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
		0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
		0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
		0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
		0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
		0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
		0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
		0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
		0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
		0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
		0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
		0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
		0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
		0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
		0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
		0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
		0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
		0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
		0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
		0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
		0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
		0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
		0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
		0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
		0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
		0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
		0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
		0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
		0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
		0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
		0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
		0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
		0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};




