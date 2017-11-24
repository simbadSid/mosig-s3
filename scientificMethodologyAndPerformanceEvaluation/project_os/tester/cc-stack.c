#include <stdlib.h>
#include <stdbool.h>
#include <numa.h>
#include "main.h"
#include "hsynch.h"
#include "ccpool.h"

#define  GUARD          INT_MIN

typedef struct ListNode {
    int64_t val;
    struct ListNode *next;
} ListNode;





ListNode *guard_node CACHE_ALIGN;
volatile ListNode *head CACHE_ALIGN;






HSynchStruct stack_lock CACHE_ALIGN;
__thread HSynchThreadState lstack_lock;

__thread PoolStruct pool_node;

static RetVal push(void* state, ArgVal arg, int pid) {
    ListNode *node = (ListNode*)alloc_obj(&pool_node);
    node->next = (ListNode*)head;
    node->val = arg;
    head = node;
    return -1;
}

static RetVal pop(void* state, ArgVal arg, int pid) {
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
    HSynchStructInit(&stack_lock);   

//TODO ASK
	init_pool(&pool_node, CACHE_LINE_SIZE);

	ListNode gn CACHE_ALIGN = {GUARD, null};

	guard_node	= alloc_obj(&pool_node);
	*guard_node	= gn;
	head		= guard_node;

}

void thread_init(void)
{
	HSynchThreadStateInit(&lstack_lock, id);
    init_pool(&pool_node, CACHE_LINE_SIZE);
}

void user_iteration(int i)
{
	if (i % 2)
        applyOp(&stack_lock, &lstack_lock, pop, null, 0, id);
    else
        applyOp(&stack_lock, &lstack_lock, push, null, 0, id);
}

bool is_server(int id)
{
	return false;
}

int server_count(void)
{
	return 0;
}

void warmup_server(int warmup)
{
}

int run_server(void)
{
	return 0;
}

void stop_server(int id) {}

void thread_cleanup() {}

void global_cleanup() {}
