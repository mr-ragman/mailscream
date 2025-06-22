# Makefile

# ====================================================================================
# For Devs With The Know-How! Git Clone and Use This Makefile To Compile This Program!
# ====================================================================================

# variables
CC=gcc

# Base flags - targeting C17 - change to your flavor!
CFLAGS_BASE = -g -Wall -Wextra -O1 -Wpedantic -std=c17

# POSIX mode
CFLAGS_POSIX = -D_POSIX_C_SOURCE=200809L $(CFLAGS_BASE)

# GNU - YOLO!
CFLAGS_GNU = -D_GNU_SOURCE -march=native $(CFLAGS_BASE)

# ASan build
CFLAGS_SANITIZE_C17 = -fsanitize=address,undefined -fno-omit-frame-pointer $(CFLAGS_BASE)
CFLAGS_SANITIZE_GNU = -fsanitize=address,undefined -fno-omit-frame-pointer $(CFLAGS_GNU)
CFLAGS_SANITIZE_POSIX = -fsanitize=address,undefined -fno-omit-frame-pointer $(CFLAGS_POSIX)

# Files
SRC=main.c utils/helper.c vault/vault.c commands/scream.c commands/persona.c

# Libraries we need
LIBS=-lsqlite3

# the executable program name
OUT=mailscream

# ========================================================
# C17 + POSIX + VALGRIND [default]
# ========================================================
c17-sanitize:
	@echo "\n=== [ Compiling For: C17 ONLY. NO AddressSanitizer] ===\n"
	$(CC) $(CFLAGS_SANITIZE_C17) $(SRC) -o $(OUT)-c17 $(LIBS)
	@echo "\n [ 🎉 C17 ONLY compilation successful ]\n"
	@echo "\n 🧪 Running AddressSanitizer memory safety tests...\n"
	./$(OUT)-c17 ping
	@echo "\n [🎉 AddressSanitizer memory checks completed successful]\n"

# ========================================================
# C17 + POSIX + VALGRIND [default]
# ========================================================
valgrind-posix:
	@echo "\n=== [ Compiling For: C17 + POSIX. NO AddressSanitizer] ===\n"
	$(CC) $(CFLAGS_POSIX) $(SRC) -o $(OUT) $(LIBS)
	@echo "\n [ 🎉 C17 + POSIX compilation successful ]\n"	
	@echo "\n 🧪 Running memory safety tests...\n"
	valgrind ./$(OUT) ping
	@echo "\n [🎉 valgrind memory checks completed successful]\n"

valgrind-posix-release:
	@echo "\n=== [🚀 RELEASE :: Compiling For: C17 + POSIX. NO AddressSanitizer] ===\n"
	$(CC) $(CFLAGS_POSIX) $(CFLAGS_RELEASE) $(SRC) -o $(OUT) $(LIBS)
	@echo "🎉 C17 + POSIX compilation successful"
	@echo "\n 🧪 Running memory safety tests...\n"
	valgrind ./$(OUT) ping
	@echo "\n [🎉 valgrind memory checks completed successful]\n"

# ========================================================
# DEFUALT: C17 + POSIX + AddressSanitizer: - FASTER DEV
# !! DON'T RUN THIS WITH VALGRIND !!
# ========================================================
sanitize:
	@echo "\n=== [Compiling For: C17 + POSIX + AddressSanitizer] ===\n"
	$(CC) $(CFLAGS_SANITIZE_POSIX) $(SRC) -o $(OUT) $(LIBS)
	@echo "\n [🎉 C17 + POSIX compilation successful]\n"
	@echo "\n 🧪 Running AddressSanitizer memory safety tests...\n"
	./$(OUT) ping
	@echo "\n [🎉 AddressSanitizer memory checks completed successful]\n"

sanitize-release:
	@echo "\n=== [🚀 RELEASE :: Compiling For: C17 + POSIX + AddressSanitizer] ===\n"
	$(CC) $(CFLAGS_SANITIZE_POSIX) $(CFLAGS_RELEASE) $(SRC) -o $(OUT) $(LIBS)
	@echo "\n [🎉 C17 + POSIX compilation successful]\n"
	@echo "\n 🧪 Running AddressSanitizer memory safety tests...\n"
	./$(OUT) ping
	@echo "\n [🎉 AddressSanitizer memory checks completed successful]\n"
