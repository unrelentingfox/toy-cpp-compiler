YACC=bison
LEX=flex
CC=cc

BISONFILE=bison
FLEXFILE=flex

SOURCES=$(BISONFILE).c $(FLEXFILE).c main.c
HEADERS=$(BISONFILE).h structs.h
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=120

all: $(SOURCES) $(EXECUTABLE)

# Build the EXECUTABLE from the objects
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@
	mv $(EXECUTABLE) test

# Build all .o files from all .c files
$(OBJECTS): $(SOURCES)
	$(CC) -c $(SOURCES)

# Create the bison.c and bison.h files
$(BISONFILE).c $(BISONFILE).h: $(BISONFILE).y
	$(YACC) -dt --verbose $<
	mv -f $(BISONFILE).tab.c $(BISONFILE).c
	mv -f $(BISONFILE).tab.h $(BISONFILE).h

# Create the flex.c file
$(FLEXFILE:.l=.c): $(FELXFILE).l
	$(LEX) -t $(FLEXFILE).l >$(FLEXFILE).c

# Remove generated files
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
	rm -f $(FLEXFILE).c $(BISONFILE).c $(BISONFILE).h
