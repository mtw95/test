#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
  
    if (argc != 3)
    {
      perror("Incorrect Usage");
      exit(EXIT_FAILURE);
    }
  
    int sockfd;
    struct sockaddr_in address;

    char *server_ip = argv[1];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //Standard to zero out address before setting:
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    //Here we will use inet_aton() since inet_addr() is deprecated:
    inet_aton(server_ip,&address.sin_addr);
    address.sin_port = htons(3060);

    if (connect(sockfd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("oops: client3");
        exit(EXIT_FAILURE);
    }
    char initial_write[13] = "<cs306chat>\n\0";
    char initial_read[100];
    char check_ok[6] = "<ok>\n\0";
    char username_input[12] = "\0\0\0\0\0\0\0\0\0\0\0\0";
    //char username_send[14];
    char input[102];
    char *uname_ptr;
    char buffer[102];
    
    sprintf(username_input, "%s", argv[2]);
    
    
    
    //Initial Protocol Start
    if (!(read(sockfd, initial_read, 100) > 0))
    {
      printf("Connection Closed\n");
      close(sockfd);
      exit(EXIT_FAILURE);
    }
    
    if (strcmp(initial_write, initial_read))
    {
      printf("Improper Protocol\n");
      close(sockfd);
      exit(EXIT_FAILURE);
    }
    else
    {
      write(sockfd, initial_write, 13);
    }
    
    if (!(read(sockfd, initial_read, 100) > 0))
    {
      printf("Connection Closed\n");
      close(sockfd);
      exit(EXIT_FAILURE);
    }
    
    if (strcmp(check_ok, initial_read))
    {
      printf("Improper Protocol\n");
      close(sockfd);
      exit(EXIT_FAILURE);
    }
    else
    {
      //printf("Enter a username: ");
      //fflush(stdout);
      //fgets(username_input, 10, stdin);
      sprintf(input, "<%s", username_input);
      uname_ptr = strchr(input, '\0');
      memcpy(uname_ptr, ">\n\0", 3);
      //write(sockfd, username_send, 14);
    }
    
    //Initial Protocol End

    pid_t pid;
    switch(pid = fork())
    {
      case -1:
      {
	perror("fork failed\n");
	exit(EXIT_FAILURE);
      }
      case 0:
      {
	while (read(sockfd, buffer, 114) > 0)
	{
	  printf("%s", buffer);
	}
	exit(EXIT_SUCCESS);
      }
      default:
      {
	while (write(sockfd, input, 102) != -1)
	{
	  if (fgets(input,101,stdin) == NULL)
	  {
	    perror("Connection closed by client by ending stdin\n");
	    break;
	  }
	  if (strcmp(input, "bye\n\0") == 0)
	  {
	    printf("Connection Closed by Client Writing 'bye'\n");
	    break;
	  }

	}
      }
    }
    close(sockfd);
    
    int status;
    wait(&status);
    
    //GET CHILD EXIT STATUS ********

    exit(EXIT_SUCCESS);
}