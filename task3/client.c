#include "mutual.h"
#include "integral.h"

void clientInt (int noThreads);

void usage (const char *argv0)
{
    fprintf (stderr, "Usage %s [N threads]\n", argv0);
}

int main (int argc, char *argv[])
{
    if (argc != 2 || strcmp(argv[1], "--help") == 0)
    {
        usage (argv[0]);
        return 0;
    }

    int noThreads = parseLong (argv[1]);

    if (noThreads > 0)
        clientInt (noThreads);

    else
        usage (argv[0]);

    return 0;
}

void clientInt (int noThreads)
{
    int                 sockFd;
    struct sockaddr_in  servAddr;

    /* find broadcast server connection */

    struct sockaddr_in peerAddr;
    struct sockaddr_in sockAddr;

    bzero (&peerAddr, sizeof peerAddr);
    bzero (&sockAddr, sizeof sockAddr);

    socklen_t peerAddrLen = sizeof (peerAddr);

    sockAddr.sin_family      = AF_INET;
    sockAddr.sin_port        = htons(CL_PORT);
    sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    
    int bcsk = socket (AF_INET, SOCK_DGRAM, 0);
    if (bcsk == -1)
    {
        perror ("socket");
        exit (1);
    }
        

    int nonZero = 1; /* setsockopt requires a nonzero *optval to turn the option on */

    if (setsockopt (bcsk, SOL_SOCKET, SO_REUSEADDR, &nonZero, sizeof(nonZero)) == -1)
    {
        perror ("setsockopt");
        exit (1);
    }

    if (bind (bcsk, (struct sockaddr *) &sockAddr, sizeof(sockAddr)) < 0)
    {
        perror ("bind");
        exit (1);
    }

    int servTcpPort = 0;

    fprintf (stderr, "Waiting for server..\n");
    recvfrom (bcsk, &servTcpPort, sizeof (int), 
                 0, (struct sockaddr *) &peerAddr, &peerAddrLen);
    fprintf (stderr, "Found server\n");
    if (servTcpPort != SERV_PORT)
    {
        exit (1);
    }

    close(bcsk);

    bzero (&servAddr, sizeof (servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_port   = htons(SERV_PORT);
    servAddr.sin_addr   = peerAddr.sin_addr;

    /* connect to server */

    if ((sockFd = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror ("socket");
        exit (1);
    }

    /* broken connection stuff */
    int keepCnt     = KEEP_CNT;
    int keepIdle    = KEEP_IDLE;
    int keepIntvl   = KEEP_INTVL;

    if (0
    || setsockopt (sockFd, IPPROTO_TCP, TCP_KEEPCNT, &keepCnt, sizeof(int))
    || setsockopt (sockFd, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int))
    || setsockopt (sockFd, IPPROTO_TCP, TCP_KEEPINTVL, &keepIntvl, sizeof(int)))
    {
        perror ("setsockopt");
        exit (1);
    }


    if (connect (sockFd, (SA *) &servAddr, sizeof(servAddr)) < 0)
    {
        perror ("connect");
        exit (1);
    }

    /* main part */

    /* send info about out NO threads*/

    struct CliInfo cliNOThreads = { .noThreads = noThreads};
    if (send (sockFd, &cliNOThreads, sizeof(cliNOThreads), MSG_NOSIGNAL)
        != sizeof(cliNOThreads))
    {
        perror ("send");
        exit (1);
    }
    
    /* receive info about noPc and noThreads */

    struct CalcInfo calcInfo;
    bzero (&calcInfo, sizeof(calcInfo));
    
    if (recv (sockFd, &calcInfo, sizeof(calcInfo), 0) < 0)
    {
        perror ("recv");
        exit (1);
    }

    /* calc integral */

    struct IntResult intRes;
    intRes.result = integrate (noThreads, 1.0, calcInfo.from, calcInfo.segm_len, calcInfo.segm_cnt);

    /* send to the server result of integral */

    send (sockFd, &intRes, sizeof(intRes), MSG_NOSIGNAL);
    fprintf (stderr, "Sent result to server\n");

    close (sockFd);

    return;
}
