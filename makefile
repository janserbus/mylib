TREEREND_FOLDER := ./treerend
DSTRUCTS_FOLDER := ./dstructs

test:
	gcc -g3 -o test test.c -I ./lib -L ./lib -ldstructs -ltreerend


all: libtreerend.so
	rm -f *.o
 
libtreerend.so: treerend.o
	gcc -shared -o libtreerend.so treerend.o -L . -ldstructs -lSDL2 -lSDL2_ttf -lm -lfontconfig

treerend.o: libdstructs.so
	gcc -Wextra -I . -fPIC -c $(TREEREND_FOLDER)/treerend.c -ldstructs -lSDL2 -lSDL2_ttf -lm

libdstructs.so: list.o tree.o
	gcc -shared -o libdstructs.so list.o tree.o

list.o:
	gcc -Wextra -I . -fPIC -c $(DSTRUCTS_FOLDER)/list.c

tree.o:
	gcc -Wextra -I . -fPIC -c $(DSTRUCTS_FOLDER)/tree.c

clean:
	rm -f *.o && rm -f *.a && rm -f *.so && rm -f a.out

# LIBRARY COMPLETION COMMANDS

COMP_FOLDER:= lib

relib:
	rm -rf $(COMP_FOLDER)
	make libexport

lib: all
	mkdir $(COMP_FOLDER)
	cp ./libdstructs.so ./$(COMP_FOLDER)
	cp ./libtreerend.so ./$(COMP_FOLDER)
	cp ./list.h ./$(COMP_FOLDER)
	cp ./tree.h ./$(COMP_FOLDER)
	cp ./treerend.h ./$(COMP_FOLDER)


#DEBUG COMPILATION
debug: d_list d_treerend d_tree
	gcc -Wall -Wextra -g3 -o test test.c list.o tree.o treerend.o -I . -lSDL2 -lSDL2_ttf -lfontconfig -lm

d_treerend: d_list
	gcc -Wall -Wextra -g3 -c $(TREEREND_FOLDER)/treerend.c -I .

d_list:
	gcc -Wall -Wextra -g3 -c $(DSTRUCTS_FOLDER)/list.c -I .

d_tree:
	gcc -Wall -Wextra -g3 -c $(DSTRUCTS_FOLDER)/tree.c -I .
