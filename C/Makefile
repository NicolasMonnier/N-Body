NAME=toy_nBody
SRCS=$(NAME).c

MPICC=mpiicc
CC=icc
CFLAGS=-O3 -mtune=core-avx2 -march=core-avx2 -fopenmp -ljson-c

$(NAME): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(NAME)
