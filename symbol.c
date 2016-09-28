#include "atom.h"
#include "log.h"
#include "symbol.h"

int symbols_init(Jay *jay) {
  jay->frameOfReference = atom_intern_cons(jay, jay->nil, jay->nil);
  jay->symbolNames = jay->nil;
  jay->symbolValues = jay->nil;
  symbol_intern(jay, string_intern(jay, "nil"), jay->nil);
  symbol_intern(jay, string_intern(jay, "#t"), jay->t);
  return 0;
}

void symbols_free(Jay *jay) {
}

void stack_push(Jay *jay) {
  jay->symbolNames = atom_intern_cons(jay, jay->frameOfReference, jay->symbolNames);
  jay->symbolValues = atom_intern_cons(jay, jay->frameOfReference, jay->symbolValues);
}

void stack_pop(Jay *jay) {
  AtomId car;
  do {
    car = atom_car(jay, jay->symbolNames);
    jay->symbolNames = atom_cdr(jay, jay->symbolNames);
    jay->symbolValues = atom_cdr(jay, jay->symbolValues);
  } while (car.id != jay->frameOfReference.id);
}

void symbol_intern(Jay *jay, StringId string, AtomId atom) {
  jay->symbolNames = atom_intern_cons(jay, atom_intern_id(jay, string), jay->symbolNames);
  jay->symbolValues = atom_intern_cons(jay, atom, jay->symbolValues);
}

AtomId symbol_lookup(Jay *jay, StringId string) {
  AtomId names = jay->symbolNames;
  AtomId values = jay->symbolValues;
  while (!is_nil(jay, names)) {
    AtomId nameCar = atom_car(jay, names);
    AtomId valueCar = atom_car(jay, values);
    if (nameCar.id != jay->frameOfReference.id &&
        string.id == atom_id(jay, nameCar).id) {
      return valueCar;
    }
    names = atom_cdr(jay, names);
    values = atom_cdr(jay, values);
  }
  LOG_FATAL("unable to find symbol with name %s", string_chars(jay, string));
  return jay->nil;
}

static void _symbol_age(Jay *jay, AtomId atom) {
  atom_age(jay, atom);
  AtomType type = atom_type(jay, atom);
  switch (type) {
  case ATOM_FNC:
    _symbol_age(jay, atom_param_names(jay, atom));
    _symbol_age(jay, atom_instructions(jay, atom));
    break;
  case ATOM_CONS:
    _symbol_age(jay, atom_car(jay, atom));
    _symbol_age(jay, atom_cdr(jay, atom));
    break;
  case ATOM_NUMBER:
  case ATOM_STRING:
  case ATOM_NIL:
  case ATOM_ID:
  case ATOM_BUILTIN:
    break;
  default:
    LOG_FATAL("unknown atom type %s", atomTypeNames[type]); 
  }
}

void symbol_age(Jay *jay) {
  AtomId names = jay->symbolNames;
  AtomId values = jay->symbolValues;
  while (!is_nil(jay, names)) {
    _symbol_age(jay, names);
    _symbol_age(jay, values);
    names = atom_cdr(jay, names);
    if (!is_nil(jay, values)) {
      values = atom_cdr(jay, values);
    }
  }
}
