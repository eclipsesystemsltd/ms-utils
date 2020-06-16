/*                           itoa_prt.c                                   */

/* Additions : */
/* Need to handle ^alternative_ part number conversion. */

/*
ITOA_PRT.C

This program does the reverse of ATOI_PRT.C.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

void main(int argc,
	  char *argv[])
   {
   FILE *fptr_in;
   char string_in[101];
   char string_out[101];
   char part_number_string[20];
   enum boolean six_digit_part_no;
   int i;
   int position;

   if ((fptr_in = fopen(argv[1],"r")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }

   while(fgets(string_in, 100, fptr_in) != NULL)
      {
      /* Analyse each line */
      if (is_blank_line(string_in) == 1)  /* i.e. really a comment line */
	 {
	 printf("%s",string_in);
         }
      else if (string_in[0] == '*')       /* i.e. a directive */
	 {
	 printf("%s",string_in);
	 }
      else                                /* Must be a part */
	 {
         position = 0;
	 if (check_ref_number(string_in,
			      string_out,
			      &position) != 0)
	    {
	    six_digit_part_no = false;
	    for (i=0; i<20; i++)
	       part_number_string[i] = ' ';
	    if (check_part_number(string_in,
				  part_number_string,
				  six_digit_part_no,
				  &position) == 1)
	       {
	       for (i=0; i<7; i++)
		  string_in[part_number_position + i] = ' ';
	       i = part_number_position;
	       while (part_number_string[i] != ' ')
		  {
		  string_in[i] = part_number_string[i];
                  i++;
		  }
	       }
	    }
	 else
	    {
            for (i=0; i<3; i++)
	       string_in[i] = 'X';
	    }
	 printf("%s",string_in);
	 }
      }
   _fcloseall();
   }  /* main() */
