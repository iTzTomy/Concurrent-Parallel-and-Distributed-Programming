# Aufgabe 1: autoUberquerung, 2 threads
Aufgabe1:
	gcc -pthread -DAufgabe=1 -DNUM_THREADS=2 2UZ.c -o autoUberquerung
	./autoUberquerung

# Aufgabe 2: autoUberquerungLock, 2 threads
Aufgabe2:
	gcc -pthread -DAufgabe=2 -DNUM_THREADS=2 2UZ.c -o autoUberquerungLock
	./autoUberquerungLock

# Aufgabe 3: autoUberquerungLockP, z.B. 5 threads
Aufgabe3:
	gcc -pthread -DAufgabe=3 -DNUM_THREADS=5 2UZ.c -o autoUberquerungLockP
	./autoUberquerungLockP

clean:
	rm -f autoUberquerung autoUberquerungLock autoUberquerungLockP 
