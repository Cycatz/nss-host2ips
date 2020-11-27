CC = gcc
CFLAGS = -fPIC -Wall -Wextra -Wstrict-prototypes -O2 -std=gnu99
LD_FLAGS= -pthread -shared -Wl,-soname,

NSS_MODULE_NAME = host2ips
NSS_LIB_NAME = libnss_$(NSS_MODULE_NAME)
SONAME = $(NSS_LIB_NAME).so.2
REAL_NAME = $(NSS_LIB_NAME)-2.31.so

SRCS = $(wildcard *.c) 
OBJS = $(SRCS:.c=.o)

DESTDIR = /
PREFIX = $(DESTDIR)
LIBDIR = $(PREFIX)/lib

.PHONY: all clean install

all: $(REAL_NAME)

$(REAL_NAME): $(OBJS)
	$(CC) $(LD_FLAGS)$(SONAME) -o $@ $^

%.o: %.c 
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm *.so
	-rm *.o 

install: 
	install $(REAL_NAME) $(LIBDIR) 
	ldconfig

# for debug 
print-%:
	@echo '$*=$($*)'


