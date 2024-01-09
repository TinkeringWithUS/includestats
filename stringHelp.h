#include <stdbool.h>

void trim_and_copy (char* restrict str_trimmed, 
                    const char* restrict str_untrimmed);

char * find_substr_backwards(char * line, int start, 
                          char needle);


bool is_keyword(char * token);