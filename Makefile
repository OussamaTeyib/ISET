CC = gcc
CFLAGS = -std=c18 -Wall -Wextra -Werror
depManSRC = src/depMan.c
mainSRC = src/main.c

all: main depMan

# compile main.c
main: $(mainSRC)
	$(CC) $(CFLAGS) $(mainSRC) -o bin/main && echo "'main.c' is compiled successfully."

# compile depMan.c
depMan: $(depManSRC)
	$(CC) $(CFLAGS) $(depManSRC) -o bin/depMan && echo "'depMan.c' is compiled successfully."

run_main:
	bin/main.exe

run_depMan:
	bin/depMan.exe

run_build:
	build.exe 

op_main:
	notepad src/main.c

op_depMan:
	notepad src/depMan.c

op_build:
	notepad src/build.c