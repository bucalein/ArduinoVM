#include "vm.h"

void setup( ){
  Serial.begin(9600);
}


//TODO we could write a Wozmon for inspecting the memory

/*
 * Read code from the serial port 
 * and run it 
 */
void loop( ){
  uint8_t buf[CODE_SIZE];
  uint8_t str[5];
  uint8_t c;
  int i = 0;

  uint16_t bl = 0;

  VM vm;

  Serial.println("READY.");

  // read a line of input
  while(true){
    if(Serial.available( ) > 0){
      c = Serial.read( );

      if(c == '\n')
       break;

      buf[i] = c;
      i++;
    }
  }


  // convert hex to bytes: most significant bits
  // must come first
  for(int j = 0; j < i; j+=4){

    for(int k = 0; k < 4; ++k)
      str[k] = buf[j + k];

    str[4] = 0;

    vm.LoadHexOpcode(str, bl);
    bl++;
  }
  
  vm.Run(0);
  
}
