#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <popnbt.h>

int main(const int argc,const char *argv[]){
	const char data[]= {0x0a,0x0b,0x00,'h','e','l','l','o',' ','w','o','r','l','d',0x08,0x04,0x00,'n','a','m','e',0x09,0x00,'B','a','n','a','n','r','a','m','a',0x00,0x0a,0x0d,0x00,'g','o','o','d','b','y','e',' ','w','o','r','l','d',0x08,0x04,0x00,'n','a','m','e',0x09,0x00,'A','p','p','l','e','r','a','m','a',0x00};
	poplibs_popnbtparser parser;
	poplibs_popnbttoken_t tokens[64];
	unsigned made;
	poplibs_popnbtparser_init(&parser,0);
	made= poplibs_popnbtparser_parse(&parser,data,68,tokens,64);
	return 0;
}