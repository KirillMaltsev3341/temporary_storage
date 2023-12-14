#include "regex_task.h"

#include <locale.h>
#include <stdlib.h>
#include <wchar.h>

#include "util/config.h"
#include "util/error.h"

int check_word(wchar_t* word, wchar_t* expression);

void regex_task(Text** _text, wchar_t* expression) {
    const wchar_t* seps = L" ,.";
    wchar_t* context = NULL;
    setlocale(LC_ALL, LOCALE);

    for (unsigned int i = 0; i < (*_text)->len; i++) {
        char all_match = 1;
        wchar_t* str_dup = wcsdup((*_text)->body[i]->body);
        wchar_t* word = wcstok(str_dup, seps, &context);
        while (word != NULL) {
            if (!check_word(word, expression)) {
                all_match = 0;
                break;
            }
            word = wcstok(NULL, seps, &context);
        }
        free(str_dup);
        if (!all_match)
            clear(&(*_text)->body[i]);
    }
}

int check_word(wchar_t* word, wchar_t* expression) {
    int n = 0;
    int match = 0;
    if (expression[n] == L'*' && expression[n + 1] == L'\0' && word[n] != L'\0' ||
        (expression[n] == L'\0' && word[n] == L'\0')) {
        return 1;
    }

    while (expression[n] != L'\0' && word[n] != L'\0') {
        if (expression[n] == L'*') {
            if (word[n + 1] == L'\0')
                return 1;

            for (unsigned int i = 0; word[i] != L'\0'; i++) {
                if (check_word(&word[n + i], &expression[n + 1]))
                    return 1;
            }
            return 0;
        } else if (expression[n] == L'?' || expression[n] == word[n]) {
            n++;
        } else {
            return 0;
            break;
        }
        match = 1;
    }
    if (expression[n] == L'\0' && word[n] != L'\0')
        return 0;

    return match;
}
