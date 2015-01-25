#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>    
#include <netdb.h>
#include "wrapsock.h"

#ifndef PORT
#define PORT 30000
#endif

#define BUF_SIZE 256

int main(int argc, char* argv[])
{ 
    int sockfd;
    char buf[BUF_SIZE];
    struct sockaddr_in server;
    fd_set allset, rset;
    int nready, maxfd, nread, err;
    int server_counter;
    char port[10];    
    struct addrinfo *info, hints;

    sprintf (port, "%d", PORT);
    
    memset (&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if ( argc != 2 ) {    
        fprintf(stderr, "Usage: %s hostname\n", argv[0]);
        exit(1);
    }

    /* fill in peer address */
    if ((err = getaddrinfo(argv[1], port, &hints, &info))) {
        fprintf (stderr, "%s: %s\n", argv[0], gai_strerror(err));
        exit (1);
    }
    server = *(struct sockaddr_in *)(info->ai_addr);
    
    printf("PORT = %d\n", PORT);


    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    Connect(sockfd, (struct sockaddr *)&server, sizeof(server));

    FD_ZERO(&allset);				/* empty the FD set */
    FD_SET(sockfd, &allset);		/* add the socket */
    FD_SET(STDIN_FILENO, &allset);	/* add standard input */

    /*sockfd is guaranteed to be bigger than STDIN_FILENO*/
    maxfd = sockfd;
                                    

    while(1) {
        rset = allset;
        // prompt user to type a number
        fprintf(stderr, "Enter a number: \n");
        
        // use select to multiplex between stdin and the socket.
        // timeout should be NULL
        
        // Add code here
        if((nready = select((maxfd + 1), &rset, NULL, NULL, NULL)) > 0){
            // If ready to read from stdin
            if(FD_ISSET(STDIN_FILENO, &rset)){
                // read a string from stdin
                if(read(STDIN_FILENO, buf, BUF_SIZE) == -1){
                    // If error, print message and continue loop
                    fprintf(stderr,"Error reading from stdin\n");
                    continue;
                }
                // Get an int from the string read
                if(sscanf(buf, "%d", &server_counter) != 1){
                    // If cannot match, print message and continue loop
                    fprintf(stderr,"Error reading from stdin\n");
                    continue;
                }
                // Send the int to the server
                server_counter = htons(server_counter); // convert to net short
                if(write(sockfd, &server_counter, sizeof(int)) == -1){
                    perror("write");
                }
            }

            // If ready to read from socket
            if(FD_ISSET(sockfd, &rset)){
                // read an int from the server
                nread = read(sockfd, &server_counter, sizeof(int));
                // if error, print message and exit loop
                if(nread == -1){
                    fprintf(stderr, "Read Error from server, closing connection\n");
                    break;
                }
                // if EOF reached, print message and exit loop
                else if(nread == 0){
                    fprintf(stderr, "Server Terminated, closing connection\n");
                    break;
                }
                // TODO: case where the num bytes read is not equal sizeof(int)
                // Convert from net to host short
                server_counter = ntohs(server_counter);
                printf("Counter is %d\n", server_counter);
            }
        }
    }

    Close(sockfd); 
    return(0);
}

/* Printf statements to insert into your code:
 printf("Counter is %d\n", server_counter);
 fprintf(stderr, "Server Terminated, closing connection\n");
 fprintf(stderr, "Read Error from server, closing connection\n");
 fprintf(stderr,"Error reading from stdin\n");
*/
