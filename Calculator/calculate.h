#ifndef CALCULATE_H
#define CALCULATE_H
#pragma once

/**
* @brief Error type.
*
* Type for storing and process calculate errors.
*/
typedef enum {	
	NO_ERROR,
	MEMORY_ERROR,
	BRACKETS_ERROR,
	DIV_ZERO_ERROR,
	TAN_DOMAINE,
	CTAN_DOMAINE,
	ARCSIN_DOMAINE,
	ARCCOS_DOMAINE,
	LOG_DOMAINE,
	SQRT_DOMAINE,
	POW_DOMAINE,
  EXP_ERROR,
  ARG_ERROR,
  COMMA_ERROR,
  VAR_ERROR,
  VAR_EXIST_ERROR,
  VAR_OVERRIDE,
	UNKNOWN_ERROR
} error_t;

/** Calculate expression
* @param [in] line Line containing expression
* @param [in] error Variable to write errors
* @return Value of the expression.
*/
double Calculate(char const* line, error_t* error);
#endif