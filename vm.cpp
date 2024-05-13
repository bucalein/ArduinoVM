/*
 * x86 inspired virtual machine.
 * 
 *  16bit opcodes, from most significant to less:
 *  - 3 bits - register code: RA, RB, RC, RD, SBP, STP
 *  - 5 bits - opcode
 *  - 8 bits - data
 *  
 *  FLAGS register, from less to most significant:
 *  - ZERO flag
 *  - SIGN flag
 *  - CARRY flag
 *  - OVERFLOW flag
 */

#include "vm.h"
#include "utils.h"

/* _LIB_ contains all the code that we don't
 * need to upload on the Arduino */

#ifndef _LIB_
 #include "Arduino.h"
#endif

#include <stdint.h>

void VM::LoadOpcode(uint16_t opcode, uint16_t at){
   this->cs[at] = opcode;
}

//there must be 4 characters
void VM::LoadHexOpcode(uint8_t *opcode, uint16_t at){
   uint16_t opc = get_number_from_string(opcode, true);

   this->cs[at] = opc;
}

#ifdef _LIB_ /* _LIB_ */
#include <cstdio>

void VM::PrintStatus( ){
  printf("\nopcode at PC: %x\n", this->cs[this->pc]);
  printf("PC: %x\n\n", this->pc);
  printf("RA: %x\n", this->RA);
  printf("RB: %x\n", this->RB);
  printf("RC: %x\n", this->RC);
  printf("RD: %x\n\n", this->RD);

  printf("SBP: %x\n", this->sbp);
  printf("STP: %x\n\n", this->stp);

  printf("Z S C O\n");
  printf("%c %c %c %c\n\n",
	(this->fs & VM_ZERO_FLAG) > 0 ? '1' : '0',
	(this->fs & VM_SIGN_FLAG) > 0 ? '1' : '0',
	(this->fs & VM_CARRY_FLAG) > 0 ? '1' : '0',
	(this->fs & VM_OVERFLOW_FLAG) > 0 ? '1' : '0');
}

/*
 * First we parse all words inside the line, next we 
 * assign the corrisponding value to each instruction
 * 
 * TODO adjust this code into multiple functions
 */
uint16_t VM::Compile(uint8_t buf[ ], int len){
   int i = 0, j = 0;

   uint16_t OPCODE = 0;

   uint8_t t_opcode[10];
   uint8_t t_reg[5];
   uint8_t t_val[10];
      
   const char *regs_txt[ ] = {
     "RA", "RB", "RC", "RD",
     "SBP", "STP", NULL
   };

   uint16_t regs_val[ ] = {
     VM_RA, VM_RB, VM_RC, VM_RD,
     VM_SBP, VM_STP
   };
  

   const char *opcodes_txt[ ] = { 
     "ext", "mov",  "ret", "jmp", "jz", 
     "add",  "sub", "or",  "and",  
     "push", "pop", "xor", "not", 
     "call", "js", "jnz",
     "so", "si", "out", "in",
     "prtc", "prts", "wait", 
     "mul", "nop", NULL
   };


   uint16_t opcodes_val[ ] = {
     VM_EXT, VM_MOVE, VM_RET, VM_JMP, VM_JMP_ZERO,
     VM_ADD, VM_SUB, VM_OR, VM_AND,
     VM_PUSH, VM_POP, VM_XOR, VM_NOT,
     VM_CALL, VM_JMP_SIGN, VM_JMP_NOT_ZERO,
     VM_SET_PIN_OUT, VM_SET_PIN_IN, VM_WRITE_TO_PIN, VM_READ_FROM_PIN,
     VM_PRINT_CHAR, VM_PRINT_STRING, VM_WAIT,
     VM_MUL, VM_NOP
   };


   // skip first whitespaces if present
   while(buf[i] == ' ' && i < len)
    i++;

   //
   // 1- read opcode
   //
   while(buf[i] != ' ' && i < len){
     t_opcode[j] = buf[i];
     
     i++;
     j++;
   }

   t_opcode[j] = 0;
   
   //skip white spaces after the opcode
   while(buf[i] == ' ' && i < len)
     i++;



   //
   // 2- read first argument if present
   //
   
   j = 0;
   
   while(buf[i] != ',' && buf[i] != ' ' && i < len){
    t_reg[j] = buf[i];
    
    i++;
    j++;
   }

   t_reg[j] = 0;

   
   // skip white spaces and comma
   while( (buf[i] == ',' || buf[i] == ' ') && i < len)
     i++;

     

   //
   // 3- read second argument if it's there
   //
   
   j = 0;
   
   while(buf[i] != ' ' && i < len){
    t_val[j] = buf[i];
    
    i++;
    j++;
   }

   t_val[j] = 0;


   /* get the right opcode */
   int k = 0;
   bool found = false;

   while(opcodes_txt[k] != NULL){

     if(strcmp((char *)t_opcode, opcodes_txt[k]) == 0){
       OPCODE |= opcodes_val[k];
       found = true;
       break;
     }

     k++;
   }

   /* if it's an unknown instruction, return */
   if(!found)
     return VM_ERROR;



   //
   // Analyze first operand: number or register
   //
   bool is_hex  = false;
   uint8_t *t2_reg = t_reg;

   if(t2_reg[0] == 'h'){
     t2_reg++;
     is_hex = true;
   }

   if( is_number(t2_reg) ){
     OPCODE |= get_number_from_string(t2_reg, is_hex);
     return OPCODE;
   }


   
   /* check if it's a known register */
   k = 0;
   found = false;

   while(regs_txt[k] != NULL){

      if(strcmp((char*)t2_reg, regs_txt[k]) == 0){
        OPCODE |= regs_val[k];
        found = true;
        break;
      }

      k++;
   }

   if(!found)
     return VM_ERROR;


   //
   //  Analyze second operand: either a number or register
   //
   is_hex  = false;
   uint8_t *t2_val = t_val;

   if(t2_val[0] == 'h'){
     t2_val++;
     is_hex = true;
   }

   // if it's not a register then
   if( is_number(t2_val) ){
     OPCODE |= get_number_from_string(t2_val, is_hex);
     return OPCODE;
   }

   k = 0;
   found = false;

   while(regs_txt[k] != NULL){

      if(strcmp((char*)t2_val, regs_txt[k]) == 0){
        OPCODE |= VM_REG_SND_OP | k;
        found = true;
        break;
      }

      k++;
   }

   if(!found)
    return VM_ERROR;
   
   return OPCODE;
   
}
#endif /* _LIB_ */



void VM::set_flag(uint8_t flag){
   this->fs |= flag;
}


void VM::unset_flag(uint8_t flag){
   this->fs &= ~flag;
}


uint32_t VM::add(uint32_t reg, uint32_t val){
   /* 
    * if the result MSB bit is set, if the operands
    * were unsigned it tells the user not to consider 
    * the result as positive
    */
   if( (reg + val) & (1 << 31) )
      set_flag(VM_OVERFLOW_FLAG);
   else unset_flag(VM_OVERFLOW_FLAG);

   if( (reg + val) > UINT32_MAX)
      set_flag(VM_CARRY_FLAG);
   else unset_flag(VM_CARRY_FLAG);

   return reg + val;
}


/* val can be a register or direct value */
uint32_t VM::sub(uint32_t reg, uint32_t val){
   
   if(reg < val)
     set_flag(VM_SIGN_FLAG);
   else unset_flag(VM_SIGN_FLAG);

   if(reg == val)
     set_flag(VM_ZERO_FLAG);
   else unset_flag(VM_ZERO_FLAG);

   return reg - val;

}


/*
 *  STACK
 */

void VM::push(uint8_t val){
  
   if(this->stp == 0)
     this->stp = STACK_SIZE -1;
   
   this->stack[ this->stp ] = val;
   
   #ifdef _TEST_
     printf("stack[ %x ]: %x\n", this->stp, this->stack[ this->stp ]);
   #endif
   
   this->stp--;
}

/*
 * reg - value to store on the stack
 * size - how many bytes reg is formed of
 * 
 * we put the less significant bytes first,
 * at a higher address in the stack
 */
void VM::push(uint32_t reg, uint8_t size){ 
   int i = 0;

   //at higher address there's the LSB
   uint8_t *data = (uint8_t*)&reg;

   while(i < size){  
     this->push(data[i]);   
     i++;
   }
   
}

uint8_t VM::pop( ){
   /* end of stack? return what's at that address
    * but don't update STP */
   if( this->stp  >= (STACK_SIZE -1))
     return this->stack[ this->stp ];

   this->stp++; 
       
   #ifdef _TEST_
      printf("Pop %x\tstack[ %x ]\n", this->stack[ this->stp ], this->stp);
   #endif
   
   return this->stack[ this->stp ];
}


/* pop stuff from the stack by one byte at a time
 * depending on the size elements required return 
 * the proper value */
uint32_t VM::pop(uint8_t size){
   uint8_t data;
   int i = 1;

   uint32_t ret = 0;

   //the first byte we get is the most significant byte
   while(i <= size){
     data = this->pop( );
     ret |= data << ((size - i)*8);
     
     i++;     
   }

   return ret;

}


// push first SBP and then PC
void VM::set_stack_frame( ){
   this->push(this->sbp, sizeof(this->sbp));
   this->push(this->pc, sizeof(this->pc));

   // point to the stored PC value on the stack
   this->sbp = this->stp;
}



void VM::check_result(uint32_t reg){
   if( reg == 0)
    set_flag(VM_ZERO_FLAG);
   else unset_flag(VM_ZERO_FLAG);

   if( reg & (1 << 31))
    set_flag(VM_SIGN_FLAG);
   else unset_flag(VM_SIGN_FLAG);
}



uint32_t VM::OR(uint32_t reg, uint32_t val){
   uint32_t res = reg | val;

   check_result(res);
   return res;
}


uint32_t VM::AND(uint32_t reg, uint32_t val){
   uint32_t res = reg & val;

   check_result(res);
   return res;
}


uint32_t VM::XOR(uint32_t reg, uint32_t val){
   uint32_t res = reg ^ val;

   check_result(res);
   return res;
}


uint32_t VM::NOT(uint32_t reg){
   uint32_t res = ~reg;

   check_result(res);
   return res;
}


#ifndef _LIB_

#ifdef ARDUINO_AVR_UNO

// these functions will execute everytime an interrupt is triggered
// and set PC to the interrupt routine
void VM::int_pin2( ){
  this->pc = this->ir[VM_INT_PIN2];
}

void VM::int_pin3( ){
  this->pc = this->ir[VM_INT_PIN3];
}
#endif



/* "where" is the start of the string base address,
 * at this location must be stored the first char */
void VM::print_string(uint8_t where){
   int i = 0;

   while(this->stack[where - i] != 0){

     Serial.write(this->stack[where -i]);
     i++;

   }
}

#endif



void VM::wait( unsigned long (*timestamp)( ) ){
   unsigned long start = (*timestamp)( );

   while( ((*timestamp)( ) - start) < this->RA ){ }
}


uint32_t VM::get_reg(uint16_t opc){
  switch(GET_DATA(opc)){
    case 0:
      return this->RA;

    case 1:
      return this->RB;

    case 2:
      return this->RC;

    case 3:
      return this->RD;

    default:
      return 0;
  }
}





/*
 * Here we decode the opcode and run 
 * the appropriate function
 */
void VM::exec(uint16_t opcode){

  switch(opcode & VM_OP_MASK){
	  
    /*
     * MOV 
     */
    case VM_MOVE_VAL_TO_RA:
      this->RA = GET_DATA(opcode);
      break;

    case VM_MOVE_VAL_TO_RB:
      this->RB = GET_DATA(opcode);
      break;

    case VM_MOVE_VAL_TO_RC:
      this->RC = GET_DATA(opcode);
      break;

    case VM_MOVE_VAL_TO_RD:
      this->RD = GET_DATA(opcode);
      break;


    case VM_MOVE_REG_TO_RA:
      this->RA = this->get_reg(opcode);
      break;

    case VM_MOVE_REG_TO_RB:
      this->RB = this->get_reg(opcode);
      break;

    case VM_MOVE_REG_TO_RC:
      this->RC = this->get_reg(opcode);
      break;

    case VM_MOVE_REG_TO_RD:
      this->RD = this->get_reg(opcode);
      break;



    /*
     * RET
     */
    case VM_RET:
      this->pc  = this->pop(sizeof(uint16_t));	// get the PC address
      this->sbp = this->pop(sizeof(uint8_t)); // get the SBP, Base Stack pointer of the previous function

      break;


    /* 
     * JUMP 
     */      
    case VM_JMP:
      this->pc = GET_DATA(opcode);
      return;

    case VM_JMP_ZERO:
      if(this->fs & VM_ZERO_FLAG){
        this->pc = GET_DATA(opcode);
        return;
      }

      break;

    case VM_JMP_SIGN:
      if(this->fs & VM_SIGN_FLAG){
        this->pc = GET_DATA(opcode);
        return;
      }

      break;

    case VM_JMP_NOT_ZERO:
     if( (this->fs & VM_ZERO_FLAG) != VM_ZERO_FLAG ){
       this->pc = GET_DATA(opcode);
       return;
     }

     break;
      

    /*
     * ADD 
     */
    case VM_ADD_VAL_TO_RA:
      this->RA = this->add(this->RA, GET_DATA(opcode));
      break;

    case VM_ADD_VAL_TO_RB:
      this->RB = this->add(this->RB, GET_DATA(opcode));
      break;

    case VM_ADD_VAL_TO_RC:
      this->RC = this->add(this->RC, GET_DATA(opcode));
      break;

    case VM_ADD_VAL_TO_RD:
      this->RD = this->add(this->RD, GET_DATA(opcode));
      break;
      

    case VM_ADD_REG_TO_RA:
      this->RA = this->add(this->RA, this->get_reg(opcode));
      break;

    case VM_ADD_REG_TO_RB:
      this->RB = this->add(this->RB, this->get_reg(opcode));
      break;

    case VM_ADD_REG_TO_RC:
      this->RC = this->add(this->RC, this->get_reg(opcode));
      break;

    case VM_ADD_REG_TO_RD:
      this->RD = this->add(this->RD, this->get_reg(opcode));
      break;

        
    /*
     * SUB 
     */
    case VM_SUB_VAL_TO_RA:
      this->RA = this->sub(this->RA, GET_DATA(opcode));
      break;

    case VM_SUB_VAL_TO_RB:
      this->RB = this->sub(this->RB, GET_DATA(opcode));
      break;

    case VM_SUB_VAL_TO_RC:
      this->RC = this->sub(this->RC, GET_DATA(opcode));
      break;

    case VM_SUB_VAL_TO_RD:
      this->RD = this->sub(this->RD, GET_DATA(opcode));
      break;


    case VM_SUB_REG_TO_RA:
      this->RA = this->sub(this->RA, this->RB);
      break;

    case VM_SUB_REG_TO_RB:
      this->RB = this->sub(this->RB, this->get_reg(opcode));
      break;

    case VM_SUB_REG_TO_RC:
      this->RC = this->sub(this->RC, this->get_reg(opcode));
      break;
      
    case VM_SUB_REG_TO_RD:
      this->RD = this->sub(this->RD, this->get_reg(opcode));
      break;



    /*
     * OR 
     */
    case VM_OR_VAL_TO_RA:
      this->RA = this->OR(this->RA, GET_DATA(opcode));
      break;

    case VM_OR_VAL_TO_RB:
      this->RB = this->OR(this->RB, GET_DATA(opcode));
      break;

    case VM_OR_VAL_TO_RC:
      this->RC = this->OR(this->RC, GET_DATA(opcode));
      break;

    case VM_OR_VAL_TO_RD:
      this->RD = this->OR(this->RD, GET_DATA(opcode));
      break;


    case VM_OR_REG_TO_RA:
      this->RA = this->OR(this->RA, this->get_reg(opcode));
      break;

    case VM_OR_REG_TO_RB:
      this->RB = this->OR(this->RB, this->get_reg(opcode));
      break;

    case VM_OR_REG_TO_RC:
      this->RC = this->OR(this->RC, this->get_reg(opcode));
      break;

    case VM_OR_REG_TO_RD:
      this->RD = this->OR(this->RD, this->get_reg(opcode));
      break;





    /*
     * AND 
     */
    case VM_AND_VAL_TO_RA:
      this->RA = this->AND(this->RA, GET_DATA(opcode));
      break;

    case VM_AND_VAL_TO_RB:
      this->RB = this->AND(this->RB, GET_DATA(opcode));
      break;

    case VM_AND_VAL_TO_RC:
      this->RC = this->AND(this->RC, GET_DATA(opcode));
      break;

    case VM_AND_VAL_TO_RD:
      this->RD = this->AND(this->RD, GET_DATA(opcode));
      break;


    case VM_AND_REG_TO_RA:
      this->RA = this->AND(this->RA, this->get_reg(opcode));
      break;

    case VM_AND_REG_TO_RB:
      this->RB = this->AND(this->RB, this->get_reg(opcode));
      break;

    case VM_AND_REG_TO_RC:
      this->RC = this->AND(this->RC, this->get_reg(opcode));
      break;
      
    case VM_AND_REG_TO_RD:
      this->RD = this->AND(this->RD, this->get_reg(opcode));
      break;


    /*
     * PUSH
     */

    // if the data byte is not null, we push it
    // onto the stack instead of RA, this approach
    // won't allow us to push a 0
    case VM_PUSH:
      if(GET_DATA(opcode) != 0){
        this->push(GET_DATA(opcode), 1);
        break;
      }
      
      this->push(this->RA, sizeof(this->RA));
      break;
     
    case VM_PUSH_RB:
      this->push(this->RB, sizeof(this->RB));
      break;

    case VM_PUSH_RC:
      this->push(this->RC, sizeof(this->RC));
      break;

    case VM_PUSH_RD:
      this->push(this->RD, sizeof(this->RD));
      break;

    case VM_PUSH_SBP:
      this->push(this->sbp, sizeof(this->sbp));
      break;

    case VM_PUSH_STP:
      this->push(this->stp, sizeof(this->stp));
      break;
      

    /*
     * POP 
     */
    case VM_POP_RA:
      this->RA = this->pop(GET_DATA(opcode));
      break;

    case VM_POP_RB:
      this->RB = this->pop(GET_DATA(opcode));
      break;

    case VM_POP_RC:
      this->RC = this->pop(GET_DATA(opcode));
      break;

    case VM_POP_RD:
      this->RD = this->pop(GET_DATA(opcode));
      break;

    //ignore the opcode if present, we can only pop one byte
    case VM_POP_SBP:
      this->sbp = this->pop(sizeof(this->sbp));
      break;

    case VM_POP_STP:
      this->stp = this->pop(sizeof(this->stp));
      break;



    /*
     * XOR 
     */
    case VM_XOR_VAL_TO_RA:
      this->RA = this->XOR(this->RA, GET_DATA(opcode));
      break;

    case VM_XOR_VAL_TO_RB:
      this->RB = this->XOR(this->RB, GET_DATA(opcode));
      break;

    case VM_XOR_VAL_TO_RC:
      this->RC = this->XOR(this->RC, GET_DATA(opcode));
      break;

    case VM_XOR_VAL_TO_RD:
      this->RD = this->XOR(this->RD, GET_DATA(opcode));
      break;


    case VM_XOR_REG_TO_RA:
      this->RA = this->XOR(this->RA, this->get_reg(opcode));
      break;

    case VM_XOR_REG_TO_RB:
      this->RB = this->XOR(this->RB, this->get_reg(opcode));
      break;

    case VM_XOR_REG_TO_RC:
      this->RC = this->XOR(this->RC, this->get_reg(opcode));
      break;

    case VM_XOR_REG_TO_RD:
      this->RD = this->XOR(this->RD, this->get_reg(opcode));
      break;

    /*
     * NOT
     */
    case VM_NOT_RA:
      this->RA = this->NOT(this->RA);
      break;

    case VM_NOT_RB:
      this->RB = this->NOT(this->RB);
      break;

    case VM_NOT_RC:
      this->RC = this->NOT(this->RC);
      break;

    case VM_NOT_RD:
      this->RD = this->NOT(this->RD);
      break;



    /*
     * CALL
     */
    case VM_CALL:
      this->set_stack_frame( );
      this->pc = GET_DATA(opcode);
      return;


    /*
     * MUL
     */
    case VM_MUL_VAL_TO_RA:
      this->RA *= GET_DATA(opcode);
      break;

    case VM_MUL_VAL_TO_RB:
      this->RB *= GET_DATA(opcode);
      break;

    case VM_MUL_VAL_TO_RC:
      this->RC *= GET_DATA(opcode);
      break;


    case VM_MUL_REG_TO_RA:
      this->RA *= this->get_reg(opcode);
      break;

    case VM_MUL_REG_TO_RB:
      this->RA *= this->get_reg(opcode);
      break;

    case VM_MUL_REG_TO_RC:
      this->RA *= this->get_reg(opcode);
      break;

    case VM_MUL_REG_TO_RD:
      this->RD *= this->get_reg(opcode);
      break;

 #ifndef _LIB_ /* _LIB_ */
    /*
     * ARDUINO CODE
     */
     
    case VM_SET_PIN_OUT:
      pinMode(GET_DATA(opcode), OUTPUT);
      break;

    case VM_SET_PIN_IN:
      pinMode(GET_DATA(opcode), INPUT);
      break;


    // 8th bit is the value to be written, while the
    // first 7bit point to the pin to write to
    case VM_WRITE_TO_PIN:
      digitalWrite(GET_DATA(opcode) & VM_PIN_MASK, GET_DATA(opcode) & VM_LEVEL_MASK);
      break;

    // the read value is stored in RA
    case VM_READ_FROM_PIN:
      this->RA = digitalRead(GET_DATA(opcode) & VM_PIN_MASK);
      break;


    case VM_PRINT_CHAR:
      Serial.write( this->stack[ GET_DATA(opcode) ]);
      break;

    case VM_PRINT_STRING:
      this->print_string(GET_DATA(opcode));
      break;


    // opcode if 0 = micros, 1 = millis, RA stores the actual value 
    // to wait for
    case VM_WAIT:
      this->wait(GET_DATA(opcode) > 0 ? millis : micros);
      break;


    // RA holds the address where the IRS is
    // RB has the value for the interrupt condition:
    //
    // RB = 0 : LOW
    // RB = 1 : CHANGE
    // RB = 2 : FALLING
    // RB = 3 : RISING
    
    case VM_INTERRUPT:
  #ifdef ARDUINO_AVR_UNO
     // TODO the object functions can't be passed to the 
     // attachInterrupt function, we need to find another way
  #endif

  #endif /* _LIB_ */


    case VM_NOP:
      break;

    /* if we couldnt recognize the opcode exit */
    default:
      break;
    
  }

  pc++;
  
}


/* we set up the memory and registers to be used */
VM::VM( ){
  this->sbp = STACK_SIZE -1, 
  this->stp = STACK_SIZE -1,
  this->pc = 0;

  this->fs = 0;
}


/*
 * we return from the execution when we encounter the
 * NULL opcode
 */
void VM::Run(uint16_t program_address){

  this->pc = program_address;
    
    #ifdef _TEST_
      this->PrintStatus( );
    #endif
  
  while(this->cs[this->pc] != 0){

    this->exec( this->cs[this->pc] );
    
    #ifdef _TEST_
      this->PrintStatus( );
    #endif
  }
    
}


#ifdef _TEST_ /* GETTERS */
uint32_t VM::GetRA( ){
  return this->RA;
}

uint32_t VM::GetRB( ){
  return this->RB;
}

uint32_t VM::GetRC( ){
  return this->RC;
}

uint32_t VM::GetRD( ){
  return this->RD;
}


uint16_t VM::GetPC( ){
  return this->pc;
}

uint8_t VM::GetSBP( ){
  return this->sbp;
}

uint8_t VM::GetSTP( ){
  return this->stp;
}


uint8_t VM::GetFS( ){
  return this->fs;
}

uint16_t VM::GetOpcodeAt(uint16_t at){
  return this->cs[at];
}

#endif
