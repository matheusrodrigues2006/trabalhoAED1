all:
	gcc -std=c11 -Wall -Wextra -pedantic *.c -o trabalho -lm

run: all
	./trabalho $(ARGS)

clean:
	rm -f trabalho trabalho.exe *.o

.PHONY: all run clean
