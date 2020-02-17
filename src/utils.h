#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


#ifndef UTILS_H
#define UTILS_H

#define LOG_MESSAGE_BUFFER_SIZE 512
#define FILE_COMPARE_BUFFER_SIZE 512

void llog(const char format_string[], ...);
void die(const char msg[]);
int file_compare(char *file1, char *file2);

#endif

