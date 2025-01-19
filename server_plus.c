/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>


void dostuff(int sockfd)
{
  char buffer[256];//The server reads characters from the socket connection into this buffer  
  int n;//the return value for the read() and write() calls
  printf("Client connected. Socket FD: %d\n",sockfd);
  while (1)
  {
     bzero(buffer,256);
     n = read(sockfd,buffer,255);
     if (n < 0) 
     {
        error("ERROR reading from socket");
        break;
     }
     printf("Here is the message: %s\n",buffer);
     n = write(sockfd,"I got your message",18);
     if (n < 0) 
     {
        error("ERROR writing to socket");
        break;
     }
  }
  close(sockfd);//close the client socket
  exit(0);
}

void *SigCatcher(int n)
{
    int status;
    //reap all terminated child process
    while (waitpid(-1,&status,WNOHANG)>0)
    {
        printf("Child process reaped.\n");
        exit(1);
    }   
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

/*
argc:the number of command-line arguments
argv:the actual arguments as strings
*/
int main(int argc,char *argv[])
{
	/*
	sockfd:values return by the socket system call
	newsockfd:values return by the accept system call
	portno:port number on which the server accepts connection
	clilen:the size of the address of the client
	*/
     int sockfd, newsockfd, portno, clilen;
//     char buffer[256];//The server reads characters from the socket connection into this buffer
     struct sockaddr_in serv_addr, cli_addr;
//     int n;//the return value for the read() and write() calls
	 //if it fails to pass the port number on which the server will accept connections as an argument
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0); //creat a new socket, if fails, return -1
     if (sockfd < 0) 
        error("ERROR opening socket");
     printf("Server socket is created\n");
     bzero((char *) &serv_addr, sizeof(serv_addr)); //sets all values in a buffer to zero
     portno = atoi(argv[1]);//pass the port number 
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
	 //binds a socket to an address
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     printf("Bind successfully!\n");
     listen(sockfd,5);//listen system call
     printf("Server is listening on port %d ...\n",portno);
	 //accept system call
     clilen = sizeof(cli_addr);
     while (1)
     {
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
          error("ERROR on accept");
        pid_t pid;
        //setup the signal handler for SIGCHILD
        signal(SIGCHLD,SigCatcher);
        pid=fork(); //fork a new process to handle new connection
        if(pid<0)
          error("ERROR on fork");
        else if(pid==0) //child process
          {
            close(sockfd);
            dostuff(newsockfd);
          }
        else //parent process
          close(newsockfd);
     }
     return 0; 
}
