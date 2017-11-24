#include <stdlib.h>
#include <stdbool.h>
#include <numa.h>
#include "main.h"

#define CLUSTER_SIZE 	20
#define NCLUSTERS		8
#define BACKOFF		150000

static int64_t* counter;
static int cluster = 0;

void global_init(void)
{
	counter = numa_alloc_onnode(sizeof(int64_t), 0);
	*counter = 0;
}

void thread_init(void) { }

bool is_server(int id) { return false; }

int server_count(void) { return 0; }

int run_server(void) { return 0; }

void stop_server(int id) { }

void user_iteration(int i) {
	int c = cluster;
	int myc = id / CLUSTER_SIZE;
	if (c != myc) {
		for(volatile int i = 0; i < BACKOFF; i++);
		__sync_bool_compare_and_swap(&cluster, c, myc);
			//printf("new: %d", myc);
	}

	__sync_fetch_and_add(counter, 1);
}

void thread_cleanup() {}

void global_cleanup() {}
