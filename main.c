#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
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
		mmm_init();
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
		mmm_freeup();
		
	}
	else {
		printf("Invalid Usage: S <Size> |OR| P <Threads> <Size>\n");
		exit(1);
	}
	// end: stuff I want to clock
	clockend = rtclock(); // stop clocking
	printf("Time taken: %.6f sec\n", (clockend - clockstart));

	return 0;
}
