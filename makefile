FLAGS = -g -fPIC
CC = g++

all: queserver queclient test_server test_guard test_singleton pollclient pollserver lib

lib: Queue.o active_object.o guard.o singleton.o reactor.o
	$(CC) -shared -o lib.so Queue.o active_object.o reactor.o


test_server: test_server.o
	$(CC) test_server.o -o test_server -lpthread	

queclient: client.o
	$(CC) -g client.cpp -o client

queserver: server.o Queue.o active_object.o
	$(CC)  -g Queue.o server.o active_object.o -o server -lpthread

Queue.o: Queue.cpp
	$(CC) $(FLAGS) -c Queue.cpp

active_object.o: active_object.cpp
	$(CC) $(FLAGS) -c active_object.cpp	

server.o: server.cpp
	$(CC) $(FLAGS) -c server.cpp

#part 4

test_guard: test_guard.o guard.o
	$(CC) test_guard.o guard.o -o test_guard -lpthread

test_guard.o:
	$(CC) $(FLAGS) -c test_guard.cpp

guard.o:
	$(CC) $(FLAGS) -c guard.cpp
# Part 5

test_singleton: singleton.o
	$(CC) singleton.o -o test_singleton -lpthread

singleton.o: singleton.cpp
	$(CC) $(FLAGS) -c singleton.cpp

# Part 6	
pollserver: pollserver.o reactor.o
	$(CC) -g pollserver.o reactor.o -o pserver -lpthread
pollclient: pollclient.cpp
	$(CC) -g pollclient.cpp -o pclient -lpthread

pollserver.o: pollserver.cpp
	$(CC) -g -c pollserver.cpp	

reactor.o: reactor.cpp
	$(CC) $(FLAGS) -c reactor.cpp
clean:
	rm *.o server client *.so test_guard test_singleton pserver pclient test_server

.PHONY: clean all lib