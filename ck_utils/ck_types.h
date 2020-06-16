
/* Define format of output */
/*								       T140
*									  TR7
*3 34-4567 Hvhfvh vnkfvfvh hvdkflvhfkjv khvkjhvfkjv hkvhhfv	       >  >  23
*								       >  > | 1
*6 12-4567 Ghjhfh hkfhkjfh nkvhkvjb hvovholvhvh 	  012345678901 >  >
*123456789012345678901234567890123456789012345678901234567890123456789012345678
*	  1	    2	      3 	4	  5	    6	      7
*/

#define ref_number_position 0
#define part_number_position 3
#define description_position 11
#define quantity_position 77
#define page_number_length 2
#define ref_number_length 2
#define part_number_length 7
#define quantity_length 2
#define alt_part_number_posn 58
#define long_part_number_posn 58
#define variant_position 71
#define one_off_position 76
#define variant_quantity_position 76
#define max_lines 100
#define max_sections 4

enum directive {
   manufacturer,
   major_model,
   minor_model,
   year,
   page,
   title,
   publication_part_no,
   engine_number,
   variant_start,
   variant,
   variant_end,
   variant_quantity,
   variant_quantity_end,
   variant_assembly_start,
   variant_assembly,
   in_assembly,
   common_assembly,
   variant_assembly_end,
   variant_one_of_set,
   end_variant_set,
   end,
   none,
   error};

enum boolean {
   true,
   false};

struct directive_id {
   char string[25];
   enum directive direct;
   };

struct assembly_defn {
   int lines_to_go;
   int components_to_go;};

struct variant_ind {
   int symbol;
   char label[5];
   enum boolean label_displayed;};

/* struct variant_assy_ind {
 *   char label[5];
 *   enum boolean label_displayed;};
 */

struct variant_assy_ind {
   char part_number[10];
   char description[80];
   char label[10];
   enum boolean label_displayed;
   int line;};

struct part_info {
   char part_number[10];
   char description[80];
   enum boolean assembly;
   int models[20];
   int stock;
   int order_level;
   float trade_cost;
   float retail_cost;
   struct part_info *part_ptr;};

struct part_desc {
   char part_number[10];
   int no_of_descs;
   char description[5][80];};

struct part_years {
   char part_number[10];
   char description[80];
   int year[20][6];
   int combination;};

struct year {
   char begin_string[10];
   char end_string[10];
   int begin_number;
   int end_number;};

struct part_model_ref {
   char part_number[10];
   int model[30];};

struct model_reference {
   char model_defn[10][20];
   char major_model[20];
   int ref_number;};

struct part {
   char part_number[10];
   int source;        /* 0 = original, 1 = new */
   char description[80];};

struct part_years_output {
   char description[80];
   char year[20][6];};

struct part_info_mod {     /* only included for 'order_db.c' */
   char part_number[10];
   char description[80];
   long part_no;
   int models[20];
   int stock;
   int order_level;
   float trade_cost;
   float retail_cost;
   struct part_info_mod *part_ptr;};
