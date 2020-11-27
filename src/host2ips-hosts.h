#ifndef NSS_HOST2IPS_HOSTS_H
#define NSS_HOST2IPS_HOSTS_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <nss.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "host2ips-parser.h"

// #define ALIGNED(i, t) ((i) + ((sizeof(t) - ((i) % sizeof(t))) % sizeof(t)))
// #define ALIGNED(i, t) ((i) + (sizeof(t) - 1)) & (-sizeof(t))

#define NSS_HOST2IPS_ALIGN(p, t) \
    ((char *) (((((long) (p) - 1) / sizeof(t)) + 1) * sizeof(t)))
#define NSS_HOST2IPS_SP(p, t, n) (NSS_HOST2IPS_ALIGN(p, t) + (n) * sizeof(t))
#define NSS_HOST2IPS_LOCK(mutex)    \
    do {                            \
        pthread_mutex_lock(mutex);  \
    } while (0);

#define NSS_HOST2IPS_UNLOCK(mutex)     \
    do {                               \
        pthread_mutex_unlock(mutex);   \
    } while (0);


static const char *NSS_HOST2IPS_CONFIG_FILE_NAME = "/etc/host2ips";

#endif /* NSS_HOST2IPS_HOSTS_H */
