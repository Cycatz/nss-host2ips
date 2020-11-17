#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define my_malloc(x, size, ret)     \
    do {                            \
        (x) = malloc((size));       \
        if ((x) == NULL) {          \
            perror("malloc");       \
            return (ret);           \
        }                           \
    } while (0)

#define my_malloc_exit(x, size)         \
    do {                                \
        (x) = malloc((size));           \
        if ((x) == NULL) {              \
            perror("malloc");           \
            exit(EXIT_FAILURE);         \
        }                               \
    } while (0)


#define MAX_LINE_SIZE 128

typedef struct HOST_INFO_ELE {
    char                 *if_name;
    struct in_addr        if_addr;  
    struct in_addr        addr;  
    struct HOST_INFO_ELE *info_next; 
} HOST_INFO ;

typedef struct HOST_ELE {
    char      *name;
    HOST_INFO *info_head, *info_tail;    
    struct HOST_ELE *host_next;
} HOST;

typedef struct {
    HOST *host_head, *host_tail;
} HOST_LIST;

int is_valid_line(char *);

int initialize_host_list(HOST_LIST **); 
int free_host_list(HOST_LIST *); 
int print_host_list(HOST_LIST *);

int add_new_host(HOST_LIST *, HOST *); 
int add_new_host_info(HOST *, HOST_INFO *); 

int parse_config_file(const char *, HOST_LIST *);
int parse_host_name(char *, HOST *);
int parse_host_info(char *, HOST_INFO *);

int is_valid_line(char *s) 
{
    char *l = s;

    if (l && *l == '#') return 1;
    while(*l != '\n') {
        if (!isspace(*l++)) {
            return 0;
        }
    }
    return 1;
}

int initialize_host_list(HOST_LIST **host_list) 
{
    my_malloc_exit(*host_list, sizeof(HOST_LIST));
    
    (*host_list)->host_head = NULL;
    (*host_list)->host_tail = NULL;

    return 0;
}

int free_host_list(HOST_LIST *host_list)
{
    HOST *host, *host_next;
    HOST_INFO *host_info, *info_next;

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
    
    return 0;
}

int add_new_host(HOST_LIST *host_list, HOST *host)
{
    if (host == NULL) {
        return 1;
    }
    if (!(host_list->host_tail)) {
        host_list->host_head = host;
    } else {
        host_list->host_tail->host_next = host;
    }
    host_list->host_tail = host;

    return 0;
}

int add_new_host_info(HOST *host, HOST_INFO *host_info)
{
    if (host_info == NULL) {
        return 1;
    }
    if(!(host->info_tail)) {
        host->info_head = host_info;
    } else {
        host->info_tail->info_next = host_info;
    }
    host->info_tail = host_info;

    return 0;
}


int parse_host_name(char *line, HOST *host) 
{
    char *_host_str = strtok(line, " \t");  
    char *name = strtok(NULL, " \t");

    my_malloc(host->name, strlen(name) + 1, 1); 
    strncpy(host->name, name, strlen(name) + 1);

    return 0;
}

int parse_host_info(char *info, HOST_INFO *host_info)
{

    int s;
    char *addr_str, *if_name, *if_addr_str;

    addr_str = strtok(info, " \t");
    s = inet_pton(AF_INET, addr_str, &(host_info->addr));
    
    if (s <= 0) {
        if (s == 0) {
            fprintf(stderr, "Not in presentation format");
        } else {
            perror("inet_pton");
        }
        return 1;
    }
    
    if_name = strtok(NULL, " \t");
    my_malloc(host_info->if_name, strlen(if_name) + 1, 1);
    strncpy(host_info->if_name, if_name, strlen(if_name) + 1);

    if_addr_str = strtok(NULL, " \t");
    s = inet_pton(AF_INET, if_addr_str, &(host_info->if_addr));
    
    if (s <= 0) {
        if (s == 0) {
            fprintf(stderr, "Not in presentation format");
        } else {
            perror("inet_pton");
        }
        return 1;
    }

    return 0;
}


int parse_config_file(const char *path, HOST_LIST *host_list)
{
    FILE *fp;
    HOST *host = NULL;
    HOST_INFO *host_info = NULL;

    char line[MAX_LINE_SIZE]; 

    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }
    
    while(fgets(line, MAX_LINE_SIZE - 1, fp)) {
        if (!is_valid_line(line)) {
            line[strlen(line) - 1] = '\0';
            if (strncmp(line, "host", 4) == 0) {
                add_new_host(host_list, host);
                my_malloc(host, sizeof(HOST), 1);
                parse_host_name(line,  host);
            } else {
                my_malloc(host_info, sizeof(HOST_INFO), 1);
                parse_host_info(line, host_info);
                add_new_host_info(host, host_info);
            }
        }
    }
    add_new_host(host_list, host);

    return 0;
}

int print_host_list(HOST_LIST *host_list)
{
    HOST *host = host_list->host_head;
    HOST_INFO *host_info;

    while(host) {
        printf("Host name: %s\n", host->name);
        printf("Fallback lists:\n");
        printf("=========================\n");
        host_info = host->info_head;    
        while (host_info) {
            printf("Fallback IP address: %s\n",  inet_ntoa(host_info->addr));
            printf("Adopt it when the IP address of interface %s is %s\n", host_info->if_name, inet_ntoa(host_info->if_addr));
            host_info = host_info->info_next;
        }
        host = host->host_next; 
    }

    return 0;
}

int main()
{
    HOST_LIST *host_list;
    initialize_host_list(&host_list);
    parse_config_file("./example1.syntax", host_list); 
    print_host_list(host_list);
    free_host_list(host_list);
}
