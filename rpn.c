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

const _rpn_type_t VarNone = {.type = VAR_TYPE_NONE,.var.i=0};

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

static uint8_t peekPtr = 0;
static uint8_t peekLast = 0;

_bas_err_e rpn_push_queue(_rpn_type_t value)
{
    if (RPNQueue.ptr < RPN_QUEUE_LEN-1)
    {
        RPNQueue.value[RPNQueue.ptr++] = value;
        return BasicError = BASIC_ERR_NONE;
    }
    return BasicError = BASIC_ERR_QUEUE_FULL;
}

_rpn_type_t *rpn_pull_queue(void)
{
    if (!RPNQueue.ptr)
    {
        BasicError = BASIC_ERR_QUEUE_EMPTY;
        return (_rpn_type_t *)&VarNone;
    }
    BasicError = BASIC_ERR_NONE;
    return &RPNQueue.value[--RPNQueue.ptr];
}

bool rpn_find_queue(_var_type_e varType)
{
    BasicError = BASIC_ERR_NONE;
    if (!RPNQueue.ptr) return false;
    peekPtr = RPNQueue.ptr;
    do
    {
        if(RPNQueue.value[--peekPtr].type == varType) 
            {
                peekLast = RPNQueue.ptr;
                RPNQueue.ptr = peekPtr; // queue data after requested value will be taken by peek_queue
                return true;
            }
    }
    while (peekPtr);
    return false;
}

_rpn_type_t *rpn_peek_queue(bool head)
{
    BasicError = BASIC_ERR_NONE;
    if (head) 
    {
        peekPtr = 0;
        peekLast = RPNQueue.ptr;
        RPNQueue.ptr = 0; // flush queue, the data will be read by peek_queue
    }
    if (peekPtr >= peekLast)
    {
        return (_rpn_type_t *)&VarNone;
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
        switch (RPNQueue.value[i].type)
        {
        case VAR_TYPE_FLOAT:
        case VAR_TYPE_LOOP:
            printf("%.02f",RPNQueue.value[i].var.f);
            break;
        case VAR_TYPE_INTEGER:
        case VAR_TYPE_BYTE:
            printf("%d",RPNQueue.value[i].var.i);
            break;
        case VAR_TYPE_BOOL:
            printf("%s",RPNQueue.value[i].var.i ? "true":"false");
            break;
        case VAR_TYPE_STRING:
            printf("%s",RPNQueue.value[i].var.str);
            break;
        default:
            printf("Var of type %d",RPNQueue.value[i].type);
            break;
        }
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

_bas_err_e rpn_eval(uint8_t op)
{
    _rpn_type_t value[2];
    bool doFloat = false;
    if (!op || (op == ' ')) return BasicError = BASIC_ERR_PAR_MISMATCH;
    if (op < __OPCODE_ARRAY) value[0] = *rpn_pull_queue(); // arrays and deffn have variable number of params
    if (op >= OPCODE_MASK)
    {
        if(op < __OPCODE_LAST)
            return BasicFunction[op - OPCODE_MASK].func(&value[0]);
        else
            return BasicError = BASIC_ERR_UNKNOWN_OP;
    }
    if (value[0].type & VAR_TYPE_ARRAY) // arrays cannot be processed, their members only
        return BasicError = BASIC_ERR_TYPE_MISMATCH;
    if (op != OPERATOR_NOT)
    {
        value[1] = *rpn_pull_queue();
        if ((value[1].type < VAR_TYPE_STRING) || ((value[0].type == VAR_TYPE_STRING ? 1 : 0) ^ (value[1].type == VAR_TYPE_STRING ? 1 : 0)))
            return BasicError = BASIC_ERR_TYPE_MISMATCH;
        if ((value[0].type & VAR_TYPE_FLOAT) || (value[1].type & VAR_TYPE_FLOAT)) // treat as float if one of the operands is float.
        {
            doFloat = true;
            if (value[0].type & VAR_TYPE_INTEGER)
            {
                value[0] = RPN_FLOAT(value[0].var.i);
//                value[0].var.f = (float)value[0].var.i;
//                value[0].type = VAR_TYPE_FLOAT;
            }
            if (value[1].type & VAR_TYPE_INTEGER)
            {
                value[1] = RPN_FLOAT(value[1].var.i);
//                value[1].var.f = (float)value[1].var.i;
//                value[1].type = VAR_TYPE_FLOAT;
            }
        }
    }
    switch(op)
    {
    // Math equations
    case OPERATOR_ADD:
        if (value[0].type == VAR_TYPE_STRING)
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
        if (value[0].type == VAR_TYPE_STRING) return BasicError = BASIC_ERR_TYPE_MISMATCH;
        value[0].var.f = value[1].var.f - value[0].var.f;
        break;
    case OPERATOR_MUL:
        if (value[0].type == VAR_TYPE_STRING) return BasicError = BASIC_ERR_TYPE_MISMATCH;
        if (doFloat)
            value[0].var.f = value[1].var.f * value[0].var.f;
        else
            value[0].var.i = value[1].var.i * value[0].var.i;
        break;
    case OPERATOR_DIV:
        if (value[0].type == VAR_TYPE_STRING) return BasicError = BASIC_ERR_TYPE_MISMATCH;
        if (!value[0].var.f) return BasicError = BASIC_ERR_DIV_ZERO;
        if (doFloat)
            value[0].var.f = value[1].var.f / value[0].var.f;
        else
            value[0].var.i = value[1].var.i / value[0].var.i;
        break;
    case OPERATOR_MOD:
        if (value[0].type == VAR_TYPE_STRING) return BasicError = BASIC_ERR_TYPE_MISMATCH;
        if (!value[0].var.f) return BasicError = BASIC_ERR_DIV_ZERO;
        if (doFloat)
            value[0].var.f = value[1].var.f / value[0].var.f;
        else
            value[0].var.i = value[1].var.i % value[0].var.i;
        break;
    case OPERATOR_PWR:
        if (value[0].type == VAR_TYPE_STRING) return BasicError = BASIC_ERR_TYPE_MISMATCH;
        if (doFloat)
            value[0].var.f = powf(value[1].var.f,value[0].var.f);
        else
            value[0].var.i = pow(value[1].var.i,value[0].var.i);

        break;

    default:
        switch(op)
        {
        // conditional equations
        case OPERATOR_MORE:
            if (doFloat)
                value[0].var.i = ((value[1].var.f > value[0].var.f) ? 1 : 0);
            else value[0].var.i = ((value[1].var.i > value[0].var.i) ? 1 : 0);
            break;
        case OPERATOR_LESS:
            if (doFloat)
                value[0].var.i = ((value[1].var.f < value[0].var.f) ? 1 : 0);
            else value[0].var.i = ((value[1].var.i < value[0].var.i) ? 1 : 0);
            break;
        case OPERATOR_EQUAL:
            if (value[0].type == VAR_TYPE_STRING)
            {
                value[0].var.i = strcmp(value[1].var.str,value[0].var.str) == 0 ? 1 : 0;
                value[0].type = VAR_TYPE_BOOL;
            }
            else
            {
                if (doFloat)
                    value[0].var.i = ((value[1].var.f == value[0].var.f) ? 1 : 0);
                else value[0].var.i = ((value[1].var.i == value[0].var.i) ? 1 : 0);
            }
            break;
        case OPERATOR_MORE_EQ:
            if (doFloat)
                value[0].var.i = ((value[1].var.f >= value[0].var.f) ? 1 : 0);
            else value[0].var.i = ((value[1].var.i >= value[0].var.i) ? 1 : 0);
            break;
        case OPERATOR_NOT_EQ:
            if (value[0].type == VAR_TYPE_STRING)
            {
                value[0].var.i = strcmp(value[1].var.str,value[0].var.str) == 0 ? 0 : 1;
                value[0].type = VAR_TYPE_BOOL;
            }
            else
            {
                if (doFloat)
                    value[0].var.i = ((value[1].var.f != value[0].var.f) ? 1 : 0);
                else value[0].var.i = ((value[1].var.i != value[0].var.i) ? 1 : 0);
            }
            break;
        case OPERATOR_LESS_EQ:
            if (doFloat)
                value[0].var.i = ((value[1].var.f <= value[0].var.f) ? 1 : 0);
            else value[0].var.i = ((value[1].var.i <= value[0].var.i) ? 1 : 0);
            break;
        case OPERATOR_NOT:
            value[0].var.i = (value[0].var.f ? 0 : 1);
            break;
        case OPERATOR_AND:
            value[0].var.i = ((value[1].var.f && value[0].var.f) ? 1 : 0);
            break;
        case OPERATOR_OR:
            if (doFloat)
                value[0].var.i = ((value[1].var.f || value[0].var.f) ? 1 : 0);
            break;
        default:
            return BasicError = BASIC_ERR_UNKNOWN_OP;
        }
        if (value[0].type == VAR_TYPE_STRING) return BasicError = BASIC_ERR_TYPE_MISMATCH;
        value[0].type = VAR_TYPE_BOOL;
    }
    rpn_push_queue(value[0]);
    return BasicError = BASIC_ERR_NONE;
}
