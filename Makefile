all:
	gcc -std=c11 -Wall -Wextra -pedantic *.c -o trabalho -lm

clean:
	rm -f trabalho trabalho.exe *.o

.PHONY: all clean
