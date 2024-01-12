#include <dirent.h>
#include <linux/stat.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "arrayList.h"

#include "parseFile.h"

#include "colors.h"

#include <errno.h>

// Don't worry about this for now, would take some work. 
// struct file {
//   struct dir * parent_dir; 
//   struct array_list * dependencies; 
//   char * filename; 
//   size_t linecount; 
// };

// struct dir {
//   struct dir * parent_dir; 
//   struct array_list * contents;
//   char * dirname; 
// };

// enum node_type {
//   file, 
//   dir,
// };

// struct file_tree_node {
//   enum node_type type; 
//   union  
// };

// struct file_tree {

// };

struct array_list * files; 

bool skip_dir(char *dir_name) {
  char *dir_ignore[4] = {".", "..", ".git", ".vscode"};

  for (int dir = 0; dir < 4; dir++) {
    if (strcmp(dir_ignore[dir], dir_name) == 0) {
      return true;
    }
  }
  return false;
}

char *subdir_path(char *entry_name, const char *parent_dir) {
  // +2 1 for null term and another for /, dont always need a /, so +1 is fine
  // occassionally
  // can eliminate the 2nd case internal frag with realloc 
  size_t subdir_path_length = strlen(parent_dir) + strlen(entry_name) + 2;
  char *subdir_path = calloc(subdir_path_length, sizeof(char));

  strncat(subdir_path, parent_dir, strlen(parent_dir));

  if (subdir_path[strlen(subdir_path) - 1] != '/') {
    strncat(subdir_path, "/", strlen("/") + 1);
  }

  strncat(subdir_path, entry_name, subdir_path_length);

  return subdir_path;
}

char * get_file_extension(const char * filename) {
  char * extension = strstr(filename, "."); 

  if(!extension) {
    return NULL; 
  }
  return extension;
}


bool is_accepted_file_type(const char * filename) {
  const static char *ACCEPTED_FILE_EXTENSION[3] = {".c", ".h", ".txt"};

  char * extension = get_file_extension(filename); 

  if(!extension) {
    return NULL;
  }

  for(int i = 0; i < 3; i++) {
    if(strcmp(extension, ACCEPTED_FILE_EXTENSION[i]) == 0) {
      return true; 
    }
  }

  return false; 
}

bool analyze_dir(const char *dir_path) {
  struct stat stat_buff;

  if (stat(dir_path, &stat_buff) == 0) {
    if (S_ISREG(stat_buff.st_mode)) {
      // we hit a file, parse it and print contents
      int line_count = 0;
      printf("dir path in analyze dir base case: %s\n", dir_path);
      // loop backwards,
      if(is_accepted_file_type(dir_path)) {
        struct array_list *dependencies = parse_file(dir_path, &line_count);
      }
      
      // struct file * root_file = malloc(sizeof(struct file)); 
      
      // root_file->dependencies = dependencies; 
      // root_file->filename = malloc(strlen(dir_path) + 1); 
      // strncpy(root_file->filename, dir_path, strlen(dir_path));

      // root_file->linecount = line_count; 
      // root_file->parent_dir = NULL; 

      return true;
    }
  }

  DIR *current_dir = opendir(dir_path);

  printf("analyze dir path %s\n", dir_path);

  if (current_dir == NULL) {
    return false;
  }

  struct dirent *current_dir_entry;

  while ((current_dir_entry = readdir(current_dir)) != NULL) {
    if (current_dir_entry->d_type == DT_UNKNOWN) {
      // resort to stat
    }
    if (current_dir_entry->d_type == DT_DIR) {
      // recursive call
      printf("subdir name %s\n", current_dir_entry->d_name);

      if (skip_dir(current_dir_entry->d_name)) {
        continue;
      }

      printf("calling analyze dir on subdir\n");

      char *subdir = subdir_path(current_dir_entry->d_name, dir_path);

      analyze_dir(subdir);
      free(subdir);
    } else if (current_dir_entry->d_type == DT_REG) {
      char *subdir = subdir_path(current_dir_entry->d_name, dir_path);
      
      int linecount = 0;
      if(is_accepted_file_type(subdir)) {
        struct array_list *dependencies = parse_file(subdir, &linecount);
      } else {
        printf("%sIgnoring this file, not supported%s\n", ANSI_COLOR_RED,
               ANSI_COLOR_RED);
      }

      // this is a file, print out the line count with fgets
      printf("filename %s%s line count: %d %s\n", current_dir_entry->d_name,
            ANSI_COLOR_RED, linecount, ANSI_COLOR_RESET);
      // count_file(subdir));
      free(subdir);
    }
  }

  closedir(current_dir);
  return true;
}

void print_stat_error() {
  switch (errno)
  {
    case EACCES:
      printf("no access\n"); 
      break;
    case EBADF: 
      printf("invalid fd\n"); 
      break;
    case EFAULT: 
      printf("bad addr\n"); 
      break;
    case EINVAL:
      printf("invalid flag\n"); 
      break;
    case ELOOP:
      printf("too many sym links\n"); 
      break;
    case ENAMETOOLONG:
      printf("path name too long\n"); 
      break;
    case ENOENT:
      printf("component of pathname does not exist\n"); 
      break;
    case ENOMEM:
      printf("no mem\n"); 
      break;
    case ENOTDIR:
      printf("component of pathname not dir\n"); 
      break;
    case EOVERFLOW:
      printf("overflow\n"); 
      break; 
    default:
      break;
  }
}

void init_files() {
  create_list(files);
}

int main(int argc, char *argv[]) {
  char *path = argv[1];

  struct stat stat_buf;

  // printf("before path check, path %s\n", path);
  if (path == NULL) {
    path = getcwd(path, buff_size);
  }

  // TODO: 
  /*
  1. main finder, program to seek out main functions, e.g. which source file
  actually runs the code 
  2. function linker, functions have pointers to function dependencies 
  (a lot of work, so maybe not now)
  3. once we have all the dependencies, construct the dir tree, each node
  is either a subdir or a file with their dependencies 
  4. using this information, generate a graph (display with react??) 
  5. perform these operations for github repos, so linking with nodejs 
  endgoal, make understanding codebases easier 
  */


  init_files(); 

  char current_dir[buff_size];
  getcwd(current_dir, buff_size);

  printf("current dir %s. argv[1], or the path: %s\n", current_dir, path);

  // struct file current_file;
  int stat_code = stat(path, &stat_buf); 

  if(stat_code == -1) {
    printf("Input is bad, got an error, path %s\n", path); 
    print_stat_error(); 
    return 0; 
  } 
  
  if (stat_code == 0) {
    if (S_ISREG(stat_buf.st_mode)) {
      int linecount = 0;
      struct array_list *lib_dependencies = parse_file(path, &linecount);
      printf("it's a file,%s line count: %d%s\n", ANSI_COLOR_RED, linecount, 
              ANSI_COLOR_RESET);

      // out of dependencies, let's construct struct file infos from them
      printf("file dependencies\n");
      print_list(lib_dependencies);

    } else if (S_ISDIR(stat_buf.st_mode)) {
      printf("it's a directory\n");

      char *current_path = "/";
      char dest[strlen(current_path) + strlen(path) + 1];
      dest[0] = '\0';

      // add 1 for null term
      strncat(dest, path, strlen(path) + 1);
      strncat(dest, current_path, strlen(current_path) + 1);

      printf("dest path %s\n", dest);

      analyze_dir(dest);
    }
  }

  return 0;
}