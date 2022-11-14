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
#ifndef _BPRIME_H_INCLUDED
#define _BPRIME_H_INCLUDED

#include "rpn.h"

_bas_stat_e __rem(_rpn_type_t param);
_bas_stat_e __stop(_rpn_type_t param);
_bas_stat_e __print(_rpn_type_t param);
_bas_stat_e __input(_rpn_type_t param);
_bas_stat_e __let(_rpn_type_t param);
_bas_stat_e __for(_rpn_type_t param);
_bas_stat_e __to(_rpn_type_t param);
_bas_stat_e __step(_rpn_type_t param);
_bas_stat_e __next(_rpn_type_t param);
_bas_stat_e __goto(_rpn_type_t param);
_bas_stat_e __if(_rpn_type_t param);
_bas_stat_e __gosub(_rpn_type_t param);
_bas_stat_e __return(_rpn_type_t param);

#endif // _BPRIME_H_INCLUDED