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
	poplibs_popnbterror_endtok= 1,
	poplibs_popnbterror_none=0,
	poplibs_popnbterror_part= -1,
	poplibs_popnbterror_nomem= -2,
	poplibs_popnbterror_badid= -3,
	poplibs_popnbterror_nostack= -4
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

typedef struct poplibs_popnbttoken_list{
	unsigned long items;
	enum poplibs_popnbttype type;
} poplibs_popnbttokenlist;

typedef struct poplibs_popnbt_stack{
	enum poplibs_popnbttype type;
	unsigned id;
	unsigned count;
} poplibs_popnbtstack;

typedef struct poplibs_popnbttoken{
	union{
		char data[8];
		unsigned long items;
		poplibs_popnbttokenlist list;
		poplibs_popnbttokencoords coords;
	} value;
	poplibs_popnbttokencoords name;
	enum poplibs_popnbttype type;
} poplibs_popnbttoken_t;

typedef struct poplibs_popnbt_parser{
	poplibs_popnbtstack *stack;
	poplibs_popnbttoken_t *tokens;
	enum poplibs_popnbterror err;
	unsigned stacksize;
	unsigned toksize;
	int nextstack;
	unsigned nexttok;
	unsigned pos;
	unsigned errPos;
	int bigendian;
} poplibs_popnbtparser;

POPLIBS_POPNBTAPI void poplibs_popnbtparser_init(poplibs_popnbtparser *parser,const int bigendian,poplibs_popnbtstack *stack,const unsigned stacksize,poplibs_popnbttoken_t *tokens,const unsigned tokensize);
POPLIBS_POPNBTAPI unsigned poplibs_popnbtparser_parse(poplibs_popnbtparser *parser,const void *vbuf,const unsigned bufsize);
POPLIBS_POPNBTAPI poplibs_popnbttoken_t *poplibs_popnbtparser_parsetoken(poplibs_popnbtparser *parser,const void *vbuf,const unsigned bufsize);
POPLIBS_POPNBTAPI void poplibs_popnbtparser_updatestack(poplibs_popnbtparser *parser,poplibs_popnbtstack *stack,const unsigned stacksize);
POPLIBS_POPNBTAPI void poplibs_popnbtparser_updatetokens(poplibs_popnbtparser *parser,poplibs_popnbttoken_t *tokens,const unsigned tokensize);

#	ifndef POPLIBS_POPNBTCOMPILED

static void *popnbt_memset(void *dest,const char set,const unsigned bytes){
	unsigned i;
	char *destbyt= (char*)dest;
	for(i=0;i<bytes;i++){
		destbyt[i]= set;
	}
	return dest;
}

static void *popnbt_memcpy(void *dest,const void *src,const unsigned bytes){
	unsigned i;
	char *destbyt= (char*)dest;
	const char *srcbyt= (char*)src;
	for(i=0;i<bytes;i++){
		destbyt[i]= srcbyt[i];
	}
	return dest;
}

static poplibs_popnbtstack *popnbt_stackpeek(const poplibs_popnbtparser *parser,const unsigned pos){
	return &parser->stack[parser->nextstack-pos];
}

static poplibs_popnbtstack *popnbt_stackpop(poplibs_popnbtparser *parser){
	return &parser->stack[parser->nextstack--];
}

static poplibs_popnbtstack *popnbt_stackpush(poplibs_popnbtparser *parser,enum poplibs_popnbttype type,const unsigned id,const unsigned val){
	poplibs_popnbtstack *sitem;
	parser->nextstack++;
	sitem= &parser->stack[parser->nextstack];
	sitem->type= type;
	sitem->id= id;
	sitem->count= val;
	return sitem;
}

static poplibs_popnbttoken_t *popnbt_alloctoken(poplibs_popnbtparser *parser){
	poplibs_popnbttoken_t *rtn= POPLIBS_POPNBTNULL;
	if(parser->nexttok<parser->toksize && parser->tokens!=POPLIBS_POPNBTNULL){
		rtn= &parser->tokens[parser->nexttok++];
		rtn->type= poplibs_popnbttype_end;
		rtn->name.start= 0;
		rtn->name.end= 0;
		popnbt_memset(rtn->value.data,0,8);
		rtn->value.items= 0;
		rtn->value.list.items= 0;
		rtn->value.list.type= poplibs_popnbttype_end;
		rtn->value.coords.start= 0;
		rtn->value.coords.end= 0;
	}
	return rtn;
}

static int popnbt_islittleendian(void){
	short check= 0x0001;
	char *out= (char*)&check;
	return out[0];
}

static void popnbt_finalizedata(poplibs_popnbtparser *parser,void *inp,const unsigned bytes){
	char *input= (char*)inp;
	if((parser->bigendian && popnbt_islittleendian()) || (!parser->bigendian && !popnbt_islittleendian())){
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
		}
	}
	return;
}

static void popnbt_applytokname(poplibs_popnbtparser *parser,const void *vbuf,const unsigned bufsize,poplibs_popnbttoken_t *token){
	char *buf= (char*)vbuf;
	if(parser->pos+2<bufsize){
		unsigned short strLen= 0;
		popnbt_memcpy(&strLen,&buf[parser->pos],2);
		popnbt_finalizedata(parser,&strLen,2);
		parser->pos+= 2;
		if(strLen>0){
			if(parser->pos+(strLen)<bufsize){
				unsigned start= parser->pos;
				parser->pos+= (strLen);
				if(token!=POPLIBS_POPNBTNULL){
					token->name.start= start;
					token->name.end= parser->pos-1;
				}
			}else{
				parser->err= poplibs_popnbterror_part;
			}
		}else if(token!=POPLIBS_POPNBTNULL){
			token->name.start= parser->pos;
			token->name.end= parser->pos-1;
		}
	}else{
		parser->err= poplibs_popnbterror_part;
	}
	return;
}

POPLIBS_POPNBTAPI poplibs_popnbttoken_t *poplibs_popnbtparser_parsetoken(poplibs_popnbtparser *parser,const void *vbuf,const unsigned bufsize){
	char *buf= (char*)vbuf;
	unsigned start= parser->pos;
	int getName= 0;
	poplibs_popnbttoken_t *rtn= POPLIBS_POPNBTNULL;
	enum poplibs_popnbttype typ= buf[parser->pos++];
	poplibs_popnbtstack *sitem= POPLIBS_POPNBTNULL;
	
	parser->err= poplibs_popnbterror_none;
	if(parser->stack!=POPLIBS_POPNBTNULL){
		if(parser->tokens!=POPLIBS_POPNBTNULL){
			while(parser->nextstack>=0){
				poplibs_popnbtstack *sitem= popnbt_stackpop(parser);
				poplibs_popnbttoken_t *stok= &parser->tokens[sitem->id];
				if(stok->type==poplibs_popnbttype_compound){
					if(typ==poplibs_popnbttype_end){
						parser->err= poplibs_popnbterror_endtok;
						parser->errPos= parser->pos-1;
						rtn= stok;
						break;
					}else{
						popnbt_stackpush(parser,sitem->type,sitem->id,sitem->count);
						getName|= 1;
						break;
					}
				}else{
					if(typ==stok->value.list.type && (parser->nexttok-1)-sitem->id<stok->value.list.items){
						popnbt_stackpush(parser,sitem->type,sitem->id,sitem->count);
						break;
					}
				}
			}
			if(parser->err==poplibs_popnbterror_none){
				getName|= 2;
			}
		}else{
			while(parser->nextstack>=0){
				poplibs_popnbtstack *sitem= popnbt_stackpop(parser);
				if(sitem->id==0){
					if(typ==poplibs_popnbttype_end){
						parser->err= poplibs_popnbterror_endtok;
						parser->errPos= parser->pos;
						break;
					}else{
						popnbt_stackpush(parser,sitem->type,sitem->id,sitem->count);
						getName|= 1;
						break;
					}
				}else{
					if(sitem->type==typ && sitem->count>0){
						popnbt_stackpush(parser,sitem->type,sitem->id,sitem->count);
						break;
					}
				}
			}
			if(parser->err==poplibs_popnbterror_none){
				getName|= 2;
			}
		}
	}else{
		getName|= 2;
	}
	if(getName&2){
		if((typ!=poplibs_popnbttype_compound && typ!=poplibs_popnbttype_list) || parser->stack!=POPLIBS_POPNBTNULL && (int)parser->nextstack<(int)parser->stacksize){
			poplibs_popnbttoken_t *tok= popnbt_alloctoken(parser);
			poplibs_popnbtstack *sitem= POPLIBS_POPNBTNULL;
			if(parser->stack!=POPLIBS_POPNBTNULL && parser->nextstack>=0){
				sitem= popnbt_stackpeek(parser,0);
			}
			if(tok==POPLIBS_POPNBTNULL && parser->tokens!=POPLIBS_POPNBTNULL){
				parser->err= poplibs_popnbterror_nomem;
				parser->errPos= parser->pos-1;
				parser->pos= start;
				return rtn;
			}
			if(getName&1 || typ==poplibs_popnbttype_compound){
				popnbt_applytokname(parser,buf,bufsize,tok);
			}
			if(parser->err==poplibs_popnbterror_none){
				tok->type= typ;
				switch(typ){
					case poplibs_popnbttype_byte:
						if(parser->pos+1<bufsize){
							if(tok!=POPLIBS_POPNBTNULL){
								tok->value.data[0]= buf[parser->pos++];
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					case poplibs_popnbttype_short:
						if(parser->pos+2<bufsize){
							if(tok!=POPLIBS_POPNBTNULL){
								popnbt_memcpy(tok->value.data,&buf[parser->pos],2);
								popnbt_finalizedata(parser,tok->value.data,2);
								parser->pos+= 1;
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					case poplibs_popnbttype_int:
						if(parser->pos+4<bufsize){
							if(tok!=POPLIBS_POPNBTNULL){
								popnbt_memcpy(tok->value.data,&buf[parser->pos],4);
								popnbt_finalizedata(parser,tok->value.data,4);
								parser->pos+= 4;
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					case poplibs_popnbttype_long:
						if(parser->pos+8<bufsize){
							if(tok!=POPLIBS_POPNBTNULL){
								popnbt_memcpy(tok->value.data,&buf[parser->pos],8);
								popnbt_finalizedata(parser,tok->value.data,8);
								parser->pos+= 8;
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					case poplibs_popnbttype_float:
						if(parser->pos+4<bufsize){
							if(tok!=POPLIBS_POPNBTNULL){
								popnbt_memcpy(tok->value.data,&buf[parser->pos],4);
								popnbt_finalizedata(parser,tok->value.data,4);
								parser->pos+= 4;
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					case poplibs_popnbttype_double:
						if(parser->pos+8<bufsize){
							if(tok!=POPLIBS_POPNBTNULL){
								popnbt_memcpy(tok->value.data,&buf[parser->pos],8);
								popnbt_finalizedata(parser,tok->value.data,8);
								parser->pos+= 8;
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					case poplibs_popnbttype_bytearr:
						if(parser->pos+4<bufsize){
							long len;
							popnbt_memcpy(&len,&buf[parser->pos],4);
							popnbt_finalizedata(parser,&len,4);
							parser->pos+= 4;
							if(len>0){
								if(parser->pos+(len)<bufsize){
									unsigned start= parser->pos;
									parser->pos+= len;
									if(tok!=POPLIBS_POPNBTNULL){
										tok->value.coords.start= start;
										tok->value.coords.end= parser->pos-1;
									}
								}else{
									parser->err= poplibs_popnbterror_part;
									parser->errPos= parser->pos;
									parser->pos= start;
									parser->nexttok--;
								}
							}else{
								if(tok!=POPLIBS_POPNBTNULL){
									tok->value.coords.start= parser->pos;
									tok->value.coords.end= parser->pos-1;
								}
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					case poplibs_popnbttype_string:
						if(parser->pos+2<bufsize){
							unsigned short len;
							popnbt_memcpy(&len,&buf[parser->pos],4);
							popnbt_finalizedata(parser,&len,4);
							parser->pos+= 2;
							if(len>=0){
								if(parser->pos+len<bufsize){
									unsigned start= parser->pos;
									parser->pos+= len;
									if(tok!=POPLIBS_POPNBTNULL){
										tok->value.coords.start= start;
										tok->value.coords.end= parser->pos-1;
									}
								}else{
									parser->err= poplibs_popnbterror_part;
									parser->errPos= parser->pos;
									parser->pos= start;
									parser->nexttok--;
								}
							}else{
								if(tok!=POPLIBS_POPNBTNULL){
									tok->value.coords.start= parser->pos;
									tok->value.coords.end= parser->pos-1;
								}
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					case poplibs_popnbttype_list:
						if(parser->pos+5<bufsize){
							enum poplibs_popnbttype ltyp= buf[parser->pos++];
							long len;
							popnbt_memcpy(&len,&buf[parser->pos],4);
							popnbt_finalizedata(parser,&len,4);
							parser->pos+= 4;
							if(len<=0 || ltyp!=poplibs_popnbttype_end){
								if(len<=0){
									len= 0;
								}
								if(tok!=POPLIBS_POPNBTNULL){
									tok->value.list.type= ltyp;
									tok->value.list.items= len;
									popnbt_stackpush(parser,ltyp,parser->nexttok-1,len);
								}else{
									popnbt_stackpush(parser,ltyp,1,len);
								}
							}else{
								parser->err= poplibs_popnbterror_badid;
								parser->errPos= parser->pos-4;
								parser->pos= start;
								parser->nexttok--;
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					case poplibs_popnbttype_compound:
						if(tok!=POPLIBS_POPNBTNULL){
							popnbt_stackpush(parser,poplibs_popnbttype_end,parser->nexttok-1,0);
						}else{
							popnbt_stackpush(parser,poplibs_popnbttype_end,0,0);
						}
						break;
					case poplibs_popnbttype_intarr:
						if(parser->pos+4<bufsize){
							long len;
							popnbt_memcpy(&len,&buf[parser->pos],4);
							popnbt_finalizedata(parser,&len,4);
							parser->pos+= 4;
							if(len>0){
								if(parser->pos+(len*4)<bufsize){
									unsigned start= parser->pos;
									parser->pos+= len*4;
									if(tok!=POPLIBS_POPNBTNULL){
										tok->value.coords.start= start;
										tok->value.coords.end= parser->pos-1;
									}
								}else{
									parser->err= poplibs_popnbterror_part;
									parser->errPos= parser->pos;
									parser->pos= start;
									parser->nexttok--;
								}
							}else{
								if(tok!=POPLIBS_POPNBTNULL){
									tok->value.coords.start= parser->pos;
									tok->value.coords.end= parser->pos-1;
								}
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					case poplibs_popnbttype_longarr:
						if(parser->pos+4<bufsize){
							long len;
							popnbt_memcpy(&len,&buf[parser->pos],4);
							popnbt_finalizedata(parser,&len,4);
							parser->pos+= 4;
							if(len>0){
								if(parser->pos+(len*8)<bufsize){
									unsigned start= parser->pos;
									parser->pos+= len*8;
									if(tok!=POPLIBS_POPNBTNULL){
										tok->value.coords.start= start;
										tok->value.coords.end= parser->pos-1;
									}
								}else{
									parser->err= poplibs_popnbterror_part;
									parser->errPos= parser->pos;
									parser->pos= start;
									parser->nexttok--;
								}
							}else{
								if(tok!=POPLIBS_POPNBTNULL){
									tok->value.coords.start= parser->pos;
									tok->value.coords.end= parser->pos-1;
								}
							}
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->errPos= parser->pos;
							parser->pos= start;
							parser->nexttok--;
						}
						break;
					default:
						parser->err= poplibs_popnbterror_badid;
						parser->errPos= parser->pos;
						parser->pos= start;
						parser->nexttok--;
						break;
				}
				if(parser->err==poplibs_popnbterror_none){
					if(sitem!=POPLIBS_POPNBTNULL){
						if(parser->tokens!=POPLIBS_POPNBTNULL){
							poplibs_popnbttoken_t *ntok= &parser->tokens[sitem->id];
							if(ntok->type==poplibs_popnbttype_compound){
								ntok->value.items++;
							}
						}else if(sitem->id==0){
							sitem->count++;
						}else if(sitem->id==1){
							sitem->count--;
						}
					}
					rtn= tok;
				}
			}else{
				parser->errPos= parser->pos;
				parser->pos= start;
			}
		}else{
			parser->err= poplibs_popnbterror_nostack;
			parser->errPos= parser->pos;
			parser->pos= start;
		}
	}
	return rtn;
}

POPLIBS_POPNBTAPI unsigned poplibs_popnbtparser_parse(poplibs_popnbtparser *parser,const void *vbuf,const unsigned bufsize){
	unsigned rtn= 0;
	parser->err= poplibs_popnbterror_none;
	while((parser->err==poplibs_popnbterror_none || parser->err==poplibs_popnbterror_endtok) && parser->pos<bufsize){
		poplibs_popnbtparser_parsetoken(parser,vbuf,bufsize);
		if(parser->err==poplibs_popnbterror_none){
			rtn++;
		}
	}
	if((parser->err==poplibs_popnbterror_none || parser->err==poplibs_popnbterror_endtok) && parser->nextstack>=0){
		parser->err= poplibs_popnbterror_part;
		parser->errPos= parser->pos;
	}else if(parser->err==poplibs_popnbterror_endtok){
		parser->err= poplibs_popnbterror_none;
	}
	return rtn;
}

POPLIBS_POPNBTAPI void poplibs_popnbtparser_updatestack(poplibs_popnbtparser *parser,poplibs_popnbtstack *stack,const unsigned stacksize){
	parser->stack= stack;
	parser->stacksize= stacksize;
	return;
}

POPLIBS_POPNBTAPI void poplibs_popnbtparser_updatetokens(poplibs_popnbtparser *parser,poplibs_popnbttoken_t *tokens,const unsigned tokensize){
	parser->tokens= tokens;
	parser->toksize= tokensize;
	return;
}

POPLIBS_POPNBTAPI void poplibs_popnbtparser_init(poplibs_popnbtparser *parser,const int bigendian,poplibs_popnbtstack *stack,const unsigned stacksize,poplibs_popnbttoken_t *tokens,const unsigned tokensize){
	parser->stack= stack;
	parser->tokens= tokens;
	parser->err= poplibs_popnbterror_none;
	parser->stacksize= stacksize;
	parser->toksize= tokensize;
	parser->nextstack= -1;
	parser->nexttok= 0;
	parser->pos= 0;
	parser->errPos= 0;
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
