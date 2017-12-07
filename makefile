YACC=bison
LEX=flex
CC=gcc
OPTS=-g -Wall -std=gnu99

###### USER MODIFIABLE FILE STRUCTURE ######
SOURCE_DIR=source
HEADER_DIR=header
FLEXBISON_DIR=flexbison
BIN_DIR=bin
TEMP_DIR=temp
BISON_FILE=bison
FLEX_FILE=flex
SOURCES=main.c tree.c nonterm.c logger.c semantic.c symtab.c type.c
HEADERS=token.h tree.h nonterm.h logger.h semantic.h symtab.h type.h

###### GENERATED FROM THE VARIABLES ABOVE ######
GEN_SOURCES=$(BISON_FILE).c $(FLEX_FILE).c
GEN_HEADERS=$(BISON_FILE).h
SOURCES+=$(GEN_SOURCES)
HEADERS+=$(GEN_HEADERS)
SOURCES_WITH_DIR=$(addprefix $(SOURCE_DIR)/, $(SOURCES))
OBJECTS=$(SOURCES_WITH_DIR:.c=.o)
HEADERS_WITH_PATH=$(addprefix $(HEADER_DIR)/, $(HEADERS))
EXECUTABLE=120++
all: $(SOURCES_WITH_DIR) $(EXECUTABLE) $(HEADERS_WITH_PATH)
# Build the EXECUTABLE from the objects
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OPTS) $(OBJECTS) -o $@
# Build all .o files from all .c files
$(OBJECTS): %.o : %.c
	$(CC) -c $(OPTS) $< -o $@
# Create the bison.c and bison.h files
$(SOURCE_DIR)/$(BISON_FILE).c $(HEADER_DIR)/$(BISON_FILE).h: $(FLEXBISON_DIR)/$(BISON_FILE).y
	$(YACC) -d --warnings=none $<
	mv $(BISON_FILE).tab.c $(SOURCE_DIR)/$(BISON_FILE).c
	mv $(BISON_FILE).tab.h $(HEADER_DIR)/$(BISON_FILE).h
# Create the flex.c file
$(SOURCE_DIR)/$(FLEX_FILE).c: $(FLEXBISON_DIR)/$(FLEX_FILE).l
	$(LEX) -t $< > $@
# Remove generated files
clean:
	rm -f $(EXECUTABLE) $(OBJECTS)
	rm -f $(HEADER_DIR)/$(GEN_HEADERS) $(SOURCE_DIR)/$(GEN_SOURCES) $(FLEXBISON_DIR)/$(BISON_FILE).output
