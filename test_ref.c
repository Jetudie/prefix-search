#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "tst.h"
#include "bench.h"
/** constants insert, delete, max word(s) & stack nodes */
enum { INS, DEL, WRDMAX = 256, STKMAX = 512, LMAX = 1024 };
#define REF INS
#define CPY DEL
#define MemoryPoolSize 10000000

/* timing helper function */
/*
static double tvgetf(void)
{
    struct timespec ts;
    double sec;

    clock_gettime(CLOCK_REALTIME, &ts);
    sec = ts.tv_nsec;
    sec /= 1e9;
    sec += ts.tv_sec;

    return sec;
}
*/

/* simple trim '\n' from end of buffer filled by fgets */
static void rmcrlf(char *s)
{
    size_t len = strlen(s);
    if (len && s[len - 1] == '\n')
        s[--len] = 0;
}

#define IN_FILE "cities.txt"
#define TST_BENCH "tst_bench_ref.txt"
#define SEARCH_BENCH "search_bench_ref.txt"


int main(int argc, char **argv)
{
    char *pptr = (char *)malloc(MemoryPoolSize * sizeof(char));
    char *pTop = pptr;

    char word[WRDMAX] = "";
    char *sgl[LMAX] = {NULL};
    tst_node *root = NULL, *res = NULL;
    int rtn = 0, idx = 0, sidx = 0;
    FILE *fp = fopen(IN_FILE, "r");
    double t1, t2;

    if (!fp) { /* prompt, open, validate file for reading */
        fprintf(stderr, "error: file open failed '%s'.\n", argv[1]);
        return 1;
    }

    t1 = tvgetf();
    while ((rtn = fscanf(fp, "%s", pTop)) != EOF) {
        char *p = pTop;
        if (!tst_ins_del(&root, &p, INS, REF)) {
            fprintf(stderr, "error: memory exhausted, tst_insert.\n");
            fclose(fp);
            return 1;
        }
        idx++;
        pTop += (strlen(pTop)+1);
    }
    t2 = tvgetf();

    fclose(fp);
    printf("ternary_tree, loaded %d words in %.6f sec\n", idx, t2 - t1);

    if (argc==2 && strcmp(argv[1], "--bench") == 0) {
        fp = fopen (TST_BENCH,"a+");
        if (!fp) {
            fprintf(stderr, "error: file open failed '%s'.\n", TST_BENCH);
            return 1;
        }
        fprintf(fp, "%.6f\n",t2 - t1);
        fclose(fp);
        int status = bench(root,sgl,SEARCH_BENCH, &sidx, LMAX);
        return status;
    }



    for (;;) {
        printf(
            "\nCommands:\n"
            " a  add word to the tree\n"
            " f  find word in tree\n"
            " s  search words matching prefix\n"
            " d  delete word from the tree\n"
            " q  quit, freeing all data\n\n"
            "choice: ");
        fgets(word, sizeof word, stdin);

        switch (*word) {
                char *p = NULL;
            case 'a':
                printf("enter word to add: ");
                if (!fgets(pTop, sizeof word, stdin)) {
                    fprintf(stderr, "error: insufficient input.\n");
                    break;
                }
                rmcrlf(pTop);
                p = pTop;
                t1 = tvgetf();
                res = tst_ins_del(&root, &p, INS, REF);
                t2 = tvgetf();
                if (res) {
                    idx++;
                    pTop += (strlen(pTop) + 1);
                    printf("  %s - inserted in %.6f sec. (%d words in tree)\n",
                           (char *) res, t2 - t1, idx);
                } else
                    printf("  %s - already exists in list.\n", (char *) res);
                break;
            case 'f':
                printf("find word in tree: ");
                if (!fgets(word, sizeof word, stdin)) {
                    fprintf(stderr, "error: insufficient input.\n");
                    break;
                }
                rmcrlf(word);
                t1 = tvgetf();
                res = tst_search(root, word);
                t2 = tvgetf();
                if (res)
                    printf("  found %s in %.6f sec.\n", (char *) res, t2 - t1);
                else
                    printf("  %s not found.\n", word);
                break;
            case 's':
                printf("find words matching prefix (at least 1 char): ");
                if (!fgets(word, sizeof word, stdin)) {
                    fprintf(stderr, "error: insufficient input.\n");
                    break;
                }
                rmcrlf(word);
                t1 = tvgetf();
                res = tst_search_prefix(root, word, sgl, &sidx, LMAX);
                t2 = tvgetf();
                if (res) {
                    printf("  %s - searched prefix in %.6f sec\n\n", word, t2 - t1);
                    for (int i = 0; i < sidx; i++)
                        printf("suggest[%d] : %s\n", i, sgl[i]);
                } else
                    printf("  %s - not found\n", word);
                break;
            case 'd':
                printf("enter word to del: ");
                if (!fgets(pTop, sizeof word, stdin)) {
                    fprintf(stderr, "error: insufficient input.\n");
                    break;
                }
                rmcrlf(pTop);
                p = pTop;
                printf("  deleting %s\n", pTop);
                t1 = tvgetf();
                res = tst_ins_del(&root, &p, DEL, REF);
                t2 = tvgetf();
                if (res)
                    printf("  delete failed.\n");
                else {
                    printf("  deleted %s in %.6f sec\n", word, t2 - t1);
                    idx--;
                    pTop -= (strlen(pTop) + 1);
                }
                break;
            case 'q':
                tst_free(root);
                free(pptr);
                return 0;
                break;
            default:
                fprintf(stderr, "error: invalid selection.\n");
                break;
        }
    }

    return 0;
}
