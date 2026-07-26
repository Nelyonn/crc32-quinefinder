.PHONY: all clean
all: findquines-crc32 findquines-crc32mp

findquines-crc32: main.c
	gcc -Wall -Wextra -Werror -O3 -march=native -lc -lz -flto main.c -o findquines-crc32

findquines-crc32mp: mainmp.c
	gcc -Wall -Wextra -Werror -O3 -march=native -fopenmp -lc -lz -flto main.c -o findquines-crc32mp

clean: findquines-crc32 findquines-crc32mp
	rm findquines-crc32 findquines-crc32mp
