#include <stdio.h>
#include <stdlib.h>
#include <nss.h>
#include <netdb.h>
#include <error.h>
#include <dlfcn.h>
#include <arpa/inet.h>

enum nss_status
_nss_myfiles_gethostbyname_r (const char *name,
                                struct hostent *result_buf,
                                char *buf,
                                size_t buflen,
                                int *errnop,
                                int *h_errnop) {

    return NSS_STATUS_SUCCESS;
}


enum nss_status
_nss_myfiles_gethostbyname2_r (const char *name, 
                                int af,
                                struct hostent *ret,
                                char *buf,
                                size_t buflen,
                                struct hostent **result,
                                int *h_errnop) {


	if (strcmp(name, "TUTU") == 0) {
		static char *addrs[2] = {"1400", NULL};
		static char *alias[2]= {"aaaa", NULL};

		ret->h_name     = "test";
		ret->h_aliases  = alias; 
		ret->h_addrtype = AF_INET;
		ret->h_length   = 4; 
		ret->h_addr_list = addrs;  
		*h_errnop = 0;

		*result = ret; 

		return NSS_STATUS_SUCCESS;
	} else {
		return NSS_STATUS_NOTFOUND;
	}
} 
