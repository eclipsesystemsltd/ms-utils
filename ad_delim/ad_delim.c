                         /* ad_delim.c */
/*
 * e.g.       C:\> ad_delim file_name # 9 24 78 ...
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

void main(int argc,
	  char *argv[])
   {
   FILE *fptr_in;
   char string_in[101];
   char dummy_string[101];
   int i;
   int delimiter_no = 0;
   int positions[10];
   int length;
   int last_char;
   int position;
   char delimiter;

   if ((fptr_in = fopen(argv[1],"r")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }

   delimiter = argv[2][0];      /* Get character for delimiter */

   for (i=3; i<argc; i++)	/* Get delimiter positions */
      {
      positions[i - 3] = atoi(argv[i]);   /* Need check on positions[] ? */
      delimiter_no++;
      }
   if (delimiter_no == 0) exit(0);

   /* for (i=0; i<101; i++)
    *    string_in[i] = ' '; */
   while(fgets(string_in, 100, fptr_in) != NULL)
      {
      position = 0;
      if (check_part_number(string_in,
			    dummy_string,
			    true,
			    &position) == 1 &&
          string_in[8] > 64 &&    /* i.e. Check description starts with */
          string_in[8] < 91)      /* capital letter */
         {
         length = strlen(string_in);
         for (i=0; i < length; i++)
            if (string_in[i] == delimiter)
               printf("*** Error: String already contains delimiter ***\n");
         for (i=0; i < delimiter_no; i++)
            {
            if (positions[i] < length)
               string_in[positions[i]] = delimiter;
            }
         /* Remove white spaces around each delimiter */
         i=0;
         last_char = 0;
         while (string_in[i] != '\0' &&
                string_in[i] != '\n')
            {
            if (string_in[i] == delimiter)
               {
               /* Remove spaces in front of delimiter */
               strcpy(&string_in[last_char + 1], &string_in[i]);
               i = last_char + 2;
               /* Remove spaces after delimiter */
               while (string_in[i] != '\0' &&
                      string_in[i] != '\n' &&
                      string_in[i] == ' ')
                  {
                  i++;
                  }
               strcpy(&string_in[last_char + 2], &string_in[i]);
               i = last_char + 2;
               }
            else if (string_in[i] != ' ')
               last_char = i;
            i++;
            }
         /* Trim spaces from end of line */
         i--;
         while (isspace(string_in[i]) != 0)
            {
            i--;
            }
         string_in[i + 1] = '\n';
         string_in[i + 2] = '\0';

         printf("%s",string_in);
         /* for (i=0; i<101; i++)
          *    string_in[i] = ' '; */
         }
      }
   }   /* main() */