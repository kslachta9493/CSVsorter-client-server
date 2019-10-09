CCFLAGS = -ggdb -lm -pthread -g

all: server client

server: server.c sorter.c builder.c
	gcc $(CCFLAGS) server.c sorter.c builder.c -o server
client: sorterClient.c
	gcc $(CCFLAGS) sorterClient.c -o client
clean:
	rm -rf client server
