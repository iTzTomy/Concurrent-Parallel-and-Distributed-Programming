# 1. Aufgabe & 2. Aufgabe:   

Aufgabe1:
	gcc -fopenmp -DAufgabe=1 7UZoror.c -o Aufgabe1
	read -p "Enter number of threads for Aufgabe1: " THREADS
	export OMP_NUM_THREADS=$THREADS
	./Aufgabe1
	make clean

Aufgabe2:
	gcc -fopenmp -DAufgabe=2 7UZoror.c -o Aufgabe2
	read -p "Enter number of threads for Aufgabe1: " THREADS
	export OMP_NUM_THREADS=$THREADS
	./Aufgabe2
	make clean


clean:
	rm -f Aufgabe1 Aufgabe2