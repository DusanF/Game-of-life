OUTPUTS = gol server
CC = gcc

all: $(OUTPUTS)

clean:
	rm -f $(OUTPUTS) *.o

.PHONY: all clean

%.o: %.c
	$(CC) -c -o $@ $<

gol: gol.o history.o game.o gui.o file.o net.o generator.o
	$(CC) -o $@ $^ -lncurses -lpthread

server: server.o
	$(CC) -o $@ $^
