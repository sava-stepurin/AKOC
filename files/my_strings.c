#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void PrintStrings(FILE* f) {
  int s;
  char* current_string = malloc(1);
  size_t size = 1;
  size_t current_index = 0;
  while ((s = fgetc(f)) != EOF) {
    if (current_index == size - 1) {
      current_string = realloc(current_string, size * 2);
      if (!current_string) {
        perror(NULL);
        exit(1);
      }
      size *= 2;
    }
    if (s == '\t' || s == ' ' || (s > 32 && s < 128)) {
      current_string[current_index] = (char) s;
      current_index++;
    } else {
      if (current_index >= 4) {
        current_string[current_index] = '\0';
        puts(current_string);
      }
      free(current_string);
      current_string = malloc(1);
      size = 1;
      current_index = 0;
    }
  }
  if (current_index >= 4) {
    current_string[current_index] = '\0';
    puts(current_string);
  }
  free(current_string);
}

int main(int argc, char** argv) {
  if (argc == 1) {
    PrintStrings(stdin);
  } else {
    FILE* f;
    for (int i = 1; i < argc; i++) {
      f = fopen(argv[i], "r");
      if (f == NULL) {
        printf("strings: %s: No such file or directory\n", argv[i]);
      } else {
        PrintStrings(f);
        fclose(f);
      }
    }
  }
  return 0;
}
