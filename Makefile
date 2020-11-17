CC = gcc
CFLAGS = -fPIC -Wall -Wextra -Wstrict-prototypes -O2 -std=c99
LD_FLAGS= -shared -Wl,-soname,

NSS_MODULE_NAME = myfiles
NSS_LIB_NAME = libnss_$(NSS_MODULE_NAME)
SONAME = $(NSS_LIB_NAME).so.2
REAL_NAME = $(NSS_LIB_NAME)-2.31.so

SRCS = libnss_myfiles.c
OBJS = $(SRCS:.c=.o)

DESTDIR = /
PREFIX = $(DESTDIR)
LIBDIR = $(PREFIX)/lib

.PHONY: all clean install

all: $(REAL_NAME)

# $@: to represent the full target name of the current target   
# $^: returns the names of all the dependencies with space as the delimiter 
$(REAL_NAME): $(OBJS)
	$(CC) $(LD_FLAGS)$(SONAME) -o $@ $^

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c -o  $@ $^

clean:
	-rm *.so
	-rm *.o 

install: 
	install $(REAL_NAME) $(LIBDIR) 
	ldconfig

