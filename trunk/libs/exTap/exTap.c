/*
some code possibly from simonc
some code possibly from peteru
some code possibly from deadbeef
some code possibly from berlios
some code possibly from iceepg
possibly some code from elsewhere ...
*/
/*
        Portions of this code may be covered under the GNU General Public License.
        Any such code is likely to have been modified, possibly beyond recognition, but
        is still covered under this license.
        All other portions are free from any license.

        Some portions may be:
        Copyright (C) 2005 Simon Capewell, http://tap.berlios.de/
        Copyright (C) 2004-2005 Peter Urbanec. All Rights Reserved. http://tap.berlios.de/
        Copyright (C) 2005 DeadBeef, http://de.geocities.com/deadbeef55/
        Copyright (C) 2005 IceTv Pty Limited, http://www.icetv.com.au/
        Copyright (C) 2005 Tony H, http://tonyspage.dyndns.org/

        This is free software; you can redistribute it and/or modify
        it under the terms of the GNU General Public License as published by
        the Free Software Foundation; either version 2 of the License, or
        (at your option) any later version.

        The software is distributed in the hope that it will be useful,
        but WITHOUT ANY WARRANTY; without even the implied warranty of
        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
        GNU General Public License for more details.

        You should have received a copy of the GNU General Public License
        along with this software; if not, write to the Free Software
        Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
/*
    v0.01 - initial release
    v0.02 - added exTAP_EPG_GetNumEvents()
          - removed runningStatus from exTAP_EPG_DeleteEvent()
          - updated for firmware v12.25
    v0.03 - added exTAP_MHEG_Status()
          - added exTAP_GetFirmwareAddress()
          - added exTAP_EPG_Resize()
          - added exTAP_EPG_GetSize()
          - added TAP_Hdd_Move()
    v0.04 - fixed leak int exTAP_EPG_GetSize()
          - fixed bug in hook code trashing $gp when multiple taps active

*/
#include <tap.h>
#include "exTap.h"


//----------------------------------------------------------------------------
type_firmware fwlist_tap_table[] = {            //the tap table base address; 0x20 bytes per tap
    {0x1204, FW_TAP_TABLE_1204},
    {0x1205, FW_TAP_TABLE_1205},
    {0x1209, FW_TAP_TABLE_1209},
    {0x1225, FW_TAP_TABLE_1225},
    {0, 0},
};
type_firmware fwlist_tap_ident[] = {            //the current tap id
    {0x1204, FW_TAP_IDENT_1204},
    {0x1205, FW_TAP_IDENT_1205},
    {0x1209, FW_TAP_IDENT_1209},
    {0x1225, FW_TAP_IDENT_1225},
    {0, 0},
};

type_firmware fwlist_event_table[] = {          //the base of the internal event table, 0x40 bytes per event
    {0x1204, 0x8032e698},
    {0x1205, 0x8032e818},
    {0x1209, 0x80326c4c},
    {0x1225, 0x8032a7c8},
    {0, 0},
};

type_firmware fwlist_event_delete[] = {         //the address of the delete event function
    {0x1204, 0x8014d3f4},
    {0x1205, 0x8014d578},
    {0x1209, 0x8014dff4},
    {0x1225, 0x8014eec0},
    {0, 0},
};

type_firmware fwlist_event_buffer[] = {         //the address of the event buffer structure
    {0x1209, 0x80383ae0},
    {0x1225, 0x8038765c},
    {0, 0},
};

type_firmware fwlist_event_alloc[] = {          //the address of the event buffer allocation function
    {0x1209, 0x80186898},
    {0x1225, 0x80187970},
    {0, 0},
};

type_firmware fwlist_mheg_entry[] = {           //the address of the mheg entry structure
    {0x1204, 0x8042be70},                       //$gp-0x7510
    {0x1205, 0x8042bff0},                       //$gp-0x7510
    {0x1209, 0x80424430},                       //$gp-0x7500
    {0x1225, 0x80427bfc},                       //$gp-0x74f4
    {0, 0},
};

type_firmware fwlist_os_free[] = {              //the address of the opererating system free() function
    {0x1204, 0x80002C24},
    {0x1205, 0x80002C24},
    {0x1209, 0x80002C24},
    {0x1225, 0x80002C68},
    {0, 0},
};

type_firmware fwlist_utc_time[] = {             //the address of the current utc time
    {0x1204, 0x819e70d4},
    {0x1205, 0x819e7254},
    {0x1209, 0x819df6dc},
    {0x1225, 0x819e2eac},
    {0, 0},
};

type_firmware fwlist_utctolocal_time[] = {      //the address of the current utc2local time function
    {0x1204, 0x8012b08c},
    {0x1205, 0x8012b210},
    {0x1209, 0x8012b730},
    {0x1225, 0x8012c34c},
    {0, 0},
};

#define FW_HDD_MOVE_1225 0x8009a348             //the address of the internal hdd_move function

int firmware_id = VERSION_1225;
dword fw_tap_ident = 0;
dword fw_tap_table = 0;
type_eventbuffer *fw_event_buffer = 0;
type_eventtable *fw_event_table = 0;
void *fw_event_delete = 0;
void *fw_event_alloc = 0;
dword *fw_mheg_entry = 0;
void *fw_os_free = 0;
type_taptime *fw_utc_time = 0;
void *fw_utctolocal_time = 0;

dword (*TAP_Hdd_Move)(char *from_dir, char *to_dir, char *filename);
int   exTAP_ApplyPatch2(type_patch2 *patch);
int   exTAP_RemovePatch2(type_patch2 *patch);
int   exTAP_ApplyPatch3(dword addr, byte *from, byte *to, int length);

//----------------------------------------------------------------------------
int exTAP_Init(void)
{
    word SysID = *(word*)pSysID;

    switch(SysID) {
        case 456:
            break;
        default:
            TAP_Print("\nexTAP_Init(), Unsupported system id - %d", SysID);
            TAP_Print("\nexTAP library only supports system id 456");
            TAP_Print("\n");
            return -1;
    }
    switch(_appl_version) {
        case 0x1204:
            firmware_id = VERSION_1204;
            break;
        case 0x1205:
            firmware_id = VERSION_1205;
            break;
        case 0x1209:
            firmware_id = VERSION_1209;
            break;
        case 0x1225:
            firmware_id = VERSION_1225;
            break;
        default:
            firmware_id = VERSION_1225;
            TAP_Print("\nexTAP_Init(), Unsupported firmware version - %04x", _appl_version);
            TAP_Print("\nexTAP library only supports firmware versions 5.12.04, 5.12.05, 5.12.09, 5.12.25");
            TAP_Print("\n");
            return -1;
    }

    fw_tap_ident = exTAP_GetFirmwareAddress(fwlist_tap_ident);
    if (!fw_tap_ident) {
        TAP_Print("\nfw_tap_ident: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }

    fw_tap_table = exTAP_GetFirmwareAddress(fwlist_tap_table);
    if (!fw_tap_table) {
        TAP_Print("\nfw_tap_table: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }

    fw_event_buffer = (type_eventbuffer *)exTAP_GetFirmwareAddress(fwlist_event_buffer);
    if (!fw_event_buffer) {
        TAP_Print("\nfw_event_buffer: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }

    fw_event_table = (type_eventtable *)exTAP_GetFirmwareAddress(fwlist_event_table);
    if (!fw_event_table) {
        TAP_Print("\nfw_event_table: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }

    fw_event_delete = (void *)exTAP_GetFirmwareAddress(fwlist_event_delete);
    if (!fw_event_delete) {
        TAP_Print("\nfw_event_delete: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }

    fw_event_alloc = (void *)exTAP_GetFirmwareAddress(fwlist_event_alloc);
    if (!fw_event_alloc) {
        TAP_Print("\nfw_event_alloc: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }

    fw_mheg_entry = (dword *)exTAP_GetFirmwareAddress(fwlist_mheg_entry);
    if (!fw_mheg_entry) {
        TAP_Print("\nfw_mheg_entry: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }

    fw_os_free = (void *)exTAP_GetFirmwareAddress(fwlist_os_free);
    if (!fw_os_free) {
        TAP_Print("\nfw_os_free: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }

    fw_utc_time = (type_taptime *)exTAP_GetFirmwareAddress(fwlist_utc_time);
    if (!fw_utc_time) {
        TAP_Print("\nfw_utc_time: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }

    fw_utctolocal_time = (void *)exTAP_GetFirmwareAddress(fwlist_utctolocal_time);
    if (!fw_utctolocal_time) {
        TAP_Print("\nfw_utctolocal_time: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }



    TAP_Hdd_Move = TAP_GetSystemProc(oTAP_Hdd_Move);

    return 0;
}


#define MAX_FIRMWARE_REVISIONS 0x10

dword exTAP_GetFirmwareAddress(type_firmware *firmware)
{
    int i;

    for (i=0; i<MAX_FIRMWARE_REVISIONS; i++) {
        if (firmware[i].revision > 0) {
            if (firmware[i].revision == _appl_version) {
                return firmware[i].address;
            }
        }
        else break;
    }
    TAP_Print("\nexTAP_GetFirmwareAddress(), Unsupported firmware version - %04x", _appl_version);
    return 0;
}

//----------------------------------------------------------------------------
/*
for reference, gcc inline requires a numeric register naming convention
$0    0         $t0   8         $s0  16             $t8  24
$at,$1   1      $t1   9         $s1  17             $t9  25
$v0   2         $t2  10         $s2  18             $k0  26
$v1   3         $t3  11         $s3  19             $k1  27
$a0   4         $t4  12         $s4  20             $gp  28
$a1   5         $t5  13         $s5  21             $sp  29
$a2   6         $t6  14         $s6  22             $s8  30
$a3   7         $t7  15         $s7  23             $ra  31
*/
//what is the order for registers after a0,a1,a2,a3, ... - uses stack
dword exTAP_CallFirmware(void *pFunction, dword p0, dword p1, dword p2, dword p3)
{
    // The function has to be implemented in assembler in order to
    // gain full control over register usage.
    // The firmware makes use of the $gp register which contains
    // a different value when running in TAP context.
    __asm__ __volatile__ (
    "addiu   $29,$29,0xffe0\n"                            //addiu   $sp,$sp,-0x20
    "sw      $31,0x0014($29)\n"                           //sw      $ra,0x0014($sp)         save ra
    "sw      $22,0x0018($29)\n"                           //sw      $s6,0x0018($sp)         save s6

    "or      $15,%0,$00\n"                                //or      $t7,tap_ident,$00       ptr to tap_ident
    "or      $22,%1,$00\n"                                //or      $s6,tap_table,$00       ptr to tap_table

    "or      $08,$00,%2\n"                                //or      $t0,$00,pFunction       beware of the order here ...
    "or      $04,$00,%3\n"                                //or      $a0,$00,p0
    "or      $05,$00,%4\n"                                //or      $a1,$00,p1
    "or      $06,$00,%5\n"                                //or      $a2,$00,p2
    "or      $07,$00,%6\n"                                //or      $a3,$00,p3

    "lw      $15,0x0000($15)\n"                           //lw      $t7,0x0000($t7)         get tap_ident
    "sll     $15,$15,0x05\n"                              //sll     $t7,$t7,0x05            tap_ident * 32
    "addu    $22,$22,$15\n"                               //addu    $s6,$s6,$t7             offset to this taps tap_table

    "lw      $03,0x0000($22)\n"                           //lw      $v1,0x0000($s6)         get firmware gp
    "or      $28,$03,$00\n"                               //or      $gp,$v1,$00             set gp

    "jal     $08\n"                                       //jal     $t0                     call function(p0, p1, p2, p3)

    "lw      $03,0x0004($22)\n"                           //lw      $v1,0x0004($s6)         get tap gp
    "or      $28,$03,$00\n"                               //or      $gp,$v1,$00             restore gp

    "lw      $31,0x0014($29)\n"                           //lw      $ra,0x0014($sp)         restore ra
    "lw      $22,0x0018($29)\n"                           //lw      $s6,0x0018($sp)         restore s6
    "addiu   $29,$29,0x0020\n"                            //addiu   $sp,$sp,0x20
    :
    : "r"(fw_tap_ident), "r"(fw_tap_table), "r"(pFunction), "r"(p0), "r"(p1), "r"(p2), "r"(p3));
}


int exTAP_ApplyPatch(type_patch *patch)
{
    int i;

    //check firmware version
    if (patch->revision != _appl_version) {
            TAP_Print("\nexTAP_ApplyPatch(), firmware version %04x not supported", _appl_version);
            TAP_Print("\n");
            return -1;
    }

    //check if firmware is already patched
    for (i=0; i<patch->count; i++) {
        if (*(dword *)(patch->info[i].addr) == *(dword *)(&patch->info[i].to)) {
            if (i == (patch->count-1)) {
                TAP_Print("\nexTAP_ApplyPatch(), firmware is already patched - 0x%08x", patch->info[i].addr);
               return -1;
            }
        }
        else {
            break;
        }
    }

    //check if original firmware matches expected data
    for (i=0; i<patch->count; i++) {
        if (*(dword *)(patch->info[i].addr) != *(dword *)(&patch->info[i].from)) {
            TAP_Print("\nexTAP_ApplyPatch(), original firmware does not match expected - 0x%08x", patch->info[i].addr);
            return -2;
        }
    }

    //apply patch
    for (i=0; i<patch->count; i++) {
        memcpy((byte *)patch->info[i].addr, patch->info[i].to, 4);
    }
    return 0;
}

int exTAP_RemovePatch(type_patch *patch)
{
    int i;

    //check firmware version
    if (patch->revision != _appl_version) {
            TAP_Print("\nexTAP_RemovePatch(), firmware version %04x not supported", _appl_version);
            TAP_Print("\n");
            return -1;
    }

    //check if firmware is already patched
    for (i=0; i<patch->count; i++) {
        if (*(dword *)(patch->info[i].addr) == *(dword *)(&patch->info[i].from)) {
            if (i == (patch->count-1)) {
                TAP_Print("\nexTAP_RemovePatch(), firmware is not already patched - 0x%08x", patch->info[i].addr);
                return -1;
            }
        }
        else {
            break;
        }
    }

    //check if patched data matches original firmware
    for (i=0; i<patch->count; i++) {
        if (*(dword *)(patch->info[i].addr) != *(dword *)(&patch->info[i].to)) {
            TAP_Print("\nexTAP_RemovePatch(), patched data does not match original firmware - 0x%08x", patch->info[i].addr);
            return -2;
        }
    }

    //apply patch
    for (i=0; i<patch->count; i++) {
        memcpy((byte *)patch->info[i].addr, patch->info[i].from, 4);
    }
    return 0;
}


int exTAP_ApplyPatch2(type_patch2 *patch)
{
    int i;

    //check firmware version
    if (patch->revision != _appl_version) {
            TAP_Print("\nexTAP_ApplyPatch2(), firmware version %04x not supported", _appl_version);
            TAP_Print("\n");
            return -1;
    }

    //check if firmware is already patched
    for (i=0; i<patch->length; i++) {
        if (*(byte *)(patch->addr+i) == patch->to[i]) {
            if (i == (patch->length-1)) {
                TAP_Print("\nexTAP_ApplyPatch2(), firmware is already patched - 0x%08x",patch->addr);
                return -1;
            }
        }
        else {
            break;
        }
    }

    //check if original firmware matches expected data
    for (i=0; i<patch->length; i++) {
        if (*(byte *)(patch->addr+i) != patch->from[i]) {
            TAP_Print("\nexTAP_ApplyPatch2(), original firmware does not match expected - 0x%08x",patch->addr+i);
            return -2;
        }
    }

    //apply patch
    memcpy((byte *)patch->addr, patch->to, patch->length);
    return 0;
}

int exTAP_RemovePatch2(type_patch2 *patch)
{
    int i;

    //check firmware version
    if (patch->revision != _appl_version) {
            TAP_Print("\nexTAP_RemovePatch2(), firmware version %04x not supported", _appl_version);
            TAP_Print("\n");
            return -1;
    }

    //check if firmware is already patched
    for (i=0; i<patch->length; i++) {
        if (*(byte *)(patch->addr+i) == patch->from[i]) {
            if (i == (patch->length-1)) {
                TAP_Print("\nexTAP_RemovePatch2(), firmware is not already patched - 0x%08x",patch->addr);
                return -1;
            }
        }
        else {
            break;
        }
    }

    //check if patched data matches original firmware
    for (i=0; i<patch->length; i++) {
        if (*(byte *)(patch->addr+i) != patch->to[i]) {
            TAP_Print("\nexTAP_RemovePatch2(), patched data does not match original firmware - 0x%08x",patch->addr+i);
            return -2;
        }
    }

    //apply patch
    memcpy((byte *)patch->addr, patch->from, patch->length);
    return 0;
}


int exTAP_ApplyPatch3(dword addr, byte *from, byte *to, int length)
{
    int i;

    //check if firmware is already patched
    for (i=0; i<length; i++) {
        if (*(byte *)(addr+i) == to[i]) {
            if (i == (length-1)) {
                TAP_Print("\nexTAP_ApplyPatch3(), firmware is already patched - 0x%08x",addr);
                return -1;
            }
        }
        else {
            break;
        }
    }

    //check if original firmware matches expected data
    for (i=0; i<length; i++) {
        if (*(byte *)(addr+i) != from[i]) {
            TAP_Print("\nexTAP_ApplyPatch3(), original firmware does not match expected - 0x%08x",addr+i);
            return -2;
        }
    }

    //apply patch
    memcpy((byte *)addr, to, length);
    return 0;
}

//----------------------------------------------------------------------------
static __inline__ int hook_prefix()
{
    //save some registers
    __asm__ __volatile__ (
    ".set noat\n"
    "addiu   $29,$29,0xff70\n"              //addiu   $sp,$sp,-0x90
    "sw      $0, 0x0010($29)\n"             //sw      $0, 0x0010($sp)         save 0
    "sw      $1, 0x0014($29)\n"             //sw      $at,0x0014($sp)         save at
    "sw      $2, 0x0018($29)\n"             //sw      $v0,0x0018($sp)         save v0
    "sw      $3, 0x001c($29)\n"             //sw      $v1,0x001c($sp)         save v1
    "sw      $4, 0x0020($29)\n"             //sw      $a0,0x0020($sp)         save a0
    "sw      $5, 0x0024($29)\n"             //sw      $a1,0x0024($sp)         save a1
    "sw      $6, 0x0028($29)\n"             //sw      $a2,0x0028($sp)         save a2
    "sw      $7, 0x002c($29)\n"             //sw      $a3,0x002c($sp)         save a3

    "sw      $8, 0x0030($29)\n"             //sw      $t0,0x0030($sp)         save t0
    "sw      $9, 0x0034($29)\n"             //sw      $t1,0x0034($sp)         save t1
    "sw      $10,0x0038($29)\n"             //sw      $t2,0x0038($sp)         save t2
    "sw      $11,0x003c($29)\n"             //sw      $t3,0x003c($sp)         save t3
    "sw      $12,0x0040($29)\n"             //sw      $t4,0x0040($sp)         save t4
    "sw      $13,0x0044($29)\n"             //sw      $t5,0x0044($sp)         save t5
    "sw      $14,0x0048($29)\n"             //sw      $t6,0x0048($sp)         save t6
    "sw      $15,0x004c($29)\n"             //sw      $t7,0x004c($sp)         save t7

    "sw      $16,0x0050($29)\n"             //sw      $s0,0x0050($sp)         save s0
    "sw      $17,0x0054($29)\n"             //sw      $s1,0x0054($sp)         save s1
    "sw      $18,0x0058($29)\n"             //sw      $s2,0x0058($sp)         save s2
    "sw      $19,0x005c($29)\n"             //sw      $s3,0x005c($sp)         save s3
    "sw      $20,0x0060($29)\n"             //sw      $s4,0x0060($sp)         save s4
    "sw      $21,0x0064($29)\n"             //sw      $s5,0x0064($sp)         save s5
    "sw      $22,0x0068($29)\n"             //sw      $s6,0x0068($sp)         save s6
    "sw      $23,0x006c($29)\n"             //sw      $s7,0x006c($sp)         save s7

    "sw      $24,0x0070($29)\n"             //sw      $t8,0x0070($sp)         save t8
    "sw      $25,0x0074($29)\n"             //sw      $t9,0x0074($sp)         save t9
    "sw      $26,0x0078($29)\n"             //sw      $k9,0x0078($sp)         save k0
    "sw      $27,0x007c($29)\n"             //sw      $k9,0x007c($sp)         save k1
    "sw      $28,0x0080($29)\n"             //sw      $gp,0x0080($sp)         save gp
    "sw      $29,0x0084($29)\n"             //sw      $sp,0x0084($sp)         save sp
    "sw      $30,0x0088($29)\n"             //sw      $s8,0x0088($sp)         save s8
    "sw      $31,0x008c($29)\n"             //sw      $ra,0x008c($sp)         save ra
    :
    :
    );
}

static __inline__ int hook_postfix()
{
    //restore the registers
    __asm__ __volatile__ (
    ".set noat\n"
    "lw      $31,0x008c($29)\n"             //lw      $ra,0x008c($sp)         restore ra
    "lw      $30,0x0088($29)\n"             //lw      $s8,0x0088($sp)         restore s8
//  "lw      $29,0x0084($29)\n"             //lw      $sp,0x0084($sp)         restore sp
    "lw      $28,0x0080($29)\n"             //lw      $gp,0x0080($sp)         restore gp
    "lw      $27,0x007c($29)\n"             //lw      $k1,0x007c($sp)         restore k1
    "lw      $26,0x0078($29)\n"             //lw      $k0,0x0078($sp)         restore k0
    "lw      $25,0x0074($29)\n"             //lw      $t9,0x0074($sp)         restore t9
    "lw      $24,0x0070($29)\n"             //lw      $t8,0x0070($sp)         restore t8

    "lw      $23,0x006c($29)\n"             //lw      $s7,0x006c($sp)         restore s7
    "lw      $22,0x0068($29)\n"             //lw      $s6,0x0068($sp)         restore s6
    "lw      $21,0x0064($29)\n"             //lw      $s5,0x0064($sp)         restore s5
    "lw      $20,0x0060($29)\n"             //lw      $s4,0x0060($sp)         restore s4
    "lw      $19,0x005c($29)\n"             //lw      $s3,0x005c($sp)         restore s3
    "lw      $18,0x0058($29)\n"             //lw      $s2,0x0058($sp)         restore s2
    "lw      $17,0x0054($29)\n"             //lw      $s1,0x0054($sp)         restore s1
    "lw      $16,0x0050($29)\n"             //lw      $s0,0x0050($sp)         restore s0

    "lw      $15,0x004c($29)\n"             //lw      $s7,0x004c($sp)         restore t7
    "lw      $14,0x0048($29)\n"             //lw      $s6,0x0048($sp)         restore t6
    "lw      $13,0x0044($29)\n"             //lw      $s5,0x0044($sp)         restore t5
    "lw      $12,0x0040($29)\n"             //lw      $s4,0x0040($sp)         restore t4
    "lw      $11,0x003c($29)\n"             //lw      $s3,0x003c($sp)         restore t3
    "lw      $10,0x0038($29)\n"             //lw      $s2,0x0038($sp)         restore t2
    "lw      $9, 0x0034($29)\n"             //lw      $s1,0x0034($sp)         restore t1
    "lw      $8, 0x0030($29)\n"             //lw      $s0,0x0030($sp)         restore t0

    "lw      $7, 0x002c($29)\n"             //lw      $a3,0x002c($sp)         restore a3
    "lw      $6, 0x0028($29)\n"             //lw      $a2,0x0028($sp)         restore a2
    "lw      $5, 0x0024($29)\n"             //lw      $a1,0x0024($sp)         restore a1
    "lw      $4, 0x0020($29)\n"             //lw      $a0,0x0020($sp)         restore a0
    "lw      $3, 0x001c($29)\n"             //lw      $v1,0x001c($sp)         restore v1
    "lw      $2, 0x0018($29)\n"             //lw      $v0,0x0018($sp)         restore v0
    "lw      $1, 0x0014($29)\n"             //lw      $at,0x0014($sp)         restore at
//  "lw      $0, 0x0010($29)\n"             //lw      $0, 0x0010($sp)         restore 0
    "addiu   $29,$29,0x0090\n"              //addiu   $sp,$sp,0x90
    :
    :
    );
}

extern void *hook_entry_1209;
extern void *hook_entry_1225;

// !!! 1209 ONLY !!!
static int hook_1209()
{
    int tap_ident;
    int tap_table;

    __asm__ __volatile__ ("hook_entry_1209:\n");
    hook_prefix();      //save the original resgiters

    //cannot use anything that relies on $gp here ...
    //i.e. constants only, no variables
    //...so for now, only support 1 firmware version
    tap_ident = FW_TAP_IDENT_1209;
    tap_table = FW_TAP_TABLE_1209;

    __asm__ __volatile__ (
    ".set noat\n"
    "hook_main_1209:\n"

    "move  $20, $0              \n"     //    move    $s4, $0                //tap_ident = 0
    "move  $21, %0              \n"     //    move    $s5, tap_ident         //&tap_ident
    "move  $22, %1              \n"     //    move    $s6, tap_table         //&tap_table
//save tap_ident
    "lw    $23, 0($21)          \n"     //    lw      $s7, 0($s5)            //save  initial tap_ident

    "next_tap_1209:             \n"
    "lw    $19, 0x10($22)       \n"     //    lw      $s3, 0x10($s6)         //tap_entry - 0 if tap not present
    "beqz  $19, tap_not_present_1209 \n"     //    beqz    $s3, tap_not_present
    "sw    $20, 0($21)          \n"     //    sw      $s4, 0($s5)            //set current tap_ident
    "lw    $16, 0x1c($19)       \n"     //    lw      $s0, 0x1c($s3)         //event_handler

    "lw    $19, 4($22)          \n"     //    lw      $s3, 4($s6)            //tap gp
    "move  $28, $19             \n"     //    move    $gp, $s3               //set gp

    "li    $4,  0xf100          \n"     //    li      $a0, EVT_HOOK          //event = EVT_HOOK (0xf100)
    "lw    $5,  0x0014($29)     \n"     //    lw      $a1,0x0014($sp)        //param1 = hook id (originally in $at)
    "addiu $6,  $29, 0x10       \n"     //    addiu   $a2, $sp, 0x10         //param2 = pointer to saved registers
    "jalr  $16                  \n"     //    jalr    $s0                    //ret = TAP_EventHandler(event, param1, param2)

    "lw    $19, 0($22)          \n"     //    lw      $s3, 0($s6)            //get firmware gp
    "move  $28, $19             \n"     //    move    $gp, $s3               //restore firmware gp

    "beqz  $2, taps_done0_1209  \n"     //    beqz    $v0, taps_done0        //bail out if v0 == 0, to stop hook from being passed to more taps
    "bltz  $2, taps_doneNEG_1209\n"     //    bltz    $v0, taps_doneNEG      //bail out if v0 < 0, to force the hook to exit with an 'rts'

    "tap_not_present_1209:      \n"
    "addiu $22, 0x20            \n"     //    addiu   $s6, 0x20              //next tap
    "addiu $20, 1               \n"     //    addiu   $s4, 1                 //next tap_ident
    "sltiu $1, $20, 0x10        \n"     //    sltiu   $at, $s4, 0x10         //got to 16 yet?
    "bnez  $1, next_tap_1209    \n"     //    bnez    $at, next_tap          //go round again

    "taps_done0_1209:           \n"
    "lw    $1, 0x0014($29)      \n"     //    lw      $at,0x0014($sp)        //hook id (originally in $at)
    "sll   $1, $1, 1            \n"     //    sll     $at, $at, 1            //hook id * 2
    "sw    $1, 0x0014($29)      \n"     //    sw      $at,0x0014($sp)        //save modified hook id
    "b     taps_done_1209       \n"     //    b       taps_done              //continue

    "taps_doneNEG_1209:         \n"
    "lw    $1, 0x0014($29)      \n"     //    lw      $at,0x0014($sp)        //hook id (originally in $at)
    "sll   $1, $1, 1            \n"     //    sll     $at, $at, 1            //hook id * 2
    "addiu $1, $1, 0x01         \n"     //    addiu   $at, $at, 0x01         //add 1 if rts
    "sw    $1, 0x0014($29)      \n"     //    sw      $at,0x0014($sp)        //save modified hook id

    "taps_done_1209:             \n"

//restore tap_ident
    "sw    $23, 0($21)          \n"     //    sw      $s7, 0($s5)            //restore initial tap_ident

    :
    : "r"(tap_ident), "r"(tap_table)
    );

    hook_postfix();     //restores the original registers

    __asm__ __volatile__ (
    ".set noat\n"
    "hook_postfix2_1209:\n"

    //a bit crude ...
    "addiu $1, -2               \n"     //    addiu   $at, -2               //0 is not used to allow event to return 0
    "beqz  $1, hook_exit1       \n"     //    beqz    $at, hook_exit1
   "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit1       \n"     //    beqz    $at, hook_quit1
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit2       \n"     //    beqz    $at, hook_exit2
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit2       \n"     //    beqz    $at, hook_quit2
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit3       \n"     //    beqz    $at, hook_exit3
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit3       \n"     //    beqz    $at, hook_quit3
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit4       \n"     //    beqz    $at, hook_exit4
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit4       \n"     //    beqz    $at, hook_quit4
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit5       \n"     //    beqz    $at, hook_exit5
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit5       \n"     //    beqz    $at, hook_quit5
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit6       \n"     //    beqz    $at, hook_exit6
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit6       \n"     //    beqz    $at, hook_quit6
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit7       \n"     //    beqz    $at, hook_exit7
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit7       \n"     //    beqz    $at, hook_quit7
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit8       \n"     //    beqz    $at, hook_exit8
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit8       \n"     //    beqz    $at, hook_quit8
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit9       \n"     //    beqz    $at, hook_exit9
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit9       \n"     //    beqz    $at, hook_quit9
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit10      \n"     //    beqz    $at, hook_exit10
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit10      \n"     //    beqz    $at, hook_quit10
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit11      \n"     //    beqz    $at, hook_exit11
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit11      \n"     //    beqz    $at, hook_quit11
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit12      \n"     //    beqz    $at, hook_exit12
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit12      \n"     //    beqz    $at, hook_quit12
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit13      \n"     //    beqz    $at, hook_exit13
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit13      \n"     //    beqz    $at, hook_quit13
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit14      \n"     //    beqz    $at, hook_exit14
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit14      \n"     //    beqz    $at, hook_quit14
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit15      \n"     //    beqz    $at, hook_exit15
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit15      \n"     //    beqz    $at, hook_quit15
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit16      \n"     //    beqz    $at, hook_exit16
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit16      \n"     //    beqz    $at, hook_quit16
    //broken ...
    "j     hook_quit16          \n"     //    j       hook_quit16
    );
}

// !!! 1225 ONLY !!!
static int hook_1225()
{
    int tap_ident;
    int tap_table;

    __asm__ __volatile__ ("hook_entry_1225:\n");
    hook_prefix();      //save the original resgiters

    //cannot use anything that relies on $gp here ...
    //i.e. constants only, no variables
    //...so for now, only support 1 firmware version
    tap_ident = FW_TAP_IDENT_1225;
    tap_table = FW_TAP_TABLE_1225;

    __asm__ __volatile__ (
    ".set noat\n"
    "hook_main_1225:\n"

    "move  $20, $0              \n"     //    move    $s4, $0                //tap_ident = 0
    "move  $21, %0              \n"     //    move    $s5, tap_ident         //&tap_ident
    "move  $22, %1              \n"     //    move    $s6, tap_table         //&tap_table
//save tap_ident
    "lw    $23, 0($21)          \n"     //    lw      $s7, 0($s5)            //save  initial tap_ident

    "next_tap_1225:             \n"
    "lw    $19, 0x10($22)       \n"     //    lw      $s3, 0x10($s6)         //tap_entry - 0 if tap not present
    "beqz  $19, tap_not_present_1225 \n"     //    beqz    $s3, tap_not_present
    "sw    $20, 0($21)          \n"     //    sw      $s4, 0($s5)            //set current tap_ident
    "lw    $16, 0x1c($19)       \n"     //    lw      $s0, 0x1c($s3)         //event_handler

    "lw    $19, 4($22)          \n"     //    lw      $s3, 4($s6)            //tap gp
    "move  $28, $19             \n"     //    move    $gp, $s3               //set gp

    "li    $4,  0xf100          \n"     //    li      $a0, EVT_HOOK          //event = EVT_HOOK (0xf100)
    "lw    $5,  0x0014($29)     \n"     //    lw      $a1,0x0014($sp)        //param1 = hook id (originally in $at)
    "addiu $6,  $29, 0x10       \n"     //    addiu   $a2, $sp, 0x10         //param2 = pointer to saved registers
    "jalr  $16                  \n"     //    jalr    $s0                    //ret = TAP_EventHandler(event, param1, param2)

    "lw    $19, 0($22)          \n"     //    lw      $s3, 0($s6)            //get firmware gp
    "move  $28, $19             \n"     //    move    $gp, $s3               //restore firmware gp

    "beqz  $2, taps_done0_1225  \n"     //    beqz    $v0, taps_done0        //bail out if v0 == 0, to stop hook from being passed to more taps
    "bltz  $2, taps_doneNEG_1225\n"     //    bltz    $v0, taps_doneNEG      //bail out if v0 < 0, to force the hook to exit with an 'rts'

    "tap_not_present_1225:      \n"
    "addiu $22, 0x20            \n"     //    addiu   $s6, 0x20              //next tap
    "addiu $20, 1               \n"     //    addiu   $s4, 1                 //next tap_ident
    "sltiu $1, $20, 0x10        \n"     //    sltiu   $at, $s4, 0x10         //got to 16 yet?
    "bnez  $1, next_tap_1225    \n"     //    bnez    $at, next_tap          //go round again

    "taps_done0_1225:           \n"
    "lw    $1, 0x0014($29)      \n"     //    lw      $at,0x0014($sp)        //hook id (originally in $at)
    "sll   $1, $1, 1            \n"     //    sll     $at, $at, 1            //hook id * 2
    "sw    $1, 0x0014($29)      \n"     //    sw      $at,0x0014($sp)        //save modified hook id
    "b     taps_done_1225       \n"     //    b       taps_done              //continue

    "taps_doneNEG_1225:         \n"
    "lw    $1, 0x0014($29)      \n"     //    lw      $at,0x0014($sp)        //hook id (originally in $at)
    "sll   $1, $1, 1            \n"     //    sll     $at, $at, 1            //hook id * 2
    "addiu $1, $1, 0x01         \n"     //    addiu   $at, $at, 0x01         //add 1 if rts
    "sw    $1, 0x0014($29)      \n"     //    sw      $at,0x0014($sp)        //save modified hook id

    "taps_done_1225:            \n"

//restore tap_ident
    "sw    $23, 0($21)          \n"     //    sw      $s7, 0($s5)            //restore initial tap_ident

    :
    : "r"(tap_ident), "r"(tap_table)
    );

    hook_postfix();     //restores the original registers

    __asm__ __volatile__ (
    ".set noat\n"
    "hook_postfix2_1225:\n"

    //a bit crude ...
    "addiu $1, -2               \n"     //    addiu   $at, -2               //0 is not used to allow event to return 0
    "beqz  $1, hook_exit1       \n"     //    beqz    $at, hook_exit1
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit1       \n"     //    beqz    $at, hook_quit1
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit2       \n"     //    beqz    $at, hook_exit2
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit2       \n"     //    beqz    $at, hook_quit2
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit3       \n"     //    beqz    $at, hook_exit3
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit3       \n"     //    beqz    $at, hook_quit3
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit4       \n"     //    beqz    $at, hook_exit4
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit4       \n"     //    beqz    $at, hook_quit4
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit5       \n"     //    beqz    $at, hook_exit5
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit5       \n"     //    beqz    $at, hook_quit5
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit6       \n"     //    beqz    $at, hook_exit6
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit6       \n"     //    beqz    $at, hook_quit6
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit7       \n"     //    beqz    $at, hook_exit7
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit7       \n"     //    beqz    $at, hook_quit7
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit8       \n"     //    beqz    $at, hook_exit8
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit8       \n"     //    beqz    $at, hook_quit8
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit9       \n"     //    beqz    $at, hook_exit9
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit9       \n"     //    beqz    $at, hook_quit9
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit10      \n"     //    beqz    $at, hook_exit10
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit10      \n"     //    beqz    $at, hook_quit10
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit11      \n"     //    beqz    $at, hook_exit11
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit11      \n"     //    beqz    $at, hook_quit11
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit12      \n"     //    beqz    $at, hook_exit12
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit12      \n"     //    beqz    $at, hook_quit12
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit13      \n"     //    beqz    $at, hook_exit13
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit13      \n"     //    beqz    $at, hook_quit13
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit14      \n"     //    beqz    $at, hook_exit14
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit14      \n"     //    beqz    $at, hook_quit14
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit15      \n"     //    beqz    $at, hook_exit15
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit15      \n"     //    beqz    $at, hook_quit15
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_exit16      \n"     //    beqz    $at, hook_exit16
    "addiu $1, -1               \n"     //    addiu   $at, -1
    "beqz  $1, hook_quit16      \n"     //    beqz    $at, hook_quit16
    //broken ...
    "j     hook_quit16          \n"     //    j       hook_quit16
    );
}

//this gets patched with the initially overwritten firmware code (up to 4 words)
//and the jump back into the firmware
static int hook_exit1()
{
    __asm__ __volatile__ (
    "nop\n"                             //  first line of pFunc
    "nop\n"                             //  second line of pFunc
    "nop\n"                             //  j  firmware+8
    "nop\n"
    "nop\n"                             //this gets optimised after the rts ...
    :
    : );
}
static int hook_quit1()
{
    __asm__ __volatile__ (
    "nop\n"                             //  first line of pFunc
    "nop\n"                             //  second line of pFunc
    "nop\n"                             //  j  $ra  (don't do it here, because gcc reoptimises for delay slots
    "nop\n"
    "nop\n"                             //this gets optimised after the rts ...
    :
    : );
}
static int hook_exit2() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit2() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit3() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit3() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit4() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit4() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit5() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit5() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit6() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit6() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit7() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit7() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit8() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit8() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit9() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit9() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit10() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit10() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit11() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit11() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit12() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit12() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit13() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit13() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit14() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit14() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit15() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit15() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_exit16() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }
static int hook_quit16() { __asm__ __volatile__ ("nop\nnop\nnop\nnop\nnop\n"); }

//----------------------------------------------------------------------------
typedef struct  {
    type_hook hook;
    dword exit;                 //hook exit address
    dword quit;                 //hook quit address
    byte  to[0x100];            //code patched to firmware
}type_hook_list;

type_hook_list hook_list[1+MAX_HOOKS];

//return -ve = error
//return +ve = hook_handle
int exTAP_ApplyHook(type_hook *hook)
{
    int i,j;
    int ret;
    int id;
    dword *hook_entry;

    //check firmware version
    switch (_appl_version) {
        case 0x1209:
            hook_entry = (dword *)&hook_entry_1209;
            break;
        case 0x1225:
            hook_entry = (dword *)&hook_entry_1225;
            break;
        default:
            TAP_Print("\nexTAP_ApplyHook(), firmware version %04x not supported", _appl_version);
            TAP_Print("\n");
            return -1;
            break;
    }

    if (hook->revision != _appl_version) {
        TAP_Print("\nexTAP_ApplyHook(), invalid firmware version %04x", hook->revision);
        TAP_Print("\n");
        return -1;
    }

    if ((hook->enable) == 0) {
        //ignore; not enabled
        return -1;
    }

    //match against an existing
    for (i=1; i<=MAX_HOOKS; i++) {
        if (hook_list[i].hook.enable) {
            if (hook->addr == hook_list[i].hook.addr) {
                if (hook->length == hook_list[i].hook.length) {
                    TAP_Print("\nexTAP_ApplyHook(), hook already exists");
                    return -1;
                }
            }
        }
    }

    //find a spare slot
    id = -1;
    for (i=1; i<=MAX_HOOKS; i++) {
        if (hook_list[i].hook.enable == 0) {
            id = i;
            break;
        }
    }
    if (id < 0) {
        TAP_Print("\nexTAP_ApplyHook(), no more free hooks");
        return -2;
    }

    //save the hook details
    hook_list[id].hook.addr   = hook->addr;
    hook_list[id].hook.length = hook->length;
    memcpy(hook_list[id].hook.from, hook->from, hook->length);

//!!!this code produces 9x Warning: Macro used $at after ".set noat"
//seems to be bogus ...
    switch (id) {
        case 1:
            hook_list[id].exit = (dword)hook_exit1;
            hook_list[id].quit = (dword)hook_quit1;
            break;
        case 2:
            hook_list[id].exit = (dword)hook_exit2;
            hook_list[id].quit = (dword)hook_quit2;
            break;
        case 3:
            hook_list[id].exit = (dword)hook_exit3;
            hook_list[id].quit = (dword)hook_quit3;
            break;
        case 4:
            hook_list[id].exit = (dword)hook_exit4;
            hook_list[id].quit = (dword)hook_quit4;
            break;
        case 5:
            hook_list[id].exit = (dword)hook_exit5;
            hook_list[id].quit = (dword)hook_quit5;
            break;
        case 6:
            hook_list[id].exit = (dword)hook_exit6;
            hook_list[id].quit = (dword)hook_quit6;
            break;
        case 7:
            hook_list[id].exit = (dword)hook_exit7;
            hook_list[id].quit = (dword)hook_quit7;
            break;
        case 8:
            hook_list[id].exit = (dword)hook_exit8;
            hook_list[id].quit = (dword)hook_quit8;
            break;
        case 9:
            hook_list[id].exit = (dword)hook_exit9;
            hook_list[id].quit = (dword)hook_quit9;
            break;
        case 10:
            hook_list[id].exit = (dword)hook_exit10;
            hook_list[id].quit = (dword)hook_quit10;
            break;
        case 11:
            hook_list[id].exit = (dword)hook_exit11;
            hook_list[id].quit = (dword)hook_quit11;
            break;
        case 12:
            hook_list[id].exit = (dword)hook_exit12;
            hook_list[id].quit = (dword)hook_quit12;
            break;
        case 13:
            hook_list[id].exit = (dword)hook_exit13;
            hook_list[id].quit = (dword)hook_quit13;
            break;
        case 14:
            hook_list[id].exit = (dword)hook_exit14;
            hook_list[id].quit = (dword)hook_quit14;
            break;
        case 15:
            hook_list[id].exit = (dword)hook_exit15;
            hook_list[id].quit = (dword)hook_quit15;
            break;
        default:
            hook_list[id].exit = (dword)hook_exit16;
            hook_list[id].quit = (dword)hook_quit16;
            break;
    }

    //patch exit
    //the original firwmare code
    exTAP_ApplyPatch3(hook_list[id].exit, (byte *)hook_list[id].exit, &hook_list[id].hook.from[0], hook_list[id].hook.length);
    //a jump back to the firmware                                                   //08xxxxxx j (hook_list[id].hook.addr+hook_list[id].hook.length)
    *(dword *)(hook_list[id].exit + hook_list[id].hook.length + 0x0) = 0x08000000 | (((hook_list[id].hook.addr+hook_list[id].hook.length) & 0x0ffffffc) >> 2);
    *(dword *)(hook_list[id].exit + hook_list[id].hook.length + 0x4) = 0x00000000;  //00000000 nop

    //an rts for patching out functions
    *(dword *)(hook_list[id].quit + 0x0) = 0x03e00008;     //03e00008 jr $ra
    *(dword *)(hook_list[id].quit + 0x4) = 0x00000000;     //00000000 nop
    *(dword *)(hook_list[id].quit + 0x8) = 0x00000000;     //00000000 nop
    *(dword *)(hook_list[id].quit + 0xc) = 0x00000000;     //00000000 nop

    //hook firmware
    *(dword *)((dword)&hook_list[id].to[0]) = 0x08000000 | ((((dword)hook_entry) & 0x0ffffffc) >> 2);  //08xxxxxx j  hook_entry
    *(dword *)((dword)&hook_list[id].to[4]) = 0x24010000 | (id & 0xffff);                               //24010003 li $at, id   //dont use 0
    ret = exTAP_ApplyPatch3(hook_list[id].hook.addr, &hook_list[id].hook.from[0], &hook_list[id].to[0], hook_list[id].hook.length);
    if (ret) {
        return ret;
    }

    hook_list[id].hook.enable = 1;
    return id;
}

int exTAP_RemoveHook(int id)
{
    int ret;

    //check firmware version
    switch (_appl_version) {
        case 0x1209:
        case 0x1225:
            break;
        default:
            TAP_Print("\nexTAP_RemoveHook(), firmware version %04x not supported", _appl_version);
            TAP_Print("\n");
            return -1;
            break;
    }

    if ((id <1) || (id > MAX_HOOKS)) {
        TAP_Print("\nexTAP_RemoveHook(), invalid id - %d", id);
        return -2;
    }
    if (hook_list[id].hook.enable == 0) {
        TAP_Print("\nexTAP_RemoveHook(), hook %d is not enabled", id);
        return -2;
    }

    //unhook firmware
    ret = exTAP_ApplyPatch3(hook_list[id].hook.addr, &hook_list[id].to[0], &hook_list[id].hook.from[0], hook_list[id].hook.length);

    if (ret == 0) hook_list[id].hook.enable = 0;
    return ret;
}

//----------------------------------------------------------------------------
//
// Event Routines
//

//
//still need to do:
//   bool exTAP_Hdd_GetPlayInfo(TexYPE_PlayInfo *playInfo);
//
TYPE_exTapEvent *exTAP_GetEvent(int chType, int chNum, int *eventNum)
{
    int i,j;
    TYPE_TapChInfo chInfo;
    int num_events;
    type_eventtable *et;
    TYPE_exTapEvent *evInfo;
    char *description;
    int description_length;

    TAP_Channel_GetInfo(chType, chNum, &chInfo);
    num_events = 0;
    for (i=0; i<5000; i++) {
        //match orgNetId, tsId, svcId
        if (chInfo.svcId == fw_event_table[i].service_id) {
            if (chInfo.tsId == fw_event_table[i].transport_stream_id) {
                if (chInfo.orgNetId == fw_event_table[i].original_network_id) {
                    num_events ++;
                }
            }
        }
    }
    evInfo = TAP_MemAlloc(num_events * sizeof(TYPE_exTapEvent));

    j = 0;
    for (i=0; i<5000; i++) {
        //match satIdx, orgNetId, tsId, svcId
        if (chInfo.svcId == fw_event_table[i].service_id) {
            if (chInfo.tsId == fw_event_table[i].transport_stream_id) {
                if (chInfo.orgNetId == fw_event_table[i].original_network_id) {
                    if (chInfo.satIdx == fw_event_table[i].sat_index) {
                        evInfo[j].evtId = fw_event_table[i].event_id;
                        evInfo[j].startTime = (fw_event_table[i].start_date << 16) | (fw_event_table[i].start_hour << 8) | fw_event_table[i].start_min;
                        evInfo[j].endTime = (fw_event_table[i].end_date << 16) | (fw_event_table[i].end_hour << 8) | fw_event_table[i].end_min;
                        evInfo[j].duration = (fw_event_table[i].duration_hour << 8) | fw_event_table[i].duration_min;
                        evInfo[j].runningStatus = fw_event_table[i].char00 & 0x1f;

                        memcpy(evInfo[j].eventName, fw_event_table[i].event_name, fw_event_table[i].event_name_length);
                        evInfo[j].eventName[fw_event_table[i].event_name_length] = '\0';

                        description = (char *)((dword)fw_event_table[i].event_name + fw_event_table[i].event_name_length);
                        description_length = strlen(description);
                        memcpy(evInfo[j].description, description, description_length);
                        evInfo[j].description[description_length] = '\0';

                        evInfo[j].parentalRating = fw_event_table[i].parental_rating;
                        evInfo[j].satIdx = fw_event_table[i].sat_index;
                        evInfo[j].svcId = fw_event_table[i].service_id;
                        evInfo[j].orgNetId = fw_event_table[i].original_network_id;
                        evInfo[j].tsId = fw_event_table[i].transport_stream_id;
                        j ++;
                    }
                }
            }
        }
    }

    *eventNum = num_events;
    return evInfo;
}

TYPE_exTapEvent *exTAP_GetCurrentEvent(int chType, int chNum)
{
    int i;
    TYPE_TapChInfo chInfo;
    TYPE_exTapEvent *evInfo;
    char *description;
    int description_length;


    TAP_Channel_GetInfo(chType, chNum, &chInfo);

    evInfo = TAP_MemAlloc(sizeof(TYPE_exTapEvent));

    for (i=0; i<5000; i++) {
        //match satIdx, orgNetId, tsId, svcId
        if ((fw_event_table[i].char00 & 0x1f) == 4) {   //running
            if (chInfo.svcId == fw_event_table[i].service_id) {
                if (chInfo.tsId == fw_event_table[i].transport_stream_id) {
                    if (chInfo.orgNetId == fw_event_table[i].original_network_id) {
                        if (chInfo.satIdx == fw_event_table[i].sat_index) {
                            evInfo->evtId = fw_event_table[i].event_id;
                            evInfo->startTime = (fw_event_table[i].start_date << 16) | (fw_event_table[i].start_hour << 8) | fw_event_table[i].start_min;
                            evInfo->endTime = (fw_event_table[i].end_date << 16) | (fw_event_table[i].end_hour << 8) | fw_event_table[i].end_min;
                            evInfo->duration = (fw_event_table[i].duration_hour << 8) | fw_event_table[i].duration_min;
                            evInfo->runningStatus = fw_event_table[i].char00 & 0x1f;

                            memcpy(evInfo->eventName, fw_event_table[i].event_name, fw_event_table[i].event_name_length);
                            evInfo->eventName[fw_event_table[i].event_name_length] = '\0';

                            description = (char *)((dword)fw_event_table[i].event_name + fw_event_table[i].event_name_length);
                            description_length = strlen(description);
                            memcpy(evInfo->description, description, description_length);
                            evInfo->description[description_length] = '\0';

                            evInfo->parentalRating = fw_event_table[i].parental_rating;
                            evInfo->satIdx = fw_event_table[i].sat_index;
                            evInfo->svcId = fw_event_table[i].service_id;
                            evInfo->orgNetId = fw_event_table[i].original_network_id;
                            evInfo->tsId = fw_event_table[i].transport_stream_id;
                            break;
                        }
                    }
                }
            }
        }
    }

    return evInfo;
}

byte *exTAP_EPG_GetExtInfo(TYPE_exTapEvent *tapEvtInfo)
{
    byte *extInfo;
    int i;
    TYPE_exTapEvent *evInfo;

    for (i=0; i<5000; i++) {
        //match satIdx, orgNetId, tsId, svcId, evtId,
        if (tapEvtInfo->evtId == fw_event_table[i].event_id) {
            if (tapEvtInfo->svcId == fw_event_table[i].service_id) {
                if (tapEvtInfo->tsId == fw_event_table[i].transport_stream_id) {
                    if (tapEvtInfo->orgNetId == fw_event_table[i].original_network_id) {
                        if (tapEvtInfo->satIdx == fw_event_table[i].sat_index) {
//TAP_Print("\nexTAP_EPG_GetExtInfo(), num_extended        = %02x", fw_event_table[i].num_extended);
//TAP_Print("\nexTAP_EPG_GetExtInfo(), extended_length     = %04x", fw_event_table[i].extended_length);
                            extInfo = TAP_MemAlloc(fw_event_table[i].extended_length+2);
                            memcpy(extInfo, fw_event_table[i].extended_event_name, fw_event_table[i].extended_length);
                            extInfo[fw_event_table[i].extended_length] = '\0';
                            extInfo[fw_event_table[i].extended_length+1] = '\0';
                            break;
                        }
                    }
                }
            }
        }
    }

    return extInfo;
}

bool exTAP_EPG_DeleteEvent(int chType, int chNum, dword evtid)
{
    int i;
    int ret;
    TYPE_TapChInfo chInfo;

    ret = TAP_Channel_GetInfo(chType, chNum, &chInfo);
    if (ret) {
        return 0;
    }

    for (i=0; i<5000; i++) {
        //match satIdx, orgNetId, tsId, svcId, evtId,
        if (evtid == fw_event_table[i].event_id) {
            if (chInfo.svcId == fw_event_table[i].service_id) {
                if (chInfo.tsId == fw_event_table[i].transport_stream_id) {
                    if (chInfo.orgNetId == fw_event_table[i].original_network_id) {
                        if (chInfo.satIdx == fw_event_table[i].sat_index) {

                            //void fw_event_delete(int event_index);
                            exTAP_CallFirmware(fw_event_delete, i, 0,0,0);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

bool exTAP_EPG_UpdateEvent(int chType, int chNum, dword evtid, dword runningStatus)
{
    int i;
    int ret;
    TYPE_TapChInfo chInfo;

    ret = TAP_Channel_GetInfo(chType, chNum, &chInfo);
    if (ret) {
        return 0;
    }

    for (i=0; i<5000; i++) {
        //match satIdx, orgNetId, tsId, svcId, evtId,
        if (evtid == fw_event_table[i].event_id) {
            if (chInfo.svcId == fw_event_table[i].service_id) {
                if (chInfo.tsId == fw_event_table[i].transport_stream_id) {
                    if (chInfo.orgNetId == fw_event_table[i].original_network_id) {
                        if (chInfo.satIdx == fw_event_table[i].sat_index) {

                            fw_event_table[i].char00 = (fw_event_table[i].char00 & 0xe0) | (runningStatus & 0x1f);
                            return 1;
                        }
                    }
                }
            }
        }
    }

    return 0;
}

//
//a quick way to zap the entire firmware internal event table
//
int exTAP_EPG_DeleteAllEvents(void)
{
    int i;
    int num_events;

    num_events = 0;
    for (i=0; i<5000; i++) {
        if (fw_event_table[i].char00 & 0xc0) {
            //void fw_event_delete(int event_index);
            exTAP_CallFirmware(fw_event_delete, i, 0,0,0);
            num_events ++;
        }
    }
    return num_events;
}

int exTAP_EPG_GetNumEvents(void)
{
    int i;
    int num_events;

    num_events = 0;
    for (i=0; i<5000; i++) {
        if (fw_event_table[i].char00 & 0xc0) {
            num_events ++;
        }
    }
    return num_events;
}

//
// alter the size of the internal epg short evant/extended event buffers
// default: 0x80000 (512K), allows ~3000 events
// better:  0x100000 (1M), allows 5000 events
// extended events are likely to need less, unless populated from external source
//
int exTAP_EPG_Resize(dword short_size, dword ext_size)
{
    byte *shortb_ptr;
    byte *extb_ptr;
    int ret;

    TAP_Print("\nexTAP_EPG_Resize(short_size=%x, ext_size=%x)", short_size, ext_size);

    //delete the epg
    exTAP_EPG_DeleteAllEvents();

    //free epg
    shortb_ptr = fw_event_buffer->short_event_buffer;       //short events
    extb_ptr   = fw_event_buffer->extended_event_buffer;    //extended events

    //call os_free
    ret = exTAP_CallFirmware(fw_os_free, (dword)shortb_ptr, 0,0,0); //short events
    ret = exTAP_CallFirmware(fw_os_free, (dword)extb_ptr, 0,0,0);   //extended events

    //alloc epg
    ret = exTAP_CallFirmware(fw_event_alloc, 0, short_size, 0,0); //short events
//ret=addr 0=fail
    if (ret = 0) {
        TAP_Print("\nexTAP_EPG_Resize(), failed, short_size=%x",short_size);
    }
    ret = exTAP_CallFirmware(fw_event_alloc, 1, ext_size, 0,0);   //extended events
    if (ret = 0) {
        TAP_Print("\nexTAP_EPG_Resize(), failed, ext_size=%x",ext_size);
    }

    return 0;
}

int exTAP_EPG_GetSize(int type)
{
    int len;

    TAP_Print("\nexTAP_EPG_GetSize(%s)", (type)?"Extended Event" : "Short Event");

    if (type == 0) {
        len = 32 * fw_event_buffer->short_event_blocks;     //short event (size is in blocks, 32 byte per block)
    }
    else {
        len = 32 * fw_event_buffer->extended_event_blocks;  //extended event
    }
    TAP_Print(", size=%x", len);

    return len;
}
//----------------------------------------------------------------------------
//
// key mapping patch
//
// this patch modifies the EVT_KEY function
// when enabled, param2 contains a pointer to the original key code
// the original key code can be used to differentiate between the
//   ch+/up, ch-/down, vol+/left, vol-/right, tv_radio/list keys
// it allows allows a modified key to be passed back to the firmware
// the return value is passed onto the nxt tap in param1 (must be a remapped keycode)
// if the return value is zero, the key is not passed to the firmware
// if the return value is not zero, the contents of param2 is passed to the firmware (must be an original keycode)
//
// if this patch is enabled, a normal tap will behave exactly as before (as long as it ignores param2)
//
type_patch key_map_patch[] = {
    {//12.04
        0x1204,
        7,
        {
            0x8012c988, {0xaf, 0x22, 0x00, 0x00}, {0x00, 0x40, 0x88, 0x25},
            0x8012c9b0, {0x8f, 0xd1, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
            0x8012c9e0, {0x00, 0x00, 0x30, 0x25}, {0x03, 0xc0, 0x30, 0x25},
            0x8012c9e4, {0xaf, 0xc2, 0x00, 0x00}, {0x00, 0x40, 0x88, 0x25},
            0x8012ca14, {0x8f, 0xd9, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
            0x8012ca18, {0x53, 0x20, 0x00, 0x03}, {0x56, 0x20, 0x00, 0x03},
            0x8012ca20, {0xaf, 0xd6, 0x00, 0x00}, {0xaf, 0xb1, 0x00, 0x00},
        },
    },
    {//12.05
        0x1205,
        7,
        {
            0x8012cb0c, {0xaf, 0x22, 0x00, 0x00}, {0x00, 0x40, 0x88, 0x25},
            0x8012cb34, {0x8f, 0xd1, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
            0x8012cb64, {0x00, 0x00, 0x30, 0x25}, {0x03, 0xc0, 0x30, 0x25},
            0x8012cb68, {0xaf, 0xc2, 0x00, 0x00}, {0x00, 0x40, 0x88, 0x25},
            0x8012cb98, {0x8f, 0xd9, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
            0x8012cb9c, {0x53, 0x20, 0x00, 0x03}, {0x56, 0x20, 0x00, 0x03},
            0x8012cba4, {0xaf, 0xd6, 0x00, 0x00}, {0xaf, 0xb1, 0x00, 0x00},
        },
    },
    {//12.09
        0x1209,     //firmware revision
        7,          //patch count
        {
        //  address,    original data,            new data
            0x8012d02c, {0xaf, 0x22, 0x00, 0x00}, {0x00, 0x40, 0x88, 0x25},     //  sw    $v0, 0($t9)           move  $s1, $v0
            0x8012d054, {0x8f, 0xd1, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},     //  lw    $s1, 0x00($fp)        nop
            0x8012d084, {0x00, 0x00, 0x30, 0x25}, {0x03, 0xc0, 0x30, 0x25},     //  move  $a2, $0               move  $a2, $fp
            0x8012d088, {0xaf, 0xc2, 0x00, 0x00}, {0x00, 0x40, 0x88, 0x25},     //  sw    $v0, 0x00($fp)        move  $s1, $v0
            0x8012d0b8, {0x8f, 0xd9, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},     //  lw    $t9, 0x00($fp)        nop
            0x8012d0bc, {0x53, 0x20, 0x00, 0x03}, {0x56, 0x20, 0x00, 0x03},     //  beqzl $t9, 0x8012d0cc       bnezl $s1, 0x8012d0cc
            0x8012d0c4, {0xaf, 0xd6, 0x00, 0x00}, {0xaf, 0xb1, 0x00, 0x00},     //  sw    $s6, 0x00($fp)        sw    $s1, 0x00($fp)
        },
    },
    {//12.25
        0x1225,
        7,
        {
            0x8012dc48, {0xaf, 0x22, 0x00, 0x00}, {0x00, 0x40, 0x88, 0x25},
            0x8012dc70, {0x8f, 0xd1, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
            0x8012dca0, {0x00, 0x00, 0x30, 0x25}, {0x03, 0xc0, 0x30, 0x25},
            0x8012dca4, {0xaf, 0xc2, 0x00, 0x00}, {0x00, 0x40, 0x88, 0x25},
            0x8012dcd4, {0x8f, 0xd9, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00},
            0x8012dcd8, {0x53, 0x20, 0x00, 0x03}, {0x56, 0x20, 0x00, 0x03},
            0x8012dce0, {0xaf, 0xd6, 0x00, 0x00}, {0xaf, 0xb1, 0x00, 0x00},
        },
    },
};

int exTAP_Key_Extend(int enable)
{
    if (enable) {
        TAP_Print("\nApplying Key Mapping Patch");
        return exTAP_ApplyPatch(&key_map_patch[firmware_id]);
    }
    else {
        TAP_Print("\nRemoving Key Mapping Patch");
        return exTAP_RemovePatch(&key_map_patch[firmware_id]);
    }
}

//----------------------------------------------------------------------------
//
// mheg off patch
//
// this patch disables the dsmcc processing in the firmware
// it does not disable the mheg engine from operating if ant previously acquired data is present
// changing channels will flush the dsmcc file caches
//
//patch out dsmcc handler
//801aacfc 27 bd ff e0   addiu $sp, -0x20               03 e0 00 08    jr   $ra
//801aad00 af b6 00 18   sw    $s6, 0x18($sp)           00 00 00 00    nop
type_patch mheg_off_patch[] = {
    {//12.04
        0x1204,
        2,
        {
            0x801ab874, {0x27, 0xbd, 0xff, 0xc8}, {0x03, 0xe0, 0x00, 0x08},
            0x801ab878, {0xaf, 0xb3, 0x00, 0x24}, {0x00, 0x00, 0x00, 0x00},
        }
    },
    {//12.05
        0x1205,
        2,
        {
            0x801ab9f8, {0x27, 0xbd, 0xff, 0xc8}, {0x03, 0xe0, 0x00, 0x08},
            0x801ab9fc, {0xaf, 0xb3, 0x00, 0x24}, {0x00, 0x00, 0x00, 0x00},
        }
    },
    {//12.09
        0x1209,     //firmware revision
        2,          //patch count
        {
        //  address,    original data,            new data
            0x801aacfc, {0x27, 0xbd, 0xff, 0xe0}, {0x03, 0xe0, 0x00, 0x08},     //  addiu $sp, -0x20            jr   $ra
            0x801aad00, {0xaf, 0xb6, 0x00, 0x18}, {0x00, 0x00, 0x00, 0x00},     //  sw    $s6, 0x18($sp)        nop
        }
    },
    {//12.25
        0x1225,
        2,
        {
            0x801AB730, {0x27, 0xbd, 0xff, 0x78}, {0x03, 0xe0, 0x00, 0x08},
            0x801AB734, {0xaf, 0xb6, 0x00, 0x80}, {0x00, 0x00, 0x00, 0x00},
        }
    },
};

int exTAP_MHEG_Disable(int disable)
{
    if (disable) {
        TAP_Print("\nApplying MHEG Off Patch");
        return exTAP_ApplyPatch(&mheg_off_patch[firmware_id]);
    }
    else {
        TAP_Print("\nRemoving MHEG Off Patch");
        return exTAP_RemovePatch(&mheg_off_patch[firmware_id]);
    }
}


dword exTAP_MHEG_Status()
{
    dword v0,v1;
    dword register_group;


    register_group = 0;
    v0 = *fw_mheg_entry;
    if (v0) {
        v1 = *(dword *)(v0 + 0x8);
        if (v1) {
            register_group = *(dword *)(v1 + 0x34);
        }
    }

    //see  dtt mheg-5 spec. v1.06, section 3.6
    //if register_group == 0 then no mheg is running
    //if register_group == 3 then mheg is running, has claimed <text><red><green><yellow><blue>
    //if register_group == 5 then mheg is running, has claimed group3 + <arrow keys>
    //if register_group == 4 then mheg is running, has claimed group3 + group5 + <number keys>
    return register_group;
}
//---------------------------------------------------------------------
type_firmware fwlist_tap_load[] = {               //the address of the tap load function
    {0x1204, 0x8016ef78},
    {0x1205, 0x8016f0fc},
    {0x1209, 0x8016fc6c},
    {0x1225, 0x80170d5c},
    {0, 0},
};
type_firmware fwlist_tap_load_data0[] = {         //$gp-0x6194 the address of the tap load data0,
    {0x1204, 0x8042d1ec},
    {0x1205, 0x8042d36c},
    {0x1209, 0x804257ac},
    {0x1225, 0x80428f88},
   {0, 0},
};
type_firmware fwlist_tap_load_data1[] = {         //$gp-0x7af0 the address of the tap load data1,
    {0x1204, 0x8042b890},
    {0x1205, 0x8042ba10},
    {0x1209, 0x80423e40},
    {0x1225, 0x804275fc},
    {0, 0},
};
type_firmware fwlist_tap_load_data2[] = {         //$gp-0x7b00 the address of the tap load data2,
    {0x1204, 0x8042b880},
    {0x1205, 0x8042ba00},
    {0x1209, 0x80423e30},
    {0x1225, 0x804275ec},
    {0, 0},
};
// TAP Load function
//
// this function installs a tap
//
dword exTAP_TAP_Load(char *filename)
{
    int ret;

    void *fw_tap_load;
    dword *fw_tap_load_data0;
    dword *fw_tap_load_data1;
    dword *fw_tap_load_data2;

    fw_tap_load = (void *)exTAP_GetFirmwareAddress(fwlist_tap_load);
    fw_tap_load_data0 = (dword *)exTAP_GetFirmwareAddress(fwlist_tap_load_data0);
    fw_tap_load_data1 = (dword *)exTAP_GetFirmwareAddress(fwlist_tap_load_data1);
    fw_tap_load_data2 = (dword *)exTAP_GetFirmwareAddress(fwlist_tap_load_data2);
    if (!fw_tap_load || !fw_tap_load_data0 || !fw_tap_load_data1 || !fw_tap_load_data2) {
        TAP_Print("\nexTAP_TAP_Load(), Unsupported firmware version - %04x", _appl_version);
        return 0;
    }

    *fw_tap_load_data0 = *fw_tap_load_data1;
    ret = exTAP_CallFirmware(fw_tap_load, (dword)filename, 0,0,0);
    *fw_tap_load_data0 = *fw_tap_load_data2;

    return ret;     //the tap load address, 0=fail,
}

//---------------------------------------------------------------------
//
// TAP Shutdown function
//
// this function shuts down the box
//
#define FW_SHUTDOWN_TASK        0xef00          //the shutdown task id

type_firmware fwlist_add_task_queue[] = {       //the address of an add task function
    {0x1209, 0x80005b70},
    {0x1225, 0x80005bb4},
    {0, 0},
};

int exTAP_Shutdown(void)
{
    void *fw_add_task_queue;

    fw_add_task_queue = (void *)exTAP_GetFirmwareAddress(fwlist_add_task_queue);
    if (!fw_add_task_queue) {
        TAP_Print("\nfw_add_task_queue: Unsupported firmware version - %04x", _appl_version);
        return -1;
    }
    exTAP_CallFirmware((void *)fw_add_task_queue, FW_SHUTDOWN_TASK, 0x0, 0,0);       //task_ef00

    return 0;
}

//---------------------------------------------------------------------
//
// Front Panel LED Patch
//
// this function patches the firmware to allow the front panel display to be manipulated
//

type_firmware fwlist_display_func[] = {             //the address of the front panel display function
    {0x1204, 0x800062dc},
    {0x1205, 0x800062dc},
    {0x1209, 0x80006330},
    {0x1225, 0x80006320},
    {0, 0},
};

type_firmware fwlist_display_string[] = {           //the location of the firmware front panel current display string
    {0x1204, 0x8042d364},                           //$gp-0x
    {0x1205, 0x8042d4e4},                           //$gp-0x
    {0x1209, 0x8042592C},                           //$gp+0x9ffc
    {0x1225, 0x80429110},                           //$gp-0x5fe0
    {0, 0},
};

//80006330 27 bd ff d8   addiu $sp, -0x28               # change to: 03 e0 00 08   jr   $ra
//80006334 af b6 00 20   sw    $s6, 0x20($sp)           # change to: 00 00 00 00   nop
type_patch led_patch[] = {
    {//12.04
        0x1204,     //firmware revision
        2,
        {
            0x800062dc, {0x27, 0xbd, 0xff, 0xd8}, {0x03, 0xe0, 0x00, 0x08},
            0x800062e0, {0xaf, 0xb6, 0x00, 0x20}, {0x00, 0x00, 0x00, 0x00},
        }
    },
    {//12.05
        0x1205,     //firmware revision
        2,
        {
            0x800062dc, {0x27, 0xbd, 0xff, 0xd8}, {0x03, 0xe0, 0x00, 0x08},
            0x800062e0, {0xaf, 0xb6, 0x00, 0x20}, {0x00, 0x00, 0x00, 0x00},
        }
    },
    {//12.09
        0x1209,     //firmware revision
        2,
        {
            0x80006330, {0x27, 0xbd, 0xff, 0xd8}, {0x03, 0xe0, 0x00, 0x08},
            0x80006334, {0xaf, 0xb6, 0x00, 0x20}, {0x00, 0x00, 0x00, 0x00},
        }
    },
    {//12.25
        0x1225,     //firmware revision
        2,
        {
            0x80006320, {0x27, 0xbd, 0xff, 0xd8}, {0x03, 0xe0, 0x00, 0x08},
            0x80006324, {0xaf, 0xb6, 0x00, 0x20}, {0x00, 0x00, 0x00, 0x00},
        }
    },
};

//pointless...
int exTAP_LED_GetStatus(void)
{
    return 2;
}

int exTAP_LED_SetControl(int enable)
{
    char *fw_display_string;
    int ret;

    fw_display_string = (char *)exTAP_GetFirmwareAddress(fwlist_display_string);
    if (!fw_display_string) {
        TAP_Print("\nexTAP_LED_SetControl(), Unsupported firmware version - %04x", _appl_version);
        return 0;
    }

    if (enable) {
        TAP_Print("\nApplying LED Patch");
        ret = exTAP_ApplyPatch(&led_patch[firmware_id]);
    }
    else {
        TAP_Print("\nRemoving LED Patch");
        ret = exTAP_RemovePatch(&led_patch[firmware_id]);
        // restore display
        exTAP_LED_SendData(0x1f, fw_display_string);
    }
    return ret;
}

int exTAP_LED_GetControl(void)
{
    int i;

    //check if firmware is already patched
    for (i=0; i<led_patch[firmware_id].count; i++) {
        if (*(dword *)(led_patch[firmware_id].info[i].addr) == *(dword *)(&led_patch[firmware_id].info[i].to)) {
            if (i == (led_patch[firmware_id].count-1)) {
                TAP_Print("\nexTAP_LED_GetControl(), firmware is already patched - 0x%08x",led_patch[firmware_id].info[i].addr);
                return -1;
            }
        }
        else {
            break;
        }
    }

    //check if original firmware matches expected data
    for (i=0; i<led_patch[firmware_id].count; i++) {
        if (*(dword *)(led_patch[firmware_id].info[i].addr) != *(dword *)(&led_patch[firmware_id].info[i].from[i])) {
            TAP_Print("\nexTAP_LED_GetControl(), original firmware does not match expected - 0x%08x",led_patch[firmware_id].info[i].addr);
            return -2;
        }
    }

    return 0;
}


//since the original function has been patched with an 'rts' we need to
//replicate the first 2 instructions, then jump into the firmware function
//at an address offset of 8
static int patched_led_string(void *pFunc)
{
    //cannot use anything that relies on $gp here ..

    __asm__ __volatile__ (
    "or      $08,$00,%0\n"          //or      $t0,$00,pFunc
    "addiu   $08,$08,0x08\n"        //addiu   $t0,$t0,0x08            adjust function entry by 8

    "or      $04,$00,$05\n"         //or      $a0,$00,$a1
    "or      $05,$00,$06\n"         //or      $a1,$00,$a2

    "addiu   $29,$29,0xffd8\n"      //addiu   $sp,$sp,-0x28           first line of pFunc
    "sw      $22,0x0020($29)\n"     //sw      $s6,0x0020($sp)         second line of pFunc

    "jr      $08\n"                 //jr      $t0                     call pFunc(mask, pData)
    :
    : "r"(pFunc));
}

//
// This is a call wrapper for the firmware function displaying
// the specified bytes on the seven segment display of TF5000.
void exTAP_LED_SendData(byte mask, byte *pData)
{
    exTAP_CallFirmware(patched_led_string, led_patch[firmware_id].info[0].addr, mask, (dword)pData, 0);
}

// This function wraps around the call to a firmware function.
// display elements
//
// control mask
//  bit0 : update first 7-segment from pData[0]
//  bit1 : update second 7-segment from pData[1]
//  bit2 : update third 7-segment from pData[2]
//  bit3 : update fourth 7-segment from pData[3]
//  bit4 : update additional signals from pData[4]
//
// pData should point to an array of 5 bytes
// The first four bytes in the pData array represent the
// seven segment + dot indicators from left to right.
// Bit assignment for seven segment + dor indicators:
//
//     b0
//     --
//  b5|  | b1
//     -- b6
//  b4|  | b2
//     -- o b7 (dot)
//     b3
//
// The fifth byte (pData[4]) represents the state of the
// additional signals (colon, upper dot and LEDs).
// Bit assignment in the fifth byte for additional signals:
//                        b2
//  b5  --     --  b0 --  O  --
//  O  |  |   |  | O |  |   |  |
//  O   --     --  b1 --     --
//  b4 |  |   |  | O |  |   |  |
//      --  o  --  o  --  o  --  o
//
//  b0 - upper dot in the colon
//  b1 - lower dot in the colon
//  b2 - upper dot
//  b4 - green LED
//  b5 - amber LED
//

//these characters are pretty poor...   KM TUVW  ek m uvw

byte charMap[] = {0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
                  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
                  0x00,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x80,0x40,0x80,0x08,     //spc,-.
                  0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x27, 0x7f,0x6f,0x08,0x08,0x08,0x08,0x08,0x08,     //0-9

                  0x08,0x77,0x7f,0x39,0x3f,0x79,0x71,0x3d, 0x76,0x06,0x1e,0x76,0x38,0x4f,0x37,0x3f,     //,A-O
                  0x73,0xbf,0x77,0x6d,0x78,0x3e,0x3e,0xbe, 0x76,0x6e,0x5b,0x08,0x08,0x08,0x08,0x08,     //P-Z
                  0x08,0xdc,0x7c,0x58,0x5e,0x7b,0x71,0x6f, 0x74,0x04,0x0e,0x76,0x30,0x4f,0x54,0x5c,     //,a-o
                  0x73,0x67,0x50,0x6d,0x78,0x1c,0x1c,0x9c, 0x76,0x6e,0x5b,0x08,0x08,0x08,0x08,0x08,     //p-z

                  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
                  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
                  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
                  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,

                  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
                  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
                  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
                  0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08, 0x08,0x08,0x08,0x08,0x08,0x08,0x08,0x08,
                 };


int exTAP_LED_GetMap(char ascii)
{
    return charMap[ascii];

}

int exTAP_LED_SetMap(char ascii, byte sevenseg)
{
    charMap[ascii] = sevenseg;
    return 0;
}

//
// This function translates the specified ASCII string
// to a seven-segment character representation.
int exTAP_LED_Translate(const char *pIn, byte *pOut)
{
    int count = 0;

    while (*pIn != 0) {

        *pOut = charMap[*pIn];

        if ((*(pIn + 1) == '.') || (*(pIn + 1) == ',')) {
            // The next character is a dot or a comma, merge it
            // with the current character if the dot is not
            // already in use.
            if (!(*pOut & 0x80)) {
                *pOut |= 0x80;
                pIn++;
            }
        }

        pIn++;
        pOut++;
        count++;
    }
    return count;
}

//-----------------------------------------------------------------------------

void exTAP_GetUTCTime(word *mjd, byte *hour, byte *minute)
{
    *mjd = fw_utc_time->mjd;
    *hour = fw_utc_time->hour;
    *minute = fw_utc_time->minute;
}

//
//?? maybe ...
//
dword exTAP_UTCtoLocalTime(dword utc)
{
    return exTAP_CallFirmware(fw_utctolocal_time, utc,  0,0,0);
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
