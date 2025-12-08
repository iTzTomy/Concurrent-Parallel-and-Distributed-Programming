# 1. Aufgabe, 2. Aufgabe & 3. Aufgabe:   

Aufgabe1:
	gcc -fopenmp -DAufgabe=1 7UZ.c -o Aufgabe1
	./Aufgabe1
	make clean

Aufgabe2:
	gcc -fopenmp -DAufgabe=2 7UZ.c -o Aufgabe2
	./Aufgabe2
	make clean


clean:
	rm -f Aufgabe1 Aufgabe2