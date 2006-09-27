#include "tap.h"

#define ABS_ADDR(x) (0x80000000 | (((dword)(x) & 0x03ffffff) << 2))
#define BASE_MASK 0xffe00000
#define LUI_GP_CMD 0x3c1c0000
#define ADDIU_GP_CMD 0x279c0000
#define LW_GP_CMD 0x8f800000
#define CMD_MASK 0xfc000000
#define REG_MASK 0x03ff0000
#define JAL_CMD 0x0c000000
#define JR_CMD 0x03e00008
#define ADD_SP_MASK 0xffffff00
#define ADD_SP_CMD 0x27bdff00
#define LUI_CMD 0x3c000000
#define ADDIU_CMD 0x24000000
// The following two lines define the mask and the commands storing an
// API function address to its offset. This operation is expected to be unique
// for the StartTap() function. If a future firmware version contains another
// instruction with this signature it will be necessary to choose another
// The mask eliminates registers used in the instruction because they may vary.
#define API_OFFSET_MASK 0xfc00ffff
#define API_OFFSET_CMD 0xac0001d4
/*
The following is a typical fragment of the StartTap() function:

558r	80185db8:    25efb1d4	addiu	$t7,$t7,-20012=0xb1d4	# 8018b1d4 TAP_Osd_PutS
55cr	80185dbc:    25cef43c	addiu	$t6,$t6,-3012=0xf43c	# 8018f43c
560r	80185dc0:    2739b31c	addiu	$t9,$t9,-19684=0xb31c	# 8018b31c TAP_Osd_PutString
564r	80185dc4:    2718b458	addiu	$t8,$t8,-19368=0xb458	# 8018b458 TAP_Osd_DrawString
568r	80185dc8:    ac8f01cc	sw	$t7,460($a0)		
56cr	80185dcc:    ac8e01d0	sw	$t6,464($a0)		
570r	80185dd0:    ac9901d4	sw	$t9,468($a0)		<= this instruction seemed to be unique
574r	80185dd4:    ac9801d8	sw	$t8,472($a0)		
578r	80185dd8:    3c0f8019	lui	$t7,0x8019		
57cr	80185ddc:    3c0e8019	lui	$t6,0x8019		
580r	80185de0:    3c198018	lui	$t9,0x8018		
584r	80185de4:    3c188018	lui	$t8,0x8018		
*/

// function prototype
// The function takes full name of a TAP file which resides in the current
// working directory. The file is expected to be in the TAP format.
typedef int (*StartTapFn)(char* pFileName);

typedef void (*SetDirDesc)(TYPE_File *pFile);

//-----------------------------------------------------------------------------
// getCallAddress() retrieves the address of the n-th function call found
// after the specified address.
dword *getCallAddress(dword *pData, int count)
{
	int i = 0, j = 0;
	dword *pAddr = NULL;

	if(count > 0)
	{
		while(pData[i] != JR_CMD)
		{
			if((pData[i] & CMD_MASK) == JAL_CMD)
			{
				j++;
				if(j == count)
				{
					// return the absolute address encoded in the opcode
					pAddr = (dword*)ABS_ADDR(pData[i]);
					break;
				}
			}

			i++;
		}
	}

	return pAddr;
}
//-----------------------------------------------------------------------------
// findStartTapFunction() retrieves the address of the function staring TAPs.
dword findStartTapFunction(dword *pData, int size)
{
	dword rc = 0;
	int i = 0, start = 0;

	while(i < size)
	{
		if((pData[i] & ADD_SP_MASK) == ADD_SP_CMD)
		{
			// detected start of a function, reset variables
			start = i;
			rc = 0;
		}
		else
		{
			// search for the unique instruction
			if((pData[i] & API_OFFSET_MASK) == API_OFFSET_CMD)
			{
				// found the unique instruction, return its address
				return (dword)(pData + start);
			}

		}

		i++;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// getCwdDescOffset() retrieves the GP offset of the current working directory
// descriptor.
// pData must point to the firmware StartTap function.
short getCwdDescOffset(dword *pData)
{
	// the offset seems to be constant among various firmware versions
	int offset = -8;

	if((pData[offset] & BASE_MASK) == LW_GP_CMD)
	{
		return (short)(pData[offset] & 0xffff);
	}
	else
	{
		return 0;
	}
}

//-----------------------------------------------------------------------------
// getTapIndexAddr() retrieves the address of the TAP table index
dword getTapIndexAddr()
{
	dword *pData = (dword*)TAP_Exit;
	if(((pData[0] & CMD_MASK) == LUI_CMD) && ((pData[1] & CMD_MASK) == ADDIU_CMD))
	{
		return (((pData[0] << 16) & 0xffff0000) + (short)(pData[1] & 0xffff));
	}
	else
	{
		return 0;
	}
}

//-----------------------------------------------------------------------------
// getFwGp() retrieves the GP value of the current firmware
dword getFwGp()
{
	int i = 0;
	dword *pAddr = (dword*)0x80000000;
	dword gp = 0;

	while(pAddr[i] != JR_CMD)
	{
		if(((pAddr[i] & (CMD_MASK | REG_MASK)) == LUI_GP_CMD) &&
			((pAddr[i + 1] & (CMD_MASK | REG_MASK)) == ADDIU_GP_CMD))
		{
			// return the absolute address encoded in the opcode
			gp = (((pAddr[i] << 16) & 0xffff0000) + (short)(pAddr[i + 1] & 0xffff));
			break;
		}

		i++;
	}

	return gp;
}

// global variables initialized by initHook() at start-up
typedef struct
{
	TYPE_File *pFwCwd; // current FW working directory
	dword fwGp;        // FW value of GP
	dword *pTapIndex;  // TAP index address
	StartTapFn fwStartTap; // addres of the FW start TAP function
	SetDirDesc setDirDesc; // address of the function setting the current FW working directory
} HookData;

HookData hookData;

//-----------------------------------------------------------------------------
// initHook retrieves firmware data and initializates hook data
// Returns TRUE if succeeded.
bool InitStartTapHook()
{
	short offset;

	hookData.fwGp = getFwGp();
	hookData.pTapIndex = (dword*)getTapIndexAddr();

	// retrieve the address of the start TAP function, it is usually located in 
	// the second megabyte of the firmware
	hookData.fwStartTap = (StartTapFn)findStartTapFunction((dword*)0x80100000, 0x100000);

	if((hookData.fwStartTap == NULL) || (hookData.fwGp == 0) || (hookData.pTapIndex == NULL))
	{
		TAP_Print("initialization failed (%p, %p, %p)\n",
			hookData.fwStartTap, hookData.fwGp, hookData.pTapIndex);
		return FALSE;
	}

	// retieve the offset of the pointer to the current working directory descriptor
	offset = getCwdDescOffset((dword*)hookData.fwStartTap);

	if(offset == 0)
	{
		TAP_Print("Current working directory descriptor not found\n");
		return FALSE;
	}

	// compute and store the descriptor address
	hookData.pFwCwd = *(TYPE_File**)(hookData.fwGp + offset);

	// retrieve the function setting the descriptor of the current working directory
	// (it is the second call in the TAP_Hdd_Fopen())
	hookData.setDirDesc = (SetDirDesc)getCallAddress((dword*)TAP_Hdd_Fopen, 2);

	return TRUE;
}

//-----------------------------------------------------------------------------
// StartTap() starts the TAP specified by filename from the current TAP directory.
// Please change to the target directory prior to calling StartTap().
// Returns 0 if failed.
int TAP_Start(char *pFilename)
{
	// use volatile local variables for the firmware context to prevent
	// the compiler using invalid $gp values
	volatile dword tapGp = 0;
	volatile dword tapIndex = 0;
	volatile HookData *pHookData = &hookData;
	int rc;
	TYPE_File tapCwd;

	if((pHookData->pFwCwd == 0) || (pHookData->setDirDesc == NULL) || (pHookData->pTapIndex == NULL))
	{
		TAP_Print("Hook is not initialized\n");
		return 0;
	}

	// retrieve the current working directory
	TAP_Hdd_FindFirst(&tapCwd);
	if((strcmp (tapCwd.name, ".") != 0) && (strcmp (tapCwd.name, "__ROOT__") != 0))
	{
		TAP_Print("Cannot determine the current working directory\n");
		return 0;
	}

	TAP_Print("starting TAP ...\n");

	// store the current TAP index
	tapIndex = *hookData.pTapIndex;;
	// Instead of using a code wrapper just save the value of the $gp register
	// and provide the firmware specific $gp value.
	__asm__ __volatile__ ("or %0,$28,$00\n" : "=r"(tapGp) : );
	__asm__ __volatile__ ("or $28,%0,$00\n" : : "r"(pHookData->fwGp));
	// set the current working directory descriptor
	pHookData->setDirDesc(&tapCwd);
	rc = pHookData->fwStartTap(pFilename);
	// restore the current working directory descriptor of the firmware
	pHookData->setDirDesc(pHookData->pFwCwd);
	// restore the TAP value of $gp
	__asm__ __volatile__ ("or $28,%0,$00\n" : : "r"(tapGp));
	// restore the TAP index which has been overwritten by fwStartTap
	*hookData.pTapIndex = tapIndex;

	if(rc != 0)
	{
		TAP_Print("succeeded (%p)\n", rc);
	}
	else
	{
		TAP_Print("failed\n");
	}

	return rc;
}
