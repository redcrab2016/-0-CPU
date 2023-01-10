/* debug of BS3 CPU */
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()

#include "bs3_asm.h"
#include "bs3_debug.h"



#define MAX 80
#define PORT 8080
#define SA struct sockaddr

struct bs3_debug_data
{
    int debug_state; /* cpu debug state , running or stopped */
    int comm_state; /* socket communication state */
    int debug_stop_at; /* address where the CPU must stop */

    int connfd; /* connection descriptor */
    int sockfd; /* listening socket descriptor */
    struct sockaddr_in servaddr; /* listening bind address */
    struct sockaddr_in cli;      /* client address */
    char buff[MAX]; /* input string from client */
    int n; /* current size of buff */
};

/*
int fdf; file descriptor flag
fdf = fcntl(socketfd ,F_GETFL, 0); // get current flag of socket file descriptor 
fcntl(socketfd, F_SETFL, fdf | O_NONBLOCK); // add non blocking flag to socket file descriptor
*/
/*
     client command : (address always 4 hexa digits, count is decimal )
        u [address] : unassemble code at 'address' or at PC if 'address' not provided
        g [address] : unpause CPU until 'address' is reached, or continue as long as no pause is requested  ('s' command) if address not provided
        t [count] : execute step 'count' times, or only once if 'count' not provided (with a 'r' trace after each step)
        s : pause cpu execution when running
        r [ register [value]] : if 'r' then show all register value, 'r register' show 'register' value, 'r register value' set 'value' to 'register'
           register : PC, SP, W0-3, B0-3, I, V, N, Z, C 
        e address value : set 'value' byte to address
        E address value : set 'value' word to address (little endian)
        d [address] : dump data at adress, or continue dump from previous dump address
        z : CPU reset (with data reset)
        Z : CPU halt (with debugger deconnection)
        q : quit debugger (deconnection but CPU program and state continue )

*/
void bs3_debug(struct bs3_cpu_data * pbs3)
{
    if (pbs3 == ((void *)0))
    {
        return; /* no debug to do, but possibly close debug connection*/
    }
}


   
// Function designed for chat between client and server.
void func(int connfd)
{
    char buff[MAX];
    int n;
    // infinite loop for chat
    for (;;) {
        bzero(buff, MAX);
   
        // read the message from client and copy it in buffer
        read(connfd, buff, sizeof(buff));
        // print buffer which contains the client contents
        printf("From client: %s\t To client : ", buff);
        bzero(buff, MAX);
        n = 0;
        // copy server message in the buffer
        while ((buff[n++] = getchar()) != '\n')
            ;
   
        // and send that buffer to client
        write(connfd, buff, sizeof(buff));
   
        // if msg contains "Exit" then server exit and chat ended.
        if (strncmp("exit", buff, 4) == 0) {
            printf("Server Exit...\n");
            break;
        }
    }
}
   
// Driver function
int fakemain()
{
    int sockfd, connfd, len;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);
   
    // Binding newly created socket to given IP and verification
    if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
        printf("socket bind failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully binded..\n");
   
    // Now server is ready to listen and verification
    if ((listen(sockfd, 5)) != 0) {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
    len = sizeof(cli);
   
    // Accept the data packet from client and verification
    connfd = accept(sockfd, (SA*)&cli, &len);
    if (connfd < 0) {
        printf("server accept failed...\n");
        exit(0);
    }
    else
        printf("server accept the client...\n");
   
    // Function for chatting between client and server
    func(connfd);
   
    // After chatting close the socket
    close(sockfd);
}