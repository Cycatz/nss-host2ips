#include "hosts.h"

static pthread_mutex_t NSS_HOST2IPS_mutex = PTHREAD_MUTEX_INITIALIZER;
static NSS_HOST2IPS_HostList *host_list = NULL;

static int nss_host2ips_is_rule_matched(NSS_HOST2IPS_HostInfo *,
                                        struct in_addr *);
static int nss_host2ips_is_reachable_with_ping(struct in_addr);
static int nss_host2ips_find_interface(char *, struct in_addr);

static int nss_host2ips_is_reachable_with_ping(struct in_addr addr)
{
    FILE *in;
    char command[512];
    char buf[512];
    char *addr_str = NULL;
    int exit_code;

    if (!(addr_str = inet_ntoa(addr))) {
        return -1;
    }

    snprintf(command, sizeof(command), "ping -q -c 1 -w 1 %s 2>/dev/null",
             addr_str);

    if (!(in = popen(command, "r"))) {
        return -1;
    }

    // must add this line to get output
    while (fgets(buf, sizeof(buf), in) != NULL)
        ;

    exit_code = pclose(in);
    return exit_code;
}

static int nss_host2ips_find_interface(char *host_name,
                                       struct in_addr host_if_addr)
{
    struct ifaddrs *ifaddr, *ifa;
    struct in_addr sin_addr;
    unsigned addr, host_addr;
    int matched_cnt;

    if (getifaddrs(&ifaddr) == -1) {
        return 0;
    }
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;
        matched_cnt = 0;
        if (host_name == NULL || strcasecmp(ifa->ifa_name, host_name) == 0)
            matched_cnt++;

        sin_addr = ((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
        addr = *(unsigned *) &sin_addr;
        host_addr = *(unsigned *) &host_if_addr;

        if (host_addr == 0xffffffff || (addr == host_addr))
            matched_cnt++;

        if (matched_cnt == 2) {
            freeifaddrs(ifaddr);
            return 1;
        }
    }
    freeifaddrs(ifaddr);
    return 0;
}

static int nss_host2ips_is_rule_matched(NSS_HOST2IPS_HostInfo *host_info,
                                        struct in_addr *addr)
{
    for (; host_info != NULL; host_info = host_info->info_next) {
        if (nss_host2ips_find_interface(host_info->if_name,
                                        host_info->if_addr)) {
            *addr = host_info->addr;
            return 1;
        }
    }
    return 0;
}


enum nss_status _nss_host2ips_sethostent_locked(int stayopen)
{
    if (nss_host2ips_initialize_host_list(&host_list) != 0) {
        return NSS_STATUS_UNAVAIL;
    }

    if (nss_host2ips_parse_config_file(NSS_HOST2IPS_CONFIG_FILE_NAME,
                                       host_list) != 0) {
        return NSS_STATUS_UNAVAIL;
    }

    return NSS_STATUS_SUCCESS;
}


enum nss_status _nss_host2ips_sethostent(int stayopen)
{
    enum nss_status ret;
    NSS_HOST2IPS_LOCK(&NSS_HOST2IPS_mutex);
    ret = _nss_host2ips_sethostent_locked(stayopen);
    NSS_HOST2IPS_UNLOCK(&NSS_HOST2IPS_mutex);

    return ret;
}

enum nss_status _nss_host2ips_endhostent_locked(void)
{
    if (host_list) {
        nss_host2ips_free_host_list(host_list);
    }

    return NSS_STATUS_SUCCESS;
}

enum nss_status _nss_host2ips_endhostent(void)
{
    enum nss_status ret;
    NSS_HOST2IPS_LOCK(&NSS_HOST2IPS_mutex);
    ret = _nss_host2ips_endhostent_locked();
    NSS_HOST2IPS_UNLOCK(&NSS_HOST2IPS_mutex);

    return ret;
}


enum nss_status _nss_host2ips_gethostbyname2_r(const char *name,
                                               int af,
                                               struct hostent *result_buf,
                                               char *buf,
                                               size_t buflen,
                                               int *errnop,
                                               int *h_errnop)
{
    enum nss_status ret = NSS_STATUS_SUCCESS;
    int len_hostname = strlen(name);
    NSS_HOST2IPS_Host *host;
    struct in_addr addr;
    char *host_name, *addr_ptr;


    if (host_list == NULL) {
        ret = _nss_host2ips_sethostent(0);
    }

    if (ret != NSS_STATUS_SUCCESS) {
        *errnop = ENOENT;
        *h_errnop = NO_RECOVERY;
        return ret;
    }

    for (host = host_list->host_head; host != NULL; host = host->host_next) {
        if (strcasecmp(host->name, name) == 0) {
            if (nss_host2ips_is_rule_matched(host->info_head, &addr)) {
                if (NSS_HOST2IPS_SP(
                        NSS_HOST2IPS_SP(
                            NSS_HOST2IPS_SP(buf, char, len_hostname + 1), addr,
                            1),
                        char *, 3) > buf + buflen) {
                    *errnop = ERANGE;
                    *h_errnop = NO_RECOVERY;

                    return NSS_STATUS_TRYAGAIN;
                }
                strcpy(buf, name);

                host_name = buf;
                buf = NSS_HOST2IPS_ALIGN(buf + len_hostname + 1, addr);
                *((struct in_addr *) buf) = addr;
                addr_ptr = buf;
                buf = NSS_HOST2IPS_ALIGN(buf + sizeof(addr), char *);
                ((char **) buf)[0] = addr_ptr;
                ((char **) buf)[1] = NULL;
                ((char **) buf)[2] = NULL;


                result_buf->h_name = host_name;
                result_buf->h_aliases = ((char **) buf) + 2;
                result_buf->h_addrtype = AF_INET;
                result_buf->h_length = sizeof(addr);
                result_buf->h_addr_list = (char **) buf;

                *errnop = 0;
                *h_errnop = 0;
                return NSS_STATUS_SUCCESS;
            }
        }
    }

    *errnop = ENOENT;
    *h_errnop = HOST_NOT_FOUND;
    return NSS_STATUS_NOTFOUND;
}


enum nss_status _nss_host2ips_gethostbyname_r(const char *name,
                                              struct hostent *ret,
                                              char *buf,
                                              size_t buflen,
                                              int *errnop,
                                              int *h_errnop)
{
    return _nss_host2ips_gethostbyname2_r(name, AF_UNSPEC, ret, buf, buflen,
                                          errnop, h_errnop);
}
