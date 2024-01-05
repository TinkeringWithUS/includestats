#include <stdbool.h>

#include "stringHelp.h"

bool to_remove(const char * character) {
  return *character == ' ' || *character == '*' || *character == '\t' ||
          *character == '}'; 
} 
/*
Credit to 2nd stackoverflow post
https://stackoverflow.com/questions/1726302/remove-spaces-from-a-string-in-c
*/
void remove_spaces (char* restrict str_trimmed, 
                    const char* restrict str_untrimmed) {
  while (*str_untrimmed != '\0') {
    if(!to_remove(str_untrimmed)) {
      *str_trimmed = *str_untrimmed;
      str_trimmed++;
    }
    str_untrimmed++;
  }
  *str_trimmed = '\0';
}