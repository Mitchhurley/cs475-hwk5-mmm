#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include "rtclock.h"
#include "mmm.h"

int matSize;
int **inputA;
int **inputB;
int **output;

int main(int argc, char *argv[]) {
	double clockstart, clockend;
	clockstart = rtclock(); // start clocking

	// start: stuff I want to clock
	//If there are 4 args, its parallel
	if (argc == 4){
		//make sure that the first val is P
		if (*argv[1] != 'P'){
			printf("Proper syntax: P <Threads> <Size>\n");
			exit(1);
		}
		else if (atoi(argv[2]) < 0 || atoi(argv[3]) < 0){
			printf("Error: Threads and Size must be positive numbers\n");
			exit(1);
		}
		else if(atoi(argv[2]) > atoi(argv[3])){
			printf("Error: Threads must be less than Size\n");
			exit(1);
		}
		for (int i = 0; i < strlen(argv[2]); i++){
			if (!isdigit(argv[2][i])){
				printf("Error: Threads must be an Integer\n");
				exit(1);
			}
		}
		for (int i = 0; i < strlen(argv[3]); i++){
			if (!isdigit(argv[3][i])){
				printf("Error: Size must be an Integer\n");
				exit(1);
			}
		}
		matSize = atoi(argv[3]);
		int threadCount = atoi(argv[2]);
		mmm_init();
		matParams *args = (matParams*) malloc(threadCount * sizeof(matParams));
		pthread_t *threads = (pthread_t*) malloc(threadCount * sizeof(pthread_t));
		/*
		double threadsPerSide = (sqrt((double)threadCount));
		double blocksize =ceil((double)matSize / sqrt((double)threadCount));
		matParams *pars = (matParams*) malloc(threadCount * sizeof(matParams));
		printf("%f is the block length, %f is threads per side\n", blocksize, threadsPerSide);
		int currthread = 0;
		for (int i = 0; i < threadsPerSide; i++){
			for (int j = 0; j < threadsPerSide; j++){
				pars[currthread].startCol = j * blocksize;
				pars[currthread].startRow = i * blocksize;
				pars[currthread].endCol = (j+1) * blocksize - 1;
				pars[currthread].endRow = (i+1) * blocksize - 1;
				currthread++;
				printf("Block (%d) assigned\n", currthread);
			}
			
			pars[last].startCol = last * blocksize;
			pars[last].startRow = i * blocksize;
			pars[last].endCol = matSize - 1;
			pars[last].endRow = matSize;
			printf("Block (%d) assigned\n",);
			
		}*/
		int numToTake = (matSize * matSize) / threadCount;
		int i= 0;
		for (i = 0; i < threadCount - 1; i++){
			args[i].Pstart = (i * numToTake);
			args[i].Pend = (i + 1)* numToTake;
			//printf("%d,%d is the thread start, %d, %d is thread end\n", args[i].Pstart / matSize, args[i].Pstart % matSize, args[i].Pend / matSize, args[i].Pend % matSize);
			pthread_create(&threads[i], NULL, mmm_par, (void*) &args[i]);
		}
		//TODO
		args[threadCount - 1].Pstart = (threadCount - 1) * numToTake;
		args[threadCount - 1].Pend = matSize * matSize;
		pthread_create(&threads[threadCount - 1], NULL, mmm_par, (void*) &args[threadCount - 1]);
		// Wait for threads to complete
		for (int i = 0; i < threadCount; i++) {
			pthread_join(threads[i], NULL);
		}
		printf("Resultant matrix:\n");
    	for (int i = 0; i < matSize; i++) {
        	for (int j = 0; j < matSize; j++) {
            	printf("%d ", output[i][j]);
        	}
        	printf("\n");
    	}
		printf("The largest error was %f \n",mmm_verify());
		free(args);
		free(threads);
		mmm_freeup();
	}

	else if (argc == 3){
		//make sure that the first value is S 
		if (*argv[1] != 'S'){
			printf("Proper syntax: S <Size>\n");
			exit(1);
		}
		else if (atoi(argv[2]) < 0){
			printf("Error: Size must be a positive number\n");
			exit(1);
		}
		for (int i = 0; i < strlen(argv[2]); i++){
			if (!isdigit(argv[2][i])){
				printf("Error: Size must be an Integer\n");
				exit(1);
			}
		}
		matSize = atoi(argv[2]);
		mmm_init();
		mmm_seq();
		printf("Resultant matrix A:\n");
    	for (int i = 0; i < matSize; i++) {
        	for (int j = 0; j < matSize; j++) {
            	printf("%d ", output[i][j]);
        	}
       		printf("\n");
    	}
		mmm_freeup();
		
	}
	else {
		printf("Invalid Usage: S <Size> |OR| P <Threads> <Size>\n");
		exit(1);
	}
	clockend = rtclock(); // stop clocking
	printf("Time taken: %.6f sec\n", (clockend - clockstart));

	return 0;
}
