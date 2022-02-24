﻿#include <stdio.h>
#include <math.h>
#include <string.h>
#include "include/externs.h"
#include "include/cephes.h"

extern double BlockFrequency_p_Value;
extern double BlockFrequency_v_Value;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                    B L O C K  F R E Q U E N C Y  T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int BlockFrequency(int M, int n, BitSequence *epsilon)
{
    int		i, j, N, blockSum;
    double	p_value, sum, pi, v, chi_squared;

    N = n/M; 		/* # OF SUBSTRING BLOCKS      */
    sum = 0.0;

    for ( i=0; i<N; i++ ) {
        blockSum = 0;
        for ( j=0; j<M; j++ )
            blockSum += epsilon[j+i*M];
        pi = (double)blockSum/(double)M;
        v = pi - 0.5;
        sum += v*v;
    }
    chi_squared = 4.0 * M * sum;
    BlockFrequency_v_Value=chi_squared;
    p_value = cephes_igamc(N/2.0, chi_squared/2.0);
    BlockFrequency_p_Value=p_value;
//	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t\tBLOCK FREQUENCY TEST\n");
//	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t---------------------------------------------\n");
//	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\tCOMPUTATIONAL INFORMATION:\n");
//	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t---------------------------------------------\n");
//	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t(a) Chi^2           = %f\n", chi_squared);
//	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t(b) # of substrings = %d\n", N);
//	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t(c) block length    = %d\n", M);
//	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t(d) Note: %d bits were discarded.\n", n % M);
//	fprintf(stats[TEST_BLOCK_FREQUENCY], "\t\t---------------------------------------------\n");
//
//	fprintf(stats[TEST_BLOCK_FREQUENCY], "%s\t\tp_value = %f\n\n", p_value < ALPHA ? "FAILURE" : "SUCCESS", p_value); fflush(stats[TEST_BLOCK_FREQUENCY]);
//	fprintf(results[TEST_BLOCK_FREQUENCY], "%f\n", p_value); fflush(results[TEST_BLOCK_FREQUENCY]);
    if (p_value < ALPHA) {
        return 0;
    } else {
        return 1;
    }
}
