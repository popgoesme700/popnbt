# popnbt
A C NBT Tag parser. Exports NBT Tags as tokens, with arrays/strings having their start and end offset in the NBT data. On the lines of `popini` and `jsmn`.
## Using the library in your own projects.
Everytime you include `popnbt.h` which is the only file needed for using popnbt, you compile the library... due to this, its best practice to toss `popnbt.h` into a C file specifically for it, like `popnbt.c`. Afterwards, every inclusion of `popnbt.h` should has the line `#define POPLIBS_POPNBTCOMPILED` before the inclusion. If you want to compile popnbt for every C file that includes it, add this line `#define POPLIBS_POPNBT_STATIC` before including `popnbt.h`, this will compile all symbols of `popnbt.h` as static, meaning only the file including `popnbt.h` will have access to the symbols of `popnbt.h` that are "compiled"
## Some useful knowledge.
Due to popnbt using the same token approach as `popini` and `jsmn`, tokens are your main form of way to obtain the data of NBT tags in a somewhat readable fashion. Tokens all contain the following keys, `type`, `name`, and `value`. The `type` key stores the token's NBT type, the `name` key stores the token's NBT name coordinates, only applies to tokens belonging to a compound NBT token (including the compound NBT token itself), and the `value` key stores the value of the token, if any. it could be any of `lnum`, `dnum`, or `coords`. `lnum` stores the number in base 10 format, aka no fractions, only whole numbers. `dnum` stores the number as a double, meaning it could contain a fraction (and even NaN). `coords` stores the same kind of data a token's `name` key would store, which is a `start` and an `end` key. Those two keys store positions in the original NBT data that this token pertains to. Keep in mind all these keys inside of the token's `value` key are a union, so any of these can pointer to eachother's data, so its best to use the right `value` key for the token. byte, short, int, long, list, and compound tokens use the `lnum` key of the `value` key, for everything except list and compound tokens, it stores the normal value, for a list or compound token, itll store the amount of tokens afterwards are apart of it (excluding the tokens belonging to child list or compound tokens). float, and double NBT token's use the `dnum` key of the `value` key, which just stores its normal value. bytearray, string, intarray, and longarray use the `coords` key of the `value` key, which stores the `start` of the array or string's index into the the NBT data, and the `end` of the array or string's index into the NBT data, best to use a form of `substr` function, which will grab all data between these two key's numbers in the NBT data (including the position indicated by the keys).
### Functions
```c
void poplibs_popnbtparser_init(poplibs_popnbtparser *parser,const int bigendian);
```
This function initalizes a `poplibs_popnbtparser` structure, setting all data in it to zero except the `bigendian` key of the structure, which is set to the argument `bigendian`. The argument `bigendian` dictates whether the data is in bigendian or littleendian, in which itll try to convert it to the host's endian.
```c
unsigned poplibs_popnbtparser_parse(poplibs_popnbtparser *parser,const char *buf,const unsigned bufsize,poplibs_popnbttoken_t *tokens,const unsigned tokensize);
```
This function will parse NBT data passed to the argument `buf`. You would pass an *initalized* `poplibs_popnbtparser` structure into the function as the argument `parser`. You would also pass the NBT data as the argument `buf`, and the bytes in that NBT data as the argument `bufsize`. You would also need to pass the tokens array as argument `tokens` and the amount of tokens allocated in the array as the argument `tokensize`.
This function will return an `unsigned int` value which contains the amount of tokens this run of the function has made, you have to keep track of total made tokens yourself, possibly by adding up all the successive calls returns.
### Structures.
```c
typedef struct poplibs_popnbt_parser{
	enum poplibs_popnbterror err;
	unsigned pos;
	unsigned errPos;
	unsigned nexttok;
	int bigendian;
} poplibs_popnbtparser;
```
This structure stores the data for the parser to keep track of on each run of `poplibs_popnbtparser_parse`. The key `err` stores the errorcode enum value `poplibs_popnbterror`. The key `pos` stores the parser's current position in the NBT data. The key `errPos` stores the position in the NBT data that tripped off the errorcode at `err`, only set if `err` equals anything other then `poplibs_popnbterror_none`. The key `nexttok` stores the index of the next token to set. The key `bigendian` stores if the NBT data is bigendian or not, if the endianess of the NBT data isnt the same as the host's itll attempt to convert it.
```c
typedef struct poplibs_popnbttoken_coords{
	unsigned start;
	unsigned end;
} poplibs_popnbttokencoords;
```
This structure stores coordinate data that leads to positions in the NBT data to pull out. The key `start` stores the start of the data to pull out, and the key `end` stores the end of the data to pull out, every number inbetween these two keys are to also be added towards the final data.
```c
typedef struct poplibs_popnbttoken{
	union{
		POPLIBS_POPNBTLONG int lnum;
		double dnum;
		poplibs_popnbttokencoords coords;
	} value;
	poplibs_popnbttokencoords name;
	enum poplibs_popnbttype type;
} poplibs_popnbttoken_t;
```
This structure stores a NBT token. This token has a key `value` which stores the data this NBT token may hold, the integer at key `lnum` stores the data NBT tokens byte, short, int, long, list and compound would use, everything by the list and compound tokens store their actual data there, while list and compound store the amount of tokens (excluding those apart of child list and compound tokens) they own. the `dnum` key stores data a float or double NBT token would use, and stores their actual value. the `coords` key stores a `poplibs_popnbttokencoords` structure, which contains the start and end positions that a byte array, string, int array, and long array have their value/data contained in, your job would be to grab those specific bytes from the NBT data.
### Enums.
```c
enum poplibs_popnbterror{
	poplibs_popnbterror_none=0,
	poplibs_popnbterror_part= -1,
	poplibs_popnbterror_nomem= -2,
	poplibs_popnbterror_badid= -3
};
```
This enum contains the errorcodes that a `poplibs_popnbtparser` structure's `err` key could be set to. `poplibs_popnbterror_none` means no error has occurred. `poplibs_popnbterror_part` means that the NBT data passed into the parsing function is fractional/part of it, but isnt complete. `poplibs_popnbterror_nomem` means that the parsing function ran out of `poplibs_popnbttoken_t` structures in the array passed. `poplibs_popnbterror_badid` means that the parsing function ran into an NBT tag that doesn't have an accepted NBT tag type.
```c
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
```
This enum contains the types a NBT token could be set to. Every type in this enum, except `poplibs_popmcnbttype_end` can be seen as a token's type. Read about the `poplibs_popnbttoken_t` structure to figure out how each token type's data might be stored in the `poplibs_popnbttoken_t` structure's `value` key.
### Examples!
Currently i have not created an example program of this library. Look at the test unit at `test/src/main.c` or wait until i update this. The usage is very similar to how you would use `popini`.