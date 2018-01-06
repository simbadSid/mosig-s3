#include <stdio.h>
#include "main.h"
#include "mp-server.h"
#include "ccpool.h"

//#define MS_SINGLE_LOCK

#define ENQ_SERVER 0
#ifdef MS_SINGLE_LOCK
	#define DEQ_SERVER 0
#else
	#define DEQ_SERVER 1
#endif

typedef struct ListNode {
    int64_t val;
    struct ListNode *next;
} ListNode;





ListNode *guard_node CACHE_ALIGN;
volatile ListNode *first CACHE_ALIGN;
volatile ListNode *last CACHE_ALIGN;

__thread PoolStruct pool_node;

volatile int a CACHE_ALIGN = 1;

static int64_t enqueue(int64_t arg, int unused) {
     ListNode *node = alloc_obj(&pool_node);
     node->next = null;
     node->val = arg;
     last->next = node;
     last = node;
     return 0;
}

static int64_t dequeue(int64_t unused1, int unused2) {
     int64_t ret;
     volatile ListNode *node = first;
     ListNode *new_first = first->next;
	 if (new_first == null)
	 {
        return -1;
	 }
	 ret = new_first->val;
	 first = new_first;
	 free_obj(&pool_node, (void*)node);
	 return ret;
}

void global_init(void)
{
	mpserver_global_init();

#ifdef MS_SINGLE_LOCK	
	if (nthreads < 2) {
#else
	if (nthreads < 3) {
#endif
		fprintf(stderr, "At least three threads!\n");
		exit(1);
	}

//TODO ASK
	init_pool(&pool_node, CACHE_LINE_SIZE);

	ListNode gn CACHE_ALIGN = {INT_MIN, null};

	guard_node	= alloc_obj(&pool_node);
	*guard_node	= gn;
	first		= guard_node;
	last		= guard_node;

}

void thread_init(void)
{
	mpserver_thread_init();
	init_pool(&pool_node, CLS);
}


void user_iteration(int i)
{	
	if (i % 2) {
        mpserver_apply_op(DEQ_SERVER, dequeue, 0);
    }
    else {
        mpserver_apply_op(ENQ_SERVER, enqueue, id);
    }
}


bool is_server(int id)
{
#ifdef MS_SINGLE_LOCK	
	return id == ENQ_SERVER;
#else
	return id == ENQ_SERVER || id == DEQ_SERVER;
#endif
}

int server_count(void)
{
#ifdef MS_SINGLE_LOCK	
	return 1;
#else
	return 2;
#endif
}

int run_server(void) {
	return mpserver_run_server();
}

void stop_server(int sid) {
	mpserver_stop_servers();
}

void thread_cleanup() {}

void global_cleanup() {}
