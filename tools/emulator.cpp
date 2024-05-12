#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "vm.h"

int main(int narg, char **args){
   VM vm;
   bool onestep = false;
   char *filename;

   if(narg < 2){
     fprintf(stderr, "error: missing filename\n");
     return 1;
   }

   filename = args[1];
   FILE *fp_r = fopen(filename, "r");

   if(fp_r == NULL){
    fprintf(stderr, "error: file not found\n");
    return 2;
   }

   int len;

   fseek(fp_r, 0, SEEK_END);
   len = ftell(fp_r);
   fseek(fp_r, 0, SEEK_SET);

   uint16_t *p;
   int bl = 0;
   void *buf = malloc(len);

   fread(buf, 1, len, fp_r);

   for(int i = 0; i < (len/2); i++){
     p = (uint16_t*)buf + i;

     vm.LoadOpcode(*p, bl);
     bl++;
   }

   vm.Run(0);
   vm.PrintStatus( );

}
