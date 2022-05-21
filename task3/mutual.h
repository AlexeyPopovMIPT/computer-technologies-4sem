#ifndef	__MUTUAL_H__
#define	__MUTUAL_H__


#include	<sys/types.h>	/* basic system data types */
#include	<sys/socket.h>	/* basic socket definitions */

#include	<time.h>		/* old system? */

#include	<netinet/in.h>	/* sockaddr_in{} and other Internet defns */
#include	<arpa/inet.h>	/* inet(3) functions */
#include	<errno.h>
#include	<fcntl.h>		/* for nonblocking */
#include	<netdb.h>
#include	<signal.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<sys/stat.h>	/* for S_xxx file mode constants */
#include	<sys/uio.h>		/* for iovec{} and readv/writev */
#include	<unistd.h>
#include	<sys/wait.h>
#include	<sys/un.h>		/* for Unix domain sockets */

#include    <sys/time.h>
#include    <math.h>

#include    <netinet/tcp.h>



/* Define bzero() as a macro if it's not in standard C library. */
#ifndef	HAVE_BZERO
#define	bzero(ptr,n)		memset(ptr, 0, n)
#endif


/* POSIX renames "Unix domain" as "local IPC."
   Not all systems define AF_LOCAL and PF_LOCAL (yet). */
#ifndef	AF_LOCAL
#define AF_LOCAL	AF_UNIX
#endif
#ifndef	PF_LOCAL
#define PF_LOCAL	PF_UNIX
#endif


/* Following could be derived from SOMAXCONN in <sys/socket.h>, but many
   kernels still #define it as 5, while actually supporting many more */
#define	LISTENQ		1024	/* 2nd argument to listen() */

/* timeout constants */

#define ACCEPT_TIMEOUT_SEC  0       /* in sec  waiting time in accept */
#define ACCEPT_TIMEOUT_USEC 100000  /* in usec -//-                                                      */

#define CALC_TIMEOUT_SEC  3000000   /* in sec  allotted time for calculating the integral for the client    */
#define CALC_TIMEOUT_USEC 0         /* in usec -//-                                                         */

/* broken connection stuff */

#define KEEP_CNT   5 /* The maximum number of keepalive probes
                        TCP should send before dropping the connection. */

#define KEEP_IDLE  2 /* The time (in seconds) the connection needs to
                        remain idle before TCP starts sending keepalive probes,
                        if the socket option SO_KEEPALIVE has been set on this socket.*/

#define KEEP_INTVL 1 /* The time (in seconds) between individual keepalive probes. */

/* Define some port number that can we use*/
#define	SERV_PORT		 9878			/* server port */
#define	SERV_PORT_STR	"9878"			/* string version of server port */

#define CL_PORT          9888           /* client port  */
#define CL_PORT_STR     "9888"          /* string version of client port */



/* structures */

struct CalcInfo
{
    double from;
    double segm_cnt;
    double segm_len;
};

struct CliInfo
{
    size_t noThreads;       /* NO client threads*/
};

struct IntResult
{
    double result;          /* client integral result */
};


/* cmd line number parser ( returns <long> number according to numString with error handling) */
long    parseLong (const char *numStr);
double  parseDouble (const char *dStr);


#define	SA struct sockaddr


int	Accept (int fd, struct sockaddr *sa, socklen_t *salenptr);
int Listen (int fd, int backlog);

#endif // ! __MUTUAL_H__