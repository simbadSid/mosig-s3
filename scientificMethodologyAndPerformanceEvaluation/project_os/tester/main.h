#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdbool.h>

extern __thread int id;
extern int nthreads;
extern volatile int main_work;

void wait_cycles (int cyc);
unsigned long long get_cycle_count(void);

void global_init(void);
void thread_init(void);
bool is_server(int id);
int server_count(void);
int run_server(void);
void stop_server(int srv_id);
void user_iteration(int i);
void thread_cleanup();
void global_cleanup();



#endif
