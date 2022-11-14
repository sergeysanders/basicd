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

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

#ifndef bool
typedef bool uint8_t;
#endif

int main(int argc, char **argv)
{
    _bas_err_e err;
#define PRINT_OPCODES 0
#if PRINT_OPCODES
    for (uint8_t i=0; i<LastOpCode; i++)
    {
        printf("__OPCODE_");
        for (uint8_t j=0; j<strlen(BasicFunction[i].name); j++)
            printf("%c",toupper(BasicFunction[i].name[j]));
        printf(",\n");
    }
    printf("__OPCODE_LAST\n");
#else
    if (argc < 2)
    {
     printf("BasicD usage: basicd file.gas\n");
     exit(0);
    }
    
    if ((err = prog_load(argv[1])))
    {
        printf("%s, %d:%d \n",BErrorText[BasicError],ExecLine.number,ExecLine.statement);
        exit(0);
    }

    prog_run(0);
#endif
    return 0;
}
