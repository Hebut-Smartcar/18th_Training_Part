/*
 * @Name       core
 * @Author    Robert Zhang
 * @E-mail    zhangzhiguo1207@163.com
 * @Date       2019-08-03
 */


#ifndef OUTPUTSTREAM_H
#define OUTPUTSTREAM_H


extern int open_media_file(const char* file_name, int width, int height);
extern int write_media_file(unsigned char *r_byte,
                                                unsigned char *g_byte,
                                                unsigned char *b_byte);
extern int close_media_file();

#endif // OUTPUTSTREAM_H
