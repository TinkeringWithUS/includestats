#include <sys/types.h>
#include <stdbool.h>

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "parseFile.h"

#include "stringHelp.h"
#include "colors.h"
#include "arrayList.h"

// grab substring excluding delim_start and delim_end
// if delim_start == delim_end, will still work (e.g. "hi" returns hi)
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
char * get_function_name(const char * line) {
  // function definitions are usually on one line 
  // assuming that
  char * function_start = strstr(line, "(");
  char * space_delim = strstr(line, " "); 
  char * bracket_start = strstr(line, "{");
  if(!function_start || !space_delim || !bracket_start) {
    return NULL;
  }
  
  int function_start_pos = function_start - line; 
  int space_pos = space_delim - line; 

  // ex: is_function. space_pos = 4. function_start_pos = 16. 
  // len is 11, but the remaining pos will be for null term
  int function_name_len = function_start_pos - space_pos; 
  char * function_name = malloc(sizeof(char) * function_name_len);
  strncpy(function_name, space_delim + 1, function_name_len - 1);
  function_name[function_name_len - 1] = '\0';

  char * trimmed_function_name = calloc(function_name_len, sizeof(char)); 
  remove_spaces(trimmed_function_name, function_name); 

  // printf("before if checks, trimmed function name %s\n", trimmed_function_name); 

  // function names and if, while, and for statement share
  // the exact same template, have to exclude them 
  if(strcmp(trimmed_function_name, "if") == 0 || 
    strcmp(trimmed_function_name, "while") == 0||
    strcmp(trimmed_function_name, "for") == 0 || 
    strcmp(trimmed_function_name, "main") == 0) {
    // if(strcmp(trimmed_function_name, "if") == 0) {
    //   printf("function name is identified as if %s\n", trimmed_function_name);
    // } else if(strcmp(trimmed_function_name, "for") == 0) {
    //   printf("function name identified as for %s\n", trimmed_function_name); 
    // } else if(strcmp(trimmed_function_name, "while") == 0) {
    //   printf("function name identified as while %s\n", trimmed_function_name); 
    // } else if(strcmp(trimmed_function_name, "main") == 0) {
    //   printf("function name identified as main %s\n", trimmed_function_name);
    // }
    // printf("is special keyword %s\n", trimmed_function_name); 
    free(function_name);  
    free(trimmed_function_name);  
    return NULL; 
  }

  printf("function name %s\n", trimmed_function_name); 

  // but then how to deal with multi line functions??
  // not going to worry about it for now 
  return trimmed_function_name; 
}

// adds a function's name the array list, if it 
// is being called in the line. e.g. if(foo()) will 
// add "foo" to the list 
char * get_function_dependency(const char * line) {
  char * function_start = strstr(line, "(");
  char * space_delim = strstr(line, " "); 

  if(function_start == NULL || space_delim == NULL) {
    return NULL; 
  }

  int function_start_pos = function_start - line; 
  int space_pos = space_delim - line; 

  // need to get  
  int function_name_len = function_start_pos - space_pos; 
  char * function_name = malloc(sizeof(char) * function_name_len);
  strncpy(function_name, space_delim + 1, function_name_len - 1);
  function_name[function_name_len - 1] = '\0'; 

  // have the name, add to list, ensure this function name 
  // isn't if, for, while, or do, they share similar structure
  // to a file call.
  if(strcmp("if", function_name) == 0 || strcmp("while", function_name) == 0 ||
    strcmp("for", function_name) == 0) {
    // need to check if these guys are calling functions themselves
    // TODO: do this
    // for now, just going to add pure function calls
    // e.g. int a = foo(); 
    // doesn't handle inner func calls, like foo(foo()) 
    printf("todo, function name %s\n", function_name);  
    return NULL;   
  } else {
    printf("Function dependency: %s\n", function_name); 
    return function_name; 
  }
}

union array_element * get_dependency(char * line) {
  char *dependency_start = strstr(line, INCLUDE);
  int dependency_start_pos = dependency_start - line;

  // if line doesn't start with #include, not dependency 
  // or if line doesn't have include keyword
  if(!dependency_start || dependency_start_pos != 0) {
    return NULL; 
  }

  const char quote_delim = '\"';
  const char angle_delim = '<';
  const char angle_delim_end = '>';

  // ex: "#include <stdio.h>", jump to get stdio.h
  // get includes with <> 
  char *angle_dependency =
      get_include_name(line, angle_delim, angle_delim_end);
  
  // get includes with "" 
  char *quote_dependency =
      get_include_name(line, quote_delim, quote_delim);

  // no idea why, but segfaults when allocated on the stack 
  union array_element * dependency_element = 
      malloc(sizeof(union array_element));

  dependency_element->str_data =
      angle_dependency ? angle_dependency : quote_dependency;
  
  printf("%sdependency %s%s\n", ANSI_COLOR_BLUE, 
        dependency_element->str_data, ANSI_COLOR_RESET);

  return dependency_element;
}

void print_main(const char * line, const char * filename, size_t num_lines) {
  const char * INT_MAIN_START = "int main(";
  char* main_start = strstr(line, INT_MAIN_START);
  int main_start_pos = main_start - line;

  if(main_start && main_start_pos == 0) {
    printf("%s File %s is a RUNNER! At line: %ld %s\n", ANSI_COLOR_GREEN, 
          filename, num_lines, ANSI_COLOR_RESET);
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
  
  char buffer[buff_size];
  struct array_list *dependencies = create_list();

  bool in_function = false; 

  while (fgets(buffer, buff_size, fptr)) {
    // printf("parse file. fgets: %s", buffer);
    *line_count = *line_count + 1; 

    union array_element * dependency = get_dependency(buffer); 

    char * function_name = get_function_name(buffer);

    if(in_function) {
      char * func_depend = get_function_dependency(buffer); 
      printf("function %s depends on %s\n", function_name, func_depend); 
    }

    if(function_name != NULL) {
      printf("function name: %s\n", function_name); 
      in_function = true; 
    }

    if(dependency) {
      add_element(dependencies, dependency);
      free(dependency);
    } 
    else {
      print_main(buffer, filename, *line_count);
    } 
  }

  if(get_size(dependencies) == 0) {
    printf("%s%s has NO dependencies %s\n", ANSI_COLOR_YELLOW, filename, 
          ANSI_COLOR_RESET);

  } else {
    printf("%s%s has %d dependencies %s\n", ANSI_COLOR_YELLOW, filename, 
          get_size(dependencies), ANSI_COLOR_RESET);
  }

  return dependencies;
}