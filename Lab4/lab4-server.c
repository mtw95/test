#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>

#define MAX_CLIENTS 5

int sock_fds[MAX_CLIENTS];
char unames[MAX_CLIENTS][11];

void *handle_client(void *client_fd_ptr);
void clear_fd(int fd);
int check_name_unique(char *username);

int main()
{
    int server_sockfd, client_sockfd;
    struct sockaddr_in server_address;
    int i, num_clients;

    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(3060);
    bind(server_sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
    
    for (i = 0; i < MAX_CLIENTS; i++)
    {
      sock_fds[i] = -1;
      memcpy(unames[i], "\0\0\0\0\0\0\0\0\0\0\0", 11);
    }

    listen(server_sockfd, 5);

    while(1) {
      //printf("server waiting for a new client...\n");
      
      num_clients = 0;
      for (i = 0; i < MAX_CLIENTS; i++)
      {
	if (sock_fds[i] != -1)
	  num_clients++;
      }
      
      if (num_clients < MAX_CLIENTS)
	client_sockfd = accept(server_sockfd,NULL,NULL);
      else
	continue;
      
      for (i = 0; i < MAX_CLIENTS; i++)
      {
	if (sock_fds[i] == -1)
	{
	  sock_fds[i] = client_sockfd;
	  break;
	}
      }
      
      pthread_t tid;
      pthread_create(&tid,NULL,handle_client,&client_sockfd);
    }
}


void *handle_client(void *client_fd_ptr)
{
  //Recover client_fd from pointer thread parameter:
  int client_fd = *(int*)client_fd_ptr;
  
  char initial_write[13] = "<cs306chat>\n\0";
  char initial_read[100];
  char buffer[102];
  char *uname_ptr;
  char username[11];
  
  int i, index = -1;
  
  for (i = 0; i < MAX_CLIENTS; i++)
  {
    if (sock_fds[i] == client_fd)
    {
      index = i;
    }
  }
  
  write(client_fd, initial_write, 13);
  
  if (!(read(client_fd, initial_read, 100) > 0))
  {
    //printf("Client Closed\n");
    close(client_fd);
    clear_fd(client_fd);
    return NULL;
  }
  
  if (strcmp(initial_write, initial_read))
  {
    //printf("Improper Protocol\n");
    close(client_fd);
    clear_fd(client_fd);
    return NULL;
  }
  else
  {
    char *ok = "<ok>\n\0";
    write(client_fd, ok, 6);
  }
  
  if (!(read(client_fd, buffer, 100) > 0))
  {
    //printf("Client Closed\n");
    close(client_fd);
    clear_fd(client_fd);
    return NULL;
  }
  
  if (buffer[0] == '<')
  {
    uname_ptr = strstr(buffer, ">\n");
    if (uname_ptr != NULL)
    {
      *uname_ptr = '\0';
      strncpy(username, buffer+1, 10);
      username[10] = '\0';
      //printf("New User: %s\n", username);
    }
    else
    {
      //printf("Improper Username Format\n");
      close(client_fd);
      clear_fd(client_fd);
      return NULL;
    }
    if(check_name_unique(username))
    {
      strcpy(unames[index], username);
    }
    else
    {
      //printf("Username In Use\n");
      write(client_fd, "Error: username already in use\n\0", 32);
      close(client_fd);
      clear_fd(client_fd);
      return NULL;
    }
  }
  else
  {
    //printf("Improper Username Format\n");
    close(client_fd);
    clear_fd(client_fd);
    return NULL;
  }
    
  char message[114];
  char *find_null;
  int init = 0;
  int j;
  
  int num_clients = 0;
  for (i = 0; i < MAX_CLIENTS; i++)
  {
    if (sock_fds[i] != -1)
      num_clients++;
  }
  char welcome[102];
  char new_user[102];
  
  sprintf(welcome, "There are %d other users...\n", num_clients-1);
  sprintf(new_user, "%s", username);
  find_null = strchr(new_user, '\0');
  sprintf(find_null, " has joined...\n");
  
  for (i = 0; i < MAX_CLIENTS; i++)
  {
    if((sock_fds[i] != 0) && (client_fd != sock_fds[i]))
    {
      write(sock_fds[i], new_user, 102);
    }
  }
  write(client_fd, welcome, 102);

  while (read(client_fd, buffer, 102) > 0)
  {
    if (init == 0)
    {
      init = 1;
      continue;
    }
    sprintf(message, "%s", username);

    find_null = strchr(message, '\0');

    sprintf(find_null, ": %s", buffer);
    for (j = 0; j < MAX_CLIENTS; j++)
    {
      if ((sock_fds[j] != -1) && (sock_fds[j] != client_fd))
      {
	write(sock_fds[j], message, 114);
      }
    }
  }


  close(client_fd);
  clear_fd(client_fd);

  pthread_detach(pthread_self());
  return NULL;
}

void clear_fd(int fd)
{
  int i;
  for (i = 0; i < MAX_CLIENTS; i++)
  {
    if (sock_fds[i] == fd)
    {
      sock_fds[i] = -1;
      unames[i][0] = '\0';
    }
  }
}

int check_name_unique(char *username)
{
  int i;
  for (i = 0; i < MAX_CLIENTS; i++)
  {
    if (strcmp(unames[i], username) == 0)
    {
      return 0;
    }
  }
  return 1;
}