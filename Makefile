# 1. Aufgabe, 2. Aufgabe & 3. Aufgabe:   

Aufgabe1:
	gcc -pthread -DAufgabe=1 6UZ.c -o Aufgabe1
	./Aufgabe1
	make clean

Aufgabe2:
	gcc -pthread -DAufgabe=2 6UZ.c -o Aufgabe2
	./Aufgabe2
	make clean

Aufgabe3:
	gcc -pthread -DAufgabe=3 6UZ.c -o Aufgabe3
	./Aufgabe3
	make clean

clean:
	rm -f Aufgabe1 Aufgabe2 Aufgabe3