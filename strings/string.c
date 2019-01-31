#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int Compare(const void* a, const void* b)
{
  const char* aa = *(const char**)a;
  const char* bb = *(const char**)b;
  return strcmp(aa,bb);
}

void ResizeWord(char** array_of_words, size_t array_index, size_t word_size, size_t new_size) {
  array_of_words[array_index] = realloc(array_of_words[array_index], new_size);
  if (!array_of_words[array_index]) {
    perror(NULL);
    exit(1);
  }
}

int main() {
  size_t array_size = 1;
  size_t word_size = 1;
  char** array_of_words = malloc(array_size * sizeof(char *));
  array_of_words[0] = malloc(word_size);

  int symbol;
  size_t array_index = 0;
  size_t word_index = 0;
  bool end_of_word = false;
  int previous_symbol = EOF;
  size_t symbol_count = 0;
  bool in_single_quotes = false;
  bool in_double_quotes = false;

  while ((symbol = getchar()) != EOF) {
    if (in_double_quotes || in_single_quotes) {
      if (in_double_quotes && symbol == '\"') {
        in_double_quotes = false;
      } else if (in_single_quotes && symbol == '\'') {
        in_single_quotes = false;
      } else {
        if (word_index == word_size) {
          ResizeWord(array_of_words, array_index, word_size, word_size * 2);
          word_size *= 2;
        }


        array_of_words[array_index][word_index] = symbol;
        word_index++;
        previous_symbol = symbol;
      }
    } else {
      if (symbol == '\"' || symbol == '\'') {
        if (end_of_word) {
          ResizeWord(array_of_words, array_index, word_size, word_size + 1);
          array_of_words[array_index][word_index] = '\0';
          array_index++;

          if (array_index == array_size) {
            array_of_words = realloc(array_of_words, array_size * 2 * sizeof(char *));
            if (!array_of_words) {
              perror(NULL);
              exit(1);
            }
            array_size *= 2;
          }

          word_size = 1;
          word_index = 0;
          array_of_words[array_index] = malloc(word_size);

          end_of_word = false;
        }

        if (symbol == '\"') {
          in_double_quotes = true;
        } else {
          in_single_quotes = true;
        }
      } else if (isspace(symbol)) {
        end_of_word = true;
        previous_symbol = symbol;
      } else {
        if (word_index == word_size) {
          ResizeWord(array_of_words, array_index, word_size, word_size * 2);
          word_size *= 2;
        }

        if ((symbol == ';') || (symbol == '|') || (symbol == '&')) {
          if ((((previous_symbol != symbol) || (symbol == ';')) && (symbol_count != 0))
              || (symbol_count != 1 && previous_symbol != EOF)) {
            end_of_word = true;
            symbol_count = 0;
          }
          symbol_count++;
        } else {
          if ((previous_symbol == ';') || (previous_symbol == '|') || (previous_symbol == '&')) {
            end_of_word = true;
          }
          symbol_count = 0;
        }

        if (end_of_word) {
          array_of_words[array_index][word_index] = '\0';
          array_index++;

          if (array_index == array_size) {
            array_of_words = realloc(array_of_words, array_size * 2 * sizeof(char *));
            if (!array_of_words) {
              perror(NULL);
              exit(1);
            }
            array_size *= 2;
          }

          word_size = 1;
          word_index = 0;
          array_of_words[array_index] = malloc(word_size);

          end_of_word = false;
        }


        array_of_words[array_index][word_index] = symbol;
        word_index++;
        previous_symbol = symbol;
      }
    }
  }

  if (in_double_quotes || in_single_quotes) {
    fprintf(stderr, "quotes error\n");
    printf("quotes error\n");
    exit(1);
  }

  ResizeWord(array_of_words, array_index, word_size, word_size + 1);
  array_of_words[array_index][word_index] = '\0';

  qsort(array_of_words, array_index + 1, sizeof(char *), Compare);

  for (int i = 0; i < array_index + 1; i++) {
    printf("\"%s\"\n", array_of_words[i]);
  }

  for (int i = 0; i < array_index + 1; i++) {
    free(array_of_words[i]);
  }
  free(array_of_words);

  return 0;
}
