			/* get_mods.c */
/* This program merges the contents of several parts list files into a */
/* single file and indicates from what source files each part entry    */
/* originates from.                                                    */

/*
GET_MODS.C

This program produces a list of part numbers along with what models they are
applicable to.

The input to this program is a definition file defining which parts lists are
to be analysed and a set of original parts lists. The file 'models.dat' as
generated by MODELS.C is used by this program to provide the model type
information.

The output of the program is a data file 'get_mods.dat' which contains records
of type 'part_model_ref'. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int load_model_reference(char models_file[],
		   /*	 struct model_reference models[], */
		         int *models_size);

int load_temp_file(int i,
	           char file_array[100][100]);

int get_model(char string_in[],
	      char first_year[],
	      char last_year[],
	      int *no_of_minor_models,
	      int *defn_level,
	      enum boolean *in_variant_assembly,
          enum boolean *in_variant_assembly_defn,
	      char current_model_defn[20][10][20]);

int compare_models(char current_model_defn[10][20],
		   int defn_level,
		   struct model_reference models[],
		   int no_of_models);

int save_temp_file(int i,
	           char file_array[200][100]);

char source_current_model_defn[50][10][20];
char source_file[100][100];
char searched_current_model_defn[50][10][20];
char searched_file[100][100];
struct part_model_ref results[100];
struct model_reference models[50];

void main(int argc,
	  char *argv[])
   {
//   FILE *fptr_in;
//   FILE *fptr_out;
   FILE *fptr_defn;
//   FILE *fptr_list;
   FILE *fptr_res;
//   FILE *fptr_mod_ref;

   enum boolean end_of_source_file;
   enum boolean source_in_variant_assembly;
   enum boolean source_in_variant_assy_defn;
   char source_first_year[10];
   char source_last_year[10];
   char source_part_number[10];
   int source_no_of_minor_models;
   int source_defn_level;
   int source_line;
   int list;

   enum boolean end_of_searched_file;
   enum boolean searched_in_variant_assembly;
   enum boolean searched_in_variant_assy_defn;
   char searched_first_year[10];
   char searched_last_year[10];
   int searched_no_of_minor_models;
   int searched_defn_level;
   int searched_line;
   char *searched_file_addr;
   char searched_char;

   char defn_file[20][50];			/* store definition file */
//   char current_section[50];			/* title of current section */
   char string_in[101];
//   char string_out[101];
//   char directive_string[81];
//   enum boolean in_section;
   enum directive directive = none;
//   enum boolean common_variant_assy[10];
//   enum boolean check_for_common_variant_assy[10];
   enum boolean model_found;
//   int i, j, k, l, m, n = 0;
   int i, j, k, m, n = 0;
   int size;
   int title_posn;
   int row, column;
//   int nr_in_section;
//   int quantity_posn;
   int models_size;
   int position;
   int model_ref;

   if ((fptr_defn = fopen(argv[1],"r")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }
   if ((fptr_res = fopen("get_mods.dat","wb")) == NULL)
      {
      printf("Cannot open file 'get_mods.dat' \n");
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
   for (i = 0; i < 50; i++)
      {
      models[i].ref_number = 0;
      models[i].major_model[0] = '\0';
      for (j = 0; j < 10; j++)
	 models[i].model_defn[j][0] = '\0';
      }
   load_model_reference("models.dat",
	      /*	models,  */
			&models_size);

/*for (i = 0; i < models_size; i++)
 *  {
 *  printf ("i = %d ",i);
 *  j = 0;
 *  while (j < 10)
 *  {
 *  printf ("%s ",models[i].model_defn[j]);
 *  j++;
 *  }
 *  printf("model ref = %d\n",models[i].ref_number);
 *  } */

   for (row = 1; row < 100; row++)	  /* initialise results array */
      {
      for (j = 0; j < 10; j++)
	 results[row].part_number[j] = ' ';
      results[row].part_number[0] = '\0';
      for (j = 0; j < 30; j++)
         {
         results[row].model[j] = 0;
	 }
      }
   if (create_temp_files(defn_file,
			 title_posn) != 1)
      {
      printf("Unable to generate duplicate files\n");
      }
   searched_file_addr = searched_file;
   row = 0;
   for (list = 1; list < title_posn; list++)   /* i.e. for each parts list */
      {
      /*load_file(defn_file,
       *	  list,
       *	  source_file); */
      load_temp_file(list,             /* Assumes standard file names */
     		     source_file);
      source_line = 0;
      end_of_source_file = false;
      source_no_of_minor_models = 0;
      source_defn_level = 0;
      source_in_variant_assembly = false;
      source_in_variant_assy_defn = false;
      while (end_of_source_file == false) /* Go through section to get part to */
	 {				  /* be searched for 		       */
	 if (source_file[source_line][0] == '*')     /* Represents lines of section	  */
	    {
	    get_model(source_file[source_line],
	  	      source_first_year,
	  	      source_last_year,
	  	      &source_no_of_minor_models,
		      &source_defn_level,
		      &source_in_variant_assembly,
                      &source_in_variant_assy_defn,
		      source_current_model_defn);
	/*    printf("minor models = %d, defn_level = %d\n",source_no_of_minor_models, source_defn_level);
	 *   for (i = 0; i < source_no_of_minor_models; i++)
         *      {
	 *      for (j = 0; j <= source_defn_level; j++)
	 *	  printf(" %s ",source_current_model_defn[j]);
         *      printf("\n");
	 *      } */
	    }
	 else if (source_file[source_line][0] == '$') /* Part already covered   */
            ;
	 else if (source_file[source_line][0] == '%')
	    {
	    end_of_source_file = true;
	    }
	 else   /* Part number definition therefore get model */
	    {
	    model_found = false;
	    column = 0;
	    position = 3;
	    get_part_number(source_file[source_line],
			    source_part_number,
			    true,
			    position);
	    strcpy(results[row].part_number, source_part_number);
	    for (i = 0; i < source_no_of_minor_models; i++)
	       {
	       if ((model_ref = compare_models(source_current_model_defn[i],
	    				       source_defn_level,
	    				       models,
	    				       models_size)) > 0)
	    	  {
	          results[row].model[column] = model_ref;
	    	  column++;
	    	  model_found = true;
	    	  }
	       }
	    if (model_found == false)
	       {
	       printf("Error: Model from source for part number %s not found\n",source_part_number);
	       }

	    /* Search through parts lists for found part */
	    for (k = list; k < title_posn; k++) /* k represents section */
	       {					 /* to be searched */
	       if (k == list)
		  {
                  /* Start search with current definitions */
	          searched_defn_level = source_defn_level;
		  for (i = 0; i < source_no_of_minor_models; i++)
		     {
		     for (j = 0; j <= source_defn_level; j++)
	                {
		        strcpy(searched_current_model_defn[i][j],
			       source_current_model_defn[i][j]);
			}
                     }
		  searched_no_of_minor_models = source_no_of_minor_models;
		  searched_in_variant_assembly = source_in_variant_assembly;
		  searched_in_variant_assy_defn = source_in_variant_assy_defn;
		  searched_line = source_line + 1;
		  searched_file_addr = source_file;
		  }
	       else
		  {
		  searched_file_addr = searched_file;
		  load_temp_file(k,
		  	         searched_file); 
		  searched_line = 0;
		  searched_no_of_minor_models = 0;
		  searched_defn_level = 0;
                  searched_in_variant_assembly = false;
		  searched_in_variant_assy_defn = false;
		  for (i = 0; i < 50; i++)
	             {
		     for (j = 0; j < 10; j++)
		        {
			strcpy(searched_current_model_defn[i][j],
			       "\0");
			}
                     }
		  }
	       end_of_searched_file = false;
	       while (end_of_searched_file == false)
		  {
		  searched_char = *(searched_file_addr + searched_line*100);
		  if (searched_char == '*')
		     {
		     get_model(searched_file_addr + searched_line*100,
			       /* searched_file[searched_line], */
			       searched_first_year,
			       searched_last_year,
			       &searched_no_of_minor_models,
			       &searched_defn_level,
			       &searched_in_variant_assembly,
                               &searched_in_variant_assy_defn,
			       searched_current_model_defn);
		     }
		  else if (searched_char == '%')
	             end_of_searched_file = true;
		  else
		     {
		     if (searched_char == '$')
		        ;  /* This part was found in previous iteration */
		     /* start at &3 as don't want to compare reference number */
		     else if (strncmp(&source_file[source_line][3],searched_file_addr + searched_line*100 + 3, 7) == 0)
		        {
	                model_found = false;
			for (i = 0; i < searched_no_of_minor_models; i++)
			   {
		           if ((model_ref = compare_models(searched_current_model_defn[i],
							    searched_defn_level,
							    models,
							    models_size)) > 0)
			      {
			      /* Look to see if model already found for this part */
			      for (j = 0; j < column; j++)
				 if (results[row].model[j] == model_ref)
				    break;
			      if (j == column) /* Relies on loop index being incremented */
                                 {
			         results[row].model[column] = model_ref;
				 column++;
                                 }
			      model_found = true;
			      }
			   }
			*(searched_file_addr + searched_line*100) = '$';
			if (model_found == false)
			   {
			   printf("Error: Model from searched for part number %s not found\n",source_part_number);
			   }
			}
	             /* part not found */
		     else
		        ;
		     }
		  searched_line++;
		  }
               if (k != list)
		  save_temp_file(k,
				 searched_file_addr);
	       }
	    row++;
	    }
	 source_line++;
	 }
      }
      fwrite(results,sizeof(results), 1, fptr_res);

      for (m = 0; m < row; m++)
	 {
	 printf("%s ",results[m].part_number);
	 n = 0;
	 while (results[m].model[n] > 0)
	    {
	    printf("%d ",results[m].model[n]);
	    n++;
	    }
	 printf("\n");
	 }
   _fcloseall();
   }  /* main() */

int get_model(char string_in[],
	      char first_year[],
	      char last_year[],
	      int *no_of_minor_models,
	      int *defn_level,
	      enum boolean *in_variant_assembly,
              enum boolean *in_variant_assembly_defn,
	      char current_model_defn[20][10][20])
   {
   enum directive directive;
   enum boolean year_read, model_read;
   enum boolean variant_change;
//   int i,j,k,l,m,n,p;
   int k,l,m,n;
//   int row;
   char directive_string[81];
   char year_string[10];
   char model_string[20];

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
	       strcpy(current_model_defn[*no_of_minor_models][0],first_year);
	       strcpy(current_model_defn[*no_of_minor_models][1],last_year);
	       strcpy(current_model_defn[*no_of_minor_models][2],model_string);
	       l = 0;
	       *no_of_minor_models = *no_of_minor_models + 1;    /* next minor model needs new row */
	       }
	    else if (directive_string[k] == '\0')
	       {
	       model_string[l] = '\0';
	       strcpy(current_model_defn[*no_of_minor_models][0],first_year);
	       strcpy(current_model_defn[*no_of_minor_models][1],last_year);
	       strcpy(current_model_defn[*no_of_minor_models][2],model_string);
	       *no_of_minor_models = *no_of_minor_models + 1;
	       *defn_level = 2;
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
	 *defn_level = *defn_level + 1;
	 for (n = 0; n < *no_of_minor_models; n++)
	    strcpy(current_model_defn[n][*defn_level],directive_string);
	 variant_change = true;
	 }
      else if (directive == variant)
         {
	 for (n = 0; n < *no_of_minor_models; n++)
	    strcpy(current_model_defn[n][*defn_level],directive_string);
	 variant_change = true;
	 }
      else if (directive == variant_end)
         {
	 for (n = 0; n < *no_of_minor_models; n++)
	    current_model_defn[n][*defn_level][0] = '\0';
	 *defn_level = *defn_level - 1;
	 variant_change = true;
         }
      else if (directive == variant_assembly_start)
         {
	 *defn_level = *defn_level + 1;
	 *in_variant_assembly_defn = true;  /* check exactly what ends defn */
         }                                  /* ie common_assembly, in_assembly, or either */
      else if (directive == variant_assembly)
         {
	 for (n = 0; n < *no_of_minor_models; n++)
	    strcpy(current_model_defn[n][*defn_level],directive_string);
	 variant_change = true;
         }
      else if (directive == in_assembly)
	 {
	 if (*in_variant_assembly_defn == true)
	    *in_variant_assembly_defn = false;
	 else if (*in_variant_assembly == false)
	    *defn_level = *defn_level + 1;
	 /* else if (*in_variant_assembly == true)
	  *    ;    */
	 for (n = 0; n < *no_of_minor_models; n++)
	    strcpy(current_model_defn[n][*defn_level],directive_string);
	 variant_change = true;
	 *in_variant_assembly = true;
	 }
      else if (directive == common_assembly)
	 {
	 if (*in_variant_assembly_defn == true ||
	     *in_variant_assembly == true)
	    {
	    for (n = 0; n < *no_of_minor_models; n++)
	       current_model_defn[n][*defn_level][0] = '\0';
	    *defn_level = *defn_level - 1;
	    *in_variant_assembly = false;      
	    *in_variant_assembly_defn = false;
            }
	 }
      else if (directive == variant_assembly_end)
         {
	 if (*in_variant_assembly == true)
            {
	    for (n = 0; n < *no_of_minor_models; n++)
	       current_model_defn[n][*defn_level][0] = '\0';
	    *defn_level = *defn_level - 1;
	    *in_variant_assembly = false;      
            }
	 }
      else if (directive == variant_one_of_set)
         {
	 *defn_level = *defn_level + 1;
	 for (n = 0; n < *no_of_minor_models; n++)
	    strcpy(current_model_defn[n][*defn_level],"1 of n part");
	 variant_change = true;  /* not same as true variant ? */
	 }
      else if (directive == end_variant_set)
         {
	 for (n = 0; n < *no_of_minor_models; n++)
	    current_model_defn[n][*defn_level][0] = '\0';
	 *defn_level = *defn_level - 1;   /* assuming this is end of definition */
	 }
      }
   return(1);
   }  /* get_model() */

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

int compare_models(char current_model_defn[10][20],
		   int defn_level,
		   struct model_reference models[],
		   int no_of_models)
   {
   int i,j;
   enum boolean model_found = false;

   for ( i = 0; i < no_of_models; i++)
      {
      for (j = 0; j <= defn_level; j++)
	 {
	 if (strcmp(models[i].model_defn[j],current_model_defn[j]) != 0)
	    break;
	 else if (j == defn_level)
	    if (models[i].model_defn[j + 1][0] == '\0')
	       model_found = true;
	 }
      if (model_found == true)
	 return(models[i].ref_number);
      }
   return(0);
   }  /* compare_models()  */

int load_file(char defn_file[20][50],
	      int file,
	      char file_array[200][100])
   {
   FILE *fptr_list;
   char string_in[100];
   int line;

   if ((fptr_list = fopen(defn_file[file],"r")) == NULL)
      {
      printf("Cannot open file %s\n",defn_file[file]);
      return(0);
      }
   else
      {
      line = 0;
      while (fgets(string_in, 100, fptr_list) != NULL &&
	     line < 99)    /******** temporary addition *********/
	 {
	 if (is_blank_line(string_in) == 0)
            {
	    strcpy(file_array[line],string_in);
	    line++;
            }
	 }
      file_array[line][0] = '%';
      }
   fclose(fptr_list);
   return(1);
   }  /* load_file() */

int load_temp_file(int i,
	           char file_array[100][100])
   {
   FILE *fptr_file;
   char file_name[15];
   char string_in[100];
   int line;

   strcpy(file_name,"TEMP00.LST");
   make_temp_file_name(file_name,
                       4,
		       i);
   if ((fptr_file = fopen(file_name,"r")) == NULL)
      {
      printf("Cannot open file %s\n",file_name);
      exit(0);
      }
   else
      {
      line = 0;
      while (fgets(string_in, 100, fptr_file) != NULL &&
	     line < 99)    /******** temporary addition *********/
	 {
	 if (is_blank_line(string_in) == 0)
            {
	    strcpy(file_array[line],string_in);
	    line++;
            }
	 }
      file_array[line][0] = '%';
      }
   fclose(fptr_file);
   return(1);
   }  /* load_temp_file() */

int save_temp_file(int i,
	           char file_array[200][100])
   {
   FILE *fptr_file;
   char file_name[15];
   int line;

   strcpy(file_name,"TEMP00.LST");
   make_temp_file_name(file_name,
                       4,
		       i);
   if ((fptr_file = fopen(file_name,"w")) == NULL)
      {
      printf("Cannot open file %s\n",file_name);
      exit(0);
      }
   else
      {
      line = 0;
      while (file_array[line][0] != '%')
	 {
	 fputs(file_array[line], fptr_file);
	 line++;
         }
      }
   fclose(fptr_file);
   return(1);
   }  /* save_temp_file() */