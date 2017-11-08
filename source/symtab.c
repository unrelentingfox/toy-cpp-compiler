#include "../header/symtab.h"
#include "../header/logger.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// creates a new symbol table, whose scope is local to (or inside) parent
Symtab *symtab_new_table(Symtab *parent) {
  Symtab *new_symtab = NULL;
  // allocate the hashtable
  if ((new_symtab = (Symtab *)malloc(sizeof(Symtab))) == NULL) {
    log_error(INTERNAL_ERROR, "ERROR: could not allocate new hashtable.");
  }
  // set parent scope
  if (parent)
    new_symtab->parent = parent;
  else
    parent = NULL;
  //allocate bucket pointers
  if ((new_symtab->buckets = (SymtabNode **)malloc(sizeof(SymtabNode *) * TABLE_SIZE)) == NULL) {
    log_error(INTERNAL_ERROR, "ERROR: could not allocate new hashtable.");
  }
  for (int i = 0; i < TABLE_SIZE; i++) {
    new_symtab->buckets[i] = NULL;
  }
  new_symtab->nNodes = 0;

  return new_symtab;
}

SymtabNode *symtab_new_node(char *key, Type *type) {
  LOG_ASSERT(key);
  LOG_ASSERT(type);
  SymtabNode *node = (SymtabNode *)malloc(sizeof(SymtabNode));
  node->key = strdup(key);
  node->type = type;
  node->next = NULL;
  return node;
}

// insert a symbol into a table
// return values correlate with symtabErrors enum
int symtab_insert(Symtab *table, char *key, Type *type) {
  LOG_ASSERT(table);
  LOG_ASSERT(key);

  //check for redeclaration
  if (symtab_lookup_local(table, key))
    return SYM_REDECLARED;

  // create the node
  SymtabNode *node = symtab_new_node(key, type);

  // insert the node
  int hash = symtab_hash(key) % TABLE_SIZE;
  if (table->buckets[hash] == NULL) {
    table->buckets[hash] = node;
  } else {
    SymtabNode *tmp = table->buckets[hash];
    while (tmp->next) {
      tmp = tmp->next;
    }
    tmp->next = node;
  }
  if (table->buckets[hash]) {
    table->nNodes++;
    return SYM_SUCCESS;
  } else {
    log_error(INTERNAL_ERROR, "ERROR: Node could not be inserted into the symboltable.\n");
    return SYM_FAILED;
  }
}

// lookup a symbol in a scope and its parent scopes
// returns NULL if symbol does not exist
SymtabNode *symtab_lookup(Symtab *table, char *key) {
  return symtab_lookup_(table, key, 1);
}
// lookup a symbol only in the scope specified, not in the parent scopes.
SymtabNode *symtab_lookup_local(Symtab *table, char *key) {
  return symtab_lookup_(table, key, 0);
}
// the search function, with a flag to decide whether or not to search parent.
static SymtabNode *symtab_lookup_(Symtab *table, char *key, int searchparent) {
  LOG_ASSERT(table);
  LOG_ASSERT(key);
  int hash = symtab_hash(key) % TABLE_SIZE;
  //printf("hash: %d\n", hash);

  SymtabNode *target = NULL;

  // search bucket for target
  if (table->buckets && table->buckets[hash])
    target = symtab_search_bucket(table->buckets[hash], key);
  if (target)
    return target;
  else if (searchparent && table->parent != NULL) /* search parent scope if it exists */
    return symtab_lookup(table->parent, key);
  else
    return NULL;
}





SymtabNode *symtab_search_bucket(SymtabNode *node, char *key) {
  LOG_ASSERT(node);
  LOG_ASSERT(key);
  int firstLoop = true;
  do {
    if (!firstLoop)
      node = node->next;
    if (strcmp(node->key, key) == 0)
      return node;
    firstLoop = false;
  } while (node->next);

  // couldn't find target
  return NULL;
}

// djb2 algorithm created by Dan Bernstein http://www.cse.yorku.ca/~oz/hash.html
int symtab_hash(char *key) {
  unsigned long hash = 5381;
  int c = 0;

  while (c = *key++)
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash % TABLE_SIZE;
}

void symtab_print_table(Symtab *table, int indent) {
  LOG_ASSERT(table);
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (table->buckets[i]) {
      for (int j = 0; j < indent; j++) {
        printf(" ");
      }
      printf("Table[%d]", i);
      symtab_print_bucket(table->buckets[i]);
      if (table->buckets[i]->type->basetype == FUNCTION_T
          && table->buckets[i]->type->info.function.symtab) {
        symtab_print_table(table->buckets[i]->type->info.function.symtab, indent + 4);
      } else if (table->buckets[i]->type->basetype == CLASS_T
                 && table->buckets[i]->type->info.class.public) {
        symtab_print_table(table->buckets[i]->type->info.class.public, indent + 4);
      }
    }
  }
}

void symtab_print_bucket(SymtabNode *node) {
  int firstLoop = true;
  do {
    if (!firstLoop)
      node = node->next;
    if (node) {
      printf(" -> %s(%d)", node->key, node->type->basetype);
      switch (node->type->basetype) {
        case FUNCTION_T:
          if (node->type->info.function.parameters) {
            Parameter **params = node->type->info.function.parameters;
            for (int i = 0; i < node->type->info.function.nparams; i++) {
              printf("-(p:%d)", params[i]->type->basetype);
            }
          }
          break;
        case ARRAY_T:
          printf("-[%d]", node->type->info.array.size);
      }
    }
    firstLoop = false;
  } while (node->next);
  printf("\n");
}