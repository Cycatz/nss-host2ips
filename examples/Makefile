CC = gcc
CFLAGS = -Wall -Wextra -Wstrict-prototypes -O2 -std=c11
LDLIBS = -ldl

main.out: main.c
	$(CC) $(CFLAGS) $(LDLIBS) -o main.out main.c
main1.out: main1.c
	$(CC) $(CFLAGS) $(LDLIBS) -o main1.out main1.c
main2.out: main2.c
	$(CC) $(CFLAGS) $(LDLIBS) -o main2.out main2.c
main3.out: main3.c
	$(CC) $(CFLAGS) $(LDLIBS) -o main3.out main3.c

clean:
	-rm *.out
	-rm *.o 

install: 
	install $(REAL_NAME) $(LIBDIR) 
	ldconfig

