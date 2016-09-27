#ifndef  BUILTIN_DEFINED
#define  BUILTIN_DEFINED

#include "io.h"
#include "lang.h"

int builtins_init(Jay*);
void builtins_free(Jay*);

AtomId print(Jay*, AtomId);
AtomId print_atom(Jay*, AtomId);
AtomId read(Jay*, AtomId);
AtomId read_token(Jay*, Token);
AtomId add(Jay*, AtomId);
AtomId mul(Jay*, AtomId);
AtomId sub(Jay*, AtomId);
AtomId divide(Jay*, AtomId);
AtomId equal(Jay*, AtomId);
AtomId quote(Jay*, AtomId);
AtomId eval(Jay*, AtomId);
AtomId ifFn(Jay*, AtomId);
AtomId orFn(Jay*, AtomId);
AtomId andFn(Jay*, AtomId);
AtomId set(Jay*, AtomId);
AtomId quit(Jay*, AtomId);
AtomId defnc(Jay*, AtomId);

#endif /*BUILTIN_DEFINED*/

