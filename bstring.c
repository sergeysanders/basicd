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
#include "banalizer.h"
#include "bstring.h"

char strTmpBuff[BASIC_STRING_LEN];
static char strVarBuff[BASIC_STRING_LEN];
static char strNumBuff[16];
_bas_err_e __val$(_rpn_type_t *param)
{
    sprintf(strNumBuff,"%.2f",param->var.f);
    rpn_push_queue(RPN_STR(strNumBuff));
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e __hex$(_rpn_type_t *param)
{
    sprintf(strNumBuff,(param->type == VAR_TYPE_BYTE) ? "%02x" : "%08x",param->var.i);
    rpn_push_queue(RPN_STR(strNumBuff));
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e string_add(char *str1, char *str2)
{
    if((strlen(str1) + strlen(str2)) > BASIC_STRING_LEN-2) return BasicError = BASIC_ERR_STRING_LENGTH;
    *strTmpBuff = '\0';
    strncpy(strTmpBuff,str1,BASIC_STRING_LEN);
    strncat(strTmpBuff,str2,BASIC_STRING_LEN);
    strncpy(strVarBuff,strTmpBuff,BASIC_STRING_LEN);
    rpn_push_queue(RPN_STR(strVarBuff));
    return BasicError = BASIC_ERR_NONE;
}

_bas_err_e var_set_string(_bas_var_t *var, char *str)
{
    if(var->param.size[0] < (strlen(str)+1))
    {
        if (var->param.size[0]) free(var->value.var.str);
        var->param.size[0] = strlen(str)+1;
        if ((var->value.var.str = (char *)malloc(var->param.size[0])) == NULL)
            return BasicError = BASIC_ERR_MEM_OUT;
        //printf("\n *** Reallocate %d bytes for %s\n", var->size, var->value.var.str);
    }
    strncpy(var->value.var.str,str,BASIC_STRING_LEN);
    return BasicError = BASIC_ERR_NONE;
}

uint8_t is_string(char *str)
{
    uint8_t lastCharPos = strlen(str);
    if (lastCharPos) lastCharPos--;
    if(str[lastCharPos] == '\"' || str[lastCharPos] == '$')  return str[lastCharPos];
    return 0;
}
