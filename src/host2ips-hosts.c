#include "host2ips-hosts.h"

static pthread_mutex_t NSS_HOST2IPS_mutex = PTHREAD_MUTEX_INITIALIZER;
static NSS_HOST2IPS_HostList *host_list = NULL;


static int nss_host2ips_is_rule_matched(NSS_HOST2IPS_HostInfo *, struct in_addr *);
static int nss_host2ips_is_reachable_with_ping(struct in_addr);

static int nss_host2ips_is_reachable_with_ping(struct in_addr addr)
{
    FILE *in;
    char command[512];
    char *addr_str = NULL;
    int exit_code;

    if (!(addr_str = inet_ntoa(addr))) {
        return -1;
    }

    snprintf(command, sizeof(command), "ping -c 1 %s 2>/dev/null", addr_str);

    if (!(in = popen(command, "r"))) {
        return -1;
    }

    exit_code = pclose(in);

    return exit_code;
}

static int nss_host2ips_is_rule_matched(NSS_HOST2IPS_HostInfo *host_info, struct in_addr *addr)
{
    for (; host_info != NULL; host_info = host_info->info_next) {
        if (nss_host2ips_is_reachable_with_ping(host_info->addr) == 0) {
            *addr = host_info->addr;
            return 1;
        }
    }

    return -1;
}


enum nss_status _nss_host2ips_sethostent_locked(int stayopen)
{
    if (nss_host2ips_initialize_host_list(&host_list) != 0) {
        return NSS_STATUS_UNAVAIL;
    }

    if (nss_host2ips_parse_config_file(NSS_HOST2IPS_CONFIG_FILE_NAME, host_list) != 0) {
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


enum nss_status _nss_host2ips_gethostbyname_r(const char *name,
                                              struct hostent *result_buf,
                                              char *buf,
                                              size_t buflen,
                                              int *errnop,
                                              int *h_errnop)
{
    enum nss_status ret = NSS_STATUS_SUCCESS;
    int len_hostname = strlen(name);
    NSS_HOST2IPS_Host *host;
    NSS_HOST2IPS_HostInfo *host_info;
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

                strncpy(buf, name, len_hostname);
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

                return NSS_STATUS_SUCCESS;
            }
        }
    }

    *errnop = ENOENT;
    *h_errnop = HOST_NOT_FOUND;
    return NSS_STATUS_NOTFOUND;
}


enum nss_status _nss_host2ips_gethostbyname2_r(const char *name,
                                               int af,
                                               struct hostent *ret,
                                               char *buf,
                                               size_t buflen,
                                               struct hostent **result,
                                               int *h_errnop)
{
    return NSS_STATUS_SUCCESS;
}