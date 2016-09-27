#ifndef  STRINGS_DEFINED
#define  STRINGS_DEFINED

#include "lang.h"

typedef struct {
  size_t id;
} StringId;

int strings_init(Jay*);
void strings_free(Jay*);
StringId string_intern(Jay*, const char*, size_t length);
#define STRING_INTERN(jay, str) string_intern(jay, str, strlen(str))
const char* string_chars(Jay*, StringId);

#endif /*STRINGS_DEFINED*/
