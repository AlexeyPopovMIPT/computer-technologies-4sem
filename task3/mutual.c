#include "mutual.h"

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

int Accept(int fd, struct sockaddr *sa, socklen_t *salenptr)
{
	int n;

	if ( (n = accept(fd, sa, salenptr)) < 0) 
    {
		if (errno == EAGAIN)
            return -2;
		else
		{
			perror ("accept");
			return -1;
		}
			
	}
	return n;
}

int Listen(int fd, int backlog)
{
	char	*ptr;

		/*4can override 2nd argument with environment variable */
	if ( (ptr = getenv("LISTENQ")) != NULL)
		backlog = atoi(ptr);

	if (listen(fd, backlog) < 0)
		return -1;

	return 0;
}


