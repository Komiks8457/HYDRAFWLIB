#include "Debug.h"

#include <cstdio>
#include <cstdarg>
#include <ctime>

#if 0
static LogHandlerFn logCallbackFn;
#else
#define logCallbackFn (*(LogHandlerFn*)0x01262a40)
#endif

#if 0
static AssertHandlerFn assertCallbackFn
#else
#define assertCallbackFn (*(AssertHandlerFn*)0x01262a78)
#endif


void Put(const char *fmt, ...) {
#ifdef CONFIG_DEBUG_CONSOLE
    time_t rawtime;
    struct tm *timeinfo;
    wchar_t buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    wcsftime(buffer, sizeof(buffer), L"[%H:%M] ", timeinfo);
    va_list args;
            va_start(args, fmt);
    wprintf(buffer);
    vprintf(fmt, args);
    putchar('\n');
#endif
}

void PutDump(const char *fmt, ...) {
    char buffer[1024] = {0};

    if (!logCallbackFn)
        return;

    va_list args;
            va_start(args, fmt);

    int count = vsnprintf_s(buffer, sizeof(buffer), sizeof(buffer) - 1, fmt, args);

    if ((count < 0) || (0x3ff <= count)) {
        buffer[1023] = '\0';
    }

    logCallbackFn(buffer);
}

void SetPutDumpHandler(LogHandlerFn fn) {
    logCallbackFn = fn;
}

void SetAssertHandler(AssertHandlerFn fn) {
    assertCallbackFn = fn;
}

