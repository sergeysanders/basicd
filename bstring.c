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
_bas_stat_e __val$(_rpn_type_t param)
{
    sprintf(strNumBuff,"%.2f",param.var.f);
    rpn_push_queue(RPN_STR(strNumBuff));
    return BasicStat = BASIC_STAT_OK;
};

_bas_stat_e __hex$(_rpn_type_t param)
{
    sprintf(strNumBuff,"0x%8x",param.var.i);
    rpn_push_queue(RPN_STR(strNumBuff));
    return BasicStat = BASIC_STAT_OK;
};

_bas_stat_e __int(_rpn_type_t param)
{
    int intVar;
    if (param.type < VAR_TYPE_FLOAT)
    {
        if (param.type == VAR_TYPE_STRING)
            intVar = (int)strtol(param.var.str,NULL,0);
        else
        {
            BasicError = BASIC_ERR_TYPE_MISMATCH;
            return BasicStat = BASIC_STAT_OK;
        }
    }
    else
    {
        intVar = (param.type & VAR_TYPE_INTEGER) ? param.var.i : (int)param.var.f;
    }
    
    rpn_push_queue(RPN_INT(intVar));
    return BasicStat = BASIC_STAT_OK;
};

_bas_stat_e string_add(char *str1, char *str2)
{
    BasicError = ((strlen(str1) + strlen(str2)) > BASIC_STRING_LEN-2) ? BASIC_ERR_STRING_LENGTH : BASIC_ERR_NONE;
    *strTmpBuff = '\0';
    strncpy(strTmpBuff,str1,BASIC_STRING_LEN);
    strncat(strTmpBuff,str2,BASIC_STRING_LEN);
    strncpy(strVarBuff,strTmpBuff,BASIC_STRING_LEN);
    rpn_push_queue(RPN_STR(strVarBuff));
    return BasicStat = BasicError ? BASIC_STAT_ERR : BASIC_STAT_OK;
}

uint8_t is_string(char *str)
{
    uint8_t lastCharPos = strlen(str);
    if (lastCharPos) lastCharPos--;
    if(str[lastCharPos] == '\"' || str[lastCharPos] == '$')  return str[lastCharPos];
    return 0;
}
