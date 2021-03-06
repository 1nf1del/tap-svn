
/* $Id: usb_io.c,v 1.15 2005/08/26 16:25:33 purbanec Exp $ */

/*

  Copyright (C) 2004 Peter Urbanec <toppy at urbanec.net>

  This file is part of puppy.

  puppy is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  puppy is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with puppy; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

*/

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "usb_io.h"
#include "tf_bytes.h"
#include "crc16.h"
#include "usb.h"

/* The Topfield packet handling is a bit unusual. All data is stored in
 * memory in big endian order, however, just prior to transmission all
 * data is byte swapped.
 *
 * Functions to read and write the memory version of packets are provided
 * in tf_bytes.c
 *
 * Routines here take care of CRC generation, byte swapping and packet
 * transmission.
 */

/* 0 - disable tracing */

/* 1 - show packet headers */

/* 2+ - dump entire packet */
int packet_trace = 0;
int verbose = 0;

/* Swap the odd and even bytes in the buffer, up to count bytes.
 * If count is odd, the last byte remains unafected.
 */
void byte_swap(__u8 * d, int count)
{
    int i;

    for(i = 0; i < (count & ~1); i += 2)
    {
        __u8 t = d[i];

        d[i] = d[i + 1];
        d[i + 1] = t;
    }
}

/* Byte swap an incoming packet. */
void swap_in_packet(struct tf_packet *packet)
{
    int size = (get_u16_raw(packet) + 1) & ~1;

    if(size > MAXIMUM_PACKET_SIZE)
    {
        size = MAXIMUM_PACKET_SIZE;
    };

    byte_swap((__u8 *) packet, size);
}

/* Byte swap an outgoing packet. */
void swap_out_packet(struct tf_packet *packet)
{
    int size = (get_u16(packet) + 1) & ~1;

    byte_swap((__u8 *) packet, size);
}

static __u8 cancel_packet[8] = {
    0x08, 0x00, 0x40, 0x01, 0x00, 0x00, 0x03, 0x00
};

static __u8 success_packet[8] = {
    0x08, 0x00, 0x81, 0xc1, 0x00, 0x00, 0x02, 0x00
};

/* Optimised packet handling to reduce overhead during bulk file transfers. */
int send_cancel(usb_dev_handle* fd)
{
    return usb_bulk_write(fd, 0x01, cancel_packet, 8, TF_PROTOCOL_TIMEOUT);
}

int send_success(usb_dev_handle* fd)
{
    return usb_bulk_write(fd, 0x01, success_packet, 8, TF_PROTOCOL_TIMEOUT);
}

int send_cmd_ready(usb_dev_handle* fd)
{
    struct tf_packet req;

    put_u16(&req.length, 8);
    put_u32(&req.cmd, CMD_READY);
    return send_tf_packet(fd, &req);
}

int send_cmd_reset(usb_dev_handle* fd)
{
    struct tf_packet req;

    put_u16(&req.length, 8);
    put_u32(&req.cmd, CMD_RESET);
    return send_tf_packet(fd, &req);
}

int send_cmd_turbo(usb_dev_handle* fd, int turbo_on)
{
    struct tf_packet req;

    put_u16(&req.length, 12);
    put_u32(&req.cmd, CMD_TURBO);
    put_u32(&req.data, turbo_on);
    return send_tf_packet(fd, &req);
}

int send_cmd_hdd_size(usb_dev_handle* fd)
{
    struct tf_packet req;

    put_u16(&req.length, 8);
    put_u32(&req.cmd, CMD_HDD_SIZE);
    return send_tf_packet(fd, &req);
}

__u16 get_crc(struct tf_packet * packet)
{
    return crc16_ansi(&(packet->cmd), get_u16(&packet->length) - 4);
}

int send_cmd_hdd_dir(usb_dev_handle* fd, char *path)
{
    struct tf_packet req;
    __u16 packetSize;
    int pathLen = strlen(path) + 1;

    if((PACKET_HEAD_SIZE + pathLen) >= MAXIMUM_PACKET_SIZE)
    {
        fprintf(stderr, "ERROR: Path is too long.\n");
        return -1;
    }

    packetSize = PACKET_HEAD_SIZE + pathLen;
    packetSize = (packetSize + 1) & ~1;
    put_u16(&req.length, packetSize);
    put_u32(&req.cmd, CMD_HDD_DIR);
    strcpy((char *) req.data, path);
    return send_tf_packet(fd, &req);
}

int send_cmd_hdd_file_send(usb_dev_handle* fd, __u8 dir, char *path)
{
    struct tf_packet req;
    __u16 packetSize;
    int pathLen = strlen(path) + 1;

    if((PACKET_HEAD_SIZE + 1 + 2 + pathLen) >= MAXIMUM_PACKET_SIZE)
    {
        fprintf(stderr, "ERROR: Path is too long.\n");
        return -1;
    }

    packetSize = PACKET_HEAD_SIZE + 1 + 2 + pathLen;
    packetSize = (packetSize + 1) & ~1;
    put_u16(&req.length, packetSize);
    put_u32(&req.cmd, CMD_HDD_FILE_SEND);
    req.data[0] = dir;
    put_u16(&req.data[1], pathLen);
    strcpy((char *) &req.data[3], path);
    return send_tf_packet(fd, &req);
}

int send_cmd_hdd_del(usb_dev_handle* fd, char *path)
{
    struct tf_packet req;
    __u16 packetSize;
    int pathLen = strlen(path) + 1;

    if((PACKET_HEAD_SIZE + pathLen) >= MAXIMUM_PACKET_SIZE)
    {
        fprintf(stderr, "ERROR: Path is too long.\n");
        return -1;
    }

    packetSize = PACKET_HEAD_SIZE + pathLen;
    packetSize = (packetSize + 1) & ~1;
    put_u16(&req.length, packetSize);
    put_u32(&req.cmd, CMD_HDD_DEL);
    strcpy((char *) req.data, path);
    return send_tf_packet(fd, &req);
}

int send_cmd_hdd_rename(usb_dev_handle* fd, char *src, char *dst)
{
    struct tf_packet req;
    __u16 packetSize;
    __u16 srcLen = strlen(src) + 1;
    __u16 dstLen = strlen(dst) + 1;

    if((PACKET_HEAD_SIZE + 2 + srcLen + 2 + dstLen) >= MAXIMUM_PACKET_SIZE)
    {
        fprintf(stderr,
                "ERROR: Combination of source and destination paths is too long.\n");
        return -1;
    }

    packetSize = PACKET_HEAD_SIZE + 2 + srcLen + 2 + dstLen;
    packetSize = (packetSize + 1) & ~1;
    put_u16(&req.length, packetSize);
    put_u32(&req.cmd, CMD_HDD_RENAME);
    put_u16(&req.data[0], srcLen);
    strcpy((char *) &req.data[2], src);
    put_u16(&req.data[2 + srcLen], dstLen);
    strcpy((char *) &req.data[2 + srcLen + 2], dst);
    return send_tf_packet(fd, &req);
}

int send_cmd_hdd_create_dir(usb_dev_handle* fd, char *path)
{
    struct tf_packet req;
    __u16 packetSize;
    __u16 pathLen = strlen(path) + 1;

    if((PACKET_HEAD_SIZE + 2 + pathLen) >= MAXIMUM_PACKET_SIZE)
    {
        fprintf(stderr, "ERROR: Path is too long.\n");
        return -1;
    }

    packetSize = PACKET_HEAD_SIZE + 2 + pathLen;
    packetSize = (packetSize + 1) & ~1;
    put_u16(&req.length, packetSize);
    put_u32(&req.cmd, CMD_HDD_CREATE_DIR);
    put_u16(&req.data[0], pathLen);
    strcpy((char *) &req.data[2], path);
    return send_tf_packet(fd, &req);
}

void print_packet(struct tf_packet *packet, char *prefix)
{
    int i;
    __u8 *d = (__u8 *) packet;
    __u16 pl = get_u16(&packet->length);

    switch (packet_trace)
    {
        case 0:
            /* Do nothing */
            break;

        case 1:
            fprintf(stderr, "%s", prefix);
            for(i = 0; i < 8; ++i)
            {
                fprintf(stderr, " %02x", d[i]);
            }
            fprintf(stderr, "\n");
            break;

        default:
            fprintf(stderr, "%s", prefix);
            for(i = 0; i < pl; ++i)
            {
                fprintf(stderr, " %02x", d[i]);
                if(31 == (i % 32))
                    fprintf(stderr, "\n%s", prefix);
            }
            fprintf(stderr, "\n");

            fprintf(stderr, "%s", prefix);
            for(i = 0; i < pl; ++i)
            {
                if(isalnum(d[i]) || ispunct(d[i]))
                    fprintf(stderr, "%c", d[i]);
                else
                    fprintf(stderr, ".");
                if(79 == (i % 80))
                    fprintf(stderr, "\n%s", prefix);
            }
            fprintf(stderr, "\n");
            break;
    }
}

/* Given a Topfield protocol packet, this function will calculate the required
 * CRC and send the packet out over a bulk pipe. */
int send_tf_packet(usb_dev_handle* fd, struct tf_packet *packet)
{
    unsigned int pl = get_u16(&packet->length);
    size_t byte_count = (pl + 1) & ~1;

    put_u16(&packet->crc, get_crc(packet));
    print_packet(packet, "OUT>");
    swap_out_packet(packet);
    return usb_bulk_write(fd, 0x01, (__u8 *) packet, byte_count,
                          TF_PROTOCOL_TIMEOUT);
}

/* Receive a Topfield protocol packet.
 * Returns a negative number if the packet read failed for some reason.
 */
int get_tf_packet(usb_dev_handle* fd, struct tf_packet * packet)
{
    __u8 *buf = (__u8 *) packet;
    int r;

    trace(3, fprintf(stderr, "get_tf_packet\n"));

    r = usb_bulk_read(fd, 0x82, buf, MAXIMUM_PACKET_SIZE,
                      TF_PROTOCOL_TIMEOUT);
    if(r < 0)
    {
        fprintf(stderr, "USB read error: %s\n", strerror(errno));
        return -1;
    }

    if(r < PACKET_HEAD_SIZE)
    {
        fprintf(stderr, "Short read. %d bytes\n", r);
        return -1;
    }

    /* Send SUCCESS as soon as we see a data transfer packet */
    if(DATA_HDD_FILE_DATA == get_u32_raw(&packet->cmd))
    {
        send_success(fd);
    }

    swap_in_packet(packet);

    {
        __u16 crc;
        __u16 calc_crc;
        __u16 len = get_u16(&packet->length);

        if(len < PACKET_HEAD_SIZE)
        {
            fprintf(stderr, "Invalid packet length %04x\n", len);
            return -1;
        }

        crc = get_u16(&packet->crc);
        calc_crc = get_crc(packet);

        /* Complain about CRC mismatch */
        if(crc != calc_crc)
        {
            fprintf(stderr, "WARNING: Packet CRC %04x, expected %04x\n", crc,
                    calc_crc);
        }
    }

    print_packet(packet, " IN<");
    return r;
}

/* Linux usbdevfs has a limit of one page size per read/write.
   4096 is the most portable maximum we can do for now.
*/

#define MAX_READ	4096
#define MAX_WRITE	4096


int read_device_descriptor(const usb_dev_handle* fd,
                               struct usb_device_descriptor * desc)
{
    int r = read(fd, desc, USB_DT_DEVICE_SIZE);

    if(r != USB_DT_DEVICE_SIZE)
    {
        fprintf(stderr, "Can not read device descriptor\n");
        return -1;
    }
    return r;
}

int read_config_descriptor(const usb_dev_handle* fd,
                               struct usb_config_descriptor * desc)
{
    int r = read(fd, desc, USB_DT_CONFIG_SIZE);

    if(r != USB_DT_CONFIG_SIZE)
    {
        fprintf(stderr, "Can not read config descriptor\n");
        return -1;
    }

    desc->wTotalLength = get_u16(&desc->wTotalLength);

    r = discard_extra_desc_data(fd, (struct usb_descriptor_header *) desc,
                                USB_DT_CONFIG_SIZE);
    return r;
}

int discard_extra_desc_data(usb_dev_handle* fd, struct usb_descriptor_header * desc,
                                size_t descSize)
{
    int r;

    /* If the descriptor is bigger than standard, read remaining data and discard */
    if(desc->bLength > descSize)
    {
        __u8 *extra;
        size_t extraSize = desc->bLength - descSize;

        fprintf(stderr, "Discarding %d bytes from oversized descriptor\n",
                extraSize);

        extra = malloc(extraSize);
        if(extra != NULL)
        {
            r = read(fd, extra, extraSize);
            free(extra);
            if(r != extraSize)
            {
                fprintf(stderr,
                        "Read %d instead of %d for oversized descriptor\n", r,
                        extraSize);
                return -1;
            }
        }
        else
        {
            fprintf(stderr,
                    "Failed to allocate %d bytes for oversized descriptor\n",
                    extraSize);
            return -1;
        }
    }
    return 0;
}

void print_device_descriptor(struct usb_device_descriptor *desc)
{
    fprintf(stderr, "\nusb_device_descriptor 0x%p\n", (void *) desc);
    fprintf(stderr, "bLength.................0x%02x\n", desc->bLength);
    fprintf(stderr, "bDescriptorType.........0x%02x\n",
            desc->bDescriptorType);
    fprintf(stderr, "bcdUSB..................0x%04x\n", desc->bcdUSB);
    fprintf(stderr, "bDeviceClass............0x%02x\n", desc->bDeviceClass);
    fprintf(stderr, "bDeviceSubClass.........0x%02x\n",
            desc->bDeviceSubClass);
    fprintf(stderr, "bDeviceProtocol.........0x%02x\n",
            desc->bDeviceProtocol);
    fprintf(stderr, "bMaxPacketSize0.........0x%02x\n",
            desc->bMaxPacketSize0);
    fprintf(stderr, "idVendor................0x%04x\n", desc->idVendor);
    fprintf(stderr, "idProduct...............0x%04x\n", desc->idProduct);
    fprintf(stderr, "bcdDevice...............0x%04x\n", desc->bcdDevice);
    fprintf(stderr, "iManufacturer...........0x%02x\n", desc->iManufacturer);
    fprintf(stderr, "iProduct................0x%02x\n", desc->iProduct);
    fprintf(stderr, "iSerialNumber...........0x%02x\n", desc->iSerialNumber);
    fprintf(stderr, "bNumConfigurations......0x%02x\n\n",
            desc->bNumConfigurations);
}

void print_config_descriptor(struct usb_config_descriptor *desc)
{
    fprintf(stderr, "\nusb_config_descriptor 0x%p\n", (void *) desc);
    fprintf(stderr, "bLength.................0x%02x\n", desc->bLength);
    fprintf(stderr, "bDescriptorType.........0x%02x\n",
            desc->bDescriptorType);
    fprintf(stderr, "wTotalLength............0x%04x\n", desc->wTotalLength);
    fprintf(stderr, "bNumInterfaces..........0x%02x\n", desc->bNumInterfaces);
    fprintf(stderr, "bConfigurationValue.....0x%02x\n",
            desc->bConfigurationValue);
    fprintf(stderr, "iConfiguration..........0x%02x\n", desc->iConfiguration);
    fprintf(stderr, "bmAttributes............0x%02x\n", desc->bmAttributes);
}

char *decode_error(struct tf_packet *packet)
{
    __u32 ecode = get_u32(packet->data);

    switch (ecode)
    {
        case 1:
            return "CRC error";
            break;

        case 2:
        case 4:
            return "Unknown command";
            break;

        case 3:
            return "Invalid command";
            break;

        case 5:
            return "Invalid block size";
            break;

        case 6:
            return "Unknown error while running";
            break;

        case 7:
            return "Memory is full";
            break;

        default:
            return "Unknown error or all your base are belong to us";
    }
}
