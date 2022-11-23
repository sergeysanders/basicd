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
#include "bstring.h"
#include "bfunc.h"
#include "bprime.h"
#include "bstring.h"
#include "berror.h"

static _bas_stat_e __basic_dummy(_rpn_type_t param)
{
    BasicError = BASIC_ERR_UNKNOWN_FUNC;
    return BasicStat = BASIC_STAT_ERR;
};
static _bas_stat_e __abs(_rpn_type_t param)
{
    rpn_push_queue(RPN_FLOAT(param.var.f < 0 ? -param.var.f : param.var.f));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __sin(_rpn_type_t param)
{
    rpn_push_queue(RPN_FLOAT(sinf(param.var.f)));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __cos(_rpn_type_t param)
{
    rpn_push_queue(RPN_FLOAT(cosf(param.var.f)));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __tan(_rpn_type_t param)
{
    rpn_push_queue(RPN_FLOAT(tanf(param.var.f)));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __atn(_rpn_type_t param)
{
    rpn_push_queue(RPN_FLOAT(atanf(param.var.f)));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __sqr(_rpn_type_t param)
{
    rpn_push_queue(RPN_FLOAT(sqrtf(param.var.f)));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __rnd(_rpn_type_t param)
{
    rpn_push_queue(RPN_FLOAT(333.33));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __log(_rpn_type_t param)
{
    rpn_push_queue(RPN_FLOAT(logf(param.var.f)));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __deg(_rpn_type_t param)
{
    rpn_push_queue(RPN_FLOAT(DEGREES(param.var.f)));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __rad(_rpn_type_t param)
{
    rpn_push_queue(RPN_FLOAT(RADIANS(param.var.f)));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __min(_rpn_type_t param)
{
    _rpn_type_t p2 = *rpn_pop_queue();
    rpn_push_queue(RPN_FLOAT(p2.var.f > param.var.f ? param.var.f : param.var.f));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __max(_rpn_type_t param)
{
    _rpn_type_t p2 = *rpn_pop_queue();
    rpn_push_queue(RPN_FLOAT(p2.var.f < param.var.f ? param.var.f : param.var.f));
    return BasicStat = BASIC_STAT_OK;
};
static _bas_stat_e __array(_rpn_type_t param);

const _bas_func_t BasicFunction[] =
{
    /// --- basic primary operators
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
    {"pause",__basic_dummy},
    {"inkey$",__basic_dummy},
    {"cls",__basic_dummy},
    /// --- data manipulation
    {"let",__let},
    {"dim",__dim},
    {"data",__basic_dummy},
    {"read",__basic_dummy},
    /// --- interpreter control
    {"run",__run},
    {"list",__list},
    {"new",__new}, /// all operators after this point should be executed in context of other operators
    /// --- basic secondary operators
    {" ",__array}, /// access to arrays element
    /// --- input/output format
    {"at",__basic_dummy},
    {"ink",__basic_dummy},
    {"paper",__basic_dummy},
    /// --- string/type
    {"val$",__val$},
    {"hex$",__hex$},
    {"int",__int},
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
    //  {NULL,NULL}
};

const uint8_t LastOpCode = (sizeof(BasicFunction) / sizeof(_bas_func_t));
uint8_t bas_func_opcode(char *name)
{
    for (uint8_t fCnt = 0; fCnt < LastOpCode; fCnt++)
        if (!strcmp(name,BasicFunction[fCnt].name)) return OPCODE_MASK + fCnt;
    return 0;
}

const char *bas_func_name(uint8_t opCode)
{
    opCode &= ~OPCODE_MASK;
    if (opCode > LastOpCode) return "noOp";
    return BasicFunction[opCode].name;
}

static _bas_stat_e __array(_rpn_type_t param)
{
    _bas_var_t *array = (rpn_peek_queue(true))->var.array; // get array
    bool stringArray = array->value.type == VAR_TYPE_ARRAY_STRING ? true : false;
    _rpn_type_t *var[2];

    if (!array->param.size[1] || stringArray) // 1 or 2 dimentions
    {
        var[0] = &param;
        var[1] = 0;
    }
    else
    {
        var[0] = rpn_pop_queue();
        var[1] = &param;
    }
    uint16_t tmpArrayPtr, arrayPtr = 0;
    for (uint8_t i=0; i < ((array->param.size[1] && !stringArray) ? 2 : 1); i++)
    {
        if(var[i]->type == VAR_TYPE_NONE) return BasicStat = BASIC_STAT_ERR;
        if (var[i]->type < VAR_TYPE_FLOAT)
        {
            BasicError = BASIC_ERR_TYPE_MISMATCH;
            return BasicStat = BASIC_STAT_ERR;
        }
        tmpArrayPtr = (var[i]->type < VAR_TYPE_INTEGER) ? (uint16_t)var[i]->var.f : (uint16_t)var[i]->var.i;
        if (tmpArrayPtr >= array->param.size[i]) // check range
        {
            BasicError = BASIC_ERR_ARRAY_OUTOFRANGE;
            return BasicStat = BASIC_STAT_ERR;
        }
        //arrayPtr = i ? arrayPtr + array->param.size[0] * tmpArrayPtr : tmpArrayPtr;
        arrayPtr = arrayPtr*array->param.size[1] + tmpArrayPtr;
    }
    //rpn_pop_queue(); // drop the array pointer
        if(rpn_pop_queue()->type != VAR_TYPE_ARRAY) // there is something in stack, too much dimentions
        {
            BasicError = BASIC_ERR_ARRAY_DIMENTION;
            return BasicStat = BASIC_STAT_ERR;
        }
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
        BasicError = BASIC_ERR_TYPE_MISMATCH;
    }
    return BasicStat = BasicError ? BASIC_STAT_ERR : BASIC_STAT_OK;
};
