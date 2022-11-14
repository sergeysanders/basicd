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

#ifndef _BERROR_H_INCLUDED
#define _BERROR_H_INCLUDED

typedef enum
{
    BASIC_ERR_NONE,
    BASIC_ERR_LOAD_NONUMBER,
    BASIC_ERR_LOAD_DUPLICATE,
    BASIC_ERR_INVALID_LINE,
    BASIC_ERR_STRING_LENGTH,
    BASIC_ERR_MEM_OUT,
    BASIC_ERR_TYPE_MISMATCH,
    BASIC_ERR_STRING_MISMATCH,
    BASIC_ERR_PAR_MISMATCH,
    BASIC_ERR_MISSING_OPERAND,
    BASIC_ERR_MISSING_OPERATOR,
    BASIC_ERR_UNKNOWN_VAR,
    BASIC_ERR_UNKNOWN_OP,
    BASIC_ERR_UNKNOWN_FUNC,
    BASIC_ERR_RESERVED_NAME,
    BASIC_ERR_NO_THEN,
    BASIC_ERR_QUEUE_EMPTY,
    BASIC_ERR_QUEUE_FULL,
    BASIC_ERR_STACK_FULL,
    BASIC_ERR_DIV_ZERO,
    BASIC_ERR_INCOMPLETE_FOR,
    BASIC_ERR_FILE_NOT_FOUND,
    BASIC_ERR_GOSUB_OVERFLOW,
    BASIC_ERR_RETURN_NO_GOSUB,
    BASIC_ERR_COUNT
}_bas_err_e;

extern _bas_err_e BasicError;
extern const char *BErrorText[BASIC_ERR_COUNT];

#endif // _BERROR_H_INCLUDED