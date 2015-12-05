#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef char* String;

struct Tokenizer {
    String stream;
    size_t index;
};

typedef struct Tokenizer* Tokenizer;

Tokenizer createTokenizer(String s);

void destroyTokenizer(Tokenizer t);

void reallocate(void** pointer, size_t size);

void skip_whitespace(Tokenizer t);

String getNextToken(Tokenizer t);

String getNextWord(Tokenizer t);
