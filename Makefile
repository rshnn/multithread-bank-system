COMPILER=gcc
FLAGS= -g  -D_POSIX_C_SOURCE -Wall -std=c99 -pthread -D_GNU_SOURCE
PROGRAM=server
INCLUDES_FOLDER=include
LIBRARY_FOLDER=lib
SOURCES=linkedlist.c hashmap.c tokenizer.c server.c
CLEANUP=*.o server client
DEBUG=FALSE


ALL_SOURCES=$(LIBRARY_SOURCES) $(SOURCES)
OBJECTS=$(ALL_SOURCES:.c=.o)


SOURCES_LINKED=$(SOURCES:%.c=%.c)
LIBRARY_SOURCES_LINKED=$(LIBRARY_SOURCES:%.c=$(LIBRARY_FOLDER)/%.c)
ALL_SOURCES_LINKED=$(LIBRARY_SOURCES_LINKED) $(SOURCES_LINKED)

ifeq ($(DEBUG), FALSE)
	SUPPRESS=@
else
	SUPPRESS=
endif

MAKE: COMPILE $(PROGRAM)

clean:
	$(SUPPRESS)rm -rf ./$(CLEANUP)

$(PROGRAM): $(OBJECTS)
	$(SUPPRESS)$(COMPILER) $(FLAGS) -I $(INCLUDES_FOLDER) $(OBJECTS) -o $(PROGRAM)
	$(SUPPRESS)gcc -o client -std=gnu99 -pthread client.c

COMPILE: $(SOURCES_LINKED)
	$(SUPPRESS)$(COMPILER) -c $(FLAGS) -I $(INCLUDES_FOLDER) $(ALL_SOURCES_LINKED)
