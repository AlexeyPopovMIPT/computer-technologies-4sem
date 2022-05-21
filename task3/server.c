#include "mutual.h"
#include <time.h>

const double FROM  = 0;
const double TO    = M_PI * 1e5 + M_PI;
const int SEGM_CNT_DEFAULT = 1e8 * 4;

int serverInt (int noPc, int segm_cnt);

void usage (const char *argv0)
{
    fprintf (stderr, "Usage: %s [N PCs]\n", argv0);
}

int main (int argc, char *argv[])
{
    if (argc != 2 && argc != 3 || strcmp (argv[1], "--help") == 0)
    {
        usage (argv[0]);
        return 0;
    }

    int noPc = parseLong (argv[1]);
    if (noPc <= 0)
    {
        usage (argv[0]);
        return 0;
    }

    int segm_cnt = SEGM_CNT_DEFAULT;
    if (argc == 3)
        segm_cnt = (int) (parseDouble(argv[2]) * segm_cnt);

    serverInt(noPc, segm_cnt);

    return 0;
}

int serverInt (int noPc, int segm_cnt)
{
    int                 listenFd = -1;
    int                 nonZero = 1;
    struct sockaddr_in  servAddr;
    int                 exitcode = 0;
    int                 broadcastFd = -1;
    int                 *clientFds = NULL;
    size_t              *clientThreadsCnt = NULL;
    struct sockaddr_in  *servAddrs = NULL;
    double              result = 0.0;
    struct timeval      acceptTimeout = {
            .tv_sec  = ACCEPT_TIMEOUT_SEC,
            .tv_usec = ACCEPT_TIMEOUT_USEC
    };
    struct sockaddr_in bcAddr = 
    {
        .sin_family = AF_INET,
        .sin_port = htons(CL_PORT),
        .sin_addr.s_addr = INADDR_BROADCAST
    };
    
    listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd == -1)
    {
        exitcode = 1;
        goto cleanup;
    }

    if (setsockopt(listenFd, SOL_SOCKET, SO_REUSEADDR, &nonZero, sizeof(nonZero)) == -1)
    {
        exitcode = 1;
        goto cleanup;
    }

    if (0
    || setsockopt(listenFd, SOL_SOCKET, SO_RCVTIMEO, &acceptTimeout, sizeof(acceptTimeout)) == -1
    || setsockopt(listenFd, SOL_SOCKET, SO_KEEPALIVE, &nonZero, sizeof(nonZero)) == -1)
    {
        exitcode = 1;
        goto cleanup;
    }


    /* broken connection stuff */

    /* IPPROTO_TCP family: TCP_KEEPIDLE TCP_KEEPINTVL TCP_KEEPINTVL */

    /*  Now, let's assume connection has been broken due to network failure.
        Network failure can be produced by removing LAN cable at the client.
        Now, server will close the connection after "TCP_KEEPINTVL * TCP_KEEPCNT". */

    /*  In Linux OS,there is a relation between TCP_KEEPIDLE and 
        TCP_KEEPINTVL. TCP_KEEPIDLE can not be lower than TCP_KEEPINTVL.*/

    {
        int keepCnt     = KEEP_CNT;
        int keepIdle    = KEEP_IDLE;
        int keepIntvl   = KEEP_INTVL;

        if (0
        || setsockopt(listenFd, IPPROTO_TCP, TCP_KEEPCNT, &keepCnt, sizeof(int)) == -1
        || setsockopt(listenFd, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int)) == -1
        || setsockopt(listenFd, IPPROTO_TCP, TCP_KEEPINTVL, &keepIntvl, sizeof(int)) == -1)
        {
            exitcode = 1;
            goto cleanup;
        }
    }

    memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family      = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port        = htons(SERV_PORT);
    

    if (bind(listenFd, (SA *) &servAddr, sizeof(servAddr)) < 0)
    {
        exitcode = 1;
        goto cleanup;
    }

    if (Listen(listenFd, LISTENQ) == -1)
    {
        exitcode = 1;
        goto cleanup;
    }

    /* broadcast stuff */

    broadcastFd = socket(AF_INET, SOCK_DGRAM, 0);

    if (broadcastFd < 0)
    {
        exitcode = 1;
        goto cleanup;
    }
    
    if (0
    || setsockopt(broadcastFd, SOL_SOCKET, SO_BROADCAST, &nonZero, sizeof(nonZero)) == -1
    || setsockopt(broadcastFd, SOL_SOCKET, SO_REUSEADDR, &nonZero, sizeof(nonZero)) == -1)
    {
        exitcode = 1;
        goto cleanup;
    }

    if (bind(broadcastFd, (struct sockaddr *) &bcAddr, sizeof(bcAddr)) < 0)
    {
        exitcode = 1;
        goto cleanup;
    }

    /* broadcast */
    {
        int bcMsg = SERV_PORT;
        if (sendto(broadcastFd, &bcMsg, sizeof(bcMsg), 0, (struct sockaddr *) &bcAddr, sizeof(bcAddr)) != sizeof(bcMsg))
        {
            exitcode = 1;
            goto cleanup;
        }      
        close (broadcastFd); broadcastFd = -1;
    }

    /*  end of broadcast  |
        start of listening  */

    clientFds = (int *) calloc (noPc, sizeof (int));
    const socklen_t addrLen = sizeof(struct sockaddr_in);
    
    servAddrs = (struct sockaddr_in *) malloc (noPc * addrLen);

    if (clientFds == NULL || servAddrs == NULL)
    {
        exitcode = 1;
        goto cleanup;
    }

    /* waiting for all clients */

    int iClient;

    for (iClient = 0; iClient < noPc; ++iClient)
    {
	    clientFds[iClient] = accept(listenFd, NULL, NULL);
        
        if (clientFds[iClient] < 0) /* EAGAIN == Resource temporarily unavailable */
        {
            if (errno == EAGAIN)
                break;
            
            exitcode = 1;
            goto cleanup;
        }

        struct timeval connFdTimeout = {
            .tv_sec  = CALC_TIMEOUT_SEC,
            .tv_usec = CALC_TIMEOUT_USEC
        };

        if (setsockopt(clientFds[iClient], SOL_SOCKET, SO_RCVTIMEO, &connFdTimeout, sizeof(connFdTimeout)) < 0)
        {
            exitcode = 1;
            goto cleanup;
        }

        servAddrs[iClient].sin_family = AF_INET;
        servAddrs[iClient].sin_port   = htons(CL_PORT);
    }

    int foundClientsCount = iClient;
    if (foundClientsCount == 0)
    {
        fprintf(stderr, "Error: no clients found\n");
        exitcode = 1;
        goto cleanup;
    }

    fprintf(stderr, "%d clients found\n", foundClientsCount);

    /* from now time can be counted */

    struct timespec begin, end;
    clock_gettime (CLOCK_MONOTONIC, &begin);

    /* got clients NOthreads to optimal divition of the work */

    size_t totalThreads = 0;
    
    clientThreadsCnt = (size_t *) malloc (foundClientsCount * sizeof (size_t));
    if (clientThreadsCnt == NULL)
    {
        exitcode = 1;
        goto cleanup;
    }

    for (int iClient = 0; iClient < foundClientsCount; ++iClient)
    {
        struct CliInfo iClientInfo;

        ssize_t recvRet = recv(clientFds[iClient], &iClientInfo, sizeof(iClientInfo), 0);

        if (recvRet != sizeof(iClientInfo))
        {
            fprintf(stderr, "Client disconnected\n");
            
            goto cleanup;
        }

        clientThreadsCnt[iClient] = iClientInfo.noThreads;
        totalThreads += iClientInfo.noThreads;
    }

    /* send needed info for calc */

    double lastEnd = FROM;
    double segm_len = (TO - FROM) / segm_cnt;

    for (int iClient = 0; iClient < foundClientsCount/*noPc*/; ++iClient)
    {
        double intLength = (double)(TO - FROM) * clientThreadsCnt[iClient]
                        / totalThreads;
        struct CalcInfo calcInfo = {
            .from = lastEnd,
            .segm_cnt = round (clientThreadsCnt[iClient] * segm_cnt / totalThreads),
            .segm_len = segm_len
        };
        lastEnd += segm_len * calcInfo.segm_cnt;
        totalThreads -= clientThreadsCnt[iClient];
        segm_cnt -= calcInfo.segm_cnt;

        if (sendto(clientFds[iClient], &calcInfo, sizeof(calcInfo), 0,
                              (struct sockaddr *)(&servAddrs[iClient]), addrLen)
            != sizeof(calcInfo))
        {
            perror ("sendto");
            goto cleanup;
        }
    }

    /* get results */

    for (int iClient = 0; iClient < foundClientsCount/*noPc*/; iClient++)
    {
        struct IntResult intRes;

        ssize_t recvRet = recv(clientFds[iClient], &intRes, sizeof(intRes), 0);

        if (recvRet != sizeof(intRes))
        {
            fprintf(stderr, "Client disconnected\n");
            
            goto cleanup;
        }
    
        result += intRes.result;
        close(clientFds[iClient]);
    }

    clock_gettime (CLOCK_MONOTONIC, &end);

    printf ("result = %lf\n", result);
    printf ("%g seconds\n", (end.tv_sec - begin.tv_sec) + 1e-9 * (end.tv_nsec - begin.tv_nsec));

cleanup:
    if (broadcastFd >= 0) close (broadcastFd);
    if (listenFd >= 0) close(listenFd);
    if (clientFds != NULL)
    {
        for (int iClient = 0; iClient < noPc; iClient++)
            if (clientFds[iClient] != 0)
                close(clientFds[iClient]);
        free (clientFds);
    }
    
    free (servAddrs);
    free (clientThreadsCnt);

    return exitcode;
    
}
