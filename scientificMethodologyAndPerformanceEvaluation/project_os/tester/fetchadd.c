#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <numa.h>
#include "main.h"

__attribute__((aligned(4096))) volatile int64_t* counter;

void global_init(void)
{
	counter = malloc(sizeof(int64_t));//numa_alloc_onnode(sizeof(int64_t), 0);
	*counter = 0;
}

void thread_init(void) { }

bool is_server(int id) { return false; }

int server_count(void) { return 0; }

int run_server(void) { return 0; }

void stop_server(int id) { }

void user_iteration(int i) {
	__sync_fetch_and_add(counter, 1);
	//(*counter)++;
}

void thread_cleanup() {}

void global_cleanup() {
	printf("%lld\n", *counter);
}
