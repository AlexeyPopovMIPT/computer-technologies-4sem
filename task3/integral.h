#ifndef __INTEGRAL_H__
#define __INTEGRAL_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <assert.h>

double integrate (int nThreads, double normKoef, double from, double segmLen, int segmCnt);

#endif // !__INTEGRAL_H__
