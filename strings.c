#include "log.h"
#include "strings.h"

typedef struct String {
  int length;
  const char *chars;
  StringId id;
} String;

#define TRY(x) if (!(x)) {LOG_FATAL("error while allocating memory"); return 1;}

int strings_init(Jay *jay) {
  TRY(jay->charPool = malloc(CHAR_POOL_SIZE));
  TRY(jay->idToString = malloc(NUM_INITIAL_STRINGS * sizeof(size_t)));
  TRY(jay->strings = malloc(NUM_INITIAL_STRINGS * sizeof(String)));
  return 0;
}

void strings_free(Jay *jay) {
  if (jay->charPool) {
    free(jay->charPool);
  }
  if (jay->idToString) {
    free(jay->idToString);
  }
  if (jay->strings) {
    free(jay->strings);
  }
}

static int _string_lookup(Jay *jay, String str, size_t *index) {
  int min = 0;
  int max = jay->stringIdSize - 1;
  int mid = 0;
  while (min <= max) {
    mid = (min + max) / 2;
    int cmp = strcmp(str.chars, jay->strings[mid].chars);
    if (cmp < 0) {
      max = mid - 1;
    } else if (cmp > 0) {
      min = mid = mid + 1;
    } else {
      *index = mid;
      return 1;
    }
  }
  *index = mid;
  return 0;
}

StringId string_intern(Jay *jay, const char *chars, size_t length) {
  String string = {
    .chars = chars,
    .length = length
  };
  size_t index;
  int found = _string_lookup(jay, string, &index);
  if (found) {
    return jay->strings[index].id;
  }
  // TODO resize if necessary
  char *charPtr = jay->charPool + jay->charPoolSize;
  strcpy(charPtr, string.chars);
  jay->charPoolSize += string.length + 1;
  jay->idToString[jay->stringIdSize] = index;
  for (size_t i = jay->stringIdSize; i > index; i--) {
    jay->strings[i] = jay->strings[i - 1];
    jay->idToString[jay->strings[i].id.id]++;
  }
  jay->strings[index].chars = charPtr;
  jay->strings[index].length = string.length;
  jay->strings[index].id = (StringId) {.id = jay->stringIdSize};
  jay->stringIdSize++;
  return jay->strings[index].id;
}

const char* string_chars(Jay *jay, StringId id) {
  return jay->strings[jay->idToString[id.id]].chars;
}
