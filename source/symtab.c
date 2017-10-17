#include "../header/symtab.h"
#include "../header/logger.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// creates a new symbol table, whose scope is local to (or inside) parent
Symtab *symtab_new_tree(Symtab *parent) {
  Symtab *new_symtab = NULL;
  // allocate the hashtable
  if ((new_symtab = (Symtab *)malloc(sizeof(Symtab))) == NULL) {
    log_error(NULL_ERROR, "ERROR: could not allocate new hashtable.");
  }
  // set parent scope
  if (parent)
    new_symtab->parent = parent;
  else
    parent = NULL;
  //allocate bucket pointers
  if ((new_symtab->buckets = (SymtabNode **)malloc(sizeof(SymtabNode *) * TABLE_SIZE)) == NULL) {
    log_error(NULL_ERROR, "ERROR: could not allocate new hashtable.");
  }
  for (int i = 0; i < TABLE_SIZE; i++) {
    new_symtab->buckets[i] = NULL;
  }
  new_symtab->nNodes = 0;

  return new_symtab;
}

SymtabNode *symtab_new_node(char *key, Type *type) {
  log_assert(key, "key");
  log_assert(type, "type");
  SymtabNode *node = (SymtabNode *)malloc(sizeof(SymtabNode));
  node->key = strdup(key);
  node->type = type;
  node->next = NULL;
  return node;
}

// insert a symbol into a table
// returns 1 if the symbol already exists
int symtab_insert(Symtab *table, char *key, Type *type) {
  log_assert(table, "table");
  log_assert(key, "key");
  // if (symtab_lookup(table, key))
  //   return 1;

  // create the node
  SymtabNode *node = symtab_new_node(key, type);

  // insert the node
  int hash = symtab_hash(key) % TABLE_SIZE;
  if (table->buckets[hash] == NULL) {
    table->buckets[hash] = node;
  } else {
    SymtabNode *tmp = table->buckets[hash];
    while (tmp->next)
      tmp = tmp->next;
    tmp->next = node;
  }
  return 0;
}

// lookup a symbol in a table;
// returns NULL if symbol does not exist
SymtabNode *symtab_lookup(Symtab *table, char *key) {
  log_assert(table, "table");
  log_assert(key, "key");
  int hash = symtab_hash(key) % TABLE_SIZE;

  SymtabNode *target = NULL;

  // search bucket for target
  int firstLoop = true;
  do {
    if (!firstLoop)
      table = table->parent;
    target = table->buckets[hash];
    if (target) {
      target = symtab_search_bucket(target, key);
    }
    firstLoop = false;
  } while (target == NULL && table->parent);

  return target;
}

SymtabNode *symtab_search_bucket(SymtabNode *node, char *key) {
  log_assert(node, "node");
  log_assert(key, "key");
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

void symtab_print_table(Symtab *table) {
  log_assert(table, "table");
  for (int i = 0; i < TABLE_SIZE; i++) {
    if (table->buckets[i]) {
      printf("%d : (BUCKET)", i);
      symtab_print_bucket(table->buckets[i]);
    } else
      printf("%d : (NULL) \n", i);
  }
}

void symtab_print_bucket(SymtabNode *node) {
  int firstLoop = true;
  do {
    if (!firstLoop)
      node = node->next;
    if (node) {
      printf("->%s(%d)", node->key, node->type->basetype);
      if (node->type->basetype == FUNCTION_T && node->type->info.function.parameters) {
        Parameter *temp = node->type->info.function.parameters;
        while (temp->next) {
          printf("-(%d)-", temp->type->basetype);
          temp = temp->next;
        }
        printf("-(%d)-", temp->type->basetype);
      }
    }
    firstLoop = false;
  } while (node->next);
  printf("\n");
}