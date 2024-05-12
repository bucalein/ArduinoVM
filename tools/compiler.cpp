#include <cstdio>
#include <cstring> 
#include <cstdlib>

#include "vm.h"


int main(int narg, char **args){
   void *text;
   char *buf;
   VM vm;

   uint16_t *res = (uint16_t*)malloc(1000);
   int i = 0;
   char *colon;

   if(narg < 2){
     fprintf(stderr, "error: missing filename\n");
     return 1;
   }

   FILE *fp_r = fopen(args[1], "r");

   if(fp_r == NULL){
    fprintf(stderr, "error: file not found\n");
    return 2;
   }

   int len, line_num = 0;

   fseek(fp_r, 0, SEEK_END);
   len = ftell(fp_r);
   fseek(fp_r, 0, SEEK_SET);

   text = malloc(len);

   fread(text, 1, len, fp_r);
   buf = strtok((char*)text, "\n");

   while(buf != NULL){

     //empty line
     if(strlen(buf) == 0){
       buf = strtok(NULL, "\n");
       continue;
     }
     
     //comment line
     if(buf[0] == '#'){
       buf = strtok(NULL, "\n");
       continue;
     }

     res[i] = vm.Compile((uint8_t*)buf, strlen(buf));
     i++; 

     buf = strtok(NULL, "\n");
   }
   
   FILE *fp_w = fopen(strcat(args[1], ".bin"), "w");

   fwrite(res, 1, i*2, fp_w);
}

