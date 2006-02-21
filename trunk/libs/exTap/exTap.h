#ifndef _exTAP_
#define _exTAP_

//----------------------------------------------------------------------------
//some firmware offsets
enum {
    VERSION_1204 = 0,
    VERSION_1205,
    VERSION_1209,
    VERSION_1225,
    NUM_VERSIONS
};

#define FW_TAP_TABLE_1204       0x8016fa98              //the tap table base address; 0x20 bytes per tap
#define FW_TAP_IDENT_1204       0x8016fc98              //the current tap id

#define FW_TAP_TABLE_1205       0x8016fc1c              //the tap table base address; 0x20 bytes per tap
#define FW_TAP_IDENT_1205       0x8016fe1c              //the current tap id

#define FW_TAP_TABLE_1209       0x8017078c              //the tap table base address; 0x20 bytes per tap
#define FW_TAP_IDENT_1209       0x8017098c              //the current tap id

#define FW_TAP_TABLE_1225       0x8017187c              //the tap table base address; 0x20 bytes per tap
#define FW_TAP_IDENT_1225       0x80171a7c              //the current tap id

//----------------------------------------------------------------------------
//tap.h extensions

#define pLoaderVersion  0xa3fffff8
#define pSysID          0xa3fffffa
extern int _appl_version;

#define ptrfix(p) (p + _tap_startAddr)
typedef void (*funcVoid)(void);
typedef int (*funcInt)(void);

//----------------------------------------------------------------------------
//local key defines (missing in key.h)
#define RKEY_TvSTB      0x10022       //was RKEY_TvSat
#define RKEY_Opt        0x10010

#define RKEY_List       0x10018       //shared with RKEY_TvRadio

#define RKEY_Right      0x10015       //shared with RKEY_VolDown
#define RKEY_Left       0x1000f       //shared with RKEY_VolUp
#define RKEY_Up         0x10012       //shared with RKEY_ChUp
#define RKEY_Down       0x1001d       //shared with RKEY_ChDown

#define RKEY_SubMove    0x10041
#define RKEY_FileList   0x10042       //or RKEY_PlayList
#define RKEY_Text       0x10043

#define RKEY_Red        0x1003f
#define RKEY_Green      0x10024
#define RKEY_Yellow     0x10025
#define RKEY_Blue       0x10026

#define RKEY_White      0x1003e
#define RKEY_SubSwap    0x10040

//unmapped key values
#define exRKEY_TvRadio  0x1000e       //0x10018 (RKEY_List)    remapped to 0x1000e (RKEY_Uhf)
#define exRKEY_Exit     0x10013       //0x10017 (RKEY_VolDown) remapped to 0x10013 (RKEY_AudioTrk)

#define exRKEY_VolUp    0x10020       //0x10015 (RKEY_Right)   remapped to 0x10020 (RKEY_Fav )
#define exRKEY_VolDown  0x10017       //0x1000f (RKEY_Left)    remapped to 0x10017 (RKEY_Exit)
#define exRKEY_ChUp     0x10021       //0x10012 (RKEY_Up)      remapped to 0x10021 (RKEY_Subt)
#define exRKEY_ChDown   0x10037       //0x1001d (RKEY_Down)    remapped to 0x10037 (RKEY_Teletext)

//----------------------------------------------------------------------------
enum
{
    oTAP_Hdd_unknown0          = 0x00,  //hdd related, called by debug functions 'rs', 'rc'     -- read sector?
    oTAP_Hdd_unknown1          = 0x01,  //hdd related, called by debug functions 'ws', 'wc'     -- write sector?
    oTAP_unknown2              = 0x02,  //writes to eeprom @ 0xa3ffffe0, 0xa3ffffe1
    oTAP_Hdd_unknown3          = 0x03,  //ata related, ??? format ???
    oTAP_Hdd_SetBookmark       = 0x04,
    oTAP_Hdd_GotoBookmark      = 0x05,
    oTAP_Hdd_ChangePlaybackPos = 0x06,
    oTAP_ControlEit            = 0x07,
    oTAP_SetBk                 = 0x08,
    oTAP_EPG_UpdateEvent       = 0x09,
    oTAP_EPG_DeleteEvent       = 0x0a,
    oTAP_EPG_GetExtInfo        = 0x0b,
    oTAP_Channel_IsStarted     = 0x0c,
    oTAP_Vfd_GetStatus         = 0x0d,
    oTAP_Vfd_Control           = 0x0e,
    oTAP_Vfd_SendData          = 0x0f,
    oTAP_Win_SetAvtice         = 0x10,
    oTAP_Win_SetDrawItemFunc   = 0x11,
    oTAP_SysOsdControl         = 0x12,
    oTAP_Hdd_Move              = 0x13,
    oTAP_Osd_unknown20         = 0x14,  //osd related, ???
};
extern void * (*TAP_GetSystemProc)( int );
extern dword (*TAP_Hdd_Move)(char *from_dir, char *to_dir, char *filename);

//----------------------------------------------------------------------------
//patch stuff

typedef struct {
    int   revision;
    dword address;
}type_firmware;

typedef struct  {
    dword revision;
    int count;
    struct{
        dword addr;
        byte  from[4];
        byte  to[4];
    } info[0x20];
}type_patch;

typedef struct  {
    dword revision;
    dword addr;
    int   length;
    byte  from[0x100];
    byte  to[0x100];
}type_patch2;

//----------------------------------------------------------------------------
//hook stuff

#define EVT_HOOK	0xf100
#define MAX_HOOKS   16

typedef struct  {
    dword revision;
    dword enable;               //0=disabled, 1=enabled
    dword addr;                 //firmware hook address
    int   length;               //length
    byte  from[0x100];          //firmware original data
}type_hook;

typedef struct  {
    dword zero;
    dword at;
    dword v0;
    dword v1;
    dword a0;
    dword a1;
    dword a2;
    dword a3;

    dword t0;
    dword t1;
    dword t2;
    dword t3;
    dword t4;
    dword t5;
    dword t6;
    dword t7;

    dword s0;
    dword s1;
    dword s2;
    dword s3;
    dword s4;
    dword s5;
    dword s6;
    dword s7;

    dword t8;
    dword t9;
    dword k0;
    dword k1;
    dword gp;
    dword sp;
    dword s8;
    dword ra;
}type_hook_registers;

//----------------------------------------------------------------------------
//event stuff

//extended to give 0x100 bytes for the name, description
typedef struct
{
	word	evtId;                  //0x00
	dword	startTime;              //0x04
	dword	endTime;                //0x08
	word	duration;               //0x0c
	byte	runningStatus;          //0x0e
	char	eventName[256];         //0x0f
	char	description[256];       //0x8f,10f
	byte	parentalRating;         //0x10f,20f

	byte	satIdx;                 //0x110,210
	word	svcId;                  //0x112,212
	word	orgNetId;               //0x114,214
	word	tsId;                   //0x116,216
} TYPE_exTapEvent;

typedef struct
{
	byte	playMode;		        //0x00    // Playback Mode, refer TYPE_PlayMode
	byte 	trickMode;		        //0x01    // Trick Mode, refer TYPE_TrickMode
	byte 	speed;					//0x02    // playback speed
	byte	svcType;				//0x03    // 0 = TV, 1 = Radio
	word	svcNum;                 //0x04
	word	duration;               //0x06
	dword	currentBlock;           //0x08
	dword	totalBlock;             //0x0c
	TYPE_File *file;                //0x10
	TYPE_exTapEvent	evtInfo;        //0x14

	byte	repeatStatus;           //0x14+0x118
	byte	reserved[3];            //0x14+0x118+3
} TYPE_exPlayInfo;

//this is the internal data structure used to store events
//it _always_ starts at address FW_EVENT_TABLE
//what are the unknown values ???
//how is the event_name, extended_event_name allocated ???
//note that the extended_event_name is a list of (num_extended) zero terminated (name,description) pairs
//with a total length of extended_length  The final pair is terminated with another 0 (i.e. 2 0's indicate end of list)
typedef struct{
    byte  char00;                   //00    // top 2 bits - valid; lower 5 bits - running status
//    byte  char00a:2;                //00
//    byte  char00b:1;                //00
//    byte  running_status:5;         //00
    byte  char01;                   //01
    byte  duration_hour;            //02
    byte  duration_min;             //03
    word  short04;                  //04    //??? normally 0000
    word  event_id;                 //06
    word  start_date;               //08    //mjd
    byte  start_hour;               //0a
    byte  start_min;                //0b
    word  end_date;                 //0c    //mjd
    byte  end_hour;                 //0e
    byte  end_min;                  //0f
    byte  event_name_length;        //10
    byte  parental_rating;          //11
    word  original_network_id;      //12
    word  transport_stream_id;      //14
    word  service_id;               //16
    word  short18;                  //18    //??? normally 0000
    word  sat_index;                //1a
    word  short1c;                  //1c    //??? normally 0000
    word  short1e;                  //1e    //??? normally 0000
    word  short20;                  //20    //??? normally 0000
    word  short22;                  //22    //??? whats this? - a 'random' number, different for each entry
    word  short24;                  //24    //??? normally 0000
    word  short26;                  //26    //??? whats this? - a 'random' number, different for each entry
    byte  *event_name;              //28    //ptr to event name, event description is offset by event_name_length bytes
    byte  char2c;                   //2c    //01 if extended present else 00
    byte  num_extended;             //2d    //number of extended items
    word  extended_length;          //2e    //extended length
    byte  *extended_event_name;     //30
    word  short34;                  //34    //0001 if extended present else 0000
    word  short36;                  //36    //0001 if extended present else 0000
    word  short38;                  //38    //mjd - date added ?   - utc time?
    word  short3a;                  //3a    //??? normally 0000
    word  short3c;                  //3c    //??? normally 0000
    word  short3e;                  //3e    //??? normally 0000
}type_eventtable;

//this is the internal table that holds pointers to the event name/description buffers
typedef struct {
    byte *short_event_buffer;
    dword short_event_blocks;
    dword s2;
    dword s3;
    byte *extended_event_buffer;
    dword extended_event_blocks;
    dword e2;
    dword e3;
} type_eventbuffer;

//----------------------------------------------------------------------------
typedef struct {
    word    mjd;
    byte    hour;
    byte    minute;
}type_taptime;
//----------------------------------------------------------------------------
extern int firmware_id;
extern dword fw_tap_ident;
extern dword fw_tap_table;
extern type_eventbuffer *fw_event_buffer;
extern type_eventtable *fw_event_table;
extern void *fw_event_delete;
extern void *fw_event_alloc;
extern dword *fw_mheg_entry;
extern void *fw_os_free;
extern type_taptime *fw_utc_time;
//----------------------------------------------------------------------------
//some functions
extern int exTAP_Init(void);
//----------------------------------------------------------------------------
extern dword exTAP_GetFirmwareAddress(type_firmware *firmware);
extern dword exTAP_CallFirmware(void *pFunction, dword p0, dword p1, dword p2, dword p3);
extern int   exTAP_ApplyPatch(type_patch *patch);
extern int   exTAP_RemovePatch(type_patch *patch);
extern int   exTAP_ApplyHook(type_hook *hook);
extern int   exTAP_RemoveHook(int id);
//----------------------------------------------------------------------------
extern TYPE_exTapEvent *exTAP_GetEvent(int chType, int chNum, int *eventNum);
extern TYPE_exTapEvent *exTAP_GetCurrentEvent(int chType, int chNum);
extern byte *exTAP_EPG_GetExtInfo(TYPE_exTapEvent *tapEvtInfo);
extern bool exTAP_EPG_DeleteEvent(int chType, int chNum, dword evtid);
extern bool exTAP_EPG_UpdateEvent(int chType, int chNum, dword evtid, dword runningStatus);
extern int  exTAP_EPG_DeleteAllEvents(void);
extern int  exTAP_EPG_GetNumEvents(void);
extern int exTAP_EPG_Resize(dword short_size, dword ext_size);
extern int exTAP_EPG_GetSize(int type);
//----------------------------------------------------------------------------
extern int exTAP_Key_Extend(int enable);
extern int exTAP_MHEG_Disable(int disable);      //0=mheg on, 1=mheg off
extern dword exTAP_MHEG_Status(void);            //returns mheg register group, 0=off, 3=idle, 5=semi-active, 4=active
extern dword exTAP_TAP_Load(char *filename);
extern int exTAP_Shutdown(void);
//----------------------------------------------------------------------------
extern int  exTAP_LED_GetStatus(void);
extern int  exTAP_LED_SetControl(int enable);
extern int  exTAP_LED_GetControl(void);
extern void exTAP_LED_SendData(byte mask, byte *pData);
extern int  exTAP_LED_Translate(const char *pIn, byte *pOut);
extern int  exTAP_LED_GetMap(char ascii);
extern int  exTAP_LED_SetMap(char ascii, byte sevenseg);
//----------------------------------------------------------------------------
extern void exTAP_GetUTCTime(word *mjd, byte *hour, byte *minute);
extern dword exTAP_UTCtoLocalTime(dword utc);
//----------------------------------------------------------------------------

#endif //_exTAP_
