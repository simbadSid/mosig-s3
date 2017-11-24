#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "main.h"
#include "hybcomb.h"

#include <tmc/alloc.h>
#include <sys/types.h>
#include <tmc/cpus.h>
#include <tmc/task.h>
#include <tmc/udn.h>

static volatile int64_t array[CS_LENGTH];

static void* critical_section(volatile void* object, void* arg) {
	for(int i = 0; i < CS_LENGTH; i++) ((int64_t*)(object))[i]++;
}

static HybLock lock;

void global_init(void) {
	//printf("%d\n", tmc_task_has_shepherd());

	if (tmc_udn_init(NULL) < 0)
    	tmc_task_die("Failure in 'tmc_udn_init'.");
    	
    hybcomb_init(&lock, array);
}

void thread_init(void) {
	if (tmc_udn_activate() < 0)
    	tmc_task_die("Failure in 'tmc_udn_activate()'.");
    	
    hybcomb_thread_init();
}

void user_iteration(int i) {
	volatile void* ret = apply_op(&lock, &critical_section, NULL);
}

bool is_server(int id) { return 0; }

int server_count(void) { return 0; }

int run_server(void) {}

void stop_server(int id) {}

void thread_cleanup() {}

void global_cleanup() {}
