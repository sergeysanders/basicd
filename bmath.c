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

/// type functions
_bas_err_e __int(_rpn_type_t *param)
{
    int intVar;
    if (param->type < VAR_TYPE_FLOAT)
        if (param->type == VAR_TYPE_STRING) intVar = (int)strtol(param->var.str,NULL,0);
        else return BasicError = BASIC_ERR_TYPE_MISMATCH;
    else
        intVar = (param->type & VAR_TYPE_INTEGER) ? param->var.i : (int)param->var.f;
    rpn_push_queue(RPN_INT(intVar));
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e __byte(_rpn_type_t *param)
{
    uint8_t intVar;
    if (param->type < VAR_TYPE_FLOAT)
        if (param->type == VAR_TYPE_STRING) intVar = (uint8_t)strtol(param->var.str,NULL,0);
        else return BasicError = BASIC_ERR_TYPE_MISMATCH;
    else
        intVar = (uint8_t)((param->type & VAR_TYPE_INTEGER) ? param->var.i : param->var.f);
    rpn_push_queue(RPN_INT(intVar));
    return BasicError = BASIC_ERR_NONE;
};

/// math functions
_bas_err_e __abs(_rpn_type_t *param)
{
    if (param->type < VAR_TYPE_FLOAT)
        return BasicError = BASIC_ERR_TYPE_MISMATCH;
    else if (param->type & VAR_TYPE_FLOAT)
        rpn_push_queue(RPN_FLOAT(param->var.f < 0 ? -param->var.f : param->var.f));
    else
        rpn_push_queue(RPN_INT(param->var.i < 0 ? -param->var.i : param->var.i));
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e var_float(_rpn_type_t *var)
{
    if (var->type < VAR_TYPE_FLOAT)
        return BasicError = BASIC_ERR_TYPE_MISMATCH;
    if (var->type & VAR_TYPE_INTEGER) var->var.f = (float)var->var.i;
    return BasicError = BASIC_ERR_NONE;
}

_bas_err_e __sin(_rpn_type_t *param)
{
    if (var_float(param) != BASIC_ERR_NONE) return BasicError;
    rpn_push_queue(RPN_FLOAT(sinf(param->var.f)));
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e __cos(_rpn_type_t *param)
{
    if (var_float(param) != BASIC_ERR_NONE) return BasicError;
    rpn_push_queue(RPN_FLOAT(cosf(param->var.f)));
    return BasicError = BASIC_ERR_NONE;
};
_bas_err_e __tan(_rpn_type_t *param)
{
    if (var_float(param) != BASIC_ERR_NONE) return BasicError;
    rpn_push_queue(RPN_FLOAT(tanf(param->var.f)));
    return BasicError = BASIC_ERR_NONE;
};
_bas_err_e __atn(_rpn_type_t *param)
{
    if (var_float(param) != BASIC_ERR_NONE) return BasicError;
    rpn_push_queue(RPN_FLOAT(atanf(param->var.f)));
    return BasicError = BASIC_ERR_NONE;
};
_bas_err_e __sqr(_rpn_type_t *param)
{
    if (var_float(param) != BASIC_ERR_NONE) return BasicError;
    rpn_push_queue(RPN_FLOAT(sqrtf(param->var.f)));
    return BasicError = BASIC_ERR_NONE;
};
_bas_err_e __rnd(_rpn_type_t *param)
{
    #include "time.h"
    static bool seeded = false;
    if (!seeded)
    {
        srand((uint32_t)time(NULL));
        seeded = true;
    }
    if (param->type < VAR_TYPE_FLOAT) return BasicError = param->type ? BASIC_ERR_TYPE_MISMATCH : BASIC_ERR_FEW_ARGUMENTS;
    uint32_t seed = (param->type & VAR_TYPE_FLOAT) ? param->var.f : param->var.i;
    if(seed < 2) seed = 2;
    rpn_push_queue(RPN_FLOAT((float)(rand() % seed)));
    return BasicError = BASIC_ERR_NONE;
};
_bas_err_e __log(_rpn_type_t *param)
{
    if (var_float(param) != BASIC_ERR_NONE) return BasicError;
    rpn_push_queue(RPN_FLOAT(logf(param->var.f)));
    return BasicError = BASIC_ERR_NONE;
};
_bas_err_e __deg(_rpn_type_t *param)
{
    if (var_float(param) != BASIC_ERR_NONE) return BasicError;
    rpn_push_queue(RPN_FLOAT(DEGREES(param->var.f)));
    return BasicError = BASIC_ERR_NONE;
};
_bas_err_e __rad(_rpn_type_t *param)
{
    if (var_float(param) != BASIC_ERR_NONE) return BasicError;
    rpn_push_queue(RPN_FLOAT(RADIANS(param->var.f)));
    return BasicError = BASIC_ERR_NONE;
};
_bas_err_e __min(_rpn_type_t *p1)
{
    _rpn_type_t *p2 = rpn_pop_queue();
    if ((p1->type < VAR_TYPE_FLOAT) || (p2->type < VAR_TYPE_FLOAT))
        return BasicError = (p1->type && p2->type) ? BASIC_ERR_TYPE_MISMATCH : BASIC_ERR_FEW_ARGUMENTS;
    else
    {
        float a = p1->type & VAR_TYPE_FLOAT ? p1->var.f : (float)p1->var.i;
        float b = p2->type & VAR_TYPE_FLOAT ? p2->var.f : (float)p2->var.i;
        rpn_push_queue(a < b ? *p1 : *p2);
    }
    return BasicError = BASIC_ERR_NONE;
};
_bas_err_e __max(_rpn_type_t *p1)
{
    _rpn_type_t *p2 = rpn_pop_queue();
    if ((p1->type < VAR_TYPE_FLOAT) || (p2->type < VAR_TYPE_FLOAT))
        return BasicError = (p1->type && p2->type) ? BASIC_ERR_TYPE_MISMATCH : BASIC_ERR_FEW_ARGUMENTS;
    else
    {
        float a = p1->type & VAR_TYPE_FLOAT ? p1->var.f : (float)p1->var.i;
        float b = p2->type & VAR_TYPE_FLOAT ? p2->var.f : (float)p2->var.i;
        rpn_push_queue(a > b ? *p1 : *p2);
    }
    return BasicError = BASIC_ERR_NONE;
};

/// logic functions

_bas_err_e __and(_rpn_type_t *p2)
{
    _rpn_type_t *p1 = rpn_pop_queue();
    if ((p1->type < VAR_TYPE_INTEGER) || (p2->type < VAR_TYPE_INTEGER))
        return BasicError = (p1->type && p2->type) ? BASIC_ERR_TYPE_MISMATCH : BASIC_ERR_FEW_ARGUMENTS;
    else
        rpn_push_queue(RPN_INT(p1->var.i & p2->var.i));
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e __or(_rpn_type_t *p2)
{
    _rpn_type_t *p1 = rpn_pop_queue();
    if ((p1->type < VAR_TYPE_INTEGER) || (p2->type < VAR_TYPE_INTEGER))
        return BasicError = (p1->type && p2->type) ? BASIC_ERR_TYPE_MISMATCH : BASIC_ERR_FEW_ARGUMENTS;
    else
        rpn_push_queue(RPN_INT(p1->var.i | p2->var.i));
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e __xor(_rpn_type_t *p2)
{
    _rpn_type_t *p1 = rpn_pop_queue();
    if ((p1->type < VAR_TYPE_INTEGER) || (p2->type < VAR_TYPE_INTEGER))
        return BasicError = (p1->type && p2->type) ? BASIC_ERR_TYPE_MISMATCH : BASIC_ERR_FEW_ARGUMENTS;
    else
        rpn_push_queue(RPN_INT(p1->var.i ^ p2->var.i));
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e __sl(_rpn_type_t *p2)
{
    _rpn_type_t *p1 = rpn_pop_queue();
    if ((p1->type < VAR_TYPE_INTEGER) || (p2->type < VAR_TYPE_FLOAT))
        return BasicError = (p1->type && p2->type) ? BASIC_ERR_TYPE_MISMATCH : BASIC_ERR_FEW_ARGUMENTS;
    else
        rpn_push_queue(RPN_INT((uint32_t)p1->var.i << (p2->type & VAR_TYPE_FLOAT ? (uint32_t)p2->var.f : p2->var.i)));
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e __sr(_rpn_type_t *p2)
{
    _rpn_type_t *p1 = rpn_pop_queue();
    if ((p1->type < VAR_TYPE_INTEGER) || (p2->type < VAR_TYPE_FLOAT))
        return BasicError = (p1->type && p2->type) ? BASIC_ERR_TYPE_MISMATCH : BASIC_ERR_FEW_ARGUMENTS;
    else
        rpn_push_queue(RPN_INT((uint32_t)p1->var.i >> (p2->type & VAR_TYPE_FLOAT ? (uint32_t)p2->var.f : p2->var.i)));
    return BasicError = BASIC_ERR_NONE;
};