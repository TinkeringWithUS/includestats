#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <direct.h>

#include <Windows.h>

#include <stdbool.h>

#define buff_size 100

bool read_working_dir(char * path) {
  char * current_dir = _getcwd(path, strlen(path));

  printf("in read working dir. current dir %s\n", current_dir);
  printf("passed in path %s\n", path);

  WIN32_FIND_DATA current_dir_entry; 
  HANDLE fileHandler = FindFirstFile(path, &current_dir_entry);

  if(fileHandler == INVALID_HANDLE_VALUE) {
    return false; 
  }

  do
  {
    if(current_dir_entry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      printf("%s is a directory\n", current_dir_entry.cFileName);
    } else if(current_dir_entry.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) {
      printf("%s is a file\n", current_dir_entry.cFileName);
    }
  } while (FindNextFile(fileHandler, &current_dir_entry) != NULL);
  
  FindClose(fileHandler);
  return true;
}

int main(int argc, char *argv[]) {
  
  const char * path = argv[1];

  FILE * fptr = fopen(path, "r"); 

  char buffer[buff_size]; 

  struct stat stat_buf; 

  // char current_dir[4096];
  char * current_dir = getcwd(path, 4096); 

  // printf("current dir %s\n\n", current_dir);

  if(stat(path, &stat_buf) == 0) {
    if(stat_buf.st_mode & S_IFDIR) {
      printf("it's a directory\n"); 
      read_working_dir(current_dir);
    } else {
      printf("it's a file\n");
    }
  }

  WIN32_FIND_DATA current_dir_entry; 

  size_t num_lines = 0; 

  while (fgets(buffer, buff_size, fptr)) {
    num_lines++;
  }
  
  // printf("file %s has %d lines\n", argv[1], num_lines); 

  return 0;
}