all:  week04
	
error.o: error.c  

hashtable.o: hashtable.c

week04.o : week04.c

week04: hashtable.o error.o week04.o
	gcc week04.o hashtable.o error.o -lcheck -lm -lrt -pthread -lsubunit -o test-hashtable

