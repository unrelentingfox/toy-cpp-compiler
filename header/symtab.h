#ifndef SYMTAB_H
#define SYMTAB_H

#define TABLE_SIZE 100

#include "type.h"

typedef struct SymbolTableNode {
  char *key;                    // the name of the symbol.
  struct Type *type;                    // type info for the symbol.
  struct SymbolTableNode *next; // the next node in the bucket.
} SymtabNode;

typedef struct SymbolTable {
  struct SymbolTable *parent;     // the parent scope. If NULL then this is global scope.
  int nNodes;                     // number of nodes stored in this table.
  struct SymbolTableNode **buckets;  // the table itself. An array of nodes or "buckets".
} Symtab;

// creates a new symbol table, whose scope is local to (or inside) parent
Symtab *symtab_new_tree(Symtab *parent);
SymtabNode *symtab_new_node(char *key, struct Type *type);

// insert a symbol into a table
int symtab_insert(Symtab *table, char *key, struct Type *type);

// lookup a symbol in a table; returns structure pointer including type and offset. lookup operations are often chained together progressively from most local scope on out to global scope.
SymtabNode *symtab_lookup(Symtab *table, char *key);

SymtabNode *symtab_search_bucket(SymtabNode *head, char *key);

int symtab_hash(char *key);

void symtab_print_table(Symtab *table);
void symtab_print_bucket(SymtabNode *node);

// sums the widths of all entries in the table.
// TODO: widthSymTab(table);

#endif