/*
	CRC32-quinefinder, a CRC32 Fixed-point brute forcer (single-threaded)
	Copyright (C) 2026  Nelyon
	
	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, see
	<https://www.gnu.org/licenses/>.
*/
#include <zlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <omp.h> // This is heavy artillery
#define TARGET_N 0xFFFFFFFF // Max unsigned 32-bit
#define RESERVE_SIZE_CHK 1024

static inline unsigned long long _getTimeNanoSec(void) {
	struct timespec iPtr;
	clock_gettime(CLOCK_MONOTONIC, &iPtr);
	return (unsigned long long) ( iPtr.tv_sec * 1000000000ULL ) + iPtr.tv_nsec;
}

int main(void) {
	// Initialize pointers
	uLong check = 0;
	unsigned long long iterations = 0;

	// Preallocated checksum array (for speed)
	uLong checksums[RESERVE_SIZE_CHK] = {0};
	unsigned long long chkPtr = 0;

	// Timing
	unsigned long long nsecStart = _getTimeNanoSec();
	unsigned long long nsecEnd;

	// Prose prose prose...
	printf("CRC32 Fixed-point brute forcer (omp-enabled)\n");
	printf("Copyright (C) 2026 Nelyon");
	printf("This program is free software, licensed under the GPL v2.\n");
	printf("You can get a copy of the GPL v2 at: https://www.gnu.org/licenses/old-licenses/gpl-2.0.html\n");
	printf("\n");
	printf("START timestamp is at  : %llu\n", nsecStart);

	// Run the loop
	crc32(0L, Z_NULL, 0);
	#pragma omp parallel for schedule(static) reduction(+:iterations)
	for ( long long int loopPtr = 0 ; loopPtr <= TARGET_N ; loopPtr++ ) {
		iterations++;
		unsigned int candidate = htonl(( unsigned int ) loopPtr); // turn little endian into big endian
		unsigned int check = crc32(0L, ( const Bytef* ) &candidate, sizeof(candidate));
		if ( check == ntohl(candidate) ) {
			#pragma omp critical
			{
				checksums[chkPtr] = check;
				chkPtr++;
			}
		}
	}

	// Now enum the results!
	nsecEnd = _getTimeNanoSec();
	
	printf("END timestamp is at    : %llu\n", nsecEnd);
	printf("Number of b.forced itrs: %llu iterations\n", iterations);
	printf("Detected fixed points  : %i\n", (unsigned int) chkPtr);
	printf("-- START LIST OF FIXED POINTS --\n");

	chkPtr = 0;
	for (; chkPtr < RESERVE_SIZE_CHK; chkPtr++) {
		check = checksums[chkPtr];
		if ( !check ) continue;
		printf("[%i] = %lu\n", (unsigned int) chkPtr, check);
	}

	printf("--  END LIST OF FIXED POINTS  --\n");
	return 0;
}
