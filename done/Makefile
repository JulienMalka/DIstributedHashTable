all:  test-hashtable
	
error.o: error.c  

week04.o: week04.c

test-hashtable.o : test-hashtable.c

test-hashtable: week04.o error.o test-hashtable.o
	gcc test-hashtable.o week04.o error.o -lcheck -lm -lrt -pthread -lsubunit -o test-hashtable

