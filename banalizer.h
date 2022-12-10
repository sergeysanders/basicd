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
#ifndef _BANALIZER_H_INCLUDED
#define _BANALIZER_H_INCLUDED

#include "bcore.h"

typedef struct __attribute ((packed))
{
    char *str;
    char op;
} _bas_token_t;

typedef struct __attribute ((packed))
{
    _bas_token_t *t;
    uint8_t ptr;
    uint8_t parCnt;
} _bas_tok_list_t;

bool tok_list_push(_bas_tok_list_t *tokensList);
bool tok_list_pull(void);

extern _bas_tok_list_t *bToken;
_bas_var_t *var_get(char *name);
bool tokenizer(char *str);
_bas_err_e token_eval_expression(uint8_t opParam);

#endif //_BANALIZER_H_INCLUDED
