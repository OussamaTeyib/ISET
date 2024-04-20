CC = gcc
CFLAGS = -std=c2x -g -Wall -Wextra -Werror

DEPMAN = bin/depMan.exe
DEPMAN_SRC = src/depMan.c
SETUP = bin/setup.exe
SETUP_SRC = src/setup.c 

BINS_DIR = res
DEPS_DIR = src/Departements
BINS = $(wildcard $(BINS_DIR)/*.bin)
DEPS = $(patsubst $(BINS_DIR)/%.bin, $(DEPS_DIR)/%.c, $(BINS))

MAIN = src/main.c

all: $(DEPMAN) $(SETUP) $(DEPS) clean

# compile depMan.c
$(DEPMAN): $(DEPMAN_SRC)
	@$(CC) $(CFLAGS) $(DEPMAN_SRC) -o bin/depMan && echo "-> 'depMan.c' is compiled successfully."

# compile setup.c
$(SETUP): $(SETUP_SRC)
	@$(CC) $(CFLAGS) $(SETUP_SRC) -o bin/setup && echo "-> 'setup.c' is compiled successfully."

# update .c files or create new ones
$(DEPS_DIR)/%.c: $(BINS_DIR)/%.bin $(MAIN)
	@cd bin; ./setup $(patsubst $(BINS_DIR)/%.bin, %, $<)

clean:
	@rm -f src/Departements/*.exe