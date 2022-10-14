/*
 * @Name       core
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-03
 */


#ifndef INPUTSTREAM_H
#define INPUTSTREAM_H

#include <stdint.h>

extern int open_import_file(char *src_file_name);
extern int read_a_frame(unsigned char *r_byte1,
                                         unsigned char *g_byte1,
                                         unsigned char *b_byte1);
extern int close_import_file();
extern void get_video_info(int64_t *frame_count, int *width1, int *height1);

extern void set_read_callback(void *caller_p, void (*read_callback)(void *caller_p));


#endif // INPUTSTREAM_H
