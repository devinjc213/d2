// Thanks to Travis Vroman and his Kohi engine
// playlist for this logger setup
// https://www.youtube.com/watch?v=l9e8PJskYnI&list=PLv8Ddw9K0JPg1BEO-RS-0MYs423cvLVtj&index=5
#include "logger.h"
#include "asserts.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

void report_assertion_failure(const char* expression,
                              const char* message,
                              const char* file,
                              int line) {
    log_output(
        LOG_LEVEL_FATAL,
        "Assertion failure: %s, message: '%s', in file: %s, line: %d\n",
        expression, message, file, line);
}

int initialize_logging() {

    return 1;
}

void shutdown_logging() {

}


void log_output(log_level level, const char* message, ...) {
    const char* level_strings[6] = {"[FATAL]: ",
                                    "[ERROR]: ",
                                    "[WARN]: ",
                                    "[INFO]: ",
                                    "[DEBUG]: ",
                                    "[TRACE]: "};
    int is_error;
    if (level < 2) {
      is_error = 1;
    } else {
      is_error = 0;
    }
    printf("Is Error: %i\n", is_error);

    char out_message[32000];
    memset(out_message, 0, sizeof(out_message));
    
    va_list arg_ptr;
    va_start(arg_ptr, message);
    vsnprintf(out_message, 32000, message, arg_ptr);
    va_end(arg_ptr);

    char out_message_write[32000];
    sprintf(out_message_write, "%s%s\n", level_strings[level], out_message);

    printf("%s", out_message_write);
}
