
SRCS= client_main.c get_temp.c
PROG= client_main
CC=gcc
CFLAGS=-g
OBJS=$(SRCS:.c=.o)
$(PROG):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
$(OBJS):get_temp.h
clean:
	rm -rf $(OBJS)
