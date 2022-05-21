#include "integral.h"
#include "mutual.h"

#ifndef CACHELINE_SIZE
#define CACHELINE_SIZE 64
#warning "CACHELINE_SIZE not defined: defaults to 64 bytes"
#endif

const double FROM  = 0;
const double TO    = M_PI * 1e5 + M_PI;
const int SEGM_CNT_DEFAULT = 1e8 * 4;

double func (double x)
{
    return sin (x);
}

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

void *thrRoutine (void *context)
{
    struct ThreadContext *cxt = (struct ThreadContext *) context;
    fprintf (stderr, "Thread: integrate from %lf to %lf\n", cxt->from, cxt->from + cxt->segmLen * cxt->segmsCnt);

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

double integrate (int nThreads, double normKoef, double from, double segm_len, int segm_cnt)
{
    fprintf (stderr, "Client: integrate from %lf %d segments\n", from, segm_cnt);

    double result = 0;

    int nCPUs = sysconf (_SC_NPROCESSORS_ONLN);
    int nDummyThreads = (nCPUs - nThreads % nCPUs) % nCPUs;
    int nThreadsPerCPU = (nThreads + nDummyThreads) / nCPUs;
    int nSegmsPerThread = segm_cnt / nThreads;

    pthread_t * threadIDs = (pthread_t *) calloc (nThreads + nDummyThreads, sizeof (*threadIDs));
    struct ThreadContext **arrsPerCPU = (struct ThreadContext **) calloc (nCPUs, sizeof (*arrsPerCPU));

    if (threadIDs == NULL || arrsPerCPU == NULL)
    {
        fprintf (stderr, "Memory error\n");
        result = -1;
        goto cleanup;
    }

    for (int i = 0; i < nCPUs; i++)
    {
        arrsPerCPU[i] = (struct ThreadContext *) malloc (nThreadsPerCPU * sizeof (arrsPerCPU[i][0]));
        if (arrsPerCPU[i] == NULL)
        {
            fprintf (stderr, "Memory error\n");
            result = -1;
            goto cleanup;
        }
    }

    double distributedTill = from;
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
            result = -1;
            goto cleanup;
        }

    }

    for (thr = 0; thr < nThreads; thr++)
    {
        pthread_join (threadIDs[thr], NULL);
        result += arrsPerCPU[thr % nCPUs][thr / nCPUs].result;
    }

#   ifdef PRINT_RESULT
    printf ("result = %lf\n", result);
#   endif

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

    return result;
}
