#ifndef  ATOM_DEFINED
#define  ATOM_DEFINED

#include "lang.h"
#include "strings.h"

typedef enum {
  ATOM_NIL, ATOM_CONS, ATOM_STRING, ATOM_NUMBER, ATOM_ID, ATOM_BUILTIN, ATOM_FNC
} AtomType;

extern const char *atomTypeNames[];

typedef AtomId (*BuiltinFn)(Jay*, AtomId cons);

int atoms_init(Jay*);
void atoms_free(Jay*);

AtomType atom_type(Jay*, AtomId);
int is_nil(Jay*, AtomId);
AtomId atom_car(Jay*, AtomId);
void atom_car_set(Jay*, AtomId, AtomId);
AtomId atom_cdr(Jay*, AtomId);
void atom_cdr_set(Jay*, AtomId, AtomId);
StringId atom_string(Jay*, AtomId);
double atom_number(Jay*, AtomId);
BuiltinFn atom_builtin(Jay*, AtomId);
AtomId atom_param_names(Jay*, AtomId);
AtomId atom_instructions(Jay*, AtomId);

AtomId atom_intern_cons(Jay*);
AtomId atom_intern_string(Jay*, StringId);
AtomId atom_intern_id(Jay*, StringId);
AtomId atom_intern_number(Jay*, double);
AtomId atom_intern_builtin(Jay*, BuiltinFn);
AtomId atom_intern_fnc(Jay*, AtomId paramNames, AtomId instructions);

#endif /*ATOM_DEFINED*/
