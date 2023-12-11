jogo: main.o UpdateDrawFrame.o InitGame.o
	gcc main.o UpdateDrawFrame.o InitGame.o UpdateGame.o DrawGame.o -o prog1 -lm -lraylib

main.o: main.c
	gcc -c main.c

InitGame.o: InitGame.c InitGame.h main.o
	gcc -c InitGame.c

UpdateGame.o: UpdateGame.c UpdateGame.h InitGame.o
	gcc -c UpdateGame.c

DrawGame.o: DrawGame.c DrawGame.h UpdateGame.o InitGame.o
	gcc -c DrawGame.c

UpdateDrawFrame.o: UpdateDrawFrame.c UpdateDrawFrame.h UpdateGame.o DrawGame.o InitGame.o
	gcc -c UpdateDrawFrame.c

clean:
	rm *.o prog1