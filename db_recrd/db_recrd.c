/*                                db_recrd.c                              */
/*                                                                        */
/* The purpose of this program is to display data for a specified part    */
/* number; this data can be amended if required.                          */

/*
DB_RECRD.C

The purpose of this program is to display data for a specified part number
from a parts database; this data can be amended if required.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <alloc.h>
#include "../ck_utils/ck_types.h"
#include "../ck_utils/ck_utils.h"

void main(int argc,
	  char *argv[])
   {
//   FILE *fptr_in;
   FILE *fptr_db;
   char part_number[10];
   char description[100];
   struct part_info *base_ptr;
   struct part_info *part_ptr;
//   int i, j;
   int field;
   int number;
   float f_number;

   if ((fptr_db = fopen(argv[1],"rb")) == NULL)
      {
      printf("Cannot open file %s \n",argv[1]);
      exit(0);
      }
   if ((base_ptr = read_tgt_dbase(fptr_db)) == NULL)
      {
      printf("Insufficient memory for target database\n");
      exit(0);
      }
   else
      {
      for (;;)
         {
         printf("Enter part number : ");
	 gets(part_number);
         _flushall();
	 if (part_number[0] == '\0')
            break;
         printf("\n");
         if ((part_ptr = find_part(part_number,
				   base_ptr)) != NULL)
	    {
            display_part_data(part_ptr);
	    do {
               printf("Update field (0 for none or number) : ");
	       scanf("%d",&field);
               _flushall();
               switch (field)
	          {
	          case 1:    /* Description */
	             printf("\nNew description : ");
		     gets(description);
		     strcpy(part_ptr->description,description);
		     display_part_data(part_ptr);
	             break;
	          case 2:    /* Stock */
	             printf("\nEnter stock : ");
		     scanf("%d",&number);
		     if (number >= 0 &&
		         number < 1000)
                        {
		        part_ptr->stock = number;
                        display_part_data(part_ptr);
                        }
		     break;
	          case 3:    /* Re-order level */
	             printf("\nEnter re-order level : ");
	             scanf("%d",&number);
		     if (number >= 0 &&
		         number < 1000)
		        {
		        part_ptr->order_level = number;
                        display_part_data(part_ptr);
                        }
	             break;
	          case 4:    /* Trade cost */
	             printf("\nEnter trade cost : ");
		     scanf("%6.2f",&f_number);
		     if (f_number >= 0.0 &&
		         f_number < 1000.00)
                        {
		        part_ptr->trade_cost = f_number;
                        display_part_data(part_ptr);
		        }     
		     break;
	          case 5:    /* Retail price */
	             printf("\nEnter retail price : ");
	             scanf("%6.2f",&f_number);
		     if (f_number >= 0.0 &&
		         f_number < 1000.00)
                        {
		        part_ptr->retail_cost = f_number;     
		        display_part_data(part_ptr);
                        }
		     break;
	          default :
	             ;
		     break;
		  }
	       _flushall();
	       } while (field != 0);
	    }
         else
	    printf("Part not found\n");
	 _flushall();
	 }
      }
   _fcloseall();
   }  /* main() */
