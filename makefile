OUTPUTS = gol server
CC = gcc

all: $(OUTPUTS)

clean:
	rm -f $(OUTPUTS) *.o

.PHONY: all clean

%.o: %.c
	$(CC) -c -o $@ $<

gol: gol.o history.o game.o gui.o file.o net.o
	$(CC) -o $@ $^ -lncurses

server: server.o
	$(CC) -o $@ $^
