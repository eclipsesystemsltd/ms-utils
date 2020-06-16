/*                              mk_base.c                                 */
/* The purpose of this program is to load a parts database onto disk.     */

/*
MK_DBASE.C

The purpose of this program is to build a new parts database.

The inputs to the program are as follows :-

   (a) A text file "descript.asc" where each line contains a part number
       followed by a description. This file can be the text file output by
       CMP_DESC.C. 

   (b) A text file "stock.asc" where each line contains a part number
       followed by a stock quantity and a trade price.

   (c) A text file "assys.asc" where each line contains a part number
       followed by a description. Each part number represents an assembly.

   (d) A data file "get_mods.dat" which defines which models a part is
       applicable to.    *** Note this is not currently implemented ***

The output is a data file "dbase.dat" containing records of type 'part_info'.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include <math.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int free_db(struct part_info *base_ptr);
int get_part_number_mod(char string_in[],
		        char string_out[],
		        int *place);
int get_stock_quantity(char string_in[],
		       int *position);

#define MAX_ASSYS 200

enum check_or_get {
   get,
   check};

enum boolean an_assembly(char part_number[],
			 char assys_list[MAX_ASSYS][10],
			 int no_of_assys);
struct part_info *read_parts_data();
void read_models_data(struct part_info *base_ptr);
void read_stock_data(struct part_info *base_ptr);
float get_stock_cost(char string_in[],
		     int *position);

void main()
   {
//   FILE *fptr_in;
//   FILE *fptr_out;
//   FILE *fptr_defn;
//   FILE *fptr_list;
   FILE *fptr_res;
//   FILE *fptr_mod_ref;
//   int i;
   struct part_info *base_ptr;
   struct part_info *this_ptr;

   printf("Reading parts data ...\n");
   if ((base_ptr = read_parts_data()) == NULL)
      printf("Insufficient memory for parts data\n");
   printf("Parts data read\n");
   /* read_models_data(base_ptr);   */
   read_stock_data(base_ptr);
   if ((fptr_res = fopen("dbase.dat","wb")) == NULL)
      {
      printf("Cannot open file 'dbase.dat' \n");
      exit(0);
      }
   printf("Stock data read\n");
   this_ptr = base_ptr;
   while (this_ptr != NULL)
      {
      fwrite(this_ptr,sizeof(struct part_info), 1, fptr_res);
      this_ptr = this_ptr->part_ptr;
      }
   free_db(base_ptr);
   fcloseall();
   }  /* main() */

int free_db(struct part_info *base_ptr)
   {
   struct part_info *this_ptr;
   struct part_info *next_ptr;

   this_ptr = base_ptr;
   while (this_ptr != NULL)
      {
      next_ptr = this_ptr->part_ptr;
      free(this_ptr);
      this_ptr = next_ptr;
      }
   return 1;
   } /* free_db() */

struct part_info *read_parts_data()
   {
   /* This function reads part number and description */
   FILE *fptr_parts;
   FILE *fptr_assys;
   char string_in[100];
   char part_number[10];
   char description[100];
   char assys_list[MAX_ASSYS][10];
   struct part_info *base_ptr;
   struct part_info *last_ptr;
   struct part_info *new_ptr;
   int i;
   int position;
   int desc_start;
   int assemblies;

   if ((fptr_parts = fopen("descript.asc","r")) == NULL)
      {
      printf("Cannot open file 'descript.asc' \n");
      exit(0);
      }
   if ((fptr_assys = fopen("assys.asc","r")) == NULL)
      {
      printf("Cannot open file 'assys.asc' \n");
      exit(0);
      }
   assemblies = 0;
   while(fgets(string_in, 100, fptr_assys) != NULL)
      {
      get_part_number(string_in,     /* Need to check returned value */
                      part_number,   /* eg 0 for blank part number */
		      true,
		      0);
      strcpy(assys_list[assemblies],part_number);
      assemblies++;
      }
   fclose(fptr_assys);
   base_ptr = (struct part_info *) NULL;

   while(fgets(string_in, 100, fptr_parts) != NULL)
      {
      if ((new_ptr = (struct part_info *) malloc(sizeof(struct part_info))) == NULL)
	 return NULL;
      else
	 {
	 position = 0;
	 get_part_number_mod(string_in,
			     part_number,
			     &position);
	 while (string_in[position] == ' ')  /* Find start of description */
	    position++;
         desc_start = position;
	 get_description(string_in,
			 description,
			 75,
			 position);
	 strcpy(new_ptr->part_number,part_number);
	 strcpy(new_ptr->description,description);
	 if (an_assembly(part_number,
			 assys_list,
			 assemblies) == true)
	    new_ptr->assembly = true;
	 else
	    new_ptr->assembly = false;
	 for (i=0; i<20; i++)
	    new_ptr->models[i] = 0;
	 new_ptr->stock = 0;
	 new_ptr->order_level = 0;
	 new_ptr->trade_cost = 0.0;
	 new_ptr->retail_cost = 0.0;
         new_ptr->part_ptr = NULL;
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
   fclose(fptr_parts);
   return base_ptr;
   } /* read_parts_data() */

struct part_info *read_parts_data_old()
   {
   /* This function reads part number and description */
   FILE *fptr_parts;
   struct part part_buf;
   struct part_info *base_ptr;
   struct part_info *last_ptr;
   struct part_info *new_ptr;
   int i;

   if ((fptr_parts = fopen("one_desc.dat","rb")) == NULL)
      {
      printf("Cannot open file 'one_desc.dat' \n");
      exit(0);
      }
   base_ptr = (struct part_info *) NULL;
   while (fread(&part_buf,sizeof(part_buf),1,fptr_parts) == 1)
      {
      if ((new_ptr = (struct part_info *) malloc(sizeof(struct part_info))) == NULL)
	 return NULL;
      else
	 {
	 strcpy(new_ptr->part_number,part_buf.part_number);
	 strcpy(new_ptr->description,part_buf.description);

	 for (i=0; i<20; i++)
	    new_ptr->models[i] = 0;
	 new_ptr->stock = 0;
	 new_ptr->order_level = 0;
	 new_ptr->trade_cost = 0.0;
         new_ptr->retail_cost = 0.0;
	 if (base_ptr == (struct part_info *) NULL)
	    base_ptr = last_ptr = new_ptr;
	 else
	    {
	    last_ptr->part_ptr = new_ptr;
	    last_ptr = new_ptr;
	    }
	 }
      }
   fclose(fptr_parts);
   return base_ptr;
   } /* read_parts_data_old() */

void read_models_data(struct part_info *base_ptr)
   {
   /* This function gets model data (i.e. series of model references) for */
   /* a given part number     */
   FILE *fptr_models;
   struct part_model_ref part_data;
   struct part_info *this_ptr;
   enum boolean part_found;
   int i;

   if ((fptr_models = fopen("get_mods.dat","rb")) == NULL)
      {
      printf("Cannot open file 'get_mods.dat' \n");
      exit(0);
      }
   while (fread(&part_data,sizeof(part_data),1,fptr_models) != 0)
      {
      this_ptr = base_ptr;
      part_found = false;
      while (this_ptr != NULL &&
             part_found == false)
         {
	 if (strcmp(this_ptr->part_number,part_data.part_number) == 0)
	    {
            i = 0;
	    while (part_data.model[i] != 0 &&
		   i < 30)
	       {
	       this_ptr->models[i] = part_data.model[i];
	       i++;
               }
	    part_found = true;
	    }
         this_ptr = this_ptr->part_ptr;
	 }
      }
   fclose(fptr_models);
   } /* read_models_data() */

void read_stock_data(struct part_info *base_ptr)
   {
   /* This procedure reads price and quantity for a series of given part numbers */   FILE *fptr_stock;
   struct part_info *this_ptr;
   char string_in[100];
   char part_number[10];
   int position;
   enum boolean part_found;

   if ((fptr_stock = fopen("stock.asc","r")) == NULL)
      {
      printf("Cannot open file 'stock.asc' \n");
      exit(0);
      }
   while(fgets(string_in, 100, fptr_stock) != NULL)
      {
      if (isalnum(string_in[0]) != 0)
         {
         position = 0;
         if (get_part_number_mod(string_in,
			         part_number,
				 &position) == 1)
	    {
	    this_ptr = base_ptr;
            part_found = false;
	    while (this_ptr != NULL &&
                   part_found == false)
               {
	       if (strcmp(this_ptr->part_number,part_number) == 0)
	          {
	          this_ptr->stock = get_stock_quantity(string_in,
					               &position);
	          this_ptr->trade_cost = get_stock_cost(string_in,
						        &position);
	          part_found = true;
	          }
	       else
                  this_ptr = this_ptr->part_ptr;
	       }
	    }
         }
      }
   fclose(fptr_stock);
   }      /* read_stock_data() */

int get_stock_quantity(char string_in[],
		       int *position)
   {
   char quantity[10];
   int i = *position;
   int j;

   while (string_in[i] == ' ')
      i++;
   j = 0;
   while (string_in[i] != ' ')
      {
      quantity[j] = string_in[i];
      i++;
      j++;
      }
   quantity[j] = '\0';
   *position = i;
   return atoi(quantity);
   }  /* get_stock_quantity() */

float get_stock_cost(char string_in[],
                     int *position)
   {
   char stock[10];
   int i = *position;
   int j;

   for (j=0; j<10; j++)
      stock[j] = ' ';
   stock[0] = '\0';
   while (string_in[i] == ' ')
      i++;
   j = 0;
   while (string_in[i] != ' ' &&
	  string_in[i] != '\0')
      {
      stock[j] = string_in[i];
      i++;
      j++;
      }
   stock[j] = '\0';
   *position = i;
   return (float) atof(stock);
   }  /* get_stock_cost() */

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

enum boolean an_assembly(char part_number[],
			 char assys_list[MAX_ASSYS][10],
			 int no_of_assys)
   {
   int i;

   for (i=0; i<no_of_assys; i++)
      {
      if (strcmp(assys_list[i],part_number) == 0)
         return true;
      }
   return false;
   } /* an_assembly() */

