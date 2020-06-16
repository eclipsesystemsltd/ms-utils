
int get_variant_assy_defn(char section[max_lines][100],
			  int *line,
			  int *variant_assy_number,
			  int *variant_assy_level,
			  struct variant_assy_ind variant_assy[]);

compare_variant_assemblies(char source_section[100][100],
			   int source_file,
               int source_section_no,
			   int ll,
			   int searched_file,
               int searched_section_no,
			   int mm,
			   int source_variant_assy_number);

int check_common_var_assy(char source_section[max_lines][100],
			  int source_file,
			  int source_section_no,
			  int *source_line,
			  char searched_section[max_lines][100],
			  int searched_file,
              int searched_section_no,
			  int *searched_line,
			  struct variant_assy_ind variant_assy[],
			  int variant_assy_number);

int print_error(int error_type,
		int source_file,
		int source_section_no,
		int source_line,
		int searched_file,
		int searched_section_no,
		int searched_line);

int find_var_assy_label(char section[max_lines][100],
			int *line,
			char label[]);

int find_var_assy_common(char section[max_lines][100],
			 int *line);

int compare_var_assy_common_sections(char source_section[max_lines][100],
				     int source_file,
				     int source_section_no,
				     int *source_line,
				     char searched_section[max_lines][100],
				     int searched_file,
                     int searched_section_no,
				     int *searched_line);




