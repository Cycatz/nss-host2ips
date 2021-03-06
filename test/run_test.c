#include <stdio.h>
#include <string.h>

// include test modules
#include "library_test.c"
#include "parser_test.c"

int main(int argc, char *argv[])
{
    int i;
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <function>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "parse") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s parse <file>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        list_configs(argv[2]);
    }
    else if (strcmp(argv[1], "library") == 0) {
        if (argv[2] == NULL) {
            fprintf(stderr, "Usage: %s library <hostname>\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        test_nss_host2ips_gethostbyname(argv[2]);
    } else {
        fprintf(stderr, "Function %s does no exist!\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    exit(EXIT_SUCCESS);
}
