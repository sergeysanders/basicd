#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include "rpn.h"

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef bool
typedef bool uint8_t;
#endif

uint8_t RPNError = RPN_ERR_NONE;
const char *RpnErrorText[RPN_ERR_COUNT] =
{
    "",
    "Unknown operator",
    "Unknown function",
    "Divide by zero",
    "Missing operand",
    "RPN queue is full",
    "RPN stack is full",
    "Mismatched parentheses",
    "Invalid variable",
};

#define RPN_FUNCTION_MASK   0x80

enum _rpn_builtin_e
{
    RPN_OP_ABS = RPN_FUNCTION_MASK,
    RPN_OP_SIN,
    RPN_OP_COS,
    RPN_OP_TAN,
    RPN_OP_ATN,
    RPN_OP_SQR,
    RPN_OP_RND,
    RPN_OP_LOG,
    RPN_OP_DEG,
    RPN_OP_RAD,
    RPN_OP_MIN,
    RPN_OP_MAX,
    RPN_OP_BUILTIN
} ;

typedef struct
{
    const char* name;
    enum _rpn_builtin_e opCode;
} _rpn_funct_t;


const _rpn_funct_t RPN_BuiltIn[] =
{
    {"abs",RPN_OP_ABS},
    {"sin",RPN_OP_SIN},
    {"cos",RPN_OP_COS},
    {"tan",RPN_OP_TAN},
    {"atn",RPN_OP_ATN},
    {"sqr",RPN_OP_SQR},
    {"rnd",RPN_OP_RND},
    {"log",RPN_OP_LOG},
    {"deg",RPN_OP_DEG},
    {"rad",RPN_OP_RAD},
    {"min",RPN_OP_MIN},
    {"max",RPN_OP_MAX},
    {0}
};

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

_rpn_err_t rpn_push_queue(_rpn_type_t value)
{
    if (RPNQueue.ptr < RPN_QUEUE_LEN-1)
    {
        RPNQueue.value[RPNQueue.ptr++] = value;
        return RPNError = RPN_ERR_NONE;
    }
    return RPNError = RPN_ERR_QUEUE_FULL;
}

_rpn_type_t rpn_pop_queue(void)
{
    if (!RPNQueue.ptr)
    {
        RPNError = RPN_ERR_QUEUE_EMPTY;
        return (_rpn_type_t){.i = 0};
    }
    RPNError = RPN_ERR_NONE;
    return RPNQueue.value[--RPNQueue.ptr];
}

_rpn_err_t rpn_push_stack(uint8_t op)
{
    if (RPNStack.ptr < RPN_STACK_LEN-1)
    {
        RPNStack.op[RPNStack.ptr++] = op;
        return RPNError = RPN_ERR_NONE;
    }
    return RPNError = RPN_ERR_STACK_FULL;
}

uint8_t rpn_pop_stack(void)
{
    RPNError = RPN_ERR_NONE;
    return RPNStack.ptr ? RPNStack.op[--RPNStack.ptr] : 0;
}

uint8_t rpn_peek_stack_last(void)
{
    return RPNStack.ptr ? RPNStack.op[RPNStack.ptr-1] : 0;
}

uint8_t rpn_find_op(char *name)
{
    uint8_t fCnt = 0;
    while (RPN_BuiltIn[fCnt].name)
    {
        if (!strcmp(name,RPN_BuiltIn[fCnt].name)) return RPN_BuiltIn[fCnt].opCode;
        fCnt++;
    }
    return 0;
}

const char *rpn_find_func_name(uint8_t opCode)
{
    uint8_t fCnt = 0;
    while (RPN_BuiltIn[fCnt].name)
    {
        if (RPN_BuiltIn[fCnt].opCode == opCode) return RPN_BuiltIn[fCnt].name;
        fCnt++;
    }
    return "?";
}

_rpn_err_t rpn_eval(uint8_t op)
{
    _rpn_type_t value[2];
    if (!op || (op == ' ')) return RPN_ERR_NONE;
    value[0] = rpn_pop_queue();
    if (op < RPN_FUNCTION_MASK)
    {
        value[1] = rpn_pop_queue();
    }
    if (RPNError) return RPNError;
    switch(op)
    {
    // conditional equations
    case '>':
        rpn_push_queue(RPN_FLOAT(value[1].f > value[0].f ? 1 : 0));
        break;
    case '<':
        rpn_push_queue(RPN_FLOAT(value[1].f < value[0].f ? 1 : 0));
        break;
    case '=':
        rpn_push_queue(RPN_FLOAT(value[1].f == value[0].f ? 1 : 0));
        break;
    case '+':
        rpn_push_queue(RPN_FLOAT(value[1].f + value[0].f));
        break;
    case '-':
        rpn_push_queue(RPN_FLOAT(value[1].f - value[0].f));
        break;
    case '*':
        rpn_push_queue(RPN_FLOAT(value[1].f * value[0].f));
        break;
    case '/':
        if (!value[0].f) return RPN_ERR_DIV_ZERO;
        rpn_push_queue(RPN_FLOAT(value[1].f / value[0].f));
        break;
    case '%':
        if (!value[0].f) return RPN_ERR_DIV_ZERO;
        rpn_push_queue(RPN_FLOAT((float)((int)value[1].f % (int)value[0].f)));
        break;
    case '^':
        rpn_push_queue(RPN_FLOAT(powf(value[1].f,value[0].f)));
        break;
    case RPN_OP_ABS:
        rpn_push_queue(RPN_FLOAT(value[0].f < 0 ? -value[0].f : value[0].f));
        break;
    case RPN_OP_SIN:
        rpn_push_queue(RPN_FLOAT(sinf(value[0].f)));
        break;
    case RPN_OP_COS:
        rpn_push_queue(RPN_FLOAT(cosf(value[0].f)));
        break;
    case RPN_OP_TAN:
        rpn_push_queue(RPN_FLOAT(tanf(value[0].f)));
        break;
    case RPN_OP_ATN:
        rpn_push_queue(RPN_FLOAT(atanf(value[0].f)));
        break;
    case RPN_OP_SQR:
        rpn_push_queue(RPN_FLOAT(sqrtf(value[0].f)));
        break;
    case RPN_OP_RND:
        rpn_push_queue(RPN_FLOAT(333.33));
        break;     
    case RPN_OP_LOG:
        rpn_push_queue(RPN_FLOAT(logf(value[0].f)));
        break;
    case RPN_OP_DEG:
        rpn_push_queue(RPN_FLOAT(DEGREES(value[0].f)));
        break;
    case RPN_OP_RAD:
        rpn_push_queue(RPN_FLOAT(RADIANS(value[0].f)));
        break;    
    case RPN_OP_MIN:
        value[1] = rpn_pop_queue();
        rpn_push_queue(RPN_FLOAT(value[1].f > value[0].f ? value[0].f : value[1].f));
        break;
    case RPN_OP_MAX:
        value[1] = rpn_pop_queue();
        rpn_push_queue(RPN_FLOAT(value[1].f < value[0].f ? value[0].f : value[1].f));
        break;
    default:
        return RPN_ERR_UNKOWN_OPERATOR;
    }
    return RPN_ERR_NONE;
}

void rpn_print_queue(void)
{
    //printf("[%d]> ",RPNQueue.ptr);
    for (uint8_t i=0; i<RPNQueue.ptr; i++)
        printf("%0.2f ",RPNQueue.value[i].f);
    //printf("\n");
}

void rpn_print_stack(void)
{
    for (uint8_t i=0; i<RPNStack.ptr; i++)
        if ((RPNStack.op[i] > ' ') && (RPNStack.op[i] < '~')) printf("%c ",RPNStack.op[i]);
        else printf("%s ",rpn_find_func_name(RPNStack.op[i]));
    //printf("\n");
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