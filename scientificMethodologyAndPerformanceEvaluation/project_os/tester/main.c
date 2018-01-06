#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <numa.h>
#include "main.h"
#include "ccrand.h"



#define PARAMETER_MAX_NB_THREAD		"-maxThread"
#define PARAMETER_TEST_TIME			"-testTime"
#define PARAMETER_STAT_FILE			"-statFile"
#define PARAMETER_ALLOCATOR_NAME	"-allocName"
#define PARAMETER_NBR_RUN			"-nbrRun"	// Number of runs to generate each point
#define PARAMETER_PRINT_HELP		"-help"

#define DEFAULT_TEST_TIME			3
#define DEFAULT_MIN_NB_THREAD		4
#define DEFAULT_NBR_RUN				4
#define DEFAULT_THREAD_STEP			1
#define DEFAULT_RESULT_DIR			"../statistic"

#define BREAK						1000
#define MAXN						50
#define MAXC						50


// ------------------------------------------
// Structures
// ------------------------------------------
	typedef struct lat
	{
		unsigned long long time;
		unsigned long long padding[7];
	} LatencyStruct;



// ------------------------------------------
// Global parameters
// ------------------------------------------
																	// Thread
	__thread int				id;									//		Id of the current thread
	int							nthreads;							//		Total number of created threads
	__thread unsigned long		next = 1;							//		For ccrand
	volatile int				main_work = 1;						//		Used to stop a thread
																	// Hardware definition
	static int 					nodes[MAXN][MAXC];					//		NUMA architecture node id
	int							nnodes;								//		Total number of NUMA nodes
	int							ncpus;								//		Total number of cpu (on all the nodes)
																	// Test statistics
	int							*ops;								//		Number of operations (array: 1 cell per thread)
	int							totops, minops, maxops;				//		Stat of all the threads
	LatencyStruct				*lats;
	unsigned long long			totlats;
																	// Thread syncronization
	static int					clients_left;
	static pthread_barrier_t 	init_barr;							//		Used to check the creation of all the expected threads


// ------------------------------------------
// Local functions
// ------------------------------------------
	/**
	 * Return the number of cpu cycles (CPU where the thread is running)
	 */
	__inline__ unsigned long long get_cycle_count(void)
	{
	  unsigned hi, lo;
	  __asm__ __volatile__ ("rdtsc" : "=a"(lo), "=d"(hi));
	  return ( (unsigned long long)lo)|( ((unsigned long long)hi)<<32 );
	}

	/**
	 * Wait for the given number of cycles. (may exceed the guiven number of cycles)
	 */
	void wait_cycles (int cyc)
	{
		unsigned long long end = get_cycle_count() + cyc;
		while(get_cycle_count() < end);
	}

	/**
	 * Signal handler to stop the client threads
	 */
	void ALRMhandler (int sig)
	{
	  main_work = 0;
	}




	//#define CUSTOM_MAPPING
	#ifndef CUSTOM_MAPPING
	int map[MAXN*MAXC] = {[0 ... MAXN*MAXC-1] = -1};
	#else

	/*
	int map[] = {
		1, //2, 3, 4, 5, 6, 7, 8, 9, 10,
		11, 12, 13, 14, 15, 16, 17, 18, 19, 20,
		21, 22, 23, 24, 25, 26, 27, 28, 29, 30,
		31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
		0, 81, 82, 83, 84, 85, 86, 87, 88, 89,
		90, 91, 92, 93, 94, 95, 96, 97, 98, 99,
		100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
		110, 111, 112, 113, 114, 115, 116, 117, 118, 119,
	};
	*/

	int map[] = {
		//1, 11, 2, 12, 3, 13, 4, 14, 5, 15, 6, 16, 7, 17, 8, 18, 9, 19, 10, 20,
		//21, 31, 22, 32, 23, 33, 24, 34, 25, 35, 26, 36, 27, 37, 28, 38, 29, 39, 30, 40,
		//0, 90, 81, 91, 82, 92, 83, 93, 84, 94, 85, 95, 86, 96, 87, 97, 88, 98, 89, 99,
		//100, 110, 101, 111, 102, 112, 103, 113, 104, 114, 105, 115, 106, 116, 107, 117, 108, 118, 109, 119
		1, 11, 21, 31, 0, 90, 100, 110, /*2,*/ 12, 22, 32, 81, 91, 101, 111,
		/*3,*/ 13, 23, 33, 82, 92, 102, 112, /*4,*/ 14, 24, 34, 83, 93, 103, 113,
		/*5,*/ 15, 25, 35, 84, 94, 104, 114, /*6,*/ 16, 26, 36, 85, 95, 105, 115,
		/*7,*/ 17, 27, 37, 86, 96, 106, 116, /*8,*/ 18, 28, 38, 87, 97, 107, 117,
		/*9,*/ 19, 29, 39, 88, 98, 108, 118, /*10,*/ 20, 30, 40, 89, 99, 109, 119
	};

	#endif


	/**
	 * Initializes the hardware configuration of the machine
	 */
	void extract_numa()
	{
		nnodes = numa_num_configured_nodes();
		/* buggy (off by 2) with some versions of libnuma */
		int wrong_ncpus = numa_num_configured_cpus();
		ncpus = 0;
		for (int i=0; i<nnodes; i++) {
			struct bitmask* b = numa_allocate_cpumask();
			numa_node_to_cpus(i, b);

			int next = 0;
			for (int j=0; j<wrong_ncpus; j++) {
				if (numa_bitmask_isbitset(b, j))
				{
					#ifdef DISABLE41
					if (j==41) continue;
					#endif

					#ifdef DISABLE42
					if (j==42) continue;
					#endif

					nodes[i][next++] = j;
					ncpus++;
				}
			}
			nodes[i][next] = -1;
			numa_free_cpumask(b);
		}

	#ifndef CUSTOM_MAPPING
	#ifndef MIXED_MAPPING
		for (int i=0, m=0; i<nnodes; i++) {
			for (int j=0; nodes[i][j]!=-1; j++) {
				map[m++] = nodes[i][j];
			}
		}
	#else
		for (int i=0, j=0, m=0; m < ncpus;) {
			if (nodes[i][j] >= 0)
				map[m++] = nodes[i][j];
			i = (i + 1) % nnodes;
			if (i == 0)
				j++;
		}
	#endif
	#endif

		printf("%d configured node\n", nnodes);
		printf("%d configured cpus\n", ncpus);
		for (int i=0; i<nnodes; i++) {
			printf("node %d: ", i);
			for(int j=0; nodes[i][j]>=0; j++)
				printf("%d ", nodes[i][j]);
			printf("\n");
		}
		printf("map: ");
		for (int i=0; i<ncpus; i++)
			printf("%d ", map[i]);
		printf("\n");
	}

	void printHelp()
	{
		printf("\nUse on of the parameters: \n");
		printf("\t%s <number of thread (< %d)>\n",		PARAMETER_MAX_NB_THREAD, ncpus);
		printf("\t%s <duration (in seconds)>\n",		PARAMETER_TEST_TIME);
		printf("\t%s <name of the allocator used>\n",	PARAMETER_ALLOCATOR_NAME);
		printf("\t%s <name of the logger file>\n",		PARAMETER_STAT_FILE);
		printf("\t%s <number of run per point>\n",		PARAMETER_NBR_RUN);
		printf("\t%s\n",								PARAMETER_PRINT_HELP);
	}

	/**
	 * Read the parameters of the prog and put them into the output parameters
	 */
	void extractParameter(int argc, char **argv, int *maxThread, int *testTime, FILE **statFile, char *allocatorName, int *nbrRun, int *threadStep)
	{
		int i;

		*maxThread	= ncpus;
		*testTime	= DEFAULT_TEST_TIME;
		*statFile	= NULL;
		*nbrRun		= DEFAULT_NBR_RUN;
		*threadStep	= DEFAULT_THREAD_STEP;

		sprintf(allocatorName, "Allocator_noName");

		for (i=1; i<argc; i+=2)
		{
			if 	(!strcmp(argv[i], PARAMETER_MAX_NB_THREAD))
				*maxThread = atoi(argv[i+1]);
			else if	(!strcmp(argv[i], PARAMETER_TEST_TIME))
				*testTime = atoi(argv[i+1]);
			else if	(!strcmp(argv[i], PARAMETER_ALLOCATOR_NAME))
				sprintf(allocatorName, "%s", argv[i+1]);
			else if	(!strcmp(argv[i], PARAMETER_NBR_RUN))
				*nbrRun = atoi(argv[i+1]);
			else if	(!strcmp(argv[i], PARAMETER_STAT_FILE))
			{
				char fileName[200];
				sprintf(fileName, "%s/%s", DEFAULT_RESULT_DIR, argv[i+1]);
				*statFile = fopen(fileName, "w");
			}
//TODO ADD a line to read the thread step
			else if	(!strcmp(argv[i], PARAMETER_PRINT_HELP))
			{
				printHelp();
				exit(0);
			}
			else
			{
				printf("\n\n**************************\n");
				printf("Unknown parameter: %s\n", argv[i]);
				printf("**************************\n");
				printHelp();
				exit(0);
			}
		}
	}

	/**
	 * Determine the waiting time of the client thread when addressing a request to the CS server (optimization 1 in the paper of Thomas Ropars)
	 */
	int extractBackoffValue()
	{
		int res;
#ifdef BACKOFF
		res = BACKOFF;
#else
		res = 0;
#endif
		return res;
	}

	/**
	 * Function executed by each client thread
	 */
	void* my_thread(void* threadid)
	{
		cpu_set_t cpuset;
		int round;

		id = (int)(int64_t)threadid;
		CPU_ZERO(&cpuset);
		CPU_SET(map[id], &cpuset);

		/* seed for the random number generator */
		simSRandom(id);

		/* pin the thread to a core */
		if (pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset))
		{
			fprintf(stderr, "Thread pinning failed!\n");
			exit(1);
		}

		/* before staring, call the initialization function provided by the user */
		thread_init();

		/* wait for the others to initialize */
		int ret = pthread_barrier_wait(&init_barr);
		if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD)
		{
			fprintf(stderr, "Waiting on the barrier failed!\n");
			exit(1);
		}

		/* server threads run different code */
		if (is_server(id))
		{
			ops[id] = run_server();
			thread_cleanup();
			return NULL;
		}

		/* go, go, go! */
		for (round = 0; main_work; round++)
		{
			int cyc;
			unsigned long long t = get_cycle_count();
			user_iteration(round);
			lats[id].time += (get_cycle_count() - t);

			cyc = simRandomRange(0, BREAK);
			//for(volatile int i = 0; i < iters; i++);
			wait_cycles(cyc);
		}

		/* store the result */
		ops[id] = round;

		/* if last client, stop the servers */
		if (!is_server(id))
		{
			int left = __sync_add_and_fetch(&clients_left, -1);

			if (left == 0)
			{
				for (int i = 0; i < nthreads; i++)
					if (is_server(i))
						stop_server(i);
			}
		}

		thread_cleanup();

		return NULL;
	}

	int main(int argc, char** argv)
	{
		pthread_t* thr;
		int testTime, maxThread, nbrRun, run, threadStep;
		FILE *statFile = NULL;
		char allocatorName[100];
		int backoff = extractBackoffValue();

		extract_numa();
		extractParameter(argc, argv, &maxThread, &testTime, &statFile, allocatorName, &nbrRun, &threadStep);
		if (statFile != NULL)
		{
			int nbrLine;
//TODO to correct:  This hack has been done for the report charts
			// nbrLine = (maxThread - DEFAULT_MIN_NB_THREAD+2) / threadStep;
			nbrLine = maxThread - DEFAULT_MIN_NB_THREAD + 1;
//TODO End to correct:  This hack has been done for the report charts
			fprintf(statFile, "%s\n%d\n%d\n%d\n%d\n", allocatorName, nnodes, ncpus, backoff, nbrLine);
		}

		signal (SIGALRM, ALRMhandler);

		thr		= (pthread_t*)		malloc(maxThread * sizeof(pthread_t));
		ops		= (int*)			malloc(maxThread * sizeof(int));
		lats	= (LatencyStruct*)	malloc(maxThread * sizeof(LatencyStruct));

		for (nthreads=DEFAULT_MIN_NB_THREAD; nthreads <= maxThread; nthreads+=threadStep)
		{
			float maxThroughput		= 0;
			float minThroughput		= (int)(1 << 30);
			float maxLatency		= 0;
			float minLatency		= (int)(1 << 30);
			float maxFairness		= 0;
			float minFairness		= (int)(1 << 30);
			float totalThroughput		= 0;
			float totalLatency		= 0;
			float totalFairness		= 0;

			for (run=0; run<nbrRun; run++)
			{
				memset(ops, 0, nthreads * sizeof(int));
				memset(lats, 0, nthreads * sizeof(LatencyStruct));
				main_work = 1;

				global_init();

				clients_left = nthreads - server_count();

				if(pthread_barrier_init(&init_barr, NULL, nthreads+1))
				{
					printf("Could not create a barrier\n");
					exit(1);
				}

				/* create the threads */
				for(int i = 0; i < nthreads; i++)
				{
					if(pthread_create(&thr[i], NULL, &my_thread, (void*)(int64_t)i))
					{
						printf("Could not create thread %d\n", i);
						exit(1);
					}
				}

				/* wait for everyone to initialize, then set the alarm */
				int ret = pthread_barrier_wait(&init_barr);
				if (ret != 0 && ret != PTHREAD_BARRIER_SERIAL_THREAD)
				{
					fprintf(stderr, "Waiting on the barrier failed!\n");
					exit(1);
				}
				alarm (testTime);

				/* wait for the clients to finish and count*/
				totops	= 0;
				minops	= 1 << 30;
				maxops	= 0;
				totlats	= 0;
				for(int i = 0; i < nthreads; i++)
				{
					/* don't wait for server threads */
					if (is_server(i))
						continue;
					if(pthread_join(thr[i], NULL))
					{
						fprintf(stderr, "Could not join thread %d\n", i);
						exit(1);
					}

					//printf("[%d]: %d\n" , i, ops[i]);

					totops += ops[i];
					if (ops[i] > maxops) maxops = ops[i];
					if (ops[i] < minops) minops = ops[i];

					totlats += lats[i].time;
				}

				/* wait for the servers to stop */
				for(int i = 0; i < nthreads; i++)
				{
					if (!is_server(i))
						continue;

					if(pthread_join(thr[i], NULL))
					{
						fprintf(stderr, "Could not join thread %d\n", i);
						exit(1);
					}
				}

				// Compute the profiler stats
				float throughput	= totops/testTime;
				float latency		= totlats/totops;
				float fairness		= ((float)maxops)/minops;
				totalThroughput	+= throughput;
				totalLatency	+= latency;
				totalFairness	+= fairness;
				if (throughput	> maxThroughput)	maxThroughput	= throughput;
				if (throughput	< minThroughput)	minThroughput	= throughput;
				if (latency		> maxLatency)		maxLatency		= latency;
				if (latency		< minLatency)		minLatency		= latency;
				if (fairness	> maxFairness)		maxFairness		= fairness;
				if (fairness	< minFairness)		minFairness		= fairness;
			}

			int			averageThroughput	= (int)			(totalThroughput	/ nbrRun);
			long long	averageLatency		= (long long)	(totalLatency		/ nbrRun);
			float		averageFairness		= 				 totalFairness		/ nbrRun;
			//printf("throughput: %d (%.2f/op)\t\t", totops/testtime, (testtime * CPU_FREQ)/totops);
			printf("- NbThread = %d\n",				nthreads);
			printf("\tThroughput: %d [%d, %d]\t\t",	averageThroughput,	(int)minThroughput,		(int)maxThroughput);
			printf("Latency: %lld [%ld, %ld]\t\t",	averageLatency,		(long long)minLatency,	(long long)maxLatency);
			printf("Fairness: %f [%f, %f]",			averageFairness,	minFairness,			maxFairness);
			printf("\n");

			if (statFile != NULL)
			{
				fprintf(statFile, "%d ",				nthreads);
				fprintf(statFile, "%d %d %d ",			averageThroughput,	(int)minThroughput,		(int)maxThroughput);
				fprintf(statFile, "%lld %lld %lld ",	averageLatency,		(long long)minLatency,	(long long)maxLatency);
				fprintf(statFile, "%f %f %f ",			averageFairness,	minFairness,			maxFairness);
				for(int i = 0; i < nthreads; i++)
				{
					fprintf(statFile, "%d ",	ops[i]/testTime);
				}
				for(int i = nthreads; i < ncpus; i++)
				{
					fprintf(statFile, "%d ",	0);
				}
				fprintf(statFile, "\n");
			}

			global_cleanup();
		}

		free(thr);
		free(ops);
		free(lats);
		if (statFile != NULL)
			fclose(statFile);

		return 0;
	}
