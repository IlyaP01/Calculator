#ifndef STACK_H
#define STACK_H
#pragma once

//type that put in the stack
//store type of token and its value
typedef struct {
	enum {
		OPERATOR,
		OPERAND,
		OPN_BRACKET,
		CLS_BRACKET,
		FUNCTION,
    COMMA,
    SEMICOLON,
    VAR
	} type;
	union {
		double dValue;
		char cValue;
	} value;
} token_t;
typedef token_t T;

//type for stacks
typedef struct {
	T* data;		
	int size;	  
	int top;	
} stack_t;

/** Create new stack
* @return Pointer to the new stack.
*/
stack_t* CreateStack();

/** Put new element on the top of stack
* @param [in] stack Pointer to stack on witch to put
* @param [in] value Object to be put
* @return {1 if success, 
 *					 0 if fail.
 *					}
*/
int Push(stack_t* stack, const T* value);

/** Get object from the top of the stack (object removes from stack)
* @param [in] stack Pointer to stack on witch to get
* @return Object from the top of the stack.
*/
T Pop(stack_t* stack);

/** Look object in the top of the stack (object DOES NOT remove from stack)
* @param [in] stack Pointer to stack on witch to look
* @return Object from the top of the stack.
*/
T Peek(const stack_t* stack);

/** Find out number of objects in the stack
* @param [in] stack Pointer to stack
* @return  Number of objects in the stack.
*/
int StackSize(stack_t* stack);

/** Remove stack, free memory
* @param [in] stack Pointer to stack to remove
*/
void freeStack(stack_t* stack);

#endif