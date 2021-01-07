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

typedef struct poplibs_popnbt_parser{
	enum poplibs_popnbterror err;
	unsigned pos;
	unsigned errPos;
	unsigned nexttok;
	int bigendian;
} poplibs_popnbtparser;

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

POPLIBS_POPNBTAPI void poplibs_popnbtparser_init(poplibs_popnbtparser *parser,const int bigendian);
POPLIBS_POPNBTAPI unsigned poplibs_popnbtparser_parse(poplibs_popnbtparser *parser,const char *buf,const unsigned bufsize,poplibs_popnbttoken_t *tokens,const unsigned tokensize);

#	ifndef POPLIBS_POPNBTCOMPILED

static void *popnbt_memcpy(void *dest,const void *src,const unsigned bytes){
	if(dest!=POPLIBS_POPNBTNULL && src!=POPLIBS_POPNBTNULL){
		char *destbyt= (char*)dest;
		char *srcbyt= (char*)src;
		unsigned i;
		for(i=0;i<bytes;i++){
			destbyt[i]= srcbyt[i];
		}
	}
	return dest;
}

static void *popnbt_memset(void *dest,const int set,const unsigned bytes){
	if(dest!=POPLIBS_POPNBTNULL){
		char *destbyt= (char*)dest;
		unsigned i;
		for(i=0;i<bytes;i++){
			destbyt[i]= set;
		}
	}
	return dest;
}

static poplibs_popnbttoken_t *popnbt_alloctoken(poplibs_popnbtparser *parser,poplibs_popnbttoken_t *tokens,const unsigned tokensize){
	poplibs_popnbttoken_t *token= POPLIBS_POPNBTNULL;
	if(parser->nexttok<tokensize){
		token= &tokens[parser->nexttok++];
		token->type= poplibs_popnbttype_end;
		token->name.start= 0;
		token->name.end= 0;
		token->value.coords.start= 0;
		token->value.coords.end= 0;
		popnbt_memset(token->value.data,0,8);
		token->value.items= 0;
	}else{
		parser->err= poplibs_popnbterror_nomem;
	}
	return token;
}

static int popnbt_welittleendian(void){
	short check= 0x0001;
	char *out= (char*)&check;
	return out[0];
}

static void popnbt_finalizedata(poplibs_popnbtparser *parser,void *inp,const int bytes){
	char *input= inp;
	if((parser->bigendian && popnbt_welittleendian()) || (!parser->bigendian && !popnbt_welittleendian())){
		char swp[7];
		switch(bytes){
			case 2:
				swp[0]= input[0];
				input[0]= input[1];
				input[1]= swp[0];
				break;
			case 4:
				swp[0]= input[0];
				swp[1]= input[1];
				swp[2]= input[2];
				input[0]= input[3];
				input[1]= swp[2];
				input[2]= swp[1];
				input[3]= swp[0];
				break;
			case 8:
				swp[0]= input[0];
				swp[1]= input[1];
				swp[2]= input[2];
				swp[3]= input[3];
				swp[4]= input[4];
				swp[5]= input[5];
				swp[6]= input[6];
				input[0]= input[7];
				input[1]= swp[6];
				input[2]= swp[5];
				input[3]= swp[4];
				input[4]= swp[3];
				input[5]= swp[2];
				input[6]= swp[1];
				input[7]= swp[0];
				break;
			default:
				break;
		}
	}
	return;
}

static void popnbt_applytokname(poplibs_popnbtparser *parser,const char *buf,const unsigned bufsize,poplibs_popnbttoken_t *token){
	if(parser!=POPLIBS_POPNBTNULL){
		if(parser->pos+2<bufsize){
			unsigned int strLen= 0;
			parser->pos++;
			popnbt_memcpy(&strLen,&buf[parser->pos],2);
			popnbt_finalizedata(parser,&strLen,2);
			parser->pos++;
			if(strLen>0){
				if(parser->pos+(strLen)<bufsize){
					unsigned start;
					parser->pos++;
					start= parser->pos;
					parser->pos+= (strLen-1);
					if(token!=POPLIBS_POPNBTNULL){
						token->name.start= start;
						token->name.end= parser->pos;
					}else{
						parser->err= poplibs_popnbterror_part;
					}
				}
			}
		}else{
			parser->err= poplibs_popnbterror_part;
		}
	}
	return;
}

static void popnbt_grabtoken(poplibs_popnbtparser *parser,const char *buf,const unsigned bufsize,poplibs_popnbttoken_t *tokens,const unsigned tokensize,unsigned *made,const int incompound);
static void popnbt_grabtoken(poplibs_popnbtparser *parser,const char *buf,const unsigned bufsize,poplibs_popnbttoken_t *tokens,const unsigned tokensize,unsigned *made,const int incompound){
	char cuf= buf[parser->pos];
	unsigned start= parser->pos;
	unsigned mTok= 0;
	poplibs_popnbttoken_t *token;
	switch(cuf){
		case poplibs_popnbttype_byte:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+1<bufsize){
					parser->pos++;
					if(token!=POPLIBS_POPNBTNULL){
						token->type= cuf;
						token->value.data[0]= buf[parser->pos];
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_short:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+2<bufsize){
					char val[2];
					parser->pos++;
					popnbt_memcpy(val,&buf[parser->pos],2);
					parser->pos++;
					popnbt_finalizedata(parser,&val,2);
					if(token!=POPLIBS_POPNBTNULL){
						token->type= cuf;
						popnbt_memcpy(token->value.data,val,2);
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_int:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+4<bufsize){
					char val[4];
					parser->pos++;
					popnbt_memcpy(val,&buf[parser->pos],4);
					parser->pos+= 3;
					popnbt_finalizedata(parser,&val,4);
					if(token!=POPLIBS_POPNBTNULL){
						token->type= cuf;
						popnbt_memcpy(token->value.data,val,4);
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_long:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+8<bufsize){
					char val[8];
					parser->pos++;
					popnbt_memcpy(val,&buf[parser->pos],8);
					parser->pos+= 7;
					popnbt_finalizedata(parser,&val,8);
					if(token!=POPLIBS_POPNBTNULL){
						token->type= cuf;
						popnbt_memcpy(token->value.data,val,8);
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_float:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+4<bufsize){
					char val[4];
					parser->pos++;
					popnbt_memcpy(val,&buf[parser->pos],4);
					parser->pos+= 3;
					popnbt_finalizedata(parser,&val,4);
					if(token!=POPLIBS_POPNBTNULL){
						token->type= cuf;
						popnbt_memcpy(token->value.data,val,4);
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_double:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+8<bufsize){
					char val[8];
					parser->pos++;
					popnbt_memcpy(val,&buf[parser->pos],8);
					parser->pos+= 7;
					popnbt_finalizedata(parser,&val,8);
					if(token!=POPLIBS_POPNBTNULL){
						token->type= cuf;
						popnbt_memcpy(token->value.data,val,8);
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_bytearr:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+4<bufsize){
					long val;
					parser->pos++;
					popnbt_memcpy(&val,&buf[parser->pos],4);
					parser->pos+= 3;
					popnbt_finalizedata(parser,&val,4);
					if(val>0){
						if(parser->pos+(val)<bufsize){
							unsigned start;
							parser->pos++;
							start= parser->pos;
							parser->pos+= (val-1);
							if(token!=POPLIBS_POPNBTNULL){
								token->type= cuf;
								token->value.coords.start= start;
								token->value.coords.end= parser->pos;
							}
						}
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_string:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+2<bufsize){
					unsigned int val;
					parser->pos++;
					popnbt_memcpy(&val,&buf[parser->pos],2);
					parser->pos++;
					popnbt_finalizedata(parser,&val,2);
					if(val>0){
						if(parser->pos+(val)<bufsize){
							unsigned start;
							parser->pos++;
							start= parser->pos;
							parser->pos+= (val-1);
							if(token!=POPLIBS_POPNBTNULL){
								token->type= cuf;
								token->value.coords.start= start;
								token->value.coords.end= parser->pos;
							}
						}
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_list:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+1<bufsize){
					char ltype;
					parser->pos++;
					ltype=  buf[parser->pos];
					if(parser->pos+4<bufsize){
						long val;
						parser->pos++;
						popnbt_memcpy(&val,&buf[parser->pos],4);
						parser->pos+= 3;
						popnbt_finalizedata(parser,&val,4);
						if(val>0){
							if(ltype!=poplibs_popnbttype_end){
								unsigned i;
								for(i=0;i<val;i++){
									unsigned tmade= 0;
									unsigned cur= parser->pos;
									popnbt_grabtoken(parser,buf,bufsize,tokens,tokensize,&tmade,0);
									if(parser->err!=poplibs_popnbterror_none){
										break;
									}else if(buf[cur+1]!=ltype){
										parser->err= poplibs_popnbterror_badid;
										mTok+= tmade;
										parser->nexttok-= mTok;
										break;
									}
									mTok+= tmade;
								}
							}else{
								parser->err= poplibs_popnbterror_badid;
								parser->nexttok-= mTok;
							}
						}
						if(token!=POPLIBS_POPNBTNULL){
							token->type= cuf;
							token->value.items= val;
						}
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_compound:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				popnbt_applytokname(parser,buf,bufsize,token);
				if(parser->err==poplibs_popnbterror_none && parser->pos+1<bufsize){
					char ltype;
					unsigned i;
					parser->pos++;
					ltype=  buf[parser->pos];
					while(buf[parser->pos]!=poplibs_popnbttype_end){
						unsigned tmade= 0;
						popnbt_grabtoken(parser,buf,bufsize,tokens,tokensize,&tmade,1);
						if(parser->err!=poplibs_popnbterror_none){
							break;
						}
						mTok+= tmade;
						i++;
						parser->pos++;
					}
					if(buf[parser->pos]!=poplibs_popnbttype_end){
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
					parser->pos--;
					if(token!=POPLIBS_POPNBTNULL){
						token->type= cuf;
						token->value.items= i;
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_intarr:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+4<bufsize){
					long val;
					parser->pos++;
					popnbt_memcpy(&val,&buf[parser->pos],4);
					parser->pos+= 3;
					popnbt_finalizedata(parser,&val,4);
					if(val>0){
						if(parser->pos+(val*4)<bufsize){
							unsigned start;
							parser->pos++;
							start= parser->pos;
							parser->pos+= (val-1)*4;
							if(token!=POPLIBS_POPNBTNULL){
								token->type= cuf;
								token->value.coords.start= start;
								token->value.coords.end= parser->pos;
							}
						}
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_longarr:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				mTok++;
				if(incompound){
					popnbt_applytokname(parser,buf,bufsize,token);
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+4<bufsize){
					long val;
					parser->pos++;
					popnbt_memcpy(&val,&buf[parser->pos],4);
					parser->pos+= 3;
					popnbt_finalizedata(parser,&val,4);
					if(val>0){
						if(parser->pos+(val*8)<bufsize){
							unsigned start;
							parser->pos++;
							start= parser->pos;
							parser->pos+= (val-1)*8;
							if(token!=POPLIBS_POPNBTNULL){
								token->type= cuf;
								token->value.coords.start= start;
								token->value.coords.end= parser->pos;
							}
						}
					}
				}else{
					if(parser->err==poplibs_popnbterror_none){
						parser->err= poplibs_popnbterror_part;
					}
					parser->nexttok-= mTok;
				}
			}
			break;
		default:
			parser->err= poplibs_popnbterror_badid;
			parser->nexttok-= mTok;
			parser->pos++;
			break;
	}
	if(parser->err==poplibs_popnbterror_none){
		*made+= mTok;
	}
	return;
}

POPLIBS_POPNBTAPI unsigned poplibs_popnbtparser_parse(poplibs_popnbtparser *parser,const char *buf,const unsigned bufsize,poplibs_popnbttoken_t *tokens,const unsigned tokensize){
	char cuf= '\0';
	unsigned made= 0;
	unsigned start= parser->pos;
	parser->err= poplibs_popnbterror_none;
	for(;parser->pos<bufsize;parser->pos++){
		cuf= buf[parser->pos];
		popnbt_grabtoken(parser,buf,bufsize,tokens,tokensize,&made,0);
		if(parser->err!=poplibs_popnbterror_none){
			parser->errPos= parser->pos;
			parser->pos= start;
			break;
		}
		start= parser->pos;
	}
	return made;
}

POPLIBS_POPNBTAPI void poplibs_popnbtparser_init(poplibs_popnbtparser *parser,const int bigendian){
	parser->err= poplibs_popnbterror_none;
	parser->errPos= 0;
	parser->nexttok= 0;
	parser->pos= 0;
	parser->bigendian= bigendian;
	return;
}

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