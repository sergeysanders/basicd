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

#ifndef _BCORE_H_INCLUDED
#define _BCORE_H_INCLUDED

#define PARSER_MAX_TOKENS   32
#define BASIC_VAR_NAME_LEN  16
#define BASIC_VAR_MAX_COUNT 64
#define BASIC_DEFFN_MAX_ARGS 4

#define BASIC_LINE_LEN 120

#define BASIC_GOSUB_STACK_SIZE 16

#include "stdint.h"
#include "rpn.h" // rpn var types
#include "bfunc.h" // status

typedef struct
{
    uint16_t number;
    void *next;
    uint8_t string[BASIC_LINE_LEN];
} _bas_line_t;

typedef struct
{
    uint16_t number;
    uint8_t statement;
} _bas_ptr_t;

typedef struct
{
    void *next;
    float limit;
    float step;
    _bas_ptr_t line;
} _bas_loop_t;

typedef struct
{
    _rpn_type_t value;
    void *next;
    union
    {
        uint8_t argc;       // function arguments count
        uint16_t size[2];   // allocated memory size for complex variables,string - size[0], arrays size[dim 0],size[dim 1]
        _bas_loop_t *loop;  // pointer to loop parameter
    }param;
    char *name;
} _bas_var_t;



uint8_t is_string(char *str);
_bas_var_t *var_get(char *name);
_bas_var_t *var_add(char *name);
_bas_err_e prog_load(char *progFileName);
void prog_list(void);
void prog_new(void);
void prog_run(uint16_t lineNum);
_bas_err_e __new(_rpn_type_t *param);
_bas_err_e __list(_rpn_type_t *param);
_bas_err_e __load(_rpn_type_t *param);
_bas_err_e __run(_rpn_type_t *param);

_bas_err_e array_set(char *name,bool init);

extern _bas_stat_e BasicStat;
extern _bas_err_e BasicError;
extern _bas_ptr_t ExecLine;
extern _bas_ptr_t NextLine;
extern _bas_line_t *BasicProg;

extern uint8_t tmpBasicLine[BASIC_LINE_LEN];

#endif //_BCORE_H_INCLUDED
