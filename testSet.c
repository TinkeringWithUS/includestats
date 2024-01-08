#include "arraySet.h"

#include <stdio.h>
#include <stdlib.h>

#include <stdbool.h>

int main() {

  struct array_set * set = create_set(); 

  for(int i = 0; i < 20; i++) {
    struct array_set_element e;
    e.data.int_data = i; 
    add_set_element(set, &e); 
  }

  bool test_one = true; 
  for(int i = 0; i < 20; i+=5) {
    struct array_set_element e; 
    e.data.int_data = i;

    if(!set_has_element(set, &e)) {
      printf("FAILED TEST 1\n"); 
      test_one = false; 
      break; 
    } 
  }

  if(test_one) {
    printf("passed test 1\n"); 
  }

  struct array_set_element e; 
  e.data.int_data = 15;

  remove_set_element(set, &e); 

  if(set_has_element(set, &e)) {
    printf("FAILED TEST 2\n");
  } else {
    printf("passed test 2\n"); 
  }

  add_set_element(set, &e); 

  if(!add_set_element(set, &e)) {
    printf("passed test 3\n");
  } else {
    printf("FAILED TEST 3\n");
  }

  return 0; 
}