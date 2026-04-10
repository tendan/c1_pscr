#include "logger.h"

#include <stdarg.h>
#include <stdio.h>

void logger_init(const char *ident)
{
    /* LOG_PID — dołącz PID do każdego wpisu
       LOG_CONS — fallback na konsolę jeśli syslog niedostępny
       LOG_NDELAY — otwórz połączenie natychmiast */
    openlog(ident, LOG_PID | LOG_CONS | LOG_NDELAY, LOG_DAEMON);
}

void logger_cleanup(void)
{
    closelog();
}

static void logger_debug(const char *fmt, va_list args)
{
    vsyslog(LOG_DEBUG, fmt, args);
}

static void logger_info(const char *fmt, va_list args)
{
    vsyslog(LOG_INFO, fmt, args);
}

static void logger_warn(const char *fmt, va_list args)
{
    vsyslog(LOG_WARNING, fmt, args);
}

static void logger_error(const char *fmt, va_list args)
{
    vsyslog(LOG_ERR, fmt, args);
}

void log_message(enum LogLevel log_level, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    switch (log_level) {
#if LOG_LEVEL <= LOG_LEVEL_INFO
        case LEVEL_INFO:
            logger_info(fmt, args);
            break;
#endif
#if LOG_LEVEL <= LOG_LEVEL_WARN
        case LEVEL_WARN:
            logger_warn(fmt, args);
            break;
#endif
        case LEVEL_ERROR:
            logger_error(fmt, args);
            break;
#if LOG_LEVEL <= LOG_LEVEL_DEBUG
        case LEVEL_DEBUG:
        default:
            logger_debug(fmt, args);
            break;
#endif
    }
    va_end(args);
}
