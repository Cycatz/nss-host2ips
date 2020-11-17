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
    enum nss_status (*get_host)(const char *, struct hostent *, char *, size_t, int *, int *);
    enum nss_status ret; 
    void *handle;

    struct hostent result_buf;
    char buf[1000];
    int errnop;
    int h_errnop;

    handle = dlopen("libnss_files-2.32.so", RTLD_LAZY);

    if (!handle) {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();

    get_host = (enum nss_status (*)(const char *, struct hostent *, char *, size_t, int *, int *)) dlsym(handle, "_nss_files_gethostbyname_r");

    error = dlerror();
    if (error != NULL) {
        fprintf(stderr, "error: %s\n", error);
        exit(EXIT_FAILURE);
    }
   
    ret = (*get_host)("localhost", &result_buf, buf, 1000, &errnop, &h_errnop);

	char ip[16];
    inet_ntop(AF_INET, result_buf.h_addr_list[0], ip, sizeof(ip));
    printf("%d %d %s\n", ret, result_buf.h_length, ip);

}
