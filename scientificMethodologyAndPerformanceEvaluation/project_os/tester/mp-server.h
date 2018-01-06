#include <stdio.h>
#include "mplib.h"
#include "main.h"
#include "cctypes.h"

typedef RetVal (*Fptr)(ArgVal, int);

static volatile int work;

void mpserver_global_init(void)
{
	mplib_global_init();
}

void mpserver_thread_init(void)
{
	mplib_thread_init();
//TODO added by Riyane: otherwise, the server can't be run after have been stoped
	work = 1;
}

int mpserver_run_server() {
	int curr = 0;
	Fptr func;
	ArgVal arg;
	RetVal res;

	while (work) {
		if(mplib_try_get2(id, curr, REQ, (int64_t*)(&func), &arg)) {
			res = func(arg, curr);
			mplib_put1(id, curr, RESP, res);
		}
		if (++curr >= nthreads) {
			curr = 0;
		}
	}
	return 0;
}

void mpserver_stop_servers() {
	work = 0;
}

int64_t mpserver_apply_op(int sid, Fptr func, int64_t arg)
{
	int64_t response;

	mplib_put2(sid, id, REQ, (int64_t)func, arg);
#ifndef EMPTY_TX_DEBUG
	mplib_get1(sid, id, RESP, &response);
	return response;
#else
	return mplib_get1(sid, id, RESP, &response);
#endif
}
