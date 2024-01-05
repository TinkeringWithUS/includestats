#include <stdio.h>

#include "arrayList.h"


int main() {
  printf("hello\n");

  struct array_list *list = create_list();

  for (int i = 0; i < 20; i++) {
    union array_element e;
    e.int_data = i;
    add_element(list, &e);
  }

  printf("testing print list \n");

  for(int i = 0; i < 20; i++) {
    if(get_element(list, i)->int_data != i) {
      printf("FAILED 1st TEST CASE\n"); 

      print_list_int(list);
      break;
    }
  }

  printf("passed test case 1\n"); 

  for (int i = 20; i >= 0; i--) {
    remove_index(list, i);
  }

  if(get_size(list) == 0) {
    printf("passed test case 2, removing all elements\n"); 
  } else {
    printf("FAILED 2nd TEST CASE REMOVING ALL\n"); 
    print_list_int(list); 
  }

  for (size_t i = 0; i < 20; i++) {
    union array_element e;
    e.int_data = i;
    add_element(list, &e);
  }

  for (size_t i = 0; i < 5; i++) {
    union array_element r;
    r.int_data = i;
    remove_element(list, &r);
  }

  for(int i = 0; i < 5; i++) {
    if(get_element(list, i)->int_data == i) {
      printf("FAILED TEST CASE 3, REMOVING FIRST 5\n"); 
      print_list_int(list); 
      break; 
    }
  }

  printf("passed test case 3, removing first 5\n");

  union array_element d;
  d.int_data = 15;

  remove_element(list, &d);

  if(get_element(list, 15) == NULL) {
    printf("passed test case 4, removing middle data\n"); 
  } else {
    printf("FAILED TEST CASE 4 removing middle data\\n"); 
    print_list_int(list); 
  }

  return 0;
}