#ifndef LIBNSS_HOST2IPS_H
#define LIBNSS_HOST2IPS_H
#include <stdio.h>
#include <stdlib.h>
#include <nss.h>
#include <netdb.h>
#include <error.h>
#include <arpa/inet.h>

// #define ALIGNED(i, t) ((i) + ((sizeof(t) - ((i) % sizeof(t))) % sizeof(t))) 
#define ALIGNED(i, t) ((i) + (sizeof(t) - 1)) & (-sizeof(t))

#define ALIGN(p, t) ((char *)(((((long)(p) - 1) / sizeof(t)) + 1) * sizeof(t)))
#define SP(p, t, n) (ALIGN(p, t) + (n) * sizeof(t))



#endif /* LIBNSS_HOST2IPS_H */
