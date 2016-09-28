#ifndef  STRINGS_DEFINED
#define  STRINGS_DEFINED

#include "lang.h"

typedef struct {
  size_t id;
} StringId;

int strings_init(Jay*);
void strings_free(Jay*);
StringId string_intern(Jay*, const char*);
const char* string_chars(Jay*, StringId);

#endif /*STRINGS_DEFINED*/
