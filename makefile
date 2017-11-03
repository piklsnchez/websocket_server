IDIR=include
CFLAGS=-I$(IDIR)
OBJ=websocket.o socket.o linked_list.o hash_map.o

websocket : $(OBJ)
	gcc $(CFLAGS) -o websocket $(OBJ) -lnettle -ggdb -Wall
websocket.o :
	gcc $(CFLAGS) -c websocket.c -ggdb -Wall
socket.o :
	gcc $(CFLAGS) -c socket.c -ggdb -Wall
linked_list.o :
	gcc $(CFLAGS) -c linked_list.c -ggdb -Wall
hash_map.o :
	gcc $(CFLAGS) -c hash_map.c -ggdb -Wall

.PHONY : clean
clean : 
	rm -f websocket $(OBJ)
