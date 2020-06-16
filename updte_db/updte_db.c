/*                              updte_db.c                                */
/* The purpose of this program is to update a parts database with data    */
/* from a text file.                                                      */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int update_database(struct part_info *base_ptr,
		    char data_file[]);
int get_part_number_mod(char string_in[],
		        char string_out[],
		        int *place);

void main(int argc,
          char *argv[])
   {
   FILE *fptr_db;
   struct part_info *base_ptr;
   struct part_info *this_ptr;

   if ((fptr_db = fopen(argv[1],"rb")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }
   if ((base_ptr = read_tgt_dbase(fptr_db)) == NULL)
      {
      printf("Insufficient memory for database\n");
      exit(0);
      }
   else
      {
      if (update_database(fptr_db,
			  argv[2]) != 1)
	 {
	 printf("Database not updated\n");
	 exit(0);
         }
      }
   fclose(fptr_db);
   if ((fptr_db = fopen("argv[1]","wb")) == NULL)
      {
      printf("Cannot open file '%s' \n",argv[1]);
      exit(0);
      }
   this_ptr = base_ptr;
   while (this_ptr != NULL)
      {
      fwrite(this_ptr,sizeof(struct part_info), 1, fptr_db);
      this_ptr = this_ptr->part_ptr;
      }
   fcloseall();
   }  /* main() */

int update_database(struct part_info *base_ptr,
                    char data_file[])
   {
   FILE *fptr_data;
   struct part_info *this_ptr;
   struct part_info *new_ptr;
   char string_in[100];
   char part_number[10];
   char description[80];
   int position;
   enum boolean part_found;

   if ((fptr_data = fopen("data_file","r")) == NULL)
      {
      printf("Cannot open file '%s' \n",data_file);
      exit(0);
      }
   while(fgets(string_in, 100, fptr_data) != NULL)
      {
      position = 0;
      get_part_number_mod(string_in,
			  part_number,
                          true,
			  &position);
         {
	 this_ptr = base_ptr;
         part_found = false;
	 while (this_ptr != NULL &&
                part_found == false)
            {
	    if (strcmp(this_ptr->part_number,part_number) == 0)
	       {
               while (string_in[position] == ' ')  /* Find start of description */
               position++;
	       get_description(string_in,
			       description,
			       75,
			       position);
	       strcpy(new_ptr->part_number,part_number);
	       strcpy(new_ptr->description,description);
	       part_found = true;
	       }
	    else
               this_ptr = this_ptr->part_ptr;
	    }
	 }
      }
   fclose(fptr_data);
   return 1;
   }      /* update_database() */

int get_part_number_mod(char string_in[],
		        char string_out[],
		        int *place)
   {
   int i,j;

   i = *place;
   j = 0;
   while (string_in[i] != ' ' &&
	  string_in[i] != '\t')
      {
      string_out[j] = string_in[i];
      j++;
      i++;
      }
   string_out[j] = '\0';
   *place = i; 
   return 1;
   }  /* get_part_number_mod() */
