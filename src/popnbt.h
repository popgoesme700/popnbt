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

#	ifdef sizeof
#		if sizeof(long)==8
#			define POPLIBS_POPNBTLONG long
#		else
#			define POPLIBS_POPNBTLONG long long
#		endif
#	else
#		define POPLIBS_POPNBTLONG long
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
		POPLIBS_POPNBTLONG int lnum;
		double dnum;
		poplibs_popnbttokencoords coords;
	} value;
	poplibs_popnbttokencoords name;
	enum poplibs_popnbttype type;
} poplibs_popnbttoken_t;

POPLIBS_POPNBTAPI void poplibs_popnbtparser_init(poplibs_popnbtparser *parser,const int bigendian);
POPLIBS_POPNBTAPI unsigned poplibs_popnbtparser_parse(poplibs_popnbtparser *parser,const char *buf,const unsigned bufsize,poplibs_popnbttoken_t *tokens,const unsigned tokensize);

#	ifndef POPLIBS_POPNBTCOMPILED

static poplibs_popnbttoken_t *popnbt_alloctoken(poplibs_popnbtparser *parser,poplibs_popnbttoken_t *tokens,const unsigned tokensize){
	poplibs_popnbttoken_t *token= POPLIBS_POPNBTNULL;
	if(parser->nexttok<tokensize){
		token= &tokens[parser->nexttok++];
		token->type= poplibs_popnbttype_end;
		token->name.start= 0;
		token->name.end= 0;
		token->value.coords.start= 0;
		token->value.coords.end= 0;
		token->value.dnum= 0.0f;
		token->value.lnum= 0;
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

static short popnbt_finalizeshort(poplibs_popnbtparser *parser,const short input){
	short rtn= input;
	if((parser->bigendian && popnbt_welittleendian()) || (!parser->bigendian && !popnbt_welittleendian())){
		unsigned short nin= input;
		rtn= (nin>>8 | nin<<8);
	}
	return rtn;
}

static int popnbt_finalizeint(poplibs_popnbtparser *parser,const int input){
	int rtn= input;
	if((parser->bigendian && popnbt_welittleendian()) || (!parser->bigendian && !popnbt_welittleendian())){
		unsigned int nin= input;
		rtn= (nin>>24) | ((nin<<8) & ((unsigned int) 0x00FF0000U)) | ((nin>>8) & ((unsigned int) 0x0000FF00U)) | (nin<<24);
	}
	return rtn;
}

static int popnbt_finalizelong(poplibs_popnbtparser *parser,const POPLIBS_POPNBTLONG int input){
	POPLIBS_POPNBTLONG int rtn= input;
	if((parser->bigendian && popnbt_welittleendian()) || (!parser->bigendian && !popnbt_welittleendian())){
		unsigned POPLIBS_POPNBTLONG int nin= input;
		rtn= (nin>>56) | ((nin<<40) & ((unsigned POPLIBS_POPNBTLONG int) 0x00FF000000000000ULL)) | ((nin<<24) & ((unsigned POPLIBS_POPNBTLONG int) 0x0000FF0000000000ULL)) | ((nin<<8) & ((unsigned POPLIBS_POPNBTLONG int) 0x000000FF00000000ULL)) | ((nin>>8) & ((unsigned POPLIBS_POPNBTLONG int) 0x00000000FF000000ULL)) | ((nin>>24) & ((unsigned POPLIBS_POPNBTLONG int) 0x0000000000FF0000ULL)) | ((nin>>40) & ((unsigned POPLIBS_POPNBTLONG int) 0x000000000000FF00ULL)) | (nin<<56);
	}
	return rtn;
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
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+1<bufsize){
					parser->pos++;
					token->value.lnum= buf[parser->pos];
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_short:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+2<bufsize){
					short val;
					char *valbt= (char*)&val;
					parser->pos++;
					valbt[0]= buf[parser->pos++];
					valbt[1]= buf[parser->pos];
					val= popnbt_finalizeshort(parser,val);
					token->value.lnum= val;
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_int:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+4<bufsize){
					int val;
					char *valbt= (char*)&val;
					parser->pos++;
					valbt[0]= buf[parser->pos++];
					valbt[1]= buf[parser->pos++];
					valbt[2]= buf[parser->pos++];
					valbt[3]= buf[parser->pos];
					val= popnbt_finalizeint(parser,val);
					token->value.lnum= val;
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_long:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+8<bufsize){
					POPLIBS_POPNBTLONG int val;
					char *valbt= (char*)&val;
					parser->pos++;
					valbt[0]= buf[parser->pos++];
					valbt[1]= buf[parser->pos++];
					valbt[2]= buf[parser->pos++];
					valbt[3]= buf[parser->pos++];
					valbt[4]= buf[parser->pos++];
					valbt[5]= buf[parser->pos++];
					valbt[6]= buf[parser->pos++];
					valbt[7]= buf[parser->pos];
					val= popnbt_finalizelong(parser,val);
					token->value.lnum= val;
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_float:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+4<bufsize){
					float val;
					char *valbt= (char*)&val;
					parser->pos++;
					valbt[0]= buf[parser->pos++];
					valbt[1]= buf[parser->pos++];
					valbt[2]= buf[parser->pos++];
					valbt[3]= buf[parser->pos];
					val= popnbt_finalizeint(parser,val);
					token->value.dnum= val;
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_double:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+8<bufsize){
					double val;
					char *valbt= (char*)&val;
					parser->pos++;
					valbt[0]= buf[parser->pos++];
					valbt[1]= buf[parser->pos++];
					valbt[2]= buf[parser->pos++];
					valbt[3]= buf[parser->pos++];
					valbt[4]= buf[parser->pos++];
					valbt[5]= buf[parser->pos++];
					valbt[6]= buf[parser->pos++];
					valbt[7]= buf[parser->pos];
					val= popnbt_finalizelong(parser,val);
					token->value.dnum= val;
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_bytearr:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+4<bufsize){
					int val;
					char *valbt= (char*)&val;
					parser->pos++;
					valbt[0]= buf[parser->pos++];
					valbt[1]= buf[parser->pos++];
					valbt[2]= buf[parser->pos++];
					valbt[3]= buf[parser->pos];
					val= popnbt_finalizeint(parser,val);
					if(val<0){
						val= 0;
					}
					if(parser->pos+val<bufsize){
						parser->pos++;
						token->value.coords.start= parser->pos;
						parser->pos+= val-1;
						token->value.coords.end= parser->pos;
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_string:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+2<bufsize){
					unsigned short val;
					char *valbt= (char*)&val;
					parser->pos++;
					valbt[0]= buf[parser->pos++];
					valbt[1]= buf[parser->pos];
					val= popnbt_finalizeshort(parser,val);
					if(parser->pos+val<bufsize){
						parser->pos++;
						token->value.coords.start= parser->pos;
						parser->pos+= val-1;
						token->value.coords.end= parser->pos;
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_list:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+1<bufsize){
					char typ;
					parser->pos++;
					typ= buf[parser->pos];
					if(parser->pos+4<bufsize){
						int i;
						int val;
						char *valbt= (char*)&val;
						parser->pos++;
						valbt[0]= buf[parser->pos++];
						valbt[1]= buf[parser->pos++];
						valbt[2]= buf[parser->pos++];
						valbt[3]= buf[parser->pos];
						val= popnbt_finalizeint(parser,val);
						if(val<0){
							val= 0;
						}
						for(i=0;i<val;i++){
							unsigned tmade= 0;
							unsigned cur= parser->nexttok;
							popnbt_grabtoken(parser,buf,bufsize,tokens,tokensize,&tmade,0);
							if(parser->err!=poplibs_popnbterror_none){
								break;
							}else if(tokens[cur].type!=typ){
								parser->err= poplibs_popnbterror_badid;
								mTok+= tmade;
								break;
							}
							mTok+= tmade;
						}
						if(parser->err==poplibs_popnbterror_none){
							token->value.lnum= val;
						}else{
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_compound:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(parser->pos+2<bufsize){
					unsigned short namelen;
					char *namelenbytes= (char*)&namelen;
					parser->pos++;
					namelenbytes[0]= buf[parser->pos++];
					namelenbytes[1]= buf[parser->pos];
					namelen= popnbt_finalizeshort(parser,namelen);
					if(parser->pos+(namelen)<bufsize){
						parser->pos++;
						token->name.start= parser->pos;
						parser->pos+= namelen-1;
						token->name.end= parser->pos;
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}else{
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+1<bufsize){
					unsigned POPLIBS_POPNBTLONG int tokpost= 0;
					parser->pos++;
					while(buf[parser->pos]!=0x00){
						unsigned tmade= 0;
						popnbt_grabtoken(parser,buf,bufsize,tokens,tokensize,&tmade,1);
						if(parser->err!=poplibs_popnbterror_none){
							break;
						}
						mTok+= tmade;
						tokpost++;
						parser->pos++;
					}
					if(parser->err!=poplibs_popnbterror_none){
						parser->nexttok-= mTok;
					}else if(buf[parser->pos]==0x00){
						token->value.lnum= tokpost;
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_intarr:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+4<bufsize){
					int val;
					char *valbt= (char*)&val;
					parser->pos++;
					valbt[0]= buf[parser->pos++];
					valbt[1]= buf[parser->pos++];
					valbt[2]= buf[parser->pos++];
					valbt[3]= buf[parser->pos];
					val= popnbt_finalizeint(parser,val);
					if(val<0){
						val= 0;
					}
					if(parser->pos+(val*4)<bufsize){
						parser->pos++;
						token->value.coords.start= parser->pos;
						parser->pos+= (val-1)*4;
						token->value.coords.end= parser->pos;
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
					parser->nexttok-= mTok;
				}
			}
			break;
		case poplibs_popnbttype_longarr:
			if((token= popnbt_alloctoken(parser,tokens,tokensize))!=POPLIBS_POPNBTNULL){
				token->type= (enum poplibs_popnbttype)cuf;
				mTok++;
				if(incompound){
					if(parser->pos+2<bufsize){
						unsigned short namelen;
						char *namelenbytes= (char*)&namelen;
						parser->pos++;
						namelenbytes[0]= buf[parser->pos++];
						namelenbytes[1]= buf[parser->pos];
						namelen= popnbt_finalizeshort(parser,namelen);
						if(parser->pos+(namelen)<bufsize){
							parser->pos++;
							token->name.start= parser->pos;
							parser->pos+= namelen-1;
							token->name.end= parser->pos;
						}else{
							parser->err= poplibs_popnbterror_part;
							parser->nexttok-= mTok;
						}
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}
				if(parser->err==poplibs_popnbterror_none && parser->pos+4<bufsize){
					int val;
					char *valbt= (char*)&val;
					parser->pos++;
					valbt[0]= buf[parser->pos++];
					valbt[1]= buf[parser->pos++];
					valbt[2]= buf[parser->pos++];
					valbt[3]= buf[parser->pos];
					val= popnbt_finalizeint(parser,val);
					if(val<0){
						val= 0;
					}
					if(parser->pos+(val*8)<bufsize){
						parser->pos++;
						token->value.coords.start= parser->pos;
						parser->pos+= (val-1)*8;
						token->value.coords.end= parser->pos;
					}else{
						parser->err= poplibs_popnbterror_part;
						parser->nexttok-= mTok;
					}
				}else if(parser->err==poplibs_popnbterror_none){
					parser->err= poplibs_popnbterror_part;
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
