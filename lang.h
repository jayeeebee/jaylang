#ifndef  LANG_DEFINED
#define  LANG_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "options.h"

typedef struct {
  size_t id;
} AtomId;

struct Atom;
struct String;
struct SymbolTable;

typedef struct Jay {
  // args
  int argc;
  const char **argv;

  // strings
  char *charPool;
  size_t *idToString;
  struct String *strings;
  size_t stringIdSize;
  size_t charPoolSize;

  // atoms
  int age;
  struct Atom *atoms;
  size_t atomsCount;
  AtomId unusedAtoms;
  AtomId nil;
  AtomId t; // 't'rue

  // symbols
  AtomId symbolNames;
  AtomId symbolValues;
  AtomId frameOfReference;

  // io
  int isInteractive;
  FILE *in;
  FILE *out;
  char *buffer;
  size_t bufferMaxSize;
  size_t bufferCurrSize;
  size_t bufferCurr;
  size_t bufferNext;
  char bufferTemp;
} Jay;

int lang_init(Jay*);
void lang_free(Jay*);

#endif /*LANG_DEFINED*/
