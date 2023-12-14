#include "io.h"

#include <locale.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>

#include "config.h"
#include "vector.h"

#define PARSE_ELLIPSIS 0

void remove_duplicats(Text** _text);

void read_text(Text** _text) {
    setlocale(LC_ALL, LOCALE);
    append_new_string(_text);
    int newline = 0;
    int ended = 0;
    int dots = 0;
    int space = 0;
    int word = 0;
    for (wchar_t ch = getwchar(); !(newline == 1 && ch == L'\n'); ch = getwchar()) {
        switch (ch) {
            case L'.':
                dots++;
                word = 0;
                space = 0;
#if !PARSE_ELLIPSIS
                if (dots < 2)
#endif
                    append_wchar(_text, ch);
                if (dots)
                    ended = 1;
#if PARSE_ELLIPSIS
                if (dots == 3)
                    ended = 0;
#endif
                break;
            case L'\n':
                newline++;
                if (!space && !ended) {
                    append_wchar(_text, L' ');
                    space = 1;
                }

                break;
            case L'\t':
            case L' ':
            case L',':
#if PARSE_ELLIPSIS
                if (dots == 1)
#else
                if (dots)
#endif
                    ended = 1;
                else
                    dots = 0;
                space = 1;
                word = 0;
                if (!ended)
                    append_wchar(_text, ch);
                break;
            default:
                if (ended)
                    append_new_string(_text);
                if (!word) {
                    (*last_string(_text))->words++;
                    word = 1;
                }
                ended = 0;
                newline = 0;
                dots = 0;
                space = 0;
                append_wchar(_text, ch);
                break;
        }
    }
    remove_duplicats(_text);
}

void output_text(Text** _text) {
    setlocale(LC_ALL, LOCALE);
    for (unsigned int i = 0; i < (*_text)->len; i++)
        if ((*_text)->body[i]->len != 0)
            printf("%ls\n", (*_text)->body[i]->body);
}

void remove_duplicats(Text** _text) {
    for (unsigned int i = 0; i < (*_text)->len - 1; i++)
        if ((*_text)->body[i]->len != 0)
            for (unsigned int j = i + 1; j < (*_text)->len; j++)
                if (wcscasecmp((*_text)->body[i]->body, (*_text)->body[j]->body) == 0)
                    clear(&(*_text)->body[j]);
};
