#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

#include "structures.h"

#define ASCII_CODE_A 65
#define ASCII_CODE_Z 90

int countLatinSymbols(Sentence* sentence) {
    int cnt = 0;
    for (int i=0; i<sentence->sentenceSize; i++) {
        int code = (int) towupper(sentence->line[i]);
        if (code >= ASCII_CODE_A && code <= ASCII_CODE_Z) cnt++;
    }
    return cnt;
}

int cmp(const void* p1, const void* p2) {
    return countLatinSymbols((Sentence*) p1) - countLatinSymbols((Sentence*) p2);
}

void sortByLatinSymbols(Text* text) {
    qsort(text->sentences, text->sentencesNumber, sizeof(Sentence), cmp);
}