# 1. Aufgabe & 2. Aufgabe:   

Aufgabe1:
	gcc -pthread -DAufgabe=1 5UZ.c -o Aufgabe1
	./Aufgabe1
	make clean

Aufgabe2:
	gcc -pthread -DAufgabe=2 5UZ.c -o Aufgabe2
	./Aufgabe2
	make clean

clean:
	rm -f Aufgabe1 Aufgabe2