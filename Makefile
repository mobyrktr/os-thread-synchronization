output: partA.o partB.o
	gcc partA.o -o partA -lpthread
	gcc partB.o -o partB -lrt -lpthread

partA.o: partA.c
	gcc -c partA.c -lpthread

partB.o: partB.c
	gcc -c partB.c -lrt -lpthread

clean:
	rm *.o