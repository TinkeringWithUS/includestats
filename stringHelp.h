#include <stdbool.h>

void remove_spaces (char* restrict str_trimmed, 
                    const char* restrict str_untrimmed);

char * find_substr_backwards(char * line, int start, 
                          char needle);


bool is_keyword(char * token);