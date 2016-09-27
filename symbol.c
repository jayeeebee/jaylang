#include "atom.h"
#include "log.h"
#include "symbol.h"

int symbols_init(Jay *jay) {
  jay->frameOfReference = atom_intern_cons(jay);
  jay->symbolNames = jay->nil;
  jay->symbolValues = jay->nil;
  symbol_intern(jay, string_intern(jay, "nil", strlen("nil")), jay->nil);
  symbol_intern(jay, string_intern(jay, "#t", strlen("#t")), jay->t);
  return 0;
}

void symbols_free(Jay *jay) {
}

void stack_push(Jay *jay) {
  AtomId cons = atom_intern_cons(jay);
  atom_car_set(jay, cons, jay->frameOfReference);
  atom_cdr_set(jay, cons, jay->symbolNames);
  jay->symbolNames = cons;
  
  cons = atom_intern_cons(jay);
  atom_car_set(jay, cons, jay->frameOfReference);
  atom_cdr_set(jay, cons, jay->symbolValues);
  jay->symbolValues = cons;
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
  AtomId names = atom_intern_cons(jay);
  atom_car_set(jay, names, atom_intern_id(jay, string));
  atom_cdr_set(jay, names, jay->symbolNames);
  jay->symbolNames = names;
  
  AtomId values = atom_intern_cons(jay);
  atom_car_set(jay, values, atom);
  atom_cdr_set(jay, values, jay->symbolValues);
  jay->symbolValues = values;
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
