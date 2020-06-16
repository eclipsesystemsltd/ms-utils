/*                              order_db.c                                */

/*
ORDER_DB.C

This program writes the contents of a parts database to the screen in order of
part number.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int sort_db(struct part_info_mod *part_ptrs[],
	    int size);
int print_sorted_db(struct part_info_mod *part_ptrs[],
                    int size);

struct part_info_mod *read_tgt_dbase_mod(FILE *fptr_dat,
				         int *size,
					 struct part_info_mod *part_ptrs[]);

void main(int argc,
	  char *argv[])
   {
   FILE *fptr_in;
   FILE *fptr_db;
   struct part_info_mod *base_ptr;
//   struct part_info_mod *part_data;
//   int items;
   struct part_info_mod *part_ptrs;
//   struct part_info_mod *temp_ptr;
   int size;
//   char string_in[101];
//   char string_out[101];
//   char part_number[10];
//   enum boolean print_string_in;

   if ((part_ptrs = (struct part_info_mod *)
		     calloc(2000,sizeof(struct part_info_mod *))) == NULL)
      {
      printf("Insufficient memory for pointer array\n");
      exit(0);
      }
   if ((fptr_in = fopen(argv[1],"r")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }
   if ((fptr_db = fopen("dbase.dat","rb")) == NULL)
      {
      printf("Cannot open file %s \n","dbase.dat");
      exit(0);
      }

   if ((base_ptr = read_tgt_dbase_mod(fptr_db,
				      &size,
				      part_ptrs)) == NULL)
      {
      printf("Insufficient memory for target database\n");
      exit(0);
      }
   sort_db(part_ptrs,
	   size);
   print_sorted_db(part_ptrs,
		   size);

  }  /* main() */

struct part_info_mod *read_tgt_dbase_mod(FILE *fptr_dat,
				         int *size,
				         struct part_info_mod *part_ptrs[])
   {
   struct part_info_mod *base_ptr;
   struct part_info_mod *new_ptr;
   struct part_info_mod *last_ptr;
   struct part_info part_buf;
   int i,j;
   char number[10];

   *size = 0;
   base_ptr = (struct part_info_mod *) NULL;
   while(fread(&part_buf,sizeof(part_buf),1,fptr_dat) != 0)
      {
      if ((new_ptr = (struct part_info_mod *) malloc(sizeof(struct part_info_mod))) == NULL)
	 return NULL;
      else
	 {
         strcpy(new_ptr->part_number,part_buf.part_number);
	 strcpy(new_ptr->description,part_buf.description);
         for (i=0; i<20; i++)
            new_ptr->models[i] = part_buf.models[i];
         new_ptr->stock = part_buf.stock;
         new_ptr->order_level = part_buf.order_level;
         new_ptr->trade_cost = part_buf.trade_cost;
         new_ptr->retail_cost = part_buf.retail_cost;
	 if (base_ptr == (struct part_info_mod *) NULL)
            {
	    base_ptr = last_ptr = new_ptr;
            new_ptr->part_ptr = NULL;
            }
	 else
	    {
	    last_ptr->part_ptr = new_ptr;
            new_ptr->part_ptr = NULL;
	    last_ptr = new_ptr;
	    }
         i=0;
	 j=0;
	 do
	    {
	    if (isdigit(new_ptr->part_number[i]))
               {
	       number[j] = new_ptr->part_number[i];
	       j++;
	       }
	    i++;
	    } while (new_ptr->part_number[i] != '\0');
         number[j] = '\0';
	 new_ptr->part_no = atol(number);
	 part_ptrs[*size] = new_ptr;
	 (*size)++;
	 }
      }
   return base_ptr;
   }  /* read_tgt_dbase() */

int sort_db(struct part_info_mod *part_ptrs[],
	    int size)
   {
   struct part_info_mod *temp_ptr;
   int a,b;

   for (a=0; a<size-1; a++)
      for (b=a+1; b<size; b++)
	 if (part_ptrs[a]->part_no > part_ptrs[b]->part_no)
	    {
	    temp_ptr = part_ptrs[b];
	    part_ptrs[b] = part_ptrs[a];
	    part_ptrs[a] = temp_ptr;
	    }
   return 1;
   }  /* sort_db() */

int print_sorted_db(struct part_info_mod *part_ptrs[],
                    int size)
   {
   int i;

   for (i=0; i<size; i++)
      {
      printf("%s %s\n",part_ptrs[i]->part_number,part_ptrs[i]->description);
      }
   return 1;
   }  /* print_sorted_db() */
