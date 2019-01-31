#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdbool.h>
#include <wctype.h>

void Count(FILE *f, size_t* strings_count, size_t* words_count, size_t* symbols_count) {
  bool is_space = false;
  wint_t s;
  while ((s = fgetwc(f)) != WEOF) {
    if (iswspace(s)) {
      if (s == L'\n') {
        (*strings_count)++;
      }
      if (is_space == false) {
        (*words_count)++;
        is_space = true;
      }
    } else {
      is_space = false;
    }
    (*symbols_count)++;
  }

  if (is_space == false) {
    (*words_count)++;
  }

}

int main (int argc, char **argv) {
  FILE *f;
  if (argc == 1) {
    size_t symbols_count = 0;
    size_t words_count = 0;
    size_t strings_count = 0;
    Count(stdin, &strings_count, &words_count, &symbols_count);
    printf("%zu %zu %zu\n", strings_count, words_count, symbols_count);
  }

  size_t total_symbols_count = 0;
  size_t total_words_count = 0;
  size_t total_strings_count = 0;

  for (int i = 1; i < argc; i++) {
    f = fopen(argv[i], "r");
    if (f == NULL) {
      printf("wc: %s: No such file or directory\n", argv[i]);
    } else {
      size_t symbols_count = 0;
      size_t words_count = 0;
      size_t strings_count = 0;
      Count(f, &strings_count, &words_count, &symbols_count);

      printf("%zu %zu %zu %s\n", strings_count, words_count, symbols_count, argv[i]);

      total_strings_count += strings_count;
      total_symbols_count += symbols_count;
      total_words_count += words_count;
      fclose(f);
    }

  }
  if (argc > 2) {
    printf("%zu %zu %zu total\n", total_strings_count, total_words_count, total_symbols_count);
  }
}
