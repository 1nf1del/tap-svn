typedef struct
{
	byte  char00;                   //00    // top 2 bits - valid; lower 5 bits - running status
//	byte  char00a:2;                //00
//	byte  char00b:1;                //00
//	byte  running_status:5;         //00
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
} type_eventtable_v1;

typedef struct
{
	byte  char00;                   //00    // top 2 bits - valid; lower 5 bits - running status
//	byte  char00a:2;                //00
//	byte  char00b:1;                //00
//	byte  running_status:5;         //00
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
	byte  *event_name;				//28    //ptr to event name, event description is offset by event_name_length bytes
	byte  *extended_event_name;		//2c
	byte  char30;					//30    //01 if extended present else 00
	byte  num_extended;				//31	//number of extended items
	word  extended_length;			//32    //extended length
	word  short34;                  //34    //0001 if extended present else 0000
	word  short36;                  //36    //0001 if extended present else 0000
	word  short38;                  //38    //mjd - date added ?   - utc time?
	word  short3a;                  //3a    //??? normally 0000
	word  short3c;                  //3c    //??? normally 0000
	word  short3e;                  //3e    //??? normally 0000
	byte  genre;					//74	// genre
	byte  subgenre;					//75	// subgenre
	byte  char76;					//76	//???
	byte  char77;					//77	//???
} type_eventtable_v2;
