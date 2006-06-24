#ifndef _UTILS_KEYMAP
#define _UTILS_KEYMAP

//
// Structure for keeping linked lists of keymappings
//
#include "ini.h"
#include "Utils.h"

#ifdef  __cplusplus
extern "C" {
#endif

#define MAX_KEYMAPS 512
#define MAX_KEY 50

//#define MAX_INI 512
//#define MAX_STR_LEN 1024

typedef struct _keyValNode{
	int		_keyVal;
	struct	_keyValNode *  _next;
} KEYVALNODE;

typedef struct _keyData{
	char	_logKeyName[ MAX_KEY ];	/* logical keyname */
	int		_len;					/* number of key values */
	int		_next;					/* index of next entry - see getNextLogicalKeyVal */
	struct	_keyValNode *  _keyList;	/* keyvalue list */
} KEYDATA;

typedef struct _logKeyNode{
	KEYDATA _keydata;				/* key data */
	struct	_logKeyNode *  _next;	/* next logical key */
} LOGKEYNODE;

extern char* KEYNAMES[];
extern const int noOfKeys;

void freeKeyData(KEYDATA kd);
void freeKeyNode(LOGKEYNODE* le);
//void addKeyNode(LOGKEYNODE **, KEYDATA);
void addKeyNode(LOGKEYNODE **, char*);
int loadKeyMap(char*, char*);
int freeMap(int);
int getKey(int, char*);
int getKeyVal(char*);
char *getKeyName(int keyVal);
int getLogicalKeyVal(char*, int);
int getNextLogicalKeyVal(char*, int);
void dumpKeyVals();
void initKeyMap();

#ifdef  __cplusplus
}
#endif

#endif //_UTILS_KEYMAP

