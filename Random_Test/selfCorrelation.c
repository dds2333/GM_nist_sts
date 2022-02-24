#include <math.h>
#include "include/stat_fncs.h"
#include "include/erf.h"

extern double SelfCorrelation_p_Value;
extern double SelfCorrelation_v_Value;

int SelfCorrelation(int d, int n, BitSequence *epsilon)
{
    int i = 0;
    int n_d = n - d;
    int Ad = 0;
    double V = 0.0, p_value = 0.0, sqrt2 = sqrt(2);//1.41421356237309504880

    for (i = 0; i <= n_d - 1; ++i) {
        Ad += (epsilon[i] ^ epsilon[i + d]);
    }

    V = 2 * ((double)Ad - ((double)n_d / 2)) / sqrt(n_d);
    SelfCorrelation_v_Value=V;

    p_value = nist_erfc(fabs(V) / sqrt2);
    SelfCorrelation_p_Value=p_value;

    if (p_value < ALPHA) {
        return 0;
    } else {
        return 1;
    }
}

