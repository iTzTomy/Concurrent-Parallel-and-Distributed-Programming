# 1. Aufgabe:  Speisende Philosophen in C 

philosoph:
	gcc -o philosoph 4UZ.c -pthread
	./philosoph
	make clean

clean:
	rm -f philosoph