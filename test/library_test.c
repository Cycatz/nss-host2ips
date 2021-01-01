#include <arpa/inet.h>
#include <dlfcn.h>
#include <errno.h>
#include <netdb.h>
#include <nss.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef LIB_NAME
#define LIB_NAME "src/libnss_host2ips-2.31.so"
#endif

#define my_malloc(x, size)      \
    do {                        \
        (x) = malloc((size));   \
        if ((x) == NULL) {      \
            perror("malloc");   \
            exit(EXIT_FAILURE); \
        }                       \
    } while (0)

#define my_realloc(x, size)         \
    do {                            \
        (x) = realloc((x), (size)); \
        if ((x) == NULL) {          \
            perror("realloc");      \
            exit(EXIT_FAILURE);     \
        }                           \
    } while (0)


static void init_dl_handle(void);
static void *load_func(const char *);
static int test_nss_host2ips_gethostbyname(const char *);
// int test_nss_host2ips_gethostbyname2(const char *);
static void close_dl_handle(void);


static void *dl_handle;

static void init_dl_handle(void)
{
    dl_handle = dlopen(LIB_NAME, RTLD_LAZY);
    if (!dl_handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

static void *load_func(const char *func_name)
{
    char *error_msg;
    void *func;
    dlerror();

    if (!dl_handle)
        init_dl_handle();

    func = dlsym(dl_handle, func_name);
    error_msg = dlerror();
    if (error_msg != NULL) {
        fprintf(stderr, "error: %s\n", error_msg);
        exit(EXIT_FAILURE);
    }

    return func;
}

static int test_nss_host2ips_gethostbyname(const char *host_name)
{
    printf("%s\n", LIB_NAME); 
    enum nss_status (*nss_host2ips_gethostbyname)(
        const char *, struct hostent *, char *, size_t, int *, int *);
    enum nss_status nss_ret;

    struct hostent *hostbuf;
    size_t hstbuflen;
    char *tmphstbuf;
    int err;
    int herr;

    char addr[16];

    my_malloc(hostbuf, sizeof(struct hostent));
    hstbuflen = 1024;
    my_malloc(tmphstbuf, hstbuflen);

    nss_host2ips_gethostbyname = (enum nss_status(*)(
        const char *, struct hostent *, char *, size_t, int *,
        int *)) load_func("_nss_host2ips_gethostbyname_r");

    while (1) {
        nss_ret = (*nss_host2ips_gethostbyname)(host_name, hostbuf, tmphstbuf,
                                                hstbuflen, &err, &herr);
        if (err == ERANGE) {
            hstbuflen *= 2;
            my_realloc(tmphstbuf, hstbuflen);
        } else {
            break;
        }
    }
    if (nss_ret != NSS_STATUS_SUCCESS || err || herr) {
        switch (nss_ret) {
        case NSS_STATUS_NOTFOUND:
            fprintf(stderr, "Error: host not found\n");
            break;
        case NSS_STATUS_TRYAGAIN:
            fprintf(stderr, "Error: try again\n");
            break;
        case NSS_STATUS_UNAVAIL:
            fprintf(stderr, "Error: service is unavailable\n");
            break;
        default:
            fprintf(stderr, "Error: service is unavailable\n");
            break;
        }
        exit(EXIT_FAILURE);
    }
    inet_ntop(AF_INET, hostbuf->h_addr, addr, sizeof(addr));
    printf("%s\n", addr);

    free(hostbuf);
    free(tmphstbuf);

    return 0;
}


static void close_dl_handle(void)
{
    if (dlclose(dl_handle)) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}
