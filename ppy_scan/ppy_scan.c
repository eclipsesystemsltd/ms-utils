                               /* ppy_scan.c */
/*
PPY_SCAN.C

* As PP_SCAN.C but modified to handle parts lists with rows of dots or some of the earlier formatting.

This program converts a file produced by a scan of a parts list into the
normal parts list format. It correctly positions reference number, part number,
description and quantity. Note that some editing of the scan file may be
necessary, e.g. to insert *title directives. Lines beginning with a '*' are
not processed.

The output is printed to the screen.

E.g. C:> PP_SCAN T140_78.SCN  where T140_78.SCN is a text file generated by
the scanning process.
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
   FILE *fptr_scan;
   char string_in[201];
   char string_out[101];
   char quantity[10];
   char alt_part_manufacturer[20];
   char alt_part_number[20];
   char alternative_part_number[40];
   char dummy_string[200];
   enum boolean first_line = true;
   enum boolean blank_line = false;
   enum boolean comment_line = false;
   enum boolean alternative_part = false;
   enum boolean quantity_found;
   int i,j,k,l;
//   int desc_ref_point;
   int no_of_spaces;
   int position;
   int alt_part_number_position;
   int part_no_length;		// - PPY
   int desc_start;			// - PPY

   if ((fptr_scan = fopen(argv[1],"r")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }
   while(fgets(string_in, 200, fptr_scan) != NULL)
      {
      for (i=0; i<100; i++)
         string_out[i] = ' ';
      string_out[0] = '\0';
      i=0;
      blank_line = false;
      comment_line = false;
      alternative_part = false;
      quantity_found = false;
      position = 0;
      alt_part_number_position = 0;
      if (string_in[0] == '*')
         {
         printf("%s", string_in);
         comment_line = true;
         if (strncmp("*title", string_in, 6) == 0)
            first_line = true;
         }
      else
         {
         /* Remove leading spaces */
         while (string_in[i] == ' ' ||
                string_in[i] == '\t')
            i++;
         if (string_in[i] == '\0' ||
             string_in[i] == '\n')
            blank_line = true;
         }
      if (blank_line == false &&
          comment_line == false)
         {
         /* Remove odd characters - PPY */
         j=0;
         while (string_out[j] != '\0' &&
                string_out[j] != '\n')
            {
            if (string_out[j] == '\'' ||
                string_out[j] == '`' ||
                string_out[j] == '~')
               {
               strcpy(&string_out[j], &string_out[j+1]);
               }
            else
               j++;
            }

         /* Look for alternative manufacturer part number */
	 k=1;
	 while (string_in[k] != '\n'&&
		string_in[k] != '\0')
	    {
	    if (string_in[k] == '*')
	       if (alt_part_number_position == 0)
                  {
		  position = k;
                  if (check_alt_part_number(string_in,
                                            alt_part_manufacturer,
                                            alt_part_number,
                                            dummy_string,
			     	            &position) == 1)
                     {
                     alternative_part = true;
                     build_alt_part_number(alternative_part_number,
                                           alt_part_manufacturer,
                                           alt_part_number);
                     for (l=0; l <= strlen(alternative_part_number); l++)
                        string_in[k + l] = ' ';   
                     }
                  }
	       else
		  {
		  printf("ERROR: ALT PART NUMBER\n");
		  break;
		  }
	    k++;
	    }
         /* Get reference number (1st field) */
         j=0;
         no_of_spaces = 0;
         while (no_of_spaces < 1 &&		// - PPY mod
                string_in[i] != '\0' &&
                string_in[i] != '\n')
            {
            if (string_in[i] != ' ' && string_in[i] != '\t')
               {
			   /* If reference number has an 'I' in it assume it should be a 1 */
 			   if (string_in[i] == 'I' || string_in[i] == 'i' || string_in[i] == 'l')
				  {
				  string_in[i] = '1';
				  }
               string_out[j] = string_in[i];
               no_of_spaces = 0;
               j++;
               }
            else
               no_of_spaces++;
            i++;
            }
		if (string_out[0] == 'N' && string_out[1] == '1')
			{
			string_out[1] = 'I';	/* Make correction for this case */
			}
        if (string_in[i] != '\0' &&
             string_in[i] != '\n')
            {
            /* Remove spaces */
            while (string_in[i] == ' ' || string_in[i] == '\t' &&
                   string_in[i] != '\n' &&
                   string_in[i] != '\0')
               i++;
            /* Get part number (2nd field) */
            j=3;
            no_of_spaces = 0;
            part_no_length = 0;				// - PPY
           while (no_of_spaces < 1 &&		// - PPY mod
                   string_in[i] != '\0' &&
                   string_in[i] != '\n')
               {
			   /* If part number has an 'I' in it assume it should be a 1 */
 			   if (string_in[i] == 'I' || string_in[i] == 'i' || string_in[i] == 'l')
				  {
				  string_in[i] = '1';
				  }
               if (string_in[i] != ' ' && string_in[i] != '\t')
                  {
                  string_out[j] = string_in[i];
                  no_of_spaces = 0;
                  part_no_length++;
                  j++;
                  }
               else
                  no_of_spaces++;
               i++;
               }
            if (string_in[i] != '\0' &&
                string_in[i] != '\n' &&
                j < 11)
               {
               /* Remove spaces */
               while (string_in[i] == ' ' &&
                      string_in[i] != '\n' &&
                      string_in[i] != '\0')
                  i++;
               if (string_in[i] != '\n' &&
                   string_in[i] != '\0')
                  {
                  /* Get description (remaining fields), ignoring indentation - PPY */
                  if (part_no_length <= 7)
                     {
                     desc_start = 11;
                     strcpy(&string_out[desc_start], &string_in[i]);
                     }
                  else
                     {
                     desc_start = 3 + part_no_length + 1;
                     strcpy(&string_out[desc_start], &string_in[i]);
                     }

                  /* Remove strings of dots (or other characters) -- PPY */
                  i = desc_start;	/* i now refers to output rather than input string */
                  while (string_out[i] != '\0' &&
                         string_out[i] != '\n')
                     {
                     if (string_out[i] == '.')
                        {
                        strcpy(&string_out[i], &string_out[i+1]);
                        }
                     else
                        i++;
                     }
                  /* Replace tabs with spaces -- PPY */
                  i = desc_start;	/* i now refers to output rather than input string */
                  while (string_out[i] != '\0' &&
                         string_out[i] != '\n')
                     {
                     if (string_out[i] == '\t')
                        {
                        string_out[i] = ' ';
                        }
                     else
                        i++;
                     }
                  /* Remove multiple spaces from description - PPY */
                  i = desc_start;
                  while (string_out[i] == ' ')
                     i++;
                  i++;
                  while (string_out[i] != '\0' &&
                         string_out[i] != '\n')
                     {
                     if (string_out[i] == ' ' &&
                         string_out[i - 1] == ' ')
                        {
                        j = i;
                        while (string_out[j] == ' ')
                           j++;
                        strcpy(&string_out[i], &string_out[j]);
                        }
                     i++;
                     }
                  /* Remove trailing blanks */
                  i--;
                  while (isspace(string_out[i]) != 0)
                     {
                     i--;
                     }
                  string_out[i + 1] = '\n';
                  string_out[i + 2] = '\0';
				  /* If last characters have 'I' in them assume they should be 1s */
				  if (string_out[i] == 'I')
				  {
				     string_out[i] = '1';
				  }
				  if (string_out[i - 1] == 'I')	/* Check doesn't cause problems */
				  {
				     string_out[i - 1] = '1';
				  }
                  /* Put $ in quantity field (last field) */
                  if (isdigit(string_out[i]) &&
                      isdigit(string_out[i - 1]) &&
                      string_out[i - 2] == ' ')
                     {
                     quantity[0] = '$';
                     strcpy(&quantity[1], &string_out[i - 1]);
                     quantity_found = true;
                     string_out[i - 1] = '\0';
                     }
                  else if (isdigit(string_out[i]) &&
                     string_out[i - 1] == ' ')
                     {
                     quantity[0] = '$';
                     strcpy(&quantity[1], &string_out[i]);
                     quantity_found = true;
                     string_out[i] = '\0';
                     }
                  else	/* No quantity value found - PPY */
                     {
                     strcpy(quantity, "$?\n");
                     quantity_found = true;	/* But not really */
                     string_out[i] = '\0';
                     }
                  if (alternative_part == true)
                     {
                     if (quantity_found != true)
                        {
                        string_out[strlen(string_out) - 1] = ' ';
                        strcat(string_out, alternative_part_number);
                        strcat(string_out, "\n");
                        }
                     else
                        strcat(string_out, alternative_part_number);
                     }
                  if (quantity_found == true)
                     {
                     if (alternative_part == true)
                        strcat(string_out, " ");
                     strcat(string_out, quantity);
                     }
                  }
               }
            else
               {
               string_out[j] = '\n';
               string_out[j + 1] = '\0';
               }
            }
         else
            {
            string_out[j] = '\n';
            string_out[j + 1] = '\0';
            }
         printf("%s", string_out);
         }
      }
   }  /* main() */