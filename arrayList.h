#include <stdbool.h>

#define DEFAULT_CAPACITY 10

union array_element {
  void *ptr_data;  // for pointing to structs
  double double_data;
  int int_data;
  char *str_data;
};

struct array_list;

struct array_list *create_list();
void delete_list(struct array_list *list);

bool add_element(struct array_list *list, union array_element *element);
bool remove_element(struct array_list *list, union array_element *element);
bool remove_index(struct array_list *list, int index);

int get_size(struct array_list *list);
union array_element *get_element(struct array_list *list, int index);

void print_list(struct array_list *list);