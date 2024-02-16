CC = gcc
CFLAGS = -std=c2x -g -Wall -Wextra -Werror
depManSRC = src/depMan.c
setupSRC = src/setup.c

all: bin/depMan.exe bin/setup.exe

# compile depMan.c
bin/depMan.exe: $(depManSRC)
	$(CC) $(CFLAGS) $(depManSRC) -o bin/depMan && echo "'depMan.c' is compiled successfully."

# compile setup.c
bin/setup.exe: $(setupSRC)
	$(CC) $(CFLAGS) $(setupSRC) -o bin/setup && echo "'setup.c' is compiled successfully."