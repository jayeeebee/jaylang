#include "atom.h"
#include "builtin.h"
#include "io.h"
#include "lang.h"
#include "log.h"
#include "strings.h"
#include "symbol.h"

static AtomId _init(Jay *jay, const char *name, BuiltinFn builtinFn) {
  AtomId atom = atom_intern_builtin(jay, builtinFn);
  symbol_intern(jay, string_intern(jay, name), atom);
  return atom;
}

int builtins_init(Jay *jay) {
  _init(jay, "print", print);
  _init(jay, "read", read);
  _init(jay, "'", quote);
  _init(jay, "eval", eval);
  _init(jay, "+", add);
  _init(jay, "-", sub);
  _init(jay, "*", mul);
  _init(jay, "/", divide);
  _init(jay, "==", equal);
  _init(jay, "while", whileFn);
  _init(jay, "if", ifFn);
  _init(jay, "or", orFn);
  _init(jay, "and", andFn);
  _init(jay, "<", lt);
  _init(jay, ">", gt);
  _init(jay, "<=", lte);
  _init(jay, ">=", gte);
  _init(jay, "set", set);
  _init(jay, "quit", quit);
  _init(jay, "defnc", defnc);
  return 0;
}

void builtins_free(Jay *jay) {
}

AtomId add(Jay *jay, AtomId cons) {
  double sum = 0;
  while (!is_nil(jay, cons)) {
    sum += atom_number(jay, eval_car(jay, cons));
    cons = atom_cdr(jay, cons);
  }
  return atom_intern_number(jay, sum);
}

AtomId mul(Jay *jay, AtomId cons) {
  double tot = 1;
  while (!is_nil(jay, cons)) {
    tot *= atom_number(jay, eval_car(jay, cons));
    cons = atom_cdr(jay, cons);
  }
  return atom_intern_number(jay, tot);
}

AtomId sub(Jay *jay, AtomId cons) {
  double diff = atom_number(jay, eval_car(jay, cons));
  while (!is_nil(jay, (cons = atom_cdr(jay, cons)))) {
    diff -= atom_number(jay, eval_car(jay, cons));
  }
  return atom_intern_number(jay, diff);
}

AtomId divide(Jay *jay, AtomId cons) {
  double val = atom_number(jay, eval_car(jay, cons));
  while (!is_nil(jay, (cons = atom_cdr(jay, cons)))) {
    val /= atom_number(jay, eval_car(jay, cons));
  }
  return atom_intern_number(jay, val);
}

AtomId quote(Jay *jay, AtomId args) {
  return atom_car(jay, args);
}

AtomId whileFn(Jay *jay, AtomId args) {
  AtomId out = jay->nil;
  while (!is_nil(jay, eval_car(jay, args))) {
    AtomId instructions = atom_cdr(jay, args);
    while (!is_nil(jay, instructions)) {
      out = eval_car(jay, instructions);
      instructions = atom_cdr(jay, instructions);
    }
  }
  return out;
}

AtomId ifFn(Jay *jay, AtomId args) {
  AtomId found = eval_car(jay, args);
  AtomId out = atom_cdr(jay, args);
  if (is_nil(jay, found)) {
    out = atom_cdr(jay, out);
  }
  if (is_nil(jay, out)) {
    return jay->nil;
  } else {
    return eval_car(jay, out);
  }
}

AtomId orFn(Jay *jay, AtomId args) {
  while (!is_nil(jay, args)) {
    AtomId found = eval_car(jay, args);
    if (!is_nil(jay, found)) {
      return found;
    }
    args = atom_cdr(jay, args);
  }
  return jay->nil;
}

AtomId andFn(Jay *jay, AtomId args) {
  AtomId found = jay->t;
  while (!is_nil(jay, args)) {
    found = eval_car(jay, args);
    if (is_nil(jay, found)) {
      return jay->nil;
    }
    args = atom_cdr(jay, args);
  }
  return found;
}

AtomId lt(Jay *jay, AtomId args) {
  AtomId lhs = eval_car(jay, args);
  AtomId rhs = eval_car(jay, atom_cdr(jay, args));
  return atom_number(jay, lhs) < atom_number(jay, rhs) ? jay->t : jay->nil;
}

AtomId gt(Jay *jay, AtomId args) {
  AtomId lhs = eval_car(jay, args);
  AtomId rhs = eval_car(jay, atom_cdr(jay, args));
  return atom_number(jay, lhs) > atom_number(jay, rhs) ? jay->t : jay->nil;
}

AtomId lte(Jay *jay, AtomId args) {
  AtomId lhs = eval_car(jay, args);
  AtomId rhs = eval_car(jay, atom_cdr(jay, args));
  return atom_number(jay, lhs) <= atom_number(jay, rhs) ? jay->t : jay->nil;
}

AtomId gte(Jay *jay, AtomId args) {
  AtomId lhs = eval_car(jay, args);
  AtomId rhs = eval_car(jay, atom_cdr(jay, args));
  return atom_number(jay, lhs) >= atom_number(jay, rhs) ? jay->t : jay->nil;
}

AtomId set(Jay *jay, AtomId args) {
  StringId name = atom_id(jay, atom_car(jay, args));
  AtomId val = eval_car(jay, atom_cdr(jay, args));
  symbol_intern(jay, name, val);
  return val;
}

AtomId quit(Jay *jay, AtomId args) {
  int exitCode = 0;
  if (!is_nil(jay, args)) {
    AtomId out = eval_car(jay, args);
    if (atom_type(jay, out) == ATOM_NUMBER) {
      exitCode = (int) atom_number(jay, out);
    }
  }
  lang_free(jay);
  exit(exitCode);
}
 
AtomId defnc(Jay *jay, AtomId atom) {
  AtomId fncName = atom_car(jay, atom);
  AtomId paramNames = atom_car(jay, atom_cdr(jay, atom));
  AtomId instructions = atom_cdr(jay, atom_cdr(jay, atom));
  AtomId fnc = atom_intern_fnc(jay, paramNames, instructions);
  symbol_intern(jay, atom_id(jay, fncName), fnc);
  return fnc;
}

/*
 * eval
 */
static AtomId _eval_fnc(Jay *jay, AtomId car, AtomId cdr) {
  stack_push(jay);
  AtomId paramNames = atom_param_names(jay, car);
  AtomId instructions = atom_instructions(jay, car);
  while (!is_nil(jay, cdr)) {
    StringId paramName = atom_id(jay, atom_car(jay, paramNames));
    AtomId paramVal = eval_car(jay, cdr);
    symbol_intern(jay, paramName, paramVal);
    paramNames = atom_cdr(jay, paramNames);
    cdr = atom_cdr(jay, cdr);
  }
  AtomId out = jay->nil;
  while (!is_nil(jay, instructions)) {
    out = eval_car(jay, instructions);
    instructions = atom_cdr(jay, instructions);
  }
  stack_pop(jay);
  return out;
}

static AtomId _eval_list(Jay *jay, AtomId car, AtomId cdr) {
  AtomType type = atom_type(jay, car);
  switch (type) {
  case ATOM_BUILTIN:
    return atom_builtin(jay, car)(jay, cdr);
  case ATOM_ID:
    return _eval_list(jay, symbol_lookup(jay, atom_id(jay, car)), cdr);
  case ATOM_FNC:
    return _eval_fnc(jay, car, cdr);
  case ATOM_CONS:
  case ATOM_STRING:
  case ATOM_NUMBER:
  case ATOM_NIL:
  default:
    LOG_FATAL("first element in list cannot be of type %s", atomTypeNames[type]);
  }
}

AtomId eval_atom(Jay *jay, AtomId atom) {
  AtomType type = atom_type(jay, atom);
  switch (type) {
  case ATOM_CONS:
    return _eval_list(jay, atom_car(jay, atom), atom_cdr(jay, atom));
  case ATOM_ID:
    return symbol_lookup(jay, atom_id(jay, atom));
  case ATOM_FNC:
  case ATOM_BUILTIN:
  case ATOM_STRING:
  case ATOM_NUMBER:
  case ATOM_NIL:
    return atom;
  default:
    LOG_FATAL("not programmed to handle type %s", atomTypeNames[type]);
  }
}

AtomId eval_car(Jay *jay, AtomId atom) {
  return eval_atom(jay, atom_car(jay, atom));
}

AtomId eval(Jay *jay, AtomId atom) {
  return eval_atom(jay, eval_car(jay, atom));
}


/*
 * equal
 */
static int _equal(Jay *jay, AtomId lhs, AtomId rhs) {
  if (atom_type(jay, lhs) != atom_type(jay, rhs)) {
    return 0;
  }
  switch (atom_type(jay, lhs)) {
  case ATOM_NUMBER:
    return atom_number(jay, lhs) == atom_number(jay, rhs);
  case ATOM_STRING:
    return atom_string(jay, lhs).id == atom_string(jay, rhs).id;
  case ATOM_NIL:
    return is_nil(jay, lhs) && is_nil(jay, rhs);
  case ATOM_FNC:
  case ATOM_CONS:
  case ATOM_ID:
  case ATOM_BUILTIN:
  default:
    return lhs.id == rhs.id;
  }
}

AtomId equal(Jay *jay, AtomId cons) {
  AtomId lhs = eval_car(jay, cons);
  while (!is_nil(jay, cons = atom_cdr(jay, cons))) {
    AtomId rhs = eval_car(jay, cons);
    if (!_equal(jay, lhs, rhs)) {
      return jay->nil;
    }
  }
  return jay->t;
}

/*
 * print
 */
static void _print(Jay *jay, AtomId atom) {
  AtomType type = atom_type(jay, atom);
  double number;
  switch (type) {
  case ATOM_CONS:
    fputc('(', jay->out);
    do {
      _print(jay, atom_car(jay, atom));
      atom = atom_cdr(jay, atom);
    } while (!is_nil(jay, atom) && fputc(' ', jay->out));
    fputc(')', jay->out);
    break;
  case ATOM_STRING:
    fputs(string_chars(jay, atom_string(jay, atom)), jay->out);
    break;
  case ATOM_ID:
    fputs(string_chars(jay, atom_id(jay, atom)), jay->out);
    break;
  case ATOM_NUMBER:
    number = atom_number(jay, atom);
    if (number == (double)(long long) number)  {
      fprintf(jay->out, "%lld", (long long) number);
    } else {
      fprintf(jay->out, "%f", number);
    }
    break;
  case ATOM_BUILTIN:
    fputs("BUILTIN", jay->out);
    break;
  case ATOM_FNC:
    fputs("FNC", jay->out);
    break;
  case ATOM_NIL:
    fputs("NIL", jay->out);
    break;
  default:
    LOG_FATAL("unrecognized print type %s", atomTypeNames[type]);
  }
}

AtomId print(Jay *jay, AtomId atom) {
  AtomId out = jay->nil;
  while (!is_nil(jay, atom)) {
    out = eval_car(jay, atom);
    _print(jay, out);
    atom = atom_cdr(jay, atom);
  }
  fputc('\n', jay->out);
  return out;
}

AtomId print_atom(Jay *jay, AtomId atom) {
  _print(jay, atom);
  fputc('\n', jay->out);
  return atom;
}

/*
 * read
 */
static AtomId _read_list(Jay *jay);

AtomId read_token(Jay *jay, Token token) {
  switch (token.type) {
  case TOKEN_POPEN:
    return _read_list(jay);
  case TOKEN_PCLOSE:
    LOG_FATAL("closing an unmatched paren");
  case TOKEN_STRING:
    return atom_intern_string(jay, string_intern(jay, token.val));
  case TOKEN_NUMBER:
    return atom_intern_number(jay, atof(token.val));
  case TOKEN_ID:     
    return atom_intern_id(jay, string_intern(jay, token.val));
  case TOKEN_EOF:
    LOG_FATAL("eof in middle of read");
  default:
    LOG_FATAL("unrecognized token %d", token);
  }
}

static AtomId _read_list(Jay *jay) {
  Token token = io_read(jay);
  if (token.type == TOKEN_PCLOSE) {
    return jay->nil;
  }
  AtomId car = read_token(jay, token);
  AtomId cdr = _read_list(jay);
  return atom_intern_cons(jay, car, cdr);
}

AtomId read(Jay *jay, AtomId atom) {
  return read_token(jay, io_read(jay));
}
