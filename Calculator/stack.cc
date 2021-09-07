// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com
#include <stdlib.h>
#include <assert.h>
#include "stack.h"

#define STACK_INIT_SIZE 5 //stack size during initialization 5
#define STACK_RESIZE 3	//memory allocated during overflow
#define NO_ERROR 1
#define ERROR 0

//reallocated memory during overflow
static int Resize(stack_t* stack) {
	T* ptr;
	stack->size += STACK_RESIZE;
	ptr = (T*)realloc(stack->data, (unsigned)stack->size * sizeof(T));
	if (ptr == NULL) {
		return ERROR;
	}
	stack->data = ptr;
	return NO_ERROR;
}

	
stack_t* CreateStack() {
	stack_t* newStack;
	newStack = (stack_t*)malloc(sizeof(stack_t));
  if (newStack == NULL) {
		return NULL;
  }
	newStack->size = STACK_INIT_SIZE;
	newStack->top = 0;
	newStack->data = (T*)malloc(STACK_INIT_SIZE * sizeof(T));
	if (newStack->data == NULL) {
		free(newStack);
		return NULL;
	}
	return newStack;
}

//puts value (second arg) at the top of stack (first arg)
int Push(stack_t* stack, const T* value) {
  assert(value);
	if (stack->top == stack->size - 1) { //realloc memory during overflow
    if(Resize(stack) == ERROR) {
			return ERROR;
    }
	}
	++stack->top;
	stack->data[stack->top] = *value;
	return NO_ERROR;
}

//get element from the top of stack (element removes)
T Pop(stack_t* stack) {
	--stack->top;
	return stack->data[stack->top + 1];
}

//return element from the top of stack (element saves)
token_t Peek(const stack_t* stack) {
	return stack->data[stack->top];
}

int StackSize(stack_t* stack) {
	return stack->top;
}

//remove stack and free memory
void freeStack(stack_t* stack) {
  if (stack != NULL) {
	  free(stack->data);
  }
	free(stack);
}