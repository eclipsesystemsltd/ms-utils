
enum directive check_directive(char string_in[],
			       char string_out[]);

int check_ref_number(char string_in[],
		     char string_out[],
		     int *place);

int check_part_number(char string_in[],
                      char string_out[],
                      enum boolean six_digit_part_no,
		      int *place);

int check_odd_part_number(char string_in[],
                          char string_out[],
                          enum boolean six_digit_part_no,
			  int place);

int get_part_number(char string_in[],
                    char string_out[],
		    enum boolean six_digit_part_no,
		    int position);

int get_odd_part_number(char string_in[],
                        char string_out[],
			enum boolean six_digit_part_no,
			int place);

int to_alpha_part_no(char string_in[],
                     char string_out[],
		     int place);

int check_description(char string_in[],
		      char string_out[],
		      int max_description_length,
		      int *place);

int get_description(char string_in[],
		    char string_out[],
		    int max_description_length,
		    int place);

int check_alt_part_number(char string_in[],
                          char alt_part_manufacturer[],
                          char alt_part_number[],
                          char string_out[],
			  int *place);

int check_assembly(char string_in[],
		   int *assembly_level,
		   int *place,
		   struct assembly_defn assembly[]);

int check_quantity(char string_in[],
		   char string_out[],
		   int *place);

void fill_in_dots(char string_out[],
		  int start_position,
		  int stop_position);

int is_blank_line(char string_in[]);

int check_desc_indentation(char string_in[],
                           int description_start,
			   int assembly_level);

int display_variant(char string_out[],
                    struct variant_ind variant_brackets[],
		    int variant_level);

int short_strcpy(char string_1[],
                 char string_2[],
                 int number);

int display_variant_assy(char string_out[],
                         struct variant_assy_ind variant_assy[],
			 int variant_assy_level);

str_build(char string_out[],
          char string_in[]);

void str_build_ltd(char string_out[],
		   char string_in[],
		   int number);

int check_ref_only(char string_in[],
		   int *position);

int check_alternative_part(char string_in[],
			   int *position);

int check_optional_part(char string_in[],
			int *position);

int remove_leading_blanks(char string[],
			  int number);

int is_assembly(char *string);

str_build_1(char string_out[],
	    char string_in[]);

int get_section(char section[100][200],
		int file_no,
		int section_no,
		char current_section[50]);

int make_temp_file_name(char file_name[],
                        int base,
			int i);

int create_temp_files(char defn_file[20][50],
		      int title_posn);

int get_section_line(int file_no,
		     int section_no,
		     char current_section[50]);

int get_file_line(int file_no,
		  int section_no,
		  char current_section[50],
		  FILE **fptr_rtn);

int display_int_data(char string_out[],
		     int number,
		     int position);

int display_float_data(char string_out[],
		       float number,
		       int position);

struct part_info *find_record(char part_number[],
			      struct part_info *base_ptr);

struct part_info *read_tgt_dbase(FILE *fptr_dat);

struct part_info *find_part(char part_number[],
                            struct part_info *base_ptr);

display_part_data(struct part_info *part_ptr);

int load_section(char section[max_sections][max_lines][200],
		 char current_section[],
		 char defn_file[20][50],
		 int title_posn);

int check_long_part_number(char string_in[],
			   char long_part_number[],
			   char string_out[],
			   int *place);

int to_int_part_no(char string_in[],
		   char string_out[],
		   int place);

int check_alpha_part_number(char string_in[],
			    char string_out[],
			    enum boolean six_digit_part_no,
			    int *place);

int build_alt_part_number(char alt_part_number[50],
			  char alt_part_manufacturer[20],
			  char source_alt_part_number[20]);

int get_ref_number(char string_in[],
		   char string_out[],
		   int place);

int get_quantity(char string_in[],
		 char string_out[],
		 int position);

int check_alternative_alpha_part(char string_in[],
				 enum boolean six_digit_part_no,
				 int *position);
