#include "atom.h"
#include "builtin.h"
#include "io.h"
#include "lang.h"
#include "log.h"
#include "strings.h"
#include "symbol.h"

#define TRY(x, msg) if (x) { LOG_ERROR(msg); return 1;}

int lang_init(Jay *jay) {
  TRY(strings_init(jay), "string init error");
  TRY(io_init(jay), "io init error");
  TRY(atoms_init(jay), "atom init error");
  TRY(symbols_init(jay), "symbol init error");
  TRY(builtins_init(jay), "builtin init error");
  return 0;
}

void lang_free(Jay *jay) {
  builtins_free(jay);
  symbols_free(jay);
  atoms_free(jay);
  io_free(jay);
  strings_free(jay);
}
