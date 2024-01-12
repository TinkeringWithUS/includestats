#include "stringHelp.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

bool to_remove(const char* character) {
  return *character == ' ' || *character == '*' || *character == '\t' ||
         *character == '}';
}
/*
Credit to 2nd stackoverflow post
https://stackoverflow.com/questions/1726302/remove-spaces-from-a-string-in-c
*/
void trim_and_copy(char* restrict str_trimmed,
                   const char* restrict str_untrimmed) {
  while (*str_untrimmed != '\0') {
    if (!to_remove(str_untrimmed)) {
      *str_trimmed = *str_untrimmed;
      str_trimmed++;
    }
    str_untrimmed++;
  }
  *str_trimmed = '\0';
}

// give a string, find first character NEEDLE
// starting at START in the string
char* find_substr_backwards(char* line, int start, char needle) {
  for (int i = start; i >= 0; i--) {
    if (*(line + i) == needle) {
      return line + i;
    }
  }
  return NULL;
}

bool is_keyword(char* token) {
  static const char* keywords[] = {"if", "for", "while", "main"};
  static const int num_keywords = 4;

  for (int keyword = 0; keyword < num_keywords; keyword++) {
    if (strcmp(token, keywords[keyword]) == 0) {
      return true;
    }
  }
  return false;
}