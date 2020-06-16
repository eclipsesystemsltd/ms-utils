/*                              cmp_db.c                                  */
/* The purpose of this program is to look for for common parts in two     */
/* databases and to compare their records.                                */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

int compare_databases(struct part_info *base_ptr,
		      FILE *fptr_db_2);

void main(int argc,
          char *argv[])
   {
   FILE *fptr_db_1;
   FILE *fptr_db_2;
   struct part_info *base_ptr;
//   struct part_info *last_ptr;
//   struct part_info *new_ptr;
//   struct part_info *cur_ptr;

   if ((fptr_db_1 = fopen(argv[1],"rb")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }
   if ((fptr_db_2 = fopen(argv[2],"rb")) == NULL)
      {
      printf("Cannot open file %s \n",argv[2]);
      exit(0);
      }
   if ((base_ptr = read_tgt_dbase(fptr_db_1)) == NULL)
      {
      printf("Insufficient memory for second database\n");
      exit(0);
      }
   else
      {
      if (compare_databases(base_ptr,
			    fptr_db_2) == 0)
	 {
	 printf("Unable to compare databases\n");
         exit(0);
         }
      }
   _fcloseall();
   }  /* main() */

int compare_databases(struct part_info *base_ptr,
		      FILE *fptr_db_2)
   {
   struct part_info *cur_ptr;
//   struct part_info *new_ptr;
   struct part_info part_buf;
//   int i,j,k;
//   int j,k;
   enum boolean part_found;
//   enum boolean model_found;

   while(fread(&part_buf,sizeof(part_buf),1,fptr_db_2) != 0)
      {
      cur_ptr = base_ptr;
      part_found = false;
      while (part_found == false)
	 {
         /* Only descriptions compared at moment */
	 if (strcmp(part_buf.part_number,cur_ptr->part_number) == 0)
	    {
	    part_found = true;
	    if (strcmp(cur_ptr->description,part_buf.description) != 0)
	       {
	       printf("%s  \n",part_buf.part_number);
	       printf("%s*\n",cur_ptr->description);
	       printf("%s*\n",part_buf.description);
	       }
	    break;
	    }
	 else
	    {
	    if (cur_ptr->part_ptr == NULL)
	       break;
            else
	       cur_ptr = cur_ptr->part_ptr;
	    }
         }
      }
   return 1;
   }  /* compare_databases() */