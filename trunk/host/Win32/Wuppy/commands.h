#ifndef __COMMANDS_H
#define __COMMANDS_H


#define PUT 0
#define GET 1

extern int quiet;
extern __u32 cmd;
extern char *arg1;
extern char *arg2;
extern __u8 sendDirection;


int do_cancel(usb_dev_handle* fd);
int do_cmd_ready(usb_dev_handle* fd);
int do_cmd_reset(usb_dev_handle* fd);
int do_hdd_size(usb_dev_handle* fd);
int do_hdd_dir(usb_dev_handle* fd, char *path);
int do_hdd_file_put(usb_dev_handle* fd, char *srcPath, char *dstPath);
int do_hdd_file_get(usb_dev_handle* fd, char *srcPath, char *dstPath);
void decode_dir(struct tf_packet *p);
int do_hdd_del(usb_dev_handle* fd, char *path);
int do_hdd_rename(usb_dev_handle* fd, char *srcPath, char *dstPath);
int do_hdd_mkdir(usb_dev_handle* fd, char *path);
int do_cmd_turbo(usb_dev_handle* fd, char *state);
void progressStats(__u64 totalSize, __u64 bytes, time_t startTime);
void finalStats(__u64 bytes, time_t startTime);


#endif __COMMANDS_H
