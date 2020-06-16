/*                               exprt_db.c                                */
/* The purpose of this program is is to output the contents of a parts     */
/* database in a text form recognisable by dbase IV.                       */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

enum format {
   full,
   line};

void main(int argc,
          char *argv[])
   {
//   char string_out[81];
   FILE *fptr_dat;
//   int i;
   struct part_info part_buf;

   if ((fptr_dat = fopen("dbase.dat","rb")) == NULL)
      {
      printf("Cannot open file 'dbase.dat' \n");
      exit(0);
      }
   while(fread(&part_buf,sizeof(part_buf),1,fptr_dat) != 0)
      {
      printf("\"%s\",",part_buf.part_number);
      printf(" \"%s\",",part_buf.description);
      if (part_buf.assembly == true)
         printf(" \"T\",");
      else
         printf(" \"F\",");
      printf(" \"%-f\",",part_buf.trade_cost);
      printf(" \"%-f\",",part_buf.retail_cost);
      printf(" \"%-d\",",part_buf.order_level);
      printf(" \"%-d\"",part_buf.stock);
      printf("\n");
      }
   fclose(fptr_dat);
   }  /* main() */
