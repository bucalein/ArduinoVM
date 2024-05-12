#ifndef VM_H
#define VM_H

#include <stddef.h>
#include <string.h>
#include <stdint.h>


/* when a bad instruction is received return this opcode value,
 * which corrisponds to VM_NOP, and does nothing */
#define VM_ERROR  	0xffff

#define CODE_SIZE     250
#define STACK_SIZE    250
#define DATA_SIZE     50

#define VM_INT_PIN2 0
#define VM_INT_PIN3 1


class VM {
    uint8_t  stack[STACK_SIZE];
    uint8_t  sbp,    /* Stack base pointer */
             stp,    /* Stack top pointer */
             fs;     /* Flags register */

            
    uint16_t cs[CODE_SIZE]; /* Code section, where the programs are written */
    uint16_t pc;            /* Program counter, index of memory array*/

    uint32_t RA,
             RB, 
             RC, 
             RD;          

  #ifdef ARDUINO_AVR_UNO
    uint8_t ir[2];          /* only pin 2, 3 are available for interrupts */

    void int_pin2( );
    void int_pin3( );
  #endif
  
    void exec(uint16_t);  /*decode and execute the given opcode*/

    void set_flag(uint8_t);
    void unset_flag(uint8_t);

    void check_result(uint32_t);

    uint32_t sub(uint32_t, uint32_t);
    uint32_t add(uint32_t, uint32_t);

    uint32_t AND(uint32_t, uint32_t);
    uint32_t XOR(uint32_t, uint32_t);
    uint32_t OR(uint32_t, uint32_t);
    uint32_t NOT(uint32_t);

    uint32_t pop(uint8_t);
    uint8_t pop( );
    
    void push(uint8_t);
    void push(uint32_t, uint8_t);
    void set_stack_frame( );

    void print_string(uint8_t );
    void wait( unsigned long (*)( ) );

    uint32_t get_reg(uint16_t);
    
  public:
    VM( );
    
    void LoadOpcode(uint16_t, uint16_t); 	/* opcode, at which address in CS */
    void LoadHexOpcode(uint8_t[ ], uint16_t);
    void Run(uint16_t);                  	/* start running the code at the specified address */
    
  #ifdef _LIB_
    uint16_t Compile(uint8_t*, int);     /* array of text characters */
    void PrintStatus( );
  #endif

  #ifdef _TEST_
    uint32_t GetRA( ),
             GetRB( ),
             GetRC( ),
             GetRD( );

    uint16_t GetPC( );
    uint16_t GetOpcodeAt(uint16_t);
    
    uint8_t GetSBP( );
    uint8_t GetSTP( );
    uint8_t GetFS( );
  #endif

};


/*
 * BIT MASKS
 */

#define VM_REG_MASK     0xe000
#define VM_OP_MASK    	0xff00
#define VM_DAT_MASK   	0x00ff

// we use the Most significant bit inside the opcode
// to treat the snd operand as a register and the value
// of the data field is the corrisponding register index
//
// this is valid only for: RA, RB, RC,RD
#define VM_REG_SND_OP   0x8000

#define VM_PIN_MASK     0x7f
#define VM_LEVEL_MASK   0x80


#define VM_RA         0x0000
#define VM_RB         0x2000
#define VM_RC         0x4000
#define VM_RD         0x6000
#define VM_SBP        0x8000
#define VM_STP        0xa000

#define VM_REG_TO_RA   0x8000
#define VM_REG_TO_RB   0xa000
#define VM_REG_TO_RC   0xc000
#define VM_REG_TO_RD   0xe000


/*---------------*
 *               *
 *    OPCODES    *
 *               *
 *---------------*/
#define VM_EXT             0x0000    /* ext */

/* MOV REG, VAL - Copies VAL into REG */

#define VM_MOVE            0x0100
#define VM_MOVE_VAL_TO_RA  0x0100    /* mov RA, val */
#define VM_MOVE_VAL_TO_RB  0x2100    /* mov RB, val */
#define VM_MOVE_VAL_TO_RC  0x4100    /* mov RC, val */
#define VM_MOVE_VAL_TO_RD  0x6100    /* mov RD, val */

#define VM_MOVE_REG_TO_RA   0x8100    /* mov RA, REG */
#define VM_MOVE_REG_TO_RB   0xa100    /* mov RA, RC */
#define VM_MOVE_REG_TO_RC   0xc100    /* mov RA, RD */
#define VM_MOVE_REG_TO_RD   0xe100    /* mov RB, RA */

#define VM_JMP             0x0200    /* jmp MEM */
#define VM_JMP_ZERO        0x2200    /* jz MEM */
#define VM_JMP_SIGN        0x4200    /* js MEM */
#define VM_JMP_NOT_ZERO	   0x6200    /* jnz MEM */


/* ADD REG, VAL - Add VAL to REG, the result is stored in REG */
#define VM_ADD             0x0300
#define VM_ADD_VAL_TO_RA   0x0300    /* add RA, val */
#define VM_ADD_VAL_TO_RB   0x2300    /* add RB, val */
#define VM_ADD_VAL_TO_RC   0x4300    /* add RC, val */
#define VM_ADD_VAL_TO_RD   0x6300    /* add RD, val */

#define VM_ADD_REG_TO_RA    0x8300    /* add RA, REG */
#define VM_ADD_REG_TO_RB    0xa300    /* add RB, REG */
#define VM_ADD_REG_TO_RC    0xc300    /* add RC, REG */
#define VM_ADD_REG_TO_RD    0xe300    /* add RD, REG */

/* SUB REG, VAL - Subtract VAL to REG, the result is stored in REG */
#define VM_SUB             0x0400
#define VM_SUB_VAL_TO_RA   0x0400    /* sub RA, val */
#define VM_SUB_VAL_TO_RB   0x2400    /* sub RB, val */
#define VM_SUB_VAL_TO_RC   0x4400    /* sub RC, val */
#define VM_SUB_VAL_TO_RD   0x6400    /* sub RD, val */

#define VM_SUB_REG_TO_RA    0x8400    /* sub RA, RB */
#define VM_SUB_REG_TO_RB    0xa400    /* sub RA, RC */
#define VM_SUB_REG_TO_RC    0xc400    /* sub RA, RD */
#define VM_SUB_REG_TO_RD    0xe400    /* sub RA, RD */

/* OR REG, VAL - Make the logic OR between REG and VAL and store the value in REG*/
#define VM_OR              0x0500
#define VM_OR_VAL_TO_RA    0x0500    /* or RA, VAL */
#define VM_OR_VAL_TO_RB    0x2500    /* or RB, VAL */
#define VM_OR_VAL_TO_RC    0x4500    /* or RC, VAL */
#define VM_OR_VAL_TO_RD    0x6500    /* or RD, VAL */

#define VM_OR_REG_TO_RA     0x8500    /* or RA, RB */
#define VM_OR_REG_TO_RB     0xa500    /* or RA, RC */
#define VM_OR_REG_TO_RC     0xc500    /* or RA, RD */
#define VM_OR_REG_TO_RD     0xe500    /* or RA, RD */

/* AND REG, VAL - Make logic AND between REG and VAL and store the value in REG */
#define VM_AND             0x0600
#define VM_AND_VAL_TO_RA   0x0600    /* and RA, VAL */
#define VM_AND_VAL_TO_RB   0x2600    /* and RB, VAL */
#define VM_AND_VAL_TO_RC   0x4600    /* and RC, VAL */
#define VM_AND_VAL_TO_RD   0x6600    /* and RD, VAL */

#define VM_AND_REG_TO_RA    0x8600    /* and RA, RB */
#define VM_AND_REG_TO_RB    0xa600    /* and RA, RC */
#define VM_AND_REG_TO_RC    0xc600    /* and RA, RD */
#define VM_AND_REG_TO_RD    0xe600    /* and RA, RD */

// TODO add a set of instructions to dereference the
// STP pointer and access directly the stack
/* PUSH VAL - push a value or a register onto the stack */
#define VM_PUSH            0x0700
#define VM_PUSH_RA         0x0700    /* push RA | push val */
#define VM_PUSH_RB         0x2700    /* push RB */
#define VM_PUSH_RC         0x4700    /* push RC */
#define VM_PUSH_RD         0x6700    /* push RD */

#define VM_PUSH_SBP        0x8700    /* push SBP */
#define VM_PUSH_STP        0xa700    /* push STP */

/* POP REG - pop a value stored at SSP in the stack and put it to REG,
 *         - "size is how many bytes can be popped: max is 4 */
#define VM_POP             0x0800
#define VM_POP_RA          0x0800    /* pop RA, size */
#define VM_POP_RB          0x2800    /* pop RB, size */
#define VM_POP_RC          0x4800    /* pop RC, size */
#define VM_POP_RD          0x6800    /* pop RD, size */

#define VM_POP_SBP         0x8800    /* pop SBP */
#define VM_POP_STP         0xa800    /* pop STP */

/* XOR REG, VAL - Xor a value in REG with VAL and the result put it to REG */
#define VM_XOR             0x0900
#define VM_XOR_VAL_TO_RA   0x0900    /* xor RA, val */
#define VM_XOR_VAL_TO_RB   0x2900    /* xor RB, val */
#define VM_XOR_VAL_TO_RC   0x4900    /* xor RC, val */
#define VM_XOR_VAL_TO_RD   0x6900    /* xor RD, val */

#define VM_XOR_REG_TO_RA    0x8900    /* xor RA, RB */
#define VM_XOR_REG_TO_RB    0xa900    /* xor RA, RC */
#define VM_XOR_REG_TO_RC    0xc900    /* xor RA, RD */
#define VM_XOR_REG_TO_RD    0xe900    /* xor RA, RD */


/* NOT REG - negate the content of REG: each bit is replaced by its opposite */
#define VM_NOT             0x0a00
#define VM_NOT_RA          0x0a00    /* not RA */
#define VM_NOT_RB          0x2a00    /* not RB */
#define VM_NOT_RC          0x4a00    /* not RC */
#define VM_NOT_RD          0x6a00    /* not RD */


/* CALL REG - jump to the address stored in RA, but saves the content of AC on the stack,
 *          - and sets sbp to point to it */
#define VM_CALL            0x8a00    /* call MEM */


/* RET - return from a subroutine call, set PC to what is stored at SBP,
 *     - and set STP to SBP */
#define VM_RET             0xaa00    /* ret */


#define VM_SET_PIN_OUT	   0x0b00    /* so PIN */
#define VM_SET_PIN_IN      0x2b00    /* si PIN */
#define VM_WRITE_TO_PIN	   0x4b00    /* out PIN */
#define VM_READ_FROM_PIN   0x6b00    /* in PIN */

/* print one or more characters stored at a given address
 * on the stack */
#define VM_PRINT_CHAR      0x8b00    /* prtc MEM */
#define VM_PRINT_STRING	   0xab00    /* prts MEM */
#define VM_WAIT	           0xcb00    /* wait VAL */

/* it sets the address of the ISR and return,
 * it's like a "call" but triggered in hardware */
#define VM_INTERRUPT       0xeb00    /* int VAL */


#define VM_MUL             0x0c00
#define VM_MUL_VAL_TO_RA   0x0c00    /* mul RA, VAL */
#define VM_MUL_VAL_TO_RB   0x2c00    /* mul RB, VAL */
#define VM_MUL_VAL_TO_RC   0x4c00    /* mul RC, VAL */
#define VM_MUL_VAL_TO_RD   0x6c00    /* mul RD, VAL */

#define VM_MUL_REG_TO_RA    0x8c00    /* mul RA, RB */
#define VM_MUL_REG_TO_RB    0xac00    /* mul RA, RC */
#define VM_MUL_REG_TO_RC    0xcc00    /* mul RA, RD */
#define VM_MUL_REG_TO_RD    0xec00    /* mul RA, RD */

#define VM_NOP             0xffff    /* nop */
/*
 * FLAGS
 */
#define VM_ZERO_FLAG     0x1
#define VM_SIGN_FLAG     0x2
#define VM_CARRY_FLAG    0x4
#define VM_OVERFLOW_FLAG 0x8

#define GET_DATA(x) ( x & VM_DAT_MASK )


#endif
