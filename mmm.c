#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "rtclock.h"
#include "mmm.h"

/**
 * Allocate and initialize the matrices on the heap. Populate
 * the input matrices with random integers from 0 to 99
 */
void mmm_init() {
	inputA = (int**) malloc(sizeof(int*) * matSize);
	inputB = (int**) malloc(sizeof(int*) * matSize);
	output = (int**) malloc(sizeof(int*) * matSize);
	for (int i = 0; i < matSize; i++){
		inputA[i] = (int*) malloc(sizeof(int) * matSize);
		inputB[i] = (int*) malloc(sizeof(int) * matSize);
		output[i] = (int*) malloc(sizeof(int) * matSize);
	}
	for (int i = 0; i < matSize; i++){
		for (int j = 0; j < matSize; j++){
			inputA[i][j] = rand() % 100;
			inputB[i][j] = rand() % 100;
		}
	}
	/*
	printf("Resultant matrix A:\n");
    for (int i = 0; i < matSize; i++) {
        for (int j = 0; j < matSize; j++) {
            printf("%d ", inputA[i][j]);
        }
        printf("\n");
    }*/
}

/**
 * Reset a given matrix to zeroes
 * @param matrix pointer to a 2D array
 */
void mmm_reset(double **matrix) {
	for (int i = 0; i < matSize; i++){
		for (int j = 0; j < matSize; j++){
			inputA[i][j] = 0;
		}
	}
}

/**
 * Free up memory allocated to all matrices
 */
void mmm_freeup() {
	for (int i = 0; i < matSize; i++) {
    	free(inputA[i]);
		free(inputB[i]);
		free(output[i]);
    	inputA[i] = NULL;  // remove dangling pointers
    	inputB[i] = NULL;
    	output[i] = NULL;
 	}
	free(inputA);
	free(inputB);
	free(output);
}

/**
 * Sequential MMM
 */
void mmm_seq() {
	for (int i = 0; i < matSize; i++) {
        for (int j = 0; j < matSize; j++) {
			int sum = 0;
            for (int k = 0; k < matSize; k++) {
                sum += inputA[i][k] * inputB[k][j];
            }
			output[i][j] = sum;
        }
    }
}

/**
 * Parallel MMM
 */
void *mmm_par(void *args) {
	matParams *pars = (matParams*) args;
	for (int i = pars->Pstart/matSize; i < pars->Pend/matSize; i++) {
        for (int j = 0; j < matSize; j++) {
			int sum = 0;
            for (int k = 0; k < matSize; k++) {
				sum += inputA[i][k] * inputB[k][j];
            }
            output[i][j] = sum;
        }
    }
	
	return NULL;
}

/**
 * Verifies the correctness between the matrices generated by
 * the sequential run and the parallel run.
 *
 * @return the largest error between two corresponding elements
 * in the result matrices
 */
double mmm_verify() {
	//making the allocation and compare not affect timing
	double rem = rtclock();
	int **outputSeq = (int**) malloc(sizeof(int*) * matSize);
	for (int i = 0; i < matSize; i++){
		outputSeq[i] = (int*) malloc(sizeof(int) * matSize);
	}
	clockOffset += rtclock() - rem;
	for (int i = 0; i < matSize; i++) {
        for (int j = 0; j < matSize; j++) {
			int sum = 0;
            for (int k = 0; k < matSize; k++) {
                sum += inputA[i][k] * inputB[k][j];
            }
			outputSeq[i][j] = sum;
        }
    }
	rem = rtclock();
	double maxError = 0;
	for (int i =0; i < matSize;i++){
		for(int j = 0; j < matSize; j++){
			if (abs(outputSeq[i][j] - output[i][j]) > maxError){
				maxError = abs(outputSeq[i][j] - output[i][j]);
			}
		}
	}
	for (int i = 0; i < matSize; i++){
    	free(outputSeq[i]);
		outputSeq[i] = NULL;
	}
	free(outputSeq);
	clockOffset += rtclock() - rem;
	return maxError;
}
