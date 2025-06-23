# Makefile

# ====================================================================================
# For Devs With The Know-How! Git Clone and Use This Makefile To Compile This Program!
# Adjust as you see fit for your env
# ====================================================================================

# variables
CC=gcc
COMPILE_VERSION=c11

# Base flags - targeting C17 - change to your flavor!
CFLAGS_BASE = -g -Wall -Wextra -O2 -Wpedantic -std=$(COMPILE_VERSION)
CFLAGS_RELEASE = -O2 -fstack-protector-strong -DNDEBUG

# POSIX mode
CFLAGS_POSIX = -D_POSIX_C_SOURCE=200809L $(CFLAGS_BASE)

# GNU - YOLO!
CFLAGS_GNU = -D_GNU_SOURCE -march=native $(CFLAGS_BASE)

# Files
SRC=src/main.c src/utils/helper.c src/vault/vault.c src/commands/scream.c src/commands/persona.c

# Critical: Also pass sanitizer flags to linker
# LDFLAGS=-fsanitize=address,undefined

# Libraries we need
LIBS=-lsqlite3

# the executable program name
OUT=mailscream-prod

# ========================================================
# C + POSIX
# ========================================================
all:
	@echo "\n=== [Compiling For: $(COMPILE_VERSION) + POSIX] ===\n"
	$(CC) $(CFLAGS_POSIX) $(CFLAGS_RELEASE) $(SRC) -o $(OUT) $(LIBS)
	@echo "\n [ OK! Compilation completed successfully..]\n"
	@echo "\n Ping the app...\n"
	./$(OUT) ping
	@echo "\n [ DONE! Now you can run [ ./$(OUT) ] to go to main menu!]\n"
