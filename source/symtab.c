#include "../header/symtab.h"

#include "../header/logger.h"

// creates a new symbol table, whose scope is local to (or inside) parent
symtab_make(Symtab *parent) {
  Symtab *symtab = (Symtab *)malloc(sizeof(Symtab));
  if (parent)
    symtab->patent = parent;
  else
    parent = NULL;
  symtab->nNodes = 0;
  return symtab;
}

// insert a symbol into a table
int symtab_insert(Symtab *table, char *key, Type *type) {
  log_assert(table, "table");
  log_assert(key, "key");
  log_assert(type, "type");

  // create the node
  SymtabNode *node = (SymtabNode *)malloc(sizeof(SymtabNode));
  node->key = key;
  node->type = type;
  log_assert(node, "node");

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
}

// lookup a symbol in a table;
// returns NULL if symbol does not exist
SymtabNode *symtab_lookup(Symtab *table, char *key) {
  log_assert(table, "table");
  log_assert(key, "key");
  int hash = symtab_hash(key) % TABLE_SIZE;

  SymtabNode *target = NULL;

  // search bucket for target
  int firstLoop = 1;
  do {
    if (!firstLoop)
      table = table->parent;
    target = table->buckets[hash];
    if (target) {
      target = symtab_search_bucket(table->buckets[hash], key);
    }
    firstLoop = 0;
  } while (target == NULL && table->parent);

  // couldn't find target
  return NULL;
}

SymtabNode *symtab_search_bucket(SymtabNode *node, char *key) {
  log_assert(node, "node");
  log_assert(key, "key");

  int firstLoop = 1;
  do {
    if (!firstLoop)
      node = node->next;
    if (strcmp(node->key, key) == 0)
      return node;
    firstLoop = 0;
  } while (node->next);

  // couldn't find target
  return NULL;
}

// djb2 algorithm created by Dan Bernstein http://www.cse.yorku.ca/~oz/hash.html
int symtab_hash(char *key) {
  unsigned long hash = 5381;
  int c;

  while (c = *key++)
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash % TABLE_SIZE;
}