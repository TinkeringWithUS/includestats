#include "arrayList.h"

struct array_set; 

struct array_set_element {
  union array_element data; 
}; 

struct array_set * create_set(); 

bool add_set_element(struct array_set * set, 
                    struct array_set_element * element);
 
bool set_has_element(struct array_set * set, 
        struct array_set_element * element);

bool remove_set_element(struct array_set * set,
                        struct array_set_element * element); 

int get_array_set_size(struct array_set * set); 

