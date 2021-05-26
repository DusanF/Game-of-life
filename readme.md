# Game of life
Cellular automaton written in C using ncurses.
Program uses two threads. Sockets are used for connection to server, that can be used to remotely save current state.

Project was created in 2018 as semester work for subject "Principes of operating systems"

![alt text](https://github.com/DusanF/Game-of-life/blob/master/gol.png "preview")

## Controls:
* H - shows controls
* P/space - pause/continue
* 1-9 - simulation speed
* 0 - one generation step
* Backspace - one step back (previous generation)
* +/> - faster
* -/< - slower
* S - save state
* L - load state
* Q - exit
* E - open/close editor
