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
#include <fcntl.h>

#include "bs3_asm.h"
#include "bs3_debug.h"

#define BS3_DEBUG_STATE_RUNNING 0
#define BS3_DEBUG_STATE_STOPPED 1
#define BS3_DEBUG_STATE_TOSTOP  2

#define BS3_DEBUG_COMM_STATE_NOSERVICE      0
#define BS3_DEBUG_COMM_STATE_NOCONNECTION   1
#define BS3_DEBUG_COMM_STATE_CONNECTED      2

#define BS3_DEBUG_COMM_MAXSIZE 80
#define BS3_DEBUG_COMM_DEFAULTPORT 35853
#define SA struct sockaddr

struct bs3_debug_data
{
    struct bs3_cpu_data * pbs3;         /* BS3 cpu object reference                         */
    int debug_state;                    /* cpu debug state , running or stopped             */
    int comm_state;                     /* socket communication state                       */
    int debug_stop_at;                  /* address where the CPU must stop                  */
    int debug_step_count;               /* how many step to execute                         */
    int lastPC;                         /* last final PC at last unassemble cmd             */
    int lastDumpAddr;                   /* last dump address for 'd' cmd                    */
    int canQuit;                        /* when CPU HALT status, can we quit the debugger   */
    /* communication tech data */
    int count;                          /* latency counter                                  */
    int port;                           /* listening port                                   */
    int connfd;                         /* connection descriptor                            */
    int sockfd;                         /* listening socket descriptor                      */
    struct sockaddr_in servaddr;        /* listening bind address                           */
    struct sockaddr_in cli;             /* client address                                   */
    /* client command */
    char buff[BS3_DEBUG_COMM_MAXSIZE];  /* input string from client                         */
    int n;                              /* current size of buff                             */
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
    pbs3debug->canQuit = 0;
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
    int i,j;
    char linebuffer[1024];
    if ( pbs3debug             == ((void *)0)                    ||
         pbs3debug->comm_state != BS3_DEBUG_COMM_STATE_CONNECTED || 
         pbs3debug->connfd     == 0                              ||
         msg                   == ((void *)0)                    ||
         msg[0]                == 0) return;


    for (i = 0,j = 0 ; i < strlen(msg); i++,j++)
    {
        switch(msg[i])
        {
            case '\n':
                linebuffer[j++] = '\r';
                linebuffer[j] = '\n';
            default:
                linebuffer[j] = msg[i];                
        }
    }
    linebuffer[j] = 0;

    isok = 0;
    do
    {
        if (write(pbs3debug->connfd, linebuffer, strlen(linebuffer)) < 0) 
        {
            switch (errno)
            {
                case EAGAIN:
#if EAGAIN != EWOULDBLOCK                
                case EWOULDBLOCK:
#endif                
                case EINTR:
                    usleep(100000); /* 1/10 of second wait*/
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
            bs3_debug_comm_send(pbs3debug,"\rrunning >");
            break;
        case BS3_DEBUG_STATE_TOSTOP:
        case BS3_DEBUG_STATE_STOPPED:
            bs3_debug_comm_send(pbs3debug,"\rstopped >");
            break;
        default:
            bs3_debug_comm_send(pbs3debug,"\runknown >"); /* should not occur */
    }
    if (pbs3debug->n > 0) {
        bs3_debug_comm_send(pbs3debug,pbs3debug->buff);
    }
}

/* return -1 : error 
   return 8  : Flag register, reg[0] == 'I', 'N', 'V', 'Z' or 'C'
   return 9  : PC register
   return 10 : SP register
   return 0 .. 7 : register number , reg[0] == 'W' or 'B'
 */
int bs3_debug_util_getregister(const char * source, char * reg)
{
    int i,j;
    int value;
    if (reg == ((void *)0)) return -1;
    if (source == ((void *)0)) return -1;
    i = 0;
    j = 0;
    value = -1;
    while (source[i] == ' ') i++;
    if (source[i] == 0) return -1;
    while (source[i] != ' ' && source[i] != 0)
    {
        switch (j)
        {
            case 0: /* first character PC, SP, Wx, Bx or flag registers */
                switch (source[i])
                {
                    case 'i':
                    case 'n':
                    case 'v':
                    case 'z':
                    case 'c':
                        value = 8;
                    case 'b':
                    case 'w':
                    case 'p':
                    case 's':
                        reg[j++] = source[i++] - 32;
                        break;
                    case 'I':
                    case 'N':
                    case 'V':
                    case 'Z':
                    case 'C':
                        value = 8;
                    case 'B':
                    case 'W':
                    case 'P':
                    case 'S':
                        reg[j++] = source[i++];
                        break;
                    default:
                        return -1;
                }
                break;
            case 1: /* second character valid only for Wx and Bx registers */
                switch (reg[0])
                {
                    case 'P':
                        switch (source[i])
                        {
                            case 'c':
                                value = 9;
                                reg[j++] = source[i++] - 32;
                                break;
                            case 'C':
                                value = 9;
                                reg[j++] = source[i++];
                                break;
                            default:
                                return -1;
                        }
                        break;
                    case 'S':
                        switch (source[i])
                        {
                            case 'p':
                                value = 10;
                                reg[j++] = source[i++] - 32;
                                break;
                            case 'P':
                                value = 10;
                                reg[j++] = source[i++];
                                break;
                            default:
                                return -1;
                        }
                        break;
                    case 'W':
                        switch (source[i])
                        {
                            case '0' ... '3':
                                value = source[i] - '0';
                                reg[j++] = source[i++];
                                break;
                            default:
                                return -1;
                        }
                        break;
                    case 'B':
                        switch (source[i])
                        {
                            case '0' ... '7':
                                value = source[i] - '0';
                                reg[j++] = source[i++];
                                break;
                            default:
                                return -1;
                        }
                        break;
                    default:
                        return -1;
                }
                break;
            default: /* more than 2 characters is not register */
                return -1;
        }
    }
    reg[j] = 0;
    if (source[i] != 0 && source[i] != ' ') return -1;
    return value;
}

int bs3_debug_util_getbyte(const char * address)
{
    int value = 0;
    int i;
    for (i=0; i < 2; i++)
    {
        switch (address[i])
        {
            case '0' ... '9':
                value = (value << 4) + (address[i] - '0');
                break;
            case 'a' ... 'f':
                value = (value << 4) + (address[i] - 'a' + 10);
                break;
            case 'A' ... 'F':
                value = (value << 4) + (address[i] - 'A' + 10);
                break;
            default:
                return -1;
        }
    }
    if (address[i] != 0 && address[i] != ' ') return -1;
    return value;
}

int bs3_debug_util_getbit(const char * address)
{
    int value = 0;
    int i;
    for (i=0; i < 1; i++)
    {
        switch (address[i])
        {
            case '0' ... '1':
                value = (value << 4) + (address[i]-'0');
                break;
            default:
                return -1;
        }
    }
    if (address[i] != 0 && address[i] != ' ') return -1;
    return value;
}


long bs3_debug_util_getaddress(const char * address)
{
    long value = 0;
    int i;
    for (i=0; i < 4; i++)
    {
        switch (address[i])
        {
            case '0' ... '9':
                value = (value << 4) + (address[i]-'0');
                break;
            case 'a' ... 'f':
                value = (value << 4) + (address[i]-'a'+10);
                break;
            case 'A' ... 'F':
                value = (value << 4) + (address[i]-'A'+10);
                break;
            default:
                return -1;
        }
    }
    if (address[i] != 0 && address[i] != ' ') return -1;
    return value;
}

long bs3_debug_util_getinteger(const char * address)
{
    long value = 0;
    int i;
    for (i=0; address[i] >= '0' && address[i] <= '9'; i++)
    {
        switch (address[i])
        {
            case '0' ... '9':
                value = (value * 10) + (address[i]-'0');
                if (value > 65535) value = 65535;
                break;
        }
    }
    if (address[i] != 0 && address[i] != ' ') return -1;
    return value;
}

void bs3_debug_comm_cmd(struct bs3_debug_data * pbs3debug) 
{
    WORD address;
    int count;
    int value;
    char linebuffer[1024];
    WORD pc;
    int i;
    long addr;
    int len;
    struct bs3_asm_line bs3_asm;

    if (pbs3debug == ((void *)0) || pbs3debug->n == 0 || pbs3debug->buff[0] == '\n')
    {
        bs3_debug_comm_prompt(pbs3debug);
        pbs3debug->n = 0;
        return;
    }
    if (pbs3debug->buff[1] != 0 && pbs3debug->buff[1] != ' ') pbs3debug->buff[0] = 0;
    
    len = strlen(pbs3debug->buff);
    switch (pbs3debug->buff[0])
    {
        case 'a' :
            if (len < 8)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect 'a' command\n");
                break;
            }
            addr = bs3_debug_util_getaddress(pbs3debug->buff+2);
            if (addr < 0)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect 'a' command\n");
                break;
            }
            bs3_asm_line_reset();
            if ((i = bs3_asm_pass1_oneline(&bs3_asm, 0, addr, pbs3debug->buff + 6)) != BS3_ASM_PASS1_PARSE_ERR_OK)
            {
                bs3_debug_comm_send(pbs3debug, bs3_asm_message[i]);
                bs3_debug_comm_send(pbs3debug,"\n");
                break;
            }
            for (i = 0; i <  bs3_asm.assemblyLength; i++)
            {
                pbs3debug->pbs3->m[addr + i] = bs3_asm.assembly[i];
            }
            break;
        case 'u':
            if (len >2 && len < 6)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect 'u' command\n");
                break;
            }
            if (len >=6)
            {
                addr = bs3_debug_util_getaddress(pbs3debug->buff+2);
                if (addr < 0)
                {
                    bs3_debug_comm_send(pbs3debug,"Incorrect 'u' command\n");
                    break;
                }
                pbs3debug->lastPC = (WORD)addr;
            }
            else  pbs3debug->lastPC = (pbs3debug->lastPC == 0)?pbs3debug->pbs3->r.PC:pbs3debug->lastPC;
            pc = pbs3debug->lastPC;
            for (i = 0 ; i < 4 ; i++)
            {
                pc = bs3_cpu_disassemble_(pc,
                                        pbs3debug->pbs3->m[pc], 
                                        pbs3debug->pbs3->m[(pc+1) & 0xFFFF], 
                                        pbs3debug->pbs3->m[(pc+2) & 0xFFFF], 
                                        pbs3debug->pbs3->m[(pc+3) & 0xFFFF], 
                                        linebuffer);
                strcat(linebuffer, "\n");
                bs3_debug_comm_send(pbs3debug,linebuffer);
            }
            pbs3debug->lastPC = pc;
            break;
        case 'g':
            if (len >2 && len < 6)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect 'g' command\n");
                break;
            }
            if (len >=6)
            {
                addr = bs3_debug_util_getaddress(pbs3debug->buff+2);
                if (addr < 0)
                {
                    bs3_debug_comm_send(pbs3debug,"Incorrect 'g' command\n");
                    break;
                }
                pbs3debug->debug_stop_at = (WORD)addr;
            }
            else   pbs3debug->debug_stop_at = 0;
            pbs3debug->debug_step_count = 0;
            pbs3debug->debug_state = BS3_DEBUG_STATE_RUNNING;
            break;
        case 't':
            if (len > 2)
            {
                addr = bs3_debug_util_getinteger(pbs3debug->buff+2);
                if (addr < 1)
                {
                    bs3_debug_comm_send(pbs3debug,"Incorrect 't' command\n");
                    break;
                } 
                pbs3debug->debug_step_count = (int)(addr & 0xFFFF);
            } 
            else pbs3debug->debug_step_count = 1;

            pbs3debug->debug_state = BS3_DEBUG_STATE_RUNNING;
            break;
        case 's':
            pbs3debug->debug_step_count = 0;
            pbs3debug->debug_state = BS3_DEBUG_STATE_STOPPED;
            break;
        case 'r':
            if (pbs3debug->debug_state == BS3_DEBUG_STATE_RUNNING)
            {
                bs3_debug_comm_send(pbs3debug,"Can't execute 'r' command while CPU is running\n");
                break;
            }
            if (len > 2)
            {
                i = bs3_debug_util_getregister(pbs3debug->buff+2, linebuffer);
                if (i < 0) /* not a register nor a flag*/
                {
                    bs3_debug_comm_send(pbs3debug,"Incorrect 'r' command\n");
                    break;
                }
                if (i != 8) /* check value existance for register*/
                {
                    if (pbs3debug->buff[4] != ' ')
                    {
                        bs3_debug_comm_send(pbs3debug,"Missing register value\n");
                        break;
                    }
                }
                else /* check value existance for flag */
                {
                    if (pbs3debug->buff[3] != ' ')
                    {
                        bs3_debug_comm_send(pbs3debug,"Missing flag value\n");
                        break;
                    }
                }
                switch (i) /* get and store value */
                {
                    case 0 ... 7:
                    case 9 ... 10:
                        switch (linebuffer[0])
                        {
                            case 'P':
                                /* get address like value (4 hexa digits )*/
                                addr = bs3_debug_util_getaddress(pbs3debug->buff+5);
                                if (addr < 0)
                                {
                                    i = -1;
                                    break;
                                }
                                pbs3debug->pbs3->r.PC =  (WORD)(addr & 0xFFFF);
                                break;
                            case 'S':
                                /* get address like value (4 hexa digits )*/
                                addr = bs3_debug_util_getaddress(pbs3debug->buff+5);
                                if (addr < 0)
                                {
                                    i = -1;
                                    break;
                                }
                                pbs3debug->pbs3->r.SP =  (WORD)(addr & 0xFFFF);
                                break;
                            case 'W':
                                /* get address like value (4 hexa digits )*/
                                addr = bs3_debug_util_getaddress(pbs3debug->buff+5);
                                if (addr < 0)
                                {
                                    i = -1;
                                    break;
                                }
                                pbs3debug->pbs3->r.W[i] =  (WORD)(addr & 0xFFFF);
                                break;
                            case 'B':
                                /* get byte value (2 hexa digits )*/
                                addr = bs3_debug_util_getbyte(pbs3debug->buff+5);
                                if (addr< 0)
                                {
                                    i = -1;
                                    break;
                                } 
                                pbs3debug->pbs3->r.B[i] =  (BYTE)(addr & 0xFF);
                                break;
                        }
                        break;
                    case 8:
                        /* get bit (1 or 0 value)*/
                        i = bs3_debug_util_getbit(pbs3debug->buff+4);
                        if (i >= 0)
                        {
                            switch (linebuffer[0]) /* Flag register, reg[0] == 'I', 'N', 'V', 'Z' or 'C' */
                            {
                                case 'I':
                                    pbs3debug->pbs3->r.I = (BIT)i;
                                    break;
                                case 'N':
                                    pbs3debug->pbs3->r.N = (BIT)i;
                                    break;
                                case 'V':
                                    pbs3debug->pbs3->r.V = (BIT)i;
                                    break;
                                case 'Z':
                                    pbs3debug->pbs3->r.Z = (BIT)i;
                                    break;
                                case 'C':
                                    pbs3debug->pbs3->r.C = (BIT)i;
                                    break;
                            }
                        }
                        break;
                }
                if (i < 0 ) {
                        bs3_debug_comm_send(pbs3debug,"Incorrect register or flag value\n");
                        break;
                }
            }
            bs3_cpu_state(pbs3debug->pbs3, linebuffer);
            bs3_debug_comm_send(pbs3debug,linebuffer);
            break;
        case 'e':
            if (len < 9)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect 'e' command\n");
                break;
            }
            addr = bs3_debug_util_getaddress(pbs3debug->buff+2);
            if (addr < 0)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect address provided in 'e' command\n");
                break;
            }
            i = bs3_debug_util_getbyte(pbs3debug->buff+7);
            if (i < 0)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect byte values provided in 'e' command\n");
                break;
            }
            pbs3debug->pbs3->m[(WORD)(addr & 0xFFFF)] = (BYTE)(i & 0xFF);
            bs3_debug_comm_send(pbs3debug,"Byte value is set to memory\n");
            break;
        case 'E':
            if (len < 11)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect 'E' command\n");
                break;
            }
            addr = bs3_debug_util_getaddress(pbs3debug->buff+2);
            if (addr < 0)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect address provided in 'E' command\n");
                break;
            }
            i = (int)bs3_debug_util_getaddress(pbs3debug->buff+7);
            if (i < 0)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect word values provided in 'E' command\n");
                break;
            }
            pbs3debug->pbs3->m[(WORD)(addr & 0xFFFF)] = (BYTE)(i & 0xFF);
            pbs3debug->pbs3->m[(WORD)((addr + 1) & 0xFFFF)] = (BYTE)((i >> 8) & 0xFF);
            bs3_debug_comm_send(pbs3debug,"Word value is set to memory\n");
            break;
        case 'd':
            if (len >2 && len < 6)
            {
                bs3_debug_comm_send(pbs3debug,"Incorrect 'd' command\n");
                break;
            }
            if (len >=6)
            {
                addr = bs3_debug_util_getaddress(pbs3debug->buff+2);
                if (addr < 0)
                {
                    bs3_debug_comm_send(pbs3debug,"Incorrect 'd' command\n");
                    break;
                }
                pbs3debug->lastDumpAddr = (WORD)addr;
            }
            for (i = 0 ; i < 16; i++)
            {
                bs3_cpu_memory_dump(pbs3debug->pbs3, (WORD)pbs3debug->lastDumpAddr, linebuffer);
                strcat(linebuffer, "\n");
                bs3_debug_comm_send(pbs3debug,linebuffer);
                pbs3debug->lastDumpAddr += 16;
                pbs3debug->lastDumpAddr = pbs3debug->lastDumpAddr & 0xFFFF;
            }
            break;
        case 'z':
            pbs3debug->pbs3->status =  BS3_STATUS_RESET;
            pbs3debug->debug_step_count = 0;
            pbs3debug->lastPC = 0;
            pbs3debug->debug_state = BS3_DEBUG_STATE_TOSTOP;
            break;
        case 'Z':
            pbs3debug->pbs3->status =  BS3_STATUS_HALT;
            pbs3debug->debug_step_count = 1;
            pbs3debug->canQuit = 1;
            pbs3debug->debug_state = BS3_DEBUG_STATE_RUNNING;
            break;
        case 'q':
            close(pbs3debug->connfd);
            pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_NOCONNECTION;
            break;
        case 'h':
            bs3_debug_comm_send(pbs3debug," Debuggger commands\n");
            bs3_debug_comm_send(pbs3debug,"   parameter is optional if showed between square brackets\n");
            bs3_debug_comm_send(pbs3debug,"   parameter 'address' : 4 hexa digits\n");
            bs3_debug_comm_send(pbs3debug,"   parameter 'count'   : decimal (1 to 65535)\n");
            bs3_debug_comm_send(pbs3debug,"   parameter 'value'   : 0 or 1 for flags,\n");
            bs3_debug_comm_send(pbs3debug,"                         2 hexa digits for byte (Bx registers or memory entry)\n");
            bs3_debug_comm_send(pbs3debug,"                         4 hexa digits for word (Wx registers or memory entry)\n\n");
            bs3_debug_comm_send(pbs3debug,"   u [address]         : unassemble code at 'address' or at PC if 'address' not provided\n");
            bs3_debug_comm_send(pbs3debug,"   g [address]         : unpause CPU until 'address' is reached\n");
            bs3_debug_comm_send(pbs3debug,"                         or continue until pause ('s' command or 'HLT' CPU instruction)\n");
            bs3_debug_comm_send(pbs3debug,"   t [count]           : execute one step 'count' times, \n");
            bs3_debug_comm_send(pbs3debug,"                         or only once if 'count' not provided \n");
            bs3_debug_comm_send(pbs3debug,"   s                   : pause cpu execution when running\n");
            bs3_debug_comm_send(pbs3debug,"   r [register value]  : 'r' then show all register values\n");
            bs3_debug_comm_send(pbs3debug,"                         'r register value' set 'value' to 'register'\n");
            bs3_debug_comm_send(pbs3debug,"                         register amongst PC, SP, W0-3, B0-7, I, V, N, Z, C \n");
            bs3_debug_comm_send(pbs3debug,"   e address value     : set 'value' byte to address\n");
            bs3_debug_comm_send(pbs3debug,"   E address value     : set 'value' word to address (little endian)\n");
            bs3_debug_comm_send(pbs3debug,"   d [address]         : dump data at adress, or continue dump from previous dump address\n");
            bs3_debug_comm_send(pbs3debug,"   a address asmcode   : assemble 'asmcode' at 'address'\n");
            bs3_debug_comm_send(pbs3debug,"   z                   : CPU reset (with data reset)\n");
            bs3_debug_comm_send(pbs3debug,"   Z                   : CPU halt (with debugger deconnection)\n");
            bs3_debug_comm_send(pbs3debug,"   q                   : Disconnect from debugger(quit but CPU still working)\n");
            bs3_debug_comm_send(pbs3debug,"   h                   : this help\n");
            break;
        default:
            bs3_debug_comm_send(pbs3debug,"Unknown command. type 'h' for help\n");
    }
    pbs3debug->n = 0; /* command treated */
    //bs3_debug_comm_prompt(pbs3debug);
}


void bs3_debug_comm(struct bs3_debug_data * pbs3debug)
{
    int len;
    int i;
    int eol;
    struct sockaddr_in cli;
    int newco;
    const char * reject = "Sorry but BS3 debugger is already attached\n";
    char buff[BS3_DEBUG_COMM_MAXSIZE];
    int fdf;  /* file descriptor flag */
    if (pbs3debug == ((void *) 0)) return;
    if (((pbs3debug->count++) & 0x0FF) != 0 && pbs3debug->debug_state == BS3_DEBUG_STATE_RUNNING) return; /* consider socket reading , once every 256 call to this function : latency */
    if (pbs3debug->debug_state == BS3_DEBUG_STATE_STOPPED) usleep(100000);
    switch (pbs3debug->comm_state) 
    {
        case BS3_DEBUG_COMM_STATE_CONNECTED:
            if ((pbs3debug->count & 0xFFF) == 0 || (((pbs3debug->count & 0x0F) == 0) && (pbs3debug->debug_state == BS3_DEBUG_STATE_STOPPED)))
            {
               len = sizeof(pbs3debug->cli); 
               newco = accept(pbs3debug->sockfd, (SA*)&cli, &len); 
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
#if EAGAIN != EWOULDBLOCK
                        case EWOULDBLOCK:
#endif                        
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
                    i = 0;
                    eol = 0;
                    while ((i < len) && 
                           (pbs3debug->n < (BS3_DEBUG_COMM_MAXSIZE-1)) && 
                           buff[i] )
                    {
                        switch (buff[i])
                        {
                            case '\r':
                            case '\n':
                                eol = 1; /* end of line detected*/
                                break;
                            default:
                                if (buff[i]> 31) pbs3debug->buff[pbs3debug->n++] = buff[i];   
                                break;
                        }
                        i++;
                    }
                    if (pbs3debug->n == (BS3_DEBUG_COMM_MAXSIZE-1)) eol = 1;
                    pbs3debug->buff[pbs3debug->n] = 0;
                    if (eol) /* if end of line detected , then submit the command */
                    {
                        bs3_debug_comm_cmd(pbs3debug);
                        bs3_debug_comm_prompt(pbs3debug);
                    }
            }
            break;

        case BS3_DEBUG_COMM_STATE_NOCONNECTION:
            /* non blocking accept connection */
            /* Accept the data packet from client and verification */
            len = sizeof(pbs3debug->cli);
            pbs3debug->connfd = accept(pbs3debug->sockfd, (SA*)&pbs3debug->cli, &len);
            if (pbs3debug->connfd < 0) 
            {
                switch(errno)
                {
                    case EAGAIN:
#if EAGAIN != EWOULDBLOCK                    
                    case EWOULDBLOCK:
#endif                    
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
            fdf = fcntl(pbs3debug->connfd, F_GETFL, 0); /* get current flag of socket file descriptor */
            fcntl(pbs3debug->connfd, F_SETFL, fdf | O_NONBLOCK); /* add non blocking flag to socket file descriptor */
            pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_CONNECTED;
            bs3_debug_comm_welcome(pbs3debug);
            for (i = 1 ; i <= 10;i++)
            {
                 bs3_debug_comm(pbs3debug);
                 usleep(100000);
            }
            pbs3debug->n = 0;
            bs3_debug_comm_prompt(pbs3debug); 
            pbs3debug->lastPC = 0;
            break;

        case BS3_DEBUG_COMM_STATE_NOSERVICE:
            /* prepare the listening service */
            /* socket create and verification */
            pbs3debug->sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (pbs3debug->sockfd == -1) {
                printf("Debug socket creation failed...errno:%d\n",errno);
                exit(0);
            }
            bzero(&pbs3debug->servaddr, sizeof(pbs3debug->servaddr));
        
            /* assign IP, PORT */
            pbs3debug->servaddr.sin_family = AF_INET;
             /* pbs3debug->servaddr.sin_addr.s_addr = htonl(INADDR_ANY); */
            pbs3debug->servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
            pbs3debug->servaddr.sin_port = htons(pbs3debug->port);
            int yes = 1;
            if (setsockopt(pbs3debug->sockfd, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes)) < 0) 
            {
                printf("setsockopt() failed. Error: %d\n", errno);
                exit(0);
            }
            /* Binding newly created socket to given IP and verification */
            if ((bind(pbs3debug->sockfd, (SA*)&pbs3debug->servaddr, sizeof(pbs3debug->servaddr))) != 0) {
                printf("Debug socket bind failed... errno:%d\n",errno);
                exit(0);
            }

            /* Now server is ready to listen and verification */
            if ((listen(pbs3debug->sockfd, 5)) != 0) {
                printf("Debug socket listen failed...errno:%d\n",errno);
                exit(0);
            }

            /* non blocking socket accept */
            
            fdf = fcntl(pbs3debug->sockfd ,F_GETFL, 0); /* get current flag of socket file descriptor */
            fcntl(pbs3debug->sockfd, F_SETFL, fdf | O_NONBLOCK); /* add non blocking flag to socket file descriptor */
            pbs3debug->comm_state = BS3_DEBUG_COMM_STATE_NOCONNECTION;
            break;
    }
}

void bs3_debug(struct bs3_cpu_data * pbs3)
{
    if (pbs3 == ((void *)0))
    {
        bs3_debug_finish(&bs3debug);
        return; /* no debug to do, but possibly close debug connection*/
    }
    bs3debug.pbs3 = pbs3;
    if (pbs3->status == BS3_STATUS_HALT)
    {
        pbs3->status = (bs3debug.canQuit)?pbs3->status:BS3_STATUS_DEFAULT;
        bs3debug.debug_state = BS3_DEBUG_STATE_TOSTOP;
        bs3debug.lastPC = 0;
        return;
    }
    if (bs3debug.debug_state == BS3_DEBUG_STATE_TOSTOP) 
    {
        bs3debug.debug_state = BS3_DEBUG_STATE_RUNNING;
        bs3debug.debug_step_count = 1;
    }
    do 
    {
      
      if (bs3debug.debug_state == BS3_DEBUG_STATE_RUNNING) 
      {
        /* should we have to stop */
        if (((WORD)bs3debug.debug_stop_at) == pbs3->r.PC ) {
            bs3debug.debug_state = BS3_DEBUG_STATE_STOPPED;
            bs3debug.lastPC = bs3debug.pbs3->r.PC;
            bs3_debug_comm_send(&bs3debug,"\n");
            strcpy(bs3debug.buff, "r");
            bs3debug.n = 1;
            bs3_debug_comm_cmd(&bs3debug);
            strcpy(bs3debug.buff, "u");
            bs3debug.n = 1;
            bs3_debug_comm_cmd(&bs3debug);
            bs3_debug_comm_prompt(&bs3debug);
            bs3debug.debug_step_count = 0;
        } 
        else
        {
            if (bs3debug.debug_step_count > 0) {
                bs3debug.debug_step_count--;
                if (bs3debug.debug_step_count == 0) 
                {
                    bs3debug.debug_state = BS3_DEBUG_STATE_STOPPED;
                    bs3debug.lastPC = bs3debug.pbs3->r.PC;
                    bs3_debug_comm_send(&bs3debug,"\n");
                    strcpy(bs3debug.buff, "r");
                    bs3debug.n = 1;
                    bs3_debug_comm_cmd(&bs3debug);
                    strcpy(bs3debug.buff, "u");
                    bs3debug.n = 1;
                    bs3_debug_comm_cmd(&bs3debug);
                    bs3_debug_comm_prompt(&bs3debug);
                     /*bs3debug.n = 0; */
                }
            }
        }
      }
      bs3_debug_comm(&bs3debug);
    } while(bs3debug.debug_state == BS3_DEBUG_STATE_STOPPED);

}
