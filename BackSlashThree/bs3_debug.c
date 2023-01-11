/* debug of BS3 CPU through telnet socket */
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // read(), write(), close()
#include <errno.h>

#include "bs3_asm.h"
#include "bs3_debug.h"

#define BS3_DEBUG_STATE_RUNNING 0
#define BS3_DEBUG_STATE_STOPPED 1

#define BS3_DEBUG_COMM_STATE_NOSERVICE      0
#define BS3_DEBUG_COMM_STATE_NOCONNECTION   1
#define BS3_DEBUG_COMM_STATE_CONNECTED      2

#define BS3_DEBUG_COMM_MAXSIZE 80
#define BS3_DEBUG_COMM_DEFAULTPORT 35853
#define SA struct sockaddr

struct bs3_debug_data
{
    int debug_state;                    /* cpu debug state , running or stopped */
    int comm_state;                     /* socket communication state           */
    int debug_stop_at;                  /* address where the CPU must stop      */
    int debug_step_count;               /* how may step                         */
    /* communication tech data */
    int count;                          /* latency counter                      */
    int port;                           /* listening port                       */
    int connfd;                         /* connection descriptor                */
    int sockfd;                         /* listening socket descriptor          */
    struct sockaddr_in servaddr;        /* listening bind address               */
    struct sockaddr_in cli;             /* client address                       */
    /* client command */
    char buff[BS3_DEBUG_COMM_MAXSIZE];  /* input string from client             */
    int n;                              /* current size of buff                 */
};

struct bs3_debug_data bs3debug;

void bs3_debug_finish(struct bs3_debug_data * pbs3debug)
{
    switch (pbs3debug->comm_state) /* close connection and listening socket */
    {
        case BS3_DEBUG_COMM_STATE_CONNECTED:
            if (pbs3debug->connfd)
            {
                close(pbs3debug->connfd);
                pbs3debug->connfd = 0;
            }
            pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_NOCONNECTION;
        case BS3_DEBUG_COMM_STATE_NOCONNECTION:
            if (pbs3debug->sockfd)
            {
                close(pbs3debug->sockfd);
                pbs3debug->sockfd = 0;
            }
            pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_NOSERVICE;
        case BS3_DEBUG_COMM_STATE_NOSERVICE:
            break;
    }
    pbs3debug->debug_state = BS3_DEBUG_STATE_RUNNING;
    pbs3debug->n = 0;
}

void bs3_debug_init(struct bs3_debug_data * pbs3debug, int port)
{
    bs3_debug_finish(pbs3debug);
    memset(pbs3debug, 0, sizeof(struct bs3_debug_data));
    pbs3debug->port = (port == 0)?BS3_DEBUG_COMM_DEFAULTPORT:port;/* if port is 0 then take default port */
    pbs3debug->debug_state = BS3_DEBUG_STATE_STOPPED;
    pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_NOSERVICE;
}

void bs3_debug_prepare(int port)
{
    bs3_debug_init(&bs3debug, port);
}

void bs3_debug_end()
{
     bs3_debug_finish(&bs3debug);
}


void bs3_debug_comm_send(struct bs3_debug_data * pbs3debug, const char * msg)
{
    int isok;
    isok = 0;
    if ( pbs3debug             == 0                              ||
         pbs3debug->comm_state != BS3_DEBUG_COMM_STATE_CONNECTED || 
         pbs3debug->connfd     == 0                              ||
         msg                   == ((void *)0)                    ||
         msg[0]                == 0) return;
    do
    

    {
        if (write(pbs3debug->connfd, msg, strlen(msg)) < 0)
        {
            switch (errno)
            {
                case EAGAIN:
                case EWOULDBLOCK:
                case EINTR:
                    usleep(100000);
                    break;
                default:
                    isok = 1;
                    close(pbs3debug->connfd);
                    pbs3debug->connfd = 0;
                    pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_NOCONNECTION;
            }
        } 
        else 
        {
            isok = 1;
        }
    } while (!isok);

}

void bs3_debug_comm_welcome(struct bs3_debug_data * pbs3debug)
{
    bs3_debug_comm_send(pbs3debug,"BS3 debugger\nYou may type 'h' for help\n");
}

void bs3_debug_comm_prompt(struct bs3_debug_data * pbs3debug)
{
    if (pbs3debug == ((void *)0)) return;
    if (pbs3debug->comm_state != BS3_DEBUG_COMM_STATE_CONNECTED) return;
    switch (pbs3debug->debug_state)
    {
        case BS3_DEBUG_STATE_RUNNING:
            bs3_debug_comm_send(pbs3debug,"running >");
            break;
        case BS3_DEBUG_STATE_STOPPED:
            bs3_debug_comm_send(pbs3debug,"stopped >");
            break;
        default:
            bs3_debug_comm_send(pbs3debug,"unknown >");
    }
}


void bs3_debug_comm_cmd(struct bs3_debug_data * pbs3debug) 
{
    if (pbs3debug == ((void *)0)) return;
    pbs3debug->n = 0; /* command treated */
    


}


void bs3_debug_comm(struct bs3_debug_data * pbs3debug)
{
    int len;
    int i;
    int newco;
    const char * reject = "Sorry but BS3 debugger is already attached\n";
    char buff[BS3_DEBUG_COMM_MAXSIZE];
    int fdf;  /* file descriptor flag */
    if (pbs3debug == ((void *) 0)) return;
    if (((pbs3debug->count++) & 0x0FF) != 0 ) return; /* consider socket reading , once every 256 call to this function : latency */
    switch (pbs3debug->comm_state) 
    {
        case BS3_DEBUG_COMM_STATE_CONNECTED:
            if ((pbs3debug->count & 0xFFF) == 0)
            {
               newco = accept(pbs3debug->sockfd, (SA*)&pbs3debug->cli, &len); 
               if (newco > 0) {
                    write(newco,reject, strlen(reject) );
                    close(newco);
               }
            }
            /* manage communication with client , handle rejection of other connection request, handle communication failure */
            len = read(pbs3debug->connfd, buff, sizeof(buff)-1 );
            switch (len)
            {
                case -1:
                    switch (errno)
                    {
                        case EAGAIN:
                        case EWOULDBLOCK:
                        case EINTR:
                            break;
                        default:
                            close(pbs3debug->connfd);
                            pbs3debug->connfd = 0;
                            pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_NOCONNECTION;
                    }
                    break;
                case 0: /* zero data read returned (End of File) : not sure if the case can occur when reading socket in non blocking mode */
                            close(pbs3debug->connfd);
                            pbs3debug->connfd = 0;
                            pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_NOCONNECTION;
                    break;
                default: /* something is read */
                    i = 0
                    while ((i < len) && 
                           (pbs3debug->n < (BS3_DEBUG_COMM_MAXSIZE-1)) && 
                           buff[i] && 
                           buff[i] != '\n' && buff[i] != '\r')
                    {
                        pbs3debug->buff[pbs3debug->n++] = buff[i++];   
                    }
                    pbs3debug->buff[pbs3debug->n] = 0;
                    buff[i] = 0;
                    if (i>0  && buff[i-1] == '\r') buff[i-1] = '\n';
                    bs3_debug_comm_send(pbs3debug,buff); /* echo to client what is taken */
                    if (i>0 && buff[i-1] == '\n' ) {
                        pbs3debug->buff[pbs3debug->n-1] = 0;
                        pbs3debug->n--;
                    }
                    if (i == 0 || (i>0 && buff[i-1] == '\n'))
                    {
                        bs3_debug_comm_cmd(pbs3debug);
                    }


            }
            break;

        case BS3_DEBUG_COMM_STATE_NOCONNECTION:
            /* non blocking accept connection */
            /* Accept the data packet from client and verification */
            len = sizeof(pbs3debug->cli);
            pbs3debug->connfd = accept(pbs3debug->sockfd, (SA*)&pbs3debug->cli, &len);
            if (pbs3debug->connfd < 0) {
                switch(errno)
                {
                    case EAGAIN:
                    case EWOULDBLOCK:
                        break;
                    default:
                        pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_NOSERVICE;
                        close(pbs3debug->sockfd);
                        pbs3debug->sockfd = 0;
                }
                pbs3debug->connfd = 0;
                break;
            }
            /* set non blocking connection file descriptor */
            fdf = fcntl(pbs3debug->connfd ,F_GETFL, 0); /* get current flag of socket file descriptor */
            fcntl(pbs3debug->connfd, F_SETFL, fdf | O_NONBLOCK); /* add non blocking flag to socket file descriptor */
            bs3_debug_comm_welcome(pbs3debug);
            bs3_debug_comm_prompt(pbs3debug); 
            pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_CONNECTED;
            break;

        case BS3_DEBUG_COMM_STATE_NOSERVICE:
            /* prepare the listening service */
            /* socket create and verification */
            pbs3debug->sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (pbs3debug->sockfd == -1) {
                printf("Debug socket creation failed...\n");
                exit(0);
            }
            bzero(&pbs3debug->servaddr, sizeof(pbs3debug->servaddr));
        
            /* assign IP, PORT */
            pbs3debug->servaddr.sin_family = AF_INET;
             /* pbs3debug->servaddr.sin_addr.s_addr = htonl(INADDR_ANY); */
            pbs3debug->servaddr.sin_addr.s_addr = inet_addr("127.0.0.1")
            pbs3debug->servaddr.sin_port = htons(pbs3debug->port);
        
            /* Binding newly created socket to given IP and verification */
            if ((bind(sockfd, (SA*)&pbs3debug->servaddr, sizeof(pbs3debug->servaddr))) != 0) {
                printf("Debug socket bind failed...\n");
                exit(0);
            }

            /* Now server is ready to listen and verification */
            if ((listen(pbs3debug->sockfd, 5)) != 0) {
                printf("Debug socket listen failed...\n");
                exit(0);
            }

            /* non blocking socket accept */
            
            fdf = fcntl(pbs3debug->sockfd ,F_GETFL, 0); /* get current flag of socket file descriptor */
            fcntl(pbs3debug->sockfd, F_SETFL, fdf | O_NONBLOCK); /* add non blocking flag to socket file descriptor */

            break;
    }
}

/* (NOTE for non blocking filedescriptor)
int fdf; file descriptor flag
fdf = fcntl(socketfd ,F_GETFL, 0); // get current flag of socket file descriptor 
fcntl(socketfd, F_SETFL, fdf | O_NONBLOCK); // add non blocking flag to socket file descriptor
*/
/*
     client command : (address always 4 hexa digits, count is decimal, value is 0/1 or 2/4 hexa digits )
        u [address] : unassemble code at 'address' or at PC if 'address' not provided
        g [address] : unpause CPU until 'address' is reached (PC register value), or continue as long as no pause is requested  ('s' command) if address not provided
        t [count] : execute one step 'count' times, or only once if 'count' not provided (with a 'r' trace after each step)
        s : pause cpu execution when running
        r [ register [value]] : if 'r' then show all register value, 'r register' show 'register' value, 'r register value' set 'value' to 'register'
           register : PC, SP, W0-3, B0-7, I, V, N, Z, C 
        e address value : set 'value' byte to address
        E address value : set 'value' word to address (little endian)
        d [address] : dump data at adress, or continue dump from previous dump address
        z : CPU reset (with data reset)
        Z : CPU halt (with debugger deconnection)
        q : quit debugger (deconnection but CPU program and state continue )
        h : short help
  bs3_debug(...) function is the function to provide as debug provider to bs3_hyper_main(....) function 
*/
void bs3_debug(struct bs3_cpu_data * pbs3)
{
    if (pbs3 == ((void *)0))
    {
        bs3_debug_finish(&bs3debug);
        return; /* no debug to do, but possibly close debug connection*/
    }
    do 
    {
      if (bs3debug.debug_state == BS3_DEBUG_STATE_RUNNING) 
      {
        /* should we have to stop */
      }
      bs3_debug_comm(&bs3debug);
    } while(bs3debug.debug_state == BS3_DEBUG_STATE_STOPPED)

}

 /* LINES BELOW HAVE TO BE DELETED WHEN bs3_debug.c IS OK */
   
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