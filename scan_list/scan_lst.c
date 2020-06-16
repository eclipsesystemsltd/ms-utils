                               /* scan_lst.c */

/*
SCAN_LST.C

This program takes two parts lists. For each part number in a given section 
in the second list which has an equivalent in the first list, the description
from the first list is used to replace that in the second.

The two parts lists to be used are specified in a definition file of the same
format as used for CHK_ASSY, etc. The first file listed in the definition file
is the new file whose descriptions will be updated, the second file is used to
provide the replacement descriptions.

E.g C:> SCAN_LST PARTS.DEF

The output is directed to the screen.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int create_temp_files_2(char defn_file[20][50],
		        int title_posn,
                        enum boolean indentation);
int get_section_2(char section[max_lines][100],
		  int file_no,
		  int section_no,
		  char current_section[50],
                  enum boolean indentation);
int find_searched_line(char string_out[],
                       char directive_string[],
                       enum boolean *output_directive,
                       char source_line[],
                       char searched_section[100][100]);
int compare_part_numbers(char source_line[],
                         char searched_line[],
                         char part_number[],
                         char alt_part_number[],
                         enum boolean six_digit_part_no);
int compare_quantity(char source_line[],
                     char searched_line[],
                     char quantity[]);
int get_assy_defn_string(char string_in[],
                         char assembly_string[]);
int build_line(char string_out[100],
               char ref_number[10],
               char part_number[20],
               char description[80],
               char alt_part_number[40],
               char assembly_string[10],
               char quantity[10]);

char source_section[100][100];
char searched_section[100][100];

void main(int argc,
	  char *argv[])
   {
   FILE *fptr_defn;
   char defn_file[20][50];			/* store definition file */
   char current_section[50];			/* title of current section */
   char string_in[101];
   char string_out[101];
   char directive_string[81];
   enum directive directive = none;
   enum boolean end_of_source_file;
   enum boolean got_source_section;
//   enum boolean end_of_searched_file;
   enum boolean got_searched_section;
   enum boolean all_sections = false;
   enum boolean end_of_source_section;
   enum boolean end_of_searched_section;
   enum boolean output_directive;
   int source_section_no;
//   int searched_section_no;
   int source_file;
   int searched_file;
   int source_sect;
//   int searched_sect;
   int size;
   int i, l;
   int title_posn;

   if ((fptr_defn = fopen(argv[1],"r")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }
   i = 0;
   while (fgets(string_in, 40, fptr_defn) != NULL)
      {
      str_build_1(defn_file[i],string_in);
      i++;
      };
   size = i;   /* the number of lines in definition file */
   if (strncmp(&defn_file[0][0],"*definition_file",16) != 0)  /* Check 1st line */
      {
      printf("Incorrect file type : %s\n",argv[1]);
      exit(0);
      }
   else
      {
      i = 1;
      while (strncmp(&defn_file[i][0],"*titles",7) != 0 &&
	     i < size)
	 i++;
      if (i == size - 1)
	 {
	 printf("No section titles found\n");
         all_sections = true;
	 title_posn = i;
	 /* exit(); */
	 }
      else
	 title_posn = i;
      }
   if (create_temp_files_2(defn_file,
			   title_posn,
                           true) != 1)
      {
      printf("Unable to generate duplicate files\n");
      }
   source_file = 1;
   end_of_source_file = false;
   source_section_no = 0;
   source_sect = 0;
   while (end_of_source_file == false)
      {
      /* Get next source section */
      if (all_sections == true)
	 {
         strcpy(current_section,"next");
	 if (get_section(source_section,
			 source_file,
			 source_section_no,
			 current_section) == 0)
	    {
	    end_of_source_file = true;
	    got_source_section = false;
	    }
	 else
	    got_source_section = true;
	 }
      else
	 {
	 strcpy(current_section,defn_file[title_posn + source_sect + 1]); /* This is the   */
								          /* section title */
	 if (get_section(source_section,
			 source_file,
		         source_section_no,
			 current_section) == 0)
	    got_source_section = false;
	 else
	    {
	    got_source_section = true;
	    }
         source_sect++;
	 if (source_sect == size - title_posn - 1)
	    end_of_source_file = true;
	 }
         /* Go through source section line by line */
      if (got_source_section == true)
	 {
         strcpy(string_out, "*title ");
         strcat(string_out, current_section);
         strcat(string_out, "\n");
	 fputs(string_out, stdout);
         /* See if equivalent section exists in searched file */
         searched_file = 2;
         if (get_section_2(searched_section,
		           searched_file,
		           0,
		           current_section,
                           true) == 0)
            got_searched_section = false;
         else
            {
	    got_searched_section = true;
	    }
         if (got_searched_section == false)
            {
            /* Copy source section exactly */
	    l = 0;
            end_of_source_section = false;
	    while (end_of_source_section == false)
               {
	       if (source_section[l][0] == '%')
	          end_of_source_section = true;
	       else 
	          {
	          fputs(source_section[l],stdout);
		  }
               l++;
	       }
            }
         else
            {
	    l = 0;
            end_of_source_section = false;
	    while (end_of_source_section == false)
               {
	       if (source_section[l][0] == '*')
                  {
                  if (strncmp(source_section[l], "*page", 5) == 0)
                     fputs(source_section[l],stdout);
                  }
	       else if (source_section[l][0] == '%')
	          end_of_source_section = true;
               else if (source_section[l][0] == '$')
	          ;
	       else 
	          {
                  /* search through other file for section and part number */
                  if (find_searched_line(string_out,
                                         directive_string,
                                         &output_directive,
                                         source_section[l],
                                         searched_section) == 1)
                     {
                     if (output_directive == true)
                        fputs(directive_string, stdout);
	             fputs(string_out,stdout);
                     }
                  else
                     {
                     printf("ORIG ");
	             fputs(source_section[l],stdout);
                     }
		  }
               l++;
	       }
            /* Output remnant of searched section  */
	    l = 0;
            end_of_searched_section = false;
            fputs("REMNANT OF SEARCHED SECTION\n", stdout);
	    while (end_of_searched_section == false)
               {
               if (searched_section[l][0] == '%')
                  end_of_searched_section = true;
               else if (searched_section[l][0] == '$')
                  ;
               else
	          fputs(searched_section[l],stdout);
               l++;
               }
            fputs("END OF SEARCHED SECTION\n", stdout);
	    }
	 source_section_no++;
	 }
      }
   fcloseall();
   }  /* main() */

int find_searched_line(char string_out[],
                       char directive_string[],
                       enum boolean *output_directive,
                       char source_line[],
                       char searched_section[100][100])
   {
   int position = 0;
   int line = 0;
   char ref_number[10];
   char part_number[20];
   char description[80];
   char alt_part_number[20];
   char quantity[10];
   char assembly_string[10];
   enum boolean end_of_section_searched = false;
   enum boolean last_line_directive = false;
   enum directive directive;

   get_ref_number(source_line,
		  ref_number,
		  position);
   while (end_of_section_searched == false)
      {
      if (searched_section[line][0] == '$')
         last_line_directive = false;
      else if (searched_section[line][0] == '%')
         end_of_section_searched = true;
      else if (searched_section[line][0] == '*')
         {
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
            last_line_directive = true;
            strcpy(directive_string, searched_section[line]);
            }
	 }
      else
         {
         if (compare_part_numbers(source_line,
                                  searched_section[line],
                                  part_number,
                                  alt_part_number,
                                  true) == 1)
            {
            get_description(searched_section[line],
		            description,
		            80,
		            11);
            compare_quantity(source_line,
                             searched_section[line],
                             quantity);
            get_assy_defn_string(searched_section[line],
                                 assembly_string);
            build_line(string_out,
                       ref_number,
                       part_number,
                       description,
                       alt_part_number,
                       assembly_string,
                       quantity);
            searched_section[line][0] = '$';
            *output_directive = last_line_directive;
            return 1;
            }
         else
            last_line_directive = false;
         }
      line++;
      }
   return 0;
   } /* find_searched_line() */

int compare_part_numbers(char source_line[],
                         char searched_line[],
                         char part_number[],
                         char alt_part_number[],
                         enum boolean six_digit_part_no)
   {
/*               Logic of part number comparison 
*  --------+---------+----------+-----------+-------------------------------
*   Src PN | Src Alt | Srchd PN | Srchd Alt |
*  --------+---------+----------+-----------+-------------------------------
*  0   0   |    0    |     0    |     0     | \
*  1   0   |    0    |     0    |     1     | |
*  2   0   |    0    |     1    |     0     |  > Skip to next
*  3   0   |    0    |     1    |     1     | |
*  4   0   |    1    |     0    |     0     | /
*  5   0   |    1    |     0    |     1     | Cmp Alt; Use Source Alt
*  6   0   |    1    |     1    |     0     | Skip to next
*  7   0   |    1    |     1    |     1     | Cmp Alt; Use Src Alt, srch PN
*  8   1   |    0    |     0    |     0     | Skip to next
*  9   1   |    0    |     0    |     1     | Skip to next
* 10   1   |    0    |     1    |     0     | Cmp PN; Use Src PN
* 11   1   |    0    |     1    |     1     | Cmp PN; Use Src PN, srch Alt
* 12   1   |    1    |     0    |     0     | Skip to next
* 13   1   |    1    |     0    |     1     | Cmp Alt; Use Src Alt, src PN
* 14   1   |    1    |     1    |     0     | Cmp PN; Use Src PN, src Alt
* 15   1   |    1    |     1    |     1     | Cmp PN & Alt; Use Src PN, src Alt
*  --------+---------+----------+-----------+------------------------------- */
   int position = 3;
   int i;
   char source_part_number[20];
   char source_alt_part_number[20];
   char source_alt_part_manufacturer[20];
   char searched_part_number[20];
   char searched_alt_part_number[20];
   char searched_alt_part_manufacturer[20];
   char dummy_string[80];
   int source_part_number_value = 0;
   int source_alt_part_number_value = 0;
   int searched_part_number_value = 0;
   int searched_alt_part_number_value = 0;
   int value;

   part_number[0] = '\0';
   alt_part_number[0] = '\0';
   source_part_number[0] = '\0';
   source_alt_part_number[0] = '\0';
   source_alt_part_manufacturer[0] = '\0';
   searched_part_number[0] = '\0';
   searched_alt_part_number[0] = '\0';
   searched_alt_part_manufacturer[0] = '\0';
   if (get_part_number(source_line,
                       source_part_number,
		       six_digit_part_no,
		       position) == 1)
      source_part_number_value = 8;
   i=1;
   while (source_line[i] != '\n'&&
          source_line[i] != '\0')
      {
      if (source_line[i] == '*')
         {
	 position = i;
         if (check_alt_part_number(source_line,
                                   source_alt_part_manufacturer,
                                   source_alt_part_number,
                                   dummy_string,
			     	   &position) == 1)
            {
            source_alt_part_number_value = 4;
            }
         break;
         }
      i++;
      }
   position = 3;
   if (get_part_number(searched_line,
                       searched_part_number,
		       six_digit_part_no,
		       position) == 1)
      searched_part_number_value = 2;
   i=1;
   while (searched_line[i] != '\n'&&
          searched_line[i] != '\0')
      {
      if (searched_line[i] == '*')
         {
	 position = i;
         if (check_alt_part_number(searched_line,
                                   searched_alt_part_manufacturer,
                                   searched_alt_part_number,
                                   dummy_string,
			     	   &position) == 1)
            {
            searched_alt_part_number_value = 1;
            }
         break;
         }
      i++;
      }
   value = source_part_number_value +
           source_alt_part_number_value +
           searched_part_number_value +
           searched_alt_part_number_value;
   switch (value)
      {
      case 0:
         return 0;
      case 5:
         if (strcmp(source_alt_part_number, searched_alt_part_number) == 0 &&
             strcmp(source_alt_part_manufacturer, searched_alt_part_manufacturer) == 0)
            {
            strcpy(part_number, "-------");
            build_alt_part_number(alt_part_number,
                                  source_alt_part_manufacturer,
                                  source_alt_part_number);
            return 1;
            }
         break;
      case 7:
         if (strcmp(source_alt_part_number, searched_alt_part_number) == 0 &&
             strcmp(source_alt_part_manufacturer, searched_alt_part_manufacturer) == 0)
            {
            strcpy(part_number, searched_part_number);
            build_alt_part_number(alt_part_number,
                                  source_alt_part_manufacturer,
                                  source_alt_part_number);
            return 1;
            }
         break;
      case 10:
         if (strcmp(source_part_number, searched_part_number) == 0)
            {
            strcpy(part_number, source_part_number);
            return 1;
            }
         break;
      case 11:
         if (strcmp(source_part_number, searched_part_number) == 0)
            {
            strcpy(part_number, source_part_number);
            build_alt_part_number(alt_part_number,
                                  searched_alt_part_manufacturer,
                                  searched_alt_part_number);
            return 1;
            }
         break;
      case 13:
         if (strcmp(source_alt_part_number, searched_alt_part_number) == 0 &&
             strcmp(source_alt_part_manufacturer, searched_alt_part_manufacturer) == 0)
            {
            strcpy(part_number, source_part_number);
            build_alt_part_number(alt_part_number,
                                  source_alt_part_manufacturer,
                                  source_alt_part_number);
            return 1;
            }
         break;
      case 14:
         if (strcmp(source_part_number, searched_part_number) == 0)
            {
            strcpy(part_number, source_part_number);
            build_alt_part_number(alt_part_number,
                                  source_alt_part_manufacturer,
                                  source_alt_part_number);
            return 1;
            }
         break;
      case 15:
         if (strcmp(source_part_number, searched_part_number) == 0)
            {
            strcpy(part_number, source_part_number);
            if (strcmp(source_alt_part_number, searched_alt_part_number) == 0 &&
                strcmp(source_alt_part_manufacturer, searched_alt_part_manufacturer) == 0)
               build_alt_part_number(alt_part_number,
                                     source_alt_part_manufacturer,
                                     source_alt_part_number);
            else
               strcpy(alt_part_number, "*ALT_ERROR");
            return 1;
            }
         break;
      default :
         return 0;
      }
   return 0;
   }   /* compare_part_numbers() */
    
int compare_quantity(char source_line[],
                     char searched_line[],
                     char quantity[])
   {
   int i;
   int position;
   char source_quantity[10];
   char searched_quantity[10];

   i=1;
   while (source_line[i] != '\n'&&
	  source_line[i] != '\0')
      {
      if (source_line[i] == '$')
         {
         position = i;
         break;
         }
      i++;
      }
   get_quantity(source_line,
		source_quantity,
		position);
   i=1;
   while (searched_line[i] != '\n'&&
	  searched_line[i] != '\0')
      {
      if (searched_line[i] == '$')
         {
         position = i;
         break;
         }
      i++;
      }
   get_quantity(searched_line,
		searched_quantity,
		position);
   if (strcmp(source_quantity, searched_quantity) == 0)
      {
      strcpy(quantity, source_quantity);
      return 1;
      }
   else
      {
      strcpy(quantity, "QTY_ERROR");
      return 0;
      }
   } /* compare_quantity() */

int get_assy_defn_string(char string_in[],
                         char assembly_string[])
   {
   int i = 1;
   int position;

   assembly_string[0] = '\0';
   while (string_in[i] != '\n' &&
	  string_in[i] != '\0')
      {
      if (string_in[i] == '#')
         {
         position = i;
         i = 0;
         while (string_in[position + i] != ' ')
            {
            assembly_string[i] = string_in[position + i];
            i++;
            }
         assembly_string[i] = '\0';
         break;
         }
      i++;
      }
   return 1;
   } /* get_assy_defn_string() */

int build_line(char string_out[100],
               char ref_number[10],
               char part_number[20],
               char description[80],
               char alt_part_number[40],
               char assembly_string[10],
               char quantity[10])
   {
   int desc_position = 11;
   int i;
   int position;

   string_out[0] = '\0';
   strcat(string_out, ref_number);
   strcat(string_out, " ");
   strcat(string_out, part_number);
   position = strlen(string_out);
   for (i = position; i < desc_position; i++)
      string_out[i] = ' ';
   string_out[i] = '\0';
   strcat(string_out, description);
   if (alt_part_number[0] != '\0')
      strcat(string_out, " ");
   strcat(string_out, alt_part_number);
   if (assembly_string[0] != '\0')
      strcat(string_out, " ");
   strcat(string_out, assembly_string);
   if (quantity[0] != '\0')
      strcat(string_out, " $");
   strcat(string_out, quantity);
   strcat(string_out,"\n");
   }  /* build_line() */

int get_section_2(char section[max_lines][100],
		  int file_no,
		  int section_no,
		  char current_section[50],
                  enum boolean indentation)
   {
   FILE *fptr_file;
   int j,k,l;
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
               if (indentation == false)
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
   }  /* get_section_2() */

int create_temp_files_2(char defn_file[20][50],
		        int title_posn,
                        enum boolean indentation)
   {
   FILE *fptr_list;
   FILE *fptr_tmp;
   char temp_file[12];
//   char number[5];
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
               if (indentation == false)
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
   } /* create_temp_files_2() */
