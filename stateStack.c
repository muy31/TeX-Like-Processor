#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define REG_TEXT 1
#define ESCAPE 2
#define COMMENT 3
#define MACRONAME 4
#define VALUE 5

//I might need a state stack???
struct states {
	size_t length;
	int* stack;
};

typedef struct states StateStack;

StateStack initializeStateStack() {
	StateStack states;
	states.length = 0;
	states.stack = malloc(sizeof(int)* 1);
	return states;
}

//Used when enetering a new state
void pushState(StateStack * states, int newState) {
	states->length++;
	states->stack = realloc(states->stack, sizeof(int) * (states->length));
	states->stack[states->length - 1] = newState;
}

//Used when exiting escape or comment
int popState(StateStack* states) {
	int state = -1;
	if (states->length > 0) {
		states->length--;
		state = states->stack[states->length];
	}
	return state;
}

int peekState(StateStack * states) {
	int state = -1;
	if (states->length > 0) {
		state = states->stack[states->length - 1];
	}
	return state;
}

void printStack(StateStack states) {
	printf("Printing state stack:");
	for (size_t i = 0; i < states.length; i++) {
		printf(" %d", states.stack[i]);
	}
	printf("\n");
}

void freeStack(StateStack * stack) {
	free(stack->stack);
}