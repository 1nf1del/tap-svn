#ifndef MOREKEYS_H
#define MOREKEYS_H

/* Pull in Topfield standard key definitions. */

#include <key.h>

/* local key defines (missing in KEY.H) */
#define RKEY_SubLeft	0x10041 
#define RKEY_SubRight 	0x10043 
#define RKEY_White 	0x1003e

#ifndef WIN32
#define RKEY_Red 	0x1003f
#else
#define RKEY_Red 	RKEY_F1
#endif

#define RKEY_Green  0x10024
#define RKEY_Yellow 0x10025
#define RKEY_Blue   0x10026
#define RKEY_Filelist 0x10042
#define RKEY_Sat 0x10040

#endif
