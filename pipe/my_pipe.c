// Also prints "Done" at the end of program

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

void Create(char* name, int d0, int d1, int d2) {
  pid_t id = fork();
  if (!id) {
    if (d0 != 0) {
      dup2(d0, 0);
      close(d0);
    }
    if (d1 != 1) {
      dup2(d1, 1);
      close(d1);
    }
    if (d2 != -1) {
      close(d2);
    }
    execlp(name, name, NULL);
    perror(name);
    exit(1);
  }
}

int main(int argc, char** argv) {
  if (argc == 2) {
    Create(argv[1], 0, 1, -1);
  } else {
    int p_prev[2];
    p_prev[0] = 0;
    p_prev[1] = 1;
    for (int i = 1; i < argc; i++) {
      if (i == argc - 1) {
        Create(argv[i], p_prev[0], 1, -1);
        close(p_prev[0]);
      } else {
        int p_curr[2];
        pipe(p_curr);

        Create(argv[i], p_prev[0], p_curr[1], p_curr[0]);
        close(p_curr[1]);
        if (p_prev[0] != 0) {
          close(p_prev[0]);
        }

        p_prev[0] = p_curr[0];
        p_prev[1] = p_curr[1];
      }
    }
  }

  for (int i = 1; i < argc; i++) {
    wait(NULL);
  }
  printf("Done\n");
  return 0;
}
