#include "parser.h"

void list_configs(const char *config_file_name)
{
    NSS_HOST2IPS_HostList *host_list;
    nss_host2ips_initialize_host_list(&host_list);
    // nss_host2ips_parse_config_file(NSS_HOST2IPS_TEST_CONFIG_FILE_NAME,
    // host_list);
    nss_host2ips_parse_config_file(config_file_name, host_list);

    NSS_HOST2IPS_Host *host = host_list->host_head;
    NSS_HOST2IPS_HostInfo *host_info;

    char if_addr[16];
    int idx;

    while (host) {
        printf("Host name: %s\n", host->name);
        printf("Fallback lists:\n");
        printf("=========================\n");
        host_info = host->info_head;
        idx = 0;
        while (host_info) {
            printf("%d. Fallback IP address: %s\n", ++idx,
                   inet_ntoa(host_info->addr));
            if (host_info->if_name != NULL) {
                printf("Obtain it when interface %s exists",
                       host_info->if_name);
                strncpy(if_addr, inet_ntoa(host_info->if_addr),
                        sizeof(if_addr));
                if (strncmp(if_addr, "255.255.255.255", sizeof(if_addr)) != 0) {
                    printf(" and its IP is %s", if_addr);
                }
                printf("\n");
            }
            host_info = host_info->info_next;
        }
        printf("\n");

        host = host->host_next;
    }
    nss_host2ips_free_host_list(host_list);
}
