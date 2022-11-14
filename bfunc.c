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

static _bas_stat_e __basic_dummy(_rpn_type_t param){BasicError = BASIC_ERR_UNKNOWN_FUNC;return BasicStat = BASIC_STAT_ERR;};
static _bas_stat_e __abs(_rpn_type_t param){rpn_push_queue(RPN_FLOAT(param.var.f < 0 ? -param.var.f : param.var.f));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __sin(_rpn_type_t param){rpn_push_queue(RPN_FLOAT(sinf(param.var.f)));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __cos(_rpn_type_t param){rpn_push_queue(RPN_FLOAT(cosf(param.var.f)));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __tan(_rpn_type_t param){rpn_push_queue(RPN_FLOAT(tanf(param.var.f)));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __atn(_rpn_type_t param){rpn_push_queue(RPN_FLOAT(atanf(param.var.f)));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __sqr(_rpn_type_t param){rpn_push_queue(RPN_FLOAT(sqrtf(param.var.f)));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __rnd(_rpn_type_t param){rpn_push_queue(RPN_FLOAT(333.33));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __log(_rpn_type_t param){rpn_push_queue(RPN_FLOAT(logf(param.var.f)));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __deg(_rpn_type_t param){rpn_push_queue(RPN_FLOAT(DEGREES(param.var.f)));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __rad(_rpn_type_t param){rpn_push_queue(RPN_FLOAT(RADIANS(param.var.f)));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __min(_rpn_type_t param){_rpn_type_t p2 = *rpn_pop_queue();rpn_push_queue(RPN_FLOAT(p2.var.f > param.var.f ? param.var.f : param.var.f));return BasicStat = BASIC_STAT_OK;};
static _bas_stat_e __max(_rpn_type_t param){_rpn_type_t p2 = *rpn_pop_queue();rpn_push_queue(RPN_FLOAT(p2.var.f < param.var.f ? param.var.f : param.var.f));return BasicStat = BASIC_STAT_OK;};

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
    {"dim",__basic_dummy},
    {"data",__basic_dummy},
    {"read",__basic_dummy},
    /// --- interpreter control 
    {"run",__run},
    {"list",__list},
    {"new",__new}, /// all operators after this point should be executed in context of other operators
    /// --- basic secondary operators  
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
    for (uint8_t fCnt = 0;fCnt < LastOpCode;fCnt++)
        if (!strcmp(name,BasicFunction[fCnt].name)) return OPCODE_MASK + fCnt;
    return 0;
}

const char *bas_func_name(uint8_t opCode)
{
    opCode &= ~OPCODE_MASK;
    if (opCode > LastOpCode) return "noOp";
    return BasicFunction[opCode].name;
}