#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void ResizeString(char** my_strings, size_t current_string, size_t new_size) {
  my_strings[current_string] = realloc(my_strings[current_string], new_size);
  if (!my_strings[current_string]) {
    perror(NULL);
    exit(1);
  }
}

void EndOfString(char** my_strings, size_t* current_string, size_t* current_symbol, bool* new_string) {
  my_strings[*current_string][*current_symbol] = '\0';
  *current_string = (*current_string + 1) % 10;
  *current_symbol = 0;
  *new_string = true;
}

void IsNewString(char** my_strings, size_t current_string, size_t current_symbol, size_t sizes[], bool* new_string) {
  if (my_strings[current_string][current_symbol] != EOF) {
    free(my_strings[current_string]);
    sizes[current_string] = 1;
    my_strings[current_string] = malloc(sizes[current_string]);
  }
  *new_string = false;
}

void ReadStrings(FILE *f, char** my_strings, size_t sizes[], size_t* current_string, int* last_symbol) {
  int s;
  size_t current_symbol = 0;
  bool new_string = false;

  while ((s = fgetc(f)) != EOF) {
    if (current_symbol == sizes[*current_string]) {
      ResizeString(my_strings, *current_string, current_symbol * 2);
      sizes[*current_string] *= 2;
    }
    if (s == '\n') {
      EndOfString(my_strings, current_string, &current_symbol, &new_string);
    } else {
      if (new_string) {
        IsNewString(my_strings, *current_string, current_symbol, sizes, &new_string);
      }
      my_strings[*current_string][current_symbol] = (char) s;
      current_symbol++;
    }
    *last_symbol = s;
  }
}


void PrintStrings(char** my_strings, size_t current_string, int last_symbol) {
  size_t counter = 0;
  if (last_symbol != '\n') {
    current_string = (current_string + 1) % 10;
  }
  while (counter < 10) {
    if (my_strings[current_string][0] != EOF) {
      puts(my_strings[current_string]);
    }
    current_string = (current_string + 1) % 10;
    counter++;
  }
}

int main(int argc, char **argv) {
  FILE* f;

  if (argc == 1) {
    char** my_strings = malloc(10 * sizeof(char *));
    for (int i = 0; i < 10; i++) {
      my_strings[i] = malloc(1);
      my_strings[i][0] = EOF;
    }

    size_t sizes[10];
    for (int i = 0; i < 10; i++) {
      sizes[i] = 1;
    }

    size_t current_string = 0;
    int last_symbol = EOF;
    ReadStrings(stdin, my_strings, sizes, &current_string, &last_symbol);

    PrintStrings(my_strings, current_string, last_symbol);

    for (int i = 0; i < 10; i++) {
      free(my_strings[i]);
    }
    free(my_strings);
  }

  for (int i = 1; i < argc; i++) {
    char** my_strings = malloc(10 * sizeof(char *));

    for (int j = 0; j < 10; j++) {
      my_strings[j] = malloc(1);
      my_strings[j][0] = EOF;
    }

    size_t sizes[10];
    for (int j = 0; j < 10; j++) {
      sizes[j] = 1;
    }

    f = fopen(argv[i], "r");
    if (f == NULL) {
      printf("tail: open: %s: No such file or directory\n", argv[i]);
    } else {
      size_t current_string = 0;
      int last_symbol = EOF;
      ReadStrings(f, my_strings, sizes, &current_string, &last_symbol);

      if (argc > 2) {
        puts(argv[i]);
      }

      PrintStrings(my_strings, current_string, last_symbol);
      fclose(f);
    }
    for (int j = 0; j < 10; j++) {
      free(my_strings[j]);
    }
    free(my_strings);
  }
  return 0;
}
