typedef struct
{
	byte  char00;                   //00    // top 2 bits - valid; lower 5 bits - running status
//  byte  char00a:2;                //00
//  byte  char00b:1;                //00
//  byte  running_status:5;         //00
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
} type_eventtable;

typedef struct
{
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
} type_eventtable_v1;

typedef struct
{
	word  short20;                  //20    //??? normally 0000
	word  short22;                  //22    //??? whats this? - a 'random' number, different for each entry
	word  short24;                  //24    //??? normally 0000
	word  short26;                  //26    //??? whats this? - a 'random' number, different for each entry
	byte  *event_name;              //28    //ptr to event name, event description is offset by event_name_length bytes
	byte  *extended_event_name;     //2c
	byte  char30;                   //30    //01 if extended present else 00
	byte  num_extended;             //31    //number of extended items
	word  extended_length;          //32    //extended length
	word  short34;                  //34    //0001 if extended present else 0000
	word  short36;                  //36    //0001 if extended present else 0000
	word  short38;                  //38    //mjd - date added ?   - utc time?
	word  short3a;                  //3a    //??? normally 0000
	word  short3c;                  //3c    //??? normally 0000
	word  short3e;                  //3e    //??? normally 0000
	byte  genre;                    //40    // genre
	byte  subgenre;                 //41    // subgenre
	byte  char76;                   //42    //???
	byte  char77;                   //43    //???
} type_eventtable_v2;

typedef struct
{
	byte  *language;                //20
	word  short20;                  //24    //??? normally 0000
	word  short22;                  //26    //??? whats this? - a 'random' number, different for each entry
	word  short24;                  //28    //??? normally 0000
	word  short26;                  //2a    //??? whats this? - a 'random' number, different for each entry
	byte  *event_name;              //2c    //ptr to event name, event description is offset by event_name_length bytes
	byte  *extended_event_name;     //30
	byte  char30;                   //34    //01 if extended present else 00
	byte  num_extended;             //35    //number of extended items
	word  extended_length;          //36    //extended length
	word  short34;                  //38    //0001 if extended present else 0000
	word  short36;                  //3a    //0001 if extended present else 0000
	word  short38;                  //3c    //mjd - date added ?   - utc time?
	word  short3a;                  //3e    //??? normally 0000
	word  short3c;                  //40    //??? normally 0000
	word  short3e;                  //42    //??? normally 0000
	byte  genre;                    //44    // genre
	byte  subgenre;                 //46    // subgenre
	byte  char76;                   //46    //???
	byte  char77;                   //47    //???
} type_eventtable_v3;

typedef struct
{
	word  short20;                  //20    //??? normally 0000
	word  short22;                  //22    //??? whats this? - a 'random' number, different for each entry
	word  short24;                  //24    //??? normally 0000
	word  short26;                  //26    //??? whats this? - a 'random' number, different for each entry
	byte  *event_name;              //28    //ptr to event name, event description is offset by event_name_length bytes
	byte  *extended_event_name;     //2c
	byte  char30;                   //30    //01 if extended present else 00
	byte  num_extended;             //31    //number of extended items
	word  extended_length;          //32    //extended length
	word  short34;                  //34    //0001 if extended present else 0000
	word  short36;                  //36    //0001 if extended present else 0000
	word  short38;                  //38    //mjd - date added ?   - utc time?
	word  short3a;                  //3a    //??? normally 0000
	word  short3c;                  //3c    //??? normally 0000
	word  short3e;                  //3e    //??? normally 0000
	word  short40;
	word  short42;
	byte  genre;                    //44    // genre
	byte  subgenre;                 //45    // subgenre
	word  short46;                  //46    //???
	word  short48;
	word  short4a;
	word  crid_length;              //4c
	word  short4e;
	byte* crid;
} type_eventtable_v4;


#if 0
// definitions from bdb
typedef struct{
    byte  char00;                   //00    //for 1288: top 2 bits = valid, next 3 bits = running status, next 3 = ???
//    byte  char00a:2;                //00
//    byte  running_status:3;         //00
//    byte  char00b:3;                //00
    byte  char01;                   //01
    byte  duration_hour;            //02
    byte  duration_min;             //03
    word  short04;                  //04    //??? normally 0000
    word  event_id;                 //06
    word  start_date;               //08    //mjd, utc time
    byte  start_hour;               //0a
    byte  start_min;                //0b
    word  end_date;                 //0c    //mjd, utc time
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
    word  short1e;                  //1e    //??? normally 0000    or 6500
    word  short20;                  //20    //??? normally 0000
    word  previous;                 //22    //previous, a link to the previous epg entry, by date/time, ffff terminates
    word  short24;                  //24    //??? normally 0000
    word  next;                     //26    //next, a link to the previous epg entry, by date/time, ffff terminates
    byte  *event_name;              //28    //ptr to event name, event description is offset by event_name_length bytes
//for >=1288, event fields reordered
    union {
        struct{
            byte  char2c;                   //2c    //01 if extended present else 00
            byte  num_extended;             //2d    //number of extended items
            word  extended_length;          //2e    //extended length
            byte  *extended_event_name;     //30
        }fw;
        struct{
            byte  *extended_event_name;     //2c
            byte  char30;                   //30    //01 if extended present else 00
            byte  num_extended;             //31    //number of extended items
            word  extended_length;          //32    //extended length
        }fw13;
    }event;
    word  short34;                  //34    //0001 if extended present else 0000
    word  short36;                  //36    //0001 if extended present else 0000
    word  date_added;               //38    //mjd, utc time - date added ?
    word  short3a;                  //3a    //??? normally 0000
    word  short3c;                  //3c    //??? normally 0000
    word  short3e;                  //3e    //??? normally 0000

//for >=1288, eventtable is 0x44 long, genre
//for >=1400, genre moves
    union {
        struct{
            byte  content_nibble1;          //40
            byte  content_nibble2;          //41
            word  short42;                  //42    //user_nibble1,2? - not filled in by fw?
        }fw13;
        struct{
            word  short40;                  //40                                            //0000
            word  short42;                  //42                                            //0000
        }fw14;
    }genre;

//for >=1400, eventtable is 0x64 long, crid
    byte  content_nibble1;          //44
    byte  content_nibble2;          //45

    byte  crid32_strlen;            //46    //crid32 strlen (series)
    byte  byte47;                   //47
    byte *crid32_str;               //48    //crid32 ptr string
    byte  crid31_strlen;            //4c    //crid31 strlen (repeat)
    byte  byte4d;                   //4d
    byte  byte4e;                   //4e
    byte  byte4f;                   //4f
    byte *crid31_str;               //50    //crid31 ptr string
    byte  crid33_strlen;            //54    //crid33 strlen (recomendation?)
    byte  byte55;                   //55
    byte  byte56;                   //56
    byte  byte57;                   //57
    byte *crid33_str;               //58    //crid33 ptr string

    dword  dword5c;                 //5c                                            //00000000
    dword  dword60;                 //60                                            //00000000
}type_eventtable; 

#endif
