                               /* chk_assy.c */
/* This program checks for consistent assembly definitions across files */
/* */
/* 22/7/94 Modified to handle files of unlimited size */
/* */

/* CHK_ASSY.C

This program checks for consistent assembly definitions across files.

The input to this program is a definition file defining which parts lists are
to be checked and a set of original parts lists. The output is a list of
common assemblies and any errors. The output is written directly to the screen,
but can be directed to a file using MS-DOS.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

struct assembly_defn searched_assembly[10][5];
char source_section[100][100];
char searched_section[100][100];

int compare_assemblies(char source_section[100][100],
                       int source_file,
					   int l,
                       int searched_file,
		               int m,
                       enum boolean use_alt_part_no,
                       char source_part[]);

void main(int argc,
	  char *argv[])
   {
//   FILE *fptr_in;
//   FILE *fptr_out;
   FILE *fptr_defn;
//   FILE *fptr_list;

   enum boolean end_of_source_section;
//   struct assembly_defn searched_assembly[5];

   char defn_file[20][50];			/* store definition file */
   char current_section[50];			/* title of current section */
   char string_in[101];
   char string_out[101];
   char directive_string[81];
   char alt_part_manufacturer[21];
   char alt_part_number[21];
   enum directive directive = none;
   enum boolean end_of_source_file;
   enum boolean got_source_section;
   enum boolean end_of_searched_file;
   enum boolean got_searched_section;
   enum boolean all_sections = false;
   enum boolean in_variant_assy_defn = false;
   enum boolean use_alt_part_no;
   int i, l, m = 0;
   int assembly_position;
   int title_posn;
   int size;
   int line;
   int source_section_no;
   int source_sect;
   int searched_section_no;
   int searched_sect;
   int source_file;
   int searched_file;
   int position;

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
   for (source_file = 1; source_file < title_posn; source_file++)   /* i.e. for each parts list (i.e. file) */
      {
      end_of_source_file = false;
      source_section_no = 0;
      source_sect = 0;
      while (end_of_source_file == false)
	 {
         /* Get next source section */
	 if (all_sections == true)
	    {
	    if (get_section(source_section,
			    source_file,
			    source_section_no,
			    "next") == 0)
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
         /* Go through source section searching for assemblies */
	 if (got_source_section == true)
	    {
	    l = 0;
            end_of_source_section = false;
	    in_variant_assy_defn = false;
	    while (end_of_source_section == false)
               {
	       if (source_section[l][0] == '*')
                  {
		  directive = check_directive(string_in, directive_string);
		  if (directive == variant_assembly_start)
		     in_variant_assy_defn = true;
		  else if (directive == common_assembly ||
			   directive == in_assembly)
		     in_variant_assy_defn = false;
                  }
               else if (source_section[l][0] == '%')
	          end_of_source_section = true;
               else if (source_section[l][0] == '$')
		  ;
               /* if an assembly is found search through other sections */
	       else if ((assembly_position = is_assembly(&source_section[l][0])) > 0 &&
			in_variant_assy_defn == false)
		  {
                  use_alt_part_no = false;
                  if (source_section[l][3] == '-')   /* i.e. no part number */
                     {
	             i=1;
                     alt_part_number[0] = '\0';
                     alt_part_manufacturer[0] = '\0';
	             while (source_section[l][i] != '\n'&&
		            source_section[l][i] != '\0')
	                {
	                if (source_section[l][i] == '*')
                           {
		           position = i;
                           check_alt_part_number(source_section[l],
                                                 alt_part_manufacturer,
                                                 alt_part_number,
                                                 string_out,
						 &position);
                           use_alt_part_no = true;
                           break;         /* if no alt. part found don't really want to continue */
                           }
                        i++;
                        }
                     }
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
		     while (end_of_searched_file == false)
	                {
                        /* Get next searched section */
	                if (all_sections == true)
	                   {
			   if ((line = get_section_line(searched_file,
			                                searched_section_no,
							"next")) == 0)
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
			   if ((line = get_section_line(searched_file,
				                        searched_section_no,
						        current_section)) == 0)
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
                           compare_assemblies(source_section,
					      source_file,
					      l,
					      searched_file,
		                              m,
                                              use_alt_part_no,
                                              alt_part_number);
			   }
			searched_section_no++;
			}
                     }
		  }
               l++;
	       }
	    }
	 source_section_no++;
	 }
      }
   _fcloseall();
   }  /* main() */

int compare_assemblies(char source_section[100][100],
                       int source_file,
					   int l,
                       int searched_file,
		               int m,
                       enum boolean use_alt_part_no,
                       char source_part[])
   {
   FILE *fptr_file;
   int i,j; //,k;
   int line;
   int assembly_position;
   int searched_assembly_level;
   int position;
   enum directive directive;
   enum boolean end_of_section_searched;
   struct assembly_defn searched_assembly[5];
   fpos_t start_of_line_last_read;
   fpos_t end_of_line_last_read;
   char file_name[20];
   char string_in[101];
   char string_out[101];   /* dummy */
   char directive_string[101];
   char alt_part_manufacturer[21];
   char alt_part_number[21];

   line = l;
   searched_assembly_level = 0;
   for (j = 0; j < 5; j++)
      {
      searched_assembly[j].lines_to_go = 0;
      searched_assembly[j].components_to_go = 0;
      }
   end_of_section_searched = false;

   /* Open file */
   strcpy(file_name,"TEMP00.LST");
   make_temp_file_name(file_name,
		       4,
		       searched_file);
   if ((fptr_file = fopen(file_name,"r+")) == NULL)
      {
      printf("Cannot open file %s\n",file_name);
      exit(0);
      }

   /* Move file pointer to start of section */
   i = 0;
   while (i < m)
      {
      if (fgets(string_in, 100, fptr_file) == NULL)
	 {
	 printf("Error: Section not reached\n");
	 exit(0);
	 }
      i++;
      }

   fgetpos(fptr_file, &start_of_line_last_read);
   while (end_of_section_searched == false &&
	  fgets(string_in, 100, fptr_file) != NULL)
      {
      fgetpos(fptr_file, &end_of_line_last_read);
      if (is_blank_line(string_in) == 0)
         {
	 if (string_in[0] == '*')
	    {
	    directive = check_directive(string_in, directive_string);
	    if (directive == title ||
		directive == end)
	       {
               end_of_section_searched = true;
	       }
	    }
         else if (string_in[0] == '$' &&         /* Marked on a previous iteration */
                  searched_assembly_level <= 0)
	    ;
	 else
	    {
	    /* Check for same part number */
            if (use_alt_part_no == true &&
                source_part[0] != '\0')
               {
	       i=1;
               alt_part_number[0] = '\0';
               alt_part_manufacturer[0] = '\0';
	       while (string_in[i] != '\n' &&
		      string_in[i] != '\0')
	          {
	          if (string_in[i] == '*')
                     {
		     position = i;
                     check_alt_part_number(string_in,
                                           alt_part_manufacturer,
                                           alt_part_number,
                                           string_out,
					   &position);
                     break;
                     }
                  i++;
                  }
               }
	    if ((searched_assembly_level > 0) ||
                (use_alt_part_no == false &&
                 strncmp(&source_section[l][3],&string_in[3],7) == 0) ||
                (use_alt_part_no == true &&
                 source_part[0] != '\0' &&
                 strcmp(source_part,alt_part_number) == 0))
	       {
	       /* line = l; */
	       if (searched_assembly_level == 0)
                  {
	          printf("\nCommon assembly : %d  %d\n",source_file,searched_file);
	          printf("%s",source_section[line]);
	          if (strcmp(&source_section[line][3],&string_in[3]) != 0)
	             {
	             printf(" *** Different descriptions ***\n");
		     printf("%s",string_in);
	             }
		  if ((assembly_position = is_assembly(&string_in[0])) > 0)
	             {
		     check_assembly(&string_in[0],
				    &searched_assembly_level,
			            &assembly_position,
			            &searched_assembly[0]);
	             }
		  string_in[0] = '$';
		  }
	       else if (searched_assembly_level > 0)
		  {
		  /* line++; */
		  if (strcmp(&source_section[line][3],&string_in[3]) != 0)
	             {
	             printf("   *** Error ***\n");
	             printf("%s",source_section[line]);
		     printf("%s",string_in);
	             }
	          if (searched_assembly_level > 0)
	             {
	             searched_assembly[searched_assembly_level].lines_to_go--;
	             if (searched_assembly[searched_assembly_level].lines_to_go == 0)
	                searched_assembly_level--;
	             }
		  if ((assembly_position = is_assembly(&string_in[0])) > 0)
	             {
		     check_assembly(&string_in[0],
				    &searched_assembly_level,
			            &assembly_position,
			            &searched_assembly[0]);
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
	       line++;
	       }
	    }
	 }
      if (fgetpos(fptr_file, &start_of_line_last_read) != 0)
         printf("Position of line in file not found\n");
      }
   fclose(fptr_file);
   return 1;
   }   /* compare_assemblies() */

