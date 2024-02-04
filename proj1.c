#include <stdio.h>
#include <stdlib.h>
#include "macroLibrary.c"

void processFile(FILE * file) {
    
    DynamicString buffer = initializeDynamicString("");
 
    int c;

    while ((c = fgetc(file)) != EOF) {
        // Process each character from the file
        // You can perform your desired operations here
        append(&buffer, c);
    }

    //See original file
    //printf("\nComments included:\n%s\n", buffer.str);

    //Remove comments and view
    DynamicString commentRemoved = removeCommentsInitial(&buffer);
    //printf("\nWithout comments:\n%s\n\n", commentRemoved.str);

    //Create dictionary and 'final' string. Append. View
    MacroDict dictionary = initializeMacroDict();

    StateStack initialStack = initializeStateStack();
    pushState(&initialStack, REG_TEXT);

    DynamicString expansion = initializeDynamicString("");

    parseString(commentRemoved.str, &initialStack, &expansion, &dictionary);

    printf("%s", expansion.str);

    freeStack(&initialStack);
    //freeDynamicString(&buffer);
    //freeDynamicString(&commentRemoved);
    //freeDynamicString(&expansion);
}

int main(int argc, char ** argv)
{
    if (argc > 1) {
        // Process each file argument
        for (int i = 1; i < argc; i++) {
            FILE* file;
            fopen_s(&file, argv[i], "r");
            if (file == NULL) {
                fprintf(stderr, "Error opening file: %s\n", argv[i]);
                continue;
            }
            processFile(file);
            fclose(file);
        }
    }
    else {
        // Read from stdin if no file arguments are provided
        processFile(stdin);
    }
    return 0;
}