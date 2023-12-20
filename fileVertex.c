#include "fileVertex.h"

#include <stdlib.h>
#include <string.h>

struct file_vertex {
  // store a list of file dependencies
  // e.g. like <string.h> <unistd.h> etc
  // store links to file_vertex ptrs
  struct array_list *dependencies;
  // num times this file has been referenced
  int reference_count;
  char *filename;
  bool been_visited;
};

struct file_vertex *create_vertex(struct array_list *dependencies,
                                  char *filename) {
  struct file_vertex * vertex = calloc(sizeof(struct file_vertex));

  strncpy(vertex->filename, filename, strlen(filename) + 1); 
  

}