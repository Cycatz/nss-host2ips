#ifndef NSS_HOST2IPS_PARSER_H
#define NSS_HOST2IPS_PARSER_H

#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct NSS_HOST2IPS_HostInfo_t {
    char *if_name;
    struct in_addr if_addr;
    struct in_addr addr;
    struct NSS_HOST2IPS_HostInfo_t *info_next;
} NSS_HOST2IPS_HostInfo;

typedef struct NSS_HOST2IPS_Host_t {
    char *name;
    NSS_HOST2IPS_HostInfo *info_head, *info_tail;
    struct NSS_HOST2IPS_Host_t *host_next;
} NSS_HOST2IPS_Host;

typedef struct {
    NSS_HOST2IPS_Host *host_head, *host_tail;
} NSS_HOST2IPS_HostList;


int nss_host2ips_free_host_list(NSS_HOST2IPS_HostList *);
int nss_host2ips_parse_config_file(const char *, NSS_HOST2IPS_HostList *);
int nss_host2ips_initialize_host_list(NSS_HOST2IPS_HostList **);
void nss_host2ips_parsing_test(void);

#endif /* NSS_HOST2IPS_PARSER_H */
