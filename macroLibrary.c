#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "stringDynamic.c"
#include "stateStack.c"

struct MacroDict {
	size_t length;
	char** macroNames;
	char** macroValues;
};

typedef struct MacroDict MacroDict;

void parseString(char* str, StateStack* states, DynamicString* expandedString, MacroDict* dict);

MacroDict initializeMacroDict (){
	MacroDict dict;
	dict.length = 0;
	dict.macroNames = malloc(sizeof(char*));
	dict.macroValues = malloc(sizeof(char*));

	return dict;
}

//Working!
int findMacro(MacroDict* dict, char* name) {
	for (size_t i = 0; i < dict->length; i++) {
		if (strcmp(name, dict->macroNames[i]) == 0) {
			return i;
		}
	}

	return -1;
}

//Working!
void defineMacro(MacroDict* dict, char* name, char* value) {

	int isDef = findMacro(dict, name);

	if (isDef != -1) {
		exit(EXIT_FAILURE); //Attempting to redefine a macro before undefining it
	}
	else {
		
		dict->macroNames = realloc(dict->macroNames, sizeof(char*) * (dict->length + 1));
		dict->macroValues = realloc(dict->macroValues, sizeof(char*) * (dict->length + 1));

		size_t nameLen = strlen(name);
		size_t valueLen = strlen(value);

		dict->macroNames[dict->length] = malloc(sizeof(char) * (nameLen + 1));
		dict->macroValues[dict->length] = malloc(sizeof(char) * (valueLen + 1));

		for (size_t i = 0; i < nameLen; i++) {
			dict->macroNames[dict->length][i] = name[i];
		}
		dict->macroNames[dict->length][nameLen] = '\0';

		for (size_t i = 0; i < valueLen; i++) {
			dict->macroValues[dict->length][i] = value[i];
		}
		dict->macroValues[dict->length][valueLen] = '\0';
		
		dict->length++;
	}
}

//Working!
void undefineMacro(MacroDict* dict, char* name) {
	int macroIndex = findMacro(dict, name);

	if (macroIndex == -1) {
		exit(EXIT_FAILURE); // Thrown because attempting to undefine a non-existent macro
	}
	else {
		dict->length--;
		for (size_t i = (size_t) macroIndex; i < dict->length; i++) {
			//I don't know if this is dangerous, but attempts to redefine this pointer as the next one
			dict->macroNames[i] = dict->macroNames[i + 1];
			dict->macroValues[i] = dict->macroValues[i + 1];
		}

		//Get rid of the old farthest string
		//free(dict->macroNames[dict->length]);
		//free(dict->macroValues[dict->length]);
	}
}

//Working!
char* ifMacro(MacroDict * dict, char * COND, char * THEN, char* ELSE) {
	if (strlen(COND) == 0) {
		return ELSE;
	}
	return THEN;
}

//Working!
char* ifDefMacro(MacroDict* dict, char* name, char* then, char* els) {
	int isDef = findMacro(dict, name);
	if (isDef != -1) {
		return then;
	}
	else {
		return els;
	}
}

DynamicString includeMacro(MacroDict* dict, char* path) {
	FILE* file;
	fopen_s(&file, path, "r");
	if (file == NULL) {
		fprintf(stderr, "Error opening file: %s\n", path);
		exit(EXIT_FAILURE); //Because path name is invalid
	}
	
	DynamicString fileContent = initializeDynamicString("");

	int c;

	while ((c = fgetc(file)) != EOF) {
		// Process each character from the file
		// You can perform your desired operations here
		append(&fileContent, c);
	}

	fclose(file);

	return fileContent;
}

DynamicString expandAfter(MacroDict* dict, char* before, char* after) {
	DynamicString expander = initializeDynamicString("");
	StateStack statesFresh = initializeStateStack();
	pushState(&statesFresh, REG_TEXT);
	
	appendString(&expander, before);

	parseString(after, &statesFresh, &expander, dict);

	return expander;
}


//Working
DynamicString parseNextValue(char* valuesToParse, size_t startIndex) {
	//Assumes receiving string immediately after braces
	int braceVal = 1;
	DynamicString value = initializeDynamicString("");

	size_t i = startIndex;

	while (true) {
		if (valuesToParse[i] == '\\') {
			append(&value, '\\');
			i++;
		}
		else if(valuesToParse[i] == '{') {
			braceVal++;
		}
		else if (valuesToParse[i] == '}') {
			braceVal--;

			if (braceVal == 0) {
				return value;
			}
		}
		append(&value, valuesToParse[i]);
		i++;
	}
}

int numParametersNeeded(MacroDict * dict, char * name) {

	//Check if pred-defined macros
	if (strcmp(name, "def") == 0) {
		return 2;
	}
	else if (strcmp(name, "undef") == 0) {
		return 1;
	}
	else if (strcmp(name, "if") == 0) {
		return 3;
	}
	else if (strcmp(name, "ifdef") == 0) {
		return 3;
	}
	else if (strcmp(name, "include") == 0) {
		return 1;
	}
	else if (strcmp(name, "expandafter") == 0) {
		return 2;
	}

	//Every other should be 1, but check it exists just in case
	int macroToExpand = findMacro(dict, name);
	if (macroToExpand == -1) {
		//Doesn't exist, throw error
		exit(EXIT_FAILURE);
	}

	return 1;
}

char * doMacro(MacroDict* dict, DynamicString name, DynamicString* values) {
	//Check if pre-defined macros
	if (strcmp(name.str, "def") == 0) {
		defineMacro(dict, values[0].str, values[1].str);
		return "";
	}
	else if (strcmp(name.str, "undef") == 0) {
		undefineMacro(dict, values[0].str);
		return "";
	}
	else if (strcmp(name.str, "if") == 0) {
		return ifMacro(dict, values[0].str, values[1].str, values[2].str);
	}
	else if (strcmp(name.str, "ifdef") == 0) {
		return ifDefMacro(dict, values[0].str, values[1].str, values[2].str);
	}
	else if (strcmp(name.str, "include") == 0) {
		DynamicString file = includeMacro(dict, values[0].str);
		return file.str;
	}
	else if (strcmp(name.str, "expandafter") == 0) {
		DynamicString expand = expandAfter(dict, values[0].str, values[1].str);
		return expand.str;
	}

	int macroToExpand = findMacro(dict, name.str);
	if (macroToExpand == -1) {
		//Doesn't exist, throw error
		exit(EXIT_FAILURE);
	}

	char * stringToExpand = dict->macroValues[macroToExpand];

	//Replace instances of # in stringToExpand with values[0].str
	
	char* expandedString = replaceAll(stringToExpand, values[0].str, '#');
	return expandedString;
}

//Working!
DynamicString removeCommentsInitial(DynamicString* string) {
	DynamicString newStr = initializeDynamicString("");

	int state = REG_TEXT;

	for (size_t i = 0; i < string->length; i++) {
		if (state == REG_TEXT) {
			//just append if not comment, escape (leading to escape or macro),
			if (string->str[i] == '\\') {
				state = ESCAPE;
			}
			else if (string->str[i] == '%') {
				state = COMMENT;
			}
			else {
				append(&newStr, string->str[i]);
			}
		}
		else if (state == COMMENT) {
			//Continue until tab (\n)
			while (string->str[i] != '\n' && i < string->length ) {
				i++;
			}
			//Continue until whitespace is removed
			//printf("Char: %d", isspace(string->str[i])); //was this skipped? why not?
			while (isspace(string->str[i])) {
				i++;
			}
			append(&newStr, string->str[i]);
			state = REG_TEXT;
		}
		else if (state == ESCAPE) {
			//Always escape!
			append(&newStr, '\\');
			append(&newStr, string->str[i]);
			state = REG_TEXT;
		}
	}

	return newStr;
}


//NAppends to expandedString
void parseString(char* str, StateStack * states, DynamicString * expandedString, MacroDict * dict) {
	
	for (size_t i = 0; str[i] != '\0'; i++) {
		
		if (peekState(states) == REG_TEXT) {
			//just append if not escape (leading to escape or macro),
			if (str[i] == '\\') {
				pushState(states, ESCAPE);
			}
			else {
				append(expandedString, str[i]);
			}
		}
		else if (peekState(states) == ESCAPE) {
			//Check if next is special char. if it is, escape, otherwise switch to macroName
			if (str[i] == '\\' || str[i] == '#' || str[i] == '{' || str[i] == '}' || str[i] == '%') {
				append(expandedString, '\\');
				append(expandedString, str[i]);
				popState(states); //Exiting from escape
			}
			else if (isalpha(str[i]) || isdigit(str[i])) {
				popState(states); //Exit from escape and treat as macroname
				pushState(states, MACRONAME);
			}
			else {
				append(expandedString, '\\');
				append(expandedString, str[i]);
				popState(states); //Exit from escape char
			}
		}
		else if (peekState(states) == MACRONAME) {
			//read as alphanumeric name
			DynamicString name = initializeDynamicString("");
			DynamicString* values;

			size_t readStart = i - 1;
			size_t j = readStart;

			while (true) {
				if (isalpha(str[j]) || isdigit(str[j])) {
					append(&name, str[j]);
					j++;
				}
				else if (str[j] == '{') {

					//printf("DEBUG| Macroname complete: %s\n", name.str);

					//end of macroname, now time to look at values after determining how many values to look for
					int numArgs = numParametersNeeded(dict, name.str);
					//printf("DEBUG| Numargs expected: %d\n", numArgs);
					values = malloc(sizeof(DynamicString) * numArgs);

					for (int m = 0; m < numArgs; m++) {
						values[m] = parseNextValue(str, j + 1);
						//printf("Value String: %s\n", values[m].str);
						j += values[m].length + 2;
					}

					
					//Debug reached
					for (int m = 0; m < numArgs; m++) {
						printf("DEBUG| %s, %s\n", name.str, values[m].str);
					}
					

					//Do macro replacement on expandedString
					char* macroExpansion = doMacro(dict, name, values);

					//appendString(expandedString, macroExpansion);

					//printf("DEBUG| Current char: %c %c\n", str[readStart - 1], str[j - 1]); //Should be \\ and }
					//printf("DEBUG| Expanded macro: %s\n", macroExpansion);

					str = replaceString(str, macroExpansion, readStart - 1, j);

					//printf("DEBUG| Replaced String: %s\n", str);

					//Then go back to initial start and expand from there.
					i = readStart - 2;

					//printf("%c\n", str[i + 1]);


					/*
					//Or just keep going through
					i = j - 1; //For now just keep expanding forward (this is } char)
					//printf("DEBUG| %c %c %c %d\n", str[i - 1] , str[i], str[i + 1], peekState(states));
					*/

					//Exit state
					int poppedState = popState(states); //Exit out of macroname state

					//printf("DEBUG| %d %d\n\n",poppedState, peekState(states));
					break;
				}
				else {
					exit(EXIT_FAILURE); //Throws error if weird value was found
				}
			}
		}

		//printf("Dynamic String: %s\n", expandedString->str);
		//printf("Current Char: %c\n", str[i]);
		//printf("Peek State: %d\n", peekState(states));
	}
}