CC = gcc
CFLAGS = -Wall -Wextra -O2

all: scheduler

scheduler: scheduler.c
	$(CC) $(CFLAGS) -o scheduler scheduler.c

clean:
	rm -f scheduler
