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
#ifndef _BFUNC_H_INCLUDED
#define _BFUNC_H_INCLUDED

#include "rpn.h"

#define OPCODE_MASK   0x80

#define FUNC_TYPE_SECONDARY __OPCODE_AT
#define FUNC_TYPE_PRIMARY   OPCODE_MASK

enum
{
    __OPCODE_REM = FUNC_TYPE_PRIMARY,
    __OPCODE_IF,
    __OPCODE_THEN,
    __OPCODE_GOTO,
    __OPCODE_GOSUB,
    __OPCODE_RETURN,
    __OPCODE_FOR,
    __OPCODE_TO,
    __OPCODE_STEP,
    __OPCODE_NEXT,
    __OPCODE_STOP,
    __OPCODE_PRINT,
    __OPCODE_INPUT,
    __OPCODE_PAUSE,
    __OPCODE_INKEY$,
    __OPCODE_CLS,
    __OPCODE_LET,
    __OPCODE_DIM,
    __OPCODE_DATA,
    __OPCODE_READ,
    __OPCODE_RUN,
    __OPCODE_LIST,
    __OPCODE_NEW,
    __OPCODE_AT,
    __OPCODE_INK,
    __OPCODE_PAPER,
    __OPCODE_VAL$,
    __OPCODE_HEX$,
    __OPCODE_INT,
    __OPCODE_ABS,
    __OPCODE_SIN,
    __OPCODE_COS,
    __OPCODE_TAN,
    __OPCODE_ATN,
    __OPCODE_SQR,
    __OPCODE_RND,
    __OPCODE_LOG,
    __OPCODE_DEG,
    __OPCODE_RAD,
    __OPCODE_MIN,
    __OPCODE_MAX,
    __OPCODE_LAST,
};

typedef enum
{
    BASIC_STAT_OK,
    BASIC_STAT_ERR,
    BASIC_STAT_SKIP,
    BASIC_STAT_JUMP,
    BASIC_STAT_STOP,
} _bas_stat_e;

typedef struct
{
    const char* name;
    _bas_stat_e (*func)(_rpn_type_t);
} _bas_func_t;

extern const _bas_func_t BasicFunction[];
extern const uint8_t LastOpCode;

#define bas_func_offset(name) (bas_func_opcode(name) & (~OPCODE_MASK))

uint8_t bas_func_opcode(char *name);
const char *bas_func_name(uint8_t opCode);


#endif // _BFUNC_H_INCLUDED
