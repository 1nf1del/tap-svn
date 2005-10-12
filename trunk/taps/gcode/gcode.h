#ifndef __GCODE_H
#define __GCODE_H

//Number of gcode digits we support
#define GCODEDIGITS  6
#define GCODEVERSION "v1.6"
#define MAXCHANNELS 50
#define OPTIONSFILE "gcode.ini"
#define MAXFILENAME 50

//Stuff for OSD
#define X	100
#define Y	 80
#define W	520
#define H	500

//define reasonable start time in minutes
#define REASONABLE_START_TIME  1


//channel table - used to get the svcNum from the channel
typedef struct _channel {
	int		lcn;
	word	svcNum;
	char	chName[MAX_SvcName];
} CHANNEL;

//phases used in EventHandler
enum inputPhases {
	_noScreens,
	_gcodeShow,
	_gcodeDetails,
	_menuActive
};


char *GetChannelName(int ch);
int GetNextChannel(int ch);
void ShowGCodeDetails();
void SetTimerDetails();
void GoLeft();
void GoRight();
void HighlightItem();
void ExitTAP( void );

#endif

