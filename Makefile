# 1. Aufgabe:  Speisende Philosophen in C 

philosoph2:
	gcc -o philosoph2 4UZ.c -pthread
	./philosoph2
	make clean

philosoph1:
	gcc -o philosoph1 4UZ1.c -pthread
	./philosoph1
	make clean

clean:
	rm -f philosoph1 philosoph2