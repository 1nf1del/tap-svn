/* $Id$
*
* Copyright (c) 2004-2005 Peter Urbanec. All Rights Reserved.
*
*  This file is part of the TAPs for Topfield PVRs project.
*      http://developer.berlios.de/projects/tap/
*
*  This is free software; you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation; either version 2 of the License, or
*  (at your option) any later version.
*
*  The software is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this software; if not, write to the Free Software
*  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/* Monitor incoming USB packets.
*
* This is very handy when analysing USB file transfer problems.
*
* This TAP will intercept an incoming USB packet, output some basic
* information about the packet to the serial port console and then
* pass the packet back to the firmware.
*
* This TAP never exits. You will need to reboot the Toppy to quit it.
*
*/

#include <tap.h>

TAP_ID(0x80818242);
TAP_AUTHOR_NAME("peteru");
TAP_PROGRAM_NAME("USB packet monitor.");
TAP_DESCRIPTION("$Id$");
TAP_ETCINFO(__DATE__);

volatile byte received = FALSE;
static byte *buffer;

#define SIZE_BUF (0xffff)
#define SIZE_HEAD (16)

word ReadCallback(word size);
const char *decode_cmd(const dword cmd);

word get_u16(const void *addr)
{
    const byte *b = addr;

    return ((b[0] << 8) | (b[1] << 0));
}

dword get_u32(const void *addr)
{
    const byte *b = addr;

    return (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | (b[3]);
}

word ReadCallback(word size)
{
    int i;
    int head_size = (size < SIZE_HEAD) ? size : SIZE_HEAD;
    word packet_size = get_u16(buffer);
    word packet_crc = get_u16(&buffer[2]);
    dword packet_cmd = get_u32(&buffer[4]);

    TAP_Print("\r\nCallBack size 0x%04x (%d)\r\n", size, size);
    TAP_Print("Packet size   0x%04x (%d)\r\n", packet_size, packet_size);
    TAP_Print("Packet CRC    0x%04x (%d)\r\n", packet_crc, packet_crc);
    TAP_Print("Packet cmd    0x%08x (%s)\r\n", packet_cmd, decode_cmd(packet_cmd));
    TAP_Print("Packet head  ");
    for(i = 0; i < head_size; i++)
    {
        TAP_Print(" %02x", buffer[i]);
    }
    TAP_Print("\r\n");

    received = TRUE;
    return 1;
}

dword TAP_EventHandler(word event, dword param1, dword param2)
{
    if( received == TRUE )
    {
        received = FALSE;
        TAP_Usb_PacketRead(buffer, SIZE_BUF, ReadCallback);
    }
    return param1;
}

int TAP_Main(void)
{
    buffer = malloc(SIZE_BUF);
    TAP_Usb_PacketRead(buffer, SIZE_BUF, ReadCallback);
    return 1;
}

const char *decode_cmd(const dword cmd)
{
    switch(cmd)
    {
    case 0x1:
        return "USB_Fail";
        break;

    case 0x2:
        return "USB_Success";
        break;

    case 0x3:
        return "USB_Cancel";
        break;

    case 0x100:
        return "USB_CmdReady";
        break;

    case 0x101:
        return "USB_CmdReset";
        break;

    case 0x102:
        return "USB_CmdTurbo";
        break;

    case 0x1000:
        return "USB_CmdHddSize";
        break;

    case 0x1001:
        return "USB_DataHddSize";
        break;

    case 0x1002:
        return "USB_CmdHddDir";
        break;

    case 0x1003:
        return "USB_DataHddDir";
        break;

    case 0x1004:
        return "USB_DataHddDirEnd";
        break;

    case 0x1005:
        return "USB_CmdHddDel";
        break;

    case 0x1006:
        return "USB_CmdHddRename";
        break;

    case 0x1007:
        return "USB_DataHddCreateDir";
        break;

    case 0x1008:
        return "USB_CmdHddFileSend";
        break;

    case 0x1009:
        return "USB_DataHddFileStart";
        break;

    case 0x100a:
        return "USB_CmdHddFileData";
        break;

    case 0x100b:
        return "USB_DataHddFileEnd";
        break;
    }
    return "";
}

