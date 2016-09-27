#ifndef  SYMBOL_DEFINED
#define  SYMBOL_DEFINED

#include "lang.h"

int symbols_init(Jay*);
void symbols_free(Jay*);

void stack_push(Jay*);
void stack_pop(Jay*);

void symbol_intern(Jay*, StringId, AtomId);
AtomId symbol_lookup(Jay*, StringId);

#endif /*SYMBOL_DEFINED*/
