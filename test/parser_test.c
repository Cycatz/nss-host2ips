#include "host2ips-parser.h"

void list_configs(const char *config_file_name)
{
    NSS_HOST2IPS_HostList *host_list;
    nss_host2ips_initialize_host_list(&host_list);
    // nss_host2ips_parse_config_file(NSS_HOST2IPS_TEST_CONFIG_FILE_NAME,
    // host_list);
    nss_host2ips_parse_config_file(config_file_name, host_list);

    NSS_HOST2IPS_Host *host = host_list->host_head;
    NSS_HOST2IPS_HostInfo *host_info;

    while (host) {
        printf("Host name: %s\n", host->name);
        printf("Fallback lists:\n");
        printf("=========================\n");
        host_info = host->info_head;
        while (host_info) {
            printf("Fallback IP address: %s\n", inet_ntoa(host_info->addr));
            printf("Obtain it when the IP address of interface %s is %s\n",
                   host_info->if_name, inet_ntoa(host_info->if_addr));
            host_info = host_info->info_next;
        }
        printf("\n");

        host = host->host_next;
    }

    nss_host2ips_free_host_list(host_list);
}
