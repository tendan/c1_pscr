#ifndef C1_LOGGER_H
#define C1_LOGGER_H

#include <sys/syslog.h>

#define LOG_LEVEL_DEBUG   0
#define LOG_LEVEL_INFO    1
#define LOG_LEVEL_WARN    2
#define LOG_LEVEL_ERROR   3

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

enum LogLevel {
    LEVEL_DEBUG = 0,
    LEVEL_INFO = 1,
    LEVEL_WARN = 2,
    LEVEL_ERROR = 3
};

void logger_init(const char *ident);

void logger_cleanup(void);

void log_message(enum LogLevel log_level, const char *fmt, ...);

#endif /* C1_LOGGER_H */
