#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "main.h"
#include "hybcomb.h"

volatile int64_t counter CACHE_ALIGN = 0;
volatile int64_t neverused CACHE_ALIGN = 0;

static int64_t increment(volatile void* state, int64_t arg) {
	int64_t ret = counter;
	counter += arg;
	return ret;
}

#ifdef INLINE_CS
void* inline_cs(volatile void* counter, void* arg) {
	return increment(counter, arg);
}
#endif

static HybLock cnt_lock CACHE_ALIGN;

void global_init(void) {
    hybcomb_init(&cnt_lock, &counter);
}

void thread_init(void) {
    hybcomb_thread_init();
}

void user_iteration(int i) {
	volatile int64_t ret = apply_op(&cnt_lock, &increment, 1);
	//printf("%d\n", ret);
}

bool is_server(int id) { return 0; }

int server_count(void) { return 0; }

int run_server(void) {}

void stop_server(int id) {}

void thread_cleanup() {
	#ifdef GET_STATS
	//printf("[%d] combining degree: %f\n", id, ((float)requests)/instances);
	//printf("[%d] CAS succ/exec: %d/%d = %f\n", id, cas_succ, cas_exec, ((float)cas_succ)/cas_exec);
	__sync_fetch_and_add(&tot_requests, requests);
	__sync_fetch_and_add(&tot_instances, instances);
	__sync_fetch_and_add(&tot_cas_succ, cas_succ);
	__sync_fetch_and_add(&tot_cas_exec, cas_exec);
	#endif
}

void global_cleanup() {
	#ifdef GET_STATS
	float tot_ops = tot_instances + tot_requests;
	printf("combining degree: %f\n", ((float)tot_requests)/tot_instances);
	printf("CAS: %f %f\n", tot_cas_exec/tot_ops, ((float)tot_cas_succ)/tot_cas_exec);
	#endif
}
