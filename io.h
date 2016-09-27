#ifndef  IO_DEFINED
#define  IO_DEFINED

#include "lang.h"

typedef enum {
  TOKEN_POPEN, TOKEN_PCLOSE, TOKEN_STRING, TOKEN_NUMBER, TOKEN_ID, TOKEN_EOF
} TokenType;

typedef struct {
  TokenType type;
  const char *val;
} Token;

int io_init(Jay*);
void io_free(Jay*);

Token io_read(Jay*);

#endif /*IO_DEFINED*/
