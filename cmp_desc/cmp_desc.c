                               /* cmp_desc.c */
/* The purpose of this program is to find and compare differing descriptions */
/* for the same part number */
/* */
/* 22/7/94 Modified to handle files of unlimited size */
/* */

/*
CMP_DESC.C

The purpose of this program is to find and compare differing descriptions
for the same part number.

The input to this program is a definition file defining which parts lists are
to be compared and a set of original parts lists.

The output is a data file "cmp_desc.dat" which contains records of type
'part_desc'. The program also writes a copy of the results directly to the
screen and these can therefore be re-directed to a text file.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int compare_descriptions(char source_section[100][200],
			 int source_file,
			 int source_section_no,
			 int l,
			 int searched_file,
             int searched_section_no,
			 int m,                 /* Not used at present */
			 struct part_desc *results,
             FILE **fptr_rtn);

char source_section[100][200];

void main(int argc,
	  char *argv[])
   {
   FILE *fptr_defn;
   FILE *fptr_res;
   FILE *fptr_searched;

   enum boolean end_of_source_section;

   char defn_file[20][50];			/* store definition file */
   char current_section[50];			/* title of current section */
   char string_in[201];
//   char directive_string[81];
   char source_part_number[10];
   enum directive directive = none;
   enum boolean end_of_source_file;
   enum boolean got_source_section;
   enum boolean end_of_searched_file;
   enum boolean got_searched_section;
   enum boolean all_sections = false;
   struct part_desc results;
   int i, l, m = 0;
   int title_posn;
   int size;
   int line;
   int position;
   int source_section_no;
   int source_sect;
   int searched_section_no;
   int searched_sect;
   int source_file;
   int searched_file;

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
   if ((fptr_res = fopen("cmp_desc.dat","wb")) == NULL)
      {
      printf("Cannot open file 'cmp_desc.dat' \n");
      exit(0);
      }
   for (source_file = 1; source_file < title_posn; source_file++)   /* i.e. for each parts list (i.e. file) */
      {
      end_of_source_file = false;
      source_section_no = 0;   /* Section reference when ALL sections are being referenced */
      source_sect = 0;         /* References section number in 'definition' file */
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
	 if (got_source_section == true)
	    {
	    l = 0;
            end_of_source_section = false;
	    while (end_of_source_section == false)
               {
	       if (source_section[l][0] == '*')        /* Represents lines of section */
                  ;                                    /* Ignore directives */
	       else if (source_section[l][0] == '$')   /* Part already covered   */
                  ;
	       else if (source_section[l][0] == '%')
	          {
	          end_of_source_section = true;
	          }
	       else   /* Part number definition therefore get description */
		  {
	          position = 3;
		  get_part_number(source_section[l],     /* Need to check returned value */
				  source_part_number,    /* eg 0 for blank part number */
			          true,
			          position);
		  strcpy(results.part_number, source_part_number);
		  position = 11;
		  get_description(source_section[l],
				  results.description[0],
				  199,
				  position);
		  results.no_of_descs = 1;
		  /* Search through parts lists for found part */
		  for (searched_file = source_file; searched_file < title_posn; searched_file++)
                     {
	             end_of_searched_file = false;
		     if (searched_file == source_file &&
	                 all_sections == true)
		        {
		        searched_section_no = source_section_no;
		        searched_sect = source_sect;
		        }
	             else
                        {
                        searched_section_no = 0;
	        	searched_sect = 0;
                        }
                     fptr_searched = NULL;
		     while (end_of_searched_file == false)
	                {
                        /* Get next searched section */
	                if (all_sections == true)
			   {
                           strcpy(current_section,"next");
			   if ((line = get_file_line(searched_file,
			                             searched_section_no,
						     current_section,
                                                     &fptr_searched)) == 0)
	                      {
			      end_of_searched_file = true;
			      got_searched_section = false;
			      }
		           else
			      got_searched_section = true;
			   /* searched_section_no++; */
			   }
			else
			   {
			   strcpy(current_section,defn_file[title_posn + searched_sect + 1]); /* This is the   */
											      /* section title */
			   if ((line = get_file_line(searched_file,
			                             searched_section_no,
						     current_section,
                                                     &fptr_searched)) == 0)
			      got_searched_section = false;
		           else
	                      got_searched_section = true;
			   /* searched_section_no++; */
			   searched_sect++;
			   if (searched_sect == size - title_posn - 1)
			      end_of_searched_file = true;
			   }
			if (got_searched_section == true)
			   {
                           if (source_file == searched_file &&
			       source_section_no == searched_section_no)
                              m = line + l + 1;
                           else
                              m = line;   /* i.e. first line in section */
                           compare_descriptions(source_section,
						source_file,
                                                source_section_no,
		                                l,
						searched_file,
						searched_section_no,
						m,
						&results,
                                                &fptr_searched);
			   }
			searched_section_no++;
			}
                     fclose(fptr_searched);
		     }
	          fwrite(&results,sizeof(results), 1, fptr_res);
		  }
               l++;
	       }
	    }
	 source_section_no++;
	 }
      }
   _fcloseall();
   if ((fptr_res = fopen("cmp_desc.dat","rb")) == NULL)
      {
      printf("Cannot open file 'cmp_desc.dat' for printout\n");
      exit(0);
      }
   else
      {
      while (fread(&results,sizeof(results),1,fptr_res) == 1)
         {
         if (results.no_of_descs == 1)
            {
	    printf("%s  ",results.part_number);
	    printf("%s\n",results.description[0]);
	    /* printf("\n"); */
            }
	 else if (results.no_of_descs > 1)
            {
	    printf("%s   ",results.part_number);
	    printf("%d\n",results.no_of_descs);
	    for (i = 0; i < results.no_of_descs; i++)
	       printf("%s\n",results.description[i]);
	    printf("\n");
	    }
         }
      }
   }  /* main() */

int compare_descriptions(char source_section[100][200],
			 int source_file,
			 int source_section_no,
			 int l,
			 int searched_file,
                         int searched_section_no,
			 int m,                 /* Not used at present */
			 struct part_desc *results,
                         FILE **fptr_rtn)
   {
   FILE *fptr_file;
   int i,j; //,k;
//   int line;
   int position;
   int ll;           /* Only required for marking source_section[] */
   enum directive directive;
   enum boolean end_of_section_searched;
   enum boolean description_found;
   fpos_t start_of_line_last_read;
   fpos_t end_of_line_last_read;
//   char file_name[20];
   char string_in[201];
   char directive_string[201];
   char searched_description[200];

   fptr_file = *fptr_rtn;
   end_of_section_searched = false;
   /* Move to first line after line in source section */
   if (source_file == searched_file &&
       source_section_no == searched_section_no)
      {
      i = 0;
      while (i <= l)
         {
         fgets(string_in, 200, fptr_file);
         if (is_blank_line(string_in) == 0)
            i++;
         }
      }
   ll = l;  /* Check that this cannot get out of synchronisation */
   /* */
   /* fgetpos(fptr_file, &start_of_line_last_read); */
   while (end_of_section_searched == false)
      {
      if (fgetpos(fptr_file, &start_of_line_last_read) != 0)
	 printf("Position of line in file not found\n");
      if (fgets(string_in, 200, fptr_file) == NULL)
         break;
      fgetpos(fptr_file, &end_of_line_last_read);
      if (is_blank_line(string_in) == 0)
	 {
         ll++;     /* Increment pointer to source section line */
	 if (string_in[0] == '*')
	    {
	    directive = check_directive(string_in, directive_string);
	    if (directive == title ||
		directive == end)
	       {
               end_of_section_searched = true;
	       }
	    }
         else if (string_in[0] == '$') /* Marked on a previous iteration */
	    ;
	    /* Start at &3 as don't want to compare reference number and */
	    /* only compare part number. */
	    /* This comparison should really be made against the description */
	    /* in the results record as in case of 'odd' part number it will */
	    /* be different to that in the section array */
	 else if (strncmp(&source_section[l][3],&string_in[3], 7) == 0)
	    {
	    position = 11;
	    get_description(string_in,
			    searched_description,
			    199,
			    position);
	    description_found = false;
	    for (j = 0; j < results->no_of_descs; j++)
	       {
	       if (strcmp(results->description[j],
			  searched_description) == 0)
	          {
		  description_found = true;
		  break;
		  }
	       }
	    if (description_found == false)
	       {
	       /* check there is space for another description */
	       if (results->no_of_descs < 5)  /* i.e. room for one more */
                  {
	          strcpy(results->description[results->no_of_descs],
		         searched_description);
		     results->no_of_descs++;
		  }
	       else
		  {
                  strcpy(results->description[4],"******************\n");
		  }
	       }
	    string_in[0] = '$';
	    if (fsetpos(fptr_file, &start_of_line_last_read) != 0)
	       printf("Error: fsetpos() error\n");
	    if (fputs(string_in,fptr_file) == EOF)
               {
	       printf("Marked string not written\n");
	       printf("%s\n",string_in);
               }
	    fsetpos(fptr_file, &end_of_line_last_read);
	    /* Mark line in source section for special case */
	    if (source_file == searched_file &&
	     	source_section_no == searched_section_no)
	       {
	       if (strncmp(&source_section[ll][3],&string_in[3], 7) != 0)
                  {
                  printf("Source and searched synchronisation error\n");
                  printf("%d %d, %d %d\n",source_file, source_section_no,
                                          searched_file, searched_section_no);
                  printf("%s\n",source_section[l]);
                  printf("l = %d, ll = %d\n",l,ll);
                  exit(0);
                  }
	       source_section[ll][0] = '$';
	       }
	    }
	 /* part not found */
	 else
	    ;
	 }
      }
   /* fclose(fptr_file);     */
   if (fsetpos(fptr_file, &start_of_line_last_read) != 0)
      printf("Error: fsetpos() error\n");
   *fptr_rtn = fptr_file;
   return 1;
   }   /* compare_descriptions() */
