#pragma once

#ifndef LOGGER_H
#define LOGGER_H

#include "RLUTIL.h"

enum LOG_COLOR
{
    LOG_INFO,
    LOG_INFO2,
    LOG_WARNING,
    LOG_WARNING2,
    LOG_ERROR,
    LOG_ERROR2,
    LOG_SUCCESS,
    LOG_SUCCESS2
};

class Logger {
public:
    static void info(const char *format, ...);
    static void info2(const char *format, ...);
    static void warn(const char *format, ...);
    static void warn2(const char *format, ...);
    static void error(const char *format, ...);
    static void error2(const char *format, ...);
    static void success(const char *format, ...);
    static void success2(const char *format, ...);
    static void Write(LOG_COLOR logColor, const char *format, ...);
private:
    static void logwrite(int color, const char *format, va_list args);
};

#endif
