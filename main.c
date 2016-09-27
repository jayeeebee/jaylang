#include "builtin.h"
#include "io.h"
#include "lang.h"
#include "log.h"

static void _repl(Jay *jay) {
  while (1) {
    if (jay->isInteractive) {
      fputs("> ", jay->out);
    }
    Token token = io_read(jay); 
    if (token.type == TOKEN_EOF) {
      break;
    }
    AtomId out = eval(jay, read_token(jay, token));
    if (jay->isInteractive) {
      print_atom(jay, out);
    }
  }
}

int main(int argc, const char **argv) {
  log_init(stderr, LOG_LEVEL_ALWAYS);
  Jay jay = {.argc = argc, .argv = argv};
  if (lang_init(&jay)) {
    LOG_ERROR("error while inniting");
    lang_free(&jay);
    return 1;
  } else {
    _repl(&jay);
    lang_free(&jay);
    return 0;
  }
}
