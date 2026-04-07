3d_test:	3d_test.c
	gcc 3d_test.c -o 3d_test $$(pkg-config allegro-5 allegro_primitives-5 --libs --cflags) -l:libm.so
