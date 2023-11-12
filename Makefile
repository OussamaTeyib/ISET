CC = gcc
CFLAGS = -std=c2x -g -Wall -Wextra -Werror
depManSRC = src/depMan.c
bin2hexSRC = src/bin2hex.c
mainSRC = src/main.c

all: bin/depMan.exe bin/bin2hex.exe bin/main.exe

# compile depMan.c
bin/depMan.exe: $(depManSRC)
	$(CC) $(CFLAGS) $(depManSRC) -o bin/depMan && echo "'depMan.c' is compiled successfully."

# compile bin2hex.c
bin/bin2hex.exe: $(bin2hexSRC)
	$(CC) $(CFLAGS) $(bin2hexSRC) -o bin/bin2hex && echo "'bin2hex.c' is compiled successfully."

# compile main.c
bin/main.exe: $(mainSRC)
	$(CC) $(CFLAGS) $(mainSRC) -o bin/main && echo "'main.c' is compiled successfully."