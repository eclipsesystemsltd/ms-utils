/*                              ck_utils.c                                */
/* This file contains all the general purpose functions required by other */
/* programs for parts list analysis and generation.                       */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_data.h"
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

enum directive check_directive(char string_in[],
			       char string_out[])
   {
   char local_string[81];
   int i,j,k;
   enum directive directive = none;

   if (string_in[0] != '*')
      return(error);
   else
      {
      for (i=0; i<81; i++)
         local_string[i] = ' ';
      i = 1;
      while (string_in[i] != ' ' &&
             string_in[i] != '\n' &&
             string_in[i] != '\0' &&
	     string_in[i] != '\t' &&
             i < 25)
	 {
         local_string[i - 1] = string_in[i];
         i++;
         }
      local_string[i - 1] = '\0';
      for (j=0; j < no_of_directive_strings; j++)
         {
         if (strcmp(local_string, directive_list[j].string) == 0)
            {
            directive = directive_list[j].direct;
            if (directive == variant_start ||
                directive == variant ||
                directive == variant_assembly ||
                directive == in_assembly ||
                directive == year ||
		directive == publication_part_no ||
		directive == manufacturer ||
		directive == engine_number ||
                directive == page)
               {
               i++;
               k = i;
               while (string_in[i] != ' ' &&
                      string_in[i] != '\n' &&
                      string_in[i] != '\0' &&
		      string_in[i] != '\t' &&
		      i < 40)
                  {
                  string_out[i - k] = string_in[i];
                  i++;
                  }
               string_out[i - k] = '\0';
               }
            else if (directive == title ||
                     directive == variant_quantity ||
                     directive == major_model ||
                     directive == minor_model)
               {
               i++;
               k = i;
               while (string_in[i] != '\n' &&
                      string_in[i] != '\0' &&
		      i < 50)
                  {
                  string_out[i - k] = string_in[i];
                  i++;
                  }
               string_out[i - k] = '\0';
               }
            return(directive);
            }
	 }
      return(error);
      }
   }  /* check_directive() */


int check_ref_number(char string_in[],
		     char string_out[],
		     int *place)
   {
   if (isdigit(string_in[*place]) != 0 &&
       isdigit(string_in[*place + 1]) != 0 &&
       string_in[*place + 2] == ' ')
      {
      string_out[ref_number_position] = string_in[*place];
      string_out[ref_number_position + 1] = string_in[*place + 1];
      }
   else if (isdigit(string_in[*place]) != 0 &&
	    string_in[*place + 1] == ' ' &&
	    string_in[*place + 2] == ' ')
      {
      string_out[ref_number_position + 1] = string_in[*place];
      }
   else if (string_in[0] == 'N' &&  /* Not illustrated */
	    string_in[1] == 'I' &&
	    string_in[2] == ' ')
      {
      string_out[ref_number_position] = 'N';
      string_out[ref_number_position + 1] = 'I';
      }
   else if (string_in[0] == 'A' &&  /* As required */
	    string_in[1] == 'R' &&
	    string_in[2] == ' ')
      {
      string_out[ref_number_position] = 'A';
      string_out[ref_number_position + 1] = 'R';
      }
   else if (string_in[0] == 'R' &&  /* Reference only */
	    string_in[1] == 'O' &&
	    string_in[2] == ' ')
      {
      string_out[ref_number_position] = 'R';
      string_out[ref_number_position + 1] = 'O';
      }
   else if (string_in[*place] == '-' &&
	    string_in[*place + 1] == '-' &&
	    string_in[*place + 2] == ' ')
      {
      string_out[ref_number_position] = ' ';
      string_out[ref_number_position + 1] = ' ';
      }
   else
      return(0);
   *place = *place + 3;
   return(1);
   }  /* check_ref_number() */

int check_part_number(char string_in[],
                      char string_out[],
                      enum boolean six_digit_part_no,
		      int *place)
   {
   int i;
   int position;
   char long_part_number[20];  /* Dummy buffer */

   position = *place;

   if (isdigit(string_in[position]) != 0 &&
       isdigit(string_in[position + 1]) != 0 &&
       string_in[position + 2] == '-' &&
       isdigit(string_in[position + 3]) != 0 &&
       isdigit(string_in[position + 4]) != 0 &&
       isdigit(string_in[position + 5]) != 0 &&
       isdigit(string_in[position + 6]) != 0 &&
       (string_in[position + 7] == ' ' ||
        string_in[position + 7] == 9))
      {
      if (six_digit_part_no == true)
         for (i=0; i<8; i++)
            string_out[part_number_position + i] = string_in[position + i];
      else
         to_alpha_part_no(string_in,
                          string_out,
			  position);
      }
   else if (string_in[position] == '-' &&
	    string_in[position + 1] == '-' &&
	    string_in[position + 2] == '-' &&
	    string_in[position + 3] == '-' &&
	    string_in[position + 4] == '-' &&
	    string_in[position + 5] == '-' &&
            string_in[position + 6] == '-' &&
            (string_in[position + 7] == ' ' ||
             string_in[position + 7] == 9))
      {
      if (six_digit_part_no == true)
         for (i=0; i<7; i++)
	    string_out[part_number_position + i] = ' ';
      else
         for (i=0; i<7; i++)
	    string_out[part_number_position + i] = '-';
      }
   else if (string_in[position] == ' ' &&
	    string_in[position + 1] == ' ' &&
	    string_in[position + 2] == ' ' &&
	    string_in[position + 3] == ' ' &&
	    string_in[position + 4] == ' ' &&
	    string_in[position + 5] == ' ' &&
	    string_in[position + 6] == ' ' &&
            (string_in[position + 7] == ' ' ||
             string_in[position + 7] == 9))
      {
      return(2);    /* for blank part number definition */
      }
   else if (string_in[position + 6] == '+')
      /* Check for a 'long' part number, i.e. over 7 characters */
      {
      if (check_long_part_number(string_in,
                                 long_part_number,
                                 string_out,
			         &position) != 1)
         return(0);
      }
   else
      /* Need to check against list of 'odd' numbers */
      {
      if (check_odd_part_number(string_in,
                                string_out,
                                six_digit_part_no,
				*place) != 1)
         return(0);
      }
   *place = *place + 8;
   return(1);
   }  /* check_part_number() */

int check_alpha_part_number(char string_in[],
                            char string_out[],
                            enum boolean six_digit_part_no,
		            int *place)
   {
   int i;
   int position;
   char long_part_number[20];   /* Dummy buffer */
   enum boolean valid_number = false;
   enum boolean is_6_digits = false;

   position = *place;
   /* First look to see if part number is of form 14-xxxx or 99-xxxx as
    * these remain unchanged.
    */
   if ((((string_in[position] == '1' &&             /* e.g. 14-2345 or 99-0876 */
	  string_in[position + 1] == '4') ||
	 (string_in[position] == '9' &&
	  string_in[position + 1] == '9')) &&
	 string_in[position + 2] == '-') &&
        isdigit(string_in[position + 3]) != 0 &&
        isdigit(string_in[position + 4]) != 0 &&
        isdigit(string_in[position + 5]) != 0 &&
        isdigit(string_in[position + 6]) != 0 &&
        (string_in[position + 7] == ' ' ||
	 string_in[position + 7] == 9))
      {
      valid_number = true;
      is_6_digits = true;
      }
   else if (string_in[position] == 'D' ||         /* e.g. D345 or S5689 */
	    string_in[position] == 'H' ||
	    string_in[position] == 'S' ||
	    string_in[position] == 'T' ||
	    string_in[position] == 'W')
      {
      i = 0;
      while (isdigit(string_in[position + i + 1]) != 0)
	 {
	 i++;
         }
      if ((i == 3 ||
	  i == 4) &&
	  string_in[position + i + 1] == ' ')
	 {
	 valid_number = true;
	 }
      }
   else if (string_in[position] == 'E' ||      
	    string_in[position] == 'F')
      {
      i = 0;
      while (isdigit(string_in[position + i + 1]) != 0)
	 {
	 i++;
         }
      if ((i == 3 ||                             /* e.g. E3903 */
	  i == 4) &&
	  string_in[position + i + 1] == ' ')
	 {
	 valid_number = true;
	 }
      else if (i == 5 &&                         /* e.g. E13352 */
               string_in[position + 1] == '1' &&
	       string_in[position + i + 1] == ' ')
	 {
	 valid_number = true;
         }
      }
   if (valid_number == true)
      {
      if (six_digit_part_no == false ||
	  is_6_digits == true)
	 {
         i = 0;
	 while (string_in[position + i] != ' ')
            {
	    string_out[part_number_position + i] = string_in[position + i];
            i++;
	    }
         }
      else
         to_int_part_no(string_in,
                        string_out,
		        position);
      }

   if (valid_number == false &&
       string_in[position] == '-' &&
       string_in[position + 1] == '-' &&
       string_in[position + 2] == '-' &&
       string_in[position + 3] == '-' &&
       string_in[position + 4] == '-' &&
       string_in[position + 5] == '-' &&
       string_in[position + 6] == '-' &&
       (string_in[position + 7] == ' ' ||
       string_in[position + 7] == 9))
      {
      if (six_digit_part_no == false)
         for (i=0; i<7; i++)
	    string_out[part_number_position + i] = ' ';
      else
         for (i=0; i<7; i++)
	    string_out[part_number_position + i] = '-';
      }
   else if (valid_number == false &&
            string_in[position] == ' ' &&
	    string_in[position + 1] == ' ' &&
	    string_in[position + 2] == ' ' &&
	    string_in[position + 3] == ' ' &&
	    string_in[position + 4] == ' ' &&
	    string_in[position + 5] == ' ' &&
	    string_in[position + 6] == ' ' &&
            (string_in[position + 7] == ' ' ||
             string_in[position + 7] == 9))
      {
      return(2);    /* for blank part number definition */
      }
   else if (valid_number == false &&
            string_in[position + 6] == '+')
      /* Check for a 'long' part number, i.e. over 7 characters */
      {
      if (check_long_part_number(string_in,
                                 long_part_number,
                                 string_out,
			         &position) != 1)
         return(0);
      }
   else if (valid_number == false)
      /* Need to check against list of 'odd' numbers */
      {
      if (check_odd_part_number(string_in,
                                string_out,
                                six_digit_part_no,
				position) != 1)
         return(0);
      }

   *place = *place + 8;
   return(1);
   }  /* check_alpha_part_number() */

int check_odd_part_number(char string_in[],
                          char string_out[],
                          enum boolean six_digit_part_no,
			  int place)
   {
   FILE *fptr_odd_parts;
   char part_number[20];
   char odd_part[30];
   char alpha_part_number[20];
   char int_part_number[20];
   int i = 0;
   int j;
   int position;
   position = place;

   while (string_in[place] != ' ')
      {
      part_number[i] = string_in[place];
      i++;
      place++;
      }
   part_number[i] = '\0';
   if ((fptr_odd_parts = fopen("odd_part.lst","r")) == NULL)
      {
      printf("Cannot open file \"odd_part.lst\"\n");
      exit(0);
      }
   while (fscanf(fptr_odd_parts,"%s %s", alpha_part_number, int_part_number) != EOF)
      {
      if (strcmp(part_number, alpha_part_number) == 0)
	 {
	 if (six_digit_part_no == true &&
	     isdigit(int_part_number[0]) != 0)
	    {
            j=0;
	    while (int_part_number[j] != '\0')
	       {
	       string_out[part_number_position + j] = int_part_number[j];
	       j++;
	       }
	    }
	 else
            {
            j=0;
	    while (part_number[j] != '\0')
	       {
	       string_out[part_number_position + j] = part_number[j];
	       j++;
               }
	    }
         fclose(fptr_odd_parts);
         return(1);
	 }
      }
   fclose(fptr_odd_parts);
   return(0);
   }  /* check_odd_part_number() */

int check_odd_part_number_orig(char string_in[],
                          char string_out[],
                          enum boolean six_digit_part_no,
			  int place)
   {
   char part_number[20];
   int i = 0;
   int j;
   int position;
   position = place;

   while (string_in[place] != ' ')
      {
      part_number[i] = string_in[place];
      i++;
      place++;
      }
   part_number[i] = '\0';
   for (i = 0; i < number_of_odd_parts; i++)
      {
      if (strcmp(part_number,odd_parts[i][0]) == 0)
	 {
	 if (six_digit_part_no == true &&
	     isdigit(odd_parts[i][1][0]) != 0)
	    {
            j=0;
	    while (odd_parts[i][1][j] != '\0')
	       {
	       string_out[part_number_position + j] = odd_parts[i][1][j];
	       j++;
	       }
	    }
	 else
            {
            j=0;
	    while (part_number[j] != '\0')
	       {
	       string_out[part_number_position + j] = part_number[j];
	       j++;
               }
	    }
         return(1);
	 }
      }
   return(0);
   }  /* check_odd_part_number_orig() */

int get_ref_number(char string_in[],
		   char string_out[],
		   int place)
   {
   string_out[0] = '\0';
   if (isdigit(string_in[place]) != 0 &&
       isdigit(string_in[place + 1]) != 0 &&
       string_in[place + 2] == ' ')
      {
      string_out[0] = string_in[place];
      string_out[1] = string_in[place + 1];
      }
   else if (isdigit(string_in[place]) != 0 &&
	    string_in[place + 1] == ' ' &&
	    string_in[place + 2] == ' ')
      {
      string_out[0] = ' '; 
      string_out[1] = string_in[place];
      }
   else if (string_in[0] == 'N' &&  /* Not illustrated */
	    string_in[1] == 'I' &&
	    string_in[2] == ' ')
      {
      string_out[0] = 'N';
      string_out[1] = 'I';
      }
   else if (string_in[0] == 'A' &&  /* As required */
	    string_in[1] == 'R' &&
	    string_in[2] == ' ')
      {
      string_out[0] = 'A';
      string_out[1] = 'R';
      }
   else if (string_in[0] == 'R' &&  /* Reference only */
	    string_in[1] == 'O' &&
	    string_in[2] == ' ')
      {
      string_out[0] = 'R';
      string_out[1] = 'O';
      }
   else if (string_in[place] == '-' &&
	    string_in[place + 1] == '-' &&
	    string_in[place + 2] == ' ')
      {
      string_out[0] = ' ';
      string_out[1] = ' ';
      }
   else
      return(0);
   string_out[2] = '\0';
   return(1);
   }  /* get_ref_number() */

int get_part_number(char string_in[],
                    char string_out[],
		    enum boolean six_digit_part_no,
		    int position)
   {
   int i;

   string_out[0] = '\0';
   if (isdigit(string_in[position]) != 0 &&
       isdigit(string_in[position + 1]) != 0 &&
       string_in[position + 2] == '-' &&
       isdigit(string_in[position + 3]) != 0 &&
       isdigit(string_in[position + 4]) != 0 &&
       isdigit(string_in[position + 5]) != 0 &&
       isdigit(string_in[position + 6]) != 0 &&
       (string_in[position + 7] == ' ' ||
        string_in[position + 7] == 9))
      {
      if (six_digit_part_no == true)
         for (i=0; i<7; i++)
	    string_out[i] = string_in[position + i];
      string_out[7] = '\0';
      }
   else if (string_in[position] == '-' &&
	    string_in[position + 1] == '-' &&
	    string_in[position + 2] == '-' &&
	    string_in[position + 3] == '-' &&
	    string_in[position + 4] == '-' &&
	    string_in[position + 5] == '-' &&
            string_in[position + 6] == '-' &&
            (string_in[position + 7] == ' ' ||
             string_in[position + 7] == 9))
      {
      string_out[0] = '\0';
      return(2);
      }
   else if (string_in[position] == ' ' &&
	    string_in[position + 1] == ' ' &&
	    string_in[position + 2] == ' ' &&
	    string_in[position + 3] == ' ' &&
	    string_in[position + 4] == ' ' &&
	    string_in[position + 5] == ' ' &&
	    string_in[position + 6] == ' ' &&
            (string_in[position + 7] == ' ' ||
             string_in[position + 7] == 9))
      {
      string_out[0] = '\0';
      return(2);    /* for blank part number definition */
      }
   else
      /* Need to check against list of 'odd' numbers */
      {
      if (get_odd_part_number(string_in,
                              string_out,
                              six_digit_part_no,
			      position) != 1)
         {
         string_out[0] = '\0';
	 return(0);
         }
      }
   return(1);
   }  /* get_part_number() */

int get_quantity(char string_in[],
		 char string_out[],
		 int position)
   {
   char number[5];
   int quantity;

   string_out[0] = '\0';
   if (string_in[position] == '$')
      {
      position++;
      if (isdigit(string_in[position]) != 0 &&
	 isdigit(string_in[position + 1]) != 0)
	 {
	 string_out[0] = string_in[position];
         string_out[1] = string_in[position + 1];
	 string_out[2] = '\0';
         number[0] = string_in[position];
         number[1] = string_in[position + 1];
         number[2] = '\0';
	 }
      else if (isdigit(string_in[position]) != 0 &&
	       (string_in[position + 1] == ' ' ||
		string_in[position + 1] == '\n' ||
		string_in[position + 1] == '\t' ||
		string_in[position + 1] == '\0'))
	 {
	 string_out[0] = string_in[position];
	 string_out[1] = '\0';
         number[0] = string_in[position];
         number[1] = '\0';
	 }
      else if ((string_in[position] == '>' ||
		string_in[position] == '<') &&
		string_in[position + 1] == '=' &&
		isdigit(string_in[position + 2]) != 0)
	 {
	 string_out[0] = string_in[position];
	 string_out[1] = '=';
	 string_out[2] = string_in[position + 2];
         string_out[3] = '\0';
	 return(0);
	 }
      else if (string_in[position] == 'R' &&	  /* Reference only */
	       string_in[position + 1] == 'O')
	 {
	 string_out[0] = 'R';
	 string_out[1] = 'O';
         string_out[2] = '\0';
	 return(0);
	 }
      else if (string_in[position] == 'A' &&	  /* As required */
	       string_in[position + 1] == 'R')
	 {
	 string_out[0] = 'A';
	 string_out[1] = 'R';
         string_out[2] = '\0';
	 return(0);
	 }
      else
         return(-1);
      }
   else
      return(-1);
   quantity = atoi(number);
   return(quantity);
   }  /* get_quantity()  */


int get_odd_part_number(char string_in[],
                        char string_out[],
                        enum boolean six_digit_part_no,
			int place)
   {
   char part_number[20];
   int i = 0;
   int j;

   while (string_in[place] != ' ')
      {
      part_number[i] = string_in[place];
      i++;
      place++;
      }
   part_number[i] = '\0';
   for (i = 0; i < number_of_odd_parts; i++)
      {
      if (strcmp(part_number,odd_parts[i][0]) == 0)
	 {
	 if (six_digit_part_no == true &&
	     isdigit(odd_parts[i][1][0]) != 0)
	    {
            j=0;
	    while (odd_parts[i][1][j] != '\0')
	       {
	       string_out[j] = odd_parts[i][1][j];
	       j++;
	       }
	    }
	 else
            {
            j=0;
	    while (part_number[j] != '\0')
	       {
	       string_out[j] = part_number[j];
	       j++;
               }
	    }
         string_out[j] = '\0';
         return(1);
	 }
      }
   return(0);
   }  /* get_odd_part_number() */

int to_alpha_part_no(char string_in[],
                     char string_out[],
		     int place)
   {
   char part_number[10];
   int i;

   /* Store part number locally */
   i=0;
   while (string_in[place + i] != ' ')
      {
      part_number[i] = string_in[place + i];
      i++;
      }
   part_number[i] = '\0';
   /* First look to see if part number is of form 14-xxxx or 99-xxxx as
    * these remain unchanged.
    */
   if ((string_in[place] == '1' &&
	string_in[place + 1] == '4') ||
       (string_in[place] == '9' &&
	string_in[place + 1] == '9') &&
       string_in[place + 2] == '-')
      ;
   else
      {
      /* Look to see if part is in 'odd' part number list */
      for (i=0; i<number_of_odd_parts; i++)
	 {
	 if (strcmp(part_number,odd_parts[i][1]) == 0)
	    {
	    strcpy(part_number,odd_parts[i][0]);
	    break;
            }
	 }
      /* If a normal part number then convert to alpha-numeric part number */
      if (i == number_of_odd_parts)
         {
         for (i=0; i < part_number_prefixes; i++)
            {
            if (string_in[place] == alpha_to_num[i][1][0] &&
	        string_in[place + 1] == alpha_to_num[i][1][1])
               {
	       strcpy(part_number, alpha_to_num[i][0]);
	       if (string_in[place + 3] != '0')
		  strncat(part_number, &string_in[place + 3], 4);
	       else
		  strncat(part_number, &string_in[place + 4], 3);
               break;
               }
	    }
	 }
      }
   i = 0;
   while (part_number[i] != '\0')
      {
      string_out[part_number_position + i] = part_number[i];
      i++;
      }
   return(1);
   }  /* to_alpha_part_no() */

int to_int_part_no(char string_in[],
                   char string_out[],
                   int place)
   {
   char part_number[10];
   int i,j;
   int digits;

   for (i=0; i < part_number_prefixes; i++)
      {
      if (string_in[place] == alpha_to_num[i][0][0])
	 {
         digits = 0;
         while (isdigit(string_in[place + digits + 1]) != 0)
	    {
	    digits++;
	    }
	 strcpy(part_number,alpha_to_num[i][1]);
	 part_number[2] = '-';      /* Will overwrite '\0' from strcpy */
         if (string_in[place] == 'E')
	    part_number[1] = '0';       /* Assume part number is 70-xxxx */
	 else if (string_in[place] == 'F')
	    part_number[1] = '2';       /* Assume part number is 82-xxxx */
	 if (digits == 5 &&
	     string_in[place + 1] == '1')
	    {
	    if (string_in[place] == 'E')
	       {
	       part_number[1] = '1';       /* Part number is 71-xxxx */
	       strncpy(&part_number[3],&string_in[place + 2],4);
	       }
	    else if (string_in[place] == 'F')
	       {
	       part_number[1] = '3';       /* Part number is 83-xxxx */
	       strncpy(&part_number[3],&string_in[place + 2],4);
               }
            }
	 else if (digits == 4)
	    strncpy(&part_number[3],&string_in[place + 1],4);
	 else
	    {
	    part_number[3] = '0';
	    strncpy(&part_number[4],&string_in[place + 1],3);
	    }
         for (j=0; j<7; j++)
            {
            string_out[part_number_position + j] = part_number[j];
	    }
	 return 1;
	 }
      }
   return 0;
   }  /* to_int_part_no() */

int check_description(char string_in[],
		      char string_out[],
		      int max_description_length,
		      int *place)
   {
   int i = 0;
   int description_start;

   description_start = *place;
   i = 0;
   while (string_in[description_start + i] != '*' &&
	  string_in[description_start + i] != '$' &&
	  string_in[description_start + i] != '#' &&
	  string_in[description_start + i] != '~' &&
	  string_in[description_start + i] != '^' &&
	  string_in[description_start + i] != '`' &&
	  string_in[description_start + i] != '\\' &&
	  string_in[description_start + i] != '\n' &&
	  i <= max_description_length)
      {
      if (i < max_description_length)
	 {
	 string_out[description_position + i] =
				  string_in[description_start + i];
	 }
      else
	 {
	 string_out[description_position + max_description_length - 2] = '-';
	 string_out[description_position + max_description_length - 1] = '>';
	 break;
	 }
      i++;
      }
   *place = description_start + i;
   return(1);
   } /* check_description() */

int get_description(char string_in[],
		    char string_out[],
		    int max_description_length,
		    int place)
   {
   int i = 0;

   while (string_in[place + i] != '*' &&
	  string_in[place + i] != '$' &&
	  string_in[place + i] != '#' &&
	  string_in[place + i] != '~' &&
	  string_in[place + i] != '^' &&
	  string_in[place + i] != '\\' &&
	  string_in[place + i] != '\n' &&
	  i < (max_description_length - 1))
      {
      string_out[i] = string_in[place + i];
      i++;
      }
   i--;
   while (isspace(string_out[i]) != 0)
      {
      i--;
      }
   string_out[i + 1] = '\0';
   return(1);
   } /* get_description() */

int check_alt_part_number(char string_in[],
                          char alt_part_manufacturer[],
                          char alt_part_number[],
                          char string_out[],
			  int *place)
   {
   int position;
   int i,j,k;
   char local_string[20];

   position = *place;
   if (string_in[position] == '*')
      {
      i=1;
      while (string_in[position + i] != '_' &&
	     i < 15)
	 {
         alt_part_manufacturer[i - 1] = string_in[position + i];
	 i++;
         }
      alt_part_manufacturer[i - 1] = '\0';
      for (j=0; j < no_of_alt_part_manufacturers; j++)
	 {
         if (strcmp(alt_part_manufacturer, &alt_manufacturer_list[j][0]) == 0)
            {
	    k=1;
	    while (string_in[position + i + k] != ' ' &&
		   k <= 15)
	       {
               alt_part_number[k - 1] = string_in[position + i + k];
               string_out[alt_part_number_posn + k - 1]
                                      = string_in[position + i + k];
	       k++;
	       }
	    alt_part_number[k - 1] = '\0';
	    if (k == 15)
	       return(0);
	    else
	       {
	       position = position + i + k + 1;
	       *place = position;
	       return(1);
	       }
	    }
         }
      }
   return(0);
   }  /* check_alt_part_number() */

int check_assembly(char string_in[],
		   int *assembly_level,
		   int *place,
		   struct assembly_defn assembly[])
   {
   char number_lines[5];
   char number_components[5];
   int i,j;
   int position;
   enum boolean short_defn = false;

   position = *place;
   if (string_in[position] == '#')
      {
      i=1;
      while (string_in[position + i] != '@' &&
             string_in[position + i] != '+' &&
             string_in[position + i] != '-' &&
	     i < 4)
	 {
         if (isdigit(string_in[position + i]) == 0)
            return(0);
         else
            {
            number_lines[i - 1] = string_in[position + i];
            i++;
            }
         }
      number_lines[i - 1] = '\0';
      if (string_in[position + i] == '@')
	 {
         i++;
         j = i;
	 while (string_in[position + i] != '+' &&
                string_in[position + i] != '-' &&
                (i - j) < 4)
	    {
            if (isdigit(string_in[position + i]) == 0)
               return(0);
            else
               {
               number_components[i - j] = string_in[position + i];
               i++;
               }
            }
         number_components[i - j] = '\0';
	 }
      else if (string_in[position + i] == '-' ||
               string_in[position + i] == '+')
	 {
	 short_defn = true;
	 }
      else
	 return(0);
      }
   else
      return(0);

   if (string_in[position + i + 1] == ' ')
      {
      *assembly_level = *assembly_level + 1;
      assembly[*assembly_level].lines_to_go = atoi(number_lines);
      if (short_defn == false)
	 assembly[*assembly_level].components_to_go = atoi(number_components);
      else
	 assembly[*assembly_level].components_to_go =
					assembly[*assembly_level].lines_to_go;
      if (string_in[position + i] == '+')
         {
         assembly[*assembly_level].lines_to_go--;
         /* assume only one of item at this level per assembly */
         assembly[*assembly_level].components_to_go--;
         }
      position = position + i + 2;
      *place = position;
      return(1);
      }
   else
      return(0);

   }  /* check_assembly()  */

int check_quantity(char string_in[],
		   char string_out[],
		   int *place)
   {
   char number[5];
   int position;
   int quantity;

   position = *place;
   if (string_in[position] == '$')
      {
      position++;
      if (isdigit(string_in[position]) != 0 &&
	 isdigit(string_in[position + 1]) != 0)
	 {
	 string_out[quantity_position] = string_in[position];
         string_out[quantity_position + 1] = string_in[position + 1];
         number[0] = string_in[position];
         number[1] = string_in[position + 1];
         number[2] = '\0';
	 }
      else if (isdigit(string_in[position]) != 0 &&
	       (string_in[position + 1] == ' ' ||
		string_in[position + 1] == '\n' ||
		string_in[position + 1] == '\t' ||
		string_in[position + 1] == '\0'))
	 {
	 string_out[quantity_position] = ' ';
	 string_out[quantity_position + 1] = string_in[position];
         number[0] = string_in[position];
         number[1] = '\0';
	 }
      else if ((string_in[position] == '>' ||
		string_in[position] == '<') &&
		string_in[position + 1] == '=' &&
		isdigit(string_in[position + 2]) != 0)
	 {
	 string_out[quantity_position - 1] = string_in[position];
	 string_out[quantity_position] = '=';
	 string_out[quantity_position + 1] = string_in[position + 2];
	 return(0);
	 }
      else if (string_in[position] == 'R' &&	  /* Reference only */
	       string_in[position + 1] == 'O')
	 {
	 string_out[quantity_position] = 'R';
	 string_out[quantity_position + 1] = 'O';
	 return(0);
	 }
      else if (string_in[position] == 'A' &&	  /* As required */
	       string_in[position + 1] == 'R')
	 {
	 string_out[quantity_position] = 'A';
	 string_out[quantity_position + 1] = 'R';
	 return(0);
	 }
      else
         return(-1);
      }
   else
      return(-1);
   *place = *place + 3;
   quantity = atoi(number);
   return(quantity);
   }  /* check_quantity()  */

void fill_in_dots(char string_out[],
		  int start_position,
		  int stop_position)
   {
   int i;
   for (i = 0; i < stop_position - start_position; i++)
      string_out[start_position + i] = '.';
   }  /* fill_in_dots() */

int is_blank_line(char string_in[])
   {
   if (string_in[0] == ' '||
       string_in[0] == '\t' ||
       string_in[0] == '\0' ||
       string_in[0] == '\n')
      return (1);
   else
      return (0);
   } /* is_blank_line() */

int check_desc_indentation(char string_in[],
                           int description_start,
			   int assembly_level)
   {
   int i;
   for (i=0; i< (3 * assembly_level); i++)
      if (string_in[description_start + i] != ' ')
         return(0);
   if (string_in[description_start + i] == ' ')
         return(0);
   else
      return(1);
   } /* check_desc_indentation() */

int display_variant(char string_out[],
                    struct variant_ind variant_brackets[],
		    int variant_level)
   {
   int i,j;
   for (i = 0; i < variant_level; i++)
      {
 /*	string_out[variant_position + i * 6] =
  *		      variant_symbol[variant_brackets[i].symbol]; */
      string_out[variant_position + i * 3] =
                     variant_symbol[variant_brackets[i].symbol];
      if (variant_brackets[i].label_displayed == false)
         {
         j = 0;
         while (variant_brackets[i].label[j] != ' ' &&
                variant_brackets[i].label[j] != '\0' &&
                j < 4)
            {
   /*	      string_out[variant_position + (i * 6) + j + 1] =
    *						variant_brackets[i].label[j];*/
	    string_out[variant_position + (i * 3) + j + 1] =
                                               variant_brackets[i].label[j];
            j++;
            }
         variant_brackets[i].label_displayed = true;
         }
      }
   return(1);
   } /* display_variant() */

int short_strcpy(char string_1[],
                 char string_2[],
                 int number)
   {
   int i = 0;

   while (i < number &&
          string_2[i] != '\0')
      {
      string_1[i] = string_2[i];
      i++;
      }
   if (i == number)
      string_1[i - 1] = '\0';
   else
      string_1[i] = '\0';
   return(1);
   } /* short_strcpy() */

int display_variant_assy(char string_out[],
                         struct variant_assy_ind variant_assy[],
			 int variant_assy_level)
   {
   int j;

   string_out[variant_position] = variant_symbol[variant_assy_level];
   if (variant_assy[variant_assy_level].label_displayed == false)
      {
      j = 0;
      while (variant_assy[variant_assy_level].label[j] != ' ' &&
	     variant_assy[variant_assy_level].label[j] != '\0' &&
             j < 4)
         {
         string_out[variant_position + j + 1] =
			      variant_assy[variant_assy_level].label[j];
         j++;
         }
      variant_assy[variant_assy_level].label_displayed = true;
      }
   return(1);
   } /* display_variant_assy() */

str_build(char string_out[],
          char string_in[])
   {
   int i = 0;

   while (string_in[i] != '\0' &&
          i < 40)
      {
      string_out[i] = string_in[i];
      i++;
      }
   if (i < 40)
      return(1);
   else
      return(0);
   }  /* str_build() */

void str_build_ltd(char string_out[],
		   char string_in[],
		   int number)
   {
   int i = 0;

   while (string_in[i] != '\0' &&
	  i < number)
      {
      string_out[i] = string_in[i];
      i++;
      }
   return;
   }  /* str_build_ltd() */

int check_ref_only(char string_in[],
		   int *position)
   {
   if (string_in[*position] == '~')
      if (strncmp(&string_in[*position + 1],"ref_only",8) == 0 &&
	  string_in[*position + 9] == ' ')
	 {
	 *position = *position + 10;
	 return(1);
	 }
      else if (strncmp(&string_in[*position + 1],"reference_only",14) == 0 &&
	       string_in[*position + 15] == ' ')
	 {
	 *position = *position + 16;
	 return(1);
	 }
      else
	 return(0);
   else
      return(0);
   } /* check_ref_only() */

int check_alternative_part(char string_in[],
			   int *position)
   {
   char local_string[80];  /* dummy string for output of check_part_number() */
   enum boolean six_digit_part_no = true;
   int place;

   if (strncmp(&string_in[*position],"^alternative_",13) == 0 )
      {
      place = *position + 13;
      if (check_part_number(string_in,
			    local_string,
			    six_digit_part_no,
			    &place) == 1)
	 {
	 *position = place;
	 return(1);
	 }
      else
	 return(0);
      }
   else
      return(0);
   } /* check_alternative_part() */

int check_alternative_alpha_part(char string_in[],
                                 enum boolean six_digit_part_no,
			         int *position)
   {
   char local_string[80];  /* dummy string for output of check_part_number() */
   int place;

   if (strncmp(&string_in[*position],"^alternative_",13) == 0 )
      {
      place = *position + 13;
      if (check_alpha_part_number(string_in,
			          local_string,
			          six_digit_part_no,
			          &place) == 1)
	 {
	 *position = place;
	 return(1);
	 }
      else
	 return(0);
      }
   else
      return(0);
   } /* check_alternative_alph_part() */

int check_optional_part(char string_in[],
			int *position)
   {
   if (strncmp(&string_in[*position],"\\optional",9) == 0 &&
       string_in[*position + 9] == ' ')
      {
      *position = *position + 10;
      return(1);
      }
   else
      return(0);
   } /* check_optional_part() */

int check_long_part_number(char string_in[],
                           char long_part_number[],
                           char string_out[],
			   int *place)
   {
   int i;
   int long_number_posn = 0;

/* Should also check that first 6 characters in the normal field */
/* are the same as in the long field. */
/* Also check check that long part number is actually defined */

   if (isspace(string_in[*place]) == 0 &&
       isspace(string_in[*place + 1]) == 0 && 
       isspace(string_in[*place + 2]) == 0 && 
       isspace(string_in[*place + 3]) == 0 && 
       isspace(string_in[*place + 4]) == 0 && 
       isspace(string_in[*place + 5]) == 0 && 
       string_in[*place + 6] == '+' &&
       string_in[*place + 7] == ' ')
      {
      /* Find where long part number is located */
      i=1;
      while (string_in[i] != '\n'&&
	     string_in[i] != '\0')
         {
	 if (string_in[i] == '`')
	    long_number_posn = i;
	 i++;
	 }
      /* Process long part number */
      i=1;
      while (string_in[long_number_posn + i] != ' ' &&
	     i <= 15 &&
             long_number_posn > 0)
         {
         long_part_number[i - 1] = string_in[long_number_posn + i];
         string_out[long_part_number_posn + i - 1] = string_in[long_number_posn + i];
	 i++;
	 }
      long_part_number[i - 1] = '\0';
      if (i == 15)
	 return(0);
      else
	 {
	 *place = *place + 8;
         str_build(&string_out[part_number_position], "see RH ");
	 return(1);
	 }
      }   return(0);
   } /* check_long_part_number() */

int remove_leading_blanks(char string[],
			  int number)
   {
   int i = 0;

   i = number;
   while (string[i] == ' ' &&
	  string[i] != '\0')
      {
      i++;
      }
   if (string[i] != '\0' &&
       i != number)
      {
      strcpy(&string[number],&string[i]);
      return(1);
      }
   else
      return(0);
   } /* remove_leading_blanks() */

int is_assembly(char *string)
   {
   int i;
   i = 0;
   while (string[i] != '\n' &&
	  string[i] != '\0')
      {
      if (string[i] == '#')
	 {
	 return(i);
	 }
      i++;
      }
   return(0);
   }  /* is_assembly() */

str_build_1(char string_out[],
	    char string_in[])
   {
   int i = 0;

   while (string_in[i] != '\0' &&
	  string_in[i] != '\n' &&
	  i < 40)
      {
      string_out[i] = string_in[i];
      i++;
      }
    string_out[i] = '\0';
   if (i < 40)
      return(1);
   else
      return(0);
   }  /* str_build_1() */

int get_section(char section[max_lines][100],
		int file_no,
		int section_no,
		char current_section[50])
   {
   FILE *fptr_file;
   int j,k,l,m;
   int line;
   int number_of_sections;
   int end_of_line;
   enum boolean section_found;
   enum boolean in_section;
   enum boolean section_read;
   enum directive directive;
   char file_name[20];
   char string_in[101];
   char directive_string[100];

   /* initialise section */
   for (l = 0; l < max_lines; l++)
      section[l][0] = '\0';

   /* Open file */
   strcpy(file_name,"TEMP00.LST");
   make_temp_file_name(file_name,
		       4,
	   	       file_no);
   if ((fptr_file = fopen(file_name,"r")) == NULL)
      {
      printf("Cannot open file %s\n",file_name);
      exit(0);
      }

   /* find section to be copied */
   number_of_sections = 0;
   section_found = false;
   in_section = false;
   section_read = false;
   while (section_found == false)
      {
      if (fgets(string_in, 100, fptr_file) == NULL)
         break;
      if (is_blank_line(string_in) == 0)
	 {
	 if (string_in[0] == '*')
	    {
	    directive = check_directive(string_in, directive_string);
	    if (directive == title)
	       {
	       if (strcmp("next",current_section) == 0)
                  {
		  if (number_of_sections == section_no)
                     {
		     section_found = true;
		     strcpy(current_section,directive_string);
                     }
                  }
	       else
	          if (strcmp(directive_string,current_section) == 0)
		     section_found = true;
	       number_of_sections++;
	       }
	    }
	 }
      }
   /* Copy found section */
   if (section_found == true)
      {
      line = 0;
      section_read = false;
      in_section = true;
      while (fgets(string_in, 100, fptr_file) != NULL &&
             in_section == true)
         {
	 if (is_blank_line(string_in) == 0)
	    {
	    if (string_in[0] == '*')
	       {
	       directive = check_directive(string_in, directive_string);
	       /* Only these directives are allowed within a section */
	       if (directive == variant_start ||
		   directive == variant ||
		   directive == variant_end ||
		   directive == variant_quantity ||
		   directive == variant_quantity_end ||
		   directive == variant_assembly_start ||
		   directive == variant_assembly ||
		   directive == in_assembly ||
		   directive == common_assembly ||
		   directive == variant_assembly_end ||
		   directive == variant_one_of_set ||
		   directive == end_variant_set ||
                   directive == page)
		   {
		   strcpy(section[line],string_in);
		   line++;
		   }
	       else
		   {
		   section[line][0] = '%';  /* mark end of */
		   section[line][1] = '\0'; /* section	   */
		   section_read = true;
		   in_section = false;
		   }
	       }
	    else
	       {
	       k = 0;
               j = 0;
	       while (string_in[k] != '\0' &&   /* Eliminate any comments */
		      string_in[k] != '\n' &&
		      k < 100)
		  {
		  if (string_in[k] == '*' &&
		      string_in[k - 1] == '/')
		     {
		     end_of_line = 0;
                     j = 0;            /* Starting from beginning of line */
                     while (j < k-1) 
			{
			if (isspace(string_in[j]) == 0)
			   end_of_line = j;
                        j++;
                        }
		     string_in[end_of_line + 1] = '\0';
		     break;
		     }
		  k++;
		  }
	       remove_leading_blanks(string_in,    /* Remove indentation */
	       	                     11);          /* from description */
	       strcpy(section[line],string_in);
	       line++;
	       }
	    }
	 }
      }

   fclose(fptr_file);
   if (section_found == true &&
       section_read == true)
      return 1;
   else
      return 0;
   }  /* get_section() */

int make_temp_file_name(char file_name[],
                        int base,
			int i)
   {
   char number[5];

   itoa(i, number, 10);
   if (number[1] == '\0')
      {
      file_name[base] = '0';
      file_name[base + 1] = number[0];
      }
   else if (number[2] == '\0')
      {
      file_name[base] = number[0];
      file_name[base + 1] = number[1];
      }
   return(1);
   } /* make_temp_file_name() */

int create_temp_files(char defn_file[20][50],
		      int title_posn)
   {
   FILE *fptr_list;
   FILE *fptr_tmp;
   char temp_file[12];
   char number[5];
   char string_in[100];
   int i,j,k;
   int end_of_line;

   /* Limited to 100 files, ie TEMP00 to TEMP99 */
   strcpy(temp_file,"temp00.lst");
   for (i = 1; i < title_posn; i++)	 /* Go through each file looking for */
      {				         /* current section */
      if ((fptr_list = fopen(defn_file[i],"r")) == NULL)
         {
	 printf("Cannot open file %s\n",defn_file[i]);
	 exit(0);
	 }
      else
         {
	 /* printf("Opened file %s\n",defn_file[i]); */
	 make_temp_file_name(temp_file,
                             4,
			     i);
	 if ((fptr_tmp = fopen(temp_file,"w")) == NULL)
            {
	    printf("Cannot open file %s\n",temp_file);
	    exit(0);
            }
	 /* printf("Opened file %s\n",temp_file); */
	 while (fgets(string_in, 100, fptr_list) != NULL)
	    {
            if (is_blank_line(string_in) == 0)
               {
	       k = 0;
               j = 0;
	       while (string_in[k] != '\0' &&   /* Eliminate any comments */
		      string_in[k] != '\n' &&
		      k < 100)
	          {
	          if (string_in[k] == '*' &&
		      string_in[k - 1] == '/')
	             {
		     end_of_line = 0;
                     j = 0;
		     while (j < k-1)   /* Improve by working from end of line */
		        {
		        if (isspace(string_in[j]) == 0)
			   end_of_line = j;
                        j++;
                        }
       		     string_in[end_of_line + 1] = '\n'; 
		     string_in[end_of_line + 2] = '\0';
		     break;
		     }
	          k++;
	          }
	       remove_leading_blanks(string_in,
	        		     11);
               fputs(string_in, fptr_tmp);
               }
            }
	 fclose(fptr_tmp);
	 fclose(fptr_list);
	 }
      }
   return(1);
   } /* create_temp_files() */

int get_section_line(int file_no,
		     int section_no,
		     char current_section[50])
   {
   FILE *fptr_file;
   int line;
   int number_of_sections;
   enum boolean section_found;
   enum directive directive;
   char file_name[20];
   char string_in[101];
   char directive_string[100];

   /* Open file */
   strcpy(file_name,"TEMP00.LST");
   make_temp_file_name(file_name,
		       4,
	   	       file_no);
   if ((fptr_file = fopen(file_name,"r")) == NULL)
      {
      printf("Cannot open file %s\n",file_name);
      exit(0);
      }

   /* find section to be copied */
   number_of_sections = 0;
   section_found = false;
   line = 0;
   while (section_found == false)
      {
      if (fgets(string_in, 100, fptr_file) == NULL)
         break;
      if (is_blank_line(string_in) == 0)
	 {
	 if (string_in[0] == '*')
	    {
	    directive = check_directive(string_in, directive_string);
	    if (directive == title)
	       {
	       if (strcmp("next",current_section) == 0)
                  {
	          if (number_of_sections == section_no)
		     section_found = true;
                  }
	       else
	          if (strcmp(directive_string,current_section) == 0)
		     section_found = true;
	       number_of_sections++;
	       }
	    }
	 }
      line++;
      }
   fclose(fptr_file);
   if (section_found == true)
      return(line);
   else
      return(0);
   }  /* get_section_line() */

int get_file_line(int file_no,
		  int section_no,
		  char current_section[50],
                  FILE **fptr_rtn)
   {
   FILE *fptr_file;
   int line;
   int number_of_sections;
   enum boolean section_found;
   enum boolean first_section;
   enum directive directive;
   char file_name[20];
   char string_in[101];
   char directive_string[100];

   /* Open file */
   if (*fptr_rtn == NULL)
      {
      first_section = true;
      strcpy(file_name,"TEMP00.LST");
      make_temp_file_name(file_name,
		          4,
	   	          file_no);
      if ((fptr_file = fopen(file_name,"r+")) == NULL)
         {
         printf("Cannot open file %s\n",file_name);
         exit(0);
         }
      }
   else
      {
      first_section = false;
      fptr_file = *fptr_rtn;
      }

   /* find section to be copied */
   number_of_sections = 0;
   section_found = false;
   line = 0;
   while (section_found == false)
      {
      if (fgets(string_in, 100, fptr_file) == NULL)
         break;
      if (is_blank_line(string_in) == 0)
	 {
	 if (string_in[0] == '*')
	    {
	    directive = check_directive(string_in, directive_string);
	    if (directive == title)
	       {
	       if (strcmp("next",current_section) == 0)
                  {
                  if (first_section == false)
                     section_found = true;
                  else
	             if (number_of_sections == section_no)
		        section_found = true;
                  }
	       else
	          if (strcmp(directive_string,current_section) == 0)
		     section_found = true;
	       number_of_sections++;
	       }
	    }
	 }
      line++;
      }

   if (section_found == true)
      {
      *fptr_rtn = fptr_file;
      return(line);
      }
   else
      {
      fclose(fptr_file);
      return(0);
      }
   }  /* get_file_line() */

int display_int_data(char string_out[],
		     int number,
		     int position)
   {
   int i = 0;
   int j, k;
   char buffer[10];

   itoa(number,buffer,10);
   k = position;
   while (buffer[i] != '\0')
      i++;
   for (j=i-1; j>=0; j--)
      {
      string_out[k] = buffer[j];
      k--;
      }
   return 1;
   }  /* display_int_data() */

int display_float_data(char string_out[],
		       float number,
		       int position)
   {
   int i = 0;
   int l = 0;
   int j, k;
   int dec = 6;
   int sign = 0;
   char *buffer;

   buffer = fcvt(number,2,&dec,&sign);
   k = position;
   while (buffer[i] != '\0')
      i++;
   for (j=i-1; j>=0; j--)
      {
      string_out[k] = buffer[j];
      l++;
      k--;
      if (l == 2)
	 {
	 string_out[k] = '.';
	 k--;
	 }
      }
   return 1;
   }  /* display_float_data() */

struct part_info *find_record(char part_number[],
			      struct part_info *base_ptr)
   {
   struct part_info *this_ptr;

   this_ptr = base_ptr;
   while (this_ptr != NULL)
      {
      if (strcmp(this_ptr->part_number,part_number) == 0)
	 {
         return this_ptr;
	 }
      else
	 {
         this_ptr = this_ptr->part_ptr;
	 }
      }
   return NULL;
   }  /* find_record() */

struct part_info *read_tgt_dbase(FILE *fptr_dat)
   {
   struct part_info *base_ptr;
   struct part_info *new_ptr;
   struct part_info *last_ptr;
   struct part_info part_buf;

   base_ptr = (struct part_info *) NULL;
   while(fread(&part_buf,sizeof(part_buf),1,fptr_dat) != 0)
      {
      if ((new_ptr = (struct part_info *) malloc(sizeof(struct part_info))) == NULL)
	 return NULL;
      else
	 {
	 *new_ptr = part_buf;
	 if (base_ptr == (struct part_info *) NULL)
	    base_ptr = last_ptr = new_ptr;
	 else
	    {
	    last_ptr->part_ptr = new_ptr;
            new_ptr->part_ptr = NULL;
	    last_ptr = new_ptr;
	    }
         }
      }
   return base_ptr;
   }  /* read_tgt_dbase() */

struct part_info *find_part(char part_number[],
                            struct part_info *base_ptr)
   {
   struct part_info *search_ptr;

   search_ptr = base_ptr;
   while (search_ptr->part_ptr != NULL)
      {
      if (strcmp(search_ptr->part_number,part_number) == 0)
	 {
         return search_ptr;
	 }
      search_ptr = search_ptr->part_ptr;
      }
   return NULL;
   } /* find_part() */

display_part_data(struct part_info *part_ptr)
   {

   printf("%s %s [1]\n",part_ptr->part_number,part_ptr->description);
   printf("stock [2] : %d, re-order level [3] : %d\n",part_ptr->stock,part_ptr->order_level);
   printf("trade [4] : %f, retail [5] : %f\n",part_ptr->trade_cost,part_ptr->retail_cost);
   return 1;
   } /* display_part_data() */

int load_section(char section[max_sections][max_lines][100],
		 char current_section[],
		 char defn_file[20][50],
		 int title_posn)
   {
   FILE *fptr_list;
   enum directive directive;
   int j,k,l,m;
   enum boolean section_read;
   enum boolean in_section;
   char directive_string[50];
   char string_in[101];
   int line;

   for (k = 0; k < max_sections; k++)  /* clear section */
      for (l = 0; l < max_lines; l++)
         for (m = 0; m < 100; m++)
	    section[k][l][m] = ' ';

   /* printf("Looking for section : %s\n",current_section); */
   for (j = 1; j < title_posn; j++)	 /* Go through each file looking for */
      {				         /* current section */
      if ((fptr_list = fopen(defn_file[j],"r")) == NULL)
         {
	 printf("Cannot open file %s\n",defn_file[j]);
	 exit(0);
	 }
      else
         {
	 /* printf("Opened file %s\n",defn_file[j]); */
	 section_read = false;
	 in_section = false;
	 line = 0;
	 /* Go through file line by line */
	 while ((fgets(string_in, 100, fptr_list) != NULL) &&
		   section_read == false)
	    {
	    if (is_blank_line(string_in) == 0)
	       {
	       if (string_in[0] == '*')
	          {
		  directive = check_directive(string_in, directive_string);
		  if (directive == title)
		     if (strcmp(directive_string,current_section) == 0)
		        {
			/* printf("Found section : %s\n",current_section); */
			in_section = true;
			}
		     else
			{
			if (in_section == true)
			   {
			   /* mark end of section */
			   section[j - 1][line][0] = '%';
			   section[j - 1][line][1] = '\0';
			   line++;
			   section_read = true;
			   }
			in_section = false;
			}
		  /* Only these directives are allowed within a section */
		  else if (directive == variant_start ||
			   directive == variant ||
			   directive == variant_end ||
			   directive == variant_quantity ||
			   directive == variant_quantity_end ||
			   directive == variant_assembly_start ||
			   directive == variant_assembly ||
			   directive == in_assembly ||
			   directive == common_assembly ||
			   directive == variant_assembly_end ||
			   directive == variant_one_of_set ||
			   directive == end_variant_set )
		     {
		     strcpy(&section[j - 1][line][0],string_in);
		     line++;
		     }
		  else
		     {
		     if (in_section == true)
		        {
			section[j - 1][line][0] = '%'; /* mark end of */
			section_read = true; 	       /* section     */
			section[j - 1][line][1] = '\0';
			line++;
			}
		     in_section = false;
		     }
		  }
	       else if (in_section == true)
	          {
		  k = 0;
		  while (string_in[k] != '\0' &&   /* Eliminate any comments */
		         string_in[k] != '\n' &&
			 k < 100)
		     {
		     if (string_in[k] == '*' &&
		         string_in[k - 1] == '/')
		        {
			string_in[k - 1] = '\0';
			break;
			}
		     k++;
		     }
		  remove_leading_blanks(string_in,           /* Remove indentation */
		                        description_position); /* from description */
		  strcpy(&section[j - 1][line][0],string_in);
		  line++;
		  }
	       }
	    }
	 }
      fclose(fptr_list);
      }	  /* At this point the same section should have been read */
	  /* from each file */
   return(1);
   }  /* load_section() */

int build_alt_part_number(char alt_part_number[50],
                          char alt_part_manufacturer[20],
                          char source_alt_part_number[20])
   {
   alt_part_number[0] = '*';
   alt_part_number[1] = '\0';
   strcat(alt_part_number, alt_part_manufacturer);
   strcat(alt_part_number, "_");
   strcat(alt_part_number, source_alt_part_number);
   return 1;
   }  /* build_alt_part_number() */

