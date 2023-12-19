#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>

#include <stdbool.h>
#include <linux/stat.h>
#include <sys/stat.h>

#include <unistd.h>

#include <string.h>

#include "arrayList.h"

#define buff_size 4096

#define INCLUDE "#include "

struct file_info {
  // store a list of file dependencies 
  // e.g. like <string.h> <unistd.h> etc
  // store links to file_info ptrs
  struct array_list * dependencies; 

  // num times this file has been referenced 
  int reference_count; 
};

// grab substring excluding delim_start and delim_end 
// if delim_start == delim_end, will still work (e.g. "hi" returns hi)
char * get_include_name(char * include_line, char delim_start, char delim_end) {
  
  int start_pos = -1; 
  int end_pos = -1; 

  int include_length = strlen(include_line); 
  for (size_t i = 0; i < include_length; i++) {
    if(start_pos == -1 && include_line[i] == delim_start) {
      start_pos = i; 
    } else if (include_line[i] == delim_end) {
      end_pos = i; 
    }
  }

  if(start_pos < 0 || end_pos < 0) {
    return NULL; 
  }
  
  // extra length needed for null terminator is already included  
  char * include_name = calloc(end_pos - start_pos, sizeof(char)); 

  int include_name_index = 0; 

  for (size_t i = start_pos + 1; i < end_pos; i++) {
    include_name[include_name_index++] = include_line[i]; 
  }
  return include_name; 
}

// get linecounts && a list of files this file depends on 
// e.g. #include <stdio.h>, <stdlib.h> 
struct array_list * parse_file(const char * filename, int * line_count) {
  char buffer[buff_size]; 
  FILE * fptr = fopen(filename, "r"); 

  if(fptr == NULL) {
    printf("fptr is null in count file. filename %s\n", filename); 
    return -1; 
  }

  size_t num_lines = 0; 

  struct array_list * dependencies = create_list(); 

  const char quote_delim = '\"'; 
  const char angle_delim = '<'; 
  const char angle_delim_end = '>'; 

  while (fgets(buffer, buff_size, fptr)) {
    // printf("count file. fgets: %s", buffer);
    num_lines++;
  
    // ex: "#include <stdio.h>", jump to get stdio.h 
    char * dependency_start = strstr(buffer, INCLUDE);
    int dependency_start_pos = dependency_start - buffer;  
    if(dependency_start && dependency_start_pos == 0) {
      // printf("buffer %s linecount %d\n", buffer, num_lines);
      char * angle_dependency = get_include_name(buffer, angle_delim, 
                                                angle_delim_end); 
      char * quote_dependency = get_include_name(buffer, quote_delim, 
                                                quote_delim);
      
      union array_element dependency_name; 
      dependency_name.str_data = angle_dependency ? angle_dependency : 
                                quote_dependency; 

      add_element(dependencies, &dependency_name);

      printf("dependency %s\n", dependency_name.str_data); 
    }
  }
  
  printf("%s has %d dependencies\n", filename, get_size(dependencies));

  *line_count = num_lines; 
  return dependencies; 
}

bool skip_dir(char * dir_name) {
  char * dir_ignore[4] = {
    ".", "..", ".git", ".vscode"
  };

  for(int dir = 0; dir < 4; dir++) {
    if(strcmp(dir_ignore[dir], dir_name) == 0) {
      return true; 
    }
  }
  return false; 
}

char * subdir_path(char * entry_name, const char * parent_dir) {
  size_t subdir_path_length = strlen(parent_dir) + strlen(entry_name) + 1; 
  char * subdir_path = calloc(sizeof(char), subdir_path_length);   

  strncat(subdir_path, parent_dir, strlen(parent_dir) + 1); 
  
  if(subdir_path[strlen(subdir_path) - 1] != '/') {
    strncat(subdir_path, "/", strlen("/") + 1);
  }
  
  strncat(subdir_path, entry_name, subdir_path_length);

  return subdir_path; 
}

bool analyze_dir(const char * dir_path) {
  DIR * current_dir = opendir(dir_path);   

  printf("analyze dir path %s\n", dir_path); 

  if(current_dir == NULL) {
    return false; 
  }

  struct dirent * current_dir_entry; 

  while ((current_dir_entry = readdir(current_dir)) != NULL) {
    if(current_dir_entry->d_type == DT_UNKNOWN) {
      // resort to stat
    }
    if(current_dir_entry->d_type == DT_DIR) {
      // recursive call
      printf("subdir name %s\n", current_dir_entry->d_name); 

      if(skip_dir(current_dir_entry->d_name)) {
        continue;
      }

      printf("calling analyze dir on subdir\n"); 
      
      char * subdir = subdir_path(current_dir_entry->d_name, dir_path); 

      analyze_dir(subdir);
      free(subdir);  
    } else if(current_dir_entry->d_type == DT_REG) {
      char * subdir = subdir_path(current_dir_entry->d_name, dir_path); 

      analyze_dir(subdir);

      int linecount = 0; 
      struct array_list * dependencies = parse_file(subdir, &linecount); 

      // this is a file, print out the line count with fgets
      printf("filename %s line count: %ld\n", current_dir_entry->d_name, 
              linecount);
              // count_file(subdir)); 
      free(subdir);  
    } 
  }
  
  closedir(current_dir); 
  return true;
}

int main(int argc, char *argv[]) {
  
  char * path = argv[1];

  FILE * fptr = fopen(path, "r"); 

  char buffer[buff_size]; 

  struct stat stat_buf; 

  // char current_dir[4096];
  char * current_dir = getcwd(path, 4096); 

  printf("current dir %s. argv[1], or the path: %s\n", current_dir, path);

  if(stat(path, &stat_buf) == 0) {
    if(S_ISREG(stat_buf.st_mode)) {
      int linecount = 0; 
      parse_file(path, &linecount); 
      printf("it's a file, lc: %ld\n", linecount); 
    } 
    else if(S_ISDIR(stat_buf.st_mode)) {
      printf("it's a directory\n"); 
      
      char * current_path = "/"; 
      char dest[strlen(current_path) + strlen(path) + 1];
      for(int i = 0; i < strlen(current_path) + strlen(path) + 1; i++) {
        dest[i] = '\0';
      }

      // add 1 for null term
      strncat(dest, path, strlen(path) + 1); 
      strncat(dest, current_path, strlen(current_path) + 1); 

      printf("dest path %s\n", dest);

      analyze_dir(dest); 
    }
  }
  
  // printf("file %s has %d lines\n", argv[1], num_lines); 

  return 0;
}