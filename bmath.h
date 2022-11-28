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
#ifndef _BMATH_H_INCLUDED
#define _BMATH_H_INCLUDED

#include "rpn.h"
/// type
_bas_err_e __int(_rpn_type_t *param);
_bas_err_e __byte(_rpn_type_t *param);
/// math
_bas_err_e __abs(_rpn_type_t *param);
_bas_err_e __sin(_rpn_type_t *param);
_bas_err_e __cos(_rpn_type_t *param);
_bas_err_e __tan(_rpn_type_t *param);
_bas_err_e __atn(_rpn_type_t *param);
_bas_err_e __sqr(_rpn_type_t *param);
_bas_err_e __rnd(_rpn_type_t *param);
_bas_err_e __log(_rpn_type_t *param);
_bas_err_e __deg(_rpn_type_t *param);
_bas_err_e __rad(_rpn_type_t *param);
_bas_err_e __min(_rpn_type_t *param);
_bas_err_e __max(_rpn_type_t *param);

_bas_err_e __and(_rpn_type_t *p1);
_bas_err_e __or(_rpn_type_t *p1);
_bas_err_e __xor(_rpn_type_t *p1);
_bas_err_e __sl(_rpn_type_t *p1);
_bas_err_e __sr(_rpn_type_t *p1);

#endif //_BMATH_H_INCLUDED