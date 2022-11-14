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

const struct
{
    const char *name;
    char op;
} BasicOperators[6] =
{
    {"and",OPERATOR_AND},
    {"or",OPERATOR_OR},
    {"not",OPERATOR_NOT},
    {">=",OPERATOR_MORE_EQ},
    {"<>",OPERATOR_NOT_EQ},
    {"<=",OPERATOR_LESS_EQ},
};
const char *OP_STR = " `=<>~!@%^&*()-+|\\,./;:\r\n";

_bas_stat_e BasicStat;
_bas_line_t *BasicProg = NULL;
_bas_ptr_t ExecLine;
_bas_ptr_t NextLine;

uint8_t tmpBasicLine[BASIC_LINE_LEN];

_bas_var_t BasicConstants[] =
{
    {.name = "pi",.next=&BasicConstants[1],.value.var.f = 3.14159,.value.type = VAR_TYPE_FLOAT },
    {.name = "e",.next=NULL,.value.var.f = 2.71828,.value.type = VAR_TYPE_FLOAT },
};

_bas_var_t *var_get(char *name)
{
    _bas_var_t *varPtr = BasicConstants;
    while(1)
    {
        if (!strcmp(name,varPtr->name)) return varPtr;
        if((varPtr = varPtr->next) == NULL) return NULL;
    }
}

_bas_var_t *var_add(char *name)
{
    _bas_var_t *varPtr = BasicConstants;
    uint8_t lastChar = name[strlen(name)-1]; // get var type: xxx - number(float),xxx$ - string, xxx# integer
    while (varPtr->next) varPtr = varPtr->next;
    uint8_t varSize = sizeof(_bas_var_t)+(strlen(name)+1);
    varPtr->next = malloc(varSize);
    if (varPtr->next == NULL)
    {
        BasicError = BASIC_ERR_MEM_OUT;
        return NULL;
    }
    varPtr = varPtr->next;
    varPtr->next = NULL; ///--- already null;
    varPtr->name = (char *)(varPtr + sizeof(_bas_var_t));
    strcpy(varPtr->name,name);
    varPtr->value.type = (lastChar == '$') ? VAR_TYPE_STRING : (lastChar == '#') ? VAR_TYPE_INTEGER : VAR_TYPE_FLOAT;
    varPtr->param.size = 0;
    return varPtr;
}

_bas_line_t *prog_find_line(uint16_t number)
{
    _bas_line_t *line = BasicProg;
    while (line)
        if (line->number == number) return line;
        else
            line = line->next;
    return NULL;
}


uint8_t *basic_line_totext(uint8_t *line) /// look for an opcode name and replace it with function name
{
    bool quoted = false;
    bool remarked = false;
    const char *opName = NULL;
    uint8_t destPtr = 0;
    while (*line)
    {
        if (destPtr > (BASIC_LINE_LEN-2)) break;

        if (*line == '\'' || remarked) // REM found
        {
            while (*line && (destPtr < (BASIC_LINE_LEN-1)))
                tmpBasicLine[destPtr++] = *line++;
            break;
        }

        if (*line == '\"')
        {
            if (quoted && !(*(char *)(line-1) == '\\'))
                quoted = false;
            else
                quoted = true;
        }

        if (quoted)
        {
            tmpBasicLine[destPtr++] = *line++;
            continue;
        }

        if (*line >= OPCODE_MASK)
            opName = bas_func_name(*line);
        for (uint8_t i=0; i<6 && !opName; i++)
            if (*line == BasicOperators[i].op) opName = BasicOperators[i].name;
        if (opName)
        {
            if (*line == __OPCODE_REM) remarked = true; // REM found
            while (*opName) tmpBasicLine[destPtr++] = *(opName++);
            opName = NULL;
        }
        else
            tmpBasicLine[destPtr++] = *line;
        line++;
    }
    tmpBasicLine[destPtr] = '\0';
    return tmpBasicLine;
}

uint8_t *basic_line_preprocess(uint8_t *line) /// look for a function name and replace it with op code
{
    bool quoted = false;
    bool remarked = false;
    uint8_t opCode = 0;
    uint8_t head = 0,tail = 0; // head points to the last character of the temp string and tail points to beginning of the last word
    while(*line == ' ') line++; // skip leading spaces
    while (*line)
    {
        /** process quoted text*/
        if (*line == '\"')
        {
            if (quoted && !(*(char *)(line-1) == '\\'))
            {
                quoted = false;
                tmpBasicLine[head++] = *(line++);
                tail = head;
                continue;
            }
            else
                quoted = true;
        }
        if (quoted)
        {
            tmpBasicLine[head++] = *(line++);
            continue;
        }
        /** skip remarked */
        if ((*line== '\'') || remarked)
        {
            while (*line && !(*line == '\r' || *line == '\n'))
                tmpBasicLine[head++] = *(line++);
            break;
        }
        /** check for delimeter */
        if ((!*line) || (strchr(OP_STR,*line))) // delimeter found
        {
            if (((*line == '=') || (*line == '>')) && ((*(line-1) == '<') || (*(line-1) == '>')))  // process <>, >= and <=
            {
                tail--;
                tmpBasicLine[head++] = *(line++);
            }
            tmpBasicLine[head] = '\0'; // terminate the temporary string
            for (uint8_t i=0; i<6 && !opCode; i++)
                if (!strcmp((char *)&tmpBasicLine[tail],BasicOperators[i].name))
                    opCode = BasicOperators[i].op;
            if (!opCode)
            {
                if ((tmpBasicLine[tail] == '?') && (tmpBasicLine[tail+1] == '\0'))
                    opCode = bas_func_opcode("print");
                else
                    opCode = bas_func_opcode((char *)&tmpBasicLine[tail]);

            }
            if (opCode) // reserved name found
            {
                tmpBasicLine[tail++] = opCode;
                head = tail;
                if (opCode == __OPCODE_REM)
                    remarked = true;
                opCode = 0;
            }
            tail = head+1;
            /** check for eol */
            if (*line == '\r' || *line == '\n') //
                break;
        }
        if (*line == ' ') // remove multiple spaces
            while (*(line+1) == ' ') line++;
        if (*line == '\"') quoted = true;
        tmpBasicLine[head++] = *(line++);
    }
    tmpBasicLine[head] = '\0';
    return tmpBasicLine;
}

bool prog_add_line(uint16_t number,uint8_t **line)
{
    uint16_t lineLen = 0;
    _bas_line_t *bLine = prog_find_line(number); // start new or update existing
    if (bLine == NULL) // create new
    {
        bLine = malloc(sizeof(_bas_line_t)); // add new line;
        if (bLine == NULL) return false;
        bLine->number = number;
        bLine->next = NULL;
        if (BasicProg == NULL)
            BasicProg = bLine; // Start a new program
        else
        {
            if (bLine->number < BasicProg->number) // swap if got smaller line number than the first one
            {
                bLine->next = BasicProg;
                BasicProg = bLine;
            }
            else
            {
                _bas_line_t *blSeek = BasicProg;
                while (blSeek->next != NULL)
                {
                    if (bLine->number > ((_bas_line_t *)(blSeek->next))->number)
                        blSeek = blSeek->next;
                    else // the new line has lower number
                    {
                        bLine->next = blSeek->next; // Insert the new line before higher line
                        blSeek->next = bLine;
                        break;
                    }
                }
                if (blSeek->next == NULL) // add to the end
                    blSeek->next = bLine;
            }
        }
    }
    bLine->number = number;
    while ((*line)[lineLen] > '\r')
        lineLen++;
    if (lineLen >= BASIC_LINE_LEN-1) lineLen = BASIC_LINE_LEN-2;
    strncpy((char *)bLine->string,(char *)basic_line_preprocess(*line),BASIC_LINE_LEN);
    *line += lineLen;
    return true;
}

_bas_stat_e __new(_rpn_type_t param)
{
    /// TODO free variables and GOSUB stack
    while (BasicProg != NULL)
    {
        _bas_line_t *bLnext = BasicProg->next;
        free(BasicProg);
        BasicProg = bLnext;
    }
    return BasicStat = BASIC_STAT_OK;
}

_bas_stat_e __list(_rpn_type_t param)
{
    _bas_line_t *bL = BasicProg;
    while (bL != NULL)
    {
        if (bL->number)
            printf("%d %s\n",bL->number,basic_line_totext(bL->string));
        //printf("%d %s\n",bL->number,bL->string);
        bL = bL->next;
    }
    return BasicStat = BASIC_STAT_OK;
}

_bas_stat_e __load(_rpn_type_t param)
{
    /** LOAD from an array
    while(*prog)
    {
        while (*prog && (*prog <= ' ')) prog++; //skip spaces and control characters
        if (!*prog) break; // eol/eof
        if ((ExecLine.number = (uint16_t)strtol((char *)prog,(char **)&prog,10)) == 0)
            return BasicError = BASIC_ERR_LOAD_NONUMBER;
        if (prog_find_line(ExecLine.number))
            return BasicError = BASIC_ERR_LOAD_DUPLICATE;
        if (!prog_add_line(ExecLine.number,&prog)) return BASIC_ERR_MEM_OUT;
    }*/
    FILE *prog = fopen(param.var.str,"r");
    if (prog == NULL)
    {
        BasicError = BASIC_ERR_FILE_NOT_FOUND;
        return BasicStat = BASIC_STAT_ERR;
    }
    char *lineBuffer = malloc(1024);
    if (lineBuffer == NULL)
    {
        fclose(prog);
        BasicError = BASIC_ERR_FILE_NOT_FOUND;
        return BasicStat = BASIC_STAT_ERR;
    }
    BasicError = BASIC_ERR_NONE;
    while (fgets(lineBuffer,1023,prog))
    {
        uint8_t *str = (uint8_t *)lineBuffer;
        while ((*str == ' ') || (*str == '\t')) str++;
        if (*str < '0' ) continue; // skip lines strarting with # or control char
        if ((!BasicError) && (ExecLine.number = (uint16_t)strtol((char *)str,(char **)&str,10)) == 0) BasicError = BASIC_ERR_LOAD_NONUMBER;
        if ((!BasicError) && prog_find_line(ExecLine.number)) BasicError = BASIC_ERR_LOAD_DUPLICATE;
        if ((!BasicError) && !prog_add_line(ExecLine.number,&str)) BasicError = BASIC_ERR_MEM_OUT;
        if (BasicError) break;
    }
    free(lineBuffer);
    fclose(prog);
    return BasicError ? BASIC_STAT_ERR : BASIC_STAT_OK;
}

_bas_stat_e basic_line_eval(void)
{
    uint8_t firstOp;

    while (1)
    {
        if(!*bToken.t[bToken.ptr].str && !bToken.t[bToken.ptr].op) return BasicStat = BASIC_STAT_SKIP; // empty line (rem found)
        if (bToken.t[bToken.ptr].op == '=') // variable assignement
        {
            if (BasicFunction[__OPCODE_LET & ~OPCODE_MASK].func(RPN_INT(0))) return BasicStat;
        }
        else
        {
            firstOp = bToken.t[bToken.ptr].op;
            char *str = bToken.t[bToken.ptr].str;
            if (*str == '\'') *str = (char)__OPCODE_REM;
            if((uint8_t)*str < OPCODE_MASK)
            {
                BasicError = BASIC_ERR_UNKNOWN_FUNC;
                return BasicStat = BASIC_STAT_ERR;
            }
            uint8_t opCode = *bToken.t[bToken.ptr++].str;
            if (opCode < __OPCODE_LAST)
            {
                if (BasicFunction[opCode & (~OPCODE_MASK)].func(RPN_INT(firstOp)))
                    return BasicStat;
            }
        }
        if (bToken.t[bToken.ptr].op == ':')
            ExecLine.statement ++;
        else if (!bToken.t[bToken.ptr].op || ((uint8_t)*bToken.t[bToken.ptr].str != __OPCODE_THEN)) return BasicStat = BASIC_STAT_OK;
        bToken.ptr++;
    }
    return BasicStat = BASIC_STAT_OK;
}

_bas_stat_e __run(_rpn_type_t param)
{
    _bas_line_t *bL = BasicProg;
    if (param.var.i)
        while (bL && (bL->number < param.var.i)) bL = bL->next; // find the executuion line (or the next one)
    if (bL == NULL) bL = BasicProg; // start from beginning
    //if (lineNum) printf("--- Running from #%d\n",bL->number);
    ExecLine.statement = 0;
    while (bL)
    {
        tokenizer((char *)bL->string);
        if (ExecLine.statement) // same line for/next implementation
            for (uint8_t s=0; s < ExecLine.statement && bToken.t[bToken.ptr].op ; s++)
                while (bToken.t[bToken.ptr++].op != ':')
                    if (!bToken.t[bToken.ptr].op) break;

        ExecLine.number = bL->number;
        NextLine.number = bL->next ? ((_bas_line_t *)bL->next)->number : 0;
        if (basic_line_eval() != BASIC_STAT_OK)
        {
            switch(BasicStat)
            {
            case BASIC_STAT_ERR:
                printf("\n%s, %d:%d\n",BErrorText[BasicError],ExecLine.number,ExecLine.statement);
                return BasicStat = BASIC_STAT_ERR;
            case BASIC_STAT_STOP:
                printf("\nStopped, %d:%d\n",ExecLine.number,ExecLine.statement);
                return BASIC_STAT_OK;
            case BASIC_STAT_JUMP:
            {
                if (bL->number == ExecLine.number) break;
                bL = BasicProg;
                while (bL && (bL->number != ExecLine.number))
                    bL = bL->next;
                if (!bL)
                {
                    BasicError = BASIC_ERR_INVALID_LINE;
                    return BasicStat = BASIC_STAT_ERR;
                }
            }
            break;
            default:
                bL = bL->next;
                ExecLine.statement = 0;
                break;
            }
        }
        else
        {
            bL = bL->next;
            ExecLine.statement = 0;
        }
    }
    printf("\nDone, %d:%d\n",ExecLine.number,ExecLine.statement);
    return BasicStat = BasicError ? BASIC_STAT_ERR : BASIC_STAT_OK;
}

_bas_err_e prog_load(char *progFileName)
{
    _rpn_type_t fileName = {.type=VAR_TYPE_STRING,.var.str = progFileName};
    __load(fileName);
    return BasicError;
}

void prog_run(uint16_t lineNumber)
{
    _rpn_type_t lNum = {.type=VAR_TYPE_INTEGER,.var.i = lineNumber};
    __run(lNum);
}
