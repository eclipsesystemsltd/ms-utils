/*                                prn_part.c                               */
/* This program prints model information for each part in the database.    */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int load_model_reference(char models_file[],
		   /*	 struct model_reference models[], */
		         int *models_size);

int load_parts_data(char parts_file[],
		    /* struct model_reference models[], */
		    int *parts_size);

int models_size;
int no_of_parts;
struct part_model_ref parts[100];
struct model_reference models[50];

void main()
   {
   int i,j,k,l;

   load_model_reference("mod_ref.dat",
			&models_size);

   load_parts_data("get_mods.dat",
	           &no_of_parts);

   for (i = 0; i < no_of_parts; i++)
      {
      printf("%s \n", parts[i].part_number);
      j = 0; 
      while (parts[i].model[j] != 0)
	 {
	 for (k = 0; k < models_size; k++)
	    {
	    if (parts[i].model[j] == models[k].ref_number)
	       {
	       printf("   ");
	       for (l = 0; l < 10; l++)
		  printf(" %s", models[k].model_defn[l]);
               printf("\n");
	       break;
	       }
	    }
         if (k == models_size)
            printf("Model reference %d not found\n", parts[i].model[j]);
         j++;
         }
      }
   }   /* main() */

int load_model_reference(char models_file[],
		   /*	 struct model_reference models[], */
		         int *models_size)
   {
   FILE *fptr_models;
//   int size, j;

   if ((fptr_models = fopen(models_file,"rb")) == NULL)
      {
      printf("Cannot open file: %s ",models_file);
      return(0);
      }
   *models_size = 0;
   while (fread(&models[*models_size],sizeof(models[*models_size]),1,fptr_models) == 1 &&
          *models_size < 40)
      {
      *models_size = *models_size + 1;
      }
   fclose(fptr_models);
   return(1);
   }  /* load_model_reference() */

int load_parts_data(char parts_file[],
		    /* struct model_reference models[], */
		    int *parts_size)
   {
   FILE *fptr_parts;
//   int size, j;

   if ((fptr_parts = fopen(parts_file,"rb")) == NULL)
      {
      printf("Cannot open file: %s ",parts_file);
      return(0);
      }
   *parts_size = 0;
   while (fread(&parts[*parts_size],sizeof(parts[*parts_size]),1,fptr_parts) == 1 &&
	  *parts_size < 40)
      {
      *parts_size = *parts_size + 1;
      }
   fclose(fptr_parts);
   return(1);
   }   /*  load_parts_data()  */

