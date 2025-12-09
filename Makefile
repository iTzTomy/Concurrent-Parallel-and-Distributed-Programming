# 1. Aufgabe & 2. Aufgabe:   

# Standardanzahl der Threads
THREADS ?= 4

Aufgabe1:
	gcc -fopenmp -DAufgabe=1 7UZ.c -o Aufgabe1
	OMP_NUM_THREADS=$(THREADS) ./Aufgabe1
	make clean

Aufgabe2:
	gcc -fopenmp -DAufgabe=2 7UZ.c -o Aufgabe2
	OMP_NUM_THREADS=$(THREADS) ./Aufgabe2
	make clean

clean:
	rm -f Aufgabe1 Aufgabe2