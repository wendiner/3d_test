3d_test.o:	3d_test.c
	gcc -c 3d_test.c
wireframe.o:	wireframe.c	wireframe.h
	gcc -c wireframe.c
3d_test:	3d_test.o	wireframe.o
	gcc 3d_test.o wireframe.o -o 3d_test $$(pkg-config allegro-5 allegro_primitives-5 --libs --cflags) -l:libm.so
converter.o:	converter.c
	gcc -c -g converter.c
converter:	converter.o	wireframe.o
	gcc converter.o wireframe.o -o converter -g $$(pkg-config allegro-5 allegro_primitives-5 --libs --cflags) -l:libm.so
clean:
	rm 3d_test converter ./*.o
test:
	DEBUGINFOD_URLS="https://debuginfod.ubuntu.com" gdb -d ../src/glibc-2.39 --args converter teapot.obj teapot.bin
