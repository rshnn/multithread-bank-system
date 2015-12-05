#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "tokenizer.h"

Tokenizer createTokenizer(String s) {
    Tokenizer t = (Tokenizer) malloc (sizeof(*t));
    t->index = 0;
    t->stream = (String) malloc (strlen(s)+1);
    strcpy(t->stream, s);
    return t;
}

void destroyTokenizer(Tokenizer t) {
    free(t->stream);
    free(t);
}

void reallocate(void** pointer, size_t size) {
    void* temp = realloc(*pointer, 2*size);
    *pointer = temp;
}

void skip_whitespace(Tokenizer t) {
    int x = t->index;
    while (t->stream[x] && !isalnum((int) t->stream[x])) {
        x++;
    }
    t->index = x;
}



String getNextToken(Tokenizer t) {

    String s = NULL;

    if (strlen(t->stream) > t->index) {
        skip_whitespace(t);
        if (t->stream[t->index]) {
            s = getNextWord(t);
        }
    }

    return s;
}

String getNextWord(Tokenizer t) {
    size_t index = t->index;
    size_t len = 2;
    size_t dot_count = 0;
    String token = (String) malloc (3);

    if(index >= strlen(t->stream))
        return NULL;

    while ( (isalnum((int) t->stream[index]) || (t->stream[index] == '.')) && (dot_count <= 1)) {
        if (t->stream[index] == '.') {
            dot_count++;
        }
        if (index - t->index + 1 == len) {
            reallocate((void**) &token, 2*len);
            len*=2;
        }
        token[index - t->index] = t->stream[index];
        index++;
    }
    token[index - t->index] = '\0';
    t->index = index;

    return token;
}
