#include "utils.h"

void llog(const char format_string[], ...) {
    /*
    *  Log a message to STDOUT.  Not signal-safe.
    */

    // Due to limit on buffer size, don't use user input here.
    // Also make sure that llog activations in the code won't
    // make the string too long.
    char msg[LOG_MESSAGE_BUFFER_SIZE];

    va_list args;
    va_start(args, format_string);
    vsprintf(msg, format_string, args);
    va_end(args);

    write(STDOUT_FILENO, msg, strlen(msg));

    return;
}


void die(const char msg[]) {
    /*
    *  Log an error message to STDERR then exit using the errno.
    */

    char errcode_msg[LOG_MESSAGE_BUFFER_SIZE];
    sprintf(errcode_msg, "%s Error code: %d\n", msg, errno);
    write(STDERR_FILENO, errcode_msg, strlen(errcode_msg));
    exit(errno);
}

