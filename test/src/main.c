#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <popnbt.h>

int main(const int argc,const char *argv[]){
	const char data[]= {0x0a,0x0b,0x00,'h','e','l','l','o',' ','w','o','r','l','d',0x09,0x04,0x00,'n','a','m','e',0x01,0x03,0x00,0x00,0x00,0x01,0xff,0x01,0xf0,0x01,0x0f,0x00};
	/* const char data[]= {0x0a,0x00,0x0b,'h','e','l','l','o',' ','w','o','r','l','d',0x03,0x00,0x04,'n','a','m','e',0x50,0xf0,0x05,0x0f,0x00}; */
	poplibs_popnbtparser parser;
	poplibs_popnbttoken_t tokens[64];
	poplibs_popnbtstack stack[64];
	poplibs_popnbttoken_t *tokp= NULL;
	unsigned made;
	poplibs_popnbtparser_init(&parser,0,stack,64,tokens,3);
	made= poplibs_popnbtparser_parse(&parser,data,33);
	if(parser.err==poplibs_popnbterror_nomem){
		poplibs_popnbtparser_updatetokens(&parser,tokens,64);
		made+= poplibs_popnbtparser_parse(&parser,data,33);
	}
	printf("Made %u tokens!\n",made);
	return 0;
}
