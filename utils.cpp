#include "utils.h"


uint32_t ipow(uint32_t base, uint8_t expo){
  uint32_t result = base;

  switch(expo){
    case 0:
      return 1;

    case 1:
      return base;

    default:
      for(int i = 2; i <= expo; ++i)
        result *= base;

      return result;
  }
  
}



/*if there were unexpected characters we remove simply them*/
uint8_t get_digit_from_char(uint8_t c){
  switch(c){
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
       return c - '0';

      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
       return (c - 'a') + 10;

      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
       return (c- 'A') + 10;

      default:
       return 0;

     }
}


/*
 * Convert a hex or dec number into 8bit value:
 * 
 * we consider the default as decimal chars
 */
uint16_t get_number_from_string(uint8_t data_txt[ ], bool is_hex){
   int i = 0, j;

  #define DIGITS_BUFFER_SIZE  10
  #define MS_DIGIT_POSITION   DIGITS_BUFFER_SIZE - 1
  
   uint16_t res = 0;
   uint8_t base = 10,
           digits[DIGITS_BUFFER_SIZE],
           *p;
   
   if(is_hex)
     base = 16;

   j = MS_DIGIT_POSITION;
   
   while(data_txt[i] != 0){
      digits[j] = get_digit_from_char(data_txt[i]);

      j--;
      i++;

   }

   j++;             // get back to the previous element
   i = 0;           // exponent
   p = digits + j;  // p[0] == address of LS digit

   while(j < DIGITS_BUFFER_SIZE){
     res += ipow(base, i) * p[i];

     ++i;
     ++j;
   }

   return res;
}


bool is_number(uint8_t *buf){
   int i = 0;

   while(buf[i] != 0){
     switch(buf[i]){
       case '0':
       case '1':
       case '2':
       case '3':
       case '4':
       case '5':
       case '6':
       case '7':
       case '8':
       case '9':
       case 'a':
       case 'b':
       case 'c':
       case 'd':
       case 'e':
       case 'f':
       case 'A':
       case 'B':
       case 'C':
       case 'D':
       case 'E':
       case 'F':
         break;

       default:
         return false;
     }

     i++;
   }

   return true;
}
