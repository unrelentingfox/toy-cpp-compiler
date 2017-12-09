#include "../header/symtab.h"
#include "../header/logger.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// creates a new symbol table, whose scope is local to (or inside) parent
Symtab *symtab_new_table(Symtab *parent, Type *type) {
  Symtab *new_symtab = NULL;
  // allocate the hashtable
  if ((new_symtab = (Symtab *)malloc(sizeof(Symtab))) == NULL) {
    log_error(INTERNAL_ERROR, "ERROR: could not allocate new hashtable.");
  }
  // set parent scope
  if (parent)
    new_symtab->parent = parent;
  else
    new_symtab->parent = NULL;
  if (type)
    new_symtab->type = type;
  else
    new_symtab->type = NULL;
  //allocate bucket pointers
  if ((new_symtab->buckets = (SymtabNode **)malloc(sizeof(SymtabNode *) * SYMTAB_SIZE)) == NULL) {
    log_error(INTERNAL_ERROR, "ERROR: could not allocate new hashtable.");
  }
  for (int i = 0; i < SYMTAB_SIZE; i++) {
    new_symtab->buckets[i] = NULL;
  }
  new_symtab->count = 0;

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
  int hash = symtab_hash(key) % SYMTAB_SIZE;
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
    table->count++;
    return SYM_SUCCESS;
  } else {
    log_error(INTERNAL_ERROR, "ERROR: Node could not be inserted into the symboltable.\n");
    return SYM_FAILED;
  }
}

// the search function, with a flag to decide whether or not to search parent.
static SymtabNode *symtab_lookup_helper(Symtab *table, char *key, int searchparent) {
  LOG_ASSERT(table);
  LOG_ASSERT(key);
  int hash = symtab_hash(key) % SYMTAB_SIZE;
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
// lookup a symbol in a scope and its parent scopes
// returns NULL if symbol does not exist
SymtabNode *symtab_lookup(Symtab *table, char *key) {
  return symtab_lookup_helper(table, key, 1);
}
// lookup a symbol only in the scope specified, not in the parent scopes.
SymtabNode *symtab_lookup_local(Symtab *table, char *key) {
  return symtab_lookup_helper(table, key, 0);
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

  while ((c = *key++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return hash % SYMTAB_SIZE;
}

int symtab_get_size_type(Type *type) {
  switch (type->basetype) {
    case CLASS_INSTANCE_T:
      LOG_ASSERT(type->info.classinstance.classtype);
      return symtab_get_size_type(type->info.classinstance.classtype);
    case CLASS_T:
      LOG_ASSERT(type->info.class.public);
      return symtab_get_size_symtab(type->info.class.public);
    case FUNCTION_T:
      LOG_ASSERT(type->info.function.symtab);
      return symtab_get_size_symtab(type->info.function.symtab);
    case ARRAY_T:
      return (type->info.array.size * symtab_get_size_type(type->info.array.type));
    default:
      return type->size;
  }
}

int symtab_get_size_symtab(Symtab *symtab) {
  int size = 0;
  SymtabNode *node = NULL;
  for (int i = 0; i < SYMTAB_SIZE; i++) { // traverse the symtab
    node = symtab->buckets[i];
    while (node != NULL) { // traverse the buckets
      LOG_ASSERT(node->type);
      size += symtab_get_size_type(node->type);
      node = node->next;
    }
  }
  return size;
}

void symtab_print_table(Symtab *table, int indent) {
  LOG_ASSERT(table);
  for (int i = 0; i < SYMTAB_SIZE; i++) {
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
      printf(" -> (");
      printf("\"%s\" t:%s ", node->key, type_to_string(node->type));
      switch (node->type->basetype) {
        case FUNCTION_T: {
          if (node->type->info.function.parameters) {
            Parameter **params = node->type->info.function.parameters;
            for (int i = 0; i < node->type->info.function.nparams; i++) {
              printf("p:%d ", params[i]->type->basetype);
            }
          }
        }
        break;
        default:
          break;
      }
      printf("w:%db ", symtab_get_size_type(node->type));
      printf("<%d,%d>", node->address->region, node->address->offset);
      printf(")");
    }
    firstLoop = false;
  } while (node->next);
  printf("\n");
}