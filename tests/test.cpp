#include <cstdlib>
#include <cstdio>

#include "test.h"
#include "vm.h"
#include "utils.h"

void test_abort( ){
  printf("\nABORTED\n");
  exit(EXIT_FAILURE);
}

bool test_is_number(uint8_t num[ ], bool expe){
   bool res = is_number(num);

   if(expe != res){
     printf("received: %d\n", res);
     printf("expected: %d\n", expe);
     return false;
   }

   return true;
}

bool test_get_number_from_string(uint8_t num[ ], bool hex, uint16_t expected){
   uint16_t res = get_number_from_string(num, hex);

   if(res != expected){
     printf("received: %x\n", res);
     printf("expected: %x\n", expected);
     return false;
   }

   return true;
   
}


bool test_LoadOpcode(uint16_t opcode, uint16_t at){
   VM vm;

   vm.LoadOpcode(opcode, at);

   if(vm.GetOpcodeAt(at) != opcode)
     return false;

   return true;
}

bool test_LoadHexOpcode(uint8_t opcode[ ], uint16_t at, uint16_t res){
  VM vm;

  vm.LoadHexOpcode(opcode, at);

  if(vm.GetOpcodeAt(at) != res){
    printf("received: %x\n", vm.GetOpcodeAt(at));
    printf("expected: %x\n", res);
    return false;
  }

  return true;
}


bool test_Compile(uint8_t *text, int size, uint16_t expe){
  VM vm;

  if(vm.Compile(text, size) != expe){
    printf("received: %x\n", vm.Compile(text, size));
    printf("expected: %x\n", expe);
    return false;
  }

  return true;
}


bool test_Emulate(const char *filename, debug_t db){
  FILE *input = fopen(filename, "r");
  bool res;

  if(input == NULL){
    printf("%s, \tFile not found\n", filename);
    test_abort( );
  }

  fseek(input, 0, SEEK_END);
  int size = ftell(input);
  fseek(input, 0, SEEK_SET);

  void *text = malloc(size);

  fread(text, 1, size, input);

  res = test_Run((uint16_t*)text, size / 2, 0, db);
  free(text);

  return res;
}


/* it executes only one function */
bool test_Run(uint16_t *code, int size, uint16_t address, debug_t db){
  uint16_t addr = address;
  VM vm;

  for(int i = 0; i < size; i++){

    vm.LoadOpcode(code[i], addr);
    addr++;
  }

  vm.Run(address);

  if(vm.GetRA( ) != db.RA){
    printf("RA received: %x\n", vm.GetRA( ));
    printf("RA expected: %x\n", db.RA);
    return false;
  }

  if(vm.GetRB( ) != db.RB){
    printf("RB received: %x\n", vm.GetRB( ));
    printf("RB expected: %x\n", db.RB);
    return false;
  }

  if(vm.GetRC( ) != db.RC){
    printf("RC received: %x\n", vm.GetRC( ));
    printf("RC expected: %x\n", db.RC);
    return false;
  }

  if(vm.GetRD( ) != db.RD){
    printf("RD received: %x\n", vm.GetRD( ));
    printf("RD expected: %x\n", db.RD);
    return false;
  }

  if(vm.GetSTP( ) != db.stp){
    printf("STP received: %x\n", vm.GetSTP( ));
    printf("STP expected: %x\n", db.stp);
    return false;
  }

  if(vm.GetSBP( ) != db.sbp){
    printf("SBP received: %x\n", vm.GetSBP( ));
    printf("SBP expected: %x\n", db.sbp);
    return false;
  }

  if(vm.GetFS( ) != db.fs){
    printf("FS received: %x\n", vm.GetFS( ));
    printf("FS expected: %x\n", db.fs);
    return false;
  }

  if(vm.GetPC( ) != db.pc){
    printf("PC received: %x\n", vm.GetPC( ));
    printf("PC expected: %x\n", db.pc);
    return false;
  }

  return true;
}
