#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <popnbt.h>

int main(const int argc,const char *argv[]){
	const char data[]= {0x0a,0x0b,0x00,'h','e','l','l','o',' ','w','o','r','l','d',0x03,0x04,0x00,'n','a','m','e',0x0f,0x05,0xf0,0x50,0x00};
	/* const char data[]= {0x0a,0x00,0x0b,'h','e','l','l','o',' ','w','o','r','l','d',0x03,0x00,0x04,'n','a','m','e',0x50,0xf0,0x05,0x0f,0x00}; */
	poplibs_popnbtparser parser;
	poplibs_popnbttoken_t tokens[64];
	unsigned made;
	return 0;
}
