.PHONY: all clean
all: findquines-crc32

findquines-crc32: main.c
	gcc -Wall -Wextra -Werror -O3 -march=native -lc -lz -flto main.c -o findquines-crc32
