#include "arrayList.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h> 

struct array_list {
  union array_element* array_store;
  int capacity;
  int size;
};

static void expand_list(struct array_list* list) {
  // double backing array store, gives O(1), amortized for growing array
  union array_element* expanded_storage =
      calloc(sizeof(union array_element), list->capacity * 2);

  // calloc failure
  if (expanded_storage == NULL) {
    return;
  }

  // copy over elements
  for (size_t element = 0; element < get_size(list); element++) {
    expanded_storage[element] = *get_element(list, element);
  }

  free(list->array_store);

  list->array_store = expanded_storage;
  list->capacity *= 2;
}

struct array_list* create_list() {
  struct array_list* new_array = malloc(sizeof(struct array_list));
  new_array->array_store =
      calloc(sizeof(union array_element), DEFAULT_CAPACITY);
  new_array->capacity = DEFAULT_CAPACITY;
  new_array->size = 0;
  return new_array;
}

void delete_list(struct array_list* list) {
  free(list->array_store);
  list->capacity = 0;
  list->size = 0;
}

bool add_element(struct array_list* list, union array_element* element) {
  if (list == NULL || element == NULL) {
    return false;
  }
  if (get_size(list) + 1 >= list->capacity) {
    expand_list(list);
  }
  list->array_store[get_size(list)] = *element;
  list->size++;
  return true;
}

bool remove_index(struct array_list* list, int index) {
  if (index < 0 || index >= list->size || list == NULL) {
    return false;
  }

  for (int i = index + 1; i < list->size; i++) {
    list->array_store[i - 1] = list->array_store[i];
  }

  list->size--;

  return true;
}

bool remove_element(struct array_list* list, union array_element* element) {
  if (list == NULL || element == NULL) {
    return false;
  }

  bool found = false;
  int foundIndex = -1;
  while (!found && foundIndex < list->size) {
    foundIndex++;
    if (list->array_store[foundIndex].double_data == element->double_data) {
      found = true;
    }
  }
  if (found) {
    return remove_index(list, foundIndex);
  }
  return false;
}

union array_element* get_element(struct array_list* list, int index) {
  if (index < 0 || index >= list->size || list == NULL) {
    return NULL;
  }
  return &list->array_store[index];
}

bool has_element(struct array_list* list, union array_element* element,
                 bool compare_string) {
  int list_size = get_size(list); 

  for (int i = 0; i < list_size; i++) {
    if (compare_string &&
        strcmp(element->str_data, get_element(list, i)->str_data) == 0) {
      return true;
    } else if (element->ptr_data == get_element(list, i)->ptr_data) {
      return true;
    }
  }
  return false;
}

int get_size(struct array_list* list) { 
  return list->size; 
}

void print_list(struct array_list* list) {
  for (int i = 0; i < get_size(list); i++) {
    printf("(idx: %d, val: %s)\n", i, get_element(list, i)->str_data);
  }
}

void print_list_int(struct array_list *list) {
  for (int i = 0; i < get_size(list); i++) {
    printf("(idx: %d, val: %d)\n", i, get_element(list, i)->int_data);
  }
} 