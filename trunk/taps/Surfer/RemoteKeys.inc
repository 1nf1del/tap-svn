/************************************************************
			Part of the ukEPG project
	This is the include file for RemoteKeys.c

Name	: RemoteKeys.inc
Author	: Darkmatter
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.0 Darkmatter: 31-05-05	Inception date

	Last change:  USE   1 Aug 105   12:33 pm
**************************************************************/

#ifndef __UkRemoteKeys__
#define __UkRemoteKeys__

#define MAX_NUMBER_OF_KEYS 256

//----------
// external structure
//
typedef struct
{
	dword	keyCode;
	char 	*ButtonText;
} key_Struct;


typedef struct
{
	int		count;
	key_Struct	*keys[MAX_NUMBER_OF_KEYS];						// an array of pointers
} keyCodes_Struct;

typedef enum
{
	TF5800,
	TF5000
} TYPE_ModelType;

//----------
// external procedures
//
void CreateTF5800Keys( keyCodes_Struct* );
void CreateTF5000Keys( keyCodes_Struct* );


char *GetButtonText( keyCodes_Struct*, dword );
int GetNumberOfKeys( keyCodes_Struct* );

dword GetNextKey( keyCodes_Struct*, dword );
dword GetPreviousKey( keyCodes_Struct*, dword );

bool IsKeyValid( keyCodes_Struct*, dword );


void KeyDestory( keyCodes_Struct* );
#endif
