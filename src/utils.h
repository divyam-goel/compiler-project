#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#ifndef UTILS_H

#define LOG_MESSAGE_BUFFER_SIZE 512

void llog(const char format_string[], ...);
void die(const char msg[]);

#endif

