#ifndef  LOG_DEFINED
#define  LOG_DEFINED

#include <stdio.h>

typedef enum {
  LOG_LEVEL_ALWAYS = -1,
  LOG_LEVEL_DEBUG  = 0, 
  LOG_LEVEL_INFO   = 1, 
  LOG_LEVEL_WARN   = 2, 
  LOG_LEVEL_ERROR  = 3, 
  LOG_LEVEL_FATAL  = 4, 
  LOG_LEVEL_NEVER  = 5
} LogLevel;

void log_init(FILE *out, LogLevel minLevel);
void log_impl(LogLevel level, const char *file, int lineNumber, ...);

#define LOG_DEBUG(...) log_impl(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) log_impl(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) log_impl(LOG_LEVEL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...) log_impl(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) do { log_impl(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__); exit(1); } while(0)

#endif /*LOG_DEFINED*/
