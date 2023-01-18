#ifndef _BS3_DEBUG_H
#define _BS3_DEBUG_H 1

void bs3_debug_prepare(const char * bindaddr, int port);
void bs3_debug_end();
void bs3_debug(struct bs3_cpu_data * pbs3);
#endif