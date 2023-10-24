CC = gcc
CFLAGS = -std=c2x -g -Wall -Wextra -Werror
depManSRC = src/depMan.c
# mainSRC = src/main.c

# compile depMan.c
bin/depMan.exe: $(depManSRC)
	$(CC) $(CFLAGS) $(depManSRC) -o bin/depMan && echo "'depMan.c' is compiled successfully."

# compile main.c
# bin/main.exe: $(mainSRC)
#	$(CC) $(CFLAGS) $(mainSRC) -o bin/main && echo "'main.c' is compiled successfully."