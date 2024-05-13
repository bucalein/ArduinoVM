
----------------
  INTRODUCTION
----------------

ArduinoVM is an Arduino sketch which implements a
x86 like virtual machine for running programs from the AVR microcontroller's RAM instead of the FLASH memory.

This allows you to load a sketch on the board once and run multiple programs from your computer, 
without the need to rewrite the FLASH each time. 

---------
  USAGE
---------

ArduinoVM is loaded onto the board like every other sketch:

  1- open the ArduinoVM.ino project with Arduino
  \
  2- Upload the sketch


For writing programs you will also need a compiler which 
takes a text program as its input and creates a .bin binary file with the relative opcodes.
To build it:

  $ cd tools
  \
  $ make compiler


Example usage:

1 - Compile one of the examples, the
    blinking led, which controls an
    led attached to pin 4:

  $ ./compiler ../examples/blink

Which will produce ../examples/blink.bin that can be
turned into a string of bytes by using hexdump:

  $ hexdump -e '32/2 "%04x" "\n"' blink.bin

Finally copy and paste the output to Arduino' serial monitor
or whichever you're using, for executing.

--------
  TODO
--------
There are a tons of things still to be done,
some examples:

|- write running tests for every opcode
\
|- write better code for some functions like VM::Compile
\
|- improve the compiler.cpp : add labels, includes
\
|- add more instructions to the VM: dereference of the stack memory and interrupt routines
\
|- automate the task of compiling and loading the code onto the Arduino with a shell script 


-------------
 PARTICIPATE
-------------
If you like the project and would like to contribute, you're more than welcome! This is
a hobby project, so every contribution makes a difference

------
 HELP 
------
Given the fact that the assembler is very rudimental: there are yet no labels which can be used as jumping addresses and you have to count manually the instruction address; you can use the following command to get the address of each opcode:

  $ grep "^[^#]" FILENAME | nl



