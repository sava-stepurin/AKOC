#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char **argv) {
  FILE* f;
  f = fopen(argv[1], "r");
  if (f == NULL) {
    perror(NULL);
    exit(1);
  }

  size_t array_size = 1;
  size_t word_size = 1;
  char** array_of_words = malloc(array_size * sizeof(char *));
  array_of_words[0] = malloc(word_size);

  int symbol;
  size_t array_index = 0;
  size_t word_index = 0;
  bool end_of_word = false;
  bool first_symbol = false;

  while ((symbol = fgetc(f)) != EOF) {
    if (isspace(symbol) && first_symbol) {
      end_of_word = true;
    } else if (!isspace(symbol)) {
      first_symbol = true;
      if (word_index == word_size) {
        array_of_words[array_index] = realloc(array_of_words[array_index], word_size * 2);
        if (!array_of_words[array_index]) {
          perror(NULL);
          exit(1);
        }
        word_size *= 2;
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

      array_of_words[array_index][word_index] = (char)symbol;
      word_index++;
    }
  }

  if (end_of_word) {
    array_of_words[array_index] = realloc(array_of_words[array_index], word_size + 1);
    if (!array_of_words[array_index]) {
      perror(NULL);
      exit(1);
    }
    array_of_words[array_index][word_index] = '\0';
  }

  fclose(f);

  long long max_sons = 0;
  int i = 0;
  while (argv[2][i] != '\0') {
    max_sons = max_sons * 10 + (long long)(argv[2][i] - '0');
    i++;
  }

  int* descrip = malloc(sizeof(int));
  int descrip_size = 1;

  for (int i = 0; i < array_index + 1; i++) {

    if (i >= max_sons) {
      wait(NULL);
    }
    if (i == descrip_size) {
      descrip = realloc(descrip, sizeof(int) * descrip_size * 2);
      if (!descrip) {
        perror(NULL);
        exit(1);
      }
      descrip_size *= 2;
    }
    char buffer[] = "XXXXXX";
    descrip[i] = mkstemp(buffer);
    pid_t id = fork();
    if (!id) {
      dup2(descrip[i], 1);
      close(descrip[i]);
      execlp("/usr/bin/wc", "wc", array_of_words[i], NULL);
      perror("wc");
      exit(1);
    }
    unlink(buffer);
  }

  for (int i = 0; i < max_sons; i++) {
    wait(NULL);
  }
  long long a = 0, b = 0, c = 0;
  for (int i = 0; i < array_index + 1; i++) {
    long long result[3] = {0, 0, 0};
    lseek(descrip[i], 0, SEEK_SET);
    FILE* f = fdopen(descrip[i], "r");

    int temp = 0;
    int result_temp = 0;
    int symbol1;
    while ((symbol1 = fgetc(f)) == ' ') {}
    result[0] = (long long) (symbol1 - '0');
    bool flag = false;
    while ((symbol1 = fgetc(f)) != EOF) {
      if (isspace(symbol1) && !flag) {
        result_temp++;
        if (result_temp > 2) {
          break;
        }
        flag = true;
      } else if (!isspace(symbol1)) {
        if (result_temp <= 2) {
          result[result_temp] = result[result_temp] * 10 + (long long) (symbol1 - '0');
        }
        flag = false;
      }
      temp++;
    }
    a += result[0];
    b += result[1];
    c += result[2];
    fclose(f);
  }

  printf("%lld %lld %lld\n", a, b, c);

  for (int i = 0; i < array_index + 1; i++) {
    free(array_of_words[i]);
  }
  free(array_of_words);
  free(descrip);
  return 0;
}
