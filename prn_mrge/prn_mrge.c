/*                             prn_mrge.c                                  */

/*
PRN_MRGE.C

The command line input to this program is a definition file defining which
parts lists were merged by MRGE_ALL.C (i.e. MRGE00.RES to MRGEnn.RES).
PRN_MRGE.C then displays part data in parts list sequence. Each line indicates
in which parts list and section a particular part number was first referred to
followed by a series of numbers indicating how that part number was used in
following parts lists or sections.

Note: This program modifies the files MRGE00.RES to MRGEnn.RES making them
useless for subsequent runs of COMBINE or PRN_MRGE. Therefore copy the MRGEnn
files if it is required to use them again.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int print_section(int previous_file,
                  int file_limit,
		  char current_section[]);
int prepare_output(struct part_years results,
		   struct part_years_output *results_output);

void main(int argc,
	  char *argv[])
   {
   FILE *fptr_res;
   FILE *fptr_defn;
//   FILE *fptr_sec;

   struct part_years_output results_output;
   struct part_years part_year_data;
   char defn_file[20][50];                      /* store definition file */
   char string_in[101];
   char string_out[101];
   char file_name[40];
   char current_section[50];
   char next_section[50];
   int i,j;
   int size;
   int title_posn;
   int source_file;
//   int secondary_file;
   enum boolean first_section;
   enum boolean in_section;
   enum boolean another_section;

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
	 title_posn = i;
	 /* exit(); */
	 }
      else
	 title_posn = i;
      }
   /* Years and models covered information is stored in "MRGE00.RES" */
   if ((fptr_res = fopen("MRGE00.RES","rb")) == NULL)
      {
      printf("Cannot open file %s\n","MRGE00.RES");
      exit(0);
      }
   while (fread(&part_year_data,sizeof(part_year_data), 1, fptr_res) != NULL)
      {
      if (part_year_data.year[0][0] > 1000)
         {
         i = 0;
         while (part_year_data.year[0][i] > 1000)
            {
            printf ("%d ",part_year_data.year[0][i]);
            i++;
            }
         printf("\n");
         }
      else
         printf("%s\n",part_year_data.description);
      }
   fclose(fptr_res);

   for (source_file = 1; source_file < title_posn; source_file++)   /* i.e. for each parts list (i.e. file) */
      {
printf("source %d\n",source_file);
      /* Open file */
      strcpy(file_name,"MRGE00.RES");
      make_temp_file_name(file_name,
			  4,
			  source_file);
      if ((fptr_res = fopen(file_name,"rb")) == NULL)
	 {
	 printf("Cannot open file %s\n",file_name);
         exit(0);
	 }
      first_section = true;
      another_section = false;
      in_section = false;
      while (fread(&part_year_data,sizeof(part_year_data), 1, fptr_res) != NULL)
	 {
	 if (another_section == true)
            {
	    strcpy(current_section,next_section);
	    printf("%s\n",current_section);
	    another_section = false;
            in_section = true;
	    }
         if (part_year_data.year[0][0] == -1)
	    {
	    if (first_section == true)
               {
	       printf("%s\n",part_year_data.description);
	       strcpy(current_section,part_year_data.description);
	       first_section = false;
	       }
	    else if (in_section == true)
               {
	       strcpy(next_section,part_year_data.description);
	       another_section = true;
	       /* Print data for same section from other files as must */
	       /* be at end of section in source file                  */
	       print_section(source_file,
                             title_posn,
			     current_section);
	       }
	    else
               {
	       strcpy(next_section,part_year_data.description);
	       another_section = true;
	       }
            in_section = true;
	    }
	 else if (part_year_data.year[0][0] == -2)  /* i.e. a previously displayed section */
	    {
	    in_section = false;
	    }
         else if (part_year_data.year[0][0] >= 0 &&
		  part_year_data.year[0][0] < 1000 &&
		  in_section == true)
	    {
	    for(j=0; j<40; j++)
	       string_out[j] = ' ';
	    string_out[40] = '\0';
            prepare_output(part_year_data,
			   &results_output);
            strncpy(string_out,part_year_data.description,40);
            printf("%s ",part_year_data.part_number);
	    printf("   %s %s %s %s\n",string_out,
				results_output.year[0],
				results_output.year[1],
				results_output.year[2]);
            }
         else if (part_year_data.year[0][0] > 1900 &&    /* Display model years covered */
                  part_year_data.year[0][0] < 2000)
	    {
	    printf("                                            ");
	    printf("%d  %d  %d\n",part_year_data.year[0][0],
			          part_year_data.year[1][0],
                                  part_year_data.year[2][0]);
	    }
	 }
      if (in_section == true)
         {
         /* Print data for same section from other files */
         print_section(source_file,
                       title_posn,
		       current_section);
         }
      fclose(fptr_res);
      }
   }   /* main() */

int print_section(int previous_file,
                  int file_limit,
		  char current_section[])
   {
   FILE *fptr_sec;
   char file_name[20];
   char string_out[101];
   int secondary_file;
   int j;
   struct part_years part_year_data;
   struct part_years_output results_output;
   enum boolean in_section;
   fpos_t start_of_struct_last_read;
   fpos_t end_of_struct_last_read;

   /* Print out current section from other files */
   for (secondary_file = previous_file + 1; secondary_file < file_limit; secondary_file++)
      {
printf("secondary %d\n",secondary_file);
      /* Open file */
      strcpy(file_name,"MRGE00.RES");
      make_temp_file_name(file_name,
			  4,
			  secondary_file);
      if ((fptr_sec = fopen(file_name,"rb+")) == NULL)
         {
	 printf("Cannot open file %s\n",file_name);
	 exit(0);
	 }
      in_section = false;
      fgetpos(fptr_sec, &start_of_struct_last_read);
      while (fread(&part_year_data,sizeof(part_year_data), 1, fptr_sec) != NULL)
         {
         fgetpos(fptr_sec, &end_of_struct_last_read);
	 if (part_year_data.year[0][0] == -1)
	    {
	    if (in_section == true)
	       {
	       break;
               }
            if (strcmp(current_section,part_year_data.description) == 0)
	        {
                in_section = true;
                /* Mark section as found */
	        part_year_data.year[0][0] = -2;
	        if (fsetpos(fptr_sec, &start_of_struct_last_read) != 0)
	           printf("Error: fsetpos() error\n");
		if (fwrite(&part_year_data,sizeof(part_year_data),1,fptr_sec) == EOF)
                   {
	           printf("Marked structure not written\n");
		   }
	        fsetpos(fptr_sec, &end_of_struct_last_read);
		}
	    }
	 else if (part_year_data.year[0][0] == -2 &&  /* Reached end of section */
                  in_section == true)
	    {
	    break;
            }
         else if (part_year_data.year[0][0] >= 0 &&
                  part_year_data.year[0][0] < 1000)
	    {
	    if (in_section == true)
	        {
		for(j=0; j<40; j++)
		   string_out[j] = ' ';
                string_out[40] = '\0';
		/*prepare_output(part_year_data,
		 *	       &results_output); */
                prepare_output(part_year_data,
			       &results_output);
                printf("%s ",part_year_data.part_number);
                strncpy(string_out,part_year_data.description,40);
	        printf("   %s %s %s %s\n",string_out,
				          results_output.year[0],
				          results_output.year[1],
				          results_output.year[2]);
 		}
	    }
         fgetpos(fptr_sec, &start_of_struct_last_read);
	 }
      fclose(fptr_sec);
      }
   }  /*  print_section()  */

int prepare_output(struct part_years results,
		   struct part_years_output *results_output)
   {
   int j,k;

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
