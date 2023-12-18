#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>

#include <stdbool.h>
#include <linux/stat.h>
#include <sys/stat.h>

#include <unistd.h>

#include <string.h>

#define buff_size 100

struct file_info {
  // store a list of file dependencies 
  // e.g. like <string.h> <unistd.h> etc
};

size_t count_file(const char * filename) {
  size_t num_lines = 1; 

  char buffer[4096]; 
  FILE * fptr = fopen(filename, "r"); 

  if(fptr == NULL) {
    printf("fptr is null in count file. filename %s\n", filename); 
    return -1; 
  }

  while (fgets(buffer, buff_size, fptr)) {
    num_lines++;
  }

  return num_lines; 
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

      // this is a file, print out the line count with fgets
      printf("filename %s line count: %ld\n", current_dir_entry->d_name, 
              count_file(subdir)); 
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
      printf("it's a file, lc: %ld\n", count_file(path)); 
    } else if(S_ISDIR(stat_buf.st_mode)) {
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