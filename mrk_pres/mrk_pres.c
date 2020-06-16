/*                         mrk_pres.c                                     */
/* Given a text file where each line starts with a part number the        */
/* purpose of this program is to indicate whether part number is in part  */
/* database */

/*
MRK_PRES.C

Given a text file where each line starts with a part number the purpose of
this program is to indicate whether that part is contained in the specified
database. The output is directed to the screen.

Input arguments are the parts list text file, the database, and optionally
the position for the marker, and also the form of the marker. If no optional
arguments are supplied the marker is a '*' positioned in column 75.

E.g. C:> MRK_PRES somefile.txt database.dat 50 pu5
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
   FILE *fptr_db;
   struct part_info *base_ptr;
   struct part_info *part_data;
   int i;
   int position;
   int place;
   int max_description_length;
   char string_in[101];
   char string_out[101];
   char part_number[10];
   char description[80];
   char dummy_string[100];
   char marker[10];
   enum boolean print_string_in;
   enum boolean six_digit_part_no = true;

   if ((fptr_in = fopen(argv[1],"r")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }
   if ((fptr_db = fopen(argv[2],"rb")) == NULL)
      {
      printf("Cannot open file %s \n",argv[2]);
      exit(0);
      }
   /*
    * Determine what data is to be displayed
    */
   if (argc > 3)
      {
      if ((place = atoi(argv[3])) == 0) 
         {
         printf("Incorrect position argument\n");
         exit(1);
         }
      }
   else
      place = 75;

   if (argc > 4)
      {
      strcpy(marker,argv[4]);
      }
   else
      strcpy(marker,"*");

   max_description_length = place - 9;

   if ((base_ptr = read_tgt_dbase(fptr_db)) == NULL)
      {
      printf("Insufficient memory for target database\n");
      exit(0);
      }
   while(fgets(string_in, 100, fptr_in) != NULL)
      {
      /* Analyse each line */
      print_string_in = false;
      if (is_blank_line(string_in) == 0)
	 {
         for (i=0; i<81; i++)        /* Clear output line */
            string_out[i] = ' ';
	 position = 0;
	 if (check_part_number(string_in,  /* Need to consider 'blank' part numbers */
	                       dummy_string,
			       six_digit_part_no,
			       &position) == 1)
	    {
            position = 0;
	    get_part_number(string_in,
			    part_number,
                            true,
			    position);
            position = 8;
	    get_description(string_in,
			    description,
	                    max_description_length,
			    position);
	    str_build(string_out,
                      part_number);
	    str_build_ltd(&string_out[8],
                          description,
                          max_description_length);

            if ((part_data = find_record(part_number,
			                 base_ptr)) != NULL)
	       {
	       str_build(&string_out[place],
                         marker);
	       }
	    else
	       {
               /* Part number not found in database */
               ;
	       }
	    }
         else
	    print_string_in = true;
	 }
      else
	 print_string_in = true;
      if (print_string_in == true)
         printf("%s",string_in);
      else
	 {
	 string_out[80] = '\n';
	 string_out[81] = '\0';
	 printf("%s",string_out);
         }
      }
   }  /* main() */
