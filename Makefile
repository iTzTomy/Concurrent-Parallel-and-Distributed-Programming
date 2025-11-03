# Aufgabe 1: Mutex

mutex:
	gcc -o mutex 3UZ.c -pthread
	./mutex
	make clean

clean:
	rm -f mutex