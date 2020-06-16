			  /* mrge_all.c */
/* This program merges the contents of several parts list files into a */
/* single file and ........  */

/*                       Data structures
 *
 * Definition file :-          Section array :-
 *
 *  +-------------------+   +-------------+ +-------------+ +-----
 *  |*definition_file   |   |  title_n    | |   title_n   | |
 *  |file_1             |   |data from 1st| |data from 2nd| | etc....
 *  |file_2             |   |   file      | |    file     | |
 *  | ...               |   +-------------+ +-------------+ +-----
 *  |*titles            |   The program will loop once for each title
 *  |title_1            |   and the above sections will be overwritten
 *  |title_2            |   each time, i.e. there will only be one section
 *  | ...               |   from each file stored in the array at one time.
 *  +-------------------+
 *
 *  results[] array of structures :-
 *
 *  |      .....     |  |
 *  +----------------+  +---------------------------
 *   part no. & desc.     array of ints for marking year
 *  +----------------+  +--------------------------- - -
 *  | line from file |  | 1  | 1  | 1  | 0  | 0  | 0  |
 *  +----------------+  +--------------------------- - -
 *  +----------------+  +---------------------------
 *  | line from file |  | 1  | 1  | 0  | 0  | 0  | 0  |
 *  +----------------+  +---------------------------
 *  +----------------+  +---------------------------
 *  | line from file |  |   |   |   |   |   |   |
 *  +----------------+  +---------------------------
 *        ........
 *   */

/*
MRGE_ALL.C

The purpose of this program is to take several parts lists and to produce a
single combined parts list. The combined parts list follows the sectional
structure of the originals, but divides the section into year groupings
indicating the applicability of parts.

The input to this program is a definition file defining which parts lists are
to be merged and a set of original parts lists.

The output is a set of data files, where each output file corresponds to one
of the input parts lists, each containing records of type 'part_years'. Each
of these files follows the sectional structure of a parts list and defines
what parts from a particular year are common to any earlier years.

The names of the output files are MRGE00.RES to MRGEnn.RES; MRGE00.RES
containing special information relating to the range of years covered and
MRGE01.RES to MRGEnn.RES containing parts data.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int get_years_covered(struct part_years *results,
		      char defn_file[20][50],
		      int title_posn);
int get_models_covered(FILE *fptr_res,
		       char defn_file[20][50],
		       int title_posn);
int get_var_assy_desc(char string_in[],
		      struct variant_assy_ind variant_assy[],
		      int variant_assy_number);
int update_assembly_status(char string_in[],
			   int *assembly_level,
			   struct assembly_defn assembly[],
			   char assembly_part_list[5][10]);
int calculate_combinations(struct part_years *results);
int find_model(struct model_reference models[],
	       int models_so_far,
	       char current_model_defn[20][10][10],
	       int defn_level,
	       int current_model,
	       int no_of_minor_models);
int add_model(struct model_reference models[],
	      int models_so_far,
	      char current_model_defn[20][10][10],
	      int defn_level,
	      int current_model);

int bit_for_year[] = {  1,  2,   4,   8,  16,  32,   64,  128,
		      256,512,1024,2048,4096,8192,16384,32768};

int determine_directive_state(char section[max_lines][100],
			      int line,
			      char variant_quantity_label[],
			      char variant_labels[5][10],
			      struct variant_assy_ind variant_assy[],
			      int *variant_assy_number,
			      int *variant_assy_level,
			      enum boolean *in_variant_assy_defn,
			      enum boolean *in_variant_assy_body,
			      /* enum boolean *common_assy, */
			      int *variant_level,
			      enum boolean *in_variant_quantity,
			      enum boolean *in_one_of_set,
                              enum boolean *get_variant_assembly_desc,
			      enum directive *directive_out,
			      char directive_string[]);

int compare_source_and_searched_state(int source_assembly_level,
				       int searched_assembly_level,
				       char source_assy_part_list[5][10],
				       char searched_assy_part_list[5][10],
				       int source_variant_assy_number,
				       int searched_variant_assy_no,
				       int source_variant_assy_level,
				       int searched_variant_assy_level,
				       enum boolean source_in_variant_assy_defn,
				       enum boolean searched_in_variant_assy_defn,
				       enum boolean source_in_variant_assy_body,
				       enum boolean searched_in_variant_assy_body,
				       enum boolean common_variant_assy,
				       struct variant_assy_ind source_variant_assy[],
				       struct variant_assy_ind searched_variant_assy[]);

int handle_searched_directive(enum directive directive,
			      enum boolean *common_variant_assy,
			      enum boolean *check_for_common_variant_assy,
			      enum boolean source_in_variant_assy_body,
			      int *source_variant_assy_number,
			      int *searched_variant_assy_no,
			      struct variant_assy_ind source_variant_assy[],
			      struct variant_assy_ind searched_variant_assy[],
			      enum boolean *end_of_section_searched);

char source_section[max_lines][100];    /* stores input from files */
char searched_section[max_lines][100];  /* stores input from files */
struct assembly_defn searched_assembly[5];
struct assembly_defn source_assembly[5];
/*struct model_reference models[20]; */ /* stores strings defining model and reference number */

void main(int argc,
	  char *argv[])
   {
//   FILE *fptr_in;
//   FILE *fptr_out;
   FILE *fptr_defn;
   FILE *fptr_file;
   FILE *fptr_res;

   char source_part_number[20];
   char source_variant_labels[5][20];
   char source_variant_quantity_label[80];
   char source_assy_part_list[5][20];
//   char searched_current_part_no[20];
   char searched_variant_labels[5][20];
   char searched_variant_quantity_label[80];
   char searched_assy_part_list[5][20];

   int source_assembly_level;
   int source_variant_level;
   int source_variant_assy_number = 0;
   int source_variant_assy_level = 0;
   enum boolean source_in_variant_assy_defn;
   enum boolean source_in_variant_assy_body;
   struct variant_assy_ind source_variant_assy[5];
   enum boolean source_in_variant_quantity;
   enum boolean source_in_one_of_set;
   enum boolean end_of_source_section;
   enum boolean get_source_var_assy_desc;

   int searched_assembly_level;
   int searched_variant_level;
   int searched_variant_assy_level;
   int searched_variant_assy_no;
   struct variant_assy_ind searched_variant_assy[5];
   enum boolean searched_in_variant_assy_defn;
   enum boolean searched_in_variant_assy_body;
   enum boolean searched_in_variant_quantity;
   enum boolean searched_in_one_of_set;
//   enum boolean end_of_searched_section;
   enum boolean get_searched_var_assy_desc;

   char defn_file[20][50];                      /* store definition file */
   struct part_years results;
//   struct part_years_output results_output;
   char current_section[50];                    /* title of current section */
   char string_in[101];
//   char string_out[101];
//   char description[100];
   char searched_description[100];
   char directive_string[81];
   char file_name[10];
//   enum boolean in_section;
   enum directive directive = none;
   enum boolean common_variant_assy;
   enum boolean check_for_common_variant_assy;
   enum boolean end_of_source_file;
   enum boolean got_source_section;
   enum boolean got_searched_section;
   enum boolean all_sections = false;
   enum boolean end_of_section_searched;
   int i, j, k, l = 0;
   int line;
   int size;
   int title_posn;
   int out;
   int nr_in_section;
//   int quantity_posn;
   int position;
   int source_section_no;
   int source_sect;
   int searched_section_no;
   int source_file;
   int searched_file;
   fpos_t start_of_line_last_read;
   fpos_t end_of_line_last_read;

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
   if (create_temp_files(defn_file,
			 title_posn) != 1)
      {
      printf("Unable to generate duplicate files\n");
      }

   /* This results file contains year and model information
    * relating to the following files
    */
   if ((fptr_res = fopen("MRGE00.RES","wb")) == NULL)
      {
      printf("Cannot open file %s\n","MRGE00.RES");
      exit(0);
      }
   /* First structure written to disk contains years covered */
   results.description[0] = '\0';
   results.part_number[0] = '\0';
   for (j = 0; j < 20; j++)
      for (k = 0; k < 6; k++)
    	 results.year[j][k] = 0;
   get_years_covered(&results,
    		     defn_file,
    		     title_posn);
   fwrite(&results,sizeof(results), 1, fptr_res);

   /* Get models covered */
   get_models_covered(fptr_res,
                      defn_file,
                      title_posn);
   fclose(fptr_res);

   for (source_file = 1; source_file < title_posn; source_file++)   /* i.e. for each parts list (i.e. file) */
      {
      /* Open file */
      strcpy(file_name,"MRGE00.RES");
      make_temp_file_name(file_name,
			  4,
			  source_file);
      if ((fptr_res = fopen(file_name,"wb")) == NULL)
         {
	 printf("Cannot open file %s\n",file_name);
         exit(0);
         }
      end_of_source_file = false;
      source_section_no = 0;
      source_sect = 0;
      while (end_of_source_file == false)
	 {
	 /* Get next source section */
	 if (all_sections == true)
	    {
	    strcpy(current_section,"next");
	    if (get_section(source_section,  /* need to modify for current_section */
			    source_file,
			    source_section_no,
			    current_section) == 0)
	       {
	       end_of_source_file = true;
	       got_source_section = false;
	       }
	    else
	       got_source_section = true;
	    /* source_section_no++; */
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
	    /* source_section_no++; */
	    source_sect++;
	    if (source_sect == size - title_posn - 1)
		end_of_source_file = true;
	    }
	 if (got_source_section == true)
	    {
	    /* First structure of section contains section title */
	    for (j = 0; j < 20; j++)
	       for (k = 0; k < 6; k++)
		  results.year[j][k] = -1;  /* All -1's indicate section title */
            results.part_number[0] = '\0';
	    strcpy(results.description,current_section);
	    fwrite(&results,sizeof(results), 1, fptr_res);
	    /* Intialise data structures for new section */
	    for (l = 0; l < 5; l++)
	       searched_variant_assy[l].label_displayed = false;
	    l = 0;
	    source_assembly_level = 0;
	    source_variant_level = 0;
	    source_in_variant_assy_defn = false;
	    source_in_variant_assy_body = false;
	    source_in_variant_quantity = false;
	    source_in_one_of_set = false;
            get_source_var_assy_desc = false;
	    end_of_source_section = false;
	    while (end_of_source_section == false)   /* Go through section to get part to */
	       {                                     /* be searched for                   */
	       if (source_section[l][0] == '*')      /* l represents lines of section     */
		  {
		  determine_directive_state(source_section,
					    l,
					    source_variant_quantity_label,
					    source_variant_labels,
					    source_variant_assy,
					    &source_variant_assy_number,
					    &source_variant_assy_level,
					    &source_in_variant_assy_defn,
					    &source_in_variant_assy_body,
					    /* &source_common_assy, */
					    &source_variant_level,
					    &source_in_variant_quantity,
					    &source_in_one_of_set,
	       	                            &get_source_var_assy_desc,
					    &directive,
					    directive_string);
		  }
	       else if (source_section[l][0] == '$') /* Part already covered   */
		  ;
	       else if (source_section[l][0] == '%')
		  {
		  end_of_source_section = true;
		  }
	       else     /* Look through other files for part */
		  {
		  /*                     */
		  if (get_source_var_assy_desc == true)
		     {
		     get_var_assy_desc(source_section[l],
				       source_variant_assy,
                                       source_variant_assy_number);
		     get_source_var_assy_desc = false;
		     }

		  /* initialise new results structure */
		  for (j = 0; j < 80; j++)
		     results.description[j] = ' ';
		  results.description[0] = '\0';
		  results.part_number[0] = '\0';
		  for (j = 0; j < 20; j++)
		     for (k = 0; k < 6; k++)
			results.year[j][k] = 0;
		  results.combination = 0;

		  results.year[source_file - 1][0] = 1; /* indicate where part found */
		  /* Get source part data */
		  position = 3;
		  get_part_number(source_section[l],     /* Need to check returned value */
				  source_part_number,    /* eg 0 for blank part number */
				  true,
				  position);
		  strcpy(results.part_number, source_part_number);
		  position = 11;
		  get_description(source_section[l],
				  results.description,
				  75,
				  position);
		  /* Search through parts lists for found part */
		  for (searched_file = source_file + 1; searched_file < title_posn; searched_file++)
		     {
		     searched_section_no = 0;

		     if ((line = get_section_line(searched_file,
				                  searched_section_no,
						  current_section)) == 0)
		        got_searched_section = false;
		     else
			got_searched_section = true;

		     if (got_searched_section == true)
		        {
			/* Open file */
			strcpy(file_name,"TEMP00.LST");
			make_temp_file_name(file_name,
					    4,
			  		    searched_file);
			if ((fptr_file = fopen(file_name,"r+")) == NULL)
			   {
			   printf("1 Cannot open file %s\n",file_name);
                           exit(0);
                           }
		        /* Move file pointer to start of section */
                        i = 0;
                        while (i < line)
                           {
			   if (fgets(string_in, 100, fptr_file) == NULL)
	                      {
	                      printf("Error: Section not reached\n");
	                      exit(0);
	                      }
                           i++;
                           }
			end_of_section_searched = false;
			nr_in_section = 0;
			searched_assembly_level = 0;
			searched_variant_level = 0;
			searched_in_variant_assy_defn = false;
			searched_in_variant_assy_body = false;
			searched_in_variant_quantity = false;
			searched_in_one_of_set = false;
                        fgetpos(fptr_file, &start_of_line_last_read);
                        while (end_of_section_searched == false &&
	                       fgets(string_in, 100, fptr_file) != NULL)
			   {
			   fgetpos(fptr_file, &end_of_line_last_read);
			   if (is_blank_line(string_in) == 0)
			      {
			      if (string_in[0] == '*')
			         {
			         determine_directive_state(string_in,   /* Really an array parameter */
							   0,           /* Array index */
							   searched_variant_quantity_label,
							   searched_variant_labels,
							   searched_variant_assy,
							   &searched_variant_assy_no,
							   &searched_variant_assy_level,
							   &searched_in_variant_assy_defn,
							   &searched_in_variant_assy_body,
							   /* &searched_common_assy, */
							   &searched_variant_level,
							   &searched_in_variant_quantity,
							   &searched_in_one_of_set,
		                                           &get_searched_var_assy_desc,
							   &directive,
							   directive_string);

				 handle_searched_directive(directive,
							   &common_variant_assy,
							   &check_for_common_variant_assy,
							   source_in_variant_assy_body,
							   &source_variant_assy_number,
							   &searched_variant_assy_no,
						           source_variant_assy,
							   searched_variant_assy,
			                                   &end_of_section_searched);
			         }
			      else if (string_in[0] == '$')
			         ;  /* This part was found in previous iteration */
				    /* may want to highlight this                */
			      /* start at &3 as don't want to compare reference number */
			      else if (strncmp(&source_section[l][3],&string_in[3],7) == 0)
			         {
	                         position = 11;
		                 get_description(string_in,
				                 searched_description,
				                 75,
				                 position);
	                         if (strcmp(results.description,searched_description) == 0)
                                    {
		                    if (get_searched_var_assy_desc == true)
		                       {
		                       get_var_assy_desc(string_in,
				                         searched_variant_assy,
		                                         searched_variant_assy_no);
		                       get_searched_var_assy_desc = false;
                                       }
			            results.year[searched_file - 1][nr_in_section] =
				       compare_source_and_searched_state(source_assembly_level,
								         searched_assembly_level,
								         source_assy_part_list,
								         searched_assy_part_list,
								         source_variant_assy_number,
								         searched_variant_assy_no,
								         source_variant_assy_level,
								         searched_variant_assy_level,
								         source_in_variant_assy_defn,
								         searched_in_variant_assy_defn,
								         source_in_variant_assy_body,
								         searched_in_variant_assy_body,
								         common_variant_assy,
								         source_variant_assy,
								         searched_variant_assy);
				    nr_in_section++;
                                    /* Mark part as found */
	                            string_in[0] = '$';
	                            if (fsetpos(fptr_file, &start_of_line_last_read) != 0)
	                               printf("Error: fsetpos() error\n");
	                            if (fputs(string_in,fptr_file) == EOF)
                                       {
	                               printf("Marked string not written\n");
	                               printf("%s\n",string_in);
                                       }
	                            fsetpos(fptr_file, &end_of_line_last_read);
			            }
                                 }
			      /* part not found */
			      else
			         ;
			      update_assembly_status(string_in,
						     &searched_assembly_level,
						     searched_assembly,
						     searched_assy_part_list);
			      }
                           if (fgetpos(fptr_file, &start_of_line_last_read) != 0)
			      printf("Position of line in file not found\n");
			   }
                        fclose(fptr_file);
			}
		     }
		  calculate_combinations(&results);
		  fwrite(&results,sizeof(results), 1, fptr_res);
		  }
	       update_assembly_status(source_section[l],
				      &source_assembly_level,
				      source_assembly,
				      source_assy_part_list);
	       l++;
	       }
	    }
	 source_section_no++;
	 }
      fclose(fptr_res);
      }
   printf("searched all sections\n");
   out = fcloseall();
   }  /* main() */

int get_years_covered(struct part_years *results,
		      char defn_file[20][50],
		      int title_posn)
   {
   FILE *fptr_list;
   enum directive directive;
   enum boolean year_read;
   enum boolean first_year;
   int j,k,l,m;
   char directive_string[50];
   char year_string[10];
   char string_in[101];

   printf("Determining which years are covered\n");
   m = 0;  /* m represents year covered as some parts lists cover multiple years  */
           /* m is limited to 20 years */
           /* Note that two rows of years are filled; the first row containing the
            * complete number of years covered by the parts lists; the second
            * contains only the first year covered by a list.
            */
   for (j = 1; j < title_posn; j++)      /* Go through each file looking for */
      {                                  /* year(s) covered */
      if ((fptr_list = fopen(defn_file[j],"r")) == NULL)
	 {
	 printf("Cannot open file %s\n",defn_file[j]);
	 exit(0);
	 }
      else
	 {
	 /*printf("Opened file %s\n",defn_file[j]);*/
	 /* Go through file line by line */
	 year_read = false;
	 while ((fgets(string_in, 100, fptr_list) != NULL) &&
		 year_read == false)
	    {
	    if (is_blank_line(string_in) == 0 &&
		string_in[0] == '*')
	       {
	       directive = check_directive(string_in, directive_string);
	       if (directive == year)
		  {
                  first_year = true;
		  k = 0;
		  l = 0;
		  do {
		     if (directive_string[k] == ',')
			{
			year_string[l] = '\0';
			/* results->year[j - 1][m] = atoi(year_string); */
	/* results->year[0][m] = atoi(year_string);
         * if (first_year == true)
	 *    results->year[1][j - 1] = atoi(year_string); */
			results->year[m][0] = atoi(year_string);
                        if (first_year == true)
			   results->year[j - 1][1] = atoi(year_string);
                        first_year = false;
			l = 0;
			m++;
			}
		     else if (directive_string[k] == '\0' ||
                              directive_string[k] == '\n')
			{
			year_string[l] = '\0';
	  /* results->year[0][m] = atoi(year_string);
           * if (first_year == true)
	   *   results->year[1][j - 1] = atoi(year_string);
	   * results->year[2][j - 1] = atoi(year_string); */
			results->year[m][0] = atoi(year_string);
                        if (first_year == true)
			   results->year[j - 1][1] = atoi(year_string);
			results->year[j - 1][2] = atoi(year_string);
			year_read = true;
                        m++;
			}
		     else
                        {
			year_string[l] = directive_string[k];
		        l++;
                        }
		     k++;
		     } while (year_read == false);
		  }
	       }
	    }
         fclose(fptr_list);
	 }
      }
   return(1);
   }  /* get_years_covered() */

int get_models_covered(FILE *fptr_res,
		       char defn_file[20][50],
		       int title_posn)
   {
   FILE *fptr_list;
   struct part_years results;
   enum directive directive;
   enum boolean /*year_read,*/ model_read;
//   enum boolean variant_change;
   int j,k,l;
//   int defn_level;
   char directive_string[50];
   char string_in[101];

   printf("Determining which models are covered\n");
   for (j = 1; j < title_posn; j++)      /* Go through each file */
      {                                
      if ((fptr_list = fopen(defn_file[j],"r")) == NULL)
	 {
	 printf("Cannot open file %s\n",defn_file[j]);
	 exit(0);
	 }
      else
	 {
	 /* printf("Opened file %s\n",defn_file[j]); */
         results.description[0] = '\0';
         results.part_number[0] = '\0';
         for (k = 0; k < 20; k++)
            for (l = 0; l < 6; l++)
    	       results.year[k][l] = 0;
	 /* Go through file line by line */
         model_read = false;
	 while (fgets(string_in, 100, fptr_list) != NULL &&
                model_read == false)
	    {
	    if (is_blank_line(string_in) == 0 &&
		string_in[0] == '*')
	       {
	       directive = check_directive(string_in, directive_string);
	       if (directive == year)
	          ;
	       else if (directive == major_model)
		  ;
	       else if (directive == minor_model)
		  {
		  strcpy(results.description,directive_string);
                  fwrite(&results,sizeof(results), 1, fptr_res);
                  model_read = true;
		  }
	       }
	    }
         fclose(fptr_list);
	 }
      }
   return(1);
   }  /* get_models_covered() */

int get_models_covered_old(struct model_reference models[],
		           char defn_file[20][50],
		           int title_posn)
   {
   FILE *fptr_list;
   enum directive directive;
   enum boolean year_read, model_read;
   enum boolean variant_change;
   int j,k,l,m,n;
   int row;
   int no_of_minor_models;
   int defn_level;
   char directive_string[50];
   char year_string[10];
   char model_string[20];
   char string_in[101];
   char first_year[5],last_year[5];
   char current_model_defn[20][10][10]; /* 20 models, each with 10 entries of 10 characters */

   printf("Determining which models are covered\n");
   row = 0;
   for (j = 1; j < title_posn; j++)      /* Go through each file */
      {                                
      if ((fptr_list = fopen(defn_file[j],"r")) == NULL)
	 {
	 printf("Cannot open file %s\n",defn_file[j]);
	 exit(0);
	 }
      else
	 {
	 /*printf("Opened file %s\n",defn_file[j]);*/
	 /* Go through file line by line */
	 while (fgets(string_in, 100, fptr_list) != NULL)
	    {
	    if (is_blank_line(string_in) == 0 &&
		string_in[0] == '*')
	       {
	       directive = check_directive(string_in, directive_string);
	       if (directive == year)
		  {
		  k = 0;         /* k is character in directive string */
		  l = 0;         /* l represents character in string */
		  m = 0;         /* m represents year covered as some */
				 /* parts lists cover multiple years  */
				 /* j only represents first year covered by a parts list */
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
			   /* first_year = atoi(year_string); */
			   strcpy(first_year,year_string);
			else if (m > 0)
			   /* last_year = atoi(year_string); */
			   strcpy(last_year,year_string);
			year_read = true;
			}
		     else
			year_string[l] = directive_string[k];
		     l++;
		     k++;
		     } while (year_read == false);
		  }
	       else if (directive == major_model)
		  strcpy(models[row].major_model,directive_string);
	       else if (directive == minor_model)
		  {
		  k = 0;
		  l = 0;
		  m = 0;   /* m represents current model from possible list */
			   /* Each minor model forms root of model definition tree */
		  model_read = false;
		  do {
		     if (directive_string[k] == ',')
			{
			model_string[l] = '\0';
			strcpy(current_model_defn[m][0],first_year);
			strcpy(current_model_defn[m][1],last_year);
			strcpy(current_model_defn[m][2],model_string);
			l = 0; 
			m++;       /* next minor model needs new row */
			}
		     else if (directive_string[k] == '\0')
			{
			model_string[l] = '\0';
			strcpy(current_model_defn[m][0],first_year);
			strcpy(current_model_defn[m][1],last_year);
			strcpy(current_model_defn[m][2],model_string);
			no_of_minor_models = m + 1;
			defn_level = 3;   /* next free slot */
			model_read = true;
			}
		     else
			model_string[l] = directive_string[k];
		     l++;
		     k++;
		     } while (model_read == false);
		  }
	       else if (directive == variant_start)    /* variants may be embedded */
		  {
		  for (n = 0; n < no_of_minor_models; n++)
		     strcpy(current_model_defn[n][defn_level],directive_string);
		  defn_level++;
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
		  defn_level--;
	       else if (directive == variant_one_of_set)
		  {
		  for (n = 0; n < no_of_minor_models; n++)
		     strcpy(current_model_defn[n][defn_level],"1 of n part");
		  defn_level++;
		  variant_change = true;  /* not same as true variant ? */
		  }
	       else if (directive == end_variant_set)
		  defn_level--;   /* assuming this is end of definition */
	       }
	    else if (is_blank_line(string_in) == 0) /* Only update model list when it is      */
	       {                                    /* certain that the model will contain at */
	       if (variant_change == true)          /* least one part */
		  {
		  for (n = 0; n < no_of_minor_models; n++)
		     {
		     if (find_model(models,
				    row,
				    current_model_defn,
				    defn_level,
				    n,
				    no_of_minor_models) == 0)
			{
			add_model(models,
				  row,
				  current_model_defn,
				  defn_level,
				  n);
			row++;
			}
		     }
		  variant_change = false;
		  }
	       }
	    row++;
	    }
         fclose(fptr_list);
	 }
      }
   return(1);
   }  /* get_models_covered_old() */

int find_model(struct model_reference models[],
	       int models_so_far,
	       char current_model_defn[20][10][10],
	       int defn_level,
	       int current_model,
	       int no_of_minor_models)
   {
   enum boolean model_found = false;
   int i,j;

   /* Look to see if current model definition inconsistent */
   for (i = 0; i < defn_level; i++)            /* Compare each part of model defn with all */
      for (j = 0; j < no_of_minor_models; j++) /* other minor_models as if one of these is */
	 {                                     /* included in defn. then defn. is invalid */
	 if (j == current_model)
	    ;                /* can ignore root minor model defn in own defn string */
	 else
	    if (strcmp(current_model_defn[current_model][i],current_model_defn[j][0]) == 0)
	       return(2);    /* invalid model definition */
	 }
   /* Look to see if current model already defined */
   for (i = 0; i < models_so_far; i++)  /* could make more selective,  */
					/* eg only look at years first */
      {                                 /* This search assumes defns may have  */
      for (j = 0; j < defn_level; j++)  /* duplicate elements */
	 {
	 if (strcmp(models[i].model_defn[j],current_model_defn[current_model][j]) != 0)
	    break;
	 else if (j == defn_level - 1)
	    model_found = true;
	 }
      if (model_found == true)
	 return(1);
      }
   return(0);  /* model not found */
   }  /* find_model() */

int add_model(struct model_reference models[],
	      int models_so_far,
	      char current_model_defn[20][10][10],
	      int defn_level,
	      int current_model)
   {
   int i;

   for (i = 0; i < defn_level; i++)
      strcpy(models[models_so_far].model_defn[i],current_model_defn[current_model][i]);
   return (1);
   } /* add_model() */

int determine_assembly_status(char string_in[],
			      int *assembly_level,
			      struct assembly_defn assembly_data[],
			      char assembly_part_list[5][10])
   {
   int i;
   int part_number_posn = 3;
   int assy_level;

   assy_level = *assembly_level;
   i = 0;
   while (string_in[i] != '\n' &&
	  string_in[i] != '\0')
      {
      if (string_in[i] == '#')
	 {
	 if (check_assembly(string_in,
			    &assy_level,
			    &i,
			    assembly_data) == 1)
	    {
	    get_part_number(string_in,
			    assembly_part_list[assy_level],
			    true,
			    part_number_posn);
	    }
	 else
	    printf("Error in assembly definition\n");
	 break;
	 }
      i++;
      }
   *assembly_level = assy_level;
   return(1);
   }  /* determine_assembly_status() */

int compare_source_and_searched_state(int source_assembly_level,
				       int searched_assembly_level,
				       char source_assy_part_list[5][10],
				       char searched_assy_part_list[5][10],
				       int source_variant_assy_number,
				       int searched_variant_assy_no,
				       int source_variant_assy_level,
				       int searched_variant_assy_level,
				       enum boolean source_in_variant_assy_defn,
				       enum boolean searched_in_variant_assy_defn,
				       enum boolean source_in_variant_assy_body,
				       enum boolean searched_in_variant_assy_body,
				       enum boolean common_variant_assy,
				       struct variant_assy_ind source_variant_assy[],
				       struct variant_assy_ind searched_variant_assy[])
   {
   int result = 99;

   if (source_assembly_level == 0 &&
       searched_assembly_level == 0 &&
       source_variant_assy_number == 0 &&
       searched_variant_assy_no == 0)
      result = 1;

   /*-----------------------------------------------------
    * Check for same parent assembly part number
    *----------------------------------------------------*/
   else if (source_assembly_level == 0 &&
	    searched_assembly_level > 0)
      result = 2;
   else if (source_assembly_level > 0 &&
	    searched_assembly_level == 0)
      result = 3;
   else if (source_assembly_level > 0 &&  /* levels do not have to be the same */
	    searched_assembly_level > 0)
      {
      if (strcmp(source_assy_part_list[source_assembly_level],
		 searched_assy_part_list[searched_assembly_level]) != 0)
	 result = 4;
      else
	 result = 1;
      }
      /*-----------------------------------------------------
       * Check for same variant assembly
       *-----------------------------------------------------*/

   else if (source_in_variant_assy_defn == true &&
	    searched_in_variant_assy_defn == false)
      result = 5;
   else if (source_in_variant_assy_defn == false &&
	    searched_in_variant_assy_defn == true)
      result = 6;
   else if (source_in_variant_assy_body == true &&
	    searched_in_variant_assy_body == false)
      result = 7;
   else if (source_in_variant_assy_body == false &&
	    searched_in_variant_assy_body == true)
      result = 8;
   else if (source_in_variant_assy_body == true &&
	    searched_in_variant_assy_body == true &&
	    common_variant_assy == false)
      result = 9;
   else if (source_in_variant_assy_body == true &&
	    searched_in_variant_assy_body == true &&
	    common_variant_assy == true &&
	    source_variant_assy_level == 0 &&   /* ie common part in source */
	    searched_variant_assy_level > 0)    /* but variant part in searched */
      {
      result = 10;
      }
   else if (source_in_variant_assy_body == true &&
	    searched_in_variant_assy_body == true &&
	    common_variant_assy == true &&
	    source_variant_assy_level > 0 &&   /* ie variant part in source */
	    searched_variant_assy_level == 0)  /* but common part in searched */
      {
      result = 11;
      }

   else if (source_in_variant_assy_body == true &&
	    searched_in_variant_assy_body == true &&
	    common_variant_assy == true &&
	    source_variant_assy_level > 0 &&
	    searched_variant_assy_level > 0)
      {
      if (strcmp(source_variant_assy[source_variant_assy_level].part_number,
		 searched_variant_assy[searched_variant_assy_level].part_number) != 0)

	 result = 12;
      else
	 result = 1;
      }

      /*-----------------------------------------------------
       * Check for same variant quantity
       *-----------------------------------------------------*/

      /*-----------------------------------------------------
       * Check for one of set
       *-----------------------------------------------------*/

   else
      result = 1;

   return(result);
   } /* compare_source_and_searched_state() */

int determine_directive_state(char section[max_lines][100],
			      int line,
			      char variant_quantity_label[],
			      char variant_labels[5][10],
			      struct variant_assy_ind variant_assy[],
			      int *variant_assy_number,
			      int *variant_assy_level,
			      enum boolean *in_variant_assy_defn,
			      enum boolean *in_variant_assy_body,
			      /* enum boolean *common_assy, */
			      int *variant_level,
			      enum boolean *in_variant_quantity,
			      enum boolean *in_one_of_set,
                              enum boolean *get_variant_assembly_desc,
			      enum directive *directive_out,
			      char directive_string[])
   {
   enum directive directive;
   /* char directive_string[80]; */
//   int part_number_posn;
   int i;

   if ((directive = check_directive(section[line],
				    directive_string)) == variant_assembly_start)
      {
      *variant_assy_number = 0;
      *variant_assy_level = 0;
      *in_variant_assy_defn = true;
      *in_variant_assy_body = false;
      }
   else if (directive == variant_assembly)
      {
      (*variant_assy_number)++;
      variant_assy[*variant_assy_number].label_displayed = false;
      variant_assy[*variant_assy_number].line = line;
      short_strcpy(variant_assy[*variant_assy_number].label,
		   directive_string, 5);
      *get_variant_assembly_desc = true;
      }
   else if (directive == common_assembly)
      {
      *in_variant_assy_defn = false;
      *in_variant_assy_body = true;
      *variant_assy_level = 0;
      /* *common_assy = true; */
      }
   else if (directive == in_assembly)
      {
      i = 1;
      while (strncmp(variant_assy[i].label, directive_string, 3) != 0 &&
	     i <= *variant_assy_number)
	 i++;
      *variant_assy_level = i;
      /* *common_assy = false; */
      }
   else if (directive == variant_assembly_end)
      {
      *variant_assy_number = 0;
      *variant_assy_level = 0;
      /* *common_assy = false; */
      }
   else if (directive == variant_start)
      {
      (*variant_level)++;
      strcpy(&variant_labels[*variant_level][0],
	     directive_string);
      }
   else if (directive == variant)
      {
      /* for each variant level there can only be one current label */
      strcpy(&variant_labels[*variant_level][0],
	     directive_string);
      }
   else if (directive == variant_end)
      {
      (*variant_level)--;
      }
   else if (directive == variant_quantity)
      {
      strcpy(variant_quantity_label,
	     directive_string);
      *in_variant_quantity = true;
      }
   else if (directive == variant_quantity_end)
      {
      *in_variant_quantity = false;
      }
   else if (directive == variant_one_of_set)
      {
      *in_one_of_set = true;
      }
   else if (directive == end_variant_set)
      {
      *in_one_of_set = false;
      }
   else if (directive == year)
      ;
   else if (directive == minor_model)
      ;
   else if (directive == major_model)
      ;
   else if (directive == title ||
            directive == end)
      ;
   else if (directive == error)
      return(0);

   *directive_out = directive;
   return(1);
   }   /* determine_directive_status() */

int get_var_assy_desc(char string_in[],
		      struct variant_assy_ind variant_assy[],
		      int variant_assy_number)
   {
   int part_number_posn;

   part_number_posn = 3;
   get_part_number(string_in,
                   variant_assy[variant_assy_number].part_number,
		   true,
		   part_number_posn);
   part_number_posn = 11;
   get_description(string_in,
                   variant_assy[variant_assy_number].description,
		   20,
		   part_number_posn);
   return 1;
   }    /* get_var_assy_desc()  */

int get_variant_assy_defn(char section[max_sections][max_lines][100],
			  int section_no,
			  int *line,
			  int *variant_assy_number,
			  int *variant_assy_level,
			  struct variant_assy_ind variant_assy[])
   {
   enum boolean variant_assy_defn_comp = false;
   enum directive directive;
   char directive_string[50];
   int part_number_posn = 3;

   *variant_assy_number = 0;
   *variant_assy_level = 0;
   if (section[section_no][*line][0] == '*')
      if ((directive = check_directive(&section[section_no][*line][0],
		       directive_string)) == variant_assembly_start)
	 {
	 while (variant_assy_defn_comp == false)
	    {
	    (*line)++;
	    if (section[section_no][*line][0] == '*')
	       {
	       if ((directive = check_directive(&section[section_no][*line][0],
				directive_string)) == variant_assembly)
		  {
		  (*variant_assy_number)++;
		  variant_assy[*variant_assy_number].label_displayed = false;
		  variant_assy[*variant_assy_number].line = *line;
		  short_strcpy(variant_assy[*variant_assy_number].label,
			       directive_string, 5);
		  (*line)++;
		  part_number_posn = 3;
		  get_part_number(&section[section_no][*line][0],
				  variant_assy[*variant_assy_number].part_number,
				  true,
				  part_number_posn);
		  part_number_posn = 11;
		  get_description(&section[section_no][*line][0],
				  variant_assy[*variant_assy_number].description,
				  20,
				  part_number_posn);
		  }
	       else if (directive == common_assembly)
		  {
		  variant_assy_defn_comp = true;
		  }
	       }
	    }
	 return(1);
	 }
      else
	 return(0);
   return(0);
   } /* get_variant_assy_defn */

int prepare_output(struct part_years results,
		   struct part_years_output *results_output)
   {
   int /*i,*/ j,k;

      for (j = 0; j < 20; j++)
	 {
	 for (k = 0; k < 5; k++)
	    {
	    if (results.year[j][k] == 0)
	       results_output->year[j][k] = ' ';
	    else if (results.year[j][k] == 99)
	       results_output->year[j][k] = '*';
	    else if (results.year[j][k] > 0 &&
		     results.year[j][k] < 10)
	       results_output->year[j][k] = results.year[j][k] + 48;
	    else if (results.year[j][k] >= 10 &&
		     results.year[j][k] <= 30)
	       results_output->year[j][k] = results.year[j][k] + 55;
	    else
	       results_output->year[j][k] = '?';
	    }
	 results_output->year[j][5] = '\0';
	 }
   return(1);
   } /* prepare_output() */

int calculate_combinations(struct part_years *results)
   {
   int /*i,*/ j;
   int perm = 0;

   for (j = 0; j < 20; j++)
      if (results->year[j][0] > 0)
	 {
	 perm = perm | bit_for_year[j];    /* note: bit orientated 'or' */
	 }
   results->combination = perm;
   return(1);
   }  /* calculate_combinations() */

int update_assembly_status(char string_in[],
			   int *assembly_level,
			   struct assembly_defn assembly[],
			   char assembly_part_list[5][10])
   {
   char dummy_string_out[100];
   int x;
   int quantity_posn = 0;

   x = 1;   /* want to avoid lines marked with '$' */
   while (string_in[x] != '\n' &&
	  string_in[x] != '\0')
      {
      if (string_in[x] == '$')
	 quantity_posn = x;
      x++;
      }
   if (check_quantity(string_in,
		      dummy_string_out,
		      &quantity_posn) > 0)
   if (*assembly_level > 0)
      assembly[*assembly_level].lines_to_go--;

   determine_assembly_status(string_in,
			     assembly_level,
			     assembly,
			     assembly_part_list);

   while (assembly[*assembly_level].lines_to_go == 0 &&
	  *assembly_level > 0)
      (*assembly_level)--;

   return(1);
   }  /*  update_assembly_status()  */ 

handle_searched_directive(enum directive directive,
			  enum boolean *common_variant_assy,
			  enum boolean *check_for_common_variant_assy,
			  enum boolean source_in_variant_assy_body,
			  int *source_variant_assy_number,
			  int *searched_variant_assy_no,
			  struct variant_assy_ind source_variant_assy[],
			  struct variant_assy_ind searched_variant_assy[],
		          enum boolean *end_of_section_searched)
   {
   int x,y;

   if (directive == variant_assembly_start)
      {
      *common_variant_assy = false;
      *check_for_common_variant_assy = true;
      }
   else if (directive == common_assembly)
      {
      if (*check_for_common_variant_assy == true &&
	  source_in_variant_assy_body == true)
	 {
	 for (x = 1; x <= *source_variant_assy_number; x++)
	    for (y = 1; y <= *searched_variant_assy_no; y++)
	       if (strcmp(source_variant_assy[x].part_number,
		   searched_variant_assy[y].part_number) == 0)
		   {
		   searched_variant_assy[y].label_displayed = true;
		   *common_variant_assy = true;
		   }
	       *check_for_common_variant_assy = false;
	 }
      }
   else if (directive == variant_assembly_end)
      *common_variant_assy = false;
   else if (directive == title ||
            directive == end)
      {
      *end_of_section_searched = true;
      }
   return(1);
   } /*  handle_searched_directive()  */

int mark_section_title(int file_no,
		       int section_no,
		       char current_section[])
   {
   FILE *fptr_file;
   int line;
   int number_of_sections;
   enum boolean section_found;
   enum directive directive;
   char file_name[20];
   char string_in[101];
   char directive_string[100];
   fpos_t start_of_line_last_read;

   /* Open file */
   strcpy(file_name,"TEMP00.LST");
   make_temp_file_name(file_name,
		       4,
		       file_no);
   if ((fptr_file = fopen(file_name,"r+")) == NULL)
      {
      printf("2 Cannot open file %s\n",file_name);
      exit(0);
      }
   /* find section to be marked */
   number_of_sections = 0;
   section_found = false;
   line = 0;
   while (section_found == false)
      {
      if (fgetpos(fptr_file, &start_of_line_last_read) != 0)
	 printf("Position of line in file not found\n");
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
		     {
		     section_found = true;
		     }
	       number_of_sections++;
	       }
	    }
	 }
      line++;
      }
   if (section_found == true)
      {
      /* Mark section as having been included */
      string_in[7] = '$';    /* *title Abc cdef ... */
      if (fsetpos(fptr_file, &start_of_line_last_read) != 0)
	 printf("Error: fsetpos() error\n");
      if (fputs(string_in,fptr_file) == EOF)
	 {
	 printf("Marked string not written\n");
	 printf("%s\n",string_in);
	 }
      }
   fclose(fptr_file);
   if (section_found == true)
      return(line);
   else
      return(0);
   }  /* mark_section_title() */
