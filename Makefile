# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS=marchingsquares

build: $(TARGETS)

marchingsquares: marchingsquares.c
	$(CC) $(CFLAGS) marchingsquares.c -o marching_squares


clean:
	rm -rf $(TARGETS)

.PHONY: pack clean
