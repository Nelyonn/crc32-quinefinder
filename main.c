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
#define TARGET_N 0xFFFFFFFF // Max unsigned 32-bit
#define RESERVE_SIZE_CHK 16

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
	unsigned long long loopPtr = 0;
	unsigned int candidate = 0;
	unsigned int check = 0;

	// Preallocated checksum array (for speed)
	unsigned int checksums[RESERVE_SIZE_CHK] = {0};
	unsigned char chkPtr = 0;

	// Timing
	unsigned long long nsecStart = _getTimeNanoSec();
	unsigned long long nsecEnd;

	// Prose prose prose...
	#ifndef NOLZ
		printf("CRC32 Fixed-point brute forcer (single-threaded)\n");
	#else
		printf("CRC32 Fixed-point brute forcer (single-threaded, no-zlib)\n");
	#endif
	printf("Copyright (C) 2026 Nelyon\n");
	printf("This program is free software, licensed under the GPL v2 (or later), and is provided with ABSOLUTELY NO WARRANTY OF ANY KIND.\n");
	printf("You can get a copy of the GPL v2 at: https://www.gnu.org/licenses/old-licenses/gpl-2.0.html\n");
	printf("\n");
	printf("START timestamp is at  : %llu\n", nsecStart);

	// Run the loop
	#ifdef NOLZ
		fastcrc32_init();
	#endif
	for ( ; loopPtr <= TARGET_N ; loopPtr++ ) {
		candidate = htonl( ( unsigned int ) loopPtr);
		#ifndef NOLZ
			check = crc32(0L, ( const Bytef* ) &candidate, sizeof(candidate));
		#else
			check = fastcrc32(candidate);
		#endif
		if ( check == ( unsigned int ) loopPtr ) {
			checksums[chkPtr] = check;
			chkPtr++;
		}
	};

	// Now enum the results!
	nsecEnd = _getTimeNanoSec();
	
	printf("END timestamp is at    : %llu\n", nsecEnd);
	printf("In seconds             : %f\n", ( double ) ( ( nsecEnd - nsecStart ) / 1e9 ));
	printf("Number of b.forced itrs: %u iterations\n", TARGET_N);
	printf("Detected fixed points  : %i\n", (unsigned int) chkPtr);
	printf("-- START LIST OF FIXED POINTS --\n");

	chkPtr = 0;
	for (; chkPtr < RESERVE_SIZE_CHK; chkPtr++) {
		check = checksums[chkPtr];
		if ( !check ) continue;
		printf("[%i] = %u\n", (unsigned int) chkPtr, check);
	}

	printf("--  END LIST OF FIXED POINTS  --\n");
	return 0;
}
