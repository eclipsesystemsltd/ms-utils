/*                           atoi_prt.c                                   */
/* The purpose of this program is to take a parts list where the part     */
/* numbers are in alpha-numeric form and to generate an identical parts   */
/* list but with all part numbers in the 6 digit numerical form.          */

/* Additions : */
/* Need to handle ^alternative_ part number conversion. */

/*
ATOI_PRT.C

The purpose of this program is to take a parts list where the part numbers are
in alpha-numeric form and to generate an identical parts list but with all
part numbers in the 6 digit numerical form. 
*/

#include <stdio.h>
#include <stdlib.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

void main(int argc,
	  char *argv[])
   {
   FILE *fptr_in;
   FILE *fptr_out;
   char string_in[201];
   char string_out[201];
   char part_number_string[25];
   int i;
   int position;
   int out;
   enum boolean six_digit_part_no = true;

   if ((fptr_in = fopen(argv[1],"r")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }
   if (argc >= 3)
      {
      if (argv[2][0] == '/' &&
	  argv[2][1] == 'l')
	 six_digit_part_no = false;
      else
	 if ((fptr_out = freopen(argv[2],"w", stdout)) == NULL)
	    {
	    printf("Cannot open file %s \n", argv[2]);
	    exit(0);
	    }
      }
   if (argc >= 4)
      {
      if (argv[3][0] == '/' &&
	  argv[3][1] == 'l')
	 six_digit_part_no = false;
      }

   while(fgets(string_in, 200, fptr_in) != NULL)
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
	    for (i=0; i<20; i++)
               part_number_string[i] = ' ';
	    if (check_alpha_part_number(string_in,
	                                part_number_string,
				        six_digit_part_no,
				        &position) == 1)
	       {
	       for (i=0; i<7; i++)
		  string_in[part_number_position + i] = ' ';
	       i= part_number_position;
	       while (part_number_string[i] != ' ')
		  {
		  string_in[i] = part_number_string[i];
                  i++;
		  }
	       }
	    else
	       string_in[part_number_position + 6] = '?';
	    }
	 else
	    {
            for (i=0; i<3; i++)
	       string_in[i] = 'X';
	    }
	 printf("%s",string_in);
	 }
      }
   out = _fcloseall();
   }  /* main() */



