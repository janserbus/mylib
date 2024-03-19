libexport: libdata.so libtreerend.so
	mkdir lib
	cp -R ./treerend ./lib
	cp ./libdata.so ./lib
	cp ./libtreerend.so ./lib
	cp ./list.h ./lib
	cp ./tree.h ./lib
	cp ./treerend.h ./lib
	make clean

all: libtreerend.so
	rm -f *.o
 
libtreerend.so: treerend.o
	gcc -shared -o libtreerend.so treerend.o -L . -ldata -lSDL2 -lSDL2_ttf -lm

treerend.o: libdata.so
	gcc -Wextra -c treerend.c -I . -fPIC -ldata -lSDL2 -lSDL2_ttf -lm

libdata.so: list.o tree.o
	gcc -shared -o libdata.so list.o tree.o

list.o:
	gcc -Wextra -I . -fPIC -c list.c

tree.o:
	gcc -Wextra -I . -fPIC -c tree.c

clean:
	rm -f *.o && rm -f *.a && rm -f *.so