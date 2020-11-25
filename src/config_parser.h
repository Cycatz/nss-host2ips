#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define my_malloc(x, size, ret) \
    do {                        \
        (x) = malloc((size));   \
        if ((x) == NULL) {      \
            perror("malloc");   \
            return (ret);       \
        }                       \
    } while (0)

#define MAX_LINE_SIZE 128

typedef struct HOST_INFO_ELE {
    char *if_name;
    struct in_addr if_addr;
    struct in_addr addr;
    struct HOST_INFO_ELE *info_next;
} HOST_INFO;

typedef struct HOST_ELE {
    char *name;
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
void parsing_test();

#endif /* CONFIG_PARSER_H */
