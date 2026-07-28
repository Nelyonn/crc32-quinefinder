.PHONY: all clean
all: findquines-crc32 findquines-crc32-nolz findquines-crc32mp findquines-crc32mp-nolz

findquines-crc32: main.c
	gcc -Wall -Wextra -Werror -O3 -march=native -flto main.c -lc -lz -o findquines-crc32

findquines-crc32-nolz: main.c
	gcc -Wall -Wextra -Werror -O3 -march=native -DNOLZ -flto main.c -lc -o findquines-crc32-nolz

findquines-crc32mp: mainmp.c
	gcc -Wall -Wextra -Werror -O3 -march=native -fopenmp -flto mainmp.c -lc -lz -o findquines-crc32mp

findquines-crc32mp-nolz: mainmp.c
	gcc -Wall -Wextra -Werror -O3 -march=native -DNOLZ -fopenmp -flto mainmp.c -lc -o findquines-crc32mp-nolz

clean:
	rm -f findquines-crc32 findquines-crc32mp findquines-crc32mp-nolz findquines-crc32-nolz
