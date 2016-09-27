CC=gcc
# http://stackoverflow.com/questions/3375697/useful-gcc-flags-for-c
WARNFLAGS=-std=c11 -Wall -Werror -pedantic -ftrapv -Wundef -Wshadow -Wpointer-arith -Wcast-align -Wstrict-prototypes -Wstrict-overflow=5 -Wwrite-strings -Wswitch-enum -Wunreachable-code
LDFLAGS=-g $(WARNFLAGS) -I.
OBJECTS= *.c
HEADERS= *.h

all: jay
	
jay: $(OBJECTS) $(HEADERS)
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

test: jay
	@cd tests && ./runTests.sh

# clean
clean:
	rm -f *.o *.exe *.stackdump
