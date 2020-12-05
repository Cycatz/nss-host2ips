#include "parser.h"

#define NSS_HOST2IPS_MAX_LINE_SIZE 128
#define NSS_HOST2IPS_MALLOC(x, size, ret) \
    do {                                  \
        (x) = malloc((size));             \
        if ((x) == NULL) {                \
            perror("malloc\n");           \
            return (ret);                 \
        }                                 \
    } while (0)

static int nss_host2ips_is_valid_line(char *);
static int nss_host2ips_add_new_host(NSS_HOST2IPS_HostList *,
                                     NSS_HOST2IPS_Host *);
static int nss_host2ips_add_new_host_info(NSS_HOST2IPS_Host *,
                                          NSS_HOST2IPS_HostInfo *);

static int nss_host2ips_parse_host_name(char *, NSS_HOST2IPS_Host *);
static int nss_host2ips_parse_host_info(char *, NSS_HOST2IPS_HostInfo *);

int nss_host2ips_initialize_host_list(NSS_HOST2IPS_HostList **host_list)
{
    NSS_HOST2IPS_MALLOC(*host_list, sizeof(NSS_HOST2IPS_HostList), 0);

    (*host_list)->host_head = NULL;
    (*host_list)->host_tail = NULL;

    return 0;
}

int nss_host2ips_free_host_list(NSS_HOST2IPS_HostList *host_list)
{
    NSS_HOST2IPS_Host *host, *host_next;
    NSS_HOST2IPS_HostInfo *host_info, *info_next;

    host = host_list->host_head;

    while (host) {
        host_info = host->info_head;
        host_next = host->host_next;
        free(host->name);
        free(host);

        while (host_info) {
            info_next = host_info->info_next;
            free(host_info->if_name);
            free(host_info);
            host_info = info_next;
        }

        host = host_next;
    }
    free(host_list);

    return 1;
}

static int nss_host2ips_is_valid_line(char *s)
{
    char *l = s;

    if (!l || *l == '#')
        return 1;
    while (*l != '\n') {
        if (!isspace(*l++)) {
            return 0;
        }
    }
    return 1;
}

static int nss_host2ips_add_new_host(NSS_HOST2IPS_HostList *host_list,
                                     NSS_HOST2IPS_Host *host)
{
    if (host == NULL) {
        return 0;
    }
    if (!(host_list->host_tail)) {
        host_list->host_head = host;
    } else {
        host_list->host_tail->host_next = host;
    }
    host_list->host_tail = host;

    return 1;
}

static int nss_host2ips_add_new_host_info(NSS_HOST2IPS_Host *host,
                                          NSS_HOST2IPS_HostInfo *host_info)
{
    if (host_info == NULL) {
        return 0;
    }
    if (!(host->info_tail)) {
        host->info_head = host_info;
    } else {
        host->info_tail->info_next = host_info;
    }
    host->info_tail = host_info;

    return 1;
}


static int nss_host2ips_parse_host_name(char *line, NSS_HOST2IPS_Host *host)
{
    NSS_HOST2IPS_MALLOC(host->name, strlen(line) + 1 - 5, 0);
    strncpy(host->name, line + 5, strlen(line) + 1 - 5);
    return 1;
}

static int nss_host2ips_parse_host_info(char *info,
                                        NSS_HOST2IPS_HostInfo *host_info)
{
    int s;
    char *addr_str, *if_name, *if_addr_str;

    addr_str = strtok(info, " \t");
    s = inet_pton(AF_INET, addr_str, &(host_info->addr));

    if (s <= 0) {
        if (s == 0) {
            fprintf(stderr, "Not in presentation format\n");
        } else {
            perror("inet_pton\n");
        }
        return 0;
    }

    if_name = strtok(NULL, " \t");
    if (if_name[0] != '-') {
        NSS_HOST2IPS_MALLOC(host_info->if_name, strlen(if_name) + 1, 0);
        strncpy(host_info->if_name, if_name, strlen(if_name));
    }

    if_addr_str = strtok(NULL, " \t");
    if (if_name[0] == '-' || if_addr_str[0] == '-') {
        if_addr_str = "255.255.255.255";
    }

    s = inet_pton(AF_INET, if_addr_str, &(host_info->if_addr));

    if (s <= 0) {
        if (s == 0) {
            fprintf(stderr, "Not in presentation format\n");
        } else {
            perror("inet_pton\n");
        }
        return 0;
    }

    return 1;
}


int nss_host2ips_parse_config_file(const char *path,
                                   NSS_HOST2IPS_HostList *host_list)
{
    FILE *fp;
    NSS_HOST2IPS_Host *host = NULL;
    NSS_HOST2IPS_HostInfo *host_info = NULL;

    char line[NSS_HOST2IPS_MAX_LINE_SIZE];

    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("fopen\n");
        return 1;
    }

    while (fgets(line, NSS_HOST2IPS_MAX_LINE_SIZE - 1, fp)) {
        if (!nss_host2ips_is_valid_line(line)) {
            line[strlen(line) - 1] = '\0';
            if (strncmp(line, "host", 4) == 0) {
                nss_host2ips_add_new_host(host_list, host);
                NSS_HOST2IPS_MALLOC(host, sizeof(NSS_HOST2IPS_Host), 0);
                nss_host2ips_parse_host_name(line, host);
            } else {
                NSS_HOST2IPS_MALLOC(host_info, sizeof(NSS_HOST2IPS_HostInfo),
                                    0);
                nss_host2ips_parse_host_info(line, host_info);
                nss_host2ips_add_new_host_info(host, host_info);
            }
        }
    }

    if (!nss_host2ips_add_new_host(host_list, host)) {
        return 1;
    }

    return 0;
}
