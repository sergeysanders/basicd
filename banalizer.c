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

/***
 * https://en.wikipedia.org/wiki/Shunting_yard_algorithm
 * https://rosettacode.org/wiki/Parsing/Shunting-yard_algorithm
*/

struct _bas_token bToken;

const struct
{
    const uint8_t opCode;
    const uint8_t prec;
} PattOps[] =
{
    {'(',-1},
    {0x80,6}, // rpn function
    {OPERATOR_PWR, 6},
    {OPERATOR_MUL, 5},
    {OPERATOR_DIV, 5},
    {OPERATOR_MOD, 5},
    {OPERATOR_ADD, 4},
    {OPERATOR_SUB, 4},
    {OPERATOR_MORE, 3},
    {OPERATOR_LESS, 3},
    {OPERATOR_EQUAL, 3},
    {OPERATOR_MORE_EQ, 3},
    {OPERATOR_NOT_EQ, 3},
    {OPERATOR_LESS_EQ, 3},
    {OPERATOR_NOT, 2},
    {OPERATOR_OR, 1},
    {OPERATOR_AND, 1},
    {0}
};

static char get_precedence(uint8_t opCode)
{
    if (opCode > 0x80) return 6;
    if (opCode)
        for (uint8_t i=0; i<255 && PattOps[i].opCode; i++)
            if (PattOps[i].opCode == opCode) return PattOps[i].prec;
    return -1;
}

bool tokenizer(char *str) // return false if total number of bToken.ts is greater than PARSER_MAX_bToken.tS, otherwise true
{
    static char tokenString[BASIC_LINE_LEN];
    uint8_t strPtr = 0;
    bool trSpace = false;
    bool tokStr = false;
    bool done = false;
    bool quoted = false;

    strncpy(tokenString,str,BASIC_LINE_LEN);
    str = tokenString;
    bToken.ptr = 0;
    bToken.t[bToken.ptr].op = 0;
    while (!done)
    {
        switch ((uint8_t)str[strPtr])
        {
        case '\r':
        case '\n':
        case '\'': // remark
            if (quoted)
            {
                strPtr++;
                break;
            }
        case '\0':
            done = true;
            quoted = false;
        case '-':
            if (!done && (strPtr == 0))// || (*str & OPCODE_MASK))) // negative number
            {
                strPtr++;
                break;
            }
        case OPERATOR_DIV:
        case OPERATOR_MOD:
        case OPERATOR_PWR:
        case OPERATOR_MORE_EQ:
        case OPERATOR_NOT_EQ:
        case OPERATOR_LESS_EQ:
        case OPERATOR_AND:
        case OPERATOR_OR:
        case OPERATOR_NOT:
        case '[':
        case ']':
        case '(' ... ',':
        case ':' ... '>':
            if (quoted)
                strPtr++;
            else
                bToken.t[bToken.ptr].op = done ? 0x01 : str[strPtr];
            break;
        case '"': // process quoted string
            if (quoted)
            {
                if (!(strPtr && str[strPtr-1] == '\\')) // skip if it's '\"'
                {
                    quoted = false;
                    strPtr++;
                    break;
                }
            }
            else
                quoted = true;
        case '.':
        case '$': // string variable
        case '#': // integer variable
        case '_': // byte variable
        case '?':
        case '0' ... '9':
        case 'a' ... 'z':
        case 'A' ... 'Z':
        case FUNC_TYPE_SECONDARY ... 0xff:
            tokStr = true;
            if (trSpace) // second statement
                bToken.t[bToken.ptr].op = ' ';
            else strPtr++;
            break;

        case FUNC_TYPE_PRIMARY ... (FUNC_TYPE_SECONDARY-1):
            if (trSpace) // there is non terminated operand
            {
                bToken.t[bToken.ptr].op = ' ';
                strPtr--;
                break;
            }

            strPtr++;
            bToken.t[bToken.ptr].op = (str[strPtr] == '(' || str[strPtr] == ':') ? str[strPtr] : ' ';
            break;
        case '\\':
                if (quoted)
                    for(uint8_t tmpPtr = strPtr; (uint8_t)str[tmpPtr] > '\r';tmpPtr++) 
                        str[tmpPtr] = str[tmpPtr+1]; // skip the backslash
                strPtr++;
                break;
        default:
            if (!tokStr)str++; // remove leading spaces or unprocessed characters
            else
            {
                if (!quoted)
                {
                    str[strPtr] = 0;
                    trSpace = true;
                }
                strPtr++;
            }
        }
        if (bToken.t[bToken.ptr].op)
        {
            if(done) bToken.t[bToken.ptr].op = '\0';
            bToken.t[bToken.ptr++].str = str;
            str[strPtr++] = 0;
            str = &str[strPtr];
            strPtr = 0;
            tokStr = trSpace = quoted = false;
            bToken.t[bToken.ptr].op = '\0';
        }
        if (bToken.ptr >= PARSER_MAX_TOKENS) return false;
    }
    bToken.ptr = 0;
    return true;
}

_bas_err_e token_eval_expression(uint8_t opParam) // if subEval is true, the will evaluate the first bracked expression, including function
{
#define RPN_PRINT_DEBUG 0
    uint8_t parCnt = opParam == '(' ? 1 : 0;
    uint8_t opCode;
    char *tokenStr;
    _bas_var_t *variable;
    BasicError = BASIC_ERR_NONE;
    rpn_purge_queue();
    if ((opParam == '(') || (opParam == '!')) rpn_push_stack(opParam); // bracket from primary operator
    for (; bToken.ptr<PARSER_MAX_TOKENS; bToken.ptr++)
    {
        tokenStr = bToken.t[bToken.ptr].str;
        if (!tokenStr || ((*tokenStr & OPCODE_MASK) && ((uint8_t)*tokenStr < FUNC_TYPE_SECONDARY))) break; // primary operator found

        if (*tokenStr)
        {
            if (*tokenStr == '\"')
            {
                tokenStr[strlen(tokenStr)-1] = '\0'; // remove the last quote
                rpn_push_queue(RPN_STR(tokenStr+1)); // store without the first quote
            }
            else if(isdigit(*tokenStr) || (*tokenStr == '-') || (*tokenStr == '.')) // support numbers, negative numbers and float numbers starting with .
            {
                if (strchr(tokenStr,'.'))
                    rpn_push_queue(RPN_FLOAT(atof(tokenStr)));
                else
                    rpn_push_queue(RPN_INT(strtol(tokenStr,NULL,0)));
            }
            else
            {
                if ((variable = var_get(bToken.t[bToken.ptr].str)) != NULL)
                {
                    if (variable->value.type & VAR_TYPE_ARRAY) // array
                    {
                        if (bToken.t[bToken.ptr].op != '[') return BasicError = BASIC_ERR_PAR_MISMATCH;
                        if (rpn_push_stack(__OPCODE_ARRAY) != BASIC_ERR_NONE) return BasicError;
                        if (rpn_push_stack('[') != BASIC_ERR_NONE) return BasicError;
                        if (rpn_push_queue(((_rpn_type_t){.type = VAR_TYPE_ARRAY,.var.array = variable})) != BASIC_ERR_NONE) return BasicError;
                        parCnt++;
                        continue;
                    }
                    else
                    rpn_push_queue(variable->value);
                }
                else
                {
                    opCode = (uint8_t)*tokenStr;
                    if ((opCode & OPCODE_MASK) && (opCode < __OPCODE_LAST)) // check that the opcode is valid
                        rpn_push_stack((uint8_t)*tokenStr);
                    else
                        return BasicError = BASIC_ERR_UNKNOWN_VAR;
                }
            }
        }
        if ((!bToken.t[bToken.ptr].op) || (bToken.t[bToken.ptr].op == ':') || (bToken.t[bToken.ptr].op == ';')) /// eol, ":" or terminator ends the evaluation
            break;
        switch(bToken.t[bToken.ptr].op)
        {
        case ' ': // skip
            break;
        case ',': // skip if inside brackets
            break;
            if (!parCnt) // evaluate the stack and store in the queue
            {
                while ((opCode = rpn_pop_stack()))
                {
                    rpn_eval(opCode);
#if RPN_PRINT_DEBUG
                    rpn_print_queue(true);
                    rpn_print_stack(true);
                    if(BasicError) printf("RPN error(0): %s\n",BErrorText[BasicError]);
#endif
                    if (BasicError) return BasicError;
                }
            }
            break;
        case '(':
            parCnt++;
            rpn_push_stack(bToken.t[bToken.ptr].op);
            break;
        case ')':
        case ']':
            if (parCnt) parCnt--;
            else
                return BasicError = BASIC_ERR_PAR_MISMATCH;
            while (rpn_peek_stack_last() != '(' && rpn_peek_stack_last() != '[')
            {
                if (!rpn_peek_stack_last()) return BasicError = BASIC_ERR_PAR_MISMATCH;
                if (rpn_eval(rpn_pop_stack()) != BASIC_ERR_NONE) break;
            }
            rpn_pop_stack(); // remove the opening bracket
            if (rpn_peek_stack_last() > OPCODE_MASK) 
                if (rpn_eval(rpn_pop_stack()) != BASIC_ERR_NONE)  return BasicError; // evaluate function
            break;
        case '^': // ^ is evaluated right-to-left, natively to RPN, so just stack it
            rpn_push_stack(bToken.t[bToken.ptr].op);
            break;
        default:
            if (get_precedence(bToken.t[bToken.ptr].op) > get_precedence(rpn_peek_stack_last())) rpn_push_stack(bToken.t[bToken.ptr].op);
            else
            {
                while ((rpn_peek_stack_last() != '(') && (get_precedence(bToken.t[bToken.ptr].op) <= get_precedence(rpn_peek_stack_last())))
                    if (rpn_eval(rpn_pop_stack()) != BASIC_ERR_NONE) break;
                rpn_push_stack(bToken.t[bToken.ptr].op);
            }
        }
#if RPN_PRINT_DEBUG
        rpn_print_queue(true);
        rpn_print_stack(true);
        if(BasicError) printf("RPN error(1): %s\n",BErrorText[BasicError]);
#endif
        if (BasicError) return BasicError;
    }

    while ((opCode = rpn_pop_stack()))
    {
        rpn_eval(opCode);
#if RPN_PRINT_DEBUG
        rpn_print_queue(true);
        rpn_print_stack(true);
        if(BasicError) printf("RPN error(2): %s\n",BErrorText[BasicError]);
#endif
        if (BasicError) return BasicError;
    }
    return BasicError = parCnt ? BASIC_ERR_PAR_MISMATCH : BASIC_ERR_NONE;
}
