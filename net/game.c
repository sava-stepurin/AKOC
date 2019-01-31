// 10 points game, also takes port as chat

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

struct Client {
  int sd, hp, round_hp, kill_sd, heal_sd;
  char login[11];
};

int d;
struct Client clients[30];
int max_clients;

void* p(void* arg) {
  while(1) {
    sleep(20);
    char end_round[] = "End of round!\r\n";
    for (int i = 0; i < max_clients; i++) {
      if (clients[i].sd != 0) {
        send(clients[i].sd, end_round, strlen(end_round), 0);
      }
    }
    
    for (int i = 0; i < max_clients; i++) {
      if (clients[i].sd != 0) {
        clients[i].hp += clients[i].round_hp;
        clients[i].round_hp = 0;
        if (clients[i].hp > 100) {
          clients[i].hp = 100;
        }
        if (clients[i].hp <= 0) {
          char you_died[] = "You are killed\r\n";
          char you_kill[100];
          sprintf(you_kill, "%s is died\r\n", clients[i].login);
          char smn_kill[] = "Somebody is dead. R.I.P.\r\n";
          for (int j = 0; j < max_clients; j++) {
            if (clients[j].kill_sd == clients[i].sd) {
              send(clients[j].sd, you_kill, strlen(you_kill), 0);
            } else if (clients[j].sd != 0 && i != j) {
              send(clients[j].sd, smn_kill, strlen(smn_kill), 0);
            }
          }

          send(clients[i].sd, you_died, strlen(you_died), 0);
          shutdown(clients[i].sd, SHUT_RDWR);
          close(clients[i].sd);
          clients[i].sd = 0;
          clients[i].hp = 100;
          clients[i].login[0] = '1';
          clients[i].login[1] = '0';
        }
      }
    }

    for (int i = 0; i < max_clients; i++) {
      clients[i].kill_sd = 0;
      clients[i].heal_sd = 0;
    }
  }
}

void catch() {
  for (int i = 0; i < max_clients; i++) {
    if (clients[i].sd != 0) {
      shutdown(clients[i].sd, SHUT_RDWR);
      close(clients[i].sd);
    }
  }
  shutdown(d, SHUT_RDWR);
  close(d);
  exit(1);
}

int main(int argc , char *argv[])
{
  pthread_t thread;
  pthread_create(&thread, NULL, p, NULL);

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

  fd_set rfds;

  for (int i = 0; i < max_clients; i++)
  {
    clients[i].sd = 0;
    clients[i].round_hp = 0;
    clients[i].kill_sd = 0;
    clients[i].login[0] = '1';
    clients[i].login[1] = '\0';
    clients[i].hp = 100;
  }

  struct sockaddr_in servaddr;
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(atoi(argv[1]));

  if(bind(d, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0){
    perror("bind");
    exit(1);
  }
  if(listen(d, 10) < 0){
    perror("listen");
    exit(1);
  }

  char login_message[] = "login:\r\n";
  char wrong_login[] = "Try another login\r\n";
  char error_message[] = "Invalid command\r\n";

  int max_sd;
  while(1)
  {
    FD_ZERO(&rfds);
    FD_SET(d, &rfds);
    max_sd = d;

    for (int i = 0; i < max_clients; i++)
    {
      int sd = clients[i].sd;
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

      send(new_socket, login_message, strlen(login_message), 0);

      for (int i = 0; i < max_clients; i++)
      {
        if(clients[i].sd == 0)
        {
          clients[i].sd = new_socket;
          break;
        }
      }
    }

    for (int i = 0; i < max_clients; i++)
    {
      int sd = clients[i].sd;
      if (FD_ISSET(sd, &rfds))
      {
        char buffer[1025];
        int buf_size = recv(sd, buffer, 1024, 0);

        if (buf_size - 2 >= 0 && buffer[buf_size - 2] == '\r') {
          buffer[buf_size - 2] = '\0';
        } else {
          buffer[buf_size - 1] = '\0';
        }

        if (clients[i].login[0] == '1') {
          int flag = 1;
          for (int j = 0; j < max_clients; j++) {
            if (!strcmp(clients[j].login, buffer)) {
              char log_mess[] = "login:\r\n";
              send(sd, wrong_login, strlen(wrong_login), 0);
              send(sd, log_mess, strlen(log_mess), 0);
              flag = 0;
              break;
            }
          }
          if (flag == 1) {
            char play[] = "Let's play!\r\n";
            strcpy(clients[i].login, buffer);
            send(sd, play, strlen(play), 0);
          }
        } else if (!strncmp(buffer, "who", 3)) {
          for (int j = 0; j < max_clients; j++) {
            if (clients[j].login[0] != '1') {
              char player[20];
              sprintf(player, "%s %d\r\n", clients[j].login, clients[j].hp);
              send(sd, player, strlen(player), 0);
            }
          }
        } else if (!strncmp(buffer, "wall", 4)) {
          char shout_message[buf_size + 30];
          if (strlen(buffer) < 6) {
            send(clients[i].sd, error_message, strlen(error_message), 0);
          } else {
            sprintf(shout_message, "%s shouts: %s\r\n", clients[i].login, &buffer[5]);
            for (int j = 0; j < max_clients; j++) {
              if (clients[j].login[0] != '1' && clients[j].sd != sd) {
                send(clients[j].sd, shout_message, strlen(shout_message), 0);
              }
            }
          }
        } else if (!strncmp(buffer, "say", 3)) {
          char name[11];
          char say_message[1025];
          int temp = 4;
          if (strlen(buffer) < 5) {
            send(clients[i].sd, error_message, strlen(error_message), 0);
          } else {
            while (buffer[temp] != ' ') {
              name[temp - 4] = buffer[temp];
              temp++;
            }
            temp++;
            int start = temp;
            if (strlen(buffer) < start + 1) {
              send(clients[i].sd, error_message, strlen(error_message), 0);
            } else {
              while (buffer[temp] != '\0') {
                say_message[temp - start] = buffer[temp];
                temp++;
              }
              say_message[temp - start] = '\0';
              char all_say_message[1100];
              sprintf(all_say_message, "%s says: %s\r\n", clients[i].login, say_message);

              int flag = 1;
              for (int j = 0; j < max_clients; j++) {
                if (!strcmp(clients[j].login, name)) {
                  send(clients[j].sd, all_say_message, strlen(all_say_message), 0);
                  flag = 0;
                  break;
                }
              }

              if (flag == 1) {
                char bad_message[] = "Error: No such person\r\n";
                send(sd, bad_message, strlen(bad_message), 0);
              }
            }
          }
        } else if (!strncmp(buffer, "kill", 4)) {
          if (clients[i].kill_sd != 0) {
            char er_mess[] = "You should attack once\r\n";
            send(clients[i].sd, er_mess, strlen(er_mess), 0);
          } else {
            char name[11];
            if (strlen(buffer) < 6) {
              send(clients[i].sd, error_message, strlen(error_message), 0);
            } else {
              sprintf(name, "%s", &buffer[5]);
              char kill_message[50];
              sprintf(kill_message, "%s attacks you\r\n", clients[i].login);
              int flag = 1;

              for (int j = 0; j < max_clients; j++) {
                if (!strcmp(clients[j].login, name)) {
                  send(clients[j].sd, kill_message, strlen(kill_message), 0);
                  clients[j].round_hp -= rand() % 10 + 1;
                  clients[i].kill_sd = clients[j].sd;
                  flag = 0;
                  break;
                }
              }

              if (flag == 1) {
                char bad_message[] = "Error: No such person\r\n";
                send(sd, bad_message, strlen(bad_message), 0);
              }
            }
          }
        } else if (!strncmp(buffer, "heal", 4)) {
          if (clients[i].heal_sd != 0) {
            char er_mess[] = "You should heal once\r\n";
            send(clients[i].sd, er_mess, strlen(er_mess), 0);
          } else {
            char name[11];
            if (strlen(buffer) < 6) {
              send(clients[i].sd, error_message, strlen(error_message), 0);
            } else {
              sprintf(name, "%s", &buffer[5]);
              char heal_message[50];
              sprintf(heal_message, "%s heals you\r\n", clients[i].login);
              int flag = 1;

              for (int j = 0; j < max_clients; j++) {
                if (!strcmp(clients[j].login, name)) {
                  send(clients[j].sd, heal_message, strlen(heal_message), 0);
                  clients[j].round_hp += rand() % 10 + 1;
                  clients[i].heal_sd = clients[j].sd;
                  flag = 0;
                  break;
                }
              }

              if (flag == 1) {
                char bad_message[] = "Error: No such person\r\n";
                send(sd, bad_message, strlen(bad_message), 0);
              }
            }
          }
        } else {
          send(clients[i].sd, error_message, strlen(error_message), 0);
        }
      }
    }
  }
  return 0;
}
