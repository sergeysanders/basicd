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
#include "bcore.h"
#include "bstring.h"
#include "bfunc.h"
#include "bprime.h"
#include "bstring.h"
#include "berror.h"
#include "banalizer.h"

#define SCREEN_PC 1
#if SCREEN_PC

#include "unistd.h"

#include <stdio.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <termios.h>
//#include <stropts.h>
static bool initialized = false;
static const int STDIN = 0;

int kbhit()
{
    if (! initialized)
    {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN, &term);
        //term.c_lflag &= ~ICANON;
        term.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
        /// disable cursor
        printf("\e[?25l");
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}

void screen_deinit(void)
{
    if (!initialized) return;
    struct termios term;
    tcgetattr(STDIN, &term);
    term.c_lflag |= (ICANON | ECHO);
    tcsetattr(STDIN, TCSANOW, &term);
    setbuf(stdin, NULL);
    /// enable cursor
    printf("\e[?25h");

}

#endif

_bas_err_e __at(_rpn_type_t *p2)
{
    _rpn_type_t *p1 = rpn_pull_queue();
    if ((p1->type < VAR_TYPE_FLOAT) || (p2->type < VAR_TYPE_FLOAT))
        return BasicError = (p1->type && p2->type) ? BASIC_ERR_TYPE_MISMATCH : BASIC_ERR_FEW_ARGUMENTS;
    //
    //  add limits check
    //
    uint8_t x = (uint8_t)(p1->type & VAR_TYPE_FLOAT ? p1->var.f : p1->var.i);
    uint8_t y = (uint8_t)(p2->type & VAR_TYPE_FLOAT ? p2->var.f : p2->var.i);

    printf("\e[%d;%dH",y,x);

    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e __ink(_rpn_type_t *p1)
{
    if (p1->type < VAR_TYPE_FLOAT) return BasicError = BASIC_ERR_TYPE_MISMATCH;
    uint8_t c = ((uint8_t)(p1->type & VAR_TYPE_FLOAT ? p1->var.f : p1->var.i)) & 0x07;
    printf("\e[%dm",30+c);
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e __paper(_rpn_type_t *p1)
{
    if (p1->type < VAR_TYPE_FLOAT) return BasicError = BASIC_ERR_TYPE_MISMATCH;
    uint8_t c = ((uint8_t)(p1->type & VAR_TYPE_FLOAT ? p1->var.f : p1->var.i)) & 0x07;
    printf("\e[%dm",40+c);
    return BasicError = BASIC_ERR_NONE;
};

_bas_err_e __cls(_rpn_type_t *param)
{
    printf("\e[2J");
    return BasicError = BASIC_ERR_NONE;
}

_bas_err_e __pause(_rpn_type_t *param)
{
    getchar();
    return BasicError = BASIC_ERR_NONE;
}

_bas_err_e __sleep(_rpn_type_t *param)
{
    _rpn_type_t *var;
    if (!param->var.i) return BasicError = BASIC_ERR_FEW_ARGUMENTS;
    if(token_eval_expression(param->var.i)) return BasicError;
    var = rpn_pull_queue();
    if (var->type < VAR_TYPE_FLOAT) return BasicError = BASIC_ERR_TYPE_MISMATCH;
    uint32_t t = (int32_t)(var->type & VAR_TYPE_FLOAT ? var->var.f : var->var.i);
    usleep(t*1000);
    return BasicError = BASIC_ERR_NONE;
}

_bas_err_e __inkey(_rpn_type_t *param)
{
    char c = 0;
    if (kbhit())
    {
        c = getchar();
        if ((c == 27) && kbhit()) // process cursor keys
            c = getchar();
    }
    return rpn_push_queue(RPN_BYTE(c));
}
