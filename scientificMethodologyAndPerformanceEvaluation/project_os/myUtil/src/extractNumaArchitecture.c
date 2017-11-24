/*
 * extractNumaArchitecture.cc
 *
 *  Created on: Jun 1, 2016
 *      Author: littlegirle
 */

#include <stdlib.h>
#include <numa.h>
#include <stdio.h>



#define MAXN	50
#define MAXC	50

#define PARAMETER_PRINT_ALL			"-all"
#define PARAMETER_PRINT_NBR_NODE	"-nn"
#define PARAMETER_PRINT_NBR_CPU		"-nc"
#define PARAMETER_PRINT_NODE_LIST	"-nl"


// ------------------------------------------
// Local global parameters
// ------------------------------------------
	static int nbrNodes;
	static int nbrCpu;
	static int nodes[MAXN][MAXC];
	static int map[MAXN*MAXC];


// ------------------------------------------
// Local functions
// ------------------------------------------
	/**
	 * Initializes the hardware configuration of the machine
	 */
	void extractNuma()
	{
		int i, j, m;

		nbrNodes = numa_num_configured_nodes();

		/* buggy (off by 2) with some versions of libnuma */
		int wrong_ncpus = numa_num_configured_cpus();
		nbrCpu = 0;
		for (i=0; i<nbrNodes; i++)
		{
			struct bitmask* b = numa_allocate_cpumask();
			numa_node_to_cpus(i, b);

			int next = 0;
			for (j=0; j<wrong_ncpus; j++)
			{
				if (numa_bitmask_isbitset(b, j))
				{
#ifdef DISABLE41
					if (j==41) continue;
#endif

#ifdef DISABLE42
					if (j==42) continue;
#endif

					nodes[i][next++] = j;
					nbrCpu++;
				}
			}
			nodes[i][next] = -1;
			numa_free_cpumask(b);
		}

#ifndef CUSTOM_MAPPING
#ifndef MIXED_MAPPING
		for (i=0, m=0; i<nbrNodes; i++)
		{
			for (j=0; nodes[i][j]!=-1; j++)
			{
				map[m++] = nodes[i][j];
			}
		}
#else
		for (i=0, j=0, m=0; m < nbrCpu;)
		{
			if (nodes[i][j] >= 0)
				map[m++] = nodes[i][j];
			i = (i + 1) % nbrNodes;
			if (i == 0)
				j++;
		}
#endif
#endif
	}


	void printHelp()
	{
		printf("\nUse on of the parameters: \n");
		printf("\t%s\t\t: To print all the NUMA parameters\n",		PARAMETER_PRINT_ALL);
		printf("\t%s\t\t: To print the number of NUMA nodes\n",		PARAMETER_PRINT_NBR_NODE);
		printf("\t%s\t\t: To print the number of NUMA cpu\n",		PARAMETER_PRINT_NBR_CPU);
		printf("\t%s\t\t: To print the number of NUMA cpu\n",		PARAMETER_PRINT_NODE_LIST);
	}


	/**
	 * Read the parameters of the prog and put them into the output parameters
	 */
	void extractParameter(int argc, char **argv, char *printAll, char *printNbrNode, char *printNbrCpu, char *printNodeList)
	{
		int i;

		*printAll		= 1;
		*printNbrNode	= 0;
		*printNbrCpu	= 0;
		*printNodeList	= 0;

		for (i=1; i<argc; i++)
		{
			if 		(!strcmp(argv[i], PARAMETER_PRINT_ALL))
			{
				*printAll		= 1;	*printNbrNode	= 0;
				*printNbrCpu	= 0;	*printNodeList	= 0;
			}
			else if	(!strcmp(argv[i], PARAMETER_PRINT_NBR_NODE))
			{
				*printAll		= 0;	*printNbrNode	= 1;
				*printNbrCpu	= 0;	*printNodeList	= 0;
			}
			else if	(!strcmp(argv[i], PARAMETER_PRINT_NBR_CPU))
			{
				*printAll		= 0;	*printNbrNode	= 0;
				*printNbrCpu	= 1;	*printNodeList	= 0;
			}
			else if	(!strcmp(argv[i], PARAMETER_PRINT_NODE_LIST))
			{
				*printAll		= 0;	*printNbrNode	= 0;
				*printNbrCpu	= 0;	*printNodeList	= 1;
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


// ------------------------------------------
// Main function
// ------------------------------------------
	int main(int argc, char** argv)
	{
		char printAll, printNbrNode, printNbrCpu, printNodeList;
		int i, j;

		extractParameter(argc, argv, &printAll, &printNbrNode, &printNbrCpu, &printNodeList);
		extractNuma();

		if (printAll)
		{
			printf("NUMA configuration of the local machine:\n");
			printf("\t- Number of configured node : %d\n", nbrNodes);
			printf("\t- Number of configured cpu  : %d\n", nbrCpu);
			for (i=0; i<nbrNodes; i++)
			{
				printf("\t- node %d: ", i);
				for(j=0; nodes[i][j]>=0; j++)
					printf("%d ", nodes[i][j]);
				printf("\n");
			}
			printf("\t- map: ");
			for (i=0; i<nbrCpu; i++)
				printf("%d ", map[i]);
			printf("\n");
		}
		else if (printNbrNode)
		{
			printf("%d\n", nbrNodes);
		}
		else if (printNbrCpu)
		{
			printf("%d\n", nbrCpu);
		}
		else if (printNodeList)
		{
			for (i=0; i<nbrNodes; i++)
			{
				if (i > 0)
					printf(",");
				printf("%d", i);
			}
			printf("\n");
		}
		return 0;
	}
