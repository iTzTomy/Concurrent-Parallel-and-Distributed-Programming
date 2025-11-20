# 1. Aufgabe:  Speisende Philosophen in C 

philosoph1:
	gcc -o philosoph 4UZ.c -pthread
	./philosoph
	make clean

philosoph2:
	gcc -o philosoph2 4UZ1.c -pthread
	./philosoph2
	make clean

clean:
	rm -f philosoph philosoph2