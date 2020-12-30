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
	poplibs_popnbtparser_init(&parser,0);
	made= poplibs_popnbtparser_parse(&parser,data,26,tokens,64);
	printf("Made %u tokens.\n",made);
	if(made>=2){
		poplibs_popnbttoken_t *ourtok= &tokens[1];
		if(ourtok->type>=poplibs_popnbttype_byte && ourtok->type<=poplibs_popnbttype_long){
			char bnum;
			short snum;
			int inum;
			switch(ourtok->type){
				case poplibs_popnbttype_byte:
					bnum= ourtok->value.lnum;
					puts("Compound token 0 is a byte.");
					printf("Compound token 0 value: 0x%.2X\n",bnum);
					break;
				case poplibs_popnbttype_short:
					snum= ourtok->value.lnum;
					puts("Compound token 0 is a short.");
					printf("Compound token 0 value: 0x%.2X\n",snum);
					break;
				case poplibs_popnbttype_int:
					inum= ourtok->value.lnum;
					puts("Compound token 0 is a int.");
					printf("Compound token 0 value: 0x%.2X\n",inum);
					break;
				case poplibs_popnbttype_long:
					puts("Compound token 0 is a long int.");
					printf("Compound token 0 value: 0x%.2lX\n",ourtok->value.lnum);
					break;
				default:
					puts("Compound token 0 has somehow slipped between the cracks?");
					break;
			}
		}
	}
	return 0;
}
