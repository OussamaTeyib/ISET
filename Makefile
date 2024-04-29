CC = gcc
CFLAGS = -std=c2x -g -Wall -Wextra -Werror

DEPMAN = bin/depMan.exe
DEPMAN_SRC = src/depMan.c
SETUP = bin/setup.exe
SETUP_SRC = src/setup.c 

BINS_DIR = res
DEPS_DIR = Departements
BINS = $(wildcard $(BINS_DIR)/*.bin)
DEPS = $(patsubst $(BINS_DIR)/%.bin, $(DEPS_DIR)/%.c, $(BINS))

MAIN = src/main.c
RELEASE_DIR = release

all: $(DEPMAN) $(SETUP) $(DEPS) clean

# compile depMan.c
$(DEPMAN): $(DEPMAN_SRC)
	@$(CC) $(CFLAGS) $(DEPMAN_SRC) -o $(DEPMAN) && echo "-> 'depMan.c' is compiled successfully."

# compile setup.c
$(SETUP): $(SETUP_SRC)
	@$(CC) $(CFLAGS) $(SETUP_SRC) -o $(SETUP) && echo "-> 'setup.c' is compiled successfully."

# update .c files or create new ones
$(DEPS_DIR)/%.c: $(BINS_DIR)/%.bin $(MAIN)
	@cd bin; ./setup $(patsubst $(BINS_DIR)/%.bin, %, $<)

.PHONY: zip push release clean

# Zip departements files
zip:
	@mkdir -p $(RELEASE_DIR)
	@cd src; 7z a -tzip ../$(RELEASE_DIR)/ISET-$(ver).zip $(DEPS_DIR)/*

# Push to github
push:
	@git add .
	@git commit -m "$(msg)"
	@git push origin main

# Publish a release
release:
	@git tag -a $(ver) -m "Release $(ver)"
	@git push origin $(ver)
	@gh release create $(ver)
	@gh release upload $(ver) $(RELEASE_DIR)/ISET-$(ver).zip

clean:
	@rm -f src/Departements/*.exe