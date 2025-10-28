CC = gcc
CFLAGS = -Wall -Wextra -O2 -pthread
SRC = 2UZ.c
OUT = bridge

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

# Task1: autoUberquerung, 2 threads
Task1:
	$(CC) $(CFLAGS) -DTASK=1 -DNUM_THREADS=2 $(SRC) -o $(OUT)
	./$(OUT)

# Task2: autoUberquerungLock, 2 threads
Task2:
	$(CC) $(CFLAGS) -DTASK=2 -DNUM_THREADS=2 $(SRC) -o $(OUT)
	./$(OUT)

# Task3: autoUberquerungLockP, 4 threads
Task3:
	$(CC) $(CFLAGS) -DTASK=3 -DNUM_THREADS=4 $(SRC) -o $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
