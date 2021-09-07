// This is a personal academic project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>
#include "calculate.h"
#include "stack.h"

// Math constants
#define PI 3.14159265358979323846
#define E 2.71828182845904523536

// Symbols of operators and functions to store in token_t 
#define UNARY_MINUS '~'
#define SIN 's'
#define COS 'c'
#define	TAN 't'
#define CTAN 'x'
#define ARCSIN 'S'
#define ARCCOS 'C'
#define ARCTAN 'T'
#define ARCCOT 'X'
#define SQRT 'r'
#define LN 'l'
#define FLOOR 'f'
#define CEIL 'F'
#define LOG 'L'

// Type to store and process variables
typedef struct {
  char name;
  double value;
} var_t;

//process error, free all memory
static void Error(error_t* errorVar, int errorType, int numOfVarArrs, int numOfTokenArrs, int numOfStacks, ...) {
  va_list args;
  int i;

  assert(errorVar);
  *errorVar = (error_t)errorType;

  va_start(args, numOfStacks);
  for (i = 0; i < numOfVarArrs; ++i) {
    free(va_arg(args, var_t*));
  }
  for (i = 0; i < numOfTokenArrs; ++i) {
    free(va_arg(args, token_t*));
  }
  for (i = 0; i < numOfStacks; ++i) {
    freeStack(va_arg(args, stack_t*));
  }

  va_end(args);
}

//return priority of token if it's operation
static int _priority(const token_t* token) {
  if (token->type == token_t::FUNCTION && token->value.cValue != UNARY_MINUS) {
		return 4;
  }	else if (token->value.cValue == '^') {
		return 3;
  }	else if (token->value.cValue == UNARY_MINUS || token->value.cValue == '*' || token->value.cValue == '/') {
		return 2;
  }	else if (token->value.cValue == '+' || token->value.cValue == '-') {
		return 1;
  } else {
		return 0;
  }
}

static int _findOperator(const char* line, int i, token_t* expression, int j) {
  int isOperator = 1;
  switch (line[i]) {
  case '^':
    expression[j].type = token_t::OPERATOR;
    expression[j].value.cValue = '^';
    break;
  case '*':
    expression[j].type = token_t::OPERATOR;
    expression[j].value.cValue = '*';
    break;
  case '/':
    expression[j].type = token_t::OPERATOR;
    expression[j].value.cValue = '/';
    break;
  case '+':
    expression[j].type = token_t::OPERATOR;
    expression[j].value.cValue = '+';
    break;
  case '-':
    if (i != 0 && (expression[j - 1].type == token_t::OPERAND || expression[j - 1].type == token_t::CLS_BRACKET || expression[j - 1].type == token_t::VAR)) {
      expression[j].type = token_t::OPERATOR;
      expression[j].value.cValue = '-';
    }
    else {
      expression[j].type = token_t::FUNCTION; //formally unary minus is operator but it's convenient to consider it a function
      expression[j].value.cValue = UNARY_MINUS;
    }
    break;
  default:
    isOperator = 0;
    break;
    }
    return isOperator;
}

static int _findOperand(const char* line, int *i, token_t* expression, int j) {
  char* checkPtr;
  int isOperand = 1;
  if (line[*i] == 'e') {
    expression[j].type = token_t::OPERAND;
    expression[j].value.dValue = E;
  }
  else if (line[*i] == 'p' && line[*i + 1] == 'i') {
    expression[j].type = token_t::OPERAND;
    expression[j].value.dValue = PI;
    ++(*i);
  }
  else if ((line[*i] >= '0' && line[*i] <= '9') || line[*i] == '.') {
    expression[j].type = token_t::OPERAND;
    expression[j].value.dValue = strtod(&line[*i], &checkPtr);
    if (checkPtr != &line[*i]) {
      *i += checkPtr - &line[*i] - 1;
    }
    else {
      return -1;
    }
  }
  else {
    isOperand = 0;
  }
  return isOperand;
}

static int _findTrigonometry(const char* line, int *i, token_t* expression, int j) {
  int isThrig = 1;
  if (line[*i] == 's' && line[*i + 1] == 'i' && line[*i + 2] == 'n') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = SIN;
    *i += 2;
  }
  else if (line[*i] == 'c' && line[*i + 1] == 'o' && line[*i + 2] == 's') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = COS;
    *i += 2;
  }
  else if (line[*i] == 't' && line[*i + 1] == 'g') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = TAN;
    *i += 1;
  }
  else if (line[*i] == 'c' && line[*i + 1] == 't' && line[*i + 2] == 'g') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = CTAN;
    *i += 2;
  }
  else if (line[*i] == 'a' && line[*i + 1] == 'r' && line[*i + 2] == 'c' && line[*i + 3] == 's' && line[*i + 4] == 'i' && line[*i + 5] == 'n') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = ARCSIN;
    *i += 5;
  }
  else if (line[*i] == 'a' && line[*i + 1] == 'r' && line[*i + 2] == 'c' && line[*i + 3] == 'c' && line[*i + 4] == 'o' && line[*i + 5] == 's') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = ARCCOS;
    *i += 5;
  }
  else if (line[*i] == 'a' && line[*i + 1] == 'r' && line[*i + 2] == 'c' && line[*i + 3] == 't' && line[*i + 4] == 'g') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = ARCTAN;
    *i += 4;
  }
  else if (line[*i] == 'a' && line[*i + 1] == 'r' && line[*i + 2] == 'c' && line[*i + 3] == 'c' && line[*i + 4] == 't' && line[*i + 5] == 'g') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = ARCCOT;
    *i += 5;
  }
  else {
    isThrig = 0;
  }
  return isThrig;
}
  
static int _findFunction(const char* line, int *i, token_t* expression, int j, char* wasLog) {
  int isFunction = 1;
  if (_findTrigonometry(line, i, expression, j)) {
   
  }
  else if (line[*i] == 's' && line[*i + 1] == 'q' && line[*i + 2] == 'r' && line[*i + 3] == 't') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = SQRT;
    *i += 3;
  }
  else if (line[*i] == 'l' && line[*i + 1] == 'n') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = LN;
    *i += 1;
  }
  else if (line[*i] == 'f' && line[*i + 1] == 'l' && line[*i + 2] == 'o' && line[*i + 3] == 'o' && line[*i + 4] == 'r') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = FLOOR;
    *i += 4;
  }
  else if (line[*i] == 'c' && line[*i + 1] == 'e' && line[*i + 2] == 'i' && line[*i + 3] == 'l') {
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = CEIL;
    *i += 3;
  }
  else if (line[*i] == 'l' && line[*i + 1] == 'o' && line[*i + 2] == 'g') {
    *wasLog = 1;
    expression[j].type = token_t::FUNCTION;
    expression[j].value.cValue = LOG;
    *i += 3;
    while (isspace((unsigned char)line[*i])) {
      ++(*i);
    }
    if (line[*i] != '(') { //make sure that have opening bracket
      return -1;;
    }
    --(*i);
  }
  else {
    isFunction = 0;
  }
  return isFunction;
}

static int _findBracket(const char* line, int i, token_t* expression, int j) {
  int isBracket = 1;
  if (line[i] == '(') {
    expression[j].type = token_t::OPN_BRACKET;
    expression[j].value.cValue = '(';
    if (i != 0 && (expression[i - 1].type == token_t::OPERAND || expression[i - 1].type == token_t::CLS_BRACKET)) {
      return -1;
    }
  }
  else if (line[i] == ')') {
    expression[j].type = token_t::CLS_BRACKET;
    expression[j].value.cValue = ')';
  }
  else {
    isBracket = 0;
  }
  return isBracket;
}

static int _findVar(const char* line, int* i, token_t* expression, int* j, var_t** vars, int* numOfVars, error_t* error) {
  int isVar = 1;
  var_t* auxPtr;
  int k; // iterator
  char name; // to save name of var
  if ((line[*i] >= 'A' && line[*i] <= 'Z') || (line[*i] >= 'a' && line[*i] <= 'z')) {
    name = line[*i];
    ++(*i);
    while (isspace((unsigned char)line[*i])) {
      ++(*i);
    }
    if (line[*i] == '=') {
      for (k = 0; k < *numOfVars; ++k) {  //check repeat variable
        if ((*vars)[k].name == name) {
          Error(error, VAR_OVERRIDE, 1, 1, 0, vars, expression);
          return -1;
        }
      }
      ++(*numOfVars);
      auxPtr = (var_t*)realloc(*vars, *numOfVars * sizeof(var_t));
      if (auxPtr != NULL) {
        *vars = auxPtr;
        auxPtr = NULL;
      }
      else {
        Error(error, MEMORY_ERROR, 1, 1, 0, vars, expression);
        return -1;
      }
      (*vars)[*numOfVars - 1].name = name;
      --(*j);
    }
    else {
      expression[*j].type = token_t::VAR;
      expression[*j].value.cValue = name;
      --(*i);
    }
  }
  else {
    isVar = 0;
  }
  return isVar;
}

// semicolons and commas
static int _findOther(const char* line, int i, token_t* expression, int j, char wasLog, char* wasComma) { 
  int isOther = 1;
  if (line[i] == ',') {
    if (wasLog == 0) {
      return -1;
    }
    *wasComma = 1;
    expression[j].type = token_t::COMMA;
  }
  else if (line[i] == ';') {
    expression[j].type = token_t::SEMICOLON;
  }
  else {
    isOther = 0;
  }
  return isOther;
}

//split string into tokens
static token_t* _parse(const char* line, int* numOfTokens, var_t** vars, int* numOfVars, error_t* error) {
  int i = 0, j = 0;
  token_t* expression = (token_t *)malloc((strlen(line) + 1) * sizeof(token_t)); //array of tokens
  char wasLog = 0; // to check if there was log
  char wasComma = 0; // to check if there was comma
  int check;
  *numOfVars = 0;

  if (expression == NULL || vars == NULL) {
    *error = MEMORY_ERROR;
    free(expression);
    return NULL;
  }
  while (line[i] != 0) {
    //skip is space symbol
    if (isspace((unsigned char)line[i])) {
      ++i;
      continue;
    }

    //find operators, operands, functions, brackets, variables, commas and semicolons
    check = _findOperator(line, i, expression, j);
    if (check == 1) {
      ++i;
      ++j;
      continue;
    }

    check = _findOperand(line, &i, expression, j);
    if (check == 1) {
      ++i;
      ++j;
      continue;
    }
    if (check == -1) {
      Error(error, EXP_ERROR, 1, 1, 0, *vars, expression);
      return NULL;
    }

    check = _findFunction(line, &i, expression, j, &wasLog);
    if (check == 1) {
      ++i;
      ++j;
      continue;
    }
    if (check == -1) {
      Error(error, BRACKETS_ERROR, 1, 1, 0, *vars, expression);
      return NULL;
    }

    check = _findBracket(line, i, expression, j);
    if (check == 1) {
      ++i;
      ++j;
      continue;
    }
    if (check == -1) {
      Error(error, BRACKETS_ERROR, 1, 1, 0, *vars, expression);
      return NULL;
    }

    check = _findVar(line, &i, expression, &j, vars, numOfVars, error);
    if (check == 1) {
      ++i;
      ++j;
      continue;
    }
    if (check == -1) {
      return NULL;
    }

    check = _findOther(line, i, expression, j, wasLog, &wasComma);
    if (check == 1) {
      ++i;
      ++j;
      continue;
    }
    if (check == -1) {
      Error(error, COMMA_ERROR, 1, 1, 0, *vars, expression);
      return NULL;
    }

    //if parser couldn't identify token or error
    Error(error, UNKNOWN_ERROR, 1, 1, 0, *vars, expression);
    return NULL;
  }
  //comma can only be in the logarithm
  if (wasLog != 0 && wasComma == 0) {
    Error(error, ARG_ERROR, 1, 1, 0, *vars, expression);
    return NULL;
  }
  *numOfTokens = j;
  return expression;
}

/* 
 applies upper operation/function in the operators stack to 
 two upper operands in the operand stack and put result into the operation stack
*/
static void _recountStack(stack_t* numStack, stack_t* opStack, error_t* error) {
	double a = 0, b = 0, res = 0;
	token_t op; //auxillary var to save operation
	token_t auxToken;	//token witch will save the result and witch will put in stack
	auxToken.type = token_t::OPERAND;
	auxToken.value.dValue = 0;
	if (StackSize(numStack) != 0 && StackSize(opStack) != 0) { //check stacks aren't empty
		op = Pop(opStack); //get opeation
    if (op.type == token_t::FUNCTION && op.value.cValue != LOG) { //only one argument
			b = Pop(numStack).value.dValue;
		}	else {
			b = Pop(numStack).value.dValue; // the first argument
      if (StackSize(numStack) != 0) { // one more check because stack could become empty
				a = Pop(numStack).value.dValue; // the second argument
      } else {
				op.value.cValue = '0'; //to sign error in default in switch (see below)
      }
		}

    //apply operation/function to argument(s)
		switch (op.value.cValue) {
			case '^':
        if ((a < 0 && floor(b) != b) || (a == 0 && b == 0)) {
          *error = POW_DOMAINE;
        }
				res = pow(a, b);
				break;
			case '*':
				res = a * b;
				break;
			case '/':
        if (b == 0) {
					*error = DIV_ZERO_ERROR;
        }
				res = a / b;
				break;
			case '+':
				res = a + b;
				break;
			case '-':
				res = a - b;
				break;
			case SIN:
				res = sin(b);
				break;
			case COS:
				res = cos(b);
				break;
			case TAN:
				res = tan(b);
				break;
			case CTAN:
				res = 1 / tan(b);
        if (b == 0) {
          *error = CTAN_DOMAINE;
        }
				break;
			case ARCSIN:
        if (b > 1 || b < -1) {
					*error = ARCSIN_DOMAINE;
        }
				res = asin(b);
				break;
			case ARCCOS:
        if (b > 1 || b < -1) {
					*error = ARCCOS_DOMAINE;
        }
				res = acos(b);
				break;
			case ARCTAN:
				res = atan(b);
				break;
			case ARCCOT:
				res = PI/2 - atan(b);
				break;
			case SQRT:
        if (b < 0) {
					*error = SQRT_DOMAINE;
        }
				res = sqrt(b);
				break;
			case LN:
        if (b <= 0) {
					*error = LOG_DOMAINE;
        }
				res = log(b);
				break;
			case FLOOR:
				res = floor(b);
				break;
			case CEIL:
				res = ceil(b);
				break;
			case UNARY_MINUS:
				res = - b;
				break;
      case LOG:
        if (b <= 0 || a == 1 || a <= 0) {
          *error = LOG_DOMAINE;
        }
        res = log(b) / log(a);
        break;
			default:
				*error = UNKNOWN_ERROR;
				res = 0;
        break;
		} 
	}	else { //if at least one stack is empty
			*error = UNKNOWN_ERROR;
			res = 0;
	}

  //put new value in the stack
	auxToken.value.dValue = res;
  if (Push(numStack, &auxToken) == 0) {
		*error = MEMORY_ERROR;
  }
}

//calculate the value of expression
double Calculate (const char* line, error_t* error) {
	int numOfTokens;
	int i = 0; //expression iterator
  int j = 0; //vars iterator
  int k = 0; //for find variable value in array of vars
  token_t aux; // just auxillary token
	double result; //result to return
  var_t* vars = NULL; //array of vars 
  int numOfVars = 0;
	token_t* expression = _parse(line, &numOfTokens, &vars, &numOfVars, error); // get array of tokens
	stack_t* numStack = CreateStack(); //stack of numbers
	stack_t* opStack = CreateStack(); //stack of operations
	if (expression == NULL) {
		freeStack(numStack);
		freeStack(opStack);
		return 0;
	}

	if (numStack == NULL || opStack == NULL) {
    if (numStack != NULL) {
      freeStack(numStack);
    }
    if (opStack != NULL) {
      freeStack(opStack);
    }
		*error = MEMORY_ERROR;
		free(expression);
    free(vars);
		return 0;
	}

	while (i < numOfTokens) { // expressions iteration 
		if (expression[i].type == token_t::OPERAND) {
			if(Push(numStack, &expression[i]) == 0) {			//operands always put into stack of numbers
				Error(error, MEMORY_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
				return 0;
			}
		} 
    else if (expression[i].type == token_t::VAR) { //process variable
      k = 0;
      while (k < numOfVars && vars[k].name != expression[i].value.cValue) { // find variable in array of variables
        ++k;
      }
      if (k == numOfVars) { // did not find, variable is not exist
        Error(error, VAR_EXIST_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
				return 0;
			}
      expression[i].value.dValue = vars[k].value; // copy value of variable to token
      if(Push(numStack, &expression[i]) == 0) {			//operands (including variables) always put into stack of numbers
				Error(error, MEMORY_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
				return 0;
			}
    } 
    else if (expression[i].type == token_t::FUNCTION) {
			if(Push(opStack, &expression[i]) == 0) {			//prefix functions always put into stack of numbers
				Error(error, MEMORY_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
				return 0;
			}
		} 
    else if (expression[i].type == token_t::OPN_BRACKET) {
			if(Push(opStack, &expression[i]) == 0) { //opening brackets always put into stack of numbers
				Error(error, MEMORY_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
				return 0;
			}
		} 
    else if (expression[i].type == token_t::CLS_BRACKET) {
			if (StackSize(opStack) == 0 || StackSize(numStack) == 0) { //can't be in the start of line
        Error(error, BRACKETS_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
				return 0;
			}
      while (Peek(opStack).type != token_t::OPN_BRACKET) { //recounts the stack until the opening bracket appears on top
				_recountStack(numStack, opStack, error);	
				if (*error != NO_ERROR) {
          Error(error, (int)*error, 1, 1, 2, vars, expression, numStack, opStack);
					return 0; 
				}
				if (StackSize(opStack) == 0) {		//if there is closing bracket but no opening, return error
          Error(error, BRACKETS_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
					return 0;
				}
			}
      Pop(opStack);		// delete open bracket
		} 
    else if (expression[i].type == token_t::COMMA) {
      if (StackSize(opStack) == 0 || StackSize(numStack) == 0) {
			  Error(error, UNKNOWN_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
				return 0;
			}
      while (Peek(opStack).type != token_t::OPN_BRACKET) { //recounts the stack until the opening bracket appears on top,
				_recountStack(numStack, opStack, error);	 //but opening bracket don't remove
				if (*error != NO_ERROR) {
					Error(error, (int)*error, 1, 1, 2, vars, expression, numStack, opStack);
					return 0; 
				}
				if (StackSize(opStack) == 0) {		//if there is comma but no log, return error
					Error(error, UNKNOWN_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
					return 0;
				}
			} 
    } 
    else if (expression[i].type == token_t::OPERATOR) {
      if (StackSize(opStack) > 0) {
        aux = Peek(opStack);
        //put an operation in stack if it's priority is higher than of operation on top of stack (expect ^), else recount stack until it's so
        while (_priority(&aux) > _priority(&expression[i]) || (Peek(opStack).value.cValue != '^' && _priority(&aux) == _priority(&expression[i]))) {
          _recountStack(numStack, opStack, error);
          if (*error != NO_ERROR) {
            Error(error, (int)*error, 1, 1, 2, vars, expression, numStack, opStack);
            return 0;
          }
          aux = Peek(opStack);
        }
      }
			if(Push(opStack, &expression[i]) == 0) { // put operation on the stack 
				Error(error, MEMORY_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
				return 0;
			}
		} 
    //if token is semicolom, we need to count all is the stacks and write result to variables array
    else if (expression[i].type == token_t::SEMICOLON) {
      //recount all
      while (StackSize(opStack) != 0) {
		    _recountStack(numStack, opStack, error);
		    if (*error != NO_ERROR) {
			    Error(error, (int)*error, 1, 1, 2, vars, expression, numStack, opStack);
			    return 0; 
		    }
	    }
	    if (StackSize(numStack) != 1) { // num stack must have only one element, it's result
		    Error(error, UNKNOWN_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
		    return 0;
	    }
      vars[j++].value = Pop(numStack).value.dValue; // write result to variable and increase iterator
     }

    // In fact, it's impossible because of all tokens from Parse are correct
    else {
      Error(error, UNKNOWN_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
      return 0;
    }
		++i;
	}
	//caclulate all remains on the stack until operations end
	while (StackSize(opStack) != 0) {
		_recountStack(numStack, opStack, error);
		if (*error != NO_ERROR) {
			Error(error, (int)*error, 1, 1, 2, vars, expression, numStack, opStack);
			return 0; 
		}
	}
	if (StackSize(numStack) != 1) { // num stack must have only one element, it's result
		Error(error, UNKNOWN_ERROR, 1, 1, 2, vars, expression, numStack, opStack);
		return 0;
	}
	result = Peek(numStack).value.dValue; //result will be at the top of the number stack

  //free all memory
	freeStack(numStack);
	freeStack(opStack);
	free(expression);
  free(vars);

	return result;		
}