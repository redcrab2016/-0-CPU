#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>

#include "bs3_asm.h"
static sig_atomic_t end = 0;
static sig_atomic_t timer_alarm = 0;

static void sighandler(int signo)
{
  if (signo == SIGVTALRM) {
    timer_alarm = 1;
  }
  else 
  {
    end = 1;
  }
}

void main() {
  struct bs3_registers reg;
  char line[120];
  WORD start = 0x1FFE;
  int i =0;
  bs3_asm_file("test.asm", "test.out.2",1);
  //bs3_asm_file("test.asm", "test.out",0);
  return;
  for (i = 0; i <= 255; i++) 
  {
    start = bs3_cpu_disassemble(start,(BYTE)i, 0xE0, 0x8F, 0xF8, line);
    printf("%s\n",line);
  }
  struct bs3_cpu_data cpu;
  cpu.r.PC = 0xFFA5;
  cpu.r.SP = 0x0400;
  cpu.r.I = 1;
  cpu.r.N = 0;
  cpu.r.V = 1;
  cpu.r.Z = 0;
  cpu.r.C = 1;
  cpu.r.W[0]=0x0102;
  cpu.r.W[1]=0x33AA;
  cpu.r.W[2]=0x4253;
  cpu.r.W[3]=0xA55A;
  
  bs3_cpu_state(&cpu, line);
  printf("%s",line);
  
  
  reg.PC = 0xFAAF;
  reg.FL = 0x005A;
  reg.SP = 0x0400;
  reg.W[0] = 0xAAFF;
  BYTE x = 0xFF;
  reg.W[0] = reg.W[0] + ((SBYTE)x);
  printf("PC %X\n",reg.PC);
  printf("FL %X\n",reg.FL);
  printf("SP %X\n",reg.SP);
  printf("W[0] %X\n",reg.W[0]);
  printf("B[0] %X\n",reg.B[0]);
  printf("B[1] %X\n",reg.B[1]);
  printf("C %X\n",reg.C);
  printf("Z %X\n",reg.Z);
  printf("V %X\n",reg.V);
  printf("N %X\n",reg.N);
  printf("I %X\n",reg.I);
  printf("reserved %X\n",reg.reserved);
  printf("sizeof cpu %d\n", (int)sizeof(struct bs3_cpu_data));
  
    struct termios oldtio, curtio;
    struct sigaction sa;

    /* Save stdin terminal attributes */
    tcgetattr(0, &oldtio);

    /* Make sure we exit cleanly */
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = sighandler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    /* add timer alarm signal handling */
    sigaction(SIGVTALRM, &sa, NULL);

    /* This is needed to be able to tcsetattr() after a hangup (Ctrl-C)
     * see tcsetattr() on POSIX
     */
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTTOU, &sa, NULL);

    /* Set non-canonical no-echo for stdin */
    tcgetattr(0, &curtio);
    curtio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &curtio);

    /* main loop */
    while (!end) {
            struct pollfd pfds[1];
            int ret;
            char c;

            /* See if there is data available */
            pfds[0].fd = 0;
            pfds[0].events = POLLIN;
            ret = poll(pfds, 1, 0);

            /* Consume data */
            if (ret > 0 && ((pfds[0].revents & 1) == 1) ) {
                    /* printf("Data available\n"); */
                    read(0, &c, 1);
                    int d = (int) c;
                    int e = pfds[0].revents;
                    printf("revents %X, ret =%X, c=%d\n",e, ret,d);
            }
    }

    /* restore terminal attributes */
    tcsetattr(0, TCSANOW, &oldtio);

}
