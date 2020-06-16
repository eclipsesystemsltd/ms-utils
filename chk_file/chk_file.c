/*                        chk_file.c                     */
/*                                                       */
/* This program checks the correctness of parts lists    */
/* In order to ensure the uniqueness of part numbers the */
/* following rules apply :-                              */
/* Variants can contain : Variants                       */
/*                        Assemblies                     */
/*                        Variant assemblies             */
/*                        Variant quantities ???         */
/*                        One in sets                    */
/* Assemblies can contain : Assemblies                   */
/* Variant assemblies can contain : Assemblies           */
/* Variant quantities can contain : Assemblies           */
/* One in sets can contain : Assemblies                  */
/* */
/* CONSIDER resetting error checking at each new section, */
/* especially wrt assembly indentation. */
/* */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

static char error_messages[28][60] = {
   " \n",                                               /* 0 */
   "Error in directive definition\n",                   /* 1 */
   "Error: Duplicate alternative part number defns\n",	/* 2 */
   "Error: Duplicate assembly definitions\n",		/* 3 */
   "Error: Duplicate quantity definitions\n",		/* 4 */
   "Error in part number\n",                            /* 5 */
   "Error in reference number\n",                       /* 6 */
   "Error in quantity definition\n",                    /* 7 */
   "Error in assembly definition\n",                    /* 8 */
   "Error in alt part definition\n",                    /* 9 */
   "Error in assembly structure\n",                     /* 10 */
   "Error in assembly identation\n",                    /* 11 */
   "Error: Unfinished assembly definition\n",		/* 12 */
   "Error with variant_assy_start\n",                   /* 13 */
   "Error with variant_assembly\n",                     /* 14 */
   "Error: Variant assembly title not defined\n",	/* 15 */
   "Error: Variant assembly definition incomplete\n",	/* 16 */
   "Error in variant assembly indentation\n",		/* 17 */
   "Error: Erroneous directive\n",			/* 18 */
   "Error: Duplicate reference only definitions\n",	/* 19 */
   "Error in reference only definition\n",		/* 20 */
   "Error: Duplicate alternative part definitions\n",	/* 21 */
   "Error in alternative definition\n", 		/* 22 */
   "Error: Duplicate optional part definitions\n",	/* 23 */
   "Error in optional part definition\n",		/* 24 */
   "Error: duplicate long part number definitions\n",	/* 25 */
   "Error in long part number definition\n",		/* 26 */
   "Error with variant\n"                   /* 27 */
   };

struct assembly_defn assembly[] = {
    0,0,
    0,0,
    0,0,
    0,0,
    0,0,
    0,0,
    0,0,
    0,0,
    0,0,
    0,0};

void main(int argc,
	  char *argv[])
   {
   struct variant_ind variant_brackets[10];
   struct variant_assy_ind variant_assy[10];

   FILE *fptr_in;
   FILE *fptr_out;
   char string_in[201];
   char string_out[201];
   char directive_string[81];
   char alt_part_manufacturer[21];
   char alt_part_number[21];
   char long_part_number[21];
   enum boolean print_line = true;
   enum boolean six_digit_part_no = true;
   enum boolean in_variant_assembly = false;
   enum boolean one_of_set = false;
   enum boolean set_label_shown = false;
   enum boolean variant_assy_defn_complete = false;
   enum boolean var_quantity = false;
   enum boolean var_quantity_label_shown = false;
   enum directive directive = none;
   enum boolean file_output = false;
   int i,j,k = 0;
   int out = 0;
   int number_of_variants = 0;
   int variant_assy_number = 0;
   int description_length = 0;
   int description_start = 0;
   int chk_part_no = 0;
   int error_type = 0;
   int alt_part_number_position = 0;
   int alternative_part_posn = 0;
   int optional_part_posn = 0;
   int assembly_position = 0;
   int quantity_posn = 0;
   int ref_only_posn = 0;
   int long_part_number_position = 0;
   int quantity = 0;
   int variant_definitions = 0;
   int variant_definitions_ended = 0;
   int variants = 0;
   int position = 0;
   int assembly_level = 0;
   int modified_assembly_level = 0;
   int variant_level = 0;
   int variant_assy_level = 0;
   int three_spaces = 1;
   int max_description_length = 50;
   int end_of_line;

   if ((fptr_in = fopen(argv[1],"r")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      printf("Cannot open file  \n");
      exit(0);
      }
   if (argc >= 3)
      {
      if (argv[2][0] == '/' && argv[2][1] == 'l')
	     six_digit_part_no = false;
      else if ((fptr_out = freopen(argv[2],"w", stdout)) == NULL)
	    {
	    printf("Cannot open file %s \n", argv[2]);
	    exit(0);
	    }
      }
   if (argc >= 4)
      {
      if (argv[3][0] == '/' && argv[3][1] == 'l')
		  six_digit_part_no = false;
      }

   for (i=0; i<201; i++)	/* Clear input line */
      string_in[i] = ' ';

   while(fgets(string_in, 200, fptr_in) != NULL)
      {
      /* Analyse each line */
      if (is_blank_line(string_in) == 0)
	     {
		 print_line = true;
		 position = 0;
		 error_type = 0;
		 alt_part_number_position = 0;
		 long_part_number_position = 0;
		 alternative_part_posn = 0;
		 optional_part_posn = 0;
		 assembly_position = 0;
		 quantity_posn = 0;
		 ref_only_posn = 0;

		 k = 0;
         j = 0;
		 while (string_in[k] != '\0' &&   /* Eliminate any comments */
				string_in[k] != '\n' &&
				k < 200)
		    {
	        if (string_in[k] == '*' && string_in[k - 1] == '/')
	           {
	           end_of_line = 0;
               j = 0;
               while (j < k-1) 
	              {
			         if (isspace(string_in[j]) != 1)
					    end_of_line = j;
                     j++;
                  }
       	       string_in[end_of_line + 1] = '\n';
	           string_in[end_of_line + 2] = '\0';
	           break;
	           }
	        k++;
            }

         for (i=0; i<81; i++)        /* Clear output line */
	        string_out[i] = ' ';

	     for (i=0; i<81; i++)	     /* Clear directive string */
	        directive_string[i] = ' ';

	     if (string_in[position] == '*')
	        {
	        if ((directive = check_directive(string_in, directive_string)) == error)
	           error_type = 1;
            else if (directive == page)
               {
		       str_build(string_out,"page ");
               str_build(&string_out[5], directive_string);
               }
            else if (directive == title)
               {
               str_build(string_out, directive_string);
               }
            else if (directive == major_model)
               {
               str_build(string_out,"Model Type : ");
               str_build(&string_out[13], directive_string);
               }
			else if (directive == minor_model)
			   {
			   str_build(string_out,"Model : ");
			   str_build(&string_out[8], directive_string);
			   }
			else if (directive == year)
			   {
               str_build(string_out,"Year : ");
               str_build(&string_out[7], directive_string);
			   }
			else if (directive == publication_part_no)
			   {
			   str_build(string_out,"Parts Catalogue : ");
			   str_build(&string_out[18], directive_string);
			   }
			else if (directive == manufacturer)
			   {
			   str_build(string_out,"Make : ");
			   str_build(&string_out[7], directive_string);
			   }
			else if (directive == engine_number)
			   {
			   str_build(string_out,"Engine Number : ");
			   str_build(&string_out[16], directive_string);
			   }
			else if (directive == variant_start)
			   {
			   variant_level++;
               variant_definitions++;
	           variant_brackets[variant_level - 1].symbol = 1;
	           /*variant_brackets[variant_level - 1].label_displayed = false;*/
	           variant_brackets[variant_level - 1].label_displayed = true;
	           short_strcpy(variant_brackets[variant_level - 1].label, directive_string, 5);
	           /*str_build_ltd(&string_out[variant_position +
		       *	  (variant_level - 1) * 6], directive_string, 4); */
	           str_build_ltd(&string_out[variant_position + (variant_level - 1) * 3], directive_string, 4);
	           print_line = true;
	           /*print_line = false; */
               }
			else if (directive == variant)
			   {
			   if (variant_level > 0)	/* 20/10/12 */
			      {
			      variants++;
			      (variant_brackets[variant_level - 1].symbol)++;
			      /*variant_brackets[variant_level - 1].label_displayed = false;*/
			      variant_brackets[variant_level - 1].label_displayed = true;
			      short_strcpy(variant_brackets[variant_level - 1].label,
														  directive_string, 5);
			      /*print_line = false;*/
			      /*str_build_ltd(&string_out[variant_position +
			       *	   (variant_level - 1) * 6], directive_string, 4); */
			      str_build_ltd(&string_out[variant_position + (variant_level - 1) * 3], directive_string, 4);
			      }
			   else
			      {
				  error_type = 27;
			      }
			   print_line = true;
			   }
			else if (directive == variant_end)
			   {
			   variant_definitions_ended++;
			   variant_level--;
			   print_line = false;
			   }
			else if (directive == variant_assembly_start)
			   {
			   if (in_variant_assembly == false &&
					/* variant_level < 1 &&              16/1/96 */
					assembly_level < 1)
			      {
				  in_variant_assembly = true;
				  variant_assy_number = 0;
				  }
			   else
				  error_type = 13;
			   print_line = false;
			   }
			else if (directive == variant_assembly)
			   {
			   if (in_variant_assembly == true && variant_assy_defn_complete == false)
				  {
				  variant_assy_number++;
	              variant_assy[variant_assy_number].label_displayed = false;
	              short_strcpy(variant_assy[variant_assy_number].label, directive_string, 5);
				  variant_assy_level = variant_assy_number;
				  }
			   else
				  error_type = 14;
			   print_line = false;
			   }
			else if (directive == common_assembly)
			   {
			   variant_assy_level = 0;
			   variant_assy_defn_complete = true;
			   print_line = false;
			   }
			else if (directive == in_assembly)
			   {
			   if (variant_assy_defn_complete == true)
				  {
				  i = 1;
	                  while (strncmp(variant_assy[i].label, directive_string, 3) != 0 && i <= variant_assy_number)
	                     i++;
				  if (i <= variant_assy_number)
					 {
	                 variant_assy[i].label_displayed = false;
					 variant_assy_level = i;
					 }
				  else
					 error_type = 15;
				  }
			   else
				  error_type = 16;
			   print_line = false;
			   }
			else if (directive == variant_assembly_end)
			   {
			   in_variant_assembly = false;
			   variant_assy_defn_complete = false;
			   print_line = false;
			   variant_assy_level = 0;
			   }
			else if (directive == variant_quantity)
			   {
			   var_quantity = true;
			   /*var_quantity_label_shown = false;*/
			   var_quantity_label_shown = true;
			   /* following statement may over run array limit */
			   str_build_ltd(&string_out[quantity_position - 10], directive_string, 10);
			   print_line = true;  /* was false */
			   }
			else if (directive == variant_quantity_end)
			   {
			   var_quantity = false;
			   print_line = false;
			   }
			else if (directive == variant_one_of_set)
			   {
			   string_out[quantity_position - 1] = '|';
			   string_out[quantity_position + 1] = '1';
			   one_of_set = true;
			   /*set_label_shown = false;*/
			   set_label_shown = true;
			   print_line = false;
			   }
			else if (directive == end_variant_set)
			   {
			   one_of_set = false;
			   print_line = false;
			   }
			else if (directive == end)
			   ;
			else
			   error_type = 18;
			}
		 else
			{
			/* Look for special characters */
			i=1;
			while (string_in[i] != '\n'&& string_in[i] != '\0')
			   {
			   if (string_in[i] == '*')
				  if (alt_part_number_position == 0)
					 alt_part_number_position = i;
				  else
					 {
					 error_type = 2;
					 break;
					 }
			   else if (string_in[i] == '#')
				  if (assembly_position == 0)
					 assembly_position = i;
				  else
					 {
					 error_type = 3;
					 break;
					 }
			   else if (string_in[i] == '$')
				  if (quantity_posn == 0)
					 quantity_posn = i;
				  else
					 {
					 error_type = 4;
					 break;
					 }
			   else if (string_in[i] == '~')
				  if (ref_only_posn == 0)
					 ref_only_posn = i;
				  else
					 {
					 error_type = 19;
					 break;
					 }
			   else if (string_in[i] == '^')
				  if (alternative_part_posn == 0)
					 alternative_part_posn = i;
				  else
					 {
					 error_type = 21;
					 break;
					 }
			   else if (string_in[i] == '\\')
				  if (optional_part_posn == 0)
					 optional_part_posn = i;
				  else
					 {
					 error_type = 23;
					 break;
					 }
			   else if (string_in[i] == '`')
				  if (long_part_number_position == 0)
					 long_part_number_position = i;
				  else
					 {
					 error_type = 25;
					 break;
					 }
			   i++;
			   }

			if (error_type == 0)
			   {
			   if (alt_part_number_position > 0)
				  max_description_length = 46;
			   else
				  max_description_length = 59;

			   if (check_ref_number(string_in,
									string_out,
									&position) != 0)
				  {
				  if ((chk_part_no = check_part_number(string_in,
													   string_out,
													   six_digit_part_no,
													   &position)) == 1)
					 {
					 description_start = position;
					 check_description(string_in,
									   string_out,
									   max_description_length,
									   &position);
					 description_length = position - description_start;

					 if (alt_part_number_position > 0)
						{
						position = alt_part_number_position;
						if (check_alt_part_number(string_in,
												  alt_part_manufacturer,
												  alt_part_number,
												  string_out,
												  &position) != 1)
						   error_type = 9;
						}
					 position = quantity_posn;
					 if ((quantity = check_quantity(string_in,
													string_out,
													&position)) < 0)
						error_type = 7;
					 if (in_variant_assembly == true &&
						 variant_assy_defn_complete == true &&
						 assembly_level < 1)
						{
						if (check_desc_indentation(string_in,
												   description_start,
												   three_spaces) != 1)
						   error_type = 17;
						}
					 else if (assembly_level > 0)
						{
						if (in_variant_assembly == true)
						   modified_assembly_level = assembly_level + 1;
						else
						   modified_assembly_level = assembly_level;

						if (check_desc_indentation(string_in,
												   description_start,
												   modified_assembly_level) == 1)
						   {
						   if (quantity > 0)
							  {
							  assembly[assembly_level].lines_to_go--;
							  assembly[assembly_level].components_to_go = assembly[assembly_level].components_to_go - quantity;
							  if (assembly[assembly_level].lines_to_go == 0)
								 {
									if (assembly[assembly_level].components_to_go != 0)
									   error_type = 10;
									assembly_level--;
								 }
							  }
						   }
						else
						   error_type = 11;
						}

					 if (var_quantity == true)
						{
						/*string_out[variant_quantity_position] = ':';*/
						string_out[quantity_position - 1] = ':';
						if (var_quantity_label_shown == false)
						   {
						   /*str_build(&string_out[variant_quantity_position + 1],
						   *	       directive_string); */
						   var_quantity_label_shown = true;
						   }
						}

					 if (one_of_set == true)
						{
						/*string_out[variant_position +
						*		    (variant_level * 6) - 2] = '|';*/
						string_out[quantity_position - 1] = '|';
						if (set_label_shown == false)
						   {
						   /* str_build(&string_out[variant_position +
						   *		 (variant_level * 6) - 1], "1 off"); */
						   str_build(&string_out[quantity_position + 1], "1");
						   set_label_shown = true;
						   }
						}

					 if (variant_level > 0)
						{
						display_variant(string_out, variant_brackets, variant_level);
						}

					 if (variant_assy_level > 0)
						{
						display_variant_assy(string_out, variant_assy, variant_assy_level);
						}

					 if (assembly_position > 0)
						{
					    position = assembly_position;
					    if (check_assembly(string_in,
										   &assembly_level,
										   &position,
										   assembly) != 1)
						   error_type = 8;
					    /*    else
					    *       {};     */
						}

					if (ref_only_posn > 0)
					   {
					   position = ref_only_posn;
					   if (check_ref_only(string_in, &position) != 1)
						  error_type = 20;
					   }

					if (alternative_part_posn > 0)
					   {
					   position = alternative_part_posn;
					   if (check_alternative_part(string_in, &position) != 1)
					      error_type = 22;
					   }

					if (optional_part_posn > 0)
					   {
					   position = optional_part_posn;
					   if (check_optional_part(string_in, &position) != 1)
						  error_type = 24;
					   }

					if (long_part_number_position > 0)
					   ;
					   /*     fill_in_dots(string_out,
					   *               description_position + description_length + 1,
					   *               quantity_position - 2);  */
				    }
			     else if (chk_part_no == 2) /* Blank part number */
				    {
				    print_line = false;
				    }
			     else
				    error_type = 5;    /* Error in part number */
			     }
		      else
			     error_type = 6;   /* Error in reference number */
		      }
		   }

		   string_out[79] = '\n';
		   string_out[80] = '\0';

		   if (error_type == 0)
			  {
			  if (print_line == true)
				 fputs(string_out,stdout);
			  }
		   else
			  fputs(error_messages[error_type],stdout);
		   }
	   }

   if (assembly_level != 0)
      fputs(error_messages[12],stdout);

   out = fcloseall();
   }  /* main() */

