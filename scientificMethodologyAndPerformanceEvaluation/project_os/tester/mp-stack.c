#include <stdio.h>
#include "main.h"
#include "mp-server.h"
#include "ccpool.h"

#define SERVER 0





typedef struct ListNode {
    int64_t val;
    struct ListNode *next;
} ListNode;





ListNode *guard_node CACHE_ALIGN;
volatile ListNode *head CACHE_ALIGN;





__thread PoolStruct pool_node;

static int64_t push(int64_t arg, int pid) {
    ListNode *node = (ListNode*)alloc_obj(&pool_node);
    node->next = (ListNode*)head;
    node->val = arg;
    head = node;
    return -1;
}

static int64_t pop(int64_t arg, int pid) {
     ListNode *node = (ListNode*)head;
     RetVal ret;
     if (head->next == null)
         return -1;
     else { 
        head = head->next;
        ret = node->val;
        free_obj(&pool_node, node);
        return ret;
     }
}

void global_init(void)
{
	mpserver_global_init();
	
	if (nthreads < 2) {
		fprintf(stderr, "At least two threads!\n");
		exit(1);
	}

//TODO ASK
	init_pool(&pool_node, CACHE_LINE_SIZE);

	ListNode gn CACHE_ALIGN = {INT_MIN, null};

	guard_node	= alloc_obj(&pool_node);
	*guard_node	= gn;
	head		= guard_node;
}

void thread_init(void)
{
	mpserver_thread_init();
	init_pool(&pool_node, CLS);
}


void user_iteration(int i)
{	
	if (i % 2) {
        mpserver_apply_op(SERVER, pop, 0);
    }
    else {
        mpserver_apply_op(SERVER, push, id);
    }
}


bool is_server(int id)
{
	return id == SERVER;
}

int server_count(void)
{
	return 1;
}

int run_server(void) {
	return mpserver_run_server();
}

void stop_server(int sid) {
	mpserver_stop_servers();
}

void thread_cleanup() {}

void global_cleanup() {}
