		    /* one_desc.c */
/* The purpose of this program is to determine a single generic description */
/* for each part number */

/*
ONE_DESC.C

The purpose of this program is to determine a single generic description for
each part number.

The input to the program is the data file "cmp_desc.dat" as generated by
CMP_DESC.C. The output is a data file "one_desc.dat' which contains records of
type 'part'.

Note that the function of this program can probably be better achieved by
editing the text file output of CMP_DESC.C.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

struct part_desc buffer;
struct part results[100];

void main()
   {
   FILE *fptr_source;
   FILE *fptr_res;
   FILE *fptr_text;

   char generic_desc[100];
   char ans;
   int entry;
   int desc_choice;
   int i;
   enum boolean description_defined;

   if ((fptr_source = fopen("cmp_desc.dat","rb")) == NULL)
      {
      printf("Cannot open file 'cmp_desc.dat' \n");
      exit(0);
      }
   entry = 0;
   while (fread(&buffer,sizeof(buffer),1,fptr_source) == 1 &&
          buffer.no_of_descs > 0)       /* as array may not be filled */
      {
      strcpy(results[entry].part_number,buffer.part_number);
      if (buffer.no_of_descs == 1)
         {
         strcpy(results[entry].description,buffer.description[0]);
	 results[entry].source = 0;
	 }
      else
	 {
	 for (i=0; i < buffer.no_of_descs; i++)
	    printf("%s\n",buffer.description[i]);
	 description_defined = false;
	 do {
	    printf("no_of_descs = %d\n",buffer.no_of_descs);
	    printf("One of these ? [1..5] : ");
	    ans = getche();
	    printf("\n");
	    if (ans < 49 ||   /* Check ASCII value */
		ans > 53 ||
		ans - 48 > buffer.no_of_descs)
	       {
	       printf("Write generic description :-\n");
	       gets(generic_desc);
               strcpy(results[entry].description,generic_desc);
	       results[entry].source = 1;
	       }
	    else
               {
	       desc_choice = ans - 48;
	       strcpy(results[entry].description,buffer.description[desc_choice - 1]);
               printf("%s\n",buffer.description[desc_choice - 1]);
	       results[entry].source = 0;
               }
            printf("\n");
	    printf("Description OK [y/n] : ");
	    ans = getche();
	    if (ans == 'y' ||
		ans == 'Y')
               description_defined = true;
            printf("\n");
	    }
	 while (description_defined == false);
	 }
      entry++;
      }
   if ((fptr_res = fopen("one_desc.dat","wb")) == NULL)
      {
      printf("Cannot open file 'one_desc.dat' \n");
      exit(0);
      }
   fwrite(results,sizeof(results), 1, fptr_res);
   if ((fptr_text = fopen("one_desc.asc","w")) == NULL)
      {
      printf("Cannot open file 'one_desc.asc' \n");
      exit(0);
      }
   for (i = 0; i < entry; i++)
      {
      fprintf(fptr_text,"%s  ", results[i].part_number);
      fprintf(fptr_text,"%s", results[i].description);
      fprintf(fptr_text,"\n");
      }
   for (i = 0; i < entry; i++)
      {
      printf("%s\n",results[i].description);
      }
   fcloseall();
   }