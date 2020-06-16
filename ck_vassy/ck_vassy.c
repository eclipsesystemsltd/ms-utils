/*                            ck_vassy.c                                   */
/* This program checks for consistent variant assembly definitions across  */
/* files.                                                                  */
/* */
/* 22/7/94 Modified to handle files of unlimited size */
/* */

/*
CK_VASSY.C

This program checks for consistent variant assembly definitions across files.
The inputs and outputs are as for CHK_ASSY.C.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"
#include "ck_vassy.h"

static char error_messages[12][60] = {
   "Unknown error\n",                                    /* 0 */
   "Error: Same part number, different labels\n",        /* 1 */
   "Error: Searched labelled part longer than source\n", /* 2 */
   "Error: Different part number/description in variant\n",/* 3 */
   "Error: Searched labelled part shorter than source\n",/* 4 */
   "Warning: Missing Variant assembly label\n",          /* 5 */
   "Error: Extra variant assembly label found \n",       /* 6 */
   "Error: Common part of variant assembly missing\n",   /* 7 */
   "Error: Extra variant assy. common section found\n",  /* 8 */
   "Error: Searched common part longer than source \n",  /* 9 */
   "Error: Searched common part shorter than source\n",  /* 10 */
   "Error: Different part number/description in common\n"/* 11 */
   };

struct variant_assy_ind variant_assy[5];
struct variant_assy_ind searched_variant_assy[5];
struct assembly_defn searched_assembly[10][5];
char source_section[100][100];
char searched_variant_assy_defn[100][100];

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
//   char string_out[101];
   char directive_string[81];
   enum directive directive = none;
   enum boolean end_of_source_file;
   enum boolean got_source_section;
   enum boolean end_of_searched_file;
   enum boolean got_searched_section;
   enum boolean all_sections = false;
   int i, l, m = 0;
//   int assembly_position;
   int title_posn;
   int size;
   int line;
   int source_section_no;
   int source_sect;
   int searched_section_no;
   int searched_sect;
   int source_file;
   int searched_file;
   int variant_assy_number;
   int variant_assy_level;

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
         /* Go through source section searching for variant assemblies */
	 if (got_source_section == true)
	    {
	    l = 0;
	    variant_assy_number = 0;
	    variant_assy_level = 0;
            end_of_source_section = false;
	    while (end_of_source_section == false)
	       {
	       if (source_section[l][0] == '*')
                  {
		  if ((directive = check_directive(&source_section[l][0],
				directive_string)) == variant_assembly_start)
		     {
		     get_variant_assy_defn(source_section,
					   &l,
					   &variant_assy_number,
					   &variant_assy_level,
					   variant_assy);
		     /*---------------------------------------------------------
		      * Search through sections for variant assemblies
		      *--------------------------------------------------------*/
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
				 /* m = line + l;  in orig. ck_vassy.c */
                              else
				 m = line;   /* i.e. first line in section */
		              compare_variant_assemblies(source_section,
							 source_file,
							 source_section_no,
							 l,
							 searched_file,
                                                         searched_section_no,
							 m,
							 variant_assy_number);
			      }
                           searched_section_no++;
			   }
			}
		     }
		  }
               else if (source_section[l][0] == '%')
	          end_of_source_section = true;
               else if (source_section[l][0] == '$')
		  ;
               l++;
	       }
	    }
	 source_section_no++;
	 }
      }
   _fcloseall();
   }  /* main() */

compare_variant_assemblies(char source_section[100][100],
			   int source_file,
               int source_section_no,
			   int ll,
			   int searched_file,
               int searched_section_no,
			   int mm,
			   int source_variant_assy_number)
   {
   FILE *fptr_file;
//   int i,j,k,l,m,x,y;
   int i,l,m,x,y;
//   int assembly_position;
   int var_assy_line;
   int searched_var_assy_no;
   int searched_var_assy_level;
   enum directive directive;
   enum boolean end_of_section_searched;
   enum boolean in_variant_assembly;
   enum boolean common_variant_assy;
//   struct assembly_defn searched_assembly[5];
//   fpos_t start_of_line_last_read;
   fpos_t end_of_line_last_read;
   char file_name[20];
   char string_in[101];
   char directive_string[101];

   l = ll;
   m = mm;
   end_of_section_searched = false;
   in_variant_assembly = false;

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
   while (end_of_section_searched == false &&
	  fgets(string_in, 100, fptr_file) != NULL)
      {
      /* This loop copies lines of searched file into searched_variant_assy_defn
       * until a variant_assembly_end is reached, at which point get_variant_assy_defn()
       * is called to build up a variant assembly structure ready for further analysis.
       */
      fgetpos(fptr_file, &end_of_line_last_read);
      if (is_blank_line(string_in) == 0)
         {
	 if (string_in[0] == '*')
	    {
	    if ((directive = check_directive(string_in,
				             directive_string)) == variant_assembly_start)
	       {
               in_variant_assembly = true;
	       var_assy_line = 0;
	       strcpy(searched_variant_assy_defn[var_assy_line],string_in);
	       var_assy_line++;
	       }
	    else if ((directive == common_assembly ||
		      directive == in_assembly ||
		      directive == variant_assembly) &&
                     in_variant_assembly == true)
	       {
	       strcpy(searched_variant_assy_defn[var_assy_line],string_in);
	       var_assy_line++;
               }
	    else if (directive == variant_assembly_end &&
		     in_variant_assembly == true)
	       {
	       strcpy(searched_variant_assy_defn[var_assy_line],string_in);
	       var_assy_line++;
	       m = 0;
	       get_variant_assy_defn(searched_variant_assy_defn,
				     &m,
				     &searched_var_assy_no,
				     &searched_var_assy_level,
				     searched_variant_assy);
	       /*--------------------------------------------------------------
	       * Compare source and found variant assemblies for common parts
	       *-------------------------------------------------------------*/
	       common_variant_assy = false;
	       for (x = 1; x <= source_variant_assy_number; x++)
		  for (y = 1; y <= searched_var_assy_no; y++)
		     if (strcmp(variant_assy[x].part_number,
			        searched_variant_assy[y].part_number) == 0)
			{
			printf("!!! Common variant assembly found : source: %d,%d, searched %d,%d !!!\n ",
                                                                    source_file, source_section_no,
                                                                    searched_file, searched_section_no);
			printf("part: %s",variant_assy[x].part_number);
			printf("   label: %s\n",variant_assy[x].label);
			if (strcmp(variant_assy[x].label,
				   searched_variant_assy[y].label) == 0)
		           {
                           /* Note: the label_displayed is not used here in its original meaning (as in chk_assy),
                            * but is used to indicate that a label is in both the source and searched variant
                            * assemblies.
                            */
			   variant_assy[x].label_displayed = true;
			   variant_assy[y].label_displayed = true;   /* added 3/10/94 */
			   common_variant_assy = true;
			   searched_variant_assy_defn[searched_variant_assy[y].line][0] = '$';
			   }
			else
			   {
                           /* Labels don't match */
                           print_error(1,
                                       source_file,
				       source_section_no,
                                       l,
                                       searched_file,
				       searched_section_no,
				       m);
			   }
			}
			/*-------------------------------------------------------
			* Go through searched section looking for label and if
			* common ones found compare source and searched variant
			* assemblies.
			*-------------------------------------------------------*/
			if (common_variant_assy == true)
			   {
			   check_common_var_assy(source_section,
						 source_file,
						 source_section_no,
						 &l,
						 searched_variant_assy_defn,
						 searched_file,
                                                 searched_section_no,
						 &m,
						 variant_assy,
						 source_variant_assy_number);
			   }
	       in_variant_assembly = false;
	       }
	    else if (directive == title ||
	             directive == end)
	       end_of_section_searched = true;
	    }
	 else
	    {
	    if (in_variant_assembly == true)
	       {
	       strcpy(searched_variant_assy_defn[var_assy_line],string_in);
	       var_assy_line++;
               }
	    }
	 }
      }
   fclose(fptr_file);
   return 1;
   }   /* compare_assemblies() */

int get_variant_assy_defn(char section[max_lines][100],
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
   if (section[*line][0] == '*')
      if ((directive = check_directive(&section[*line][0],
		       directive_string)) == variant_assembly_start)
         {
	 while (variant_assy_defn_comp == false)
	    {
            (*line)++;
            if (section[*line][0] == '*')
	       {
	       if ((directive = check_directive(&section[*line][0],
		                directive_string)) == variant_assembly)
	          {
		  (*variant_assy_number)++;
		  variant_assy[*variant_assy_number].label_displayed = false;
		  variant_assy[*variant_assy_number].line = *line;
		  short_strcpy(variant_assy[*variant_assy_number].label,
			       directive_string, 5);
		  (*line)++;
		  part_number_posn = 3;
		  get_part_number(&section[*line][0],
				  variant_assy[*variant_assy_number].part_number,
				  true,
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
   } /* get_variant_assy_defn() */

int check_common_var_assy(char source_section[max_lines][100],
			  int source_file,
			  int source_section_no,
			  int *source_line,
			  char searched_section[max_lines][100],
			  int searched_file,
              int searched_section_no,
			  int *searched_line,
			  struct variant_assy_ind variant_assy[],
			  int variant_assy_number)
   {
   int i, l, m;
   enum boolean source_var_assy_end;
   enum boolean searched_var_assy_end;
   enum boolean common_var_assy;
   enum directive directive;
   char directive_string[50];

   /*-----------------------------------------------------------------
    * First, check labelled parts of variant assembly definitions.
   /*----------------------------------------------------------------*/
   for (i = 1; i <= variant_assy_number; i++)
      {
      l = *source_line;
      m = *searched_line;
      if (variant_assy[i].label_displayed == true)
	 {
         source_var_assy_end = false;
	 while (source_var_assy_end == false)
	    {
	    if (find_var_assy_label(source_section,
				    &l,
				    variant_assy[i].label) == 1)
	       {
	       /* Search through searched section for label */
	       if (find_var_assy_label(searched_section,
				       &m,
				       variant_assy[i].label) == 1)
		  {
		  common_var_assy = false;
		  while (common_var_assy == false)
		     {
		     l++;
		     m++;
		     if ((directive = check_directive(&source_section[l][0],
					      directive_string)) == common_assembly ||
	                  directive == in_assembly ||
	                  directive == variant_assembly_end)
			{
			common_var_assy = true;
	                if ((directive = check_directive(&searched_section[m][0],
					      directive_string)) != common_assembly &&
			     directive != in_assembly &&
			     directive != variant_assembly_end)
			   {
                           /* Searched variant part longer than that of source */
			   print_error(2,
			               source_file, source_section_no, l,
				       searched_file, searched_section_no, m);
			   }
			}
		     else if (searched_section[m][0] == '*')
                        {
                        if ((directive = check_directive(searched_section[m],
					            directive_string)) == common_assembly ||
	                                            directive == in_assembly ||
	                                            directive == variant_assembly_end)
			   {
			   /* Searched labelled part shorter than source */
	                   print_error(4,
                                       source_file, source_section_no, l,
                                       searched_file, searched_section_no, m);
                           common_var_assy = true;
                   	   }
                        }
		     else if (directive == error) /* ie not a directive */
			if (strcmp(&source_section[l][3],
			           &searched_section[m][3]) != 0)
			   {
                           /* Differing part number or description */
	                   print_error(3,
                                       source_file, source_section_no, l,
                                       searched_file, searched_section_no, m);
			   }
			else
			   ;
		     }
		  }
	       else
		  {
                  /* Missing Variant assembly label */
	          print_error(5,
                              source_file, source_section_no, l,
                              searched_file, searched_section_no, m);
		  /* This is not really an error as two variant assemblies don't */
                  /* need to have identical sets of labels. */
		  }
	       }
	    else
	       {
	       source_var_assy_end = true;
	       if (find_var_assy_label(searched_section,
				       &m,
				       variant_assy[i].label) == 1)
		  {
		  /* Extra variant assembly label found */
	          print_error(6,
                              source_file, source_section_no, l,
                              searched_file, searched_section_no, m);
		  }
	       }
	    }
	 }
      }
   /*---------------------------------------------------------------------
    * Then go through searched section looking for Common parts
    *--------------------------------------------------------------------*/
   l = *source_line;
   m = *searched_line;
   source_var_assy_end = false;
   searched_var_assy_end = false;
   while (source_var_assy_end == false &&
          searched_var_assy_end == false)
      {
      if (find_var_assy_common(source_section,
			       &l) == 1)
	 {
	 /* Search through searched section for common part */
	 if (find_var_assy_common(searched_section,
				  &m) == 1)
	    {
	    if (compare_var_assy_common_sections(source_section,
						 source_file,
						 source_section_no,
						 &l,
						 searched_section,
						 searched_file,
						 searched_section_no,
						 &m) != 1)
               {
	       print_error(0,
                           source_file, source_section_no, l,
			   searched_file, searched_section_no, m);
	       }
            }
	 else
	    {
	    /* Common part of variant assembly missing */
	    print_error(7,
                        source_file, source_section_no, l,
		       	searched_file, searched_section_no, m);
	    searched_var_assy_end = true;
	    }
	 }
      else
         {
	 source_var_assy_end = true;
	 if (find_var_assy_common(searched_section,
				     &m) == 1)
	    {
	    /* Extra variant assembly common section found */
	    print_error(8,
                        source_file, source_section_no, l,
                        searched_file, searched_section_no, m);
	    }
	 }
      }
   return(1);
   }  /* check_common_var_assy() */

int find_var_assy_label(char section[max_lines][100],
			int *line,
			char label[])
   {
   enum boolean variant_assy_end = false;
   enum directive directive;
   char directive_string[50];

   while (variant_assy_end == false)
      {
      if (section[*line][0] == '*')
	 if ((directive = check_directive(&section[*line][0],
                    directive_string)) == variant_assembly_end)
            {
	    variant_assy_end = true;
	    }
         else if (directive == in_assembly)
            if (strcmp(label,
		       directive_string) == 0)
	       {
	       return(1);
	       }
      (*line)++;
      }
   return(0);   /* label not found */
   } /* find_var_assy_label() */

int find_var_assy_common(char section[max_lines][100],
			 int *line)
   {
   enum boolean variant_assy_end = false;
   enum directive directive;
   char directive_string[50];

   while (variant_assy_end == false)
      {
      if (section[*line][0] == '*')
         {
	 if ((directive = check_directive(&section[*line][0],
		  directive_string)) == variant_assembly_end)
	    {
	    variant_assy_end = true;
	    }
         else if (directive == common_assembly)
	    {
	    return(1);
            }
	 }
      (*line)++;
      }
   return(0);
   } /* find_var_assy_common() */

int compare_var_assy_common_sections(char source_section[max_lines][100],
				     int source_file,
				     int source_section_no,
				     int *source_line,
				     char searched_section[max_lines][100],
				     int searched_file,
                     int searched_section_no,
				     int *searched_line)
   {
   enum boolean source_common_var_assy = false;
   enum directive source_directive;
   enum directive searched_directive;
   char directive_string[50];

   if ((source_directive = check_directive(
		       &source_section[*source_line][0],
		       directive_string)) == common_assembly &&
       (searched_directive = check_directive(
		       &searched_section[*searched_line][0],
		       directive_string)) == common_assembly)
      {
      source_common_var_assy = true;
      while (source_common_var_assy == true)
         {
	 (*source_line)++;
	 (*searched_line)++;
	 if (source_section[*source_line][0] == '*')
	    {
	    if ((source_directive = check_directive(
			     &source_section[*source_line][0],
			     directive_string)) == variant_assembly_end ||
                 source_directive == in_assembly)
	       {
	       source_common_var_assy = false;
	       if ((searched_directive = check_directive(
				  &searched_section[*searched_line][0],
				  directive_string)) != variant_assembly_end &&
		    searched_directive != in_assembly)
		  {
		  /* Searched Variant Assembly Common part longer than source */
	          print_error(9,
                              source_file, source_section_no, *source_line,
                              searched_file, searched_section_no, *searched_line);
		  }    /* Not necessarily longer as searched section may already have ended */
	               /* need to cover this case */
               }
	    }
	 else if (searched_section[*searched_line][0] == '*')
	    {
	    if ((searched_directive = check_directive(
			     &searched_section[*searched_line][0],
			     directive_string)) == variant_assembly_end ||
	        searched_directive == in_assembly)
               {
               /* Searched common part shorter than source */
	       print_error(10,
                           source_file, source_section_no, *source_line,
			   searched_file, searched_section_no, *searched_line);
	       source_common_var_assy = false;
               }
	    }
	 else
	    {
	    if (strcmp(&source_section[*source_line][3],
		       &searched_section[*searched_line][3]) != 0)
	       {
	       /* Different part number or description */
	       print_error(11,
                           source_file, source_section_no, *source_line,
			   searched_file, searched_section_no, *searched_line);
	       }     /* need check here to see if searched section ended */
	    else
	       ;
	    }
	 }
      return(1);
      }
   else
      return(0);
   } /* compare_var_assy_common_sections() */

int print_error(int error_type,
		int source_file,
		int source_section_no,
		int source_line,
		int searched_file,
		int searched_section_no,
		int searched_line)
   {

   printf("   %s",error_messages[error_type]);
   switch (error_type)
      {
      case 1:
	 {
	 break;
         }
      case 4: case 5: case 7: case 8: case 10:
         {
         printf("   source: %d,%d  searched: %d,%d\n",source_file,source_section_no,
					           searched_file,searched_section_no);
         printf("   %s",source_section[source_line]);
	 break;
         }
      case 3: case 11:
         {
         printf("   source: %d,%d  searched: %d,%d\n",source_file,source_section_no,
					           searched_file,searched_section_no);
         printf("   %s",source_section[source_line]);
	 printf("   %s",searched_variant_assy_defn[searched_line]);
	 break;
	 }
      case 2: case 6: case 9:
         {
         printf("   source: %d,%d  searched: %d,%d\n",source_file,source_section_no,
					           searched_file,searched_section_no);
	 printf("   %s",searched_variant_assy_defn[searched_line]);
	 break;
	 }
      }
   /* printf("%d %d\n",source_line, searched_line); */
   printf("\n");
   return 1;
   } /* print_error() */

