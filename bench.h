#ifndef BENCH_H
#define BENCH_H

#include "tst.h"
double tvgetf();
int  bench(const tst_node *root,
           char **a,
           char *out_file,
           int *n,
           const int max);
#endif
