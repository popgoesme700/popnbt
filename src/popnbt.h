#ifndef POPLIBS_POPNBT_H
#	define POPLIBS_POPNBT_H

#	ifdef POPLIBS_POPNBT_STATIC
#		define POPLIBS_POPNBTAPI static
#	else
#		define POPLIBS_POPNBTAPI extern
#	endif

#	ifdef NULL
#		define POPLIBS_POPNBTNULL NULL
#	else
#		define POPLIBS_POPNBTNULL (void*)0
#	endif

enum poplibs_popnbterror{
	poplibs_popnbterror_none=0,
	poplibs_popnbterror_part= -1,
	poplibs_popnbterror_nomem= -2,
	poplibs_popnbterror_badid= -3
};

enum poplibs_popnbttype{
	poplibs_popnbttype_end=0,
	poplibs_popnbttype_byte=1,
	poplibs_popnbttype_short=2,
	poplibs_popnbttype_int=3,
	poplibs_popnbttype_long=4,
	poplibs_popnbttype_float=5,
	poplibs_popnbttype_double=6,
	poplibs_popnbttype_bytearr=7,
	poplibs_popnbttype_string=8,
	poplibs_popnbttype_list=9,
	poplibs_popnbttype_compound=10,
	poplibs_popnbttype_intarr=11,
	poplibs_popnbttype_longarr=12
};

typedef struct poplibs_popnbttoken_coords{
	unsigned start;
	unsigned end;
} poplibs_popnbttokencoords;

typedef struct poplibs_popnbttoken{
	union{
		char data[8];
		long items;
		poplibs_popnbttokencoords coords;
	} value;
	poplibs_popnbttokencoords name;
	enum poplibs_popnbttype type;
} poplibs_popnbttoken_t;

typedef struct poplibs_popnbt_parser{
	unsigned *stack;
	poplibs_popnbttoken_t *tokens;
	enum poplibs_popnbterror err;
	unsigned stacksize;
	unsigned toksize;
	unsigned nextstack;
	unsigned nexttok;
	unsigned pos;
	unsigned errPos;
	int bigendian;
} poplibs_popnbtparser;

POPLIBS_POPNBTAPI void poplibs_popnbtparser_init(poplibs_popnbtparser *parser,const int bigendian,unsigned *stack,const unsigned stacksize,poplibs_popnbttoken_t *tokens,const unsigned tokensize);
POPLIBS_POPNBTAPI unsigned poplibs_popnbtparser_parse(poplibs_popnbtparser *parser,const char *buf,const unsigned bufsize);
POPLIBS_POPNBTAPI void poplibs_popnbtparser_parsetoken(poplibs_popnbtparser *parser,const char *buf,const unsigned bufsize,unsigned *tokid);

#	ifndef POPLIBS_POPNBTCOMPILED



#	endif

#endif

/*
MIT License

Copyright (c) 2021 Azrael James Addy

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/