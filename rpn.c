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
#include "bstring.h"
#include "bfunc.h"
#include "berror.h"

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef bool
typedef bool uint8_t;
#endif

const _rpn_type_t VarError = {.type = VAR_TYPE_NONE};

struct
{
    _rpn_type_t value[RPN_QUEUE_LEN];
    uint8_t ptr;
} RPNQueue =
{
    .ptr = 0
};

struct
{
    uint8_t op[RPN_STACK_LEN];
    uint8_t ptr;
} RPNStack =
{
    .ptr = 0
};

_bas_err_e rpn_push_queue(_rpn_type_t value)
{
    if (RPNQueue.ptr < RPN_QUEUE_LEN-1)
    {
        RPNQueue.value[RPNQueue.ptr++] = value;
        return BasicError = BASIC_ERR_NONE;
    }
    return BasicError = BASIC_ERR_QUEUE_FULL;
}

_rpn_type_t *rpn_pop_queue(void)
{
    if (!RPNQueue.ptr)
    {
        BasicError = BASIC_ERR_QUEUE_EMPTY;
        return (_rpn_type_t *)&VarError;
    }
    BasicError = BASIC_ERR_NONE;
    return &RPNQueue.value[--RPNQueue.ptr];
}

_rpn_type_t *rpn_peek_queue(bool head)
{
    static uint8_t peekPtr = 0;
    BasicError = BASIC_ERR_NONE;
    if (head) peekPtr = 0;
    if (!RPNQueue.ptr || peekPtr >= RPNQueue.ptr)
    {
        return (_rpn_type_t *)&VarError;
    }
    return &RPNQueue.value[peekPtr++];
}

_bas_err_e rpn_push_stack(uint8_t op)
{
    if (RPNStack.ptr < RPN_STACK_LEN-1)
    {
        RPNStack.op[RPNStack.ptr++] = op;
        return BasicError = BASIC_ERR_NONE;
    }
    return BasicError = BASIC_ERR_STACK_FULL;
}

uint8_t rpn_pop_stack(void)
{
    return RPNStack.ptr ? RPNStack.op[--RPNStack.ptr] : 0;
}

uint8_t rpn_peek_stack_last(void)
{
    return RPNStack.ptr ? RPNStack.op[RPNStack.ptr-1] : 0;
}

void rpn_print_queue(bool newline)
{
    for (uint8_t i=0; i<RPNQueue.ptr; i++)
        printf("%0.2f ",RPNQueue.value[i].var.f);
    if (newline) printf("\n");
}

void rpn_print_stack(bool newline)
{
    for (uint8_t i=0; i<RPNStack.ptr; i++)
        if ((RPNStack.op[i] > ' ') && (RPNStack.op[i] < '~')) printf("%c ",RPNStack.op[i]);
        else printf("%s ",bas_func_name(RPNStack.op[i]));
    if (newline) printf("\n");
}

void rpn_purge_queue(void)
{
    RPNQueue.ptr = 0;
    RPNStack.ptr = 0;
}

void rpn_purge_stack(void)
{
    RPNStack.ptr = 0;
}

#if 1 // short eval
_bas_err_e rpn_eval(uint8_t op)
{
    _rpn_type_t value[2];
    //if (!op || (op == ' ')) return BASIC_ERR_NONE;
    if (!op || (op == ' ')) return BasicError = BASIC_ERR_PAR_MISMATCH;
    value[0] = *rpn_pop_queue();
    if (op >= OPCODE_MASK)
    {
        if(op < LastOpCode+OPCODE_MASK)
            BasicFunction[op - OPCODE_MASK].func(value[0]);
        else
            return BasicError = BASIC_ERR_UNKNOWN_OP;
        return BasicError;
    }
    if (op != OPERATOR_NOT)
    {
        value[1] = *rpn_pop_queue();
        if (value[0].type >= VAR_TYPE_INTEGER)
        {
            value[0].var.f = (float)value[0].var.i;
            value[0].type = VAR_TYPE_FLOAT;
        }
        if (value[1].type >= VAR_TYPE_INTEGER)
        {
            value[1].var.f = (float)value[1].var.i;
            value[1].type = VAR_TYPE_FLOAT;
        }
    }
    if (BasicError) return BasicError;
    switch(op)
    {
    // conditional equations
    case OPERATOR_MORE:
        value[0].var.i = ((value[1].var.f > value[0].var.f) ? 1 : 0);
        value[0].type = VAR_TYPE_BOOL;
        break;
    case OPERATOR_LESS:
        value[0].var.i = ((value[1].var.f < value[0].var.f) ? 1 : 0);
        value[0].type = VAR_TYPE_BOOL;
        break;
    case OPERATOR_EQUAL:
        value[0].var.i = ((value[1].var.f == value[0].var.f) ? 1 : 0);
        value[0].type = VAR_TYPE_BOOL;
        break;
    case OPERATOR_MORE_EQ:
        value[0].var.i = ((value[1].var.f >= value[0].var.f) ? 1 : 0);
        value[0].type = VAR_TYPE_BOOL;
        break;
    case OPERATOR_NOT_EQ:
        value[0].var.i = ((value[1].var.f != value[0].var.f) ? 1 : 0);
        value[0].type = VAR_TYPE_BOOL;
        break;
    case OPERATOR_LESS_EQ:
        value[0].var.i = ((value[1].var.f <= value[0].var.f) ? 1 : 0);
        value[0].type = VAR_TYPE_BOOL;
        break;
    case OPERATOR_NOT:
        value[0].var.i = (value[0].var.f ? 0 : 1);
        value[0].type = VAR_TYPE_BOOL;
        break;
    case OPERATOR_AND:
        value[0].var.i = ((value[1].var.f && value[0].var.f) ? 1 : 0);
        value[0].type = VAR_TYPE_BOOL;
        break;
    case OPERATOR_OR:
        value[0].var.i = ((value[1].var.f || value[0].var.f) ? 1 : 0);
        value[0].type = VAR_TYPE_BOOL;
        break;
    // Math equations
    case OPERATOR_ADD:
        if ((value[0].type == VAR_TYPE_STRING) && (value[1].type == VAR_TYPE_STRING))
        {
            string_add(value[1].var.str,value[0].var.str);
            return BasicError;
        }
        value[0].var.f = value[1].var.f + value[0].var.f;
        break;
    case OPERATOR_SUB:
        value[0].var.f = value[1].var.f - value[0].var.f;
        break;
    case OPERATOR_MUL:
        value[0].var.f = value[1].var.f * value[0].var.f;
        break;
    case OPERATOR_DIV:
        if (!value[0].var.f) 
            return BasicError = BASIC_ERR_DIV_ZERO;
        value[0].var.f = value[1].var.f / value[0].var.f;
        break;
    case OPERATOR_MOD:
        if (!value[0].var.f) 
            return BasicError = BASIC_ERR_DIV_ZERO;
        value[0].var.f = fmod(value[1].var.f,value[0].var.f);
        break;
    case OPERATOR_PWR:
        value[0].var.f = powf(value[1].var.f,value[0].var.f);
        break;

    default:
        return BasicError = BASIC_ERR_UNKNOWN_OP;
    }
    rpn_push_queue(value[0]);
    return BASIC_ERR_NONE;
}

#else // fast eval
_bas_err_e rpn_eval(uint8_t op)
{
    _rpn_type_t value[2];
    bool doFloat = false;
    //if (!op || (op == ' ')) return BASIC_ERR_NONE;
    if (!op || (op == ' ')) return BasicError = BASIC_ERR_PAR_MISMATCH;
    value[0] = *rpn_pop_queue();
    if (op >= OPCODE_MASK)
    {
        if(op < LastOpCode+OPCODE_MASK)
            BasicFunction[op - OPCODE_MASK].func(value[0]);
        else
            return BasicError = BASIC_ERR_UNKNOWN_OP;
        return BasicError = BASIC_ERR_NONE;
    }
    if (op != OPERATOR_NOT)
    {
        value[1] = *rpn_pop_queue();
        if ((value[0].type == VAR_TYPE_FLOAT) && (value[0].type == VAR_TYPE_FLOAT))
        {
            doFloat = true;
            if (value[0].type > VAR_TYPE_FLOAT) value[0].var.f = (float)value[0].var.i;
            if (value[1].type > VAR_TYPE_FLOAT) value[1].var.f = (float)value[1].var.i;
        }
    }
    if (BasicError) return BasicError;
    switch(op)
    {
    // conditional equations
    case OPERATOR_MORE:
        if (doFloat)
            value[0].var.i = ((value[1].var.f > value[0].var.f) ? 1 : 0);
        else
            value[0].var.i = ((value[1].var.i > value[0].var.i) ? 1 : 0);
        break;
    case OPERATOR_LESS:
        if (doFloat)
            value[0].var.i = ((value[1].var.f < value[0].var.f) ? 1 : 0);
        else
            value[0].var.i = ((value[1].var.i < value[0].var.i) ? 1 : 0);
        break;
    case OPERATOR_EQUAL:
        if (doFloat)
            value[0].var.i = ((value[1].var.f == value[0].var.f) ? 1 : 0);
        else
            value[0].var.i = ((value[1].var.i == value[0].var.i) ? 1 : 0);
        break;
    case OPERATOR_MORE_EQ:
        if (doFloat)
            value[0].var.i = ((value[1].var.f >= value[0].var.f) ? 1 : 0);
        else
            value[0].var.i = ((value[1].var.i >= value[0].var.i) ? 1 : 0);
        break;
    case OPERATOR_NOT_EQ:
        if (doFloat)
            value[0].var.i = ((value[1].var.f != value[0].var.f) ? 1 : 0);
        else
            value[0].var.i = ((value[1].var.i != value[0].var.i) ? 1 : 0);
        break;
    case OPERATOR_LESS_EQ:
        if (doFloat)
            value[0].var.i = ((value[1].var.f <= value[0].var.f) ? 1 : 0);
        else
            value[0].var.i = ((value[1].var.i <= value[0].var.i) ? 1 : 0);
        break;
    case OPERATOR_NOT:
        value[0].var.i = (value[0].var.i ? 1 : 0);
        break;
    case OPERATOR_AND:
        value[0].var.i = ((value[1].var.i && value[0].var.i) ? 1 : 0);
        break;
    case OPERATOR_OR:
        value[0].var.i = ((value[1].var.i || value[0].var.i) ? 1 : 0);
        break;
    // Math equations
    case OPERATOR_ADD:
        if ((value[0].type == VAR_TYPE_STRING) && (value[1].type == VAR_TYPE_STRING))
        {
            string_add(value[1].var.str,value[0].var.str);
            return BasicError;
        }
        if (doFloat)
            value[0].var.f = value[1].var.f + value[0].var.f;
        else
            value[0].var.i = value[1].var.i + value[0].var.i;
        break;
    case OPERATOR_SUB:
        if (doFloat)
            value[0].var.f = value[1].var.f - value[0].var.f;
        else
            value[0].var.i = value[1].var.i - value[0].var.i;
        break;
    case OPERATOR_MUL:
        if (doFloat)
            value[0].var.f = value[1].var.f * value[0].var.f;
        else
            value[0].var.i = value[1].var.i * value[0].var.i;
        break;
    case OPERATOR_DIV:
        if (!value[0].var.f) return BASIC_ERR_DIV_ZERO;
        if (doFloat)
            value[0].var.f = value[1].var.f / value[0].var.f;
        else
            value[0].var.i = value[1].var.i / value[0].var.i;
        break;
    case OPERATOR_MOD:
        if (!value[0].var.f) return BASIC_ERR_DIV_ZERO;
        if (doFloat)
            value[0].var.f = fmod(value[1].var.f,value[0].var.f);
        else
            value[0].var.i = value[1].var.i % value[0].var.i;
        break;
    case OPERATOR_PWR:
        if (doFloat)
            value[0].var.f = powf(value[1].var.f,value[0].var.f);
        else
            value[0].var.i = pow(value[1].var.i,value[0].var.i);
        break;

    default:
        return BasicError = BASIC_ERR_UNKNOWN_OP;
    }
    rpn_push_queue(value[0]);
    return BASIC_ERR_NONE;
}
#endif
