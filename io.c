#include "io.h"
#include "log.h"

int io_init(Jay *jay) {
  if (jay->argc == 1) {
    jay->in = stdin;
    jay->out = stdout;
    jay->isInteractive = 1;
  } else if (jay->argc == 3) {
    jay->in = fopen(jay->argv[1], "r");
    if (!jay->in) {
      LOG_ERROR("error while opening infile %s", jay->argv[1]);
      return 1;
    }
    jay->out = fopen(jay->argv[2], "w");
    if (!jay->out) {
      LOG_ERROR("error while opening outfile %s", jay->argv[2]);
      return 1;
    }
    jay->isInteractive = 0;
  } else {
    LOG_ERROR("usage: ./%s <infile> <outfile> OR ./%s", jay->argv[0], jay->argv[0]);
    return 1;
  }

  jay->bufferMaxSize = BUFFER_MAX_SIZE;
  jay->bufferCurrSize = BUFFER_MAX_SIZE;
  jay->buffer = malloc(jay->bufferMaxSize + 1); // room for trailing \0 on identifiers
  if (!(jay->buffer)) {
    LOG_ERROR("malloc returned nothing");
    return 1;
  }
  jay->bufferCurr = BUFFER_MAX_SIZE;
  jay->bufferNext = BUFFER_MAX_SIZE;
  return 0;
}

void io_free(Jay *jay) {
  if (jay->buffer) {
    free(jay->buffer);
  }
  if (!jay->isInteractive) {
    if (jay->in) {
      fclose(jay->in);
    }
    if (jay->out) {
      fclose(jay->out);
    }
  }
}

static size_t _read_chars(Jay *jay, size_t length) {
  size_t got;
  if (jay->isInteractive) {
    fgets(jay->buffer + length, jay->bufferMaxSize - length + 1, jay->in);
    for (got = 0; jay->buffer[got + length]; got++) {
    }
  } else {
    got = fread(jay->buffer + length, sizeof(char), jay->bufferMaxSize - length, jay->in);
  }
  return got;
}

static void _read_into_buffer(Jay *jay) {
  if (jay->bufferCurr == 0 && jay->bufferNext >= jay->bufferMaxSize) {
    LOG_FATAL("identifier can be at most %d characters long", jay->bufferMaxSize);
  }
  size_t length = jay->bufferNext - jay->bufferCurr;
  if (length && jay->bufferCurr) {
    memmove(jay->buffer, jay->buffer + jay->bufferCurr, length);
  }
  size_t got = _read_chars(jay, length);
  jay->bufferNext = length;
  jay->bufferCurr = 0;
  if (got == 0) {
    jay->buffer[length] = EOF;
    jay->bufferCurrSize = length + 1;
  } else {
    jay->bufferCurrSize = length + got;
  }
}

static int _getchar(Jay *jay) {
  if (jay->bufferNext >= jay->bufferCurrSize) {
    _read_into_buffer(jay);
  }
  int c = jay->buffer[jay->bufferNext];
  jay->bufferNext++;
  return c;
}

static void _returnchar(Jay *jay) {
 jay->bufferNext--;
}

static TokenType _token_string(Jay *jay) {
  jay->bufferCurr++;
  while (_getchar(jay) != '\"');
  jay->buffer[jay->bufferNext - 1] = '\0';
  return TOKEN_STRING;
}

static TokenType _token_number(Jay *jay) {
  while (isdigit(_getchar(jay)));
  _returnchar(jay);
  return TOKEN_NUMBER;
}

static int _isid(int c) {
  return !isspace(c) && c != '(' && c != ')' && isprint(c);
}

static TokenType _token_id(Jay *jay) {
  while (_isid(_getchar(jay)));
  _returnchar(jay);
  return TOKEN_ID;
}

static TokenType _token_next(Jay *jay) {
  int c = _getchar(jay);
  if (c == EOF) {
    return TOKEN_EOF;
  } else if (c == '(') {
    return TOKEN_POPEN;
  } else if (c == ')') {
    return TOKEN_PCLOSE;
  } else if (c == '\"') {
    return _token_string(jay);
  } else if (isdigit(c)) {
    return _token_number(jay);
  } else if (isspace(c)) {
    jay->bufferCurr++;
    return _token_next(jay);
  } else {
    return _token_id(jay);
  }
}

Token io_read(Jay *jay) {
  if (jay->bufferCurr != jay->bufferNext) {
    jay->buffer[jay->bufferNext] = jay->bufferTemp;
    jay->bufferCurr = jay->bufferNext;
  }
  TokenType tokenType = _token_next(jay);
  jay->bufferTemp = jay->buffer[jay->bufferNext];
  jay->buffer[jay->bufferNext] = '\0';
  Token token = {
    .type = tokenType,
    .val = jay->buffer + jay->bufferCurr
  };
  return token;
}

