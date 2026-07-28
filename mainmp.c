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
#ifndef NOLZ
	#include <zlib.h>
#endif
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <omp.h> // This is heavy artillery
#define TARGET_N 0xFFFFFFFF // Max unsigned 32-bit
#define RESERVE_SIZE_CHK 256

#ifdef NOLZ
	uint32_t crc_table[4][256];

	void fastcrc32_init(void)
	{
	    for (uint32_t i = 0; i < 256; i++) {
	        uint32_t crc = i;
	        for (int j = 0; j < 8; j++)
	            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320u : 0);
	        crc_table[0][i] = crc;
	    }
	    for (int t = 1; t < 4; t++) {
	        for (uint32_t i = 0; i < 256; i++) {
	            uint32_t crc = crc_table[t - 1][i];

	            crc_table[t][i] =
	                crc_table[0][crc & 0xff] ^
	                (crc >> 8);
	        }
	    }
	}

	uint32_t fastcrc32(unsigned int x)
	{
	    uint32_t crc = 0xFFFFFFFFu;
	    crc ^= x;

	    crc =
	        crc_table[3][ crc        & 0xff] ^
	        crc_table[2][(crc >>  8) & 0xff] ^
	        crc_table[1][(crc >> 16) & 0xff] ^
	        crc_table[0][(crc >> 24) & 0xff];

	    return ~crc;
	}
#endif

static inline unsigned long long _getTimeNanoSec(void) {
	struct timespec iPtr;
	clock_gettime(CLOCK_MONOTONIC, &iPtr);
	return (unsigned long long) ( iPtr.tv_sec * 1000000000ULL ) + iPtr.tv_nsec;
}

int main(void) {
	// Initialize pointers
	unsigned int check = 0;
	unsigned long long iterations = 0;

	// Preallocated checksum array (for speed)
	unsigned int checksums[RESERVE_SIZE_CHK];
	unsigned long long chkPtr = 0;

	// Timing
	unsigned long long nsecStart = _getTimeNanoSec();
	unsigned long long nsecEnd;

	// Prose prose prose...
	#ifndef NOLZ
		printf("CRC32 Fixed-point brute forcer (omp-enabled)\n");
	#else
		printf("CRC32 Fixed-point brute forcer (omp-enabled, no-zlib)\n");
	#endif
	printf("Copyright (C) 2026 Nelyon\n");
	printf("This program is free software, licensed under the GPL v2.\n");
	printf("You can get a copy of the GPL v2 at: https://www.gnu.org/licenses/old-licenses/gpl-2.0.html\n");
	printf("\n");
	printf("Note: each timestamp here is monotonically increasing since system startup!\n");
	printf("START timestamp is at  : %llu\n", nsecStart);

	// Run an init loop on the checksums array
	for ( unsigned short loopPtr = 0 ; loopPtr < RESERVE_SIZE_CHK ; loopPtr++ ) {
		checksums[ loopPtr ] = 0xE4E4E4E4; // magic placeholder!
	}

	// Run the loop
	#ifndef NOLZ
		crc32(0L, Z_NULL, 0);
	#else
		fastcrc32_init();
	#endif
	#pragma omp parallel for schedule(static) reduction(+:iterations)
	for ( long long int loopPtr = 0 ; loopPtr <= TARGET_N ; loopPtr++ ) {
		iterations++;
		unsigned int candidate = htonl(( unsigned int ) loopPtr); // turn little endian into big endian
		#ifndef NOLZ
			unsigned int check = crc32(0L, ( const Bytef* ) &candidate, sizeof(candidate));
		#else
			unsigned int check = fastcrc32(candidate);
		#endif
	
		if ( check == ( unsigned int ) loopPtr ) {
			#pragma omp critical
			{
				checksums[ chkPtr ] = check;
				chkPtr++;
			}
		}
	}

	// Now enum the results!
	nsecEnd = _getTimeNanoSec();
	
	printf("END timestamp is at    : %llu\n", nsecEnd);
	printf("In seconds             : %f\n", ( double ) ( ( nsecEnd - nsecStart ) / 1e9 ));
	printf("Number of b.forced itrs: %llu iterations\n", iterations);
	printf("Detected fixed points  : %i\n", ( unsigned int ) chkPtr);
	printf("-- START LIST OF FIXED POINTS --\n");

	chkPtr = 0;
	for (; chkPtr < RESERVE_SIZE_CHK; chkPtr++) {
		check = checksums[chkPtr];
		if ( check == 0xE4E4E4E4 ) continue;
		printf("[%i] = %u\n", ( unsigned int ) chkPtr, check);
	}

	printf("--  END LIST OF FIXED POINTS  --\n");
	return 0;
}
