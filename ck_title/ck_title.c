/*                               ck_title.c                                 */
/* This program lists the section titles in a parts list and then indicates */
/* variations to those titles in following parts lists. The parts lists to  */
/* be compared are specified in a file.                  		    */

/*
CK_TITLE.C

This program lists the section titles in a given parts list and then compares
that list of titles to those titles contained in other parts lists.

The input to this program is a definition file defining which parts lists are
to be compared and a set of original parts lists. The output is a complete
list of section titles from the first parts list and then variations to those
titles in the subsequent parts lists.

The output is written directly to the screen, but can be directed to a file
using MS-DOS.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

char source_titles[50][100];
char searched_titles[50][100];

int get_titles(int file,
	       char titles[50][100],
	       char defn_file[20][50]);

int compare_titles(char source_titles[50][100],
		   char searched_titles[50][100]);

void main(int argc,
	  char *argv[])
   {
   FILE *fptr_defn;
//   FILE *fptr_res;
   char string_in[100];
   char defn_file[20][50];			/* store definition file */
   int i;
   int size;
   int source_file;
   int searched_file;
   int title_posn;

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
      title_posn = i;
      }
   source_file = 1;
   get_titles(source_file,
	      source_titles,
	      defn_file);
   printf("%s contains following titles :- \n",defn_file[1]);
   /* Print all titles  */
   i=0;
   while (isalpha(source_titles[i][0]) != 0)
      {
      printf("%s\n",source_titles[i]);
      i++;
      }
   printf("\n");
   for (searched_file = source_file + 1; searched_file < title_posn; searched_file++)   /* i.e. for each parts list (i.e. file) */
      {
      get_titles(searched_file,
		 searched_titles,
		 defn_file);
      printf("%s contains following differences :- \n",defn_file[searched_file]);
      printf("\n");
      compare_titles(source_titles,
                     searched_titles);
      }
   }  /* main() */

int get_titles(int file,
	       char titles[50][100],
	       char defn_file[20][50])
   {
   FILE *fptr_list;
//   FILE *fptr_file;
   char directive_string[81];
   char string_in[100];
   enum directive directive;
   int i; //,j;

   /* initialise titles array */
   for (i = 0; i < 50; i++)
      titles[i][0] = '\0';

   /* Open file */
   if ((fptr_list = fopen(defn_file[file],"r")) == NULL)
      {
      printf("Cannot open file %s\n",defn_file[file]);
      exit(0);
      }
   i = 0;
   while (fgets(string_in, 100, fptr_list) != NULL)
      {
      if (string_in[0] == '*')
         {
	 if ((directive = check_directive(string_in, directive_string)) == title)
	    {
	    strcpy(titles[i],directive_string);
	    i++;
            }
	 }
      }
   fclose(fptr_list);
   return(1);
   }   /* get_titles() */

int compare_titles(char source_titles[50][100],
		   char searched_titles[50][100])
   {
   int i,j;
   enum boolean title_found;

   i=0;
   printf("Titles not found :- \n");
   while (isalpha(source_titles[i][0]) != 0)
      {
      j=0;
      title_found = false;
      while (isalpha(searched_titles[j][0]) != 0 ||
	     searched_titles[j][0] == '$')
	 {
	 if (strcmp(source_titles[i],searched_titles[j]) == 0)
	    {
	    title_found = true;
            searched_titles[j][0] = '$';
	    break;
	    }
	 j++;
	 }
      if (title_found == false)
	 printf("%s\n",source_titles[i]);
      i++;
      }
   printf("\n");
   printf("Titles not in source :- \n");
   i=0;
   while (isalpha(searched_titles[i][0]) != 0 ||
          searched_titles[i][0] == '$')
      {
      if (searched_titles[i][0] != '$')
         printf("%s\n",searched_titles[i]);
      i++;
      }
   return(1);
   }  /* compare_titles() */
