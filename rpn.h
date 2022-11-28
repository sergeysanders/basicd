/**
 * Copyright (c) 2022 Sergey Sanders
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _RPN_H_INCLUDED
#define _RPN_H_INCLUDED

#include "stdint.h"
#include "berror.h"

#define RPN_QUEUE_LEN   32
#define RPN_STACK_LEN   32

#define DEGREES(rad)    ((rad) * 180.0 / M_PI)
#define RADIANS(deg)    ((deg) * M_PI / 180.0)
/// Math operators
#define OPERATOR_PWR        '^'
#define OPERATOR_MUL        '*'
#define OPERATOR_DIV        '/'
#define OPERATOR_MOD        '%'
#define OPERATOR_ADD        '+'
#define OPERATOR_SUB        '-'
/// Logic operators
#define OPERATOR_AND        '&'
#define OPERATOR_OR         '|'
#define OPERATOR_NOT        '!'
/// Condition operators
#define OPERATOR_MORE       '>'
#define OPERATOR_LESS       '<'
#define OPERATOR_EQUAL      '='
#define OPERATOR_MORE_EQ    '~'
#define OPERATOR_NOT_EQ     '@'
#define OPERATOR_LESS_EQ    '`'

typedef enum
{
    VAR_TYPE_NONE,             
    VAR_TYPE_ARRAY,             // Pointer to an array
    VAR_TYPE_ARRAY_FLOAT = 0x08,// multi-byte variables
    VAR_TYPE_ARRAY_INTEGER,
    VAR_TYPE_ARRAY_BYTE,
    VAR_TYPE_ARRAY_STRING,
    VAR_TYPE_STRING = 0x10,     // string,  variable name ended with '$'
    VAR_TYPE_FLOAT = 0x20,      // numeric variables
    VAR_TYPE_LOOP,              // for loop variable, .size points to the loop descriptor  
    VAR_TYPE_INTEGER = 0x40,    // integer, variable name ended with '#'
    VAR_TYPE_BYTE,              // byte,  variable name ended with '_'
    VAR_TYPE_BOOL,
} _var_type_e;

typedef struct
{
    _var_type_e type;
    union
    {
        float       f;
        int32_t     i;
        uint32_t    w;
        char*       str;
        void*       array;
    } var;
} _rpn_type_t;

#define RPN_FLOAT(x)    ((_rpn_type_t){.type = VAR_TYPE_FLOAT,.var.f = x})
#define RPN_INT(x)      ((_rpn_type_t){.type = VAR_TYPE_INTEGER,.var.i = x})
#define RPN_BYTE(x)      ((_rpn_type_t){.type = VAR_TYPE_BYTE,.var.i = x})
#define RPN_STR(x)      ((_rpn_type_t){.type = VAR_TYPE_STRING,.var.str = x})

_bas_err_e rpn_push_queue(_rpn_type_t var);
_rpn_type_t *rpn_pop_queue(void);
_rpn_type_t *rpn_peek_queue(bool head);

_bas_err_e rpn_push_stack(uint8_t op);
uint8_t rpn_pop_stack(void);
uint8_t rpn_peek_stack_last(void);
_bas_err_e rpn_eval(uint8_t op);

void rpn_print_queue(bool newline);
void rpn_print_stack(bool newline);

void rpn_purge_queue(void);
void rpn_purge_stack(void);

#endif //_RPN_H_INCLUDED
