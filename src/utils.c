#include <string.h>  // For using string functions
#include "utils.h"   // Include the header file

// Function to find a substring in a string
char* find_substring_in_string(const char* str, const char* substr) {
    // Use strstr to find the substring
    return strstr(str, substr);
}
