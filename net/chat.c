// 6 points chat

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

int d;
int clients[30];
int max_clients;

void catch() {
  for (int i = 0; i < max_clients; i++) {
    if (clients[i] != 0) {
      shutdown(clients[i], SHUT_RDWR);
      close(clients[i]);
    }
  }
  shutdown(d, SHUT_RDWR);
  close(d);
  exit(1);
}

int main(int argc , char *argv[])
{
  struct sigaction* sa = malloc(sizeof(struct sigaction));
  sa->sa_handler = catch;
  sigaction(SIGINT, sa, NULL);

  if ((d = socket(AF_INET , SOCK_STREAM , 0)) == 0) {
    perror("socket");
    exit(1);
  };

  int opt = 1;

  if (setsockopt (d, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof (opt)) < 0) {
    perror("setsockopt");
    exit(1);
  }

  max_clients = 30;
  struct sockaddr_in address;
  char buffer[1025];
  fd_set rfds;

  for (int i = 0; i < max_clients; i++)
  {
    clients[i] = 0;
  }

  struct sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));

  if(bind(d, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
    perror("bind");
    exit(1);
  }
  if(listen(d, 10)<0){
    perror("listen");
    exit(1);
  }

  int max_sd;
  while(1)
  {
    FD_ZERO(&rfds);
    FD_SET(d, &rfds);
    max_sd = d;

    for (int i = 0; i < max_clients; i++)
    {
      int sd = clients[i];
      if(sd > 0) {
        FD_SET(sd, &rfds);
      }
      if(sd > max_sd) {
        max_sd = sd;
      }
    }

    int activity = select(max_sd + 1, &rfds, NULL, NULL, NULL);

    if (FD_ISSET(d, &rfds))
    {
      int new_socket;
      int length = sizeof(servaddr);
      if ((new_socket = accept(d, (struct sockaddr *)&servaddr, (socklen_t*)&length)) < 0)
      {
        perror("accept");
        exit(1);
      }

      for (int i = 0; i < max_clients; i++)
      {
        if(clients[i] == 0)
        {
          clients[i] = new_socket;
          break;
        }
      }
    }

    for (int i = 0; i < max_clients; i++)
    {
      int sd = clients[i];
      if (FD_ISSET( sd , &rfds))
      {
        int buf_size = recv(sd, buffer, 1024, 0);
        buffer[buf_size] = '\0';
        if (!(strcmp(buffer, "quit\n") && strcmp(buffer, "quit\r\n"))) {
          shutdown(sd, SHUT_RDWR);
          close(sd);
          clients[i] = 0;
        }
        else {
          printf("%s", buffer);
          for (int j = 0; j < max_clients; j++) {
            if (clients[j] != 0 && j != i) {
              send(clients[j], buffer, strlen(buffer), 0);
            }
          }
        }
      }
    }
  }
  return 0;
}
