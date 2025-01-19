#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <string.h>

#define Server_Hostname "jiahewu-VMware-Virtual-Platform"

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc,char *argv[])
{
    int sockfd, portno, n;

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];
    if (argc < 2) {
      fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    portno = atoi(argv[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);//creat a new socket
    if (sockfd < 0) 
        error("ERROR opening socket");
    printf("Client socket is created\n");
    server = gethostbyname(Server_Hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
	//connect system call
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) 
      error("ERROR connecting");
    printf("Connected to the Server at %s:%d\n",Server_Hostname,portno);
    while (1)
    {
      printf("Please enter the message(type 'exit' to quit): ");
      bzero(buffer,256);
      fgets(buffer,255,stdin);
      buffer[strcspn(buffer,"\n")]='\0';//remove newline character made by fgets()
      //quit if enter 'exit'
      if(strcmp(buffer,"exit")==0)
      {
        printf("Exiting the client program.\n");
        break;
      }
      n = write(sockfd,buffer,strlen(buffer));
      if (n < 0) 
         error("ERROR writing to socket");
      bzero(buffer,256);
      n = read(sockfd,buffer,255);
      if (n < 0) 
         error("ERROR reading from socket");
      printf("%s\n",buffer);
    } 
    return 0;
}