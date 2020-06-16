/*                               rd_dbase.c                                */
/* The purpose of this program is to read and display the contents of a    */
/* parts database created by mk_dbase.c.                                   */

/*
RD_DBASE.C

This program writes the contents of a parts database to the screen. The user
may specify that each record is displayed completly or that only a sub-set of
the data is displayed. Displaying only a sub-set allows each record to be
displayed on a single line.
*/

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
   char string_out[81];
   FILE *fptr_dat;
   int i;
   struct part_info part_buf;
   enum format output_format;

   output_format = full;
   if (argc > 1)
      {
      if (strcmp("full",argv[1]) == 0)
         output_format = full;
      else if (strcmp("line",argv[1]) == 0)
         output_format = line;
      }
   if ((fptr_dat = fopen("dbase.dat","rb")) == NULL)
      {
      printf("Cannot open file 'dbase.dat' \n");
      exit(0);
      }
   while(fread(&part_buf,sizeof(part_buf),1,fptr_dat) != 0)
      {
      switch (output_format)
         {
         case full:
            {
	    printf("%s ",part_buf.part_number);
	    if (part_buf.assembly == true)
	       printf("*  ");
	    else
               printf("   ");
            printf("%s\n",part_buf.description);
            printf("Models: ");
            i = 0;
            while (part_buf.models[i] != 0)
	       {
	       printf("%d  ",part_buf.models[i]);
	       i++;
	       }
            printf("\n");
            printf("Stock : %4d   ",part_buf.stock);
            printf("Re-order level : %4d\n",part_buf.order_level);
            printf("Cost : %6.2f    ",part_buf.trade_cost);
            printf("Retail : %6.2f\n",part_buf.retail_cost);
            printf("\n");
            break;
            }
         case line:
            {
            for (i=0; i<81; i++)        /* Clear output line */
	       string_out[i] = ' ';
            str_build_ltd(&string_out[0],part_buf.part_number,10);
            str_build_ltd(&string_out[11],part_buf.description ,50);
	    if (part_buf.assembly == true)
	       string_out[8] = '*';
	    string_out[60] = '\0';
            printf("%s",string_out);
            printf("  %4d   ",part_buf.stock);
            printf("%6.2f\n",part_buf.trade_cost); /*** Should be retail ***/
            break;
            }
         }
      }
   fclose(fptr_dat);
   }
