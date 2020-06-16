			  /* models.c */
/* This program produces a table containing all the models included in the  */
/* parts list files specified in the program input argument file.           */
/* The first two entries in each row of the model type table are the years  */
/* covered by the model, the third entry is the particular minor_model, and */
/* the following entries represent variants of the minor_model.             */
/* */ 
/*			 Data structures
 *
 * Definition file :-	   
 *
 *  +-------------------+  
 *  |*definition_file	| 
 *  |file_1		|   
 *  |file_2		|  
 *  | ...		|  
 *  |*titles		|   
 *  |title_1		| 
 *  |title_2		| 
 *  | ...		| 
 *  +-------------------+
 *                            */

/*
MODELS.C

This program produces a table containing all the models included in the parts
list files specified in the program input argument file. The first two entries
in each row of the model type table are the years covered by the model, the
third entry is the particular minor_model, and the following entries represent
variants of the minor_model.     

The input to this program is a definition file defining which parts lists are
to be analysed for model types and a set of original parts lists.

The output of the program is a data file 'models.dat' containing records of
type 'model_reference'.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int get_models_covered(char defn_file[20][50],
                       int file,
		       int *model_ref);
int find_model(/* struct model_reference models[], */
	       int models_so_far,
	       /* char current_model_defn[20][10][20], */
	       int defn_level,
               int current_model,
	       int no_of_minor_models);
int add_model(/* struct model_reference models[], */
	      int models_so_far,
	      /* char current_model_defn[20][10][20], */
	      int defn_level,
	      int current_model,
	      int model_ref);

char defn_file[20][50];		     /* store definition file */
struct model_reference models[200];   /* stores strings defining model and reference number */
char current_model_defn[20][10][20]; /* 20 models, each with 10 entries of 10 characters */

void main(int argc,
	  char *argv[])
   {
//   FILE *fptr_in;
//   FILE *fptr_out;
   FILE *fptr_defn;
//   FILE *fptr_list;
   FILE *fptr_res;

   char string_in[101];
//   char string_out[101];
//   char directive_string[81];
   enum directive directive = none;
//   enum boolean common_variant_assy[10];
//   enum boolean check_for_common_variant_assy[10];
   int i, k = 0;
   int size;
   int title_posn;
//   int row;
   int out;
   int start_model_ref;
//   int num_write;
   int source_file;

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
      if (i >= size)
	 {
	 printf("No section titles found\n");
	 exit(0);
	 }
      else
	 title_posn = i;
      }
   if ((fptr_res = fopen("models.dat","wb")) == NULL)
      {
      printf("Cannot open file 'models.dat' \n");
      exit(0);
      }
   start_model_ref = 1;
   for (source_file = 1; source_file < title_posn; source_file++)   /* i.e. for each parts list (i.e. file) */
      {
      get_models_covered(defn_file,
                         source_file,
			 &start_model_ref);
      i = 0;
      while (models[i].ref_number != 0)
	 {
	 fwrite(&models[i],sizeof(models[i]), 1, fptr_res);
	 printf("ref = %d  ",models[i].ref_number);
	 for (k = 0; k < 10; k++)
	    printf("%s ",models[i].model_defn[k]);
	 printf("\n");
         i++;
	 }
      }
   out = fcloseall();
   }  /* main() */

int get_models_covered(char defn_file[20][50],
                       int file,
		       int *model_ref)
   {
   FILE *fptr_list;
   enum directive directive;
   enum boolean year_read, model_read;
   enum boolean variant_change;
   int i,j,k,l,m,n;
   int row;
   int no_of_minor_models = 0;
   int defn_level;
   char directive_string[81];
   char year_string[10];
   char model_string[20];
   char string_in[101];
   char first_year[10],last_year[10];

   row = 0;
   if ((fptr_list = fopen(defn_file[file],"r")) == NULL)
      {
      printf("Cannot open file %s\n",defn_file[file]);
      exit(0);
      }
   else
      {
      printf("Opened file %s\n",defn_file[file]);
      /* Initialise models[] array */
      for (i = 0; i < 50; i++)
         {
	 models[i].ref_number = 0;
         models[i].major_model[0] = '\0';
         for (j = 0; j < 10; j++)
	    models[i].model_defn[j][0] = '\0';
         }
      /* Go through file line by line */
      for (i = 0; i < 20; i++)
         for (j = 0; j < 10; j++)
            {
	    for (l = 0; l < 20; l++)
	       current_model_defn[i][j][l] = ' ';
	    current_model_defn[i][j][0] = '\0';
	    }
      no_of_minor_models = 0;
      while (fgets(string_in, 100, fptr_list) != NULL)
	    {
	    if (string_in[0] == '*')
	       {
	       directive = check_directive(string_in, directive_string);
	       if (directive == year)
		  {
		  k = 0;         /* k is character in directive string */
		  l = 0;         /* l represents character in string */
		  m = 0;         /* m represents year covered as some */
				 /* parts lists cover multiple years  */
                  year_read = false;
                  do {
		     if (directive_string[k] == ',')
			{
			year_string[l] = '\0';
			if (m == 0)
			   /* first_year = atoi(year_string); */
			   strcpy(first_year,year_string);
			l = 0;
			m++;
			}
		     else if (directive_string[k] == '\0')
		        {
			year_string[l] = '\0';
			if (m == 0)
			   {
			   /* first_year = atoi(year_string); */
			   strcpy(first_year,year_string);
                           last_year[0] = '\0';
			   }
			else if (m > 0)
			   /* last_year = atoi(year_string); */
		           strcpy(last_year,year_string);
			year_read = true;
                        }
		     else
                        {
			year_string[l] = directive_string[k];
			l++;
                        }
		     k++;
		     } while (year_read == false);
		  }
	       else if (directive == major_model)
                  ;
		  /* strcpy(models[row].major_model,directive_string);*/
	       else if (directive == minor_model)
		  {
		  k = 0;
		  l = 0;
			   /* Each minor model forms root of model definition tree */
                  model_read = false;
		  do {
		     if (directive_string[k] == ',')
			{
			model_string[l] = '\0';
			strcpy(current_model_defn[no_of_minor_models][0],first_year);
			strcpy(current_model_defn[no_of_minor_models][1],last_year);
			strcpy(current_model_defn[no_of_minor_models][2],model_string);
			l = 0;
                        no_of_minor_models++;    /* next minor model needs new row */
			}
		     else if (directive_string[k] == '\0')
		        {
			model_string[l] = '\0';
			strcpy(current_model_defn[no_of_minor_models][0],first_year);
			strcpy(current_model_defn[no_of_minor_models][1],last_year);
			strcpy(current_model_defn[no_of_minor_models][2],model_string);
			no_of_minor_models++;
			defn_level = 2; 
		        variant_change = true;
			model_read = true;
                        }
		     else
                        {
			model_string[l] = directive_string[k];
			l++;
                        }
		     k++;
		     } while (model_read == false);
		  }
	       else if (directive == variant_start)    /* variants may be embedded */
		  {
		  defn_level++;
		  for (n = 0; n < no_of_minor_models; n++)
		     strcpy(current_model_defn[n][defn_level],directive_string);
		  variant_change = true;
		  }
	       else if (directive == variant)
		  {
		  for (n = 0; n < no_of_minor_models; n++)
		     strcpy(current_model_defn[n][defn_level],directive_string);
		  variant_change = true;
		  }
	       else if (directive == variant_end)
		  {
		  for (n = 0; n < no_of_minor_models; n++)
		     current_model_defn[n][defn_level][0] = '\0';
		  defn_level--;
		  variant_change = true;
                  }
	       else if (directive == variant_assembly_start)
		  {
		  defn_level++;
		  }
	       else if (directive == variant_assembly)
		  {
		  for (n = 0; n < no_of_minor_models; n++)
		     strcpy(current_model_defn[n][defn_level],directive_string);
		  variant_change = true;      
		  }
	       else if (directive == variant_assembly_end)
                  {
		  for (n = 0; n < no_of_minor_models; n++)
		     current_model_defn[n][defn_level][0] = '\0';
		  defn_level--;
                  }
	       else if (directive == variant_one_of_set)
		  {
	          defn_level++;
		  for (n = 0; n < no_of_minor_models; n++)
		     strcpy(current_model_defn[n][defn_level],"1 of n part");
		  variant_change = true;  /* not same as true variant ? */
		  }
	       else if (directive == end_variant_set)
                  {
		  for (n = 0; n < no_of_minor_models; n++)
		     current_model_defn[n][defn_level][0] = '\0';
		  defn_level--;   /* assuming this is end of definition */
		  }
	       }
	    else if (is_blank_line(string_in) == 0) /* Only update model list when it is      */
	       {                                    /* certain that the model will contain at */
	       if (variant_change == true)          /* least one part */
		  {
		  for (n = 0; n < no_of_minor_models; n++)
		     {
		     if (find_model(/* models,  */
 		   		    row,
		   		    /* current_model_defn, */
		   		    defn_level,
                                    n,
				    no_of_minor_models) == 0)
		        {
		        add_model(/* models, */
		      	          row,
		      		  /* current_model_defn, */
                                  defn_level,
				  n,
				  *model_ref);
                        (*model_ref)++;
			row++;
		        } 
		     }
		  variant_change = false;
		  }
	       }
	    }
      fclose(fptr_list);
      }
   return(1);
   }  /* get_models_covered() */

int find_model(/* struct model_reference models[], */
	       int models_so_far,
	       /* char current_model_defn[20][10][20], */
	       int defn_level,
               int current_model,
	       int no_of_minor_models)
   {
   enum boolean model_found = false;
   int i,j;

   /* Look to see if current model definition inconsistent */
   for (i = 2; i <= defn_level; i++)            /* Compare each part of model defn with all */
      for (j = 0; j < no_of_minor_models; j++) /* other minor_models as if one of these is */
	 {                                     /* included in defn. then defn. is invalid */
	 if (j == current_model)
	    ;                /* can ignore root minor model defn in own defn string */
	 else
	    if (strcmp(current_model_defn[current_model][i],current_model_defn[j][0]) == 0)
	       {
	       printf("Error: invalid model definition found\n");
	       return(2);    /* invalid model definition */
	       }
	 }
   /* Look to see if current model already defined */
   for (i = 0; i <= models_so_far; i++) /* could make more selective,  */
					   /* eg only look at years first */
      {                                 /* This search assumes defns may have  */
      for (j = 0; j <= defn_level; j++)  /* duplicate elements */
	 {
	 if (strcmp(models[i].model_defn[j],current_model_defn[current_model][j]) != 0)
	    break;
	 else if (j == defn_level)
	    if (models[i].model_defn[j + 1][0] == '\0')  /* to cover case of current model string  */
	       model_found = true;                       /* being shorter than earlier one */
	 }                                               /* NOTE: operation here depends on initialisation */
      if (model_found == true)
	 return(1);
      }
   return(0);  /* model not found */
   }  /* find_model() */

int add_model(/* struct model_reference models[], */
	      int models_so_far,
	      /* char current_model_defn[20][10][20], */
	      int defn_level,
	      int current_model,
	      int model_ref)
   {
   int i;

   for (i = 0; i <= defn_level; i++)
      strcpy(models[models_so_far].model_defn[i],current_model_defn[current_model][i]);
   models[models_so_far].ref_number = model_ref;
   return (1);
   } /* add_model() */
