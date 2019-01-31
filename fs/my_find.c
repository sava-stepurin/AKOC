//Work as find argv[1] -name argv[2] -user argv[3]

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>

void FindFile(char* path, char* object_name, char* user_name) {
  DIR* dirp = opendir(path);
  if (dirp == NULL) {
    perror(NULL);
  } else {
    struct dirent* dp;
    struct stat buf;

    while ((dp = readdir(dirp)) != NULL) {
      char* new_path = malloc(2 + strlen(path) + strlen(dp->d_name));
      if (new_path == NULL) {
        perror(NULL);
        exit(1);
      }
      new_path[0] = '\0';
      strcat(new_path, path);
      strcat(new_path, "/");
      strcat(new_path, dp->d_name);
      stat(new_path, &buf);
      if (!strcmp(object_name, dp->d_name) && !strcmp(user_name, getpwuid(buf.st_uid)->pw_name)) {
        puts(new_path);
      }
      if (S_ISDIR(buf.st_mode) && strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")) {
        FindFile(new_path, object_name, user_name);
      }
      free(new_path);
    }
    free(dp);
    closedir(dirp);
  }
}

int main(int argc, char** argv) {
  FindFile(argv[1], argv[3], argv[5]);
  return 0;
}
