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

#include "berror.h"

_bas_err_e BasicError;

const char *BErrorText[BASIC_ERR_COUNT] =
{
    "",
    "No line number",
    "Duplicate line number",
    "Invalid line number",
    "String too long",
    "Out of memory",
    "Type mismatch",
    "Wrong usage of string var",
    "Parentheses micmatch",
    "Missing operand",
    "Missing operator",
    "Unknown variable",
    "Unknown operator",
    "Unknown function",
    "Using reserved name",
    "Missing \"then\"",
    "RPN queue is empty",
    "RPN queue is full",
    "RPN stack is full",
    "Divide by zero",
    "Incomplete FOR loop",
    "File not found",
    "GOSUB stack overflow",
    "RETURN without GOSUB",
};
