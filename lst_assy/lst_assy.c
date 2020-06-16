/*                              lst_assy.c                                 */
/*                                                                         */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

#define MAX_ASSYS 200

struct assy_data {
   char part_number[10];
   char description[80];
   };
		  
enum boolean part_is_assy(char part_number[],
			  int number,
			  struct assy_data assembly_list[]);

struct assy_data assembly_list[MAX_ASSYS];
	       
void main(int argc,
	  char *argv[])
   {
   FILE *fptr_in;
   FILE *fptr_defn;

   char defn_file[20][50];			/* store definition file */
   char string_in[101];
   char part_number[10];
   int i;
   int title_posn;
   int size;
   int position;
   int assembly_no;
   int source_file;
   enum boolean is_assembly;

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
   fclose(fptr_defn);
   size = i;   /* the number of lines in definition file */
   if (strncmp(&defn_file[0][0],"*definition_file",16) != 0)  /* Check 1st line */
      {
      printf("Incorrect file type : %s\n",argv[1]);
      exit(0);
      }
   i = 1;
   while (strncmp(&defn_file[i][0],"*titles",7) != 0 &&
	  i < size)
      i++;
   title_posn = i;               /* Ignore any section titles */
   for (i=0; i<MAX_ASSYS; i++)   /* Initialise results array */
      assembly_list[i].part_number[0] = '\0';
   assembly_no = 0;
   for (source_file = 1; source_file < title_posn; source_file++)   /* i.e. for each parts list (i.e. file) */
      {
      if ((fptr_in = fopen(defn_file[source_file],"r")) == NULL)
         {
         printf("Cannot open file %s \n",defn_file[source_file]);
         exit(0);
         }
      while(fgets(string_in, 100, fptr_in) != NULL)
	 {
         is_assembly = false;
         if (is_blank_line(string_in) == 0)
	    if (string_in[0] == '*')
	       ;
	    else
	       {
	       /* Look for assembly definition */
	       i=1;
	       while (string_in[i] != '\n'&&
		      string_in[i] != '\0')
		  {
		  if (string_in[i] == '#')
		     {
		     is_assembly = true;
		     break;
		     }
		  i++;
		  }
	       if (is_assembly == true)
                  {
	          position = 3;
	          get_part_number(string_in,     /* Need to check returned value */
			          part_number,   /* eg 0 for blank part number */
			          true,
				  position);
		  if (part_is_assy(part_number,
				   assembly_no,
				   assembly_list) == false)      /* Look to see if part is already covered */
		     {
		     strcpy(assembly_list[assembly_no].part_number,part_number);
		     position = 11;
		     get_description(string_in,
		                     assembly_list[assembly_no].description,
			             75,
				     position);
                     remove_leading_blanks(assembly_list[assembly_no].description,
		     			   0);
		     assembly_no++;
		     }
		  }
	       }
	 }
      fclose(fptr_in);
      }
   for (i=0; i<assembly_no; i++)
      {
      printf("%s  ",assembly_list[i].part_number);
      printf("%s  ",assembly_list[i].description);
      printf("\n");
      }
   fcloseall();
   }  /* main() */

enum boolean part_is_assy(char part_number[],
			  int number,
			  struct assy_data assembly_list[])
   {
   int i;

   for (i=0; i<number; i++)
      {
      if (strcmp(assembly_list[i].part_number, part_number) == 0)
	 return true;
      }
   return false;
   }  /* part_is_assy() */

