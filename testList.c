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
  print_list(list);

  for (int i = 20; i >= 0; i--) {
    remove_index(list, i);
  }

  printf("after removing all elements from list\n");
  print_list(list);

  for (size_t i = 0; i < 20; i++) {
    union array_element e;
    e.int_data = i;
    add_element(list, &e);
  }

  printf("testing list again\n");

  for (size_t i = 0; i < 5; i++) {
    union array_element r;
    r.int_data = i;
    remove_element(list, &r);
  }

  print_list(list);

  printf("removing middle element 15\n");
  union array_element d;
  d.int_data = 15;

  remove_element(list, &d);

  print_list(list);

  return 0;
}