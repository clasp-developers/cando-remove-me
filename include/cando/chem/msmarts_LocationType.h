/*
    File: msmarts_LocationType.h
*/
/*
Open Source License
Copyright (c) 2016, Christian E. Schafmeister
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
 
This is an open source license for the CANDO software from Temple University, but it is not the only one. Contact Temple University at mailto:techtransfer@temple.edu if you would like a different license.
*/
/* -^- */
#ifndef msmarts_LocationType_H
#define msmarts_LocationType_H

#include <clasp/core/foundation.h>
#include <clasp/core/object.h>
#include <clasp/core/lisp.h>
#include <clasp/core/cons.h>



// define the type for flex and bison                                                      
// Location aware type
//
// Defined in cons.h
//
#define YYLTYPE core::LispParserPos



#define YYLLOC_DEFAULT(Current, Rhs, N)                                \
         do                                                                  \
	     if (N)                                                            \
             {                                                               \
		 (Current).first_line   = YYRHSLOC(Rhs, 1).first_line;         \
		 (Current).first_column = YYRHSLOC(Rhs, 1).first_column;       \
		 (Current).last_line    = YYRHSLOC(Rhs, N).last_line;          \
		 (Current).last_column  = YYRHSLOC(Rhs, N).last_column;        \
             }                                                               \
	     else                                                              \
             {                                                               \
		 (Current).first_line   = (Current).last_line   =              \
		     YYRHSLOC(Rhs, 0).last_line;                                 \
		 (Current).first_column = (Current).last_column =              \
		     YYRHSLOC(Rhs, 0).last_column;                               \
             }                                                               \
         while (0)


#endif // msmarts_LocationType_H
