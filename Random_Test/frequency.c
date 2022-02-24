﻿#include <stdio.h>
#include <math.h>
#include <string.h>
#include "include/externs.h"
#include "include/erf.h"
#include "global.h"

extern double Frequency_p_Value;
extern double Frequency_v_Value;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
                          F R E Q U E N C Y  T E S T
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

int Frequency(int n, BitSequence *epsilon)
{
    int		i;
    double	f, s_obs, p_value, sum, sqrt2 = 1.41421356237309504880;

    sum = 0.0;
    for ( i=0; i<n; i++ )
        sum += 2*(int)epsilon[i]-1;
    s_obs = fabs(sum)/sqrt(n);
    f = s_obs/sqrt2;
    p_value = nist_erfc(f);
    Frequency_p_Value=p_value;
    Frequency_v_Value=s_obs;

//	fprintf(stats[TEST_FREQUENCY], "\t\t\t      FREQUENCY TEST\n");
//	fprintf(stats[TEST_FREQUENCY], "\t\t---------------------------------------------\n");
//	fprintf(stats[TEST_FREQUENCY], "\t\tCOMPUTATIONAL INFORMATION:\n");
//	fprintf(stats[TEST_FREQUENCY], "\t\t---------------------------------------------\n");
//	fprintf(stats[TEST_FREQUENCY], "\t\t(a) The nth partial sum = %d\n", (int)sum);
//	fprintf(stats[TEST_FREQUENCY], "\t\t(b) S_n/n               = %f\n", sum/n);
//	fprintf(stats[TEST_FREQUENCY], "\t\t---------------------------------------------\n");

//	fprintf(stats[TEST_FREQUENCY], "%s\t\tp_value = %f\n\n", p_value < ALPHA ? "FAILURE" : "SUCCESS", p_value); fflush(stats[TEST_FREQUENCY]);
//	fprintf(results[TEST_FREQUENCY], "%f\n", p_value); fflush(results[TEST_FREQUENCY]);
    if (p_value < ALPHA) {
        return 0;
    } else {
        return 1;
    }
}
