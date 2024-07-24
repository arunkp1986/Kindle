CC=gcc
CFLAGS=-I.
DEPS = benchmark_run.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

benchmark_run: benchmark_run.o
	$(CC) -o benchmark_run benchmark_run.o 
clean:
	rm benchmark_run *.o
