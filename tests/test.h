#ifndef TEST_H
#define TEST_H

#include <cstdint>

struct debug_t {
   uint32_t RA, RB, RC, RD;

   uint16_t pc;

   uint8_t sbp,
	   stp,
	   fs;
};

bool test_is_number(uint8_t [ ], bool);
bool test_get_number_from_string(uint8_t [ ], bool, uint16_t);

bool test_Run(uint16_t [ ], int, uint16_t, debug_t);
bool test_Emulate(const char *, debug_t);
bool test_Compile(uint8_t*, int, uint16_t);
bool test_LoadHexOpcode(uint8_t [ ], uint16_t, uint16_t);
bool test_LoadOpcode(uint16_t, uint16_t);

void test_abort( );

#endif 
