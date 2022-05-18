#ifndef CACHELINE_SIZE
#define CACHELINE_SIZE 64
#warning "CACHELINE_SIZE not defined: defaults to 64 bytes"
#endif

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

double func (double x)
{
    return sin (x);
}

const double FROM  = 0;
const double TO    = M_PI * 1e5 + M_PI;
const int SEGM_CNT_DEFAULT = 1e8 * 4;

struct ThreadContext
{
    int nCPU, segmsCnt;
    double from;
    double result;
    double segmLen;
    char fool [CACHELINE_SIZE - 32];
} __attribute__((__aligned__(CACHELINE_SIZE)));

static_assert (sizeof (struct ThreadContext) == CACHELINE_SIZE, 
               "Error: sizeof (struct ThreadContext) != CACHELINE_SIZE");

void usage (const char* argv0)
{
    fprintf (stderr, "Usage: %s [N] [norm coef (optional)]\n", argv0);
}

long parseLong (const char *numStr)
{
    char *endptr;
    long ret = strtol (numStr, &endptr, 10);
    if (*endptr)
        return 0;
    return ret;
}

double parseDouble (const char *dStr)
{
    char *endptr;
    double ret = strtod (dStr, &endptr);
    if (*endptr)
        return 0.0;
    return ret;
}

void *thrRoutine (void *context)
{
    struct ThreadContext *cxt = (struct ThreadContext *) context;

    cpu_set_t cpuSet;
    pthread_t tid = pthread_self ();

    CPU_ZERO (&cpuSet);
    CPU_SET (cxt->nCPU, &cpuSet);

    if (pthread_setaffinity_np (tid, sizeof (cpu_set_t), &cpuSet) != 0)
    {
        fprintf (stderr, "Error in pthread_setaffinity_np for cpu %d\n", cxt->nCPU);
    }


    for (int segm = 0; segm < cxt->segmsCnt; segm++)
        cxt->result += cxt->segmLen * func (cxt->from + segm * cxt->segmLen);

    return NULL;
}

int main (int argc, const char **argv)
{
    if (argc != 2 && argc != 3)
    {
        usage (argv[0]);
        return 0;
    }

    int nThreads = parseLong (argv[1]);
    if (nThreads <= 0)
    {
        usage (argv[0]);
        return 0;
    }

    int segm_cnt = SEGM_CNT_DEFAULT;
    if (argc == 3)
    {
        segm_cnt = (int)(parseDouble (argv[2]) * segm_cnt);
        if (segm_cnt <= 0)
        {
            usage (argv[0]);
            return 0;
        }
    }
    const double segm_len = (TO - FROM) / segm_cnt;

    int exitcode = 0;

    int nCPUs = sysconf (_SC_NPROCESSORS_ONLN);
    int nDummyThreads = (nCPUs - nThreads % nCPUs) % nCPUs;
    int nThreadsPerCPU = (nThreads + nDummyThreads) / nCPUs;
    int nSegmsPerThread = segm_cnt / nThreads;

    pthread_t * threadIDs = (pthread_t *) calloc (nThreads + nDummyThreads, sizeof (*threadIDs));
    struct ThreadContext **arrsPerCPU = (struct ThreadContext **) calloc (nCPUs, sizeof (*arrsPerCPU));

    if (threadIDs == NULL || arrsPerCPU == NULL)
    {
        fprintf (stderr, "Memory error\n");
        exitcode = -1;
        goto cleanup;
    }

    for (int i = 0; i < nCPUs; i++)
    {
        arrsPerCPU[i] = (struct ThreadContext *) malloc (nThreadsPerCPU * sizeof (arrsPerCPU[i][0]));
        if (arrsPerCPU[i] == NULL)
        {
            fprintf (stderr, "Memory error\n");
            exitcode = -1;
            goto cleanup;
        }
    }

    double distributedTill = 0;
    int thr;
    for (thr = 0; thr < segm_cnt % nThreads; thr++)
    {
        arrsPerCPU[thr % nCPUs][thr / nCPUs].nCPU = thr % nCPUs;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].from = distributedTill;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].segmsCnt = nSegmsPerThread + 1;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].result = 0;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].segmLen = segm_len;
        

        distributedTill += (nSegmsPerThread + 1) * segm_len;
    }

    for (; thr < nThreads; thr++)
    {
        arrsPerCPU[thr % nCPUs][thr / nCPUs].nCPU = thr % nCPUs;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].from = distributedTill;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].segmsCnt = nSegmsPerThread;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].result = 0;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].segmLen = segm_len;

        distributedTill += nSegmsPerThread * segm_len;
    }

    for (; thr < nThreads + nDummyThreads; thr++)
    {
        arrsPerCPU[thr % nCPUs][thr / nCPUs].nCPU = thr % nCPUs;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].from = 0;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].segmsCnt = nSegmsPerThread;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].result = 0;
        arrsPerCPU[thr % nCPUs][thr / nCPUs].segmLen = segm_len;
    };

    for (thr = 0; thr < nThreads + nDummyThreads; thr++)
    {
        if (
        pthread_create (threadIDs + thr, NULL, thrRoutine, &(arrsPerCPU[thr % nCPUs][thr / nCPUs]))
        != 0) 
        {
            perror ("Error in pthread_create");
            exitcode = -1;
            goto cleanup;
        }

    }

    double result = 0;
    for (thr = 0; thr < nThreads; thr++)
    {
        pthread_join (threadIDs[thr], NULL);
        result += arrsPerCPU[thr % nCPUs][thr / nCPUs].result;
    }

    printf ("result = %lf\n", result);

    for (; thr < nThreads + nDummyThreads; thr++)
    {
        pthread_join (threadIDs[thr], NULL);
    }

    
    cleanup:

    free (threadIDs);
    if (arrsPerCPU != NULL)
    {
        for (int i = 0; i < nCPUs; i++)
            free (arrsPerCPU[i]);
        free (arrsPerCPU);
    }

    return exitcode;
}
