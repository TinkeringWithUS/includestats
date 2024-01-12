#pragma once

#include "hashmap.h"

#include "arrayList.h"
#include <stdlib.h>

struct hash_element {
  size_t hash; 
  
};


struct hashmap {
  struct hash_element * backing_store; 
  int capacity;  
  int size;  
}; 

// struct hashmap  
