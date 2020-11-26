#include "libnss_host2ips.h"

static const char *CONFIG_FILE = "/etc/host2ips";  
static HOST_LIST *host_list = NULL;


static int is_reachable (struct in_addr addr) {
    FILE *in;
    char command[512]; 
    char *addr_str = NULL;   


    if (!(addr_str = inet_ntoa(addr)))  {
        return -1;
    }

    snprintf(command, sizeof(command), "ping -c 1 %s 2>/dev/null", addr_str);

    if (!(in = popen(command, "r"))) {
        return -1;
    }
    
    int exit_code = pclose(in);
    if (exit_code != 0)  {
        return -1;
    } else {
        return 1;
    }
}

static int is_rule_matched (HOST_INFO *host_info, struct in_addr *addr) {

    for (; host_info != NULL; host_info = host_info->info_next) {
        if(is_reachable(host_info->addr)) {
            *addr = host_info->addr; 
            return 1;      
        }
    }

    return -1;
}


enum nss_status
_nss_host2ips_sethostent (void) {
    if (initialize_host_list(&host_list) != 0)  {
        return NSS_STATUS_UNAVAIL;
    }

    if (parse_config_file(CONFIG_FILE, host_list) != 0) {
        return NSS_STATUS_UNAVAIL;
    }

    return NSS_STATUS_SUCCES;
}


enum nss_status
_nss_host2ips_gethostbyname_r (const char *name,
                                struct hostent *result_buf,
                                char *buf,
                                size_t buflen,
                                int *errnop,
                                int *h_errnop) {

    enum nss_status ret = NSS_STATUS_SUCCESS; 
    int len_hostname = strlen(name), idx = 0;
    HOST *host;
    HOST_INFO *host_info;
    struct if_addr addr; 
    char *host_name, *addr_ptr; 


    if (host_list == NULL) {
        ret = _nss_host2ips_sethostent(0);
    }

    if (ret != NSS_STATUS_SUCCESS) {
        *erronop = ENOENT; 
        *h_errnop = NO_RECOVERY;
        return ret;  
    }
   
    for (host = host_list->head; host != NULL; host = host->host_next) {
        if (strlen(host->name) == strlen(name)) {
            if (strncasecmp(host->name, name, strlen(name)) == 0) {
                if (is_rule_matched(host_info->info_head, &addr)) {

                    if (SP(SP(SP(buf, char, len_hostname + 1), addr, 1), char *, 3) > buf + buf_size) {
                        *errnop = ERANGE;
                        *h_errnop = NO_RECOVERY;
                        return NSS_STATUS_TRYAGAIN; 
                    }
                    
                    strncpy(buf, name, len_hostname); 
                    host_name = buf;
                    buf = ALIGN(buf + len_hostname + 1, addr);
                    *((struct in_addr *) buf) = addr;
                    addr_ptr = buf;
                    buf = ALIGN(buf + sizeof(addr), char *);
                    ((char **) buf)[0] = addr_ptr;
                    ((char **) buf)[1] = NULL;
                    ((char **) buf)[2] = NULL;

                    
                    result_buf->h_name = host_name;
                    result_buf->h_aliases = ((char **) buf) + 2;
                    result_buf->h_addrtype = AF_INET;
                    result_buf->h_length = sizeof(addr);
                    result_buf->h_addr_list = (char **) buf;

                    *errnop = SUCCESS; 
                    return NSS_STATUS_SUCCESS;

                }
            }
        }
    }

    *erronop = ENOENT;
    *h_errnop = HOST_NOT_FOUND;
    return NSS_STATUS_NOTFOUND;
    
}


enum nss_status
_nss_host2ips_gethostbyname2_r (const char *name, 
                                int af,
                                struct hostent *ret,
                                char *buf,
                                size_t buflen,
                                struct hostent **result,
                                int *h_errnop) {

} 
