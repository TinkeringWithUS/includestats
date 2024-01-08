#include "arraySet.h"

#include <stdlib.h>

struct array_set {
  struct array_list* backing_store;
};

struct array_set* create_set() {
  struct array_set* new_set = malloc(sizeof(struct array_set));

  new_set->backing_store = create_list();

  return new_set;
}

// Doesn't add if element in set, otherwise adds in set.
bool add_set_element(struct array_set* set, 
                    struct array_set_element* element) {
  if(set == NULL || element == NULL) {
    return false; 
  }
  if(set_has_element(set, element)) {
    return false; 
  }
  return add_element(set->backing_store, &element->data);
}

// O(N), typically a hashmap / self balancing BST are used, 
// but too much to implement, for another day. 
// If null inputs, return false. 
bool set_has_element(struct array_set* set, 
                    struct array_set_element* element) {
  if(set == NULL || element == NULL) {
    return false; 
  }

  int set_size = get_array_set_size(set); 

  for(int i = 0; i < set_size; i++) {
    union array_element * set_item = get_element(set->backing_store, i); 

    if(set_item->ptr_data == element->data.ptr_data) {
      return true; 
    }
  }
  return false; 
}

// returns false if inputs are false or doesn't exist 
bool remove_set_element(struct array_set* set,
                        struct array_set_element* element) {
  if(set == NULL || element == NULL) {
    return false; 
  }
  if(!set_has_element(set, element)) {
    return false; 
  }
  return remove_element(set->backing_store, &element->data); 
}

// returns -1 if set is null, otherwise gets size.
int get_array_set_size(struct array_set* set) {
  if(set == NULL) {
    return -1; 
  }
  return get_size(set->backing_store); 
}

// void print_set(struct array_set* set) {
//   if(set == NULL)  {
//     return; 
//   }

//   for(int i = 0; i < get_array_set_size(set); i++) {
//     printf("")
//   }
// }