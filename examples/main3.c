#include <stdio.h>
#include <stdlib.h>
#include <nss.h>
#include <netdb.h>
#include <error.h>
#include <dlfcn.h>
#include <arpa/inet.h>

int main()
{

    char *error; 
    enum nss_status (*get_host)(const char *, int, struct hostent *, char *, size_t, struct hostent **, int *);
    enum nss_status nss_ret; 
    void *handle;

    struct hostent ret;
    struct hostent *ret_buf[1];

    char buf[1000];
    int errnop;
    int h_errnop;

    // handle = dlopen("libnss_files-2.32.so", RTLD_LAZY);
    handle = dlopen("libnss_myfiles.so.2", RTLD_LAZY);

    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();

    get_host = (enum nss_status (*)(const char *, int, struct hostent *, char *, size_t, struct hostent **, int *)) dlsym(handle, "_nss_myfiles_gethostbyname2_r");

    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "error: %s\n", error);
        exit(EXIT_FAILURE);
    }
   
    nss_ret = (*get_host)("google.com", AF_INET, &ret, buf, 1000, ret_buf, &h_errnop);

	char ip[16];
    inet_ntop(AF_INET, ret.h_addr_list[0], ip, sizeof(ip));
    printf("%s\n", ip);
}
