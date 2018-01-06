#include <stdio.h>
#include "mp-server.h"
#include "main.h"
#include "ccrand.h"

#define SERVER1_ID 0
#define SERVER2_ID 1

int64_t cs(int64_t arg, int unused) {
	return arg;
}

void global_init(void)
{
	mpserver_global_init();
	
	if (nthreads < 3) {
		fprintf(stderr, "At least two threads!\n");
		exit(1);
	}
}

void thread_init(void)
{
	mpserver_thread_init();
}

bool is_server(int id)
{
	return id == SERVER1_ID || id == SERVER2_ID;
}

int server_count(void)
{
	return 2;
}

int run_server(void) {
	return mpserver_run_server();
}

void stop_server(int sid) {
	mpserver_stop_servers();
}

void user_iteration(int i)
{
	mpserver_apply_op(id%2?SERVER1_ID:SERVER2_ID, cs, 0);
}

void thread_cleanup() {
}

void global_cleanup() {
}
