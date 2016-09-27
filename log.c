#include "log.h"

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

static FILE *_out;
static LogLevel _minLevel;
static const char *_logLevelNamesWithSpacing[] = {"DEBUG",
                                                  "INFO ",
                                                  "WARN ",
                                                  "ERROR",
                                                  "FATAL"};

void log_init(FILE *out, LogLevel minLevel) {
  _out = out;
  _minLevel = minLevel;
}

void log_impl(LogLevel level, const char *file, int lineNumber, ...) {
  if (level < _minLevel) {
    return;
  }

  // log current time
  time_t timer;
  time(&timer);
  struct tm *tmInfo = localtime(&timer);
  char buffer[30];
  strftime(buffer, 25, "%Y-%m-%d %H:%M:%S ", tmInfo);
  fputs(buffer, _out);

  // log level name
  fputs(_logLevelNamesWithSpacing[level], _out);

  // log location
  fprintf(_out,  " %s:%d ", file, lineNumber);

  // log message
  va_list args;
  va_start(args, lineNumber);
  const char *message = va_arg(args, const char*);
  vfprintf(_out, message, args);
  va_end(args);

  // trailing newline
  fputc('\n', _out);
}
