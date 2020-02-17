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


int file_compare(char *file1, char *file2) {
    /*
    *  Compare two files based on the filenames.
    *  @param       file1   Name of the first file to compare.
    *  @param       file2   Name of the second file to compare.
    *  @returns             0 if the files are equal else the first byte
    *                       where the files differ.
    */
    int f1 = open(file1, O_RDONLY);
    if (f1 == -1) {
        die("Failed to open the file 1.");
    }
    int f2 = open(file2, O_RDONLY);
    if (f2 == -1) {
        close(f1);
        die("Failed to open the file 2.");
    }

    int idx, rd1, rd2, total;
    char f1_buf[FILE_COMPARE_BUFFER_SIZE];
    char f2_buf[FILE_COMPARE_BUFFER_SIZE];
    memset(f1_buf, 0, FILE_COMPARE_BUFFER_SIZE);
    memset(f2_buf, 0, FILE_COMPARE_BUFFER_SIZE);

    while (true) {
        if((rd1 = read(f1, f1_buf, FILE_COMPARE_BUFFER_SIZE)) == -1) {
            close(f1);
            close(f2);
            die("Failed to read file 1.");
        }

        if((rd2 = read(f2, f2_buf, FILE_COMPARE_BUFFER_SIZE)) == -1) {
            close(f1);
            close(f2);
            die("Failed to read file 2.");
        }

        int min = rd1 < rd2 ? rd1 : rd2;

        if (min == 0) {
            break;
        }

        if (rd1 == rd2 && memcmp(f1_buf, f2_buf, min) == 0) {
            total += min;
            memset(f1_buf, 0, FILE_COMPARE_BUFFER_SIZE);
            memset(f2_buf, 0, FILE_COMPARE_BUFFER_SIZE);
        } else {
            for (idx = 0; idx < min; ++idx) {
                if (f1_buf[idx] != f2_buf[idx]) {
                    return total + idx;
                }
            }
            return total + idx;
        }
    }

    return 0;
}
