#include "parseFile.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "arrayList.h"
#include "colors.h"
#include "stringHelp.h"

#include "arraySet.h"

bool is_comment(char *line) {
  char trimmed_line[strlen(line)];
  trim_and_copy(trimmed_line, line);
  char *comment = strstr(trimmed_line, "//");
  if (comment) {
    int comment_pos = comment - trimmed_line;
    // printf("got a comment %s\n", comment);
    // printf("comment pos %d\n", comment_pos);
    if (comment_pos == 0) {
      return true;
    }
  }
  return false;
}

// Assumption, function declaration's end is } on a new line
// e.g.
/*
int add() {

}
can't deal with one liners yet
*/
bool function_definition_end(char *line) { return line[0] == '}'; }

// grab substring excluding delim_start and delim_end
// if delim_start == delim_end, will still work (e.g. "hi" returns hi)
// ex: takes in <hi/, delim_start = < delim_end = /, returns hi
char *get_include_name(char *include_line, char delim_start, char delim_end) {
  int start_pos = -1;
  int end_pos = -1;

  int include_length = strlen(include_line);
  for (size_t i = 0; i < include_length; i++) {
    if (start_pos == -1 && include_line[i] == delim_start) {
      start_pos = i;
    } else if (include_line[i] == delim_end) {
      end_pos = i;
    }
  }

  if (start_pos < 0 || end_pos < 0) {
    return NULL;
  }

  // extra length needed for null terminator is already included
  char *include_name = calloc(end_pos - start_pos, sizeof(char));

  int include_name_index = 0;

  for (size_t i = start_pos + 1; i < end_pos; i++) {
    include_name[include_name_index++] = include_line[i];
  }
  return include_name;
}

// BIG ASSUMPTION:
// function definition is in the template of
// <return type> <function name>(<inputs>) {
// <body>
// }
// thus, () { will define a function
// misnomer, gets the function name when a function is
// defined. e.g. char * get_function_name would return
// the function name, "get_function_name"
char *get_function_name(char *line) {
  // function definitions are usually on one line
  // assuming that
  char *function_name_end = strstr(line, "(");
  // char *bracket_start = strstr(line, "{");
  if (!function_name_end) {
    return NULL;
  }

  int function_end_pos = function_name_end - line;

  char *function_name_start =
      find_substr_backwards(line, function_end_pos, ' ');

  // printf("function name end in get func name %s\n", function_name_start); 
  
  // deal with inline function calls, such as if(foo())
  // if (!function_definition_end(line)) {
  if(!function_name_start) {
    // TODO: modify this, ex: int add(int a, int b), 
    // the function_name_end start at ' ' works fine
    // '(' is no good 
    function_name_start = find_substr_backwards(line, function_end_pos, '(');
    if (!function_name_start) {
      return NULL;
    }
  }

  int function_start_pos = function_name_start - line;

  // ex: is_function. space_pos = 4. function_start_pos = 16.
  // len is 11, but the remaining pos will be for null term
  int function_name_len = function_end_pos - function_start_pos;
  char function_name[function_name_len];
  strncpy(function_name, function_name_start + 1, function_name_len - 1);
  function_name[function_name_len - 1] = '\0';

  char *trimmed_function_name = calloc(function_name_len, sizeof(char));
  // remove spaces and copy over to trimmed function name 
  trim_and_copy(trimmed_function_name, function_name);

  // function names and if, while, and for statement share
  // the exact same template, have to exclude them
  if(is_keyword(trimmed_function_name)) {
    free(trimmed_function_name);
    return NULL;
  }

  // but then how to deal with multi line functions??
  // not going to worry about it for now
  return trimmed_function_name;
}

// adds a function's name the array list, if it
// is being called in the line. e.g. if(foo()) will
// add "foo" to the list
char *get_function_dependency(char *line) {
  char *function_start = strstr(line, "(");

  if (function_start == NULL) {
    return NULL;
  }

  // also need to handle function calls that end with
  // (), e.g. if(foo()) needs to return foo
  int function_start_pos = function_start - line;

  char *space_delim = find_substr_backwards(line, function_start_pos, ' '); 
  int space_pos = space_delim - line;

  // need to get
  int function_name_len = function_start_pos - space_pos;
  char function_name[function_name_len];
  strncpy(function_name, space_delim + 1, function_name_len - 1);
  function_name[function_name_len - 1] = '\0';

  char *trimmed_function_name = calloc(function_name_len, sizeof(char));
  trim_and_copy(trimmed_function_name, function_name);

  // have the name, add to list, ensure this function name
  // isn't if, for, while, or do, they share similar structure
  // to a file call.
  if(is_keyword(trimmed_function_name)) {
    // need to check if these guys are calling functions themselves
    // TODO:
    // for now, just going to add pure function calls
    // e.g. int a = foo();
    // doesn't handle inner func calls, like foo(foo())
    printf("todo, function name %s\n", trimmed_function_name);
    free(trimmed_function_name);
    return NULL;
  } else {
    // printf("Function dependency: %s\n", trimmed_function_name);
    return trimmed_function_name;
  }
}

union array_element *get_dependency(char *line) {
  char *dependency_start = strstr(line, INCLUDE);
  int dependency_start_pos = dependency_start - line;

  // if line doesn't start with #include, not dependency
  // or if line doesn't have include keyword
  if (!dependency_start || dependency_start_pos != 0) {
    return NULL;
  }

  const char quote_delim = '\"';
  const char angle_delim = '<';
  const char angle_delim_end = '>';

  // ex: "#include <stdio.h>", jump to get stdio.h
  // get includes with <>
  char *angle_dependency = get_include_name(line, angle_delim, angle_delim_end);

  // get includes with ""
  char *quote_dependency = get_include_name(line, quote_delim, quote_delim);

  // no idea why, but segfaults when allocated on the stack
  union array_element *dependency_element = malloc(sizeof(union array_element));

  dependency_element->str_data =
      angle_dependency ? angle_dependency : quote_dependency;

  printf("%sdependency %s%s\n", ANSI_COLOR_BLUE, dependency_element->str_data,
         ANSI_COLOR_RESET);

  return dependency_element;
}

void print_main(const char *line, const char *filename, size_t num_lines) {
  const char *INT_MAIN_START = "int main(";
  char *main_start = strstr(line, INT_MAIN_START);
  int main_start_pos = main_start - line;

  if (main_start && main_start_pos == 0) {
    printf("%sFile %s is a RUNNER! At line: %ld %s\n", ANSI_COLOR_GREEN,
           filename, num_lines, ANSI_COLOR_RESET);
  }
}

void print_function(char * line, char *current_func_def,
                    int *line_count) {
  char *func_depend = get_function_dependency(line);
  if (func_depend) {
    // TODO: fix get_function_dependency, returning prod+=add
    // we only want add
    if (strlen(current_func_def) == 0 || 
        strcmp(current_func_def, func_depend) == 0) {
      // this is a top level module function
      // if(strcmp(current_func_def, func_depend) == 0) {
      //   printf("current function def == func depend\n"); 
      // }
      printf("%s is a module function lc: %d\n", func_depend, *line_count);
    } else {
      printf("function %s <= %s lc: %d\n", current_func_def, func_depend,
             *line_count);
    }
  }
}

void print_lib_dependencies(struct array_list *lib_dependencies,
                            const char *filename) {
  if (get_size(lib_dependencies) == 0) {
    printf("%s%s has NO dependencies %s\n", ANSI_COLOR_YELLOW, filename,
           ANSI_COLOR_RESET);
  } else {
    printf("%s%s has %d dependencies %s\n", ANSI_COLOR_YELLOW, filename,
           get_size(lib_dependencies), ANSI_COLOR_RESET);
  }
}

// get linecounts && a list of files this file depends on
// e.g. #include <stdio.h>, <stdlib.h>
struct array_list *parse_file(const char *filename, int *line_count) {
  FILE *fptr = fopen(filename, "r");

  if (fptr == NULL) {
    printf("fptr is null in count file. filename %s\n", filename);
    return NULL;
  }

  printf("in parse file, filename %s\n", filename); 

  char buffer[buff_size];
  char *current_func_def = NULL;
  struct array_list *dependencies = create_list();
  // struct array_set *unique_functions = create_set(); 

  bool in_function_definition = false;

  while (fgets(buffer, buff_size, fptr)) {
    // printf("parse file. fgets: %s", buffer);
    *line_count = *line_count + 1;

    if (is_comment(buffer)) {
      continue;
    }

    union array_element *lib_dependency = get_dependency(buffer);
    // printf("before getting function name, filename %s\n", filename); 
    char *possible_func_def = get_function_name(buffer);

    if (possible_func_def && !in_function_definition) {
      current_func_def = possible_func_def;
      in_function_definition = true; 
    } else if (function_definition_end(buffer)) {
      in_function_definition = false;
      current_func_def = ""; 
    }

    if (in_function_definition) {
      print_function(buffer, current_func_def, line_count);
    } else if (lib_dependency) {
      add_element(dependencies, lib_dependency);
      free(lib_dependency);
    } else {
      print_main(buffer, filename, *line_count);
    }
  }

  print_lib_dependencies(dependencies, filename);
  // print_set(set); 

  return dependencies;
}
