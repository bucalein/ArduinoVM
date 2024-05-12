#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdlib>

#include "test.h"
#include "vm.h"

#define EXAMPLE_PATH "../examples/tests/"

const char *test_program_txt[ ] = {
  "mov RA, 4", "mov RB, h10", "mov RC, 32", "mov RD, 64",
  "mov RA, RA", "mov RA, RB", "mov RA, RC", "mov RA, RD", 

  "add RA, 2", "add RB, 3", "add RC, 4", "add RD, 5",
  "add RA, RA", "add RA, RB", "add RA, RC", "add RA, RD",

  "sub RA, h5", "sub RB, 90", "sub RC, 40", "sub RD, 15",
  "sub RA, RA", "sub RA, RB", "sub RA, RC", "sub RA, RD",

  "jmp 4", "jz 5", "js 20", "jnz 8",

  "or RA, 5", "or RB, 4", "or RC, 4", "or RD, 4",
  "or RA, RA", "or RA, RB", "or RA, RC", "or RA, RD",
  
  "and RA, 5", "and RB, 4", "and RC, 4", "and RD, 4",
  "and RA, RA", "and RA, RB", "and RA, RC", "and RA, RD",

  "push RA", "push RB", "push RC", "push RD",
  "push SBP", "push STP",

  "pop RA", "pop RB", "pop RC", "pop RD",
  "pop SBP", "pop STP",

  "xor RA,4", "xor RB, 4", "xor RC, 4", "xor RD,4",
  "xor RA, RA", "xor RA, RB", "xor RA, RC", "xor RA, RD",

  "not RA", "not RB", "not RC", "not RD",

  "call",

  "so 3", "si 4", "out 3", "in 4",
  "prtc 4", "prts 5", "wait 1",

  "ext",
  NULL
};


uint16_t test_program[ ] = { 
   0x0104, 0x2110, 0x4120, 0x6140,
   0x8100, 0x8101, 0x8102, 0x8103,

   0x0302, 0x2303, 0x4304, 0x6305,
   0x8300, 0x8301, 0x8302, 0x8303,

   0x0405, 0x245a, 0x4428, 0x640f,
   0x8400, 0x8401, 0x8402, 0x8403,

   0x0204, 0x2205, 0x4214, 0x6208,

   0x0505, 0x2504, 0x4504, 0x6504,
   0x8500, 0x8501, 0x8502, 0x8503,

   0x0605, 0x2604, 0x4604, 0x6604,
   0x8600, 0x8601, 0x8602, 0x8603,

   0x0700, 0x2700, 0x4700, 0x6700,
   0x8700, 0xa700,

   0x0800, 0x2800, 0x4800, 0x6800,
   0x8800, 0xa800,

   0x0904, 0x2904, 0x4904, 0x6904,
   0x8900, 0x8901, 0x8902, 0x8903,

   0x0a00, 0x2a00, 0x4a00, 0x6a00,

   0x8a00,

   0x0b03, 0x2b04, 0x4b03, 0x6b04,

   0x8b04, 0xab05, 0xcb01,

   0x0000
};




int main( ){
  //
  // test_is_number
  //
  printf("test_is_number: ");

  if(!test_is_number((uint8_t*)"245", true))
    test_abort( );

  if(!test_is_number((uint8_t*)"ra3", false))
    test_abort( );

   printf("PASSED\n");

  //
  // test_get_number_from_string
  //
  printf("test_get_number_from_string: ");

  if(!test_get_number_from_string((uint8_t*)"1240", false, 1240))
    test_abort( );

  if(!test_get_number_from_string((uint8_t*)"10", true, 0x0010))
    test_abort( );

  if(!test_get_number_from_string((uint8_t*)"0302", true, 0x0302))
    test_abort( );

  printf("PASSED\n");


  //
  // test_LoadOpcode
  //
  printf("test_LoadOpcode: ");

  if(!test_LoadOpcode(0x0302, 5))
   test_abort( );

  printf("PASSED\n");


  //
  // test_LoadHexOpcode
  //
  printf("test_LoadHexOpcode: ");

  if(!test_LoadHexOpcode((uint8_t*)"h0302", 5, 0x0302))
   test_abort( );

  printf("PASSED\n");


  //
  // test_Compile
  //
  printf("test_Compile: ");

  // test every opcode
  for(int i = 0; i < sizeof(test_program) / sizeof(uint16_t); ++i)
    if(!test_Compile((uint8_t*)test_program_txt[i], strlen(test_program_txt[i]), test_program[i]))
      test_abort( );

  // test bad strings
  const char *str = "   mov  RA,   2";

  if(!test_Compile((uint8_t*)str, strlen(str), 0x0102))
    test_abort( );

  str = "MOV ra, 2";
  if(!test_Compile((uint8_t*)str, strlen(str), 0xffff))
    test_abort( );

  str = "mov RA, ghi";
  if(!test_Compile((uint8_t*)str, strlen(str), 0xffff))
    test_abort( );

  printf("PASSED\n");



  //
  // test_Run
  //
  printf("test_Run: ");

  char *path = (char*)malloc(100);

  memcpy(path, EXAMPLE_PATH, strlen(EXAMPLE_PATH));


  printf("\n\nexample 1, test: add mov\n");

  if(!test_Emulate(strcat(path, "example1.bin"), { 28, 6, 8, 10, 11, STACK_SIZE -1, STACK_SIZE -1, 0 }))
    test_abort( );

  path[ strlen(EXAMPLE_PATH) ] = 0;



  printf("\n\nexample 2, test: jnz\n");

  if(!test_Emulate(strcat(path, "example2.bin"), { 40, 0, 0, 0, 7, STACK_SIZE -1, STACK_SIZE -1, VM_ZERO_FLAG }))
   test_abort( );

  path[ strlen(EXAMPLE_PATH) ] = 0;



  printf("\n\nexample 3, test: js, jmp\n");

  if(!test_Emulate(strcat(path, "example3.bin"), { 7, 4, 5, 6, 11, STACK_SIZE -1, STACK_SIZE -1, VM_SIGN_FLAG }))
    test_abort( );

  path[ strlen(EXAMPLE_PATH) ] = 0;



  printf("\n\nexample 4, test: or\n");

  if(!test_Emulate(strcat(path, "example4.bin"), { 0x1e, 4, 8, 16, 11, STACK_SIZE -1, STACK_SIZE -1, 0 }))
    test_abort( );

  path[ strlen(EXAMPLE_PATH) ] = 0;



  printf("\n\nexample 5, test: and\n");

  if(!test_Emulate(strcat(path, "example5.bin"), { 0, 4, 8, 16, 11, STACK_SIZE -1, STACK_SIZE -1, VM_ZERO_FLAG }))
    test_abort( );

  path[ strlen(EXAMPLE_PATH) ] = 0;
  
  printf("\n\nexample 6, test: push, pop, call, ret\n");

  if(!test_Emulate(strcat(path, "example6.bin"), { 5, 0xf, 0, 0, 8, STACK_SIZE -1, STACK_SIZE -1, 0 }))
    test_abort( );

  
  //TODO write tests for every opcode

  printf("PASSED\n");
}
