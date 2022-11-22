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
#include "bcore.h"
#include "bfunc.h"
#include "bstring.h"

struct
{
    _bas_ptr_t line[BASIC_GOSUB_STACK_SIZE];
    uint8_t ptr;
} GosubStack = {.ptr = 0};

_bas_stat_e __rem(_rpn_type_t param)
{
    return BasicStat = BASIC_STAT_SKIP;
}

_bas_stat_e __stop(_rpn_type_t param)
{
    return BasicStat = BASIC_STAT_STOP;
}

_bas_stat_e __goto(_rpn_type_t param)
{
    _rpn_type_t *tmpVar;
    _bas_line_t *bL = BasicProg;
    uint16_t lineNum;
    if (token_eval_expression(param.var.i))
        return BasicStat = BASIC_STAT_ERR;
    tmpVar = rpn_pop_queue();
    if (tmpVar->type < VAR_TYPE_FLOAT)
    {
        BasicError = BASIC_ERR_INVALID_LINE;
        return BasicStat = BASIC_STAT_ERR;
    }
    lineNum = (tmpVar->type == VAR_TYPE_FLOAT) ? (uint16_t) tmpVar->var.f :  (uint16_t) tmpVar->var.i;
    while (bL && (bL->number != lineNum))  bL = bL->next;
    if (!bL)
    {
        BasicError = BASIC_ERR_INVALID_LINE;
        return BasicStat = BASIC_STAT_ERR;
    }
    ExecLine.number = lineNum;
    ExecLine.statement = 0;
    return BasicStat = BASIC_STAT_JUMP;
}

_bas_stat_e __if(_rpn_type_t param)
{
    _rpn_type_t *tmpVar;

    if (token_eval_expression(param.var.i))
        return BasicStat = BASIC_STAT_ERR;
    tmpVar = rpn_pop_queue();
    if (tmpVar->type < VAR_TYPE_FLOAT)
    {
        BasicError = BASIC_ERR_TYPE_MISMATCH;
        return BasicStat = BASIC_STAT_ERR;
    }
    if ((uint8_t)*bToken.t[bToken.ptr].str != __OPCODE_THEN)
    {
        BasicError = BASIC_ERR_NO_THEN;
        return BasicStat = BASIC_STAT_ERR;
    }
    return BasicStat = tmpVar->var.i ? BASIC_STAT_OK : BASIC_STAT_SKIP;
}

_bas_stat_e __print(_rpn_type_t param)
{
    _rpn_type_t *var;
    bool head = true;
    if (!param.var.i)
    {
        printf("\n");
        return BasicStat = BASIC_STAT_OK;
    }
    while (1)
    {
        if(token_eval_expression(param.var.i))
        {
            return BasicStat = BASIC_STAT_ERR;
        }
        while (1)
        {
            var = rpn_peek_queue(head);
            if (var->type == VAR_TYPE_NONE)// nothing to print
                break;
            if (!head) printf("\n"); // print new line when separated by commas
            head = false;
            switch(var->type)
            {
            case VAR_TYPE_NONE:
                break;
            case VAR_TYPE_FLOAT:
            case VAR_TYPE_LOOP:
                printf("%.02f",var->var.f);
                break;
            case VAR_TYPE_INTEGER:
                printf("%d",var->var.i);
                break;
            case VAR_TYPE_BOOL:
                printf("%s",var->var.i ? "true":"false");
                break;
            case VAR_TYPE_STRING:
                printf("%s",var->var.str);
                break;
            default:
                break;
            }
        }
        if(bToken.t[bToken.ptr].op == '\0' || bToken.t[bToken.ptr].op == ':') break;
        bToken.ptr++;
        param.var.i = '0';
        head = true;
    }
    if (!((bToken.t[bToken.ptr-1].op == ';') && (*bToken.t[bToken.ptr].str == '\0'))) // string termination
        printf("\n");
    return BasicStat = BASIC_STAT_OK;
};

_bas_stat_e __input(_rpn_type_t param)
{
    _bas_var_t *var;
    do
    {
        if (*bToken.t[bToken.ptr].str == '\"')
        {
            bToken.t[bToken.ptr].str++;
            bToken.t[bToken.ptr].str[strlen(bToken.t[bToken.ptr].str)-1] = '\0';
            printf("%s",bToken.t[bToken.ptr++].str);
        }
        if (*bToken.t[bToken.ptr].str == '\0')
        {
            BasicError = BASIC_ERR_MISSING_OPERAND;
            return BasicStat = BASIC_STAT_ERR;
        }
        if ((var = var_get(bToken.t[bToken.ptr].str)) == NULL)
            var = var_add(bToken.t[bToken.ptr].str);
        if (var == NULL) return BasicStat = BASIC_STAT_ERR;
        scanf("%127s",strTmpBuff);

        switch (var->value.type)
        {
        case VAR_TYPE_FLOAT:
            var->value.var.f = atof(strTmpBuff);
            break;
        case VAR_TYPE_BOOL:
            var->value.var.i = (strchr("0nNfF",*strTmpBuff) || !strlen(strTmpBuff)) ? 0 : 1;
            break;
        case VAR_TYPE_INTEGER:
            var->value.var.i = strtol(strTmpBuff,NULL,0);
            break;
        case VAR_TYPE_STRING:
            var_set_string(var,strTmpBuff);
            /* if(var->param.size[0] < (strlen(strTmpBuff)+1))
             {
                 if (var->param.size[0]) free(var->value.var.str);
                 var->param.size[0] = strlen(strTmpBuff)+1;
                 if ((var->value.var.str = (char *)malloc(var->param.size[0])) == NULL)
                 {
                     BasicError = BASIC_ERR_MEM_OUT;
                     break;
                 }
             }
             strncpy(var->value.var.str,strTmpBuff,BASIC_STRING_LEN);*/
            break;
        default:
            BasicError = BASIC_ERR_TYPE_MISMATCH; // you cannot input loop
            break;
        }
    }
    while ((bToken.t[bToken.ptr++].op == ',') && !BasicError);
    return BasicStat = BasicError ? BASIC_STAT_ERR : BASIC_STAT_OK;
}

_bas_stat_e __let(_rpn_type_t param)
{
    _bas_var_t *var;
    _rpn_type_t tmpVar;
    uint8_t opParam;
    char *varName = bToken.t[bToken.ptr].str;
    BasicError = BASIC_ERR_NONE;
    if (bToken.t[bToken.ptr].op != '=')
    {
        BasicError = BASIC_ERR_MISSING_OPERATOR;
        return BasicStat = BASIC_STAT_ERR;
    }
    if (bas_func_opcode(varName))
    {
        BasicError = BASIC_ERR_RESERVED_NAME;
        return BasicStat = BASIC_STAT_ERR;
    }
    if ((var = var_get(bToken.t[bToken.ptr].str)) == NULL)
        var = var_add(bToken.t[bToken.ptr].str);
    opParam = bToken.t[bToken.ptr].op;
    bToken.ptr++;
    if(token_eval_expression(opParam))
    {
        return BasicStat = BASIC_STAT_ERR;
    }
    else
        tmpVar = *rpn_pop_queue();
    switch (var->value.type)
    {
    case VAR_TYPE_FLOAT:
    case VAR_TYPE_LOOP:
        if(tmpVar.type < VAR_TYPE_FLOAT)
            BasicError = BASIC_ERR_TYPE_MISMATCH;
        else
            var->value.var.f = (tmpVar.type == VAR_TYPE_FLOAT) ? tmpVar.var.f : (float)tmpVar.var.i;
        break;
    case VAR_TYPE_INTEGER:
    case VAR_TYPE_BOOL:
        if(tmpVar.type < VAR_TYPE_FLOAT)
            BasicError = BASIC_ERR_TYPE_MISMATCH;
        else
            var->value.var.i = (tmpVar.type == VAR_TYPE_FLOAT) ? (int32_t)tmpVar.var.f : tmpVar.var.i;
        break;
    case VAR_TYPE_STRING:
        if(tmpVar.type != VAR_TYPE_STRING)
        {
            BasicError = BASIC_ERR_TYPE_MISMATCH;
            break;
        }
        var_set_string(var,tmpVar.var.str);
        /*
        if(var->param.size[0] < (strlen(tmpVar.var.str)+1))
        {
            if (var->param.size) free(var->value.var.str);
            var->param.size = strlen(tmpVar.var.str)+1;
            if ((var->value.var.str = (char *)malloc(var->param.size)) == NULL)
            {
                BasicError = BASIC_ERR_MEM_OUT;
                break;
            }
            //printf("\n *** Reallocate %d bytes for %s\n", var->size, var->value.var.str);
        }
        strncpy(var->value.var.str,tmpVar.var.str,BASIC_STRING_LEN); */
        break;
    default:
        break;
    }
    return BasicStat = (BasicError == BASIC_ERR_NONE) ? BASIC_STAT_OK : BASIC_STAT_ERR;
};


static _bas_stat_e var_get_loop(float *var)
{
    _rpn_type_t *tmpVar;
    uint8_t opParam = bToken.t[bToken.ptr].op;
    bToken.ptr++;
    if(token_eval_expression(opParam))
    {
        return BasicStat = BASIC_STAT_ERR;
    }
    else
        tmpVar = rpn_pop_queue();
    switch (tmpVar->type)
    {
    case VAR_TYPE_INTEGER:
    case VAR_TYPE_BOOL:
        tmpVar->var.f = (float)tmpVar->var.i;
    case VAR_TYPE_FLOAT:
    case VAR_TYPE_LOOP:
        *var = tmpVar->var.f;
        break;
    default:
        BasicError = BASIC_ERR_TYPE_MISMATCH;
    }
    return BasicStat = (BasicError == BASIC_ERR_NONE) ? BASIC_STAT_OK : BASIC_STAT_ERR;
}

_bas_stat_e __for(_rpn_type_t param)
{
    _bas_var_t *var;
    char *varName = bToken.t[bToken.ptr].str;
    BasicError = BASIC_ERR_NONE;
    if (bToken.t[bToken.ptr].op != '=')
    {
        BasicError = BASIC_ERR_MISSING_OPERATOR;
        return BasicStat = BASIC_STAT_ERR;
    }
    if (bas_func_opcode(varName))
    {
        BasicError = BASIC_ERR_RESERVED_NAME;
        return BasicStat = BASIC_STAT_ERR;
    }
    if ((var = var_get(bToken.t[bToken.ptr].str)) == NULL)
        var = var_add(bToken.t[bToken.ptr].str);
    if (var->value.type != VAR_TYPE_LOOP)
    {
        var->value.type = VAR_TYPE_LOOP;
        var->param.loop = malloc(sizeof(_bas_loop_t));

        if (var->param.loop == NULL)
        {
            BasicError = BASIC_ERR_MEM_OUT;
            return BasicStat = BASIC_STAT_ERR;
        }
    }

    if (var_get_loop(&var->value.var.f)) return BasicStat = BASIC_STAT_ERR;
    var->value.type = VAR_TYPE_LOOP;
    if (!bToken.t[bToken.ptr].op || ((uint8_t)*bToken.t[bToken.ptr].str != __OPCODE_TO))
    {
        BasicError = BASIC_ERR_INCOMPLETE_FOR;
        return BasicStat = BASIC_STAT_ERR;
    }
    if (var_get_loop(&var->param.loop->limit)) return BasicStat = BASIC_STAT_ERR;
    var->param.loop->step = 1;
    if ((uint8_t)*bToken.t[bToken.ptr].str == __OPCODE_STEP)
        if (var_get_loop(&var->param.loop->step)) return BasicStat = BASIC_STAT_ERR;

    var->param.loop->line.number = bToken.t[bToken.ptr].op == ':' ? ExecLine.number : NextLine.number;
    var->param.loop->line.statement = bToken.t[bToken.ptr].op == ':' ? ExecLine.statement + 1 : 0;
    //printf("\nFOR param - var: %.0f, limit: %.0f, step: %.0f, line: %d, statement: %d\n",var->value.var.f,var->param.loop->limit,var->param.loop->step,var->param.loop->line.number,var->param.loop->line.statement);
    return BasicStat = BASIC_STAT_OK;
};

_bas_stat_e __to(_rpn_type_t param)
{
    BasicError = BASIC_ERR_INCOMPLETE_FOR;
    return BasicStat = BASIC_STAT_ERR;
}

_bas_stat_e __step(_rpn_type_t param)
{
    BasicError = BASIC_ERR_INCOMPLETE_FOR;
    return BasicStat = BASIC_STAT_ERR;
}

_bas_stat_e __next(_rpn_type_t param)
{
    _bas_var_t *var;
    BasicError = BASIC_ERR_NONE;
    if (((var = var_get(bToken.t[bToken.ptr].str)) == NULL) || (var->value.type != VAR_TYPE_LOOP))
    {
        BasicError = BASIC_ERR_INCOMPLETE_FOR;
        return BasicStat = BASIC_STAT_ERR;
    }

    var->value.var.f += var->param.loop->step;
    if (((var->param.loop->step > 0) && (var->value.var.f <= var->param.loop->limit)) || ((var->param.loop->step < 0) && (var->value.var.f >= var->param.loop->limit)))
    {
        ExecLine = var->param.loop->line;
        return BasicStat = BASIC_STAT_JUMP;
    }
    return BasicStat = BASIC_STAT_OK;
}

_bas_stat_e __gosub(_rpn_type_t param)
{
    _rpn_type_t *tmpVar;
    _bas_line_t *bL = BasicProg;
    uint16_t lineNum;
    if (GosubStack.ptr >= BASIC_GOSUB_STACK_SIZE)
    {
        BasicError = BASIC_ERR_GOSUB_OVERFLOW;
        return BasicStat = BASIC_STAT_ERR;
    }
    if (token_eval_expression(param.var.i))
        return BasicStat = BASIC_STAT_ERR;
    tmpVar = rpn_pop_queue();
    if (tmpVar->type < VAR_TYPE_FLOAT)
    {
        BasicError = BASIC_ERR_INVALID_LINE;
        return BasicStat = BASIC_STAT_ERR;
    }

    GosubStack.line[GosubStack.ptr].number = bToken.t[bToken.ptr].op == ':' ? ExecLine.number : NextLine.number;
    GosubStack.line[GosubStack.ptr++].statement = bToken.t[bToken.ptr].op == ':' ? ExecLine.statement + 1 : 0;
    // printf("GOSUB return: %d:%d\n",GosubStack.line[GosubStack.ptr-1].number,GosubStack.line[GosubStack.ptr-1].statement);
    lineNum = (tmpVar->type == VAR_TYPE_FLOAT) ? (uint16_t) tmpVar->var.f :  (uint16_t) tmpVar->var.i;
    while (bL && (bL->number != lineNum))  bL = bL->next;
    if (!bL)
    {
        BasicError = BASIC_ERR_INVALID_LINE;
        return BasicStat = BASIC_STAT_ERR;
    }
    ExecLine.number = lineNum;
    ExecLine.statement = 0;
    return BasicStat = BASIC_STAT_JUMP;
};

_bas_stat_e __return(_rpn_type_t param)
{
    if (!GosubStack.ptr)
    {
        BasicError = BASIC_ERR_RETURN_NO_GOSUB;
        return BasicStat = BASIC_STAT_ERR;
    }
    ExecLine = GosubStack.line[--GosubStack.ptr];
    return BasicStat = BASIC_STAT_JUMP;
};

_bas_stat_e __dim(_rpn_type_t param)
{
    _bas_var_t *var;
    char *varName = bToken.t[bToken.ptr].str;
    BasicError = BASIC_ERR_NONE;
    if (bToken.t[bToken.ptr].op != '[') BasicError = BASIC_ERR_PAR_MISMATCH;
    if (!BasicError && bas_func_opcode(varName)) BasicError = BASIC_ERR_RESERVED_NAME;
    if (!BasicError && (var = var_get(bToken.t[bToken.ptr].str)) != NULL) BasicError = BASIC_ERR_ARRAY_REDEFINE;
    if (!BasicError)
    {
        var = var_add(bToken.t[bToken.ptr++].str);
        var->param.size[0] = (uint16_t) strtol(bToken.t[bToken.ptr].str,NULL,0);
        var->param.size[1] = (bToken.t[bToken.ptr].op == ',') ? (uint16_t) strtol(bToken.t[++bToken.ptr].str,NULL,0) : 0;
        if ((var->param.size[0] == 0 && var->param.size[1] == 0) || (bToken.t[bToken.ptr].op != ']'))
            BasicError = BASIC_ERR_ARRAY_DIMENTION;
        else
        {
            size_t arraySize = var->param.size[0] * (var->param.size[1] ? var->param.size[1] : 1) * ((var->value.type == VAR_TYPE_BYTE) ? 1 : sizeof(var->value));
            if ((var->value.var.array = (char *)malloc(arraySize)) == NULL)
                BasicError = BASIC_ERR_MEM_OUT;
        }
    }
    if (BasicError) return BasicStat = BASIC_STAT_ERR;

    switch (var->value.type)
    {
    case VAR_TYPE_FLOAT:
    case VAR_TYPE_LOOP:
        var->value.type = VAR_TYPE_ARRAY_FLOAT;
        break;
    case VAR_TYPE_INTEGER:
    case VAR_TYPE_BOOL:
        var->value.type = VAR_TYPE_ARRAY_INTEGER;
        break;
    case VAR_TYPE_BYTE:
        var->value.type = VAR_TYPE_ARRAY_BYTE;
        break;
    case VAR_TYPE_STRING:
        var->value.type = VAR_TYPE_ARRAY_STRING;
        break;
    default:
        break;
    }

    //printf("Array allocated: dim0 = %d, dim1 = %d, size = %lu\n",var->param.size[0],var->param.size[1],(var->param.size[0] * (var->param.size[1] ? var->param.size[1] : 1) * ((var->value.type == VAR_TYPE_ARRAY_BYTE) ? 1 : sizeof(var->value))));
    bToken.ptr++;
    if (bToken.t[bToken.ptr].op == '=')
    {
        if (array_set(varName,true)) return BASIC_STAT_ERR;
        /**
        printf("Array data: ");
        for (uint16_t i=0; i < (var->param.size[0] + var->param.size[0] * var->param.size[1]); i++)
        {
            printf("0x%02x ",*(uint8_t *)(var->value.var.array + i*(var->value.type = VAR_TYPE_BYTE ? 1 : 4)));
        }
        printf("\n");
        */
    }
    return BasicError ? BASIC_STAT_ERR : BASIC_STAT_OK;
}
