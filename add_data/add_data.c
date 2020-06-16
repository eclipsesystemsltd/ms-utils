/*                         add_data.c                                     */
/* Given a text file where each line starts with a part number the        */
/* purpose of this program is to add selected data from the part database */
/* to the end of each line. */

/*
ADD_DATA.C

Given a text file where each line starts with a part number the purpose of
this program is to add selected data from the part database to the end of each
line. The output is directed to the screen.

The database name is the second argument.

Valid switches are: \Q (quantity), \R (retail), \T (trade), \RO (re-order),
                    \D (description).

If \D is used the description is added immediately after the part number; the
\D switch if used must be the last switch on the command line.

E.g. C:> ADD_DATA somefile.lst database.dat \Q \R
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <alloc.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

#define item_1_position = 75
#define item_2_position = 70
#define item_3_position = 65
#define item_4_position = 60
#define item_5_position = 55
#define item_6_position = 50

int positions[] = {78,72,66,60,54,48};

void main(int argc,
	  char *argv[])
   {
   FILE *fptr_in;
   FILE *fptr_db;
   struct part_info *base_ptr;
   struct part_info *part_data;
   int d_stock = 0;     /* Indicates whether to display item and its order */
   int d_retail = 0;
   int d_trade = 0;
   int d_reorder = 0;
   int d_desc = 0;
   int p_stock = 0;     /* Indicates where to display item */
   int p_retail = 0;
   int p_trade = 0;
   int p_reorder = 0;
   int i, j;
   int items;
   int position;
   int max_description_length;
   char string_in[101];
   char string_out[101];
   char part_number[10];
   char description[80];
   char dummy_string[100];
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
   items = 0;
   for (i=3; i < argc; i++)
      {
      if (strcmp(argv[i],"\\Q") == 0)
         {
	 d_stock = i - 2;
	 items++;
         }
      else if (strcmp(argv[i],"\\R") == 0)
         {
	 d_retail = i - 2;
         items++;
         }
      else if (strcmp(argv[i],"\\T") == 0)
         {
	 d_trade = i - 2;
         items++;
         }
      else if (strcmp(argv[i],"\\RO") == 0)
         {
	 d_reorder = i - 2;
         items++;
         }
      else if (strcmp(argv[i],"\\D") == 0)
         {
	 d_desc = 1;
         }
      else
	 {
         printf("Unrecognised switch : %s\n",argv[i]);
	 exit(0);
         }
      }
      /*
       * Determine display positions
       */
      j = 0;
      for (i=items; i>0; i--)
         {
	 if (d_stock == i)
	    {
	    p_stock = positions[j];
            j++;
	    }
	 else if (d_retail == i)
	    {
	    p_retail = positions[j];
            j++;
	    }
	 else if (d_trade == i)
	    {
	    p_trade = positions[j];
            j++;
	    }
	 else if (d_reorder == i)
	    {
	    p_reorder = positions[j];
            j++;
            }
	 }
   max_description_length = 70 - (6 * items);
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
            /* May be dangerous as don't always know how description will be
             * terminated ! */
	    get_description(string_in,
			    description,
	                    max_description_length,
			    position);
            position = 0;
	    get_part_number(string_in,
			    part_number,
                            true,
			    position);
	    str_build(string_out,
                      part_number);
      
	    if ((part_data = find_record(part_number,
			                 base_ptr)) != NULL)
	       {
               if (d_desc == 0)
	          str_build(&string_out[8],
                            description);
               else
	          str_build(&string_out[8],
                            part_data->description);

	       if (d_stock > 0)
		  {
		  display_int_data(string_out,
				   part_data->stock,
                                   p_stock);
		  }
	       if (d_retail > 0)
		  {
		  display_float_data(string_out,
				     part_data->retail_cost,
                                     p_retail);
                  }
	       if (d_trade > 0)
		  {
		  display_float_data(string_out,
				     part_data->trade_cost,
                                     p_trade);
                  }
	       if (d_reorder > 0)
		  {
		  display_int_data(string_out,
				   part_data->order_level,
                                   p_reorder);
                  }
	       }
	    else
	       {
               /* Part number not found in database */
	       str_build(&string_out[8],
                         description);
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
	 string_out[79] = '\n';
	 string_out[80] = '\0';
	 printf("%s",string_out);
         }
      }
   }  /* main() */
