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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "rpn.h"
#include "bcore.h"
#include "banalizer.h"
#include "bfunc.h"
#include "bprime.h"
#include "bmath.h"
#include "bscreen.h"
#include "bstring.h"
#include "berror.h"

static _bas_err_e __basic_dummy(_rpn_type_t *param);
static _bas_err_e __array(_rpn_type_t *param);
static _bas_err_e __deffn(_rpn_type_t *param);

const _bas_func_t BasicFunction[] =
{
    /// --- basic primary operators (procedures)
    {"rem",__rem},
    /// --- program flow ctrol
    {"if",__if},
    {"then",__basic_dummy},
    {"goto",__goto},
    {"gosub",__gosub},
    {"return",__return},
    {"for",__for},
    {"to",__to},
    {"step",__step},
    {"next",__next},
    {"stop",__stop},
    /// --- input/output
    {"print",__print},
    {"input",__input},
    {"pause",__pause},
    {"sleep",__sleep},
    {"cls",__cls},
    /// --- data manipulation
    {"let",__let},
    {"dim",__dim},
    {"def",__def},
    {"data",__basic_dummy},
    {"read",__basic_dummy},
    /// --- interpreter control
    {"run",__run},
    {"list",__list},
    {"new",__new}, /// all operators after this point should be executed in context of other operators
    /// --- basic secondary operators (functions)
    /// --- input/output format
    {"at",__at},
    {"ink",__ink},
    {"paper",__paper},
    /// --- string/type
    {"val$",__val$},
    {"hex$",__hex$},
    /// --- data type
    {"int",__int},
    {"byte",__byte},
    /// --- math
    {"abs",__abs},
    {"sin",__sin},
    {"cos",__cos},
    {"tan",__tan},
    {"atn",__atn},
    {"sqr",__sqr},
    {"rnd",__rnd},
    {"log",__log},
    {"deg",__deg},
    {"rad",__rad},
    {"min",__min},
    {"max",__max},
    /// --- logic
    {"and#",__and},
    {"or#",__or},
    {"xor#",__xor},
    {"sl#",__sl},
    {"sr#",__sr},
    /// --- no argument functions
    {"inkey",__inkey},
    /// data type 
    {" ",__array}, /// access to arrays element
    {" ",__deffn}, /// implement define function
    //  {NULL,NULL}
};

uint8_t bas_func_opcode(char *name)
{
    for (uint8_t fCnt = 0; fCnt < (__OPCODE_LAST - OPCODE_MASK); fCnt++)
        if (!strcmp(name,BasicFunction[fCnt].name)) return OPCODE_MASK + fCnt;
    return 0;
}

const char *bas_func_name(uint8_t opCode)
{
    if (opCode >= __OPCODE_LAST) return "noOp";
    return BasicFunction[opCode - OPCODE_MASK].name;
}

static _bas_err_e __basic_dummy(_rpn_type_t *param)
{
    BasicError = BASIC_ERR_UNKNOWN_FUNC;
    return BasicError = BASIC_ERR_NONE;
};

static _bas_err_e __array(_rpn_type_t *param)
{
    _bas_var_t *array;
    if (!rpn_find_queue(VAR_TYPE_ARRAY)) return BASIC_ERR_QUEUE_EMPTY; // should not happened
    array = (rpn_peek_queue(false))->var.array; // get array
    bool stringArray = array->value.type == VAR_TYPE_ARRAY_STRING ? true : false;
    _rpn_type_t *var;
    uint16_t tmpArrayPtr, arrayPtr = 0;
    for (uint8_t i=0; i < ((array->param.size[1] && !stringArray) ? 2 : 1); i++)
    {
        if ((var = rpn_peek_queue(false))->type  == VAR_TYPE_NONE) return BasicError = BASIC_ERR_ARRAY_DIMENTION;
        if (!var->type || (var->type < VAR_TYPE_FLOAT)) return BasicError = BASIC_ERR_TYPE_MISMATCH;

        tmpArrayPtr = (var->type < VAR_TYPE_INTEGER) ? (uint16_t)var->var.f : (uint16_t)var->var.i;
        if (tmpArrayPtr >= array->param.size[i]) // check range
            return BasicError = BASIC_ERR_ARRAY_OUTOFRANGE;
        arrayPtr = arrayPtr*array->param.size[1] + tmpArrayPtr;
    }
    if (rpn_peek_queue(false)->type != VAR_TYPE_NONE) return BasicError = BASIC_ERR_ARRAY_DIMENTION; // there is something in stack, too much dimentions
    void *data;
    if (stringArray)
        data = array->value.var.array + arrayPtr*array->param.size[1];
    else
        data = array->value.var.array + arrayPtr*(array->value.type == VAR_TYPE_ARRAY_BYTE ? 1 : 4);
    switch (array->value.type)
    {
    case VAR_TYPE_ARRAY_BYTE:
        rpn_push_queue(RPN_INT(*(uint8_t *)data));
        break;
    case VAR_TYPE_ARRAY_INTEGER:
        rpn_push_queue(RPN_INT(*(int32_t *)data));
        break;
    case VAR_TYPE_ARRAY_FLOAT:
        rpn_push_queue(RPN_FLOAT(*(float *)data));
        break;
    case VAR_TYPE_ARRAY_STRING:
        rpn_push_queue(RPN_STR(data));
        break;
    default:
        return BasicError = BASIC_ERR_TYPE_MISMATCH;
    }
    return BasicError = BASIC_ERR_NONE;
};

static _bas_err_e __deffn(_rpn_type_t *param)
{
    char argVarName[BASIC_VAR_NAME_LEN];
    _bas_var_t *func;
    if (!rpn_find_queue(VAR_TYPE_DEFFN)) return BASIC_ERR_QUEUE_EMPTY; // should not happened
    func = (rpn_peek_queue(false))->var.deffn; // get function tokens
    if (func->param.argc) // there are arguments
    {
        uint8_t indexChr = strlen(func->name);
        _bas_var_t *var;
        strcpy(argVarName,func->name);
        argVarName[indexChr+1]=0;
        for (uint8_t i=0;i<func->param.argc;i++)
        {
            argVarName[indexChr] = '0'+i;
            if ((var = var_get(argVarName)) == NULL) return BasicError = BASIC_ERR_UNKNOWN_VAR;
            var->value = *rpn_peek_queue(false);
            if (var->value.type == VAR_TYPE_NONE) return BasicError = BASIC_ERR_FEW_ARGUMENTS;
        }
        if (rpn_peek_queue(false)->type != VAR_TYPE_NONE) return BasicError = BASIC_ERR_MANY_ARGUMENTS;
    }
    ((_bas_tok_list_t *)func->value.var.deffn)->ptr = -1;
    ((_bas_tok_list_t *)func->value.var.deffn)->parCnt = 0;
    tok_list_push(func->value.var.deffn);
    return BasicError = BASIC_ERR_NONE;
};
