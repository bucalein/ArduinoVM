
----------------
  INTRODUCTION
----------------

ArduinoVM is a framework for hardware testing: it provides a
x86 like virtual machine environment to run programs on the board's RAM
of the AVR microcontroller instead of the FLASH memory.

Not all Arduino functionalities are supported at the moment except
for digitalRead, digitalWrite, pinMode, Serial print and timing functions.

---------
  USAGE
---------

To load the VM onto the Arduino:

  1- open the ArduinoVM.ino project with Arduino
  \
  2- Upload the sketch

Also you will need a compiler for assembling the code: 

  $ cd tools
  \
  $ make compiler

Next, to test how it works you can compile one of the examples:

  $ ./compiler ../examples/blink

Which will produce ../examples/blink.bin that can be
turned into a string of bytes with hexdump:

  $ hexdump -e '32/2 "%04x" "\n"' blink.bin

Finally copy and paste the output to the serial monitor
of Arduino or whichever you're using.

--------
  TODO
--------
Where to go from here? There are a tons of things still to be done,
some examples:

|- write more documentation
\
|- write better code for some functions like VM::Compile
\
|- improve the compiler.cpp : add labels, includes
\
|- add more instructions to the VM: dereference of the stack memory and interrupt routines
\
|- automate the task of compiling and loading the code onto the Arduino with a
\  shell script 


-------------
 PARTICIPATE
-------------
If you like the project and would like to contribute, you're more than welcome! This is
a hobby project, so every contribution makes a difference

------
 HELP 
------
Given the fact that the assembler is very rudimenta; you can use this 
commandline to calculate the addresses to be used with call, jmp instructions:

  $ grep "^[^#]" FILENAME | nl



