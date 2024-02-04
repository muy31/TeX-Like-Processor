#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Define the dynamic string structure
typedef struct {
    size_t length;
    char* str;
} DynamicString;

// Function to initialize a dynamic string
DynamicString initializeDynamicString(const char* str) {
    DynamicString dynStr;
    dynStr.length = strlen(str);
    dynStr.str = malloc((dynStr.length + 1) * sizeof(char));
    
    for (size_t i = 0; i < dynStr.length; i++) {
        dynStr.str[i] = str[i];
    }

    dynStr.str[dynStr.length] = '\0';

    return dynStr;
}

void append(DynamicString* str, char ch) {
    str->str = (char *) realloc(str->str, sizeof(char) * (str->length + 1));
    str->str[str->length] = ch;
    str->length++;
    str->str[str->length] = '\0';
}

void appendString(DynamicString* dynStr, char* st) {
    size_t len = strlen(st);
    size_t newlen = dynStr->length + len;

    dynStr->str = (char *) realloc(dynStr->str, sizeof(char) * (newlen + 1));

    for (size_t i = 0; i < len; i++) {
        dynStr->str[i + dynStr->length] = st[i];
    }
    
    dynStr->str[newlen] = '\0';
    dynStr->length = newlen;
}

// Function to split a dynamic string based on a delimiter
DynamicString* splitDynamicString(const DynamicString* input, char * delimiter, size_t* numTokens) {
    // Count the number of tokens (this is because we have to know what to malloc)
    *numTokens = 1;
    size_t delimLen = strlen(delimiter);

    for (size_t i = 0; i < input->length; i++) {
        if (input->str[i] == delimiter[0]) {
            bool isToken = true;

            for (size_t j = 0; j < delimLen; j++) {
                if (delimiter[j] != input->str[i + j]) {
                    isToken = false;
                    break;
                }
            }

            if (isToken) {
                (*numTokens)++;
            }
        }
    }

    // Allocate memory for an array of DynamicString for tokens
    DynamicString* tokens = malloc((*numTokens) * sizeof(DynamicString));

    // Extract tokens
    size_t tokenIndex = 0;
    size_t tokenStart = 0;
    for (size_t i = 0; i <= input->length; i++) {

        bool isToken = true;

        for (size_t j = 0; j < delimLen; j++) {
            if (delimiter[j] != input->str[i + j]) {
                isToken = false;
                break;
            }
        }

        if (i == input->length || isToken) {
            size_t tokenLength = i - tokenStart;
            tokens[tokenIndex].str = malloc((tokenLength + 1) * sizeof(char));
            //strncpy(tokens[tokenIndex].str, input->str + tokenStart, tokenLength);

            for (size_t z = 0; z < tokenLength; z++) {
                    tokens[tokenIndex].str[z] = input->str[tokenStart + z];
            }

            tokens[tokenIndex].str[tokenLength] = '\0';
            tokens[tokenIndex].length = tokenLength;
            tokenStart = i + 1;
            tokenIndex++;
        }
    }

    return tokens;
}

// Function to check if a dynamic string contains a substring
int containsSubstring(const DynamicString* input, const char* substring) {
    return strstr(input->str, substring) != NULL;
}

// Function to check if a dynamic string starts with a prefix
int startsWithPrefix(const DynamicString* input, const char* prefix) {
    return strncmp(input->str, prefix, strlen(prefix)) == 0;
}

// Function to free memory allocated for a dynamic string
void freeDynamicString(DynamicString* dynStr) {
    free(dynStr->str);
}

char* replaceString(char * str, char * replacement, size_t start, size_t end) {
    DynamicString appender = initializeDynamicString("");
    for (size_t i = 0; i < start; i++) {
        append(&appender, str[i]);
    }

    for (size_t i = 0; i < strlen(replacement); i++) {
        append(&appender, replacement[i]);
    }

    //Does not replace end index
    for (size_t i = end; i < strlen(str); i++) {
        append(&appender, str[i]);
    }

    return appender.str;
}

char* replaceAll(char* str, char* replacement, char delimiter) {
    DynamicString appender = initializeDynamicString("");
    for (size_t i = 0; i < strlen(str); i++) {

        bool escaped = false;
        if (i > 0 && str[i - 1] == '\\') {
            escaped = true;
        }

        if (str[i] == delimiter && !escaped) {
            appendString(&appender, replacement);
        }
        else {
            append(&appender, str[i]);
        }
    }
    return appender.str;
}