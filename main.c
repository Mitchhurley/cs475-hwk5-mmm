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
double clockOffset;
double clocktotalPar;
double clocktotalSeq;
double biggestMistake = 0;

int main(int argc, char *argv[]) {
	int numruns = 4;
	clocktotalPar = 0;
	clocktotalSeq = 0;
	while (numruns > 0){
		double clockstart, clockend;
		// start clocking
		//If there are 4 args, its parallel
		if (argc == 4){
			//Error handling, making sure val 1 is p, that size and threads are acceptable intergers etc...
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
			//Print out run info on first go around
			if (numruns == 4){
				printf("========\nmode: Parallel\nThread Count: %d\nSize: %d\n========\n", threadCount, matSize);
			}
			mmm_init();
			matParams *args = (matParams*) malloc(threadCount * sizeof(matParams));
			pthread_t *threads = (pthread_t*) malloc(threadCount * sizeof(pthread_t));
			int numToTake = (matSize * matSize) / threadCount;
			int i= 0;
			clockstart = rtclock();
			for (i = 0; i < threadCount - 1; i++){
				args[i].Pstart = (i * numToTake);
				args[i].Pend = (i + 1)* numToTake;
				pthread_create(&threads[i], NULL, mmm_par, (void*) &args[i]);
				
			}
			args[threadCount - 1].Pstart = (threadCount - 1) * numToTake;
			args[threadCount - 1].Pend = matSize * matSize;
			pthread_create(&threads[threadCount - 1], NULL, mmm_par, (void*) &args[threadCount - 1]);
			// Wait for threads to complete
			for (int i = 0; i < threadCount; i++) {
				pthread_join(threads[i], NULL);
			}
			clockend = rtclock();
			if (numruns != 4){ clocktotalPar += clockend - clockstart;}
			clockstart = rtclock();
			clockOffset = 0;
			biggestMistake = fmax(biggestMistake,mmm_verify());
			clockend = rtclock();
			if (numruns != 4){clocktotalSeq += clockend - clockstart; clocktotalSeq -= clockOffset;}
			free(args);
			free(threads);
			mmm_freeup();
		}

		else if (argc == 3){
			//make sure that the first value is S 
			clockstart = rtclock();
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
			clockstart = rtclock();
			mmm_seq();
			
			clockend = rtclock();
			mmm_freeup();
			
			if (numruns == 4){
				printf("========\nmode: Sequential\nThread Count: %d\nSize: %d\n========\n", 1, matSize);
			}else clocktotalSeq += clockend - clockstart;
			
		}
		else {
			printf("Invalid Usage: S <Size> |OR| P <Threads> <Size>\n");
			exit(1);
		}
		 // stop clocking
		if (numruns != 4){
			
			numruns--;
		}else{
			numruns--;
		}
	}
	if (argc == 4){
		printf("Sequential Time (avg of 3 runs): %.6f sec\nParallel Time (avg of 3 runs): %.6f sec\nSpeedup: %.6f\n",clocktotalSeq/3,clocktotalPar/3, clocktotalSeq/clocktotalPar);
		printf("Verifying... The largest error was %f \n",biggestMistake);
	}
	
	else{
		printf("Sequential Time (avg of 3 runs): %.6f sec\n", clocktotalSeq/3);
	}
	return 0;
}
