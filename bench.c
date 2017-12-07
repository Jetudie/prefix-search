#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "bench.h"


#define SEARCH_BENCH "search-cpy-bench.txt"


double tvgetf(void)
{
    struct timespec ts;
    double sec;

    clock_gettime(CLOCK_REALTIME, &ts);
    sec = ts.tv_nsec;
    sec /= 1e9;
    sec += ts.tv_sec;

    return sec;
}

int bench(const tst_node *root,
          char **a,
          char *out_file,
          int *n,
          const int max)
{
    FILE *fp = fopen(out_file, "a+");
    if (!fp) {
        fprintf(stderr, "error: file open failed '%s'.\n", SEARCH_BENCH);
        return 1;
    }

    double t1, t2, totaltime;
    char word[]="A";
    totaltime = 0;
    for (int i=0 ; i<25 ; i++) {
        t1 = tvgetf();
        tst_search_prefix(root, word, a, n, max);
        t2 = tvgetf();
        word[0]++;
        totaltime += t2 - t1;
    }
    word[0] = 'a';
    for (int i=0 ; i<25 ; i++) {
        t1 = tvgetf();
        tst_search_prefix(root, word, a, n, max);
        t2 = tvgetf();
        word[0]++;
        totaltime += t2 - t1;
    }
    fprintf(fp, "%.6f\n",totaltime);
    fclose(fp);
    return 0;
}




