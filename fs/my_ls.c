// Works as: ls -aR "names of objects", only recursion for 3 points


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void ReadDirectory(char* path) {
  DIR *dirp = opendir(path);
  printf("%s:\n", path);
  if (dirp == NULL) {
    perror(NULL);
  } else {
    struct dirent *dp;
    char** names = malloc(100 * sizeof(char *));
    size_t names_count = 0;
    size_t names_size = 100;
    struct stat buf;

    while ((dp = readdir(dirp)) != NULL) {
      if (names_count >= names_size) {
        names = realloc(names, names_size * 2 * sizeof(char *));
        if (!names) {
          perror(NULL);
          exit(1);
        }
        names_size *= 2;
      }
      names[names_count] = malloc(strlen(dp->d_name) + 1);
      if (!names[names_count]) {
        perror(NULL);
        exit(1);
      }
      names[names_count][0] = '\0';
      strcat(names[names_count], dp->d_name);
      names_count++;
      printf("%s ", dp->d_name);
    }
    printf("\n\n");
   
    for (size_t i = 0; i < names_count; i++) {
      char* new_path = malloc(2 + strlen(path) + strlen(names[i]));
      if (!new_path) {
        perror(NULL);
        exit(1);
      }
      new_path[0] = '\0';
      strcat(new_path, path);
      strcat(new_path, "/");
      strcat(new_path, names[i]);
      stat(new_path, &buf);
      if (S_ISDIR(buf.st_mode) && strcmp(names[i], ".") && strcmp(names[i], "..")) {
        ReadDirectory(new_path);
      }
      free(new_path);
    }

    for (size_t i = 0; i < names_count; i++) {
      free(names[i]);
    }
    free(names);
    free(dp);
    closedir(dirp);
  }
}

int main(int argc, char **argv) {
  if (argc == 2) {
    ReadDirectory(".");
  }
  for (int i = 2; i < argc; i++) {
    ReadDirectory(argv[i]);
  }
  return 0;
}
