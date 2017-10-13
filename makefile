YACC=bison
LEX=flex
CC=cc
OPTS=-g -Wall

BISONFILE=bison
FLEXFILE=flex

SOURCES=$(BISONFILE).tab.c $(FLEXFILE).c main.c tree.c nonterm.c
HEADERS=$(BISONFILE).tab.h token.h tree.h nonterm.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=120++

all: $(SOURCES) $(EXECUTABLE)

# Build the EXECUTABLE from the objects
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OPTS) $(OBJECTS) -o $@

# Build all .o files from all .c files
$(OBJECTS): $(SOURCES)
	$(CC) -c -g $(SOURCES)

# Create the bison.c and bison.h files
$(BISONFILE).tab.c $(BISONFILE).tab.h: $(BISONFILE).y
	$(YACC) -dt --verbose $<


# Create the flex.c file
$(FLEXFILE:.l=.c): $(FELXFILE).l
	$(LEX) -t $(FLEXFILE).l >$(FLEXFILE).c

# Remove generated files
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
	rm -f $(FLEXFILE).c $(BISONFILE).tab.c $(BISONFILE).tab.h $(BISONFILE).output
