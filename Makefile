CC = gcc
CFLAGS = -fPIC -Wall -Wextra -Wstrict-prototypes -O2 -std=gnu99
LD_FLAGS = -pthread -shared -Wl,-soname,

NSS_MODULE_NAME = host2ips
NSS_LIB_NAME = libnss_$(NSS_MODULE_NAME)
SONAME = $(NSS_LIB_NAME).so.2
REAL_NAME = $(NSS_LIB_NAME)-2.31.so

SRC_DIR = src
TAR_DIR = target
TAR_LIB = $(TAR_DIR)/$(REAL_NAME)

SRCS = $(wildcard $(SRC_DIR)/*.c) 
OBJS = $(patsubst $(SRC_DIR)/%.c, $(TAR_DIR)/%.o, $(SRCS))

DESTDIR = /
PREFIX = $(DESTDIR)
LIBDIR = $(PREFIX)/lib

TEST_DIR = test
TEST_SRC = $(TEST_DIR)/test_library.c
# TEST_SRC = $(TEST_DIR)/list_configs.c
TEST_INCLUDES = -I$(SRC_DIR)
TEST_LD_FLAGS = -ldl
TEST_BIN = $(TAR_DIR)/run_test
TEST_OBJS = $(patsubst $(SRC_DIR)/%.c, $(TAR_DIR)/test-%.o, $(SRCS))
TEST_TAR_LIB = $(TAR_DIR)/test-$(REAL_NAME) 

.PHONY: all clean install test

all: $(TAR_LIB)

$(TAR_LIB): $(OBJS)
	$(CC) $(LD_FLAGS)$(SONAME) -o $@ $^

$(TAR_DIR)/%.o: $(SRC_DIR)/%.c 
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TAR_LIB)
	$(CC) $(TEST_LD_FLAGS) $(TEST_INCLUDES) -D LIB_NAME=\"$(abspath $(TEST_TAR_LIB))\"                                    \
											-D NSS_HOST2IPS_TEST_CONFIG_FILE_NAME=\"$(abspath $(TEST_DIR)/files/hosts)\"  \
											-o $(TEST_BIN) $(TEST_OBJS) $(TEST_SRC)
$(TEST_TAR_LIB): $(TEST_OBJS)
	$(CC) $(LD_FLAGS)$(SONAME) -o $@ $^

$(TAR_DIR)/test-%.o: $(SRC_DIR)/%.c 
	$(CC) $(CFLAGS) -D NSS_HOST2IPS_TEST \
	                -D NSS_HOST2IPS_CONFIG_FILE_NAME=\"$(abspath $(TEST_DIR)/files/hosts)\" \
					-c $< -o $@ 
clean:
	-rm $(TAR_LIB)
	-rm $(OBJS)
	-rm $(TEST_OBJS)
	-rm $(TEST_BIN)
	-rm $(TEST_TAR_LIB)

install: 
	install $(TAR_LIB) $(LIBDIR) 
	ldconfig

# for debug 
print-%:
	@echo '$*=$($*)'

