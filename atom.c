#include "atom.h"
#include "log.h"

const char *atomTypeNames[] = {
  "NIL", "CONS", "STRING", "NUMBER", "ID", "BUILTIN", "FNC"
};

typedef struct Atom {
  AtomType type;
  union {
    struct {
      AtomId car;
      AtomId cdr;
    };
    StringId string;
    double number;
    BuiltinFn builtinFn;
    struct {
      AtomId paramNames;
      AtomId instructions;
    };
  };
} Atom;

static Atom* _atom(Jay *jay, AtomId atom) {
  return &(jay->atoms[atom.id]);
}

int atoms_init(Jay *jay) {
  // atoms
  jay->atoms = malloc(sizeof(Atom) * NUM_INITIAL_ATOMS);
  if (!jay->atoms) {
    LOG_ERROR("malloc returned nothing");
    return 1;
  }
  jay->atomsCount = NUM_INITIAL_ATOMS;

  // unused atoms
  for (size_t i = 1; i < jay->atomsCount; i++) {
    jay->atoms[i].type = ATOM_CONS;
    jay->atoms[i].cdr.id = i - 1;
    jay->unusedAtoms = (AtomId) {.id = i};
  }

  // basic atoms
  jay->nil = (AtomId) {.id = 0};
  _atom(jay, jay->nil)->type = ATOM_NIL;
  jay->t = atom_intern_string(jay, string_intern(jay, "#t", strlen("#t")));

  return 0;
}

void atoms_free(Jay *jay) {
  if (jay->atoms) {
    free(jay->atoms);
  }
}

AtomType atom_type(Jay *jay, AtomId atom) {
  return _atom(jay, atom)->type;
}

int is_nil(Jay *jay, AtomId atom) {
  return jay->nil.id == atom.id;
}

AtomId atom_car(Jay *jay, AtomId atom) {
  return _atom(jay, atom)->car;
}

void atom_car_set(Jay *jay, AtomId atom, AtomId car) {
  _atom(jay, atom)->car = car;
}

AtomId atom_cdr(Jay *jay, AtomId atom) {
  return _atom(jay, atom)->cdr;
}

void atom_cdr_set(Jay *jay, AtomId atom, AtomId cdr) {
  _atom(jay, atom)->cdr = cdr;
}

StringId atom_string(Jay *jay, AtomId atom) {
  return _atom(jay, atom)->string;
}

double atom_number(Jay *jay, AtomId atom) {
  return _atom(jay, atom)->number;
}

BuiltinFn atom_builtin(Jay *jay, AtomId atom) {
  return _atom(jay, atom)->builtinFn;
}

AtomId atom_param_names(Jay *jay, AtomId atom) {
  return _atom(jay, atom)->paramNames;
}

AtomId atom_instructions(Jay *jay, AtomId atom) {
  return _atom(jay, atom)->instructions;
}

static AtomId _atom_intern(Jay *jay, Atom atom) {
  AtomId out = jay->unusedAtoms;
  if (is_nil(jay, out)) {
    LOG_FATAL("out of things to intern");
  }
  jay->unusedAtoms = atom_cdr(jay, out);
  jay->atoms[out.id] = atom;
  return out;
}

AtomId atom_intern_cons(Jay *jay) {
  Atom atom = {.type = ATOM_CONS, .car = jay->nil, .cdr = jay->nil};
  return _atom_intern(jay, atom);
}

AtomId atom_intern_string(Jay *jay, StringId string) {
  Atom atom = {.type = ATOM_STRING, .string = string};
  return _atom_intern(jay, atom);
}

AtomId atom_intern_id(Jay *jay, StringId string) {
  Atom atom = {.type = ATOM_ID, .string = string};
  return _atom_intern(jay, atom);
}

AtomId atom_intern_number(Jay *jay, double number) {
  Atom atom = {.type = ATOM_NUMBER, .number = number};
  return _atom_intern(jay, atom);
}

AtomId atom_intern_builtin(Jay *jay, BuiltinFn builtinFn) {
  Atom atom = {.type = ATOM_BUILTIN, .builtinFn = builtinFn};
  return _atom_intern(jay, atom);
}

AtomId atom_intern_fnc(Jay *jay, AtomId paramNames, AtomId instructions) {
  Atom atom = {.type = ATOM_FNC, .paramNames = paramNames, .instructions = instructions};
  return _atom_intern(jay, atom);
}
